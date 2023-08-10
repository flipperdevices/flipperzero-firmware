/* ecc.c
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

/* in case user set HAVE_ECC there */
#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_ECC_NO_SMALL_STACK
#undef WOLFSSL_SMALL_STACK
#undef WOLFSSL_SMALL_STACK_CACHE
#endif

/*
Possible ECC enable options:
 * HAVE_ECC:            Overall control of ECC                  default: on
 * HAVE_ECC_ENCRYPT:    ECC encrypt/decrypt w/AES and HKDF      default: off
 * HAVE_ECC_SIGN:       ECC sign                                default: on
 * HAVE_ECC_VERIFY:     ECC verify                              default: on
 * HAVE_ECC_DHE:        ECC build shared secret                 default: on
 * HAVE_ECC_CDH:        ECC cofactor DH shared secret           default: off
 * HAVE_ECC_KEY_IMPORT: ECC Key import                          default: on
 * HAVE_ECC_KEY_EXPORT: ECC Key export                          default: on
 * ECC_SHAMIR:          Enables Shamir calc method              default: on
 * HAVE_COMP_KEY:       Enables compressed key                  default: off
 * WOLFSSL_VALIDATE_ECC_IMPORT: Validate ECC key on import      default: off
 * WOLFSSL_VALIDATE_ECC_KEYGEN: Validate ECC key gen            default: off
 * WOLFSSL_CUSTOM_CURVES: Allow non-standard curves.            default: off
 *                        Includes the curve "a" variable in calculation
 * ECC_DUMP_OID:        Enables dump of OID encoding and sum    default: off
 * ECC_CACHE_CURVE:     Enables cache of curve info to improve performance
 *                                                              default: off
 * FP_ECC:              ECC Fixed Point Cache                   default: off
 *                      FP cache is not supported for SECP160R1, SECP160R2,
 *                      SECP160K1 and SECP224K1. These do not work with scalars
 *                      that are the length of the order when the order is
 *                      longer than the prime. Use wc_ecc_fp_free to free cache.
 * USE_ECC_B_PARAM:     Enable ECC curve B param                default: off
 *                      (on for HAVE_COMP_KEY)
 * WOLFSSL_ECC_CURVE_STATIC:                                    default off (on for windows)
 *                      For the ECC curve parameters `ecc_set_type` use fixed
 *                      array for hex string
 * WC_ECC_NONBLOCK:     Enable non-blocking support for sign/verify.
 *                      Requires SP with WOLFSSL_SP_NONBLOCK
 * WC_ECC_NONBLOCK_ONLY Enable the non-blocking function only, no fall-back to
 *                      normal blocking API's
 * WOLFSSL_ECDSA_SET_K: Enables the setting of the 'k' value to use during ECDSA
 *                      signing. If the value is invalid, a new random 'k' is
 *                      generated in the loop. (For testing)
 *                                                              default: off
 * WOLFSSL_ECDSA_SET_K_ONE_LOOP:
 *                      Enables the setting of the 'k' value to use during ECDSA
 *                      signing. If the value is invalid then an error is
 *                      returned rather than generating a new 'k'. (For testing)
 *                                                              default: off
 * WOLFSSL_ECDSA_DETERMINISTIC_K: Enables RFC6979 implementation of
 *                      deterministic ECC signatures. The following function
 *                      can be used to set the deterministic signing flag in the
 *                      ecc key structure.
 *                      int wc_ecc_set_deterministic(ecc_key* key, byte flag)
 *                                                              default: off
 *
 * WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT: RFC6979 lists a variant that uses the
 *                      hash directly instead of doing bits2octets(H(m)), when
 *                      the variant macro is used the bits2octets operation on
 *                      the hash is removed.
 *                                                              default: off
 *
 * WC_PROTECT_ENCRYPTED_MEM:
 *                      Enables implementations that protect data that is in
 *                      encrypted memory.
 *                                                              default: off
 */

/*
ECC Curve Types:
 * NO_ECC_SECP          Disables SECP curves                    default: off (not defined)
 * HAVE_ECC_SECPR2      Enables SECP R2 curves                  default: off
 * HAVE_ECC_SECPR3      Enables SECP R3 curves                  default: off
 * HAVE_ECC_BRAINPOOL   Enables Brainpool curves                default: off
 * HAVE_ECC_KOBLITZ     Enables Koblitz curves                  default: off
 * WOLFSSL_SM2          Enables SM2 curves                      default: off
 */

/*
ECC Curve Sizes:
 * ECC_USER_CURVES: Allows custom combination of key sizes below
 * HAVE_ALL_CURVES: Enable all key sizes (on unless ECC_USER_CURVES is defined)
 * ECC_MIN_KEY_SZ: Minimum supported ECC key size
 * HAVE_ECC112: 112 bit key
 * HAVE_ECC128: 128 bit key
 * HAVE_ECC160: 160 bit key
 * HAVE_ECC192: 192 bit key
 * HAVE_ECC224: 224 bit key
 * HAVE_ECC239: 239 bit key
 * NO_ECC256: Disables 256 bit key (on by default)
 * HAVE_ECC320: 320 bit key
 * HAVE_ECC384: 384 bit key
 * HAVE_ECC512: 512 bit key
 * HAVE_ECC521: 521 bit key
 */


#ifdef HAVE_ECC

/* Make sure custom curves is enabled for Brainpool or Koblitz curve types */
#if (defined(HAVE_ECC_BRAINPOOL) || defined(HAVE_ECC_KOBLITZ)) &&\
    !defined(WOLFSSL_CUSTOM_CURVES)
    #error Brainpool and Koblitz curves requires WOLFSSL_CUSTOM_CURVES
#endif

#if defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$f")
        #pragma const_seg(".fipsB$f")
    #endif
#endif

/* public ASN interface */
#include <wolfssl/wolfcrypt/asn_public.h>

#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_HAVE_SP_ECC
#include <wolfssl/wolfcrypt/sp.h>
#endif

#ifdef HAVE_ECC_ENCRYPT
    #include <wolfssl/wolfcrypt/kdf.h>
    #include <wolfssl/wolfcrypt/aes.h>
#endif

#ifdef HAVE_X963_KDF
    #include <wolfssl/wolfcrypt/hash.h>
#endif

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(FREESCALE_LTC_ECC)
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif

#if defined(WOLFSSL_STM32_PKA)
    #include <wolfssl/wolfcrypt/port/st/stm32.h>
#endif

#if defined(WOLFSSL_PSOC6_CRYPTO)
    #include <wolfssl/wolfcrypt/port/cypress/psoc6_crypto.h>
#endif

#if defined(WOLFSSL_CAAM)
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#endif

#if defined(WOLFSSL_KCAPI_ECC)
    #include <wolfssl/wolfcrypt/port/kcapi/kcapi_ecc.h>
#endif

#ifdef WOLFSSL_SE050
    #include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    #include <xsecure_ellipticclient.h>
#endif

#if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
    #include <wolfssl/wolfcrypt/hmac.h>
#endif

#if defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_SP_MATH_ALL)
    #define GEN_MEM_ERR MP_MEM
#elif defined(USE_FAST_MATH)
    #define GEN_MEM_ERR FP_MEM
#else
    #define GEN_MEM_ERR MP_MEM
#endif

#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SILABS_SE_ACCEL) && \
    !defined(WOLFSSL_KCAPI_ECC) && !defined(WOLFSSL_SE050) && \
    !defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    #undef  HAVE_ECC_VERIFY_HELPER
    #define HAVE_ECC_VERIFY_HELPER
#endif

#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SILABS_SE_ACCEL) && \
    !defined(WOLFSSL_KCAPI_ECC) && !defined(NO_ECC_MAKE_PUB) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC)
    #undef  HAVE_ECC_MAKE_PUB
    #define HAVE_ECC_MAKE_PUB
#endif

#if !defined(WOLFSSL_SP_MATH) && \
    !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SILABS_SE_ACCEL) && \
    !defined(WOLFSSL_SE050) && !defined(WOLFSSL_STM32_PKA) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC)
    #undef  HAVE_ECC_CHECK_PUBKEY_ORDER
    #define HAVE_ECC_CHECK_PUBKEY_ORDER
#endif

#if defined(WOLFSSL_SP_MATH_ALL) && SP_INT_BITS < MAX_ECC_BITS_NEEDED
#define MAX_ECC_BITS_USE    SP_INT_BITS
#else
#define MAX_ECC_BITS_USE    MAX_ECC_BITS_NEEDED
#endif
#if !defined(WOLFSSL_CUSTOM_CURVES) && (ECC_MIN_KEY_SZ > 160) && \
    (!defined(HAVE_ECC_KOBLITZ) || (ECC_MIN_KEY_SZ > 224))
#define ECC_KEY_MAX_BITS(key)                                       \
    ((((key) == NULL) || ((key)->dp == NULL)) ? MAX_ECC_BITS_USE :  \
        ((unsigned)((key)->dp->size * 8)))
#else
/* Add one bit for cases when order is a bit greater than prime. */
#define ECC_KEY_MAX_BITS(key)                                       \
    ((((key) == NULL) || ((key)->dp == NULL)) ? MAX_ECC_BITS_USE :  \
        ((unsigned)((key)->dp->size * 8 + 1)))
#endif

/* forward declarations */
static int  wc_ecc_new_point_ex(ecc_point** point, void* heap);
static void wc_ecc_del_point_ex(ecc_point* p, void* heap);
#if defined(HAVE_ECC_SIGN) && (defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT))
static int deterministic_sign_helper(const byte* in, word32 inlen, ecc_key* key);
#endif

/* internal ECC states */
enum {
    ECC_STATE_NONE = 0,

    ECC_STATE_SHARED_SEC_GEN,
    ECC_STATE_SHARED_SEC_RES,

    ECC_STATE_SIGN_DO,
    ECC_STATE_SIGN_ENCODE,

    ECC_STATE_VERIFY_DECODE,
    ECC_STATE_VERIFY_DO,
    ECC_STATE_VERIFY_RES
};


/* map
   ptmul -> mulmod
*/

/* 256-bit curve on by default whether user curves or not */
#if (defined(HAVE_ECC112) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 112
    #define ECC112
#endif
#if (defined(HAVE_ECC128) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 128
    #define ECC128
#endif
#if (defined(HAVE_ECC160) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 160
    #define ECC160
#endif
#if (defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 192
    #define ECC192
#endif
#if (defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 224
    #define ECC224
#endif
#if (defined(HAVE_ECC239) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 239
    #define ECC239
#endif
#if (!defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 256
    #define ECC256
#endif
#if (defined(HAVE_ECC320) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 320
    #define ECC320
#endif
#if (defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 384
    #define ECC384
#endif
#if (defined(HAVE_ECC512) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 512
    #define ECC512
#endif
#if (defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)) && ECC_MIN_KEY_SZ <= 521
    #define ECC521
#endif

/* The encoded OID's for ECC curves */
#ifdef ECC112
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP112R1    {1,3,132,0,6}
            #define CODED_SECP112R1_SZ 5
        #else
            #define CODED_SECP112R1    {0x2B,0x81,0x04,0x00,0x06}
            #define CODED_SECP112R1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp112r1[] = CODED_SECP112R1;
        #else
            #define ecc_oid_secp112r1 CODED_SECP112R1
        #endif
        #define ecc_oid_secp112r1_sz CODED_SECP112R1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP112R2    {1,3,132,0,7}
            #define CODED_SECP112R2_SZ 5
        #else
            #define CODED_SECP112R2    {0x2B,0x81,0x04,0x00,0x07}
            #define CODED_SECP112R2_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp112r2[] = CODED_SECP112R2;
        #else
            #define ecc_oid_secp112r2 CODED_SECP112R2
        #endif
        #define ecc_oid_secp112r2_sz CODED_SECP112R2_SZ
    #endif /* HAVE_ECC_SECPR2 */
#endif /* ECC112 */
#ifdef ECC128
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP128R1    {1,3,132,0,28}
            #define CODED_SECP128R1_SZ 5
        #else
            #define CODED_SECP128R1    {0x2B,0x81,0x04,0x00,0x1C}
            #define CODED_SECP128R1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp128r1[] = CODED_SECP128R1;
        #else
            #define ecc_oid_secp128r1 CODED_SECP128R1
        #endif
        #define ecc_oid_secp128r1_sz CODED_SECP128R1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP128R2    {1,3,132,0,29}
            #define CODED_SECP128R2_SZ 5
        #else
            #define CODED_SECP128R2    {0x2B,0x81,0x04,0x00,0x1D}
            #define CODED_SECP128R2_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp128r2[] = CODED_SECP128R2;
        #else
            #define ecc_oid_secp128r2 CODED_SECP128R2
        #endif
        #define ecc_oid_secp128r2_sz CODED_SECP128R2_SZ
    #endif /* HAVE_ECC_SECPR2 */
#endif /* ECC128 */
#ifdef ECC160
#ifndef FP_ECC
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP160R1    {1,3,132,0,8}
            #define CODED_SECP160R1_SZ 5
        #else
            #define CODED_SECP160R1    {0x2B,0x81,0x04,0x00,0x08}
            #define CODED_SECP160R1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp160r1[] = CODED_SECP160R1;
        #else
            #define ecc_oid_secp160r1 CODED_SECP160R1
        #endif
        #define ecc_oid_secp160r1_sz CODED_SECP160R1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP160R2    {1,3,132,0,30}
            #define CODED_SECP160R2_SZ 5
        #else
            #define CODED_SECP160R2    {0x2B,0x81,0x04,0x00,0x1E}
            #define CODED_SECP160R2_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp160r2[] = CODED_SECP160R2;
        #else
            #define ecc_oid_secp160r2 CODED_SECP160R2
        #endif
        #define ecc_oid_secp160r2_sz CODED_SECP160R2_SZ
    #endif /* HAVE_ECC_SECPR2 */
    #ifdef HAVE_ECC_KOBLITZ
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP160K1    {1,3,132,0,9}
            #define CODED_SECP160K1_SZ 5
        #else
            #define CODED_SECP160K1    {0x2B,0x81,0x04,0x00,0x09}
            #define CODED_SECP160K1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp160k1[] = CODED_SECP160K1;
        #else
            #define ecc_oid_secp160k1 CODED_SECP160K1
        #endif
        #define ecc_oid_secp160k1_sz CODED_SECP160K1_SZ
    #endif /* HAVE_ECC_KOBLITZ */
#endif /* !FP_ECC */
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP160R1    {1,3,36,3,3,2,8,1,1,1}
            #define CODED_BRAINPOOLP160R1_SZ 10
        #else
            #define CODED_BRAINPOOLP160R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x01}
            #define CODED_BRAINPOOLP160R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp160r1[] = CODED_BRAINPOOLP160R1;
        #else
            #define ecc_oid_brainpoolp160r1 CODED_BRAINPOOLP160R1
        #endif
        #define ecc_oid_brainpoolp160r1_sz CODED_BRAINPOOLP160R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC160 */
#ifdef ECC192
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP192R1    {1,2,840,10045,3,1,1}
            #define CODED_SECP192R1_SZ 7
        #else
            #define CODED_SECP192R1    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x01}
            #define CODED_SECP192R1_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp192r1[] = CODED_SECP192R1;
        #else
            #define ecc_oid_secp192r1 CODED_SECP192R1
        #endif
        #define ecc_oid_secp192r1_sz CODED_SECP192R1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
        #ifdef HAVE_OID_ENCODING
            #define CODED_PRIME192V2    {1,2,840,10045,3,1,2}
            #define CODED_PRIME192V2_SZ 7
        #else
            #define CODED_PRIME192V2    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x02}
            #define CODED_PRIME192V2_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_prime192v2[] = CODED_PRIME192V2;
        #else
            #define ecc_oid_prime192v2 CODED_PRIME192V2
        #endif
        #define ecc_oid_prime192v2_sz CODED_PRIME192V2_SZ
    #endif /* HAVE_ECC_SECPR2 */
    #ifdef HAVE_ECC_SECPR3
        #ifdef HAVE_OID_ENCODING
            #define CODED_PRIME192V3    {1,2,840,10045,3,1,3}
            #define CODED_PRIME192V3_SZ 7
        #else
            #define CODED_PRIME192V3    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x03}
            #define CODED_PRIME192V3_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_prime192v3[] = CODED_PRIME192V3;
        #else
            #define ecc_oid_prime192v3 CODED_PRIME192V3
        #endif
        #define ecc_oid_prime192v3_sz CODED_PRIME192V3_SZ
    #endif /* HAVE_ECC_SECPR3 */
    #ifdef HAVE_ECC_KOBLITZ
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP192K1    {1,3,132,0,31}
            #define CODED_SECP192K1_SZ 5
        #else
            #define CODED_SECP192K1    {0x2B,0x81,0x04,0x00,0x1F}
            #define CODED_SECP192K1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp192k1[] = CODED_SECP192K1;
        #else
            #define ecc_oid_secp192k1 CODED_SECP192K1
        #endif
        #define ecc_oid_secp192k1_sz CODED_SECP192K1_SZ
    #endif /* HAVE_ECC_KOBLITZ */
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP192R1    {1,3,36,3,3,2,8,1,1,3}
            #define CODED_BRAINPOOLP192R1_SZ 10
        #else
            #define CODED_BRAINPOOLP192R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x03}
            #define CODED_BRAINPOOLP192R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp192r1[] = CODED_BRAINPOOLP192R1;
        #else
            #define ecc_oid_brainpoolp192r1 CODED_BRAINPOOLP192R1
        #endif
        #define ecc_oid_brainpoolp192r1_sz CODED_BRAINPOOLP192R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC192 */
#ifdef ECC224
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP224R1    {1,3,132,0,33}
            #define CODED_SECP224R1_SZ 5
        #else
            #define CODED_SECP224R1    {0x2B,0x81,0x04,0x00,0x21}
            #define CODED_SECP224R1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp224r1[] = CODED_SECP224R1;
        #else
            #define ecc_oid_secp224r1 CODED_SECP224R1
        #endif
        #define ecc_oid_secp224r1_sz CODED_SECP224R1_SZ
    #endif /* !NO_ECC_SECP */
    #if defined(HAVE_ECC_KOBLITZ) && !defined(FP_ECC)
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP224K1    {1,3,132,0,32}
            #define CODED_SECP224K1_SZ 5
        #else
            #define CODED_SECP224K1    {0x2B,0x81,0x04,0x00,0x20}
            #define CODED_SECP224K1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp224k1[] = CODED_SECP224K1;
        #else
            #define ecc_oid_secp224k1 CODED_SECP224K1
        #endif
        #define ecc_oid_secp224k1_sz CODED_SECP224K1_SZ
    #endif /* HAVE_ECC_KOBLITZ && !FP_ECC */
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP224R1    {1,3,36,3,3,2,8,1,1,5}
            #define CODED_BRAINPOOLP224R1_SZ 10
        #else
            #define CODED_BRAINPOOLP224R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x05}
            #define CODED_BRAINPOOLP224R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp224r1[] = CODED_BRAINPOOLP224R1;
        #else
            #define ecc_oid_brainpoolp224r1 CODED_BRAINPOOLP224R1
        #endif
        #define ecc_oid_brainpoolp224r1_sz CODED_BRAINPOOLP224R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC224 */
#ifdef ECC239
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_PRIME239V1    {1,2,840,10045,3,1,4}
            #define CODED_PRIME239V1_SZ 7
        #else
            #define CODED_PRIME239V1    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x04}
            #define CODED_PRIME239V1_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_prime239v1[] = CODED_PRIME239V1;
        #else
            #define ecc_oid_prime239v1 CODED_PRIME239V1
        #endif
        #define ecc_oid_prime239v1_sz CODED_PRIME239V1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
        #ifdef HAVE_OID_ENCODING
            #define CODED_PRIME239V2    {1,2,840,10045,3,1,5}
            #define CODED_PRIME239V2_SZ 7
        #else
            #define CODED_PRIME239V2    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x05}
            #define CODED_PRIME239V2_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_prime239v2[] = CODED_PRIME239V2;
        #else
            #define ecc_oid_prime239v2 CODED_PRIME239V2
        #endif
        #define ecc_oid_prime239v2_sz CODED_PRIME239V2_SZ
    #endif /* HAVE_ECC_SECPR2 */
    #ifdef HAVE_ECC_SECPR3
        #ifdef HAVE_OID_ENCODING
            #define CODED_PRIME239V3    {1,2,840,10045,3,1,6}
            #define CODED_PRIME239V3_SZ 7
        #else
            #define CODED_PRIME239V3    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x06}
            #define CODED_PRIME239V3_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_prime239v3[] = CODED_PRIME239V3;
        #else
            #define ecc_oid_prime239v3 CODED_PRIME239V3
        #endif
        #define ecc_oid_prime239v3_sz CODED_PRIME239V3_SZ
    #endif /* HAVE_ECC_SECPR3 */
#endif /* ECC239 */
#ifdef ECC256
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP256R1    {1,2,840,10045,3,1,7}
            #define CODED_SECP256R1_SZ 7
        #else
            #define CODED_SECP256R1    {0x2A,0x86,0x48,0xCE,0x3D,0x03,0x01,0x07}
            #define CODED_SECP256R1_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp256r1[] = CODED_SECP256R1;
        #else
            #define ecc_oid_secp256r1 CODED_SECP256R1
        #endif
        #define ecc_oid_secp256r1_sz CODED_SECP256R1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_KOBLITZ
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP256K1    {1,3,132,0,10}
            #define CODED_SECP256K1_SZ 5
        #else
            #define CODED_SECP256K1    {0x2B,0x81,0x04,0x00,0x0A}
            #define CODED_SECP256K1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp256k1[] = CODED_SECP256K1;
        #else
            #define ecc_oid_secp256k1 CODED_SECP256K1
        #endif
        #define ecc_oid_secp256k1_sz CODED_SECP256K1_SZ
    #endif /* HAVE_ECC_KOBLITZ */
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP256R1    {1,3,36,3,3,2,8,1,1,7}
            #define CODED_BRAINPOOLP256R1_SZ 10
        #else
            #define CODED_BRAINPOOLP256R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x07}
            #define CODED_BRAINPOOLP256R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp256r1[] = CODED_BRAINPOOLP256R1;
        #else
            #define ecc_oid_brainpoolp256r1 CODED_BRAINPOOLP256R1
        #endif
        #define ecc_oid_brainpoolp256r1_sz CODED_BRAINPOOLP256R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
    #if defined(WOLFSSL_SM2) && !defined(WOLFSSL_SP_MATH)
        #ifdef HAVE_OID_ENCODING
            #define CODED_SM2P256V1    {1,2,156,10197,1,301}
            #define CODED_SM2P256V1_SZ 6
        #else
            #define CODED_SM2P256V1 {0x2A,0x81,0x1C,0xCF,0x55,0x01,0x82,0x2d}
            #define CODED_SM2P256V1_SZ 8
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_sm2p256v1[] = CODED_SM2P256V1;
        #else
            #define ecc_oid_sm2p256v1 CODED_SM2P256V1
        #endif
        #define ecc_oid_sm2p256v1_sz CODED_SM2P256V1_SZ
    #endif /* WOLFSSL_SM2 && !WOLFSSL_SP_MATH */
#endif /* ECC256 */
#ifdef ECC320
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP320R1    {1,3,36,3,3,2,8,1,1,9}
            #define CODED_BRAINPOOLP320R1_SZ 10
        #else
            #define CODED_BRAINPOOLP320R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x09}
            #define CODED_BRAINPOOLP320R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp320r1[] = CODED_BRAINPOOLP320R1;
        #else
            #define ecc_oid_brainpoolp320r1 CODED_BRAINPOOLP320R1
        #endif
        #define ecc_oid_brainpoolp320r1_sz CODED_BRAINPOOLP320R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC320 */
#ifdef ECC384
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP384R1    {1,3,132,0,34}
            #define CODED_SECP384R1_SZ 5
        #else
            #define CODED_SECP384R1    {0x2B,0x81,0x04,0x00,0x22}
            #define CODED_SECP384R1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp384r1[] = CODED_SECP384R1;
            #define CODED_SECP384R1_OID ecc_oid_secp384r1
        #else
            #define ecc_oid_secp384r1 CODED_SECP384R1
        #endif
        #define ecc_oid_secp384r1_sz CODED_SECP384R1_SZ
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP384R1    {1,3,36,3,3,2,8,1,1,11}
            #define CODED_BRAINPOOLP384R1_SZ 10
        #else
            #define CODED_BRAINPOOLP384R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x0B}
            #define CODED_BRAINPOOLP384R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp384r1[] = CODED_BRAINPOOLP384R1;
        #else
            #define ecc_oid_brainpoolp384r1 CODED_BRAINPOOLP384R1
        #endif
        #define ecc_oid_brainpoolp384r1_sz CODED_BRAINPOOLP384R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC384 */
#ifdef ECC512
    #ifdef HAVE_ECC_BRAINPOOL
        #ifdef HAVE_OID_ENCODING
            #define CODED_BRAINPOOLP512R1    {1,3,36,3,3,2,8,1,1,13}
            #define CODED_BRAINPOOLP512R1_SZ 10
        #else
            #define CODED_BRAINPOOLP512R1    {0x2B,0x24,0x03,0x03,0x02,0x08,0x01,0x01,0x0D}
            #define CODED_BRAINPOOLP512R1_SZ 9
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_brainpoolp512r1[] = CODED_BRAINPOOLP512R1;
        #else
            #define ecc_oid_brainpoolp512r1 CODED_BRAINPOOLP512R1
        #endif
        #define ecc_oid_brainpoolp512r1_sz CODED_BRAINPOOLP512R1_SZ
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC512 */
#ifdef ECC521
    #ifndef NO_ECC_SECP
        #ifdef HAVE_OID_ENCODING
            #define CODED_SECP521R1     {1,3,132,0,35}
            #define CODED_SECP521R1_SZ 5
        #else
            #define CODED_SECP521R1     {0x2B,0x81,0x04,0x00,0x23}
            #define CODED_SECP521R1_SZ 5
        #endif
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            static const ecc_oid_t ecc_oid_secp521r1[] = CODED_SECP521R1;
        #else
            #define ecc_oid_secp521r1 CODED_SECP521R1
        #endif
        #define ecc_oid_secp521r1_sz CODED_SECP521R1_SZ
    #endif /* !NO_ECC_SECP */
#endif /* ECC521 */


/* This holds the key settings.
   ***MUST*** be organized by size from smallest to largest. */

const ecc_set_type ecc_sets[] = {
#ifdef ECC112
    #ifndef NO_ECC_SECP
    {
        14,                             /* size/bytes */
        ECC_SECP112R1,                  /* ID         */
        "SECP112R1",                    /* curve name */
        "DB7C2ABF62E35E668076BEAD208B", /* prime      */
        "DB7C2ABF62E35E668076BEAD2088", /* A          */
        "659EF8BA043916EEDE8911702B22", /* B          */
        "DB7C2ABF62E35E7628DFAC6561C5", /* order      */
        "9487239995A5EE76B55F9C2F098",  /* Gx         */
        "A89CE5AF8724C0A23E0E0FF77500", /* Gy         */
        ecc_oid_secp112r1,              /* oid/oidSz  */
        ecc_oid_secp112r1_sz,
        ECC_SECP112R1_OID,              /* oid sum    */
        1,                              /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
    {
        14,                             /* size/bytes */
        ECC_SECP112R2,                  /* ID         */
        "SECP112R2",                    /* curve name */
        "DB7C2ABF62E35E668076BEAD208B", /* prime      */
        "6127C24C05F38A0AAAF65C0EF02C", /* A          */
        "51DEF1815DB5ED74FCC34C85D709", /* B          */
        "36DF0AAFD8B8D7597CA10520D04B", /* order      */
        "4BA30AB5E892B4E1649DD0928643", /* Gx         */
        "ADCD46F5882E3747DEF36E956E97", /* Gy         */
        ecc_oid_secp112r2,              /* oid/oidSz  */
        ecc_oid_secp112r2_sz,
        ECC_SECP112R2_OID,              /* oid sum    */
        4,                              /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR2 */
#endif /* ECC112 */
#ifdef ECC128
    #ifndef NO_ECC_SECP
    {
        16,                                 /* size/bytes */
        ECC_SECP128R1,                      /* ID         */
        "SECP128R1",                        /* curve name */
        "FFFFFFFDFFFFFFFFFFFFFFFFFFFFFFFF", /* prime      */
        "FFFFFFFDFFFFFFFFFFFFFFFFFFFFFFFC", /* A          */
        "E87579C11079F43DD824993C2CEE5ED3", /* B          */
        "FFFFFFFE0000000075A30D1B9038A115", /* order      */
        "161FF7528B899B2D0C28607CA52C5B86", /* Gx         */
        "CF5AC8395BAFEB13C02DA292DDED7A83", /* Gy         */
        ecc_oid_secp128r1,                  /* oid/oidSz  */
        ecc_oid_secp128r1_sz,
        ECC_SECP128R1_OID,                  /* oid sum    */
        1,                                  /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
    {
        16,                                 /* size/bytes */
        ECC_SECP128R2,                      /* ID         */
        "SECP128R2",                        /* curve name */
        "FFFFFFFDFFFFFFFFFFFFFFFFFFFFFFFF", /* prime      */
        "D6031998D1B3BBFEBF59CC9BBFF9AEE1", /* A          */
        "5EEEFCA380D02919DC2C6558BB6D8A5D", /* B          */
        "3FFFFFFF7FFFFFFFBE0024720613B5A3", /* order      */
        "7B6AA5D85E572983E6FB32A7CDEBC140", /* Gx         */
        "27B6916A894D3AEE7106FE805FC34B44", /* Gy         */
        ecc_oid_secp128r2,                  /* oid/oidSz  */
        ecc_oid_secp128r2_sz,
        ECC_SECP128R2_OID,                  /* oid sum    */
        4,                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR2 */
#endif /* ECC128 */
#ifdef ECC160
#ifndef FP_ECC
    #ifndef NO_ECC_SECP
    {
        20,                                         /* size/bytes */
        ECC_SECP160R1,                              /* ID         */
        "SECP160R1",                                /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFC", /* A          */
        "1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45", /* B          */
        "100000000000000000001F4C8F927AED3CA752257",/* order      */
        "4A96B5688EF573284664698968C38BB913CBFC82", /* Gx         */
        "23A628553168947D59DCC912042351377AC5FB32", /* Gy         */
        ecc_oid_secp160r1,                          /* oid/oidSz  */
        ecc_oid_secp160r1_sz,
        ECC_SECP160R1_OID,                          /* oid sum    */
        1,                                          /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
    {
        20,                                         /* size/bytes */
        ECC_SECP160R2,                              /* ID         */
        "SECP160R2",                                /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC73", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC70", /* A          */
        "B4E134D3FB59EB8BAB57274904664D5AF50388BA", /* B          */
        "100000000000000000000351EE786A818F3A1A16B",/* order      */
        "52DCB034293A117E1F4FF11B30F7199D3144CE6D", /* Gx         */
        "FEAFFEF2E331F296E071FA0DF9982CFEA7D43F2E", /* Gy         */
        ecc_oid_secp160r2,                          /* oid/oidSz  */
        ecc_oid_secp160r2_sz,
        ECC_SECP160R2_OID,                          /* oid sum    */
        1,                                          /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR2 */
    #ifdef HAVE_ECC_KOBLITZ
    {
        20,                                         /* size/bytes */
        ECC_SECP160K1,                              /* ID         */
        "SECP160K1",                                /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFAC73", /* prime      */
        "0000000000000000000000000000000000000000", /* A          */
        "0000000000000000000000000000000000000007", /* B          */
        "100000000000000000001B8FA16DFAB9ACA16B6B3",/* order      */
        "3B4C382CE37AA192A4019E763036F4F5DD4D7EBB", /* Gx         */
        "938CF935318FDCED6BC28286531733C3F03C4FEE", /* Gy         */
        ecc_oid_secp160k1,                          /* oid/oidSz  */
        ecc_oid_secp160k1_sz,
        ECC_SECP160K1_OID,                          /* oid sum    */
        1,                                          /* cofactor   */
    },
    #endif /* HAVE_ECC_KOBLITZ */
#endif /* !FP_ECC */
    #ifdef HAVE_ECC_BRAINPOOL
    {
        20,                                         /* size/bytes */
        ECC_BRAINPOOLP160R1,                        /* ID         */
        "BRAINPOOLP160R1",                          /* curve name */
        "E95E4A5F737059DC60DFC7AD95B3D8139515620F", /* prime      */
        "340E7BE2A280EB74E2BE61BADA745D97E8F7C300", /* A          */
        "1E589A8595423412134FAA2DBDEC95C8D8675E58", /* B          */
        "E95E4A5F737059DC60DF5991D45029409E60FC09", /* order      */
        "BED5AF16EA3F6A4F62938C4631EB5AF7BDBCDBC3", /* Gx         */
        "1667CB477A1A8EC338F94741669C976316DA6321", /* Gy         */
        ecc_oid_brainpoolp160r1,                    /* oid/oidSz  */
        ecc_oid_brainpoolp160r1_sz,
        ECC_BRAINPOOLP160R1_OID,                    /* oid sum    */
        1,                                          /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC160 */
#ifdef ECC192
    #ifndef NO_ECC_SECP
    {
        24,                                                 /* size/bytes */
        ECC_SECP192R1,                                      /* ID         */
        "SECP192R1",                                        /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC", /* A          */
        "64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22831", /* order      */
        "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012", /* Gx         */
        "7192B95FFC8DA78631011ED6B24CDD573F977A11E794811",  /* Gy         */
        ecc_oid_secp192r1,                                  /* oid/oidSz  */
        ecc_oid_secp192r1_sz,
        ECC_SECP192R1_OID,                                  /* oid sum    */
        1,                                                  /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
    {
        24,                                                 /* size/bytes */
        ECC_PRIME192V2,                                     /* ID         */
        "PRIME192V2",                                       /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC", /* A          */
        "CC22D6DFB95C6B25E49C0D6364A4E5980C393AA21668D953", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFE5FB1A724DC80418648D8DD31", /* order      */
        "EEA2BAE7E1497842F2DE7769CFE9C989C072AD696F48034A", /* Gx         */
        "6574D11D69B6EC7A672BB82A083DF2F2B0847DE970B2DE15", /* Gy         */
        ecc_oid_prime192v2,                                 /* oid/oidSz  */
        ecc_oid_prime192v2_sz,
        ECC_PRIME192V2_OID,                                 /* oid sum    */
        1,                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR2 */
    #ifdef HAVE_ECC_SECPR3
    {
        24,                                                 /* size/bytes */
        ECC_PRIME192V3,                                     /* ID         */
        "PRIME192V3",                                       /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC", /* A          */
        "22123DC2395A05CAA7423DAECCC94760A7D462256BD56916", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFF7A62D031C83F4294F640EC13", /* order      */
        "7D29778100C65A1DA1783716588DCE2B8B4AEE8E228F1896", /* Gx         */
        "38A90F22637337334B49DCB66A6DC8F9978ACA7648A943B0", /* Gy         */
        ecc_oid_prime192v3,                                 /* oid/oidSz  */
        ecc_oid_prime192v3_sz,
        ECC_PRIME192V3_OID,                                 /* oid sum    */
        1,                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR3 */
    #ifdef HAVE_ECC_KOBLITZ
    {
        24,                                                 /* size/bytes */
        ECC_SECP192K1,                                      /* ID         */
        "SECP192K1",                                        /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFEE37", /* prime      */
        "000000000000000000000000000000000000000000000000", /* A          */
        "000000000000000000000000000000000000000000000003", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFE26F2FC170F69466A74DEFD8D", /* order      */
        "DB4FF10EC057E9AE26B07D0280B7F4341DA5D1B1EAE06C7D", /* Gx         */
        "9B2F2F6D9C5628A7844163D015BE86344082AA88D95E2F9D", /* Gy         */
        ecc_oid_secp192k1,                                  /* oid/oidSz  */
        ecc_oid_secp192k1_sz,
        ECC_SECP192K1_OID,                                  /* oid sum    */
        1,                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_KOBLITZ */
    #ifdef HAVE_ECC_BRAINPOOL
    {
        24,                                                 /* size/bytes */
        ECC_BRAINPOOLP192R1,                                /* ID         */
        "BRAINPOOLP192R1",                                  /* curve name */
        "C302F41D932A36CDA7A3463093D18DB78FCE476DE1A86297", /* prime      */
        "6A91174076B1E0E19C39C031FE8685C1CAE040E5C69A28EF", /* A          */
        "469A28EF7C28CCA3DC721D044F4496BCCA7EF4146FBF25C9", /* B          */
        "C302F41D932A36CDA7A3462F9E9E916B5BE8F1029AC4ACC1", /* order      */
        "C0A0647EAAB6A48753B033C56CB0F0900A2F5C4853375FD6", /* Gx         */
        "14B690866ABD5BB88B5F4828C1490002E6773FA2FA299B8F", /* Gy         */
        ecc_oid_brainpoolp192r1,                            /* oid/oidSz  */
        ecc_oid_brainpoolp192r1_sz,
        ECC_BRAINPOOLP192R1_OID,                            /* oid sum    */
        1,                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC192 */
#ifdef ECC224
    #ifndef NO_ECC_SECP
    {
        28,                                                         /* size/bytes */
        ECC_SECP224R1,                                              /* ID         */
        "SECP224R1",                                                /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE", /* A          */
        "B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFF16A2E0B8F03E13DD29455C5C2A3D", /* order      */
        "B70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21", /* Gx         */
        "BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34", /* Gy         */
        ecc_oid_secp224r1,                                          /* oid/oidSz  */
        ecc_oid_secp224r1_sz,
        ECC_SECP224R1_OID,                                          /* oid sum    */
        1,                                                          /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #if defined(HAVE_ECC_KOBLITZ) && !defined(FP_ECC)
    {
        28,                                                         /* size/bytes */
        ECC_SECP224K1,                                              /* ID         */
        "SECP224K1",                                                /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFE56D", /* prime      */
        "00000000000000000000000000000000000000000000000000000000", /* A          */
        "00000000000000000000000000000000000000000000000000000005", /* B          */
        "10000000000000000000000000001DCE8D2EC6184CAF0A971769FB1F7",/* order      */
        "A1455B334DF099DF30FC28A169A467E9E47075A90F7E650EB6B7A45C", /* Gx         */
        "7E089FED7FBA344282CAFBD6F7E319F7C0B0BD59E2CA4BDB556D61A5", /* Gy         */
        ecc_oid_secp224k1,                                          /* oid/oidSz  */
        ecc_oid_secp224k1_sz,
        ECC_SECP224K1_OID,                                          /* oid sum    */
        1,                                                          /* cofactor   */
    },
    #endif /* HAVE_ECC_KOBLITZ && !FP_ECC */
    #ifdef HAVE_ECC_BRAINPOOL
    {
        28,                                                         /* size/bytes */
        ECC_BRAINPOOLP224R1,                                        /* ID         */
        "BRAINPOOLP224R1",                                          /* curve name */
        "D7C134AA264366862A18302575D1D787B09F075797DA89F57EC8C0FF", /* prime      */
        "68A5E62CA9CE6C1C299803A6C1530B514E182AD8B0042A59CAD29F43", /* A          */
        "2580F63CCFE44138870713B1A92369E33E2135D266DBB372386C400B", /* B          */
        "D7C134AA264366862A18302575D0FB98D116BC4B6DDEBCA3A5A7939F", /* order      */
        "0D9029AD2C7E5CF4340823B2A87DC68C9E4CE3174C1E6EFDEE12C07D", /* Gx         */
        "58AA56F772C0726F24C6B89E4ECDAC24354B9E99CAA3F6D3761402CD", /* Gy         */
        ecc_oid_brainpoolp224r1,                                    /* oid/oidSz  */
        ecc_oid_brainpoolp224r1_sz,
        ECC_BRAINPOOLP224R1_OID,                                    /* oid sum    */
        1,                                                          /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC224 */
#ifdef ECC239
    #ifndef NO_ECC_SECP
    {
        30,                                                             /* size/bytes */
        ECC_PRIME239V1,                                                 /* ID         */
        "PRIME239V1",                                                   /* curve name */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFF8000000000007FFFFFFFFFFF", /* prime      */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFF8000000000007FFFFFFFFFFC", /* A          */
        "6B016C3BDCF18941D0D654921475CA71A9DB2FB27D1D37796185C2942C0A", /* B          */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFF9E5E9A9F5D9071FBD1522688909D0B", /* order      */
        "0FFA963CDCA8816CCC33B8642BEDF905C3D358573D3F27FBBD3B3CB9AAAF", /* Gx         */
        "7DEBE8E4E90A5DAE6E4054CA530BA04654B36818CE226B39FCCB7B02F1AE", /* Gy         */
        ecc_oid_prime239v1,                                             /* oid/oidSz  */
        ecc_oid_prime239v1_sz,
        ECC_PRIME239V1_OID,                                             /* oid sum    */
        1,                                                              /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_SECPR2
    {
        30,                                                             /* size/bytes */
        ECC_PRIME239V2,                                                 /* ID         */
        "PRIME239V2",                                                   /* curve name */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFF8000000000007FFFFFFFFFFF", /* prime      */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFF8000000000007FFFFFFFFFFC", /* A          */
        "617FAB6832576CBBFED50D99F0249C3FEE58B94BA0038C7AE84C8C832F2C", /* B          */
        "7FFFFFFFFFFFFFFFFFFFFFFF800000CFA7E8594377D414C03821BC582063", /* order      */
        "38AF09D98727705120C921BB5E9E26296A3CDCF2F35757A0EAFD87B830E7", /* Gx         */
        "5B0125E4DBEA0EC7206DA0FC01D9B081329FB555DE6EF460237DFF8BE4BA", /* Gy         */
        ecc_oid_prime239v2,                                             /* oid/oidSz  */
        ecc_oid_prime239v2_sz,
        ECC_PRIME239V2_OID,                                             /* oid sum    */
        1,                                                              /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR2 */
    #ifdef HAVE_ECC_SECPR3
    {
        30,                                                             /* size/bytes */
        ECC_PRIME239V3,                                                 /* ID         */
        "PRIME239V3",                                                   /* curve name */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFF8000000000007FFFFFFFFFFF", /* prime      */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFFFFFFFF8000000000007FFFFFFFFFFC", /* A          */
        "255705FA2A306654B1F4CB03D6A750A30C250102D4988717D9BA15AB6D3E", /* B          */
        "7FFFFFFFFFFFFFFFFFFFFFFF7FFFFF975DEB41B3A6057C3C432146526551", /* order      */
        "6768AE8E18BB92CFCF005C949AA2C6D94853D0E660BBF854B1C9505FE95A", /* Gx         */
        "1607E6898F390C06BC1D552BAD226F3B6FCFE48B6E818499AF18E3ED6CF3", /* Gy         */
        ecc_oid_prime239v3,                                             /* oid/oidSz  */
        ecc_oid_prime239v3_sz,
        ECC_PRIME239V3_OID,                                             /* oid sum    */
        1,                                                              /* cofactor   */
    },
    #endif /* HAVE_ECC_SECPR3 */
#endif /* ECC239 */
#ifdef ECC256
    #ifndef NO_ECC_SECP
    {
        32,                                                                 /* size/bytes */
        ECC_SECP256R1,                                                      /* ID         */
        "SECP256R1",                                                        /* curve name */
        "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF", /* prime      */
        "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC", /* A          */
        "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B", /* B          */
        "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551", /* order      */
        "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296", /* Gx         */
        "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5", /* Gy         */
        ecc_oid_secp256r1,                                                  /* oid/oidSz  */
        ecc_oid_secp256r1_sz,
        ECC_SECP256R1_OID,                                                  /* oid sum    */
        1,                                                                  /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_KOBLITZ
    {
        32,                                                                 /* size/bytes */
        ECC_SECP256K1,                                                      /* ID         */
        "SECP256K1",                                                        /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", /* prime      */
        "0000000000000000000000000000000000000000000000000000000000000000", /* A          */
        "0000000000000000000000000000000000000000000000000000000000000007", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141", /* order      */
        "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798", /* Gx         */
        "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8", /* Gy         */
        ecc_oid_secp256k1,                                                  /* oid/oidSz  */
        ecc_oid_secp256k1_sz,
        ECC_SECP256K1_OID,                                                  /* oid sum    */
        1,                                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_KOBLITZ */
    #ifdef HAVE_ECC_BRAINPOOL
    {
        32,                                                                 /* size/bytes */
        ECC_BRAINPOOLP256R1,                                                /* ID         */
        "BRAINPOOLP256R1",                                                  /* curve name */
        "A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D52620282013481D1F6E5377", /* prime      */
        "7D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D9", /* A          */
        "26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6", /* B          */
        "A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7", /* order      */
        "8BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262", /* Gx         */
        "547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997", /* Gy         */
        ecc_oid_brainpoolp256r1,                                            /* oid/oidSz  */
        ecc_oid_brainpoolp256r1_sz,
        ECC_BRAINPOOLP256R1_OID,                                            /* oid sum    */
        1,                                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
    #if defined(WOLFSSL_SM2) && !defined(WOLFSSL_SP_MATH)
    {
        32,                                                     /* size/bytes */
        ECC_SM2P256V1,                                          /* ID         */
        "SM2P256V1",                                            /* curve name */

        /* bottom of draft-shen-sm2-ecdsa-02, recommended values */
        "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF", /* prime */
        "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC", /* A */
        "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93", /* B */
        "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123", /* order */
        "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7", /* Gx */
        "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0", /* Gy */
        ecc_oid_sm2p256v1,                                      /* oid/oidSz  */
        ecc_oid_sm2p256v1_sz,
        ECC_SM2P256V1_OID,                                      /* oid sum    */
        1,                                                      /* cofactor   */
    },
    #endif /* WOLFSSL_SM2 && !WOLFSSL_SP_MATH */
#endif /* ECC256 */
#ifdef ECC320
    #ifdef HAVE_ECC_BRAINPOOL
    {
        40,                                                                                 /* size/bytes */
        ECC_BRAINPOOLP320R1,                                                                /* ID         */
        "BRAINPOOLP320R1",                                                                  /* curve name */
        "D35E472036BC4FB7E13C785ED201E065F98FCFA6F6F40DEF4F92B9EC7893EC28FCD412B1F1B32E27", /* prime      */
        "3EE30B568FBAB0F883CCEBD46D3F3BB8A2A73513F5EB79DA66190EB085FFA9F492F375A97D860EB4", /* A          */
        "520883949DFDBC42D3AD198640688A6FE13F41349554B49ACC31DCCD884539816F5EB4AC8FB1F1A6", /* B          */
        "D35E472036BC4FB7E13C785ED201E065F98FCFA5B68F12A32D482EC7EE8658E98691555B44C59311", /* order      */
        "43BD7E9AFB53D8B85289BCC48EE5BFE6F20137D10A087EB6E7871E2A10A599C710AF8D0D39E20611", /* Gx         */
        "14FDD05545EC1CC8AB4093247F77275E0743FFED117182EAA9C77877AAAC6AC7D35245D1692E8EE1", /* Gy         */
        ecc_oid_brainpoolp320r1, ecc_oid_brainpoolp320r1_sz,                                /* oid/oidSz  */
        ECC_BRAINPOOLP320R1_OID,                                                            /* oid sum    */
        1,                                                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC320 */
#ifdef ECC384
    #ifndef NO_ECC_SECP
    {
        48,                                                                                                 /* size/bytes */
        ECC_SECP384R1,                                                                                      /* ID         */
        "SECP384R1",                                                                                        /* curve name */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF", /* prime      */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC", /* A          */
        "B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF", /* B          */
        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973", /* order      */
        "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7", /* Gx         */
        "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F", /* Gy         */
        ecc_oid_secp384r1, ecc_oid_secp384r1_sz,                                                            /* oid/oidSz  */
        ECC_SECP384R1_OID,                                                                                  /* oid sum    */
        1,                                                                                                  /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
    #ifdef HAVE_ECC_BRAINPOOL
    {
        48,                                                                                                 /* size/bytes */
        ECC_BRAINPOOLP384R1,                                                                                /* ID         */
        "BRAINPOOLP384R1",                                                                                  /* curve name */
        "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B412B1DA197FB71123ACD3A729901D1A71874700133107EC53", /* prime      */
        "7BC382C63D8C150C3C72080ACE05AFA0C2BEA28E4FB22787139165EFBA91F90F8AA5814A503AD4EB04A8C7DD22CE2826", /* A          */
        "04A8C7DD22CE28268B39B55416F0447C2FB77DE107DCD2A62E880EA53EEB62D57CB4390295DBC9943AB78696FA504C11", /* B          */
        "8CB91E82A3386D280F5D6F7E50E641DF152F7109ED5456B31F166E6CAC0425A7CF3AB6AF6B7FC3103B883202E9046565", /* order      */
        "1D1C64F068CF45FFA2A63A81B7C13F6B8847A3E77EF14FE3DB7FCAFE0CBD10E8E826E03436D646AAEF87B2E247D4AF1E", /* Gx         */
        "8ABE1D7520F9C2A45CB1EB8E95CFD55262B70B29FEEC5864E19C054FF99129280E4646217791811142820341263C5315", /* Gy         */
        ecc_oid_brainpoolp384r1, ecc_oid_brainpoolp384r1_sz,                                                /* oid/oidSz  */
        ECC_BRAINPOOLP384R1_OID,                                                                            /* oid sum    */
        1,                                                                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC384 */
#ifdef ECC512
    #ifdef HAVE_ECC_BRAINPOOL
    {
        64,                                                                                                                                 /* size/bytes */
        ECC_BRAINPOOLP512R1,                                                                                                                /* ID         */
        "BRAINPOOLP512R1",                                                                                                                  /* curve name */
        "AADD9DB8DBE9C48B3FD4E6AE33C9FC07CB308DB3B3C9D20ED6639CCA703308717D4D9B009BC66842AECDA12AE6A380E62881FF2F2D82C68528AA6056583A48F3", /* prime      */
        "7830A3318B603B89E2327145AC234CC594CBDD8D3DF91610A83441CAEA9863BC2DED5D5AA8253AA10A2EF1C98B9AC8B57F1117A72BF2C7B9E7C1AC4D77FC94CA", /* A          */
        "3DF91610A83441CAEA9863BC2DED5D5AA8253AA10A2EF1C98B9AC8B57F1117A72BF2C7B9E7C1AC4D77FC94CADC083E67984050B75EBAE5DD2809BD638016F723", /* B          */
        "AADD9DB8DBE9C48B3FD4E6AE33C9FC07CB308DB3B3C9D20ED6639CCA70330870553E5C414CA92619418661197FAC10471DB1D381085DDADDB58796829CA90069", /* order      */
        "81AEE4BDD82ED9645A21322E9C4C6A9385ED9F70B5D916C1B43B62EEF4D0098EFF3B1F78E2D0D48D50D1687B93B97D5F7C6D5047406A5E688B352209BCB9F822", /* Gx         */
        "7DDE385D566332ECC0EABFA9CF7822FDF209F70024A57B1AA000C55B881F8111B2DCDE494A5F485E5BCA4BD88A2763AED1CA2B2FA8F0540678CD1E0F3AD80892", /* Gy         */
        ecc_oid_brainpoolp512r1, ecc_oid_brainpoolp512r1_sz,                                                                                /* oid/oidSz  */
        ECC_BRAINPOOLP512R1_OID,                                                                                                            /* oid sum    */
        1,                                                                                                                                  /* cofactor   */
    },
    #endif /* HAVE_ECC_BRAINPOOL */
#endif /* ECC512 */
#ifdef ECC521
    #ifndef NO_ECC_SECP
    {
        66,                                                                                                                                    /* size/bytes */
        ECC_SECP521R1,                                                                                                                         /* ID         */
        "SECP521R1",                                                                                                                           /* curve name */
        "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", /* prime      */
        "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC", /* A          */
        "51953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00",  /* B          */
        "1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409", /* order      */
        "C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66",  /* Gx         */
        "11839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650", /* Gy         */
        ecc_oid_secp521r1, ecc_oid_secp521r1_sz,                                                                                               /* oid/oidSz  */
        ECC_SECP521R1_OID,                                                                                                                     /* oid sum    */
        1,                                                                                                                                     /* cofactor   */
    },
    #endif /* !NO_ECC_SECP */
#endif /* ECC521 */
#ifdef WOLFCRYPT_HAVE_SAKKE
    {
        128,
        ECC_SAKKE_1,
        "SAKKE1",
        "997ABB1F0A563FDA65C61198DAD0657A416C0CE19CB48261BE9AE358B3E01A2EF40AAB27E2FC0F1B228730D531A59CB0E791B39FF7C88A19356D27F4A666A6D0E26C6487326B4CD4512AC5CD65681CE1B6AFF4A831852A82A7CF3C521C3C09AA9F94D6AF56971F1FFCE3E82389857DB080C5DF10AC7ACE87666D807AFEA85FEB",
        "997ABB1F0A563FDA65C61198DAD0657A416C0CE19CB48261BE9AE358B3E01A2EF40AAB27E2FC0F1B228730D531A59CB0E791B39FF7C88A19356D27F4A666A6D0E26C6487326B4CD4512AC5CD65681CE1B6AFF4A831852A82A7CF3C521C3C09AA9F94D6AF56971F1FFCE3E82389857DB080C5DF10AC7ACE87666D807AFEA85FE8",
        "0",
        "265EAEC7C2958FF69971846636B4195E905B0338672D20986FA6B8D62CF8068BBD02AAC9F8BF03C6C8A1CC354C69672C39E46CE7FDF222864D5B49FD2999A9B4389B1921CC9AD335144AB173595A07386DABFD2A0C614AA0A9F3CF14870F026AA7E535ABD5A5C7C7FF38FA08E2615F6C203177C42B1EB3A1D99B601EBFAA17FB",
        "53FC09EE332C29AD0A7990053ED9B52A2B1A2FD60AEC69C698B2F204B6FF7CBFB5EDB6C0F6CE2308AB10DB9030B09E1043D5F22CDB9DFA55718BD9E7406CE8909760AF765DD5BCCB337C86548B72F2E1A702C3397A60DE74A7C1514DBA66910DD5CFB4CC80728D87EE9163A5B63F73EC80EC46C4967E0979880DC8ABEAE63895",
        "0A8249063F6009F1F9F1F0533634A135D3E82016029906963D778D821E141178F5EA69F4654EC2B9E7F7F5E5F0DE55F66B598CCF9A140B2E416CFF0CA9E032B970DAE117AD547C6CCAD696B5B7652FE0AC6F1E80164AA989492D979FC5A4D5F213515AD7E9CB99A980BDAD5AD5BB4636ADB9B5706A67DCDE75573FD71BEF16D7",
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            NULL, 0,
        #else
            {0}, 0,
        #endif
        0,
        4,
    },
#endif
#if defined(WOLFSSL_CUSTOM_CURVES) && defined(ECC_CACHE_CURVE)
    /* place holder for custom curve index for cache */
    {
        1, /* non-zero */
        ECC_CURVE_CUSTOM,
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        #else
            {0},{0},{0},{0},{0},{0},{0},{0},
        #endif
        0, 0, 0
    },
#endif
    {
        0,
        ECC_CURVE_INVALID,
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        #else
            {0},{0},{0},{0},{0},{0},{0},{0},
        #endif
        0, 0, 0
    }
};
#define ECC_SET_COUNT   (sizeof(ecc_sets)/sizeof(ecc_set_type))
const size_t ecc_sets_count = ECC_SET_COUNT - 1;


#ifdef HAVE_OID_ENCODING
    /* encoded OID cache */
    typedef struct {
        word32 oidSz;
        byte oid[ECC_MAX_OID_LEN];
    } oid_cache_t;
    static oid_cache_t ecc_oid_cache[ECC_SET_COUNT];
#endif

/* Forward declarations */
#if defined(HAVE_COMP_KEY) && defined(HAVE_ECC_KEY_EXPORT)
static int wc_ecc_export_x963_compressed(ecc_key* key, byte* out, word32* outLen);
#endif
#ifdef HAVE_ECC_CHECK_PUBKEY_ORDER
static int ecc_check_pubkey_order(ecc_key* key, ecc_point* pubkey, mp_int* a,
    mp_int* prime, mp_int* order);
#endif
static int _ecc_validate_public_key(ecc_key* key, int partial, int priv);
#if (FIPS_VERSION_GE(5,0) || defined(WOLFSSL_VALIDATE_ECC_KEYGEN)) && \
    !defined(WOLFSSL_KCAPI_ECC)
static int _ecc_pairwise_consistency_test(ecc_key* key, WC_RNG* rng);
#endif


int mp_jacobi(mp_int* a, mp_int* n, int* c);
int mp_sqrtmod_prime(mp_int* n, mp_int* prime, mp_int* ret);


/* Curve Specs */
typedef struct ecc_curve_spec {
    const ecc_set_type* dp;

    mp_int* prime;
    mp_int* Af;
    #ifdef USE_ECC_B_PARAM
        mp_int* Bf;
    #endif
    mp_int* order;
    mp_int* Gx;
    mp_int* Gy;

#ifdef ECC_CACHE_CURVE
    mp_int prime_lcl;
    mp_int Af_lcl;
    #ifdef USE_ECC_B_PARAM
        mp_int Bf_lcl;
    #endif
    mp_int order_lcl;
    mp_int Gx_lcl;
    mp_int Gy_lcl;
#else
#ifdef WOLFSSL_SP_MATH_ALL
    unsigned char* spec_ints;
#else
    mp_int* spec_ints;
#endif
    word32 spec_count;
    word32 spec_use;
#endif

    byte load_mask;
} ecc_curve_spec;

enum ecc_curve_load_mask {
    ECC_CURVE_FIELD_NONE    = 0x00,
    ECC_CURVE_FIELD_PRIME   = 0x01,
    ECC_CURVE_FIELD_AF      = 0x02,
#ifdef USE_ECC_B_PARAM
    ECC_CURVE_FIELD_BF      = 0x04,
#endif
    ECC_CURVE_FIELD_ORDER   = 0x08,
    ECC_CURVE_FIELD_GX      = 0x10,
    ECC_CURVE_FIELD_GY      = 0x20,
#ifdef USE_ECC_B_PARAM
    ECC_CURVE_FIELD_ALL     = 0x3F,
    ECC_CURVE_FIELD_COUNT   = 6,
#else
    ECC_CURVE_FIELD_ALL     = 0x3B,
    ECC_CURVE_FIELD_COUNT   = 5,
#endif
    WOLF_ENUM_DUMMY_LAST_ELEMENT(ecc_curve_load_mask)
};

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
static const u32 xil_curve_type[ECC_CURVE_MAX] = {
        [ECC_SECP384R1] = WOLFSSL_XSECURE_ECC_NIST_P384,
        [ECC_SECP521R1] = WOLFSSL_XSECURE_ECC_NIST_P521,
};

static void buf_reverse(byte *outbuf, const byte *inbuf, word32 len)
{
    word32 up, down;
    up = 0;
    down = len - 1;
    while (up < len)
        outbuf[up++] = inbuf[down--];
}

static int xil_mpi_import(mp_int *mpi,
                          const byte *inbuf,
                          word32 len,
                          void* heap)
{
    int err;
#ifdef WOLFSSL_SMALL_STACK
    byte* buf = NULL;
#else
    byte buf[MAX_ECC_BYTES];

    if (len > MAX_ECC_BYTES)
        return BUFFER_E;
#endif

#ifdef WOLFSSL_SMALL_STACK
    buf = (byte*)XMALLOC(len, heap, DYNAMIC_TYPE_PRIVATE_KEY);
    if (buf == NULL)
        return MEMORY_E;
#endif
    buf_reverse(buf, inbuf, len);

    err = mp_read_unsigned_bin(mpi, buf, len);
    ForceZero(buf, len);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(buf, heap, DYNAMIC_TYPE_PRIVATE_KEY);
#endif
    return err;
}
#endif

#ifdef ECC_CACHE_CURVE
    /* cache (mp_int) of the curve parameters */
    static ecc_curve_spec* ecc_curve_spec_cache[ECC_SET_COUNT];
    #ifndef SINGLE_THREADED
        static wolfSSL_Mutex ecc_curve_cache_mutex;
    #endif

    #define DECLARE_CURVE_SPECS(intcount) ecc_curve_spec* curve = NULL
    #define ALLOC_CURVE_SPECS(intcount, err) WC_DO_NOTHING
    #define FREE_CURVE_SPECS() WC_DO_NOTHING
#elif defined(WOLFSSL_SMALL_STACK)
#ifdef WOLFSSL_SP_MATH_ALL
    #define DECLARE_CURVE_SPECS(intcount)                               \
        unsigned char* spec_ints = NULL;                                \
        ecc_curve_spec curve_lcl;                                       \
        ecc_curve_spec* curve = &curve_lcl;                             \
        XMEMSET(curve, 0, sizeof(ecc_curve_spec));                      \
        curve->spec_count = intcount

    #define ALLOC_CURVE_SPECS(intcount, err)                            \
        spec_ints = (unsigned char*)XMALLOC(MP_INT_SIZEOF(MP_BITS_CNT(  \
            MAX_ECC_BITS_USE)) * (intcount), NULL,                      \
            DYNAMIC_TYPE_ECC);                                          \
        if (spec_ints == NULL)                                          \
            (err) = MEMORY_E;                                           \
        else                                                            \
            curve->spec_ints = spec_ints
#else
    #define DECLARE_CURVE_SPECS(intcount)                               \
        mp_int* spec_ints = NULL;                                       \
        ecc_curve_spec curve_lcl;                                       \
        ecc_curve_spec* curve = &curve_lcl;                             \
        XMEMSET(curve, 0, sizeof(ecc_curve_spec));                      \
        curve->spec_count = intcount

    #define ALLOC_CURVE_SPECS(intcount, err)                            \
        spec_ints = (mp_int*)XMALLOC(sizeof(mp_int) * (intcount), NULL, \
                            DYNAMIC_TYPE_ECC);                          \
        if (spec_ints == NULL)                                          \
            (err) = MEMORY_E;                                           \
        else                                                            \
            curve->spec_ints = spec_ints
#endif
    #define FREE_CURVE_SPECS()                                          \
        XFREE(spec_ints, NULL, DYNAMIC_TYPE_ECC)
#else
#ifdef WOLFSSL_SP_MATH_ALL
    #define DECLARE_CURVE_SPECS(intcount)                               \
        unsigned char spec_ints[MP_INT_SIZEOF(MP_BITS_CNT(              \
            MAX_ECC_BITS_USE)) * (intcount)];                           \
        ecc_curve_spec curve_lcl;                                       \
        ecc_curve_spec* curve = &curve_lcl;                             \
        XMEMSET(curve, 0, sizeof(ecc_curve_spec));                      \
        curve->spec_ints = spec_ints;                                   \
        curve->spec_count = (intcount)
#else
    #define DECLARE_CURVE_SPECS(intcount)                               \
        mp_int spec_ints[(intcount)];                                   \
        ecc_curve_spec curve_lcl;                                       \
        ecc_curve_spec* curve = &curve_lcl;                             \
        XMEMSET(curve, 0, sizeof(ecc_curve_spec));                      \
        curve->spec_ints = spec_ints;                                   \
        curve->spec_count = (intcount)
#endif
    #define ALLOC_CURVE_SPECS(intcount, err) WC_DO_NOTHING
    #define FREE_CURVE_SPECS() WC_DO_NOTHING
#endif /* ECC_CACHE_CURVE */

static void wc_ecc_curve_cache_free_spec_item(ecc_curve_spec* curve, mp_int* item,
    byte mask)
{
    if (item) {
    #ifdef HAVE_WOLF_BIGINT
        wc_bigint_free(&item->raw);
    #endif
        mp_clear(item);
    }
    curve->load_mask &= ~mask;
}
static void wc_ecc_curve_cache_free_spec(ecc_curve_spec* curve)
{
    if (curve == NULL) {
        return;
    }

    if (curve->load_mask & ECC_CURVE_FIELD_PRIME)
        wc_ecc_curve_cache_free_spec_item(curve, curve->prime, ECC_CURVE_FIELD_PRIME);
    if (curve->load_mask & ECC_CURVE_FIELD_AF)
        wc_ecc_curve_cache_free_spec_item(curve, curve->Af, ECC_CURVE_FIELD_AF);
#ifdef USE_ECC_B_PARAM
    if (curve->load_mask & ECC_CURVE_FIELD_BF)
        wc_ecc_curve_cache_free_spec_item(curve, curve->Bf, ECC_CURVE_FIELD_BF);
#endif
    if (curve->load_mask & ECC_CURVE_FIELD_ORDER)
        wc_ecc_curve_cache_free_spec_item(curve, curve->order, ECC_CURVE_FIELD_ORDER);
    if (curve->load_mask & ECC_CURVE_FIELD_GX)
        wc_ecc_curve_cache_free_spec_item(curve, curve->Gx, ECC_CURVE_FIELD_GX);
    if (curve->load_mask & ECC_CURVE_FIELD_GY)
        wc_ecc_curve_cache_free_spec_item(curve, curve->Gy, ECC_CURVE_FIELD_GY);

    curve->load_mask = 0;
}

static void wc_ecc_curve_free(ecc_curve_spec* curve)
{
    if (curve) {
    #ifdef ECC_CACHE_CURVE
        #ifdef WOLFSSL_CUSTOM_CURVES
        /* only free custom curves (rest are globally cached) */
        if (curve->dp && curve->dp->id == ECC_CURVE_CUSTOM) {
            wc_ecc_curve_cache_free_spec(curve);
            XFREE(curve, NULL, DYNAMIC_TYPE_ECC);
        }
        #endif
    #else
        wc_ecc_curve_cache_free_spec(curve);
    #endif
    }
}

static int wc_ecc_curve_cache_load_item(ecc_curve_spec* curve, const char* src,
    mp_int** dst, byte mask)
{
    int err;

#ifndef ECC_CACHE_CURVE
    /* get mp_int from temp */
    if (curve->spec_use >= curve->spec_count) {
        WOLFSSL_MSG("Invalid DECLARE_CURVE_SPECS count");
        return ECC_BAD_ARG_E;
    }
#ifdef WOLFSSL_SP_MATH_ALL
    *dst = (mp_int*)(curve->spec_ints + MP_INT_SIZEOF(MP_BITS_CNT(
        MAX_ECC_BITS_USE)) * curve->spec_use++);
#else
    *dst = &curve->spec_ints[curve->spec_use++];
#endif
#endif

#ifdef WOLFSSL_SP_MATH_ALL
    err = mp_init_size(*dst, MP_BITS_CNT(MAX_ECC_BITS_USE));
#else
    err = mp_init(*dst);
#endif
    if (err == MP_OKAY) {
        curve->load_mask |= mask;

        err = mp_read_radix(*dst, src, MP_RADIX_HEX);

    #ifdef HAVE_WOLF_BIGINT
        if (err == MP_OKAY)
            err = wc_mp_to_bigint(*dst, &(*dst)->raw);
    #endif
    }
    return err;
}

static int wc_ecc_curve_load(const ecc_set_type* dp, ecc_curve_spec** pCurve,
    byte load_mask)
{
    int ret = 0;
    ecc_curve_spec* curve;
    byte load_items = 0; /* mask of items to load */
#ifdef ECC_CACHE_CURVE
    int x;
#endif

    if (dp == NULL || pCurve == NULL)
        return BAD_FUNC_ARG;

#ifdef ECC_CACHE_CURVE
    x = wc_ecc_get_curve_idx(dp->id);
    if (x == ECC_CURVE_INVALID)
        return ECC_BAD_ARG_E;

#if !defined(SINGLE_THREADED)
    ret = wc_LockMutex(&ecc_curve_cache_mutex);
    if (ret != 0) {
        return ret;
    }
#endif

    /* make sure cache has been allocated */
    if (ecc_curve_spec_cache[x] == NULL
    #ifdef WOLFSSL_CUSTOM_CURVES
        || dp->id == ECC_CURVE_CUSTOM
    #endif
    ) {
        curve = (ecc_curve_spec*)XMALLOC(sizeof(ecc_curve_spec), NULL, DYNAMIC_TYPE_ECC);
        if (curve == NULL) {
        #if defined(ECC_CACHE_CURVE) && !defined(SINGLE_THREADED)
            wc_UnLockMutex(&ecc_curve_cache_mutex);
        #endif
            return MEMORY_E;
        }
        XMEMSET(curve, 0, sizeof(ecc_curve_spec));

        /* set curve pointer to cache */
    #ifdef WOLFSSL_CUSTOM_CURVES
        if (dp->id != ECC_CURVE_CUSTOM)
    #endif
        {
            ecc_curve_spec_cache[x] = curve;
        }
    }
    else {
        curve = ecc_curve_spec_cache[x];
    }
    /* return new or cached curve */
    *pCurve = curve;
#else
    curve = *pCurve;
#endif /* ECC_CACHE_CURVE */

    /* make sure the curve is initialized */
    if (curve->dp != dp) {
        curve->load_mask = 0;

    #ifdef ECC_CACHE_CURVE
        curve->prime = &curve->prime_lcl;
        curve->Af = &curve->Af_lcl;
        #ifdef USE_ECC_B_PARAM
            curve->Bf = &curve->Bf_lcl;
        #endif
        curve->order = &curve->order_lcl;
        curve->Gx = &curve->Gx_lcl;
        curve->Gy = &curve->Gy_lcl;
    #endif
    }
    curve->dp = dp; /* set dp info */

    /* determine items to load */
    load_items = (byte)(((byte)~(word32)curve->load_mask) & load_mask);
    curve->load_mask |= load_items;

    /* load items */
    if (load_items & ECC_CURVE_FIELD_PRIME)
        ret += wc_ecc_curve_cache_load_item(curve, dp->prime, &curve->prime,
            ECC_CURVE_FIELD_PRIME);
    if (load_items & ECC_CURVE_FIELD_AF)
        ret += wc_ecc_curve_cache_load_item(curve, dp->Af, &curve->Af,
            ECC_CURVE_FIELD_AF);
#ifdef USE_ECC_B_PARAM
    if (load_items & ECC_CURVE_FIELD_BF)
        ret += wc_ecc_curve_cache_load_item(curve, dp->Bf, &curve->Bf,
            ECC_CURVE_FIELD_BF);
#endif
    if (load_items & ECC_CURVE_FIELD_ORDER)
        ret += wc_ecc_curve_cache_load_item(curve, dp->order, &curve->order,
            ECC_CURVE_FIELD_ORDER);
    if (load_items & ECC_CURVE_FIELD_GX)
        ret += wc_ecc_curve_cache_load_item(curve, dp->Gx, &curve->Gx,
            ECC_CURVE_FIELD_GX);
    if (load_items & ECC_CURVE_FIELD_GY)
        ret += wc_ecc_curve_cache_load_item(curve, dp->Gy, &curve->Gy,
            ECC_CURVE_FIELD_GY);

    /* check for error */
    if (ret != 0) {
        wc_ecc_curve_free(curve);
        ret = MP_READ_E;
    }

#if defined(ECC_CACHE_CURVE) && !defined(SINGLE_THREADED)
    wc_UnLockMutex(&ecc_curve_cache_mutex);
#endif

    return ret;
}

#ifdef ECC_CACHE_CURVE
int wc_ecc_curve_cache_init(void)
{
    int ret = 0;
#if defined(ECC_CACHE_CURVE) && !defined(SINGLE_THREADED)
    ret = wc_InitMutex(&ecc_curve_cache_mutex);
#endif
    return ret;
}

void wc_ecc_curve_cache_free(void)
{
    int x;

    /* free all ECC curve caches */
    for (x = 0; x < (int)ECC_SET_COUNT; x++) {
        if (ecc_curve_spec_cache[x]) {
            wc_ecc_curve_cache_free_spec(ecc_curve_spec_cache[x]);
            XFREE(ecc_curve_spec_cache[x], NULL, DYNAMIC_TYPE_ECC);
            ecc_curve_spec_cache[x] = NULL;
        }
    }

#if defined(ECC_CACHE_CURVE) && !defined(SINGLE_THREADED)
    wc_FreeMutex(&ecc_curve_cache_mutex);
#endif
}
#endif /* ECC_CACHE_CURVE */


/* Retrieve the curve name for the ECC curve id.
 *
 * curve_id  The id of the curve.
 * returns the name stored from the curve if available, otherwise NULL.
 */
const char* wc_ecc_get_name(int curve_id)
{
    int curve_idx = wc_ecc_get_curve_idx(curve_id);
    if (curve_idx == ECC_CURVE_INVALID)
        return NULL;
    return ecc_sets[curve_idx].name;
}

int wc_ecc_set_curve(ecc_key* key, int keysize, int curve_id)
{
    if (key == NULL || (keysize <= 0 && curve_id < 0)) {
        return BAD_FUNC_ARG;
    }

    if (keysize > ECC_MAXSIZE) {
        return ECC_BAD_ARG_E;
    }

    /* handle custom case */
    if (key->idx != ECC_CUSTOM_IDX) {
        int x;

        /* default values */
        key->idx = 0;
        key->dp = NULL;

        /* find ecc_set based on curve_id or key size */
        for (x = 0; ecc_sets[x].size != 0; x++) {
            if (curve_id > ECC_CURVE_DEF) {
                if (curve_id == ecc_sets[x].id)
                  break;
            }
            else if (keysize <= ecc_sets[x].size) {
                break;
            }
        }
        if (ecc_sets[x].size == 0) {
            WOLFSSL_MSG("ECC Curve not found");
            return ECC_CURVE_OID_E;
        }

        key->idx = x;
        key->dp  = &ecc_sets[x];
    }

    return 0;
}


#ifdef ALT_ECC_SIZE
static void alt_fp_init(mp_int* a)
{
    a->size = FP_SIZE_ECC;
    mp_zero(a);
}
#endif /* ALT_ECC_SIZE */


#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLF_CRYPTO_CB_ONLY_ECC)

#if !defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_PUBLIC_ECC_ADD_DBL)
static int _ecc_projective_dbl_point(ecc_point *P, ecc_point *R, mp_int* a,
                                     mp_int* modulus, mp_digit mp);

/**
   Add two ECC points
   P        The point to add
   Q        The point to add
   R        [out] The destination of the double
   a        ECC curve parameter a
   modulus  The modulus of the field the ECC curve is in
   mp       The "b" value from montgomery_setup()
   return   MP_OKAY on success
*/
static int _ecc_projective_add_point(ecc_point* P, ecc_point* Q, ecc_point* R,
                                     mp_int* a, mp_int* modulus, mp_digit mp)
{
#if !defined(WOLFSSL_SP_MATH)
   DECL_MP_INT_SIZE_DYN(t1, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(t2, mp_bitsused(modulus), MAX_ECC_BITS_USE);
#ifdef ALT_ECC_SIZE
   DECL_MP_INT_SIZE_DYN(rx, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(ry, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(rz, mp_bitsused(modulus), MAX_ECC_BITS_USE);
#endif
   mp_int  *x, *y, *z;
   int     err;

   /* if Q == R then swap P and Q, so we don't require a local x,y,z */
   if (Q == R) {
      ecc_point* tPt  = P;
      P = Q;
      Q = tPt;
   }

#ifdef WOLFSSL_SMALL_STACK
#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (R->key != NULL) {
       t1 = R->key->t1;
       t2 = R->key->t2;
#ifdef ALT_ECC_SIZE
       rx = R->key->x;
       ry = R->key->y;
       rz = R->key->z;
#endif
   }
   else
#endif /* WOLFSSL_SMALL_STACK_CACHE */
#endif /* WOLFSSL_SMALL_STACK */
   {
      NEW_MP_INT_SIZE(t1, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(t2, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
   #ifdef MP_INT_SIZE_CHECK_NULL
      if (t1 == NULL || t2 == NULL) {
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
         return MEMORY_E;
      }
   #endif
#ifdef ALT_ECC_SIZE
      NEW_MP_INT_SIZE(rx, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(ry, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(rz, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
   #ifdef MP_INT_SIZE_CHECK_NULL
      if (rx == NULL || ry == NULL || rz == NULL) {
         FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
         return MEMORY_E;
      }
   #endif
#endif
   }

   err = INIT_MP_INT_SIZE(t1, mp_bitsused(modulus));
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(t2, mp_bitsused(modulus));
   }
   if (err != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
   #ifdef WOLFSSL_SMALL_STACK_CACHE
      if (R->key == NULL)
   #endif
#endif
      {
      #ifdef ALT_ECC_SIZE
         FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
      #endif
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
      }
      return err;
   }

   /* should we dbl instead? */
   if (err == MP_OKAY) {
#ifdef ECC_TIMING_RESISTANT
      err = mp_submod_ct(modulus, Q->y, modulus, t1);
#else
      err = mp_sub(modulus, Q->y, t1);
#endif
   }
   if (err == MP_OKAY) {
      if ( (mp_cmp(P->x, Q->x) == MP_EQ) &&
           (get_digit_count(Q->z) && mp_cmp(P->z, Q->z) == MP_EQ) &&
           (mp_cmp(P->y, Q->y) == MP_EQ || mp_cmp(P->y, t1) == MP_EQ)) {
          mp_clear(t1);
          mp_clear(t2);
   #ifdef WOLFSSL_SMALL_STACK
      #ifdef WOLFSSL_SMALL_STACK_CACHE
         if (R->key == NULL)
      #endif
   #endif
         {
         #ifdef ALT_ECC_SIZE
            FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
            FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
            FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
         #endif
            FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
            FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
         }
         return _ecc_projective_dbl_point(P, R, a, modulus, mp);
      }
   }

   if (err != MP_OKAY) {
      goto done;
   }

/* If use ALT_ECC_SIZE we need to use local stack variable since
   ecc_point x,y,z is reduced size */
#ifdef ALT_ECC_SIZE
   /* Use local stack variable */
   x = rx;
   y = ry;
   z = rz;

   err = INIT_MP_INT_SIZE(x, mp_bitsused(modulus));
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(y, mp_bitsused(modulus));
   }
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(z, mp_bitsused(modulus));
   }
   if (err != MP_OKAY) {
      goto done;
   }
#else
   /* Use destination directly */
   x = R->x;
   y = R->y;
   z = R->z;
#endif

   if (err == MP_OKAY)
       err = mp_copy(P->x, x);
   if (err == MP_OKAY)
       err = mp_copy(P->y, y);
   if (err == MP_OKAY)
       err = mp_copy(P->z, z);

   /* if Z is one then these are no-operations */
   if (err == MP_OKAY) {
       if (!mp_iszero(Q->z)) {
           /* T1 = Z' * Z' */
           err = mp_sqr(Q->z, t1);
           if (err == MP_OKAY)
               err = mp_montgomery_reduce(t1, modulus, mp);

           /* X = X * T1 */
           if (err == MP_OKAY)
               err = mp_mul(t1, x, x);
           if (err == MP_OKAY)
               err = mp_montgomery_reduce(x, modulus, mp);

           /* T1 = Z' * T1 */
           if (err == MP_OKAY)
               err = mp_mul(Q->z, t1, t1);
           if (err == MP_OKAY)
               err = mp_montgomery_reduce(t1, modulus, mp);

           /* Y = Y * T1 */
           if (err == MP_OKAY)
               err = mp_mul(t1, y, y);
           if (err == MP_OKAY)
               err = mp_montgomery_reduce(y, modulus, mp);
       }
   }

   /* T1 = Z*Z */
   if (err == MP_OKAY)
       err = mp_sqr(z, t1);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t1, modulus, mp);

   /* T2 = X' * T1 */
   if (err == MP_OKAY)
       err = mp_mul(Q->x, t1, t2);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t2, modulus, mp);

   /* T1 = Z * T1 */
   if (err == MP_OKAY)
       err = mp_mul(z, t1, t1);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t1, modulus, mp);

   /* T1 = Y' * T1 */
   if (err == MP_OKAY)
       err = mp_mul(Q->y, t1, t1);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t1, modulus, mp);

   /* Y = Y - T1 */
   if (err == MP_OKAY)
       err = mp_submod_ct(y, t1, modulus, y);
   /* T1 = 2T1 */
   if (err == MP_OKAY)
       err = mp_addmod_ct(t1, t1, modulus, t1);
   /* T1 = Y + T1 */
   if (err == MP_OKAY)
       err = mp_addmod_ct(t1, y, modulus, t1);
   /* X = X - T2 */
   if (err == MP_OKAY)
       err = mp_submod_ct(x, t2, modulus, x);
   /* T2 = 2T2 */
   if (err == MP_OKAY)
       err = mp_addmod_ct(t2, t2, modulus, t2);
   /* T2 = X + T2 */
   if (err == MP_OKAY)
       err = mp_addmod_ct(t2, x, modulus, t2);

   if (err == MP_OKAY) {
       if (!mp_iszero(Q->z)) {
           /* Z = Z * Z' */
           err = mp_mul(z, Q->z, z);
           if (err == MP_OKAY)
               err = mp_montgomery_reduce(z, modulus, mp);
       }
   }

   /* Z = Z * X */
   if (err == MP_OKAY)
       err = mp_mul(z, x, z);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(z, modulus, mp);

   /* T1 = T1 * X  */
   if (err == MP_OKAY)
       err = mp_mul(t1, x, t1);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t1, modulus, mp);

   /* X = X * X */
   if (err == MP_OKAY)
       err = mp_sqr(x, x);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(x, modulus, mp);

   /* T2 = T2 * x */
   if (err == MP_OKAY)
       err = mp_mul(t2, x, t2);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t2, modulus, mp);

   /* T1 = T1 * X  */
   if (err == MP_OKAY)
       err = mp_mul(t1, x, t1);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t1, modulus, mp);

   /* X = Y*Y */
   if (err == MP_OKAY)
       err = mp_sqr(y, x);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(x, modulus, mp);

   /* X = X - T2 */
   if (err == MP_OKAY)
       err = mp_submod_ct(x, t2, modulus, x);
   /* T2 = T2 - X */
   if (err == MP_OKAY)
       err = mp_submod_ct(t2, x, modulus, t2);
   /* T2 = T2 - X */
   if (err == MP_OKAY)
       err = mp_submod_ct(t2, x, modulus, t2);
   /* T2 = T2 * Y */
   if (err == MP_OKAY)
       err = mp_mul(t2, y, t2);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t2, modulus, mp);

   /* Y = T2 - T1 */
   if (err == MP_OKAY)
       err = mp_submod_ct(t2, t1, modulus, y);
   /* Y = Y/2 */
   if (err == MP_OKAY)
       err = mp_div_2_mod_ct(y, modulus, y);

#ifdef ALT_ECC_SIZE
   if (err == MP_OKAY)
       err = mp_copy(x, R->x);
   if (err == MP_OKAY)
       err = mp_copy(y, R->y);
   if (err == MP_OKAY)
       err = mp_copy(z, R->z);
#endif

done:

   /* clean up */
   mp_clear(t1);
   mp_clear(t2);
#ifdef WOLFSSL_SMALL_STACK
#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (R->key == NULL)
#endif
#endif
   {
   #ifdef ALT_ECC_SIZE
      FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
      FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
      FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
   #endif
      FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
      FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
   }

   return err;
#else
    int modBits = mp_count_bits(modulus);

    (void)a;
    (void)mp;

#ifndef WOLFSSL_SP_NO_256
    if (modBits == 256) {
#ifdef SM2_SP_IMPL_AVAILABLE
    #ifdef WOLFSSL_SM2
        if (!mp_is_bit_set(modulus, 224)) {
           return sp_ecc_proj_add_point_sm2_256(P->x, P->y, P->z, Q->x, Q->y,
                                                Q->z, R->x, R->y, R->z);
        }
    #endif
#endif
        return sp_ecc_proj_add_point_256(P->x, P->y, P->z, Q->x, Q->y, Q->z,
                                         R->x, R->y, R->z);
    }
#endif
#ifdef WOLFSSL_SP_384
    if (modBits == 384) {
        return sp_ecc_proj_add_point_384(P->x, P->y, P->z, Q->x, Q->y, Q->z,
                                         R->x, R->y, R->z);
    }
#endif
#ifdef WOLFSSL_SP_521
    if (modBits == 521) {
        return sp_ecc_proj_add_point_521(P->x, P->y, P->z, Q->x, Q->y, Q->z,
                                         R->x, R->y, R->z);
    }
#endif
    return ECC_BAD_ARG_E;
#endif
}

int ecc_projective_add_point(ecc_point* P, ecc_point* Q, ecc_point* R,
                             mp_int* a, mp_int* modulus, mp_digit mp)
{
    if (P == NULL || Q == NULL || R == NULL || modulus == NULL) {
        return ECC_BAD_ARG_E;
    }

    if (mp_cmp(P->x, modulus) != MP_LT ||
        mp_cmp(P->y, modulus) != MP_LT ||
        mp_cmp(P->z, modulus) != MP_LT ||
        mp_cmp(Q->x, modulus) != MP_LT ||
        mp_cmp(Q->y, modulus) != MP_LT ||
        mp_cmp(Q->z, modulus) != MP_LT) {
        return ECC_OUT_OF_RANGE_E;
    }

    return _ecc_projective_add_point(P, Q, R, a, modulus, mp);
}

/* ### Point doubling in Jacobian coordinate system ###
 *
 * let us have a curve:                 y^2 = x^3 + a*x + b
 * in Jacobian coordinates it becomes:  y^2 = x^3 + a*x*z^4 + b*z^6
 *
 * The doubling of P = (Xp, Yp, Zp) is given by R = (Xr, Yr, Zr) where:
 * Xr = M^2 - 2*S
 * Yr = M * (S - Xr) - 8*T
 * Zr = 2 * Yp * Zp
 *
 * M = 3 * Xp^2 + a*Zp^4
 * T = Yp^4
 * S = 4 * Xp * Yp^2
 *
 * SPECIAL CASE: when a == 3 we can compute M as
 * M = 3 * (Xp^2 - Zp^4) = 3 * (Xp + Zp^2) * (Xp - Zp^2)
 */

/**
   Double an ECC point
   P   The point to double
   R   [out] The destination of the double
   a   ECC curve parameter a
   modulus  The modulus of the field the ECC curve is in
   mp       The "b" value from montgomery_setup()
   return   MP_OKAY on success
*/
static int _ecc_projective_dbl_point(ecc_point *P, ecc_point *R, mp_int* a,
                                     mp_int* modulus, mp_digit mp)
{
#if !defined(WOLFSSL_SP_MATH)
   DECL_MP_INT_SIZE_DYN(t1, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(t2, mp_bitsused(modulus), MAX_ECC_BITS_USE);
#ifdef ALT_ECC_SIZE
   DECL_MP_INT_SIZE_DYN(rx, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(ry, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(rz, mp_bitsused(modulus), MAX_ECC_BITS_USE);
#endif
   mp_int *x, *y, *z;
   int    err;

#ifdef WOLFSSL_SMALL_STACK
#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (R->key != NULL) {
       t1 = R->key->t1;
       t2 = R->key->t2;
   #ifdef ALT_ECC_SIZE
       rx = R->key->x;
       ry = R->key->y;
       rz = R->key->z;
   #endif
   }
   else
#endif /* WOLFSSL_SMALL_STACK_CACHE */
#endif
   {
      NEW_MP_INT_SIZE(t1, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(t2, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
   #ifdef MP_INT_SIZE_CHECK_NULL
      if (t1 == NULL || t2 == NULL) {
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
         return MEMORY_E;
      }
   #endif
   #ifdef ALT_ECC_SIZE
      NEW_MP_INT_SIZE(rx, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(ry, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(rz, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
   #ifdef MP_INT_SIZE_CHECK_NULL
      if (rx == NULL || ry == NULL || rz == NULL) {
          FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
          return MEMORY_E;
      }
   #endif
   #endif
   }

   err = INIT_MP_INT_SIZE(t1, mp_bitsused(modulus));
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(t2, mp_bitsused(modulus));
   }
   if (err != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
   #ifdef WOLFSSL_SMALL_STACK_CACHE
      if (R->key == NULL)
   #endif
#endif
      {
      #ifdef ALT_ECC_SIZE
         FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
      #endif
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
      }
      return err;
   }

/* If use ALT_ECC_SIZE we need to use local stack variable since
   ecc_point x,y,z is reduced size */
#ifdef ALT_ECC_SIZE
   /* Use local stack variable */
   x = rx;
   y = ry;
   z = rz;

   err = INIT_MP_INT_SIZE(x, mp_bitsused(modulus));
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(y, mp_bitsused(modulus));
   }
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(z, mp_bitsused(modulus));
   }
   if (err != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
   #ifdef WOLFSSL_SMALL_STACK_CACHE
      if (R->key == NULL)
   #endif
#endif
      {
      #ifdef ALT_ECC_SIZE
         FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
      #endif
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
      }
      return err;
   }
#else
   /* Use destination directly */
   x = R->x;
   y = R->y;
   z = R->z;
#endif

   if (err == MP_OKAY)
       err = mp_copy(P->x, x);
   if (err == MP_OKAY)
       err = mp_copy(P->y, y);
   if (err == MP_OKAY)
       err = mp_copy(P->z, z);

   /* T1 = Z * Z */
   if (err == MP_OKAY)
       err = mp_sqr(z, t1);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t1, modulus, mp);

   /* Z = Y * Z */
   if (err == MP_OKAY)
       err = mp_mul(z, y, z);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(z, modulus, mp);

   /* Z = 2Z */
   if (err == MP_OKAY)
       err = mp_addmod_ct(z, z, modulus, z);

   /* Determine if curve "a" should be used in calc */
#ifdef WOLFSSL_CUSTOM_CURVES
   if (err == MP_OKAY) {
      /* Use a and prime to determine if a == 3 */
      err = mp_submod(modulus, a, modulus, t2);
   }
   if (err == MP_OKAY && mp_iszero((MP_INT_SIZE*)t2)) {
      /* T2 = X * X */
      if (err == MP_OKAY)
          err = mp_sqr(x, t2);
      if (err == MP_OKAY)
          err = mp_montgomery_reduce(t2, modulus, mp);
      /* T1 = T2 + T1 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t2, t2, modulus, t1);
      /* T1 = T2 + T1 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t1, t2, modulus, t1);
   }
   else if (err == MP_OKAY && mp_cmp_d(t2, 3) != MP_EQ) {
      /* use "a" in calc */

      /* T2 = T1 * T1 */
      if (err == MP_OKAY)
          err = mp_sqr(t1, t2);
      if (err == MP_OKAY)
          err = mp_montgomery_reduce(t2, modulus, mp);
      /* T1 = T2 * a */
      if (err == MP_OKAY)
          err = mp_mulmod(t2, a, modulus, t1);
      /* T2 = X * X */
      if (err == MP_OKAY)
          err = mp_sqr(x, t2);
      if (err == MP_OKAY)
          err = mp_montgomery_reduce(t2, modulus, mp);
      /* T1 = T2 + T1 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t1, t2, modulus, t1);
      /* T1 = T2 + T1 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t1, t2, modulus, t1);
      /* T1 = T2 + T1 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t1, t2, modulus, t1);
   }
   else
#endif /* WOLFSSL_CUSTOM_CURVES */
   {
      /* assumes "a" == 3 */
      (void)a;

      /* T2 = X - T1 */
      if (err == MP_OKAY)
          err = mp_submod_ct(x, t1, modulus, t2);
      /* T1 = X + T1 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t1, x, modulus, t1);
      /* T2 = T1 * T2 */
      if (err == MP_OKAY)
          err = mp_mul(t1, t2, t2);
      if (err == MP_OKAY)
          err = mp_montgomery_reduce(t2, modulus, mp);

      /* T1 = 2T2 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t2, t2, modulus, t1);
      /* T1 = T1 + T2 */
      if (err == MP_OKAY)
          err = mp_addmod_ct(t1, t2, modulus, t1);
   }

   /* Y = 2Y */
   if (err == MP_OKAY)
       err = mp_addmod_ct(y, y, modulus, y);
   /* Y = Y * Y */
   if (err == MP_OKAY)
       err = mp_sqr(y, y);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(y, modulus, mp);

   /* T2 = Y * Y */
   if (err == MP_OKAY)
       err = mp_sqr(y, t2);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(t2, modulus, mp);

   /* T2 = T2/2 */
   if (err == MP_OKAY)
       err = mp_div_2_mod_ct(t2, modulus, t2);

   /* Y = Y * X */
   if (err == MP_OKAY)
       err = mp_mul(y, x, y);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(y, modulus, mp);

   /* X = T1 * T1 */
   if (err == MP_OKAY)
       err = mp_sqr(t1, x);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(x, modulus, mp);

   /* X = X - Y */
   if (err == MP_OKAY)
       err = mp_submod_ct(x, y, modulus, x);
   /* X = X - Y */
   if (err == MP_OKAY)
       err = mp_submod_ct(x, y, modulus, x);

   /* Y = Y - X */
   if (err == MP_OKAY)
       err = mp_submod_ct(y, x, modulus, y);
   /* Y = Y * T1 */
   if (err == MP_OKAY)
       err = mp_mul(y, t1, y);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(y, modulus, mp);

   /* Y = Y - T2 */
   if (err == MP_OKAY)
       err = mp_submod_ct(y, t2, modulus, y);

#ifdef ALT_ECC_SIZE
   if (err == MP_OKAY)
       err = mp_copy(x, R->x);
   if (err == MP_OKAY)
       err = mp_copy(y, R->y);
   if (err == MP_OKAY)
       err = mp_copy(z, R->z);
#endif

   /* clean up */
   mp_clear(t1);
   mp_clear(t2);

#ifdef WOLFSSL_SMALL_STACK
#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (R->key == NULL)
#endif
#endif
   {
    #ifdef ALT_ECC_SIZE
       FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
       FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
       FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
    #endif
       FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
       FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
    }

   return err;
#else
    int modBits = mp_count_bits(modulus);

    (void)a;
    (void)mp;

#ifndef WOLFSSL_SP_NO_256
    if (modBits == 256) {
#ifdef SM2_SP_IMPL_AVAILABLE
    #ifdef WOLFSSL_SM2
        if (!mp_is_bit_set(modulus, 224)) {
           return sp_ecc_proj_dbl_point_sm2_256(P->x, P->y, P->z, R->x, R->y,
                                                R->z);
        }
    #endif
#endif
        return sp_ecc_proj_dbl_point_256(P->x, P->y, P->z, R->x, R->y, R->z);
    }
#endif
#ifdef WOLFSSL_SP_384
    if (modBits == 384) {
        return sp_ecc_proj_dbl_point_384(P->x, P->y, P->z, R->x, R->y, R->z);
    }
#endif
#ifdef WOLFSSL_SP_521
    if (modBits == 521) {
        return sp_ecc_proj_dbl_point_521(P->x, P->y, P->z, R->x, R->y, R->z);
    }
#endif
    return ECC_BAD_ARG_E;
#endif
}

int ecc_projective_dbl_point(ecc_point *P, ecc_point *R, mp_int* a,
                             mp_int* modulus, mp_digit mp)
{
    if (P == NULL || R == NULL || modulus == NULL)
        return ECC_BAD_ARG_E;

    if (mp_cmp(P->x, modulus) != MP_LT ||
        mp_cmp(P->y, modulus) != MP_LT ||
        mp_cmp(P->z, modulus) != MP_LT) {
        return ECC_OUT_OF_RANGE_E;
    }

    return _ecc_projective_dbl_point(P, R, a, modulus, mp);
}

#if !defined(FREESCALE_LTC_ECC) && !defined(WOLFSSL_STM32_PKA) && \
    !defined(WOLFSSL_CRYPTOCELL)


/**
  Map a projective Jacobian point back to affine space
  P        [in/out] The point to map
  modulus  The modulus of the field the ECC curve is in
  mp       The "b" value from montgomery_setup()
  ct       Operation should be constant time.
  return   MP_OKAY on success
*/
int ecc_map_ex(ecc_point* P, mp_int* modulus, mp_digit mp, int ct)
{
#if !defined(WOLFSSL_SP_MATH)
   DECL_MP_INT_SIZE_DYN(t1, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(t2, mp_bitsused(modulus), MAX_ECC_BITS_USE);
#ifdef ALT_ECC_SIZE
   DECL_MP_INT_SIZE_DYN(rx, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(ry, mp_bitsused(modulus), MAX_ECC_BITS_USE);
   DECL_MP_INT_SIZE_DYN(rz, mp_bitsused(modulus), MAX_ECC_BITS_USE);
#endif
   mp_int *x, *y, *z;
   int    err;

   (void)ct;

   if (P == NULL || modulus == NULL)
       return ECC_BAD_ARG_E;

   /* special case for point at infinity */
   if (mp_cmp_d(P->z, 0) == MP_EQ) {
       err = mp_set(P->x, 0);
       if (err == MP_OKAY)
           err = mp_set(P->y, 0);
       if (err == MP_OKAY)
           err = mp_set(P->z, 1);
       return err;
   }

#ifdef WOLFSSL_SMALL_STACK
#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (P->key != NULL) {
       t1 = P->key->t1;
       t2 = P->key->t2;
   #ifdef ALT_ECC_SIZE
       rx = P->key->x;
       ry = P->key->y;
       rz = P->key->z;
   #endif
   }
   else
#endif /* WOLFSSL_SMALL_STACK_CACHE */
#endif
   {
      NEW_MP_INT_SIZE(t1, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(t2, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
   #ifdef MP_INT_SIZE_CHECK_NULL
      if (t1 == NULL || t2 == NULL) {
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
         return MEMORY_E;
      }
   #endif
   #ifdef ALT_ECC_SIZE
      NEW_MP_INT_SIZE(rx, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(ry, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
      NEW_MP_INT_SIZE(rz, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
   #ifdef MP_INT_SIZE_CHECK_NULL
      if (rx == NULL || ry == NULL || rz == NULL) {
          FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
          FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
          return MEMORY_E;
      }
   #endif
   #endif
   }

   err = INIT_MP_INT_SIZE(t1, mp_bitsused(modulus));
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(t2, mp_bitsused(modulus));
   }
   if (err != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
   #ifdef WOLFSSL_SMALL_STACK_CACHE
      if (P->key == NULL)
   #endif
#endif
      {
      #ifdef ALT_ECC_SIZE
         FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
      #endif
         FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
         FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
      }
      return MEMORY_E;
   }

#ifdef ALT_ECC_SIZE
   /* Use local stack variable */
   x = rx;
   y = ry;
   z = rz;

   err = INIT_MP_INT_SIZE(x, mp_bitsused(modulus));
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(y, mp_bitsused(modulus));
   }
   if (err == MP_OKAY) {
      err = INIT_MP_INT_SIZE(z, mp_bitsused(modulus));
   }
   if (err != MP_OKAY) {
      goto done;
   }

   if (err == MP_OKAY)
      err = mp_copy(P->x, x);
   if (err == MP_OKAY)
      err = mp_copy(P->y, y);
   if (err == MP_OKAY)
      err = mp_copy(P->z, z);

   if (err != MP_OKAY) {
      goto done;
   }
#else
   /* Use destination directly */
   x = P->x;
   y = P->y;
   z = P->z;
#endif

   /* get 1/z */
   if (err == MP_OKAY) {
#if defined(ECC_TIMING_RESISTANT) && (defined(USE_FAST_MATH) || \
                       defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_SP_MATH_ALL))
       if (ct) {
           err = mp_invmod_mont_ct(z, modulus, t1, mp);
           if (err == MP_OKAY)
               err = mp_montgomery_reduce(t1, modulus, mp);
       }
       else
#endif
       {
           /* first map z back to normal */
           err = mp_montgomery_reduce(z, modulus, mp);
           if (err == MP_OKAY)
               err = mp_invmod(z, modulus, t1);
       }
   }

   /* get 1/z^2 and 1/z^3 */
   if (err == MP_OKAY)
       err = mp_sqr(t1, t2);
   if (err == MP_OKAY)
       err = mp_mod(t2, modulus, t2);
   if (err == MP_OKAY)
       err = mp_mul(t1, t2, t1);
   if (err == MP_OKAY)
       err = mp_mod(t1, modulus, t1);

   /* multiply against x/y */
   if (err == MP_OKAY)
       err = mp_mul(x, t2, x);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(x, modulus, mp);
   if (err == MP_OKAY)
       err = mp_mul(y, t1, y);
   if (err == MP_OKAY)
       err = mp_montgomery_reduce(y, modulus, mp);

   if (err == MP_OKAY)
       err = mp_set(z, 1);

#ifdef ALT_ECC_SIZE
   /* return result */
   if (err == MP_OKAY)
      err = mp_copy(x, P->x);
   if (err == MP_OKAY)
      err = mp_copy(y, P->y);
   if (err == MP_OKAY)
      err = mp_copy(z, P->z);

done:
#endif

   /* clean up */
   mp_clear(t1);
   mp_clear(t2);

#ifdef WOLFSSL_SMALL_STACK
#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (P->key == NULL)
#endif
#endif
   {
   #ifdef ALT_ECC_SIZE
      FREE_MP_INT_SIZE(rz, NULL, DYNAMIC_TYPE_ECC);
      FREE_MP_INT_SIZE(ry, NULL, DYNAMIC_TYPE_ECC);
      FREE_MP_INT_SIZE(rx, NULL, DYNAMIC_TYPE_ECC);
   #endif
      FREE_MP_INT_SIZE(t2, NULL, DYNAMIC_TYPE_ECC);
      FREE_MP_INT_SIZE(t1, NULL, DYNAMIC_TYPE_ECC);
   }

   return err;
#else
   if (P == NULL || modulus == NULL)
       return ECC_BAD_ARG_E;

   (void)mp;
   (void)ct;

#ifndef WOLFSSL_SP_NO_256
   if (mp_count_bits(modulus) == 256) {
#ifdef SM2_SP_IMPL_AVAILABLE
    #ifdef WOLFSSL_SM2
        if (!mp_is_bit_set(modulus, 224)) {
           return sp_ecc_map_sm2_256(P->x, P->y, P->z);
        }
    #endif
#endif
       return sp_ecc_map_256(P->x, P->y, P->z);
   }
#endif
#ifdef WOLFSSL_SP_384
   if (mp_count_bits(modulus) == 384) {
       return sp_ecc_map_384(P->x, P->y, P->z);
   }
#endif
#ifdef WOLFSSL_SP_521
   if (mp_count_bits(modulus) == 521) {
       return sp_ecc_map_521(P->x, P->y, P->z);
   }
#endif
   return ECC_BAD_ARG_E;
#endif
}
#endif /* !FREESCALE_LTC_ECC && !WOLFSSL_STM32_PKA */

int ecc_map(ecc_point* P, mp_int* modulus, mp_digit mp)
{
    return ecc_map_ex(P, modulus, mp, 0);
}
#endif /* !WOLFSSL_SP_MATH || WOLFSSL_PUBLIC_ECC_ADD_DBL */

#if !defined(FREESCALE_LTC_ECC) && !defined(WOLFSSL_STM32_PKA) && \
    !defined(WOLFSSL_CRYPTOCELL)
#if !defined(WOLFSSL_SP_MATH)

#ifndef ECC_TIMING_RESISTANT

/* size of sliding window, don't change this! */
#define WINSIZE  4
#define M_POINTS 8

static int ecc_mulmod(const mp_int* k, ecc_point* tG, ecc_point* R,
    ecc_point** M, mp_int* a, mp_int* modulus, mp_digit mp, WC_RNG* rng)
{
   int      err = MP_OKAY;
   int      i;
   int      first = 1, bitbuf = 0, bitcpy = 0, j;
   int      bitcnt = 0, mode = 0, digidx = 0;
   mp_digit buf;
   int      infinity;

   (void)rng;

   /* calc the M tab, which holds kG for k==8..15 */
   /* M[0] == 8G */
   if (err == MP_OKAY)
       err = ecc_projective_dbl_point_safe(tG, M[0], a, modulus, mp);
   if (err == MP_OKAY)
       err = ecc_projective_dbl_point_safe(M[0], M[0], a, modulus, mp);
   if (err == MP_OKAY)
       err = ecc_projective_dbl_point_safe(M[0], M[0], a, modulus, mp);

   /* now find (8+k)G for k=1..7 */
   if (err == MP_OKAY)
       for (j = 9; j < 16; j++) {
           err = ecc_projective_add_point_safe(M[j-9], tG, M[j-M_POINTS], a,
                                                        modulus, mp, &infinity);
           if (err != MP_OKAY) break;
       }

   /* setup sliding window */
   if (err == MP_OKAY) {
       mode   = 0;
       bitcnt = 1;
       buf    = 0;
       digidx = get_digit_count(k) - 1;
       bitcpy = bitbuf = 0;
       first  = 1;

       /* perform ops */
       for (;;) {
           /* grab next digit as required */
           if (--bitcnt == 0) {
               if (digidx == -1) {
                   break;
               }
               buf    = get_digit(k, digidx);
               bitcnt = (int) DIGIT_BIT;
               --digidx;
           }

           /* grab the next msb from the ltiplicand */
           i = (int)(buf >> (DIGIT_BIT - 1)) & 1;
           buf <<= 1;

           /* skip leading zero bits */
           if (mode == 0 && i == 0)
               continue;

           /* if the bit is zero and mode == 1 then we double */
           if (mode == 1 && i == 0) {
               err = ecc_projective_dbl_point_safe(R, R, a, modulus, mp);
               if (err != MP_OKAY) break;
               continue;
           }

           /* else we add it to the window */
           bitbuf |= (i << (WINSIZE - ++bitcpy));
           mode = 2;

           if (bitcpy == WINSIZE) {
               /* if this is the first window we do a simple copy */
               if (first == 1) {
                   /* R = kG [k = first window] */
                   err = mp_copy(M[bitbuf-M_POINTS]->x, R->x);
                   if (err != MP_OKAY) break;

                   err = mp_copy(M[bitbuf-M_POINTS]->y, R->y);
                   if (err != MP_OKAY) break;

                   err = mp_copy(M[bitbuf-M_POINTS]->z, R->z);
                   first = 0;
               } else {
                   /* normal window */
                   /* ok window is filled so double as required and add  */
                   /* double first */
                   for (j = 0; j < WINSIZE; j++) {
                       err = ecc_projective_dbl_point_safe(R, R, a, modulus,
                                                                            mp);
                       if (err != MP_OKAY) break;
                   }
                   if (err != MP_OKAY) break;  /* out of first for(;;) */

                   /* now add, bitbuf will be 8..15 [8..2^WINSIZE] guaranteed */
                   err = ecc_projective_add_point_safe(R, M[bitbuf-M_POINTS], R,
                                                     a, modulus, mp, &infinity);
               }
               if (err != MP_OKAY) break;
               /* empty window and reset */
               bitcpy = bitbuf = 0;
               mode = 1;
           }
       }
   }

   /* if bits remain then double/add */
   if (err == MP_OKAY) {
       if (mode == 2 && bitcpy > 0) {
           /* double then add */
           for (j = 0; j < bitcpy; j++) {
               /* only double if we have had at least one add first */
               if (first == 0) {
                   err = ecc_projective_dbl_point_safe(R, R, a, modulus, mp);
                   if (err != MP_OKAY) break;
               }

               bitbuf <<= 1;
               if ((bitbuf & (1 << WINSIZE)) != 0) {
                   if (first == 1) {
                       /* first add, so copy */
                       err = mp_copy(tG->x, R->x);
                       if (err != MP_OKAY) break;

                       err = mp_copy(tG->y, R->y);
                       if (err != MP_OKAY) break;

                       err = mp_copy(tG->z, R->z);
                       if (err != MP_OKAY) break;
                       first = 0;
                   } else {
                       /* then add */
                       err = ecc_projective_add_point_safe(R, tG, R, a, modulus,
                                                                 mp, &infinity);
                       if (err != MP_OKAY) break;
                   }
               }
           }
       }
   }

   #undef WINSIZE

   return err;
}

#else

static int wc_ecc_gen_z(WC_RNG* rng, int size, ecc_point* p, mp_int* modulus,
    mp_digit mp, mp_int* tx, mp_int* ty, mp_int* mu)
{
    int err = MP_OKAY;

    err = mp_montgomery_calc_normalization(mu, modulus);
    /* Generate random value to multiply into p->z. */
    if (err == MP_OKAY)
        err = wc_ecc_gen_k(rng, size, ty, modulus);
    /* Convert to montogmery form. */
    if (err == MP_OKAY)
        err = mp_mulmod(ty, mu, modulus, ty);
    /* Multiply random value into p->z. */
    if (err == MP_OKAY)
        err = mp_mul(p->z, ty, p->z);
    if (err == MP_OKAY)
        err = mp_montgomery_reduce(p->z, modulus, mp);
    /* Square random value for X (X' = X / Z^2). */
    if (err == MP_OKAY)
        err = mp_sqr(ty, tx);
    if (err == MP_OKAY)
        err = mp_montgomery_reduce(tx, modulus, mp);
    /* Multiply square of random by random value for Y. */
    if (err == MP_OKAY)
        err = mp_mul(ty, tx, ty);
    if (err == MP_OKAY)
        err = mp_montgomery_reduce(ty, modulus, mp);
    /* Multiply square into X. */
    if (err == MP_OKAY)
        err = mp_mul(p->x, tx, p->x);
    if (err == MP_OKAY)
        err = mp_montgomery_reduce(p->x, modulus, mp);
    /* Multiply cube into Y (Y' = Y / Z^3). */
    if (err == MP_OKAY)
        err = mp_mul(p->y, ty, p->y);
    if (err == MP_OKAY)
        err = mp_montgomery_reduce(p->y, modulus, mp);

    return err;
}

#ifndef WC_PROTECT_ENCRYPTED_MEM
#define M_POINTS 3

/* Joye double-add ladder.
 * "Highly Regular Right-to-Left Algorithms for Scalar Multiplication"
 * by Marc Joye (2007)
 *
 * Algorithm 1':
 *   Input: P element of curve, k = (k[t-1],..., k[0]) base 2
 *   Output: Q = kP
 *   1: R[0] = P; R[1] = P
 *   2: for j = 1 to t-1 do
 *   3:   b = 1 - k[j]; R[b] = 2*R[b] + R[k[j]]
 *   4: end for
 *   5: b = k[0]; R[b] = R[b] - P
 *   6: return R[0]
 *
 * Assumes: k < order.
 */
static int ecc_mulmod(const mp_int* k, ecc_point* P, ecc_point* Q,
    ecc_point** R, mp_int* a, mp_int* modulus, mp_digit mp, WC_RNG* rng)
{
    int      err = MP_OKAY;
    int      bytes = (mp_count_bits(modulus) + 7) / 8;
    int      i;
    int      j = 1;
    int      cnt = DIGIT_BIT;
    int      t = 0;
    mp_digit b;
    mp_digit v = 0;
    mp_int*  kt = R[2]->x;
#ifndef WC_NO_CACHE_RESISTANT
    /* First bit always 1 (fix at end) and swap equals first bit */
    int      swap = 1;
#ifdef WOLFSSL_SMALL_STACK
    mp_int*  tmp = NULL;
#else
    mp_int   tmp[1];
#endif
#endif
    int      infinity;

#ifdef WOLFSSL_SMALL_STACK
    tmp = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
    if (tmp == NULL) {
        err = MEMORY_E;
    }
#endif
    if (err == MP_OKAY)
        err = mp_init(tmp);

    /* Step 1: R[0] = P; R[1] = P */
    /* R[0] = P */
    if (err == MP_OKAY)
        err = mp_copy(P->x, R[0]->x);
    if (err == MP_OKAY)
        err = mp_copy(P->y, R[0]->y);
    if (err == MP_OKAY)
        err = mp_copy(P->z, R[0]->z);

    /* R[1] = P */
    if (err == MP_OKAY)
        err = mp_copy(P->x, R[1]->x);
    if (err == MP_OKAY)
        err = mp_copy(P->y, R[1]->y);
    if (err == MP_OKAY)
        err = mp_copy(P->z, R[1]->z);

    /* Randomize z ordinates to obfuscate timing. */
    if ((err == MP_OKAY) && (rng != NULL))
        err = wc_ecc_gen_z(rng, bytes, R[0], modulus, mp, R[2]->x, R[2]->y, kt);
    if ((err == MP_OKAY) && (rng != NULL))
        err = wc_ecc_gen_z(rng, bytes, R[1], modulus, mp, R[2]->x, R[2]->y, kt);

    if (err == MP_OKAY) {
        /* Order could be one greater than the size of the modulus. */
        t = mp_count_bits(modulus) + 1;
        v = k->dp[0] >> 1;
        if (cnt > t) {
            cnt = t;
        }
        err = mp_copy(k, kt);
    }
    if (err == MP_OKAY) {
        err = mp_grow(kt, (int)modulus->used + 1);
    }
    /* Step 2: for j = 1 to t-1 do */
    for (i = 1; (err == MP_OKAY) && (i < t); i++) {
        if (--cnt == 0) {
            v = kt->dp[j++];
            cnt = DIGIT_BIT;
        }

        /* Step 3: b = 1 - k[j]; R[b] = 2*R[b] + R[k[j]] */
        b = v & 1;
        v >>= 1;
#ifdef WC_NO_CACHE_RESISTANT
        err = ecc_projective_dbl_point_safe(R[b^1], R[b^1], a, modulus, mp);
        if (err == MP_OKAY) {
            err = ecc_projective_add_point_safe(R[b^1], R[b], R[b^1], a,
                                                        modulus, mp, &infinity);
        }
#else
        /* Swap R[0] and R[1] if other index is needed. */
        swap ^= (int)b;
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->x, R[1]->x, (int)modulus->used, swap,
                tmp);
        }
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->y, R[1]->y, (int)modulus->used, swap,
                tmp);
        }
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->z, R[1]->z, (int)modulus->used, swap,
                tmp);
        }
        swap = (int)b;

        if (err == MP_OKAY)
            err = ecc_projective_dbl_point_safe(R[0], R[0], a, modulus, mp);
        if (err == MP_OKAY) {
            err = ecc_projective_add_point_safe(R[0], R[1], R[0], a, modulus,
                                                                 mp, &infinity);
        }
#endif /* WC_NO_CACHE_RESISTANT */
    }
    /* Step 4: end for */
#ifndef WC_NO_CACHE_RESISTANT
    /* Swap back if last bit is 0. */
    swap ^= 1;
    if (err == MP_OKAY) {
        err = mp_cond_swap_ct_ex(R[0]->x, R[1]->x, (int)modulus->used, swap,
            tmp);
    }
    if (err == MP_OKAY) {
        err = mp_cond_swap_ct_ex(R[0]->y, R[1]->y, (int)modulus->used, swap,
            tmp);
    }
    if (err == MP_OKAY) {
        err = mp_cond_swap_ct_ex(R[0]->z, R[1]->z, (int)modulus->used, swap,
            tmp);
    }
#endif

    /* Step 5: b = k[0]; R[b] = R[b] - P */
    /* R[2] = -P */
    if (err == MP_OKAY)
        err = mp_copy(P->x, R[2]->x);
    if (err == MP_OKAY)
        err = mp_sub(modulus, P->y, R[2]->y);
    if (err == MP_OKAY)
        err = mp_copy(P->z, R[2]->z);
    /* Subtract point by adding negative. */
    if (err == MP_OKAY) {
        b = k->dp[0] & 1;
#ifdef WC_NO_CACHE_RESISTANT
        err = ecc_projective_add_point_safe(R[b], R[2], R[b], a, modulus, mp,
                                                                     &infinity);
#else
        /* Swap R[0] and R[1], if necessary, to operate on the one we want. */
        err = mp_cond_swap_ct_ex(R[0]->x, R[1]->x, (int)modulus->used, (int)b,
            tmp);
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->y, R[1]->y, (int)modulus->used,
                (int)b, tmp);
        }
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->z, R[1]->z, (int)modulus->used,
                (int)b, tmp);
        }
        if (err == MP_OKAY)
            err = ecc_projective_add_point_safe(R[0], R[2], R[0], a, modulus,
                                                                 mp, &infinity);
        /* Swap back if necessary. */
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->x, R[1]->x, (int)modulus->used,
                (int)b, tmp);
        }
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->y, R[1]->y, (int)modulus->used,
                (int)b, tmp);
        }
        if (err == MP_OKAY) {
            err = mp_cond_swap_ct_ex(R[0]->z, R[1]->z, (int)modulus->used,
                (int)b, tmp);
        }
#endif
    }

    /* Step 6: return R[0] */
    if (err == MP_OKAY)
        err = mp_copy(R[0]->x, Q->x);
    if (err == MP_OKAY)
        err = mp_copy(R[0]->y, Q->y);
    if (err == MP_OKAY)
        err = mp_copy(R[0]->z, Q->z);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmp, NULL, DYNAMIC_TYPE_ECC);
#endif

    return err;
}

#else
/* Number of points to allocate for use during scalar multiplication. */
#define M_POINTS        5
/* Last of the points is used as a temporary during calculations. */
#define TMP_IDX         M_POINTS - 1

static void mp_cond_swap_into_ct(mp_int* ra, mp_int* rb, mp_int* a, mp_int* b,
    int digits, int m)
{
    int i;

#if !defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_SP_INT_NEGATIVE)
    /* Only using positive numbers in ECC operations. */
    ra->sign = 0;
    rb->sign = 0;
#endif
    /* Don't store 0 when mask is 0, it will be in a register. */
    ra->used = (int)(((a->used ^ b->used) & ((mp_digit)0 - (m & 1))) ^ a->used);
    rb->used = (int)(((a->used ^ b->used) & ((mp_digit)0 - (m & 1))) ^ b->used);
    for (i = 0; i < digits; i++) {
        ra->dp[i] = ((a->dp[i] ^ b->dp[i]) & ((mp_digit)0 - (m & 1))) ^
                    a->dp[i];
        rb->dp[i] = ((a->dp[i] ^ b->dp[i]) & ((mp_digit)0 - (m & 1))) ^
                    b->dp[i];
    }
}

static void ecc_cond_swap_into_ct(ecc_point* ra, ecc_point* rb, ecc_point* a,
    ecc_point* b, int digits, int m)
{
    /* Conditionally swap each ordinate. */
    mp_cond_swap_into_ct(ra->x, rb->x, a->x, b->x, digits, m);
    mp_cond_swap_into_ct(ra->y, rb->y, a->y, b->y, digits, m);
    mp_cond_swap_into_ct(ra->z, rb->z, a->z, b->z, digits, m);
}

/* Joye double-add ladder.
 * "Highly Regular Right-to-Left Algorithms for Scalar Multiplication"
 * by Marc Joye (2007)
 *
 * Algorithm 1':
 *   Input: P element of curve, k = (k[t-1],..., k[0]) base 2
 *   Output: Q = kP
 *   1: R[0] = P; R[1] = P
 *   2: for j = 1 to t-1 do
 *   3:   b = 1 - k[j]; R[b] = 2*R[b] + R[k[j]]
 *   4: end for
 *   5: b = k[0]; R[b] = R[b] - P
 *   6: return R[0]
 *
 * Assumes: k < order.
 */
static int ecc_mulmod(const mp_int* k, ecc_point* P, ecc_point* Q,
    ecc_point** R, mp_int* a, mp_int* modulus, mp_digit mp, WC_RNG* rng)
{
    int          err = MP_OKAY;
    int          bytes = (mp_count_bits(modulus) + 7) / 8;
    int          i;
    int          j = 1;
    int          cnt;
    int          t = 0;
    mp_int*      kt = R[TMP_IDX]->x;
    /* First bit always 1 (fix at end) and swap equals first bit */
    register int swap = 1;
    /* Which pair of points has current value. R[0,1] or R[2,3] */
    int          set = 0;
    int          infinity;

    /* Step 1: R[0] = P; R[1] = P */
    /* R[0] = P */
    if (err == MP_OKAY)
        err = mp_copy(P->x, R[0]->x);
    if (err == MP_OKAY)
        err = mp_copy(P->y, R[0]->y);
    if (err == MP_OKAY)
        err = mp_copy(P->z, R[0]->z);

    /* R[1] = P */
    if (err == MP_OKAY)
        err = mp_copy(P->x, R[1]->x);
    if (err == MP_OKAY)
        err = mp_copy(P->y, R[1]->y);
    if (err == MP_OKAY)
        err = mp_copy(P->z, R[1]->z);

    /* Randomize z ordinates to obfuscate timing. */
    if ((err == MP_OKAY) && (rng != NULL))
        err = wc_ecc_gen_z(rng, bytes, R[0], modulus, mp, R[TMP_IDX]->x,
                           R[TMP_IDX]->y, kt);
    if ((err == MP_OKAY) && (rng != NULL))
        err = wc_ecc_gen_z(rng, bytes, R[1], modulus, mp, R[TMP_IDX]->x,
                           R[TMP_IDX]->y, kt);

    if (err == MP_OKAY) {
        /* Order could be one greater than the size of the modulus. */
        t = mp_count_bits(modulus) + 1;
        err = mp_copy(k, kt);
    }
    if (err == MP_OKAY) {
        err = mp_grow(kt, modulus->used + 1);
    }
    /* Step 2: for j = 1 to t-1 do */
    for (i = 1, j = 0, cnt = 0; (err == MP_OKAY) && (i < t); i++) {
        if (++cnt == DIGIT_BIT) {
            j++;
            cnt = 0;
        }

        /* Step 3: b = 1 - k[j]; R[b] = 2*R[b] + R[k[j]] */
        /* Swap R[0] and R[1] if other index is needed. */
        /* Ensure 'swap' changes when shifted word is 0. */
        swap += (kt->dp[j] >> cnt) + 2;
        ecc_cond_swap_into_ct(R[(2 - set) + 0], R[(2 - set) + 1],
                              R[set + 0], R[set + 1], modulus->used, swap);
        /* Change to operate on set copied into. */
        set = 2 - set;
        /* Ensure 'swap' changes to a previously unseen value. */
        swap += (kt->dp[j] >> cnt) + swap;

        /* R[0] = 2*R[0] */
        err = ecc_projective_dbl_point_safe(R[set + 0], R[set + 0], a, modulus,
                                            mp);
        if (err == MP_OKAY) {
            /* R[0] = R[1] + R[0] */
            err = ecc_projective_add_point_safe(R[set + 0], R[set + 1],
                                         R[set + 0], a, modulus, mp, &infinity);
        }
        /*  R[1]->z * 2 - same point. */
        mp_addmod_ct(R[set + 1]->z, R[set + 1]->z, modulus, R[set + 1]->z);
        mp_addmod_ct(R[set + 1]->x, R[set + 1]->x, modulus, R[set + 1]->x);
        mp_addmod_ct(R[set + 1]->x, R[set + 1]->x, modulus, R[set + 1]->x);
        mp_addmod_ct(R[set + 1]->y, R[set + 1]->y, modulus, R[set + 1]->y);
        mp_addmod_ct(R[set + 1]->y, R[set + 1]->y, modulus, R[set + 1]->y);
        mp_addmod_ct(R[set + 1]->y, R[set + 1]->y, modulus, R[set + 1]->y);
    }
    /* Step 4: end for */
    /* Swap back if last bit is 0. */
    /* Ensure 'swap' changes. */
    swap += 1;
    if (err == MP_OKAY) {
        ecc_cond_swap_into_ct(R[(2 - set) + 0], R[(2 - set) + 1],
                              R[set + 0], R[set + 1], modulus->used, swap);
        set = 2 - set;
    }

    /* Step 5: b = k[0]; R[b] = R[b] - P */
    /* R[TMP_IDX] = -P */
    if (err == MP_OKAY)
        err = mp_copy(P->x, R[TMP_IDX]->x);
    if (err == MP_OKAY)
        err = mp_sub(modulus, P->y, R[TMP_IDX]->y);
    if (err == MP_OKAY)
        err = mp_copy(P->z, R[TMP_IDX]->z);
    /* Subtract point by adding negative. */
    if (err == MP_OKAY) {
        /* Swap R[0] and R[1], if necessary, to operate on the one we want.
         * Last bit of k->dp[0] is being used to make decision to swap.
         */
        ecc_cond_swap_into_ct(R[(2 - set) + 0], R[(2 - set) + 1],
                              R[set + 0], R[set + 1], modulus->used,
                              (int)k->dp[0]);
        set = 2 - set;
        err = ecc_projective_add_point_safe(R[set + 0], R[TMP_IDX], R[set + 0],
                                            a, modulus, mp, &infinity);
        /* Swap back if necessary. */
        if (err == MP_OKAY) {
            ecc_cond_swap_into_ct(R[(2 - set) + 0], R[(2 - set) + 1],
                                  R[set + 0], R[set + 1], modulus->used,
                                  (int)k->dp[0]);
            set = 2 - set;
        }
    }

    /* Step 6: return R[0] */
    if (err == MP_OKAY)
        err = mp_copy(R[set + 0]->x, Q->x);
    if (err == MP_OKAY)
        err = mp_copy(R[set + 0]->y, Q->y);
    if (err == MP_OKAY)
        err = mp_copy(R[set + 0]->z, Q->z);

    return err;
}

#endif

#endif

/* Convert the point to montgomery form.
 *
 * @param  [in]   p        Point to convert.
 * @param  [out]  r        Point in montgomery form.
 * @param  [in]   modulus  Modulus of ordinates.
 * @return  0 on success.
 * @return  -ve on failure.
 */
static int ecc_point_to_mont(ecc_point* p, ecc_point* r, mp_int* modulus,
                             void* heap)
{
   int err = MP_OKAY;
   DECL_MP_INT_SIZE_DYN(mu, mp_bitsused(modulus), MAX_ECC_BITS_USE);

   (void)heap;

   NEW_MP_INT_SIZE(mu, mp_bitsused(modulus), heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
   if (mu == NULL)
       err = MEMORY_E;
#endif
   if (err == MP_OKAY)
       err = INIT_MP_INT_SIZE(mu, mp_bitsused(modulus));
   if (err == MP_OKAY) {
       err = mp_montgomery_calc_normalization(mu, modulus);

       if (err == MP_OKAY) {
           if (mp_cmp_d(mu, 1) == MP_EQ) {
               err = mp_copy(p->x, r->x);
               if (err == MP_OKAY)
                   err = mp_copy(p->y, r->y);
               if (err == MP_OKAY)
                   err = mp_copy(p->z, r->z);
           }
           else {
               err = mp_mulmod(p->x, mu, modulus, r->x);
               if (err == MP_OKAY)
                   err = mp_mulmod(p->y, mu, modulus, r->y);
               if (err == MP_OKAY)
                   err = mp_mulmod(p->z, mu, modulus, r->z);
           }
       }

       mp_clear(mu);
   }

   FREE_MP_INT_SIZE(mu, heap, DYNAMIC_TYPE_ECC);
   return err;
}

#ifdef WOLFSSL_SMALL_STACK_CACHE
static int ecc_key_tmp_init(ecc_key* key, void* heap)
{
   int err = MP_OKAY;

   (void)heap;

   if (key == NULL) {
       return ECC_BAD_ARG_E;
   }

   XMEMSET(key, 0, sizeof(*key));

#if defined(WOLFSSL_SP_MATH_ALL) && defined(WOLFSSL_SMALL_STACK)
   NEW_MP_INT_SIZE(key->t1, ECC_KEY_MAX_BITS(key), heap, DYNAMIC_TYPE_ECC);
   NEW_MP_INT_SIZE(key->t2, ECC_KEY_MAX_BITS(key), heap, DYNAMIC_TYPE_ECC);
#ifdef ALT_ECC_SIZE
   NEW_MP_INT_SIZE(key->x, ECC_KEY_MAX_BITS(key), heap, DYNAMIC_TYPE_ECC);
   NEW_MP_INT_SIZE(key->y, ECC_KEY_MAX_BITS(key), heap, DYNAMIC_TYPE_ECC);
   NEW_MP_INT_SIZE(key->z, ECC_KEY_MAX_BITS(key), heap, DYNAMIC_TYPE_ECC);
#endif
   if (key->t1 == NULL || key->t2 == NULL
#ifdef ALT_ECC_SIZE
      || key->x == NULL || key->y == NULL || key->z == NULL
#endif
   ) {
       err = MEMORY_E;
   }
   if (err == 0) {
       err = INIT_MP_INT_SIZE(key->t1, ECC_KEY_MAX_BITS(key));
   }
   if (err == 0) {
       err = INIT_MP_INT_SIZE(key->t2, ECC_KEY_MAX_BITS(key));
   }
#ifdef ALT_ECC_SIZE
   if (err == 0) {
       err = INIT_MP_INT_SIZE(key->x, ECC_KEY_MAX_BITS(key));
   }
   if (err == 0) {
       err = INIT_MP_INT_SIZE(key->y, ECC_KEY_MAX_BITS(key));
   }
   if (err == 0) {
       err = INIT_MP_INT_SIZE(key->z, ECC_KEY_MAX_BITS(key));
   }
#endif
#else
   key->t1 = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
   key->t2 = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
#ifdef ALT_ECC_SIZE
   key->x = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
   key->y = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
   key->z = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
#endif
   if (key->t1 == NULL || key->t2 == NULL
#ifdef ALT_ECC_SIZE
      || key->x == NULL || key->y == NULL || key->z == NULL
#endif
   ) {
       err = MEMORY_E;
   }
#endif

   return err;
}

static void ecc_key_tmp_final(ecc_key* key, void* heap)
{
    (void)heap;

#if defined(WOLFSSL_SP_MATH_ALL) && defined(WOLFSSL_SMALL_STACK)
#ifdef ALT_ECC_SIZE
   FREE_MP_INT_SIZE(key->z, heap, DYNAMIC_TYPE_ECC);
   FREE_MP_INT_SIZE(key->y, heap, DYNAMIC_TYPE_ECC);
   FREE_MP_INT_SIZE(key->x, heap, DYNAMIC_TYPE_ECC);
#endif
   FREE_MP_INT_SIZE(key->t2, heap, DYNAMIC_TYPE_ECC);
   FREE_MP_INT_SIZE(key->t1, heap, DYNAMIC_TYPE_ECC);
#else
#ifdef ALT_ECC_SIZE
   if (key->z != NULL)
      XFREE(key->z, heap, DYNAMIC_TYPE_ECC);
   if (key->y != NULL)
      XFREE(key->y, heap, DYNAMIC_TYPE_ECC);
   if (key->x != NULL)
      XFREE(key->x, heap, DYNAMIC_TYPE_ECC);
#endif
   if (key->t2 != NULL)
      XFREE(key->t2, heap, DYNAMIC_TYPE_ECC);
   if (key->t1 != NULL)
      XFREE(key->t1, heap, DYNAMIC_TYPE_ECC);
#endif
}
#endif /* WOLFSSL_SMALL_STACK_CACHE */
#endif /* !WOLFSSL_SP_MATH */

#if !defined(WOLFSSL_SP_MATH) || !defined(FP_ECC)
/**
   Perform a point multiplication
   k    The scalar to multiply by
   G    The base point
   R    [out] Destination for kG
   a    ECC curve parameter a
   modulus  The modulus of the field the ECC curve is in
   map      Boolean whether to map back to affine or not
                (1==map, 0 == leave in projective)
   return MP_OKAY on success
*/
#ifdef FP_ECC
static int normal_ecc_mulmod(const mp_int* k, ecc_point *G, ecc_point *R,
                             mp_int* a, mp_int* modulus, WC_RNG* rng, int map,
                             void* heap)
#else
int wc_ecc_mulmod_ex(const mp_int* k, ecc_point *G, ecc_point *R, mp_int* a,
                     mp_int* modulus, int map, void* heap)
#endif
#if !defined(WOLFSSL_SP_MATH)
{
   ecc_point     *tG, *M[M_POINTS];
#ifdef WOLFSSL_NO_MALLOC
   ecc_point     lcl_tG, lcl_M[M_POINTS];
#endif
   int           i, err;
#ifdef WOLFSSL_SMALL_STACK_CACHE
   ecc_key       *key = (ecc_key *)XMALLOC(sizeof(*key), heap, DYNAMIC_TYPE_ECC);
#endif
   mp_digit      mp;

   /* init variables */
   tG = NULL;
   XMEMSET(M, 0, sizeof(M));

   if (k == NULL || G == NULL || R == NULL || modulus == NULL) {
       err = ECC_BAD_ARG_E;
       goto exit;
   }

   /* k can't have more bits than modulus count plus 1 */
   if (mp_count_bits(k) > mp_count_bits(modulus) + 1) {
       err = ECC_OUT_OF_RANGE_E;
       goto exit;
   }

#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (key == NULL) {
       err = MP_MEM;
       goto exit;
   }
   err = ecc_key_tmp_init(key, heap);
   if (err != MP_OKAY)
      goto exit;
   R->key = key;
#endif /* WOLFSSL_SMALL_STACK_CACHE */

  /* alloc ram for window temps */
  for (i = 0; i < M_POINTS; i++) {
  #ifdef WOLFSSL_NO_MALLOC
      M[i] = &lcl_M[i];
  #endif
      err = wc_ecc_new_point_ex(&M[i], heap);
      if (err != MP_OKAY) {
         goto exit;
      }
#ifdef WOLFSSL_SMALL_STACK_CACHE
      M[i]->key = key;
#endif
  }

   /* make a copy of G in case R==G */
#ifdef WOLFSSL_NO_MALLOC
   tG = &lcl_tG;
#endif
   err = wc_ecc_new_point_ex(&tG, heap);
   if (err != MP_OKAY) {
       goto exit;
   }
   if ((err = ecc_point_to_mont(G, tG, modulus, heap)) != MP_OKAY) {
       goto exit;
   }

   /* init montgomery reduction */
   if ((err = mp_montgomery_setup(modulus, &mp)) != MP_OKAY) {
       goto exit;
   }

#ifdef FP_ECC
   err = ecc_mulmod(k, tG, R, M, a, modulus, mp, rng);
#else
   err = ecc_mulmod(k, tG, R, M, a, modulus, mp, NULL);
#endif
   /* map R back from projective space */
   if (err == MP_OKAY && map)
       err = ecc_map(R, modulus, mp);

exit:

   /* done */
   wc_ecc_del_point_ex(tG, heap);
   for (i = 0; i < M_POINTS; i++) {
       wc_ecc_del_point_ex(M[i], heap);
   }

#ifdef WOLFSSL_SMALL_STACK_CACHE
   if (key) {
       if (R)
           R->key = NULL;
       if (err == MP_OKAY)
           ecc_key_tmp_final(key, heap);
       XFREE(key, heap, DYNAMIC_TYPE_ECC);
   }
#endif /* WOLFSSL_SMALL_STACK_CACHE */

   return err;
}
#else
{
   if (k == NULL || G == NULL || R == NULL || modulus == NULL) {
       return ECC_BAD_ARG_E;
   }

   (void)a;

   /* For supported curves the order is the same length in bits as the modulus.
    * Can't have more than order bits for the scalar.
    */
   if (mp_count_bits(k) > mp_count_bits(modulus)) {
       return ECC_OUT_OF_RANGE_E;
   }
   if (mp_count_bits(G->x) > mp_count_bits(modulus) ||
       mp_count_bits(G->y) > mp_count_bits(modulus) ||
       mp_count_bits(G->z) > mp_count_bits(modulus)) {
       return IS_POINT_E;
   }

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
   if (mp_count_bits(modulus) == 256) {
#ifdef SM2_SP_IMPL_AVAILABLE
   #ifdef WOLFSSL_SM2
       if (!mp_is_bit_set(modulus, 224)) {
           return sp_ecc_mulmod_sm2_256(k, G, R, map, heap);
       }
   #endif
#endif
       return sp_ecc_mulmod_256(k, G, R, map, heap);
   }
#endif
#ifdef WOLFSSL_SP_384
   if (mp_count_bits(modulus) == 384) {
       return sp_ecc_mulmod_384(k, G, R, map, heap);
   }
#endif
#ifdef WOLFSSL_SP_521
   if (mp_count_bits(modulus) == 521) {
       return sp_ecc_mulmod_521(k, G, R, map, heap);
   }
#endif
#else
   (void)map;
   (void)map;
   (void)heap;
#endif
   return ECC_BAD_ARG_E;
}
#endif
#endif /* !WOLFSSL_SP_MATH || !FP_ECC */

#ifndef FP_ECC
#if !defined(WOLFSSL_SP_MATH)
#ifdef ECC_TIMING_RESISTANT
static int ecc_check_order_minus_1(const mp_int* k, ecc_point* tG, ecc_point* R,
   mp_int* modulus, mp_int* order)
{
    int err;
    DECL_MP_INT_SIZE_DYN(t, mp_bitsused(order), MAX_ECC_BITS_USE);

    NEW_MP_INT_SIZE(t, mp_bitsused(modulus), NULL, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (t == NULL) {
        err = MEMORY_E;
    }
    else
#endif
    {
        err = INIT_MP_INT_SIZE(t, mp_bitsused(modulus));
    }
    if (err == MP_OKAY) {
        /* Check for k == order - 1. Result will be 0 point which is not correct
         * Calculates order / 2 and adds order / 2 + 1 and gets infinity.
         * (with constant time implementation)
         */
        err = mp_sub_d(order, 1, t);
        if (err == MP_OKAY) {
            int kIsMinusOne = (mp_cmp((mp_int*)k, t) == MP_EQ);
            err = mp_cond_copy(tG->x, kIsMinusOne, R->x);
            if (err == MP_OKAY) {
                err = mp_sub(modulus, tG->y, t);
            }
            if (err == MP_OKAY) {
                err = mp_cond_copy(t, kIsMinusOne, R->y);
            }
            if (err == MP_OKAY) {
                err = mp_cond_copy(tG->z, kIsMinusOne, R->z);
            }
        }

        mp_free(t);
    }

    FREE_MP_INT_SIZE(t, NULL, DYNAMIC_TYPE_ECC);
    return err;
}
#endif /* ECC_TIMING_RESISTANT */
#endif

/**
   Perform a point multiplication
   k    The scalar to multiply by
   G    The base point
   R    [out] Destination for kG
   a    ECC curve parameter a
   modulus  The modulus of the field the ECC curve is in
   map      Boolean whether to map back to affine or not
                (1==map, 0 == leave in projective)
   return MP_OKAY on success
*/
int wc_ecc_mulmod_ex2(const mp_int* k, ecc_point* G, ecc_point* R, mp_int* a,
                      mp_int* modulus, mp_int* order, WC_RNG* rng, int map,
                      void* heap)
#if !defined(WOLFSSL_SP_MATH)
{
   ecc_point     *tG, *M[M_POINTS];
#ifdef WOLFSSL_NO_MALLOC
   ecc_point     lcl_tG, lcl_M[M_POINTS];
#endif
   int           i, err;
#ifdef WOLFSSL_SMALL_STACK_CACHE
   ecc_key       key;
#endif
   mp_digit      mp;

   if (k == NULL || G == NULL || R == NULL || modulus == NULL) {
      return ECC_BAD_ARG_E;
   }

#ifdef HAVE_ECC_CDH
   if (mp_count_bits(modulus) > mp_count_bits(order)) {
      if (mp_count_bits(k) > mp_count_bits(modulus)) {
          return ECC_OUT_OF_RANGE_E;
      }
   }
   else
#endif
   /* k can't have more bits than order */
   if (mp_count_bits(k) > mp_count_bits(order)) {
      return ECC_OUT_OF_RANGE_E;
   }

   /* init variables */
   tG = NULL;
   XMEMSET(M, 0, sizeof(M));

#ifdef WOLFSSL_SMALL_STACK_CACHE
   err = ecc_key_tmp_init(&key, heap);
   if (err != MP_OKAY)
      goto exit;
   R->key = &key;
#endif /* WOLFSSL_SMALL_STACK_CACHE */

   /* alloc ram for window temps */
   for (i = 0; i < M_POINTS; i++) {
   #ifdef WOLFSSL_NO_MALLOC
      M[i] = &lcl_M[i];
   #endif
      err = wc_ecc_new_point_ex(&M[i], heap);
      if (err != MP_OKAY) {
         goto exit;
      }
#ifdef WOLFSSL_SMALL_STACK_CACHE
      M[i]->key = &key;
#endif
  }

   /* make a copy of G in case R==G */
#ifdef WOLFSSL_NO_MALLOC
   tG = &lcl_tG;
#endif
   err = wc_ecc_new_point_ex(&tG, heap);
   if (err != MP_OKAY) {
       goto exit;
   }
   if ((err = ecc_point_to_mont(G, tG, modulus, heap)) != MP_OKAY) {
       goto exit;
   }

   /* init montgomery reduction */
   if ((err = mp_montgomery_setup(modulus, &mp)) != MP_OKAY) {
      goto exit;
   }

   err = ecc_mulmod(k, tG, R, M, a, modulus, mp, rng);
#ifdef ECC_TIMING_RESISTANT
   if (err == MP_OKAY) {
       err = ecc_check_order_minus_1(k, tG, R, modulus, order);
   }
#else
   (void)order;
#endif
   /* map R back from projective space */
   if (err == MP_OKAY && map)
      err = ecc_map(R, modulus, mp);

exit:

   /* done */
   wc_ecc_del_point_ex(tG, heap);
   for (i = 0; i < M_POINTS; i++) {
      wc_ecc_del_point_ex(M[i], heap);
   }
#ifdef WOLFSSL_SMALL_STACK_CACHE
   R->key = NULL;
   ecc_key_tmp_final(&key, heap);
#endif /* WOLFSSL_SMALL_STACK_CACHE */

   return err;
}
#else
{
   if (k == NULL || G == NULL || R == NULL || modulus == NULL) {
       return ECC_BAD_ARG_E;
   }
   if (mp_count_bits(G->x) > mp_count_bits(modulus) ||
       mp_count_bits(G->y) > mp_count_bits(modulus) ||
       mp_count_bits(G->z) > mp_count_bits(modulus)) {
       return IS_POINT_E;
   }

   (void)a;
   (void)order;
   (void)rng;

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
   if (mp_count_bits(modulus) == 256) {
       return sp_ecc_mulmod_256(k, G, R, map, heap);
   }
#endif
#ifdef WOLFSSL_SP_384
   if (mp_count_bits(modulus) == 384) {
       return sp_ecc_mulmod_384(k, G, R, map, heap);
   }
#endif
#ifdef WOLFSSL_SP_521
   if (mp_count_bits(modulus) == 521) {
       return sp_ecc_mulmod_521(k, G, R, map, heap);
   }
#endif
#else
   (void)map;
   (void)heap;
#endif
   return ECC_BAD_ARG_E;
}
#endif /* !WOLFSSL_SP_MATH */
#endif /* !FP_ECC */

#endif /* !FREESCALE_LTC_ECC && !WOLFSSL_STM32_PKA */

/** ECC Fixed Point mulmod global
    k        The multiplicand
    G        Base point to multiply
    R        [out] Destination of product
    a        ECC curve parameter a
    modulus  The modulus for the curve
    map      [boolean] If non-zero maps the point back to affine coordinates,
             otherwise it's left in jacobian-montgomery form
    return MP_OKAY if successful
*/
int wc_ecc_mulmod(const mp_int* k, ecc_point *G, ecc_point *R, mp_int* a,
                  mp_int* modulus, int map)
{
    return wc_ecc_mulmod_ex(k, G, R, a, modulus, map, NULL);
}

#endif /* !WOLFSSL_ATECC508A */

/**
 * Allocate a new ECC point (if one not provided)
 * use a heap hint when creating new ecc_point
 * return an allocated point on success or NULL on failure
*/
static int wc_ecc_new_point_ex(ecc_point** point, void* heap)
{
   int err = MP_OKAY;
   ecc_point* p;

   if (point == NULL) {
       return BAD_FUNC_ARG;
   }

   p = *point;
#ifndef WOLFSSL_NO_MALLOC
   if (p == NULL) {
      p = (ecc_point*)XMALLOC(sizeof(ecc_point), heap, DYNAMIC_TYPE_ECC);
   }
#endif
   if (p == NULL) {
      return MEMORY_E;
   }
   XMEMSET(p, 0, sizeof(ecc_point));

#ifndef ALT_ECC_SIZE
   err = mp_init_multi(p->x, p->y, p->z, NULL, NULL, NULL);
   if (err != MP_OKAY) {
   #ifndef WOLFSSL_NO_MALLOC
      XFREE(p, heap, DYNAMIC_TYPE_ECC);
   #endif
      return err;
   }
#else
   p->x = (mp_int*)&p->xyz[0];
   p->y = (mp_int*)&p->xyz[1];
   p->z = (mp_int*)&p->xyz[2];
   alt_fp_init(p->x);
   alt_fp_init(p->y);
   alt_fp_init(p->z);
#endif

   *point = p;
   (void)heap;
   return err;
}
ecc_point* wc_ecc_new_point_h(void* heap)
{
    ecc_point* p = NULL;
    (void)wc_ecc_new_point_ex(&p, heap);
    return p;
}
ecc_point* wc_ecc_new_point(void)
{
   ecc_point* p = NULL;
   (void)wc_ecc_new_point_ex(&p, NULL);
   return p;
}

/** Free an ECC point from memory
  p   The point to free
*/
static void wc_ecc_del_point_ex(ecc_point* p, void* heap)
{
   if (p != NULL) {
      mp_clear(p->x);
      mp_clear(p->y);
      mp_clear(p->z);
   #ifndef WOLFSSL_NO_MALLOC
      XFREE(p, heap, DYNAMIC_TYPE_ECC);
   #endif
   }
   (void)heap;
}
void wc_ecc_del_point_h(ecc_point* p, void* heap)
{
   wc_ecc_del_point_ex(p, heap);
}
void wc_ecc_del_point(ecc_point* p)
{
    wc_ecc_del_point_ex(p, NULL);
}

void wc_ecc_forcezero_point(ecc_point* p)
{
    if (p != NULL) {
        mp_forcezero(p->x);
        mp_forcezero(p->y);
        mp_forcezero(p->z);
    }
}


/** Copy the value of a point to an other one
  p    The point to copy
  r    The created point
*/
int wc_ecc_copy_point(const ecc_point* p, ecc_point *r)
{
    int ret;

    /* prevents null arguments */
    if (p == NULL || r == NULL)
        return ECC_BAD_ARG_E;

    ret = mp_copy(p->x, r->x);
    if (ret != MP_OKAY)
        return ret;
    ret = mp_copy(p->y, r->y);
    if (ret != MP_OKAY)
        return ret;
    ret = mp_copy(p->z, r->z);
    if (ret != MP_OKAY)
        return ret;

    return MP_OKAY;
}

/** Compare the value of a point with an other one
 a    The point to compare
 b    The other point to compare

 return MP_EQ if equal, MP_LT/MP_GT if not, < 0 in case of error
 */
int wc_ecc_cmp_point(ecc_point* a, ecc_point *b)
{
    int ret;

    /* prevents null arguments */
    if (a == NULL || b == NULL)
        return BAD_FUNC_ARG;

    ret = mp_cmp(a->x, b->x);
    if (ret != MP_EQ)
        return ret;
    ret = mp_cmp(a->y, b->y);
    if (ret != MP_EQ)
        return ret;
    ret = mp_cmp(a->z, b->z);
    if (ret != MP_EQ)
        return ret;

    return MP_EQ;
}


/** Returns whether an ECC idx is valid or not
  n      The idx number to check
  return 1 if valid, 0 if not
*/
int wc_ecc_is_valid_idx(int n)
{
   int x;

   if (n >= (int)ECC_SET_COUNT)
       return 0;

   for (x = 0; ecc_sets[x].size != 0; x++)
       ;
   /* -1 is a valid index --- indicating that the domain params
      were supplied by the user */
   if ((n >= ECC_CUSTOM_IDX) && (n < x)) {
      return 1;
   }

   return 0;
}

int wc_ecc_get_curve_idx(int curve_id)
{
    int curve_idx;
    for (curve_idx = 0; ecc_sets[curve_idx].size != 0; curve_idx++) {
        if (curve_id == ecc_sets[curve_idx].id)
            break;
    }
    if (ecc_sets[curve_idx].size == 0) {
        return ECC_CURVE_INVALID;
    }
    return curve_idx;
}

int wc_ecc_get_curve_id(int curve_idx)
{
    if (wc_ecc_is_valid_idx(curve_idx)) {
        return ecc_sets[curve_idx].id;
    }
    return ECC_CURVE_INVALID;
}

/* Returns the curve size that corresponds to a given ecc_curve_id identifier
 *
 * id      curve id, from ecc_curve_id enum in ecc.h
 * return  curve size, from ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_size_from_id(int curve_id)
{
    int curve_idx = wc_ecc_get_curve_idx(curve_id);
    if (curve_idx == ECC_CURVE_INVALID)
        return ECC_BAD_ARG_E;
    return ecc_sets[curve_idx].size;
}

/* Returns the curve index that corresponds to a given curve name in
 * ecc_sets[] of ecc.c
 *
 * name    curve name, from ecc_sets[].name in ecc.c
 * return  curve index in ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_idx_from_name(const char* curveName)
{
    int curve_idx;

    if (curveName == NULL)
        return BAD_FUNC_ARG;

    for (curve_idx = 0; ecc_sets[curve_idx].size != 0; curve_idx++) {
        if (
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            ecc_sets[curve_idx].name &&
        #endif
                XSTRCASECMP(ecc_sets[curve_idx].name, curveName) == 0) {
            break;
        }
    }
    if (ecc_sets[curve_idx].size == 0) {
        WOLFSSL_MSG("ecc_set curve name not found");
        return ECC_CURVE_INVALID;
    }
    return curve_idx;
}

/* Returns the curve size that corresponds to a given curve name,
 * as listed in ecc_sets[] of ecc.c.
 *
 * name    curve name, from ecc_sets[].name in ecc.c
 * return  curve size, from ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_size_from_name(const char* curveName)
{
    int curve_idx;

    if (curveName == NULL)
        return BAD_FUNC_ARG;

    curve_idx = wc_ecc_get_curve_idx_from_name(curveName);
    if (curve_idx < 0)
        return curve_idx;

    return ecc_sets[curve_idx].size;
}

/* Returns the curve id that corresponds to a given curve name,
 * as listed in ecc_sets[] of ecc.c.
 *
 * name   curve name, from ecc_sets[].name in ecc.c
 * return curve id, from ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_id_from_name(const char* curveName)
{
    int curve_idx;

    if (curveName == NULL)
        return BAD_FUNC_ARG;

    curve_idx = wc_ecc_get_curve_idx_from_name(curveName);
    if (curve_idx < 0)
        return curve_idx;

    return ecc_sets[curve_idx].id;
}

/* Compares a curve parameter (hex, from ecc_sets[]) to given input
 * parameter for equality.
 * encType is WC_TYPE_UNSIGNED_BIN or WC_TYPE_HEX_STR
 * Returns MP_EQ on success, negative on error */
static int wc_ecc_cmp_param(const char* curveParam,
                            const byte* param, word32 paramSz, int encType)
{
    int err = MP_OKAY;
#ifdef WOLFSSL_SMALL_STACK
    mp_int* a = NULL;
    mp_int* b = NULL;
#else
    mp_int  a[1], b[1];
#endif

    if (param == NULL || curveParam == NULL)
        return BAD_FUNC_ARG;

    if (encType == WC_TYPE_HEX_STR)
        return XSTRNCMP(curveParam, (char*) param, paramSz);

#ifdef WOLFSSL_SMALL_STACK
    a = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
    if (a == NULL)
        return MEMORY_E;
    b = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
    if (b == NULL) {
        XFREE(a, NULL, DYNAMIC_TYPE_ECC);
        return MEMORY_E;
    }
#endif

    if ((err = mp_init_multi(a, b, NULL, NULL, NULL, NULL)) != MP_OKAY) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(a, NULL, DYNAMIC_TYPE_ECC);
        XFREE(b, NULL, DYNAMIC_TYPE_ECC);
    #endif
        return err;
    }

    if (err == MP_OKAY) {
        err = mp_read_unsigned_bin(a, param, paramSz);
    }
    if (err == MP_OKAY)
        err = mp_read_radix(b, curveParam, MP_RADIX_HEX);

    if (err == MP_OKAY) {
        if (mp_cmp(a, b) != MP_EQ) {
            err = -1;
        } else {
            err = MP_EQ;
        }
    }

    mp_clear(a);
    mp_clear(b);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(b, NULL, DYNAMIC_TYPE_ECC);
    XFREE(a, NULL, DYNAMIC_TYPE_ECC);
#endif

    return err;
}

/* Returns the curve id in ecc_sets[] that corresponds to a given set of
 * curve parameters.
 *
 * fieldSize  the field size in bits
 * prime      prime of the finite field
 * primeSz    size of prime in octets
 * Af         first coefficient a of the curve
 * AfSz       size of Af in octets
 * Bf         second coefficient b of the curve
 * BfSz       size of Bf in octets
 * order      curve order
 * orderSz    size of curve in octets
 * Gx         affine x coordinate of base point
 * GxSz       size of Gx in octets
 * Gy         affine y coordinate of base point
 * GySz       size of Gy in octets
 * cofactor   curve cofactor
 *
 * return curve id, from ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_id_from_params(int fieldSize,
        const byte* prime, word32 primeSz, const byte* Af, word32 AfSz,
        const byte* Bf, word32 BfSz, const byte* order, word32 orderSz,
        const byte* Gx, word32 GxSz, const byte* Gy, word32 GySz, int cofactor)
{
    int idx;
    int curveSz;

    if (prime == NULL || Af == NULL || Bf == NULL || order == NULL ||
        Gx == NULL || Gy == NULL)
        return BAD_FUNC_ARG;

    curveSz = (fieldSize + 1) / 8;    /* round up */

    for (idx = 0; ecc_sets[idx].size != 0; idx++) {
        if (curveSz == ecc_sets[idx].size) {
            if ((wc_ecc_cmp_param(ecc_sets[idx].prime, prime,
                            primeSz, WC_TYPE_UNSIGNED_BIN) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Af, Af, AfSz,
                                  WC_TYPE_UNSIGNED_BIN) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Bf, Bf, BfSz,
                                  WC_TYPE_UNSIGNED_BIN) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].order, order,
                                  orderSz, WC_TYPE_UNSIGNED_BIN) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Gx, Gx, GxSz,
                                  WC_TYPE_UNSIGNED_BIN) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Gy, Gy, GySz,
                                  WC_TYPE_UNSIGNED_BIN) == MP_EQ) &&
                (cofactor == ecc_sets[idx].cofactor)) {
                    break;
            }
        }
    }

    if (ecc_sets[idx].size == 0)
        return ECC_CURVE_INVALID;

    return ecc_sets[idx].id;
}

/* Returns the curve id in ecc_sets[] that corresponds
 * to a given domain parameters pointer.
 *
 * dp   domain parameters pointer
 *
 * return curve id, from ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_id_from_dp_params(const ecc_set_type* dp)
{
    int idx;

    if (dp == NULL
    #ifndef WOLFSSL_ECC_CURVE_STATIC
         || dp->prime == NULL ||  dp->Af == NULL ||
        dp->Bf == NULL || dp->order == NULL || dp->Gx == NULL || dp->Gy == NULL
    #endif
    ) {
        return BAD_FUNC_ARG;
    }

    for (idx = 0; ecc_sets[idx].size != 0; idx++) {
        if (dp->size == ecc_sets[idx].size) {
            if ((wc_ecc_cmp_param(ecc_sets[idx].prime, (const byte*)dp->prime,
                    (word32)XSTRLEN(dp->prime), WC_TYPE_HEX_STR) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Af, (const byte*)dp->Af,
                    (word32)XSTRLEN(dp->Af),WC_TYPE_HEX_STR) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Bf, (const byte*)dp->Bf,
                    (word32)XSTRLEN(dp->Bf),WC_TYPE_HEX_STR) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].order, (const byte*)dp->order,
                    (word32)XSTRLEN(dp->order),WC_TYPE_HEX_STR) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Gx, (const byte*)dp->Gx,
                    (word32)XSTRLEN(dp->Gx),WC_TYPE_HEX_STR) == MP_EQ) &&
                (wc_ecc_cmp_param(ecc_sets[idx].Gy, (const byte*)dp->Gy,
                    (word32)XSTRLEN(dp->Gy),WC_TYPE_HEX_STR) == MP_EQ) &&
                (dp->cofactor == ecc_sets[idx].cofactor)) {
                    break;
            }
        }
    }

    if (ecc_sets[idx].size == 0)
        return ECC_CURVE_INVALID;

    return ecc_sets[idx].id;
}

/* Returns the curve id that corresponds to a given OID,
 * as listed in ecc_sets[] of ecc.c.
 *
 * oid   OID, from ecc_sets[].name in ecc.c
 * len   OID len, from ecc_sets[].name in ecc.c
 * return curve id, from ecc_sets[] on success, negative on error
 */
int wc_ecc_get_curve_id_from_oid(const byte* oid, word32 len)
{
    int curve_idx;
#if defined(HAVE_OID_DECODING) || defined(HAVE_OID_ENCODING)
    int ret;
    #ifdef HAVE_OID_DECODING
    word16 decOid[MAX_OID_SZ/sizeof(word16)];
    #else
    byte  decOid[MAX_OID_SZ];
    #endif
    word32 decOidSz;
#endif

    if (oid == NULL)
        return BAD_FUNC_ARG;

#ifdef HAVE_OID_DECODING
    decOidSz = (word32)sizeof(decOid);
    ret = DecodeObjectId(oid, len, decOid, &decOidSz);
    if (ret != 0) {
        return ret;
    }
#endif

#if !defined(HAVE_OID_ENCODING) && !defined(HAVE_OID_DECODING)
    if (len == 0) {
        /* SAKKE has zero oidSz and will otherwise match with len==0. */
        WOLFSSL_MSG("zero oidSz");
        return ECC_CURVE_INVALID;
    }
#endif

    for (curve_idx = 0; ecc_sets[curve_idx].size != 0; curve_idx++) {
    #if defined(HAVE_OID_ENCODING) && !defined(HAVE_OID_DECODING)
        decOidSz = (word32)sizeof(decOid);
        ret = EncodeObjectId(ecc_sets[curve_idx].oid, ecc_sets[curve_idx].oidSz,
            decOid, &decOidSz);
        if (ret != 0) {
            continue;
        }
    #endif

        if (
        #ifndef WOLFSSL_ECC_CURVE_STATIC
            ecc_sets[curve_idx].oid &&
        #endif
        #if defined(HAVE_OID_ENCODING) && !defined(HAVE_OID_DECODING)
            decOidSz == len &&
                XMEMCMP(decOid, oid, len) == 0
        #elif defined(HAVE_OID_ENCODING) && defined(HAVE_OID_DECODING)
            /* We double because decOidSz is a count of word16 elements. */
            ecc_sets[curve_idx].oidSz == decOidSz &&
                XMEMCMP(ecc_sets[curve_idx].oid, decOid, decOidSz * 2) == 0
        #else
            ecc_sets[curve_idx].oidSz == len &&
                XMEMCMP(ecc_sets[curve_idx].oid, oid, len) == 0
        #endif
        ) {
            break;
        }
    }
    if (ecc_sets[curve_idx].size == 0) {
        WOLFSSL_MSG("ecc_set curve name not found");
        return ECC_CURVE_INVALID;
    }

    return ecc_sets[curve_idx].id;
}

/* Get curve parameters using curve index */
const ecc_set_type* wc_ecc_get_curve_params(int curve_idx)
{
    const ecc_set_type* ecc_set = NULL;

    if (curve_idx >= 0 && curve_idx < (int)ECC_SET_COUNT) {
        ecc_set = &ecc_sets[curve_idx];
    }
    return ecc_set;
}


#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
static WC_INLINE int wc_ecc_alloc_mpint(ecc_key* key, mp_int** mp)
{
   if (key == NULL || mp == NULL)
      return BAD_FUNC_ARG;
   if (*mp == NULL) {
      *mp = (mp_int*)XMALLOC(sizeof(mp_int), key->heap, DYNAMIC_TYPE_BIGINT);
      if (*mp == NULL) {
         return MEMORY_E;
      }
      XMEMSET(*mp, 0, sizeof(mp_int));
   }
   return 0;
}
static WC_INLINE void wc_ecc_free_mpint(ecc_key* key, mp_int** mp)
{
   if (key && mp && *mp) {
      mp_clear(*mp);
      XFREE(*mp, key->heap, DYNAMIC_TYPE_BIGINT);
      *mp = NULL;
   }
}

static int wc_ecc_alloc_async(ecc_key* key)
{
    int err = wc_ecc_alloc_mpint(key, &key->r);
    if (err == 0)
        err = wc_ecc_alloc_mpint(key, &key->s);
    return err;
}

static void wc_ecc_free_async(ecc_key* key)
{
    wc_ecc_free_mpint(key, &key->r);
    wc_ecc_free_mpint(key, &key->s);
#ifdef HAVE_CAVIUM_V
    wc_ecc_free_mpint(key, &key->e);
    wc_ecc_free_mpint(key, &key->signK);
#endif /* HAVE_CAVIUM_V */
}
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_ECC */


#ifdef HAVE_ECC_DHE
/**
  Create an ECC shared secret between two keys
  private_key      The private ECC key (heap hint based off of private key)
  public_key       The public key
  out              [out] Destination of the shared secret
                         Conforms to EC-DH from ANSI X9.63
  outlen           [in/out] The max size and resulting size of the shared secret
  return           MP_OKAY if successful
*/
WOLFSSL_ABI
int wc_ecc_shared_secret(ecc_key* private_key, ecc_key* public_key, byte* out,
                      word32* outlen)
{
   int err = 0;

#if defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_ATECC508A) && \
   !defined(WOLFSSL_ATECC608A)
   CRYS_ECDH_TempData_t tempBuff;
#endif

   (void)err;

   if (private_key == NULL || public_key == NULL || out == NULL ||
                                                            outlen == NULL) {
       return BAD_FUNC_ARG;
   }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (private_key->devId != INVALID_DEVID)
    #endif
    {
        err = wc_CryptoCb_Ecdh(private_key, public_key, out, outlen);
        #ifndef WOLF_CRYPTO_CB_ONLY_ECC
        if (err != CRYPTOCB_UNAVAILABLE)
            return err;
        /* fall-through when unavailable */
        #endif
        #ifdef WOLF_CRYPTO_CB_ONLY_ECC
        if (err == CRYPTOCB_UNAVAILABLE) {
            err = NO_VALID_DEVID;
        }
        #endif
    }
#endif

#ifndef WOLF_CRYPTO_CB_ONLY_ECC
   /* type valid? */
   if (private_key->type != ECC_PRIVATEKEY &&
           private_key->type != ECC_PRIVATEKEY_ONLY) {
      return ECC_BAD_ARG_E;
   }

   /* Verify domain params supplied */
   if (wc_ecc_is_valid_idx(private_key->idx) == 0 || private_key->dp == NULL ||
       wc_ecc_is_valid_idx(public_key->idx)  == 0 || public_key->dp == NULL) {
      return ECC_BAD_ARG_E;
   }

   /* Verify curve id matches */
   if (private_key->dp->id != public_key->dp->id) {
      return ECC_BAD_ARG_E;
   }

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
   /* For SECP256R1 use hardware */
   if (private_key->dp->id == ECC_SECP256R1) {
       err = atmel_ecc_create_pms(private_key->slot, public_key->pubkey_raw, out);
       *outlen = private_key->dp->size;
   }
   else {
      err = NOT_COMPILED_IN;
   }
#elif defined(WOLFSSL_CRYPTOCELL)

    /* generate a secret*/
    err = CRYS_ECDH_SVDP_DH(&public_key->ctx.pubKey,
                            &private_key->ctx.privKey,
                            out,
                            (uint32_t*)outlen,
                            &tempBuff);

    if (err != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_ECDH_SVDP_DH for secret failed");
        return err;
    }
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
   err = silabs_ecc_shared_secret(private_key, public_key, out, outlen);
#elif defined(WOLFSSL_KCAPI_ECC)
   err = KcapiEcc_SharedSecret(private_key, public_key, out, outlen);
#elif defined(WOLFSSL_SE050)
   err = se050_ecc_shared_secret(private_key, public_key, out, outlen);
#else
   err = wc_ecc_shared_secret_ex(private_key, &public_key->pubkey, out, outlen);
#endif /* WOLFSSL_ATECC508A */
#endif /* WOLF_CRYPTO_CB_ONLY_ECC */

   return err;
}


#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_KCAPI_ECC) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC)

int wc_ecc_shared_secret_gen_sync(ecc_key* private_key, ecc_point* point,
                               byte* out, word32* outlen)
{
    int err = MP_OKAY;
    mp_int* k = private_key->k;
#ifdef HAVE_ECC_CDH
#ifdef WOLFSSL_SMALL_STACK
    mp_int *k_lcl = NULL;
#else
    mp_int k_lcl[1];
#endif
#endif
#if defined(WOLFSSL_HAVE_SP_ECC) && defined(WC_ECC_NONBLOCK) && \
    defined(WC_ECC_NONBLOCK_ONLY)
    ecc_nb_ctx_t nb_ctx;
    XMEMSET(&nb_ctx, 0, sizeof(nb_ctx));
#endif /* WOLFSSL_HAVE_SP_ECC && WC_ECC_NONBLOCK && WC_ECC_NONBLOCK_ONLY */

#ifdef HAVE_ECC_CDH
    /* if cofactor flag has been set */
    if (private_key->flags & WC_ECC_FLAG_COFACTOR) {
        mp_digit cofactor = (mp_digit)private_key->dp->cofactor;
        /* only perform cofactor calc if not equal to 1 */
        if (cofactor != 1) {
#ifdef WOLFSSL_SMALL_STACK
            if ((k_lcl = (mp_int *)XMALLOC(sizeof(*k_lcl), private_key->heap, DYNAMIC_TYPE_ECC_BUFFER)) == NULL)
                return MEMORY_E;
#endif
            k = k_lcl;
            if (mp_init(k) != MP_OKAY) {
                err = MEMORY_E;
                goto errout;
            }
            /* multiply cofactor times private key "k" */
            err = mp_mul_d(private_key->k, cofactor, k);
            if (err != MP_OKAY)
                goto errout;
        }
    }
#endif

#ifdef WOLFSSL_HAVE_SP_ECC

#ifndef WOLFSSL_SP_NO_256
    if (private_key->idx != ECC_CUSTOM_IDX &&
        ecc_sets[private_key->idx].id == ECC_SECP256R1) {
    #ifndef WC_ECC_NONBLOCK
        err = sp_ecc_secret_gen_256(k, point, out, outlen, private_key->heap);
    #else
        if (private_key->nb_ctx) {
            err = sp_ecc_secret_gen_256_nb(&private_key->nb_ctx->sp_ctx, k,
                                           point, out, outlen,
                                           private_key->heap);
        }
        else {
        #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_secret_gen_256_nb(&nb_ctx.sp_ctx, k, point, out,
                                               outlen, private_key->heap);
            } while (err == FP_WOULDBLOCK);
        #else
            err = sp_ecc_secret_gen_256(k, point, out, outlen,
                                        private_key->heap);
        #endif /* WC_ECC_NONBLOCK_ONLY */
        }
    #endif /* !WC_ECC_NONBLOCK */
    }
    else
#ifdef SM2_SP_IMPL_AVAILABLE
#ifdef WOLFSSL_SM2
    if (private_key->idx != ECC_CUSTOM_IDX &&
                               ecc_sets[private_key->idx].id == ECC_SM2P256V1) {
        err = sp_ecc_secret_gen_sm2_256(k, point, out, outlen,
                                                             private_key->heap);
    }
    else
#endif
#endif
#endif /* ! WOLFSSL_SP_NO_256 */
#ifdef WOLFSSL_SP_384
    if (private_key->idx != ECC_CUSTOM_IDX &&
        ecc_sets[private_key->idx].id == ECC_SECP384R1) {
    #ifndef WC_ECC_NONBLOCK
        err = sp_ecc_secret_gen_384(k, point, out, outlen, private_key->heap);
    #else
        if (private_key->nb_ctx) {
            err = sp_ecc_secret_gen_384_nb(&private_key->nb_ctx->sp_ctx, k,
                                           point, out, outlen,
                                           private_key->heap);
        }
        else {
        #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_secret_gen_384_nb(&nb_ctx.sp_ctx, k, point, out,
                                               outlen, private_key->heap);
            } while (err == FP_WOULDBLOCK);
        #else
            err = sp_ecc_secret_gen_384(k, point, out, outlen,
                                        private_key->heap);
        #endif /* WC_ECC_NONBLOCK_ONLY */
        }
    #endif /* !WC_ECC_NONBLOCK */
    }
    else
#endif /* WOLFSSL_SP_384 */
#ifdef WOLFSSL_SP_521
    if (private_key->idx != ECC_CUSTOM_IDX &&
                               ecc_sets[private_key->idx].id == ECC_SECP521R1) {
    #ifndef WC_ECC_NONBLOCK
        err = sp_ecc_secret_gen_521(k, point, out, outlen, private_key->heap);
    #else
        if (private_key->nb_ctx) {
            err = sp_ecc_secret_gen_521_nb(&private_key->nb_ctx->sp_ctx, k,
                                           point, out, outlen,
                                           private_key->heap);
        }
        else {
        #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_secret_gen_521_nb(&nb_ctx.sp_ctx, k, point, out,
                                               outlen, private_key->heap);
            } while (err == FP_WOULDBLOCK);
        #else
            err = sp_ecc_secret_gen_521(k, point, out, outlen,
                                        private_key->heap);
        #endif /* WC_ECC_NONBLOCK_ONLY */
        }
    #endif /* !WC_ECC_NONBLOCK */
    }
    else
#endif /* WOLFSSL_SP_521 */
#else
    (void)point;
    (void)out;
    (void)outlen;
    (void)k;
#endif
#if defined(WOLFSSL_SP_MATH)
    {
        err = WC_KEY_SIZE_E;
        goto errout;
    }
#else
    {
        ecc_point* result = NULL;
        #ifdef WOLFSSL_NO_MALLOC
        ecc_point  lcl_result;
        #endif
        int x = 0;
        mp_digit mp = 0;
        DECLARE_CURVE_SPECS(3);

        /* load curve info */
        ALLOC_CURVE_SPECS(3, err);
        if (err == MP_OKAY) {
            err = wc_ecc_curve_load(private_key->dp, &curve,
                (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_AF |
                 ECC_CURVE_FIELD_ORDER));
        }

        if (err != MP_OKAY) {
            FREE_CURVE_SPECS();
            goto errout;
        }

        /* make new point */
    #ifdef WOLFSSL_NO_MALLOC
        result = &lcl_result;
    #endif
        err = wc_ecc_new_point_ex(&result, private_key->heap);
        if (err != MP_OKAY) {
            wc_ecc_curve_free(curve);
            FREE_CURVE_SPECS();
            goto errout;
        }

#ifdef ECC_TIMING_RESISTANT
        if (private_key->rng == NULL) {
            err = MISSING_RNG_E;
        }
#endif

        if (err == MP_OKAY) {
            /* Map in a separate call as this should be constant time */
#ifdef ECC_TIMING_RESISTANT
            err = wc_ecc_mulmod_ex2(k, point, result, curve->Af, curve->prime,
                                              curve->order, private_key->rng, 0,
                                              private_key->heap);
#else
            err = wc_ecc_mulmod_ex2(k, point, result, curve->Af, curve->prime,
                                      curve->order, NULL, 0, private_key->heap);
#endif
        }
        if (err == MP_OKAY) {
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            mp_memzero_add("wc_ecc_shared_secret_gen_sync result->x",
                result->x);
            mp_memzero_add("wc_ecc_shared_secret_gen_sync result->y",
                result->y);
        #endif
            err = mp_montgomery_setup(curve->prime, &mp);
        }
        if (err == MP_OKAY) {
            /* Use constant time map if compiled in */
            err = ecc_map_ex(result, curve->prime, mp, 1);
        }
        if (err == MP_OKAY) {
            x = mp_unsigned_bin_size(curve->prime);
            if (*outlen < (word32)x || x < mp_unsigned_bin_size(result->x)) {
                err = BUFFER_E;
            }
        }

        if (err == MP_OKAY) {
            XMEMSET(out, 0, (size_t)x);
            err = mp_to_unsigned_bin(result->x, out +
                                     (x - mp_unsigned_bin_size(result->x)));
        }
        *outlen = (word32)x;

        mp_forcezero(result->x);
        mp_forcezero(result->y);
        wc_ecc_del_point_ex(result, private_key->heap);

        wc_ecc_curve_free(curve);
        FREE_CURVE_SPECS();
    }
#endif

  errout:

#ifdef HAVE_ECC_CDH
    if (k == k_lcl)
        mp_clear(k);
#ifdef WOLFSSL_SMALL_STACK
    if (k_lcl != NULL)
        XFREE(k_lcl, private_key->heap, DYNAMIC_TYPE_ECC_BUFFER);
#endif
#endif

    return err;
}

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
static int wc_ecc_shared_secret_gen_async(ecc_key* private_key,
            ecc_point* point, byte* out, word32 *outlen)
{
    int err = 0;

#if defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA)
    DECLARE_CURVE_SPECS(3);

    /* load curve info */
    ALLOC_CURVE_SPECS(3, err);
    if (err == MP_OKAY) {
        err = wc_ecc_curve_load(private_key->dp, &curve,
            (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_AF |
             ECC_CURVE_FIELD_ORDER));
    }

    if (err != MP_OKAY) {
        FREE_CURVE_SPECS();
        return err;
    }

    if (private_key->dp
    #ifdef WOLFSSL_CUSTOM_CURVES
        && private_key->dp->id != ECC_CURVE_CUSTOM
    #endif
    #ifdef HAVE_CAVIUM_V
        /* verify the curve is supported by hardware */
        && NitroxEccIsCurveSupported(private_key)
    #endif
    ) {
        word32 keySz = private_key->dp->size;

        /* sync public key x/y */
        err = wc_mp_to_bigint_sz(private_key->k, &private_key->k->raw, keySz);
        if (err == MP_OKAY)
            err = wc_mp_to_bigint_sz(point->x, &point->x->raw, keySz);
        if (err == MP_OKAY)
            err = wc_mp_to_bigint_sz(point->y, &point->y->raw, keySz);
    #ifdef HAVE_CAVIUM_V
        /* allocate buffer for output */
        if (err == MP_OKAY)
            err = wc_ecc_alloc_mpint(private_key, &private_key->e);
        if (err == MP_OKAY)
            err = wc_bigint_alloc(&private_key->e->raw,
                NitroxEccGetSize(private_key)*2);
        if (err == MP_OKAY)
            err = NitroxEcdh(private_key,
                &private_key->k->raw, &point->x->raw, &point->y->raw,
                private_key->e->raw.buf, &private_key->e->raw.len,
                &curve->prime->raw);
    #else
        if (err == MP_OKAY)
            err = wc_ecc_curve_load(private_key->dp, &curve, ECC_CURVE_FIELD_BF);
        if (err == MP_OKAY)
            err = IntelQaEcdh(&private_key->asyncDev,
                &private_key->k->raw, &point->x->raw, &point->y->raw,
                out, outlen,
                &curve->Af->raw, &curve->Bf->raw, &curve->prime->raw,
                private_key->dp->cofactor);
    #endif
    }
    else
#elif defined(WOLFSSL_ASYNC_CRYPT_SW)
    if (wc_AsyncSwInit(&private_key->asyncDev, ASYNC_SW_ECC_SHARED_SEC)) {
        WC_ASYNC_SW* sw = &private_key->asyncDev.sw;
        sw->eccSharedSec.private_key = private_key;
        sw->eccSharedSec.public_point = point;
        sw->eccSharedSec.out = out;
        sw->eccSharedSec.outLen = outlen;
        err = WC_PENDING_E;
    }
    else
#endif
    {
        /* use sync in other cases */
        err = wc_ecc_shared_secret_gen_sync(private_key, point, out, outlen);
    }

    if (err == WC_PENDING_E) {
        private_key->state++;
    }

#if defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA)
    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();
#endif

    return err;
}
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_ECC */

#ifndef WOLF_CRYPTO_CB_ONLY_ECC
/**
 Create an ECC shared secret between private key and public point
 private_key      The private ECC key (heap hint based on private key)
 point            The point to use (public key)
 out              [out] Destination of the shared secret
                        Conforms to EC-DH from ANSI X9.63
 outlen           [in/out] The max size and resulting size of the shared secret
 return           MP_OKAY if successful
*/
int wc_ecc_shared_secret_ex(ecc_key* private_key, ecc_point* point,
                            byte* out, word32 *outlen)
{
    int err;

    if (private_key == NULL || point == NULL || out == NULL ||
                                                            outlen == NULL) {
        return BAD_FUNC_ARG;
    }

    /* type valid? */
    if (private_key->type != ECC_PRIVATEKEY &&
            private_key->type != ECC_PRIVATEKEY_ONLY) {
        WOLFSSL_MSG("ECC_BAD_ARG_E");
        return ECC_BAD_ARG_E;
    }

    /* Verify domain params supplied */
    if (wc_ecc_is_valid_idx(private_key->idx) == 0 || private_key->dp == NULL) {
        WOLFSSL_MSG("wc_ecc_is_valid_idx failed");
        return ECC_BAD_ARG_E;
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    switch (private_key->state) {
        case ECC_STATE_NONE:
        case ECC_STATE_SHARED_SEC_GEN:
            private_key->state = ECC_STATE_SHARED_SEC_GEN;

        #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
            if (private_key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
                err = wc_ecc_shared_secret_gen_async(private_key, point,
                    out, outlen);
                if (err == 0) {
                    /* exit early */
                    RESTORE_VECTOR_REGISTERS();
                    return err;
                }
            }
            else
        #endif
            {
                err = wc_ecc_shared_secret_gen_sync(private_key, point,
                    out, outlen);
            }
            if (err < 0) {
                break;
            }
            FALL_THROUGH;

        case ECC_STATE_SHARED_SEC_RES:
            private_key->state = ECC_STATE_SHARED_SEC_RES;
        #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
            if (private_key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
            #ifdef HAVE_CAVIUM_V
                /* verify the curve is supported by hardware */
                if (NitroxEccIsCurveSupported(private_key)) {
                    /* copy output */
                    *outlen = private_key->dp->size;
                    XMEMCPY(out, private_key->e->raw.buf, *outlen);
                }
            #endif /* HAVE_CAVIUM_V */
            }
        #endif /* WOLFSSL_ASYNC_CRYPT */
            err = 0;
            break;

        default:
            err = BAD_STATE_E;
    } /* switch */

    RESTORE_VECTOR_REGISTERS();

    /* if async pending then return and skip done cleanup below */
    if (err == WC_PENDING_E) {
        return err;
    }

    /* cleanup */
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    wc_ecc_free_async(private_key);
#endif
    private_key->state = ECC_STATE_NONE;

    return err;
}
#endif /* WOLF_CRYPTO_CB_ONLY_ECC */
#elif defined(WOLFSSL_KCAPI_ECC)
int wc_ecc_shared_secret_ex(ecc_key* private_key, ecc_point* point,
                            byte* out, word32 *outlen)
{
    int err;
    ecc_key public_key;

    err = wc_ecc_init_ex(&public_key, private_key->heap, INVALID_DEVID);
    if (err == MP_OKAY) {
        err = wc_ecc_set_curve(&public_key, private_key->dp->size,
                               private_key->dp->id);
        if (err == MP_OKAY) {
            err = mp_copy(point->x, public_key.pubkey.x);
        }
        if (err == MP_OKAY) {
            err = mp_copy(point->y, public_key.pubkey.y);
        }
        if (err == MP_OKAY) {
            err = wc_ecc_shared_secret(private_key, &public_key, out, outlen);
        }

        wc_ecc_free(&public_key);
    }

    return err;
}
#endif /* !WOLFSSL_ATECC508A && !WOLFSSL_CRYPTOCELL && !WOLFSSL_KCAPI_ECC */
#endif /* HAVE_ECC_DHE */

#ifdef USE_ECC_B_PARAM
/* Checks if a point p lies on the curve with index curve_idx */
int wc_ecc_point_is_on_curve(ecc_point *p, int curve_idx)
{
    int err = MP_OKAY;
    DECLARE_CURVE_SPECS(3);

    if (p == NULL)
        return BAD_FUNC_ARG;

    /* is the IDX valid ?  */
    if (wc_ecc_is_valid_idx(curve_idx) == 0) {
       return ECC_BAD_ARG_E;
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    ALLOC_CURVE_SPECS(3, err);
    if (err == MP_OKAY) {
        err = wc_ecc_curve_load(wc_ecc_get_curve_params(curve_idx), &curve,
                                ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_AF |
                                ECC_CURVE_FIELD_BF);
    }

    if (err == MP_OKAY) {
        err = wc_ecc_is_point(p, curve->Af, curve->Bf, curve->prime);
    }

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();

    RESTORE_VECTOR_REGISTERS();

    return err;
}
#endif /* USE_ECC_B_PARAM */

#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLF_CRYPTO_CB_ONLY_ECC)
/* return 1 if point is at infinity, 0 if not, < 0 on error */
int wc_ecc_point_is_at_infinity(ecc_point* p)
{
    if (p == NULL)
        return BAD_FUNC_ARG;
    if (mp_iszero(p->x) && mp_iszero(p->y))
        return 1;

    return 0;
}
#endif /* !WOLFSSL_ATECC508A && !WOLFSSL_CRYPTOCELL */

/* generate random and ensure its greater than 0 and less than order */
int wc_ecc_gen_k(WC_RNG* rng, int size, mp_int* k, mp_int* order)
{
#ifndef WC_NO_RNG
    int err;
    byte buf[ECC_MAXSIZE_GEN];

    if (rng == NULL || size < 0 || size + 8 > ECC_MAXSIZE_GEN || k == NULL ||
                                                                order == NULL) {
        return BAD_FUNC_ARG;
    }

    /* generate 8 extra bytes to mitigate bias from the modulo operation below */
    /* see section A.1.2 in 'Suite B Implementor's Guide to FIPS 186-3 (ECDSA)' */
    size += 8;

    /* make up random string */
    err = wc_RNG_GenerateBlock(rng, buf, (word32)size);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_ecc_gen_k buf", buf, size);
#endif

    /* load random buffer data into k */
    if (err == 0)
        err = mp_read_unsigned_bin(k, buf, (word32)size);

    /* the key should be smaller than the order of base point */
    if (err == MP_OKAY) {
        if (mp_cmp(k, order) != MP_LT) {
            err = mp_mod(k, order, k);
        }
    }

    /* quick sanity check to make sure we're not dealing with a 0 key */
    if (err == MP_OKAY) {
        if (mp_iszero(k) == MP_YES)
          err = MP_ZERO_E;
    }

    ForceZero(buf, ECC_MAXSIZE_GEN);
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(buf, ECC_MAXSIZE_GEN);
#endif

    return err;
#else
    (void)rng;
    (void)size;
    (void)k;
    (void)order;
    return NOT_COMPILED_IN;
#endif /* !WC_NO_RNG */
}

static WC_INLINE void wc_ecc_reset(ecc_key* key)
{
    /* make sure required key variables are reset */
    key->state = ECC_STATE_NONE;
}

/* create the public ECC key from a private key
 *
 * key     an initialized private key to generate public part from
 * curve   [in]curve for key, cannot be NULL
 * pubOut  [out]ecc_point holding the public key, if NULL then public key part
 *         is cached in key instead.
 *
 * Note this function is local to the file because of the argument type
 *      ecc_curve_spec. Having this argument allows for not having to load the
 *      curve type multiple times when generating a key with wc_ecc_make_key().
 * For async the results are placed directly into pubOut, so this function
 *      does not need to be called again
 *
 * returns MP_OKAY on success
 */
static int ecc_make_pub_ex(ecc_key* key, ecc_curve_spec* curve,
        ecc_point* pubOut, WC_RNG* rng)
{
    int err = MP_OKAY;
#ifdef HAVE_ECC_MAKE_PUB
    ecc_point* pub;
#endif /* HAVE_ECC_MAKE_PUB */

    (void)rng;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

#ifdef HAVE_ECC_MAKE_PUB
    /* if ecc_point passed in then use it as output for public key point */
    if (pubOut != NULL) {
        pub = pubOut;
    }
    else {
        /* caching public key making it a ECC_PRIVATEKEY instead of
           ECC_PRIVATEKEY_ONLY */
        pub = &key->pubkey;
        key->type = ECC_PRIVATEKEY_ONLY;
    }

    if ((err == MP_OKAY) && (mp_iszero(key->k) || mp_isneg(key->k) ||
                                      (mp_cmp(key->k, curve->order) != MP_LT)))
    {
        err = ECC_PRIV_KEY_E;
    }

    if (err == MP_OKAY) {
    #ifndef ALT_ECC_SIZE
        err = mp_init_multi(pub->x, pub->y, pub->z, NULL, NULL, NULL);
    #else
        pub->x = (mp_int*)&pub->xyz[0];
        pub->y = (mp_int*)&pub->xyz[1];
        pub->z = (mp_int*)&pub->xyz[2];
        alt_fp_init(pub->x);
        alt_fp_init(pub->y);
        alt_fp_init(pub->z);
    #endif
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC_KEYGEN) && \
    defined(HAVE_INTEL_QA)
    if (err == MP_OKAY && key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
        word32 keySz = key->dp->size;
        /* sync private key to raw */
        err = wc_mp_to_bigint_sz(key->k, &key->k->raw, keySz);
        if (err == MP_OKAY) {
            err = IntelQaEccPointMul(&key->asyncDev,
                &key->k->raw, pub->x, pub->y, pub->z,
                &curve->Gx->raw, &curve->Gy->raw,
                &curve->Af->raw, &curve->Bf->raw, &curve->prime->raw,
                key->dp->cofactor);
        }
    }
    else
#endif
    { /* BEGIN: Software Crypto */
#ifdef WOLFSSL_HAVE_SP_ECC
    /* Single-Precision Math (optimized for specific curves) */
    if (err != MP_OKAY) {
    }
    else
#ifndef WOLFSSL_SP_NO_256
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP256R1) {
        err = sp_ecc_mulmod_base_256(key->k, pub, 1, key->heap);
    }
    else
#ifdef SM2_SP_IMPL_AVAILABLE
#ifdef WOLFSSL_SM2
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SM2P256V1) {
        err = sp_ecc_mulmod_base_sm2_256(&key->k, pub, 1, key->heap);
    }
    else
#endif
#endif
#endif /* WOLFSSL_SP_NO_256 */
#ifdef WOLFSSL_SP_384
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP384R1) {
        err = sp_ecc_mulmod_base_384(key->k, pub, 1, key->heap);
    }
    else
#endif
#ifdef WOLFSSL_SP_521
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP521R1) {
        err = sp_ecc_mulmod_base_521(key->k, pub, 1, key->heap);
    }
    else
#endif
#endif /* WOLFSSL_HAVE_SP_ECC */

#if defined(WOLFSSL_SP_MATH)
        err = WC_KEY_SIZE_E;
#else
    if (err == MP_OKAY) {
        /* Multi-Precision Math: compute public curve */
        mp_digit mp = 0;
        ecc_point* base = NULL;
    #ifdef WOLFSSL_NO_MALLOC
        ecc_point  lcl_base;
        base = &lcl_base;
    #endif
        err = wc_ecc_new_point_ex(&base, key->heap);

        /* read in the x/y for this key */
        if (err == MP_OKAY)
            err = mp_copy(curve->Gx, base->x);
        if (err == MP_OKAY)
            err = mp_copy(curve->Gy, base->y);
        if (err == MP_OKAY)
            err = mp_montgomery_setup(curve->prime, &mp);
        if (err == MP_OKAY)
            err = mp_set(base->z, 1);

        /* make the public key */
        if (err == MP_OKAY) {
            /* Map in a separate call as this should be constant time */
            err = wc_ecc_mulmod_ex2(key->k, base, pub, curve->Af, curve->prime,
                                               curve->order, rng, 0, key->heap);
            if (err == MP_MEM) {
               err = MEMORY_E;
            }
        }
        if (err == MP_OKAY) {
            /* Use constant time map if compiled in */
            err = ecc_map_ex(pub, curve->prime, mp, 1);
        }

        wc_ecc_del_point_ex(base, key->heap);
    }
#endif /* WOLFSSL_SP_MATH */
    } /* END: Software Crypto */

    if (err != MP_OKAY
    #ifdef WOLFSSL_ASYNC_CRYPT
        && err != WC_PENDING_E
    #endif
    ) {
        /* clean up if failed */
    #ifndef ALT_ECC_SIZE
        mp_clear(pub->x);
        mp_clear(pub->y);
        mp_clear(pub->z);
    #endif
    }

#else
    /* Using hardware crypto, that does not support ecc_make_pub_ex */
    (void)curve;
    err = NOT_COMPILED_IN;
#endif /* HAVE_ECC_MAKE_PUB */

    /* change key state if public part is cached */
    if (key->type == ECC_PRIVATEKEY_ONLY && pubOut == NULL) {
        key->type = ECC_PRIVATEKEY;
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}


/* create the public ECC key from a private key
 *
 * key     an initialized private key to generate public part from
 * pubOut  [out]ecc_point holding the public key, if NULL then public key part
 *         is cached in key instead.
 *
 *
 * returns MP_OKAY on success
 */
int wc_ecc_make_pub(ecc_key* key, ecc_point* pubOut)
{
    WOLFSSL_ENTER("wc_ecc_make_pub");

    return wc_ecc_make_pub_ex(key, pubOut, NULL);
}

/* create the public ECC key from a private key - mask timing use random z
 *
 * key     an initialized private key to generate public part from
 * pubOut  [out]ecc_point holding the public key, if NULL then public key part
 *         is cached in key instead.
 *
 *
 * returns MP_OKAY on success
 */
int wc_ecc_make_pub_ex(ecc_key* key, ecc_point* pubOut, WC_RNG* rng)
{
    int err = MP_OKAY;
    DECLARE_CURVE_SPECS(ECC_CURVE_FIELD_COUNT);

    WOLFSSL_ENTER("wc_ecc_make_pub_ex");

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    /* load curve info */
    ALLOC_CURVE_SPECS(ECC_CURVE_FIELD_COUNT, err);
    if (err == MP_OKAY) {
        err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
    }
    if (err == MP_OKAY) {
        err = ecc_make_pub_ex(key, curve, pubOut, rng);
    }

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();

    return err;
}


static int _ecc_make_key_ex(WC_RNG* rng, int keysize, ecc_key* key,
        int curve_id, int flags)
{
    int err = 0;
#if defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_ATECC508A) && \
    !defined(WOLFSSL_ATECC608A)
    const CRYS_ECPKI_Domain_t*  pDomain;
    CRYS_ECPKI_KG_TempData_t    tempBuff;
    CRYS_ECPKI_KG_FipsContext_t fipsCtx;
    byte ucompressed_key[ECC_MAX_CRYPTO_HW_SIZE*2 + 1];
    word32 raw_size = 0;
#endif
#if defined(WOLFSSL_HAVE_SP_ECC) && defined(WC_ECC_NONBLOCK) && \
    defined(WC_ECC_NONBLOCK_ONLY)
    ecc_nb_ctx_t nb_ctx;
    XMEMSET(&nb_ctx, 0, sizeof(nb_ctx));
#endif /* WOLFSSL_HAVE_SP_ECC && WC_ECC_NONBLOCK && WC_ECC_NONBLOCK_ONLY */

    if (key == NULL || rng == NULL) {
        return BAD_FUNC_ARG;
    }

    /* make sure required variables are reset */
    wc_ecc_reset(key);

    err = wc_ecc_set_curve(key, keysize, curve_id);
    if (err != 0) {
        return err;
    }

    key->flags = (byte)flags;

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (key->devId != INVALID_DEVID)
    #endif
    {
        err = wc_CryptoCb_MakeEccKey(rng, keysize, key, curve_id);
        #ifndef WOLF_CRYPTO_CB_ONLY_ECC
        if (err != CRYPTOCB_UNAVAILABLE)
            return err;
        /* fall-through when unavailable */
        #endif
        #ifdef WOLF_CRYPTO_CB_ONLY_ECC
        if (err == CRYPTOCB_UNAVAILABLE) {
            return NO_VALID_DEVID;
        }
        return err;
        #endif
    }
#endif

#ifndef WOLF_CRYPTO_CB_ONLY_ECC
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
    #ifdef HAVE_CAVIUM
        /* TODO: Not implemented */
    #elif defined(HAVE_INTEL_QA)
        /* Implemented in ecc_make_pub_ex for the pub calc */
    #else
        if (wc_AsyncSwInit(&key->asyncDev, ASYNC_SW_ECC_MAKE)) {
            WC_ASYNC_SW* sw = &key->asyncDev.sw;
            sw->eccMake.rng = rng;
            sw->eccMake.key = key;
            sw->eccMake.size = keysize;
            sw->eccMake.curve_id = curve_id;
            return WC_PENDING_E;
        }
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_ECC */

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
   if (key->dp->id == ECC_SECP256R1) {
       key->type = ECC_PRIVATEKEY;
       key->slot = atmel_ecc_alloc(ATMEL_SLOT_ECDHE);
       err = atmel_ecc_create_key(key->slot, key->pubkey_raw);

       /* populate key->pubkey */
       if (err == 0
       #ifdef ALT_ECC_SIZE
          && key->pubkey.x
       #endif
       ) {
           err = mp_read_unsigned_bin(key->pubkey.x, key->pubkey_raw,
                                      ECC_MAX_CRYPTO_HW_SIZE);
       }
       if (err == 0
       #ifdef ALT_ECC_SIZE
          && key->pubkey.y
       #endif
       ) {
           err = mp_read_unsigned_bin(key->pubkey.y,
                                      key->pubkey_raw + ECC_MAX_CRYPTO_HW_SIZE,
                                      ECC_MAX_CRYPTO_HW_SIZE);
       }
   }
   else {
      err = NOT_COMPILED_IN;
   }
#elif defined(WOLFSSL_SE050)
    err = se050_ecc_create_key(key, key->dp->id, key->dp->size);
    key->type = ECC_PRIVATEKEY;
#elif defined(WOLFSSL_CRYPTOCELL)

    pDomain = CRYS_ECPKI_GetEcDomain(cc310_mapCurve(key->dp->id));
    raw_size = (word32)(key->dp->size)*2 + 1;

    /* generate first key pair */
    err = CRYS_ECPKI_GenKeyPair(&wc_rndState,
                                wc_rndGenVectFunc,
                                pDomain,
                                &key->ctx.privKey,
                                &key->ctx.pubKey,
                                &tempBuff,
                                &fipsCtx);

    if (err != SA_SILIB_RET_OK){
        WOLFSSL_MSG("CRYS_ECPKI_GenKeyPair for key pair failed");
        return err;
    }
    key->type = ECC_PRIVATEKEY;

    err = CRYS_ECPKI_ExportPublKey(&key->ctx.pubKey,
                                   CRYS_EC_PointUncompressed,
                                   &ucompressed_key[0],
                                   (uint32_t*)&raw_size);

    if (err == SA_SILIB_RET_OK && key->pubkey.x && key->pubkey.y) {
        err = mp_read_unsigned_bin(key->pubkey.x,
                                   &ucompressed_key[1], key->dp->size);
        if (err == MP_OKAY) {
            err = mp_read_unsigned_bin(key->pubkey.y,
                            &ucompressed_key[1+key->dp->size],key->dp->size);
        }
    }
    raw_size = key->dp->size;
    if (err == MP_OKAY) {
        err = CRYS_ECPKI_ExportPrivKey(&key->ctx.privKey,
                                       ucompressed_key,
                                       (uint32_t*)&raw_size);
    }

    if (err == SA_SILIB_RET_OK) {
        err = mp_read_unsigned_bin(key->k, ucompressed_key, raw_size);
    }

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    return silabs_ecc_make_key(key, keysize);
#elif defined(WOLFSSL_KCAPI_ECC)

    err = KcapiEcc_MakeKey(key, keysize, curve_id);
    (void)rng;

#elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    if (xil_curve_type[key->dp->id] == 0)
        return ECC_CURVE_OID_E;

    err = wc_RNG_GenerateBlock(rng, key->privKey, key->dp->size);
    if (err)
        return err;
    /* Make sure that private key is max. 521 bits */
    if (key->dp->size == 66)
        key->privKey[65] &= 0x1U;

    WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(key->privKey), key->dp->size);

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE(XIL_CAST_U64(key->keyRaw),
                                        2 * key->dp->size);

    err = XSecure_EllipticGenerateKey(&(key->xSec.cinst),
                                      xil_curve_type[key->dp->id],
                                      XIL_CAST_U64(key->privKey),
                                      XIL_CAST_U64(key->keyRaw));
    if (err != XST_SUCCESS) {
        WOLFSSL_XIL_ERROR("Generate ECC key failed", err);
        err = WC_HW_E;
    }

    WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE(XIL_CAST_U64(key->keyRaw),
                                        2 * key->dp->size);

#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    if (err == 0)
        err = XSecure_EllipticValidateKey(&(key->xSec.cinst),
                                          xil_curve_type[key->dp->id],
                                          XIL_CAST_U64(key->keyRaw));
#endif

    if (err == 0)
        err = xil_mpi_import(key->pubkey.x, key->keyRaw, key->dp->size,
                             key->heap);
    if (err == 0)
        err = xil_mpi_import(key->pubkey.y, key->keyRaw + key->dp->size,
                             key->dp->size, key->heap);
    if (err == 0)
        err = xil_mpi_import(key->k, key->privKey, key->dp->size, key->heap);
    if (err == 0)
        err = mp_set(key->pubkey.z, 1);
    if (err) {
        key->privKey = NULL;
        XMEMSET(key->keyRaw, 0, sizeof(key->keyRaw));
        return err;
    }

    key->type = ECC_PRIVATEKEY;

#else

#ifdef WOLFSSL_HAVE_SP_ECC

#ifndef WOLFSSL_SP_NO_256
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP256R1) {
    #ifndef WC_ECC_NONBLOCK
        err = sp_ecc_make_key_256(rng, key->k, &key->pubkey, key->heap);
    #else
        if (key->nb_ctx) {
            err = sp_ecc_make_key_256_nb(&key->nb_ctx->sp_ctx, rng, key->k,
                                         &key->pubkey, key->heap);
        }
        else {
        #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_make_key_256_nb(&nb_ctx.sp_ctx, rng, key->k,
                                             &key->pubkey, key->heap);
            } while (err == FP_WOULDBLOCK);
        #else
            err = sp_ecc_make_key_256(rng, key->k, &key->pubkey, key->heap);
        #endif /* WC_ECC_NONBLOCK_ONLY */
        }
    #endif /* !WC_ECC_NONBLOCK */

        if (err == MP_OKAY) {
            key->type = ECC_PRIVATEKEY;
        }
    }
    else
#ifdef SM2_SP_IMPL_AVAILABLE
#ifdef WOLFSSL_SM2
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SM2P256V1) {
        err = sp_ecc_make_key_sm2_256(rng, &key->k, &key->pubkey, key->heap);
        if (err == MP_OKAY) {
            key->type = ECC_PRIVATEKEY;
        }
    }
    else
#endif
#endif
#endif /* !WOLFSSL_SP_NO_256 */
#ifdef WOLFSSL_SP_384
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP384R1) {
    #ifndef WC_ECC_NONBLOCK
        err = sp_ecc_make_key_384(rng, key->k, &key->pubkey, key->heap);
    #else
        if (key->nb_ctx) {
            err = sp_ecc_make_key_384_nb(&key->nb_ctx->sp_ctx, rng, key->k,
                                         &key->pubkey, key->heap);
        }
        else {
        #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_make_key_384_nb(&nb_ctx.sp_ctx, rng, key->k,
                                             &key->pubkey, key->heap);
            } while (err == FP_WOULDBLOCK);
        #else
            err = sp_ecc_make_key_384(rng, key->k, &key->pubkey, key->heap);
        #endif /* WC_ECC_NONBLOCK_ONLY */
        }
    #endif /* !WC_ECC_NONBLOCK */

        if (err == MP_OKAY) {
            key->type = ECC_PRIVATEKEY;
        }
    }
    else
#endif /* WOLFSSL_SP_384 */
#ifdef WOLFSSL_SP_521
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP521R1) {
    #ifndef WC_ECC_NONBLOCK
        err = sp_ecc_make_key_521(rng, key->k, &key->pubkey, key->heap);
    #else
        if (key->nb_ctx) {
            err = sp_ecc_make_key_521_nb(&key->nb_ctx->sp_ctx, rng, key->k,
                                         &key->pubkey, key->heap);
        }
        else {
        #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_make_key_521_nb(&nb_ctx.sp_ctx, rng, key->k,
                                             &key->pubkey, key->heap);
            } while (err == FP_WOULDBLOCK);
        #else
            err = sp_ecc_make_key_521(rng, key->k, &key->pubkey, key->heap);
        #endif /* WC_ECC_NONBLOCK_ONLY */
        }
    #endif /* !WC_ECC_NONBLOCK */

        if (err == MP_OKAY) {
            key->type = ECC_PRIVATEKEY;
        }
    }
    else
#endif /* WOLFSSL_SP_521 */
#endif /* WOLFSSL_HAVE_SP_ECC */

   { /* software key gen */
#if defined(WOLFSSL_SP_MATH)
        err = WC_KEY_SIZE_E;
#else
        DECLARE_CURVE_SPECS(ECC_CURVE_FIELD_COUNT);

        /* setup the key variables */
#ifndef ALT_ECC_SIZE
        err = mp_init(key->k);
#else
        err = 0;
        key->k = (mp_int*)key->ka;
        alt_fp_init(key->k);
#endif

        /* load curve info */
        if (err == MP_OKAY) {
            ALLOC_CURVE_SPECS(ECC_CURVE_FIELD_COUNT, err);
        }
        if (err == MP_OKAY) {
            err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
        }

        /* generate k */
        if (err == MP_OKAY) {
            err = wc_ecc_gen_k(rng, key->dp->size, key->k, curve->order);
        }

        /* generate public key from k */
        if (err == MP_OKAY) {
            err = ecc_make_pub_ex(key, curve, NULL, rng);
        }

        if (err == MP_OKAY
        #ifdef WOLFSSL_ASYNC_CRYPT
            || err == WC_PENDING_E
        #endif
        ) {
            key->type = ECC_PRIVATEKEY;
        }
        else {
            /* cleanup these on failure case only */
            mp_forcezero(key->k);
        }

        /* cleanup allocations */
        wc_ecc_curve_free(curve);
        FREE_CURVE_SPECS();
#endif /* WOLFSSL_SP_MATH */
    }

#ifdef HAVE_WOLF_BIGINT
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(key->k, &key->k->raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(key->pubkey.x, &key->pubkey.x->raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(key->pubkey.y, &key->pubkey.y->raw);
    if (err == MP_OKAY)
         err = wc_mp_to_bigint(key->pubkey.z, &key->pubkey.z->raw);
#endif

#endif /* HAVE_ECC_MAKE_PUB */

    return err;
#endif /* WOLF_CRYPTO_CB_ONLY_ECC */
}


int wc_ecc_make_key_ex2(WC_RNG* rng, int keysize, ecc_key* key, int curve_id,
                        int flags)
{
    int err;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    err = _ecc_make_key_ex(rng, keysize, key, curve_id, flags);

#if (FIPS_VERSION_GE(5,0) || defined(WOLFSSL_VALIDATE_ECC_KEYGEN)) && \
    !defined(WOLFSSL_KCAPI_ECC)
    if (err == MP_OKAY) {
        err = _ecc_validate_public_key(key, 0, 0);
    }
    if (err == MP_OKAY
#if defined(WOLF_CRYPTO_CB)
        /* even if WOLF_CRYPTO_CB we generate the key if the devId is invalid */
        && key->devId == INVALID_DEVID
#endif
        ) {
        err = _ecc_pairwise_consistency_test(key, rng);
    }
#endif

    RESTORE_VECTOR_REGISTERS();

    return err;
}

WOLFSSL_ABI
int wc_ecc_make_key_ex(WC_RNG* rng, int keysize, ecc_key* key, int curve_id)
{
    return wc_ecc_make_key_ex2(rng, keysize, key, curve_id, WC_ECC_FLAG_NONE);
}

#ifdef ECC_DUMP_OID
/* Optional dump of encoded OID for adding new curves */
static int mOidDumpDone;
static void wc_ecc_dump_oids(void)
{
    int x;

    if (mOidDumpDone) {
        return;
    }

    /* find matching OID sum (based on encoded value) */
    for (x = 0; ecc_sets[x].size != 0; x++) {
        int i;
        byte* oid;
        word32 oidSz, sum = 0;

        printf("ECC %s (%d):\n", ecc_sets[x].name, x);

    #ifdef HAVE_OID_ENCODING
        byte oidEnc[ECC_MAX_OID_LEN];

        oid = oidEnc;
        oidSz = ECC_MAX_OID_LEN;

        printf("OID: ");
        for (i = 0; i < (int)ecc_sets[x].oidSz; i++) {
            printf("%d.", ecc_sets[x].oid[i]);
        }
        printf("\n");

        EncodeObjectId(ecc_sets[x].oid, ecc_sets[x].oidSz, oidEnc, &oidSz);
    #else
        oid = (byte*)ecc_sets[x].oid;
        oidSz = ecc_sets[x].oidSz;
    #endif

        printf("OID Encoded: ");
        for (i = 0; i < (int)oidSz; i++) {
            printf("0x%02X,", oid[i]);
        }
        printf("\n");

        for (i = 0; i < (int)oidSz; i++) {
            sum += oid[i];
        }
        printf("Sum: %u\n", sum);

        /* validate sum */
        if (ecc_sets[x].oidSum != sum) {
            fprintf(stderr, "  Sum %u Not Valid!\n", ecc_sets[x].oidSum);
        }
    }
    mOidDumpDone = 1;
}
#endif /* ECC_DUMP_OID */


WOLFSSL_ABI
ecc_key* wc_ecc_key_new(void* heap)
{
    int devId = INVALID_DEVID;
    ecc_key* key;

#if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    /* assume all keys are using CAAM for ECC unless explicitly set otherwise */
    devId = WOLFSSL_CAAM_DEVID;
#endif
    key = (ecc_key*)XMALLOC(sizeof(ecc_key), heap, DYNAMIC_TYPE_ECC);
    if (key) {
        if (wc_ecc_init_ex(key, heap, devId) != 0) {
            XFREE(key, heap, DYNAMIC_TYPE_ECC);
            key = NULL;
        }
    }

    return key;
}


WOLFSSL_ABI
void wc_ecc_key_free(ecc_key* key)
{
    if (key) {
        void* heap = key->heap;

        wc_ecc_free(key);
        ForceZero(key, sizeof(ecc_key));
        XFREE(key, heap, DYNAMIC_TYPE_ECC);
        (void)heap;
    }
}


/**
 Make a new ECC key
 rng          An active RNG state
 keysize      The keysize for the new key (in octets from 20 to 65 bytes)
 key          [out] Destination of the newly created key
 return       MP_OKAY if successful,
 upon error all allocated memory will be freed
 */
WOLFSSL_ABI
int wc_ecc_make_key(WC_RNG* rng, int keysize, ecc_key* key)
{
    return wc_ecc_make_key_ex(rng, keysize, key, ECC_CURVE_DEF);
}

/* Setup dynamic pointers if using normal math for proper freeing */
WOLFSSL_ABI
int wc_ecc_init_ex(ecc_key* key, void* heap, int devId)
{
    int ret      = 0;
#if defined(HAVE_PKCS11)
    int isPkcs11 = 0;
#endif

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(HAVE_PKCS11)
    if (key->isPkcs11) {
        isPkcs11 = 1;
    }
#endif

#ifdef ECC_DUMP_OID
    wc_ecc_dump_oids();
#endif

    XMEMSET(key, 0, sizeof(ecc_key));
    key->state = ECC_STATE_NONE;

#if defined(PLUTON_CRYPTO_ECC) || defined(WOLF_CRYPTO_CB)
    key->devId = devId;
#else
    (void)devId;
#endif

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
    key->slot = ATECC_INVALID_SLOT;
#elif defined(WOLFSSL_KCAPI_ECC)
    key->handle = NULL;
#else
#ifdef ALT_ECC_SIZE
    key->pubkey.x = (mp_int*)&key->pubkey.xyz[0];
    key->pubkey.y = (mp_int*)&key->pubkey.xyz[1];
    key->pubkey.z = (mp_int*)&key->pubkey.xyz[2];
    alt_fp_init(key->pubkey.x);
    alt_fp_init(key->pubkey.y);
    alt_fp_init(key->pubkey.z);
    key->k = (mp_int*)key->ka;
    alt_fp_init(key->k);
#else
    ret = mp_init_multi(key->k, key->pubkey.x, key->pubkey.y, key->pubkey.z,
                                                                    NULL, NULL);
    if (ret != MP_OKAY) {
        return MEMORY_E;
    }
#endif /* ALT_ECC_SIZE */
#endif /* WOLFSSL_ATECC508A */
#if (defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP) || \
     defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
     defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)) && \
     defined(WOLFSSL_NO_MALLOC)
    ret = mp_init(key->sign_k);
    if (ret != MP_OKAY) {
        return MEMORY_E;
    }
#endif

#ifdef WOLFSSL_HEAP_TEST
    key->heap = (void*)WOLFSSL_HEAP_TEST;
#else
    key->heap = heap;
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    #if defined(HAVE_PKCS11)
        if (!isPkcs11)
    #endif
        {
            /* handle as async */
            ret = wolfAsync_DevCtxInit(&key->asyncDev, WOLFSSL_ASYNC_MARKER_ECC,
                                                            key->heap, devId);
        }
#elif defined(HAVE_PKCS11)
    (void)isPkcs11;
#endif

#if defined(WOLFSSL_DSP)
    key->handle = -1;
#endif

#ifdef WOLFSSL_SE050
    key->keyId = 0;
    key->keyIdSet = 0;
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    mp_memzero_add("ECC k", key->k);
#endif

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    key->privKey = key->keyRaw + (2 * ECC_MAX_CRYPTO_HW_SIZE);

    if (wc_InitXsecure(&(key->xSec))) {
        WOLFSSL_MSG("Can't initialize Xsecure");
        return WC_HW_E;
    }
#endif

    return ret;
}

WOLFSSL_ABI
int wc_ecc_init(ecc_key* key)
{
#if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    return wc_ecc_init_ex(key, NULL, WOLFSSL_CAAM_DEVID);
#else
    return wc_ecc_init_ex(key, NULL, INVALID_DEVID);
#endif
}

#ifdef WOLF_PRIVATE_KEY_ID
int wc_ecc_init_id(ecc_key* key, unsigned char* id, int len, void* heap,
                   int devId)
{
    int ret = 0;
#ifdef WOLFSSL_SE050
    /* SE050 TLS users store a word32 at id, need to cast back */
    word32* keyPtr = NULL;
#endif

    if (key == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0 && (len < 0 || len > ECC_MAX_ID_LEN))
        ret = BUFFER_E;

#if defined(HAVE_PKCS11)
    XMEMSET(key, 0, sizeof(ecc_key));
    key->isPkcs11 = 1;
#endif

    if (ret == 0)
        ret = wc_ecc_init_ex(key, heap, devId);
    if (ret == 0 && id != NULL && len != 0) {
        XMEMCPY(key->id, id, (size_t)len);
        key->idLen = len;
    #ifdef WOLFSSL_SE050
        /* Set SE050 ID from word32, populate ecc_key with public from SE050 */
        if (len == (int)sizeof(word32)) {
            keyPtr = (word32*)key->id;
            ret = wc_ecc_use_key_id(key, *keyPtr, 0);
        }
    #endif
    }

    return ret;
}

int wc_ecc_init_label(ecc_key* key, const char* label, void* heap, int devId)
{
    int ret = 0;
    int labelLen = 0;

    if (key == NULL || label == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0) {
        labelLen = (int)XSTRLEN(label);
        if (labelLen == 0 || labelLen > ECC_MAX_LABEL_LEN)
            ret = BUFFER_E;
    }

#if defined(HAVE_PKCS11)
    XMEMSET(key, 0, sizeof(ecc_key));
    key->isPkcs11 = 1;
#endif

    if (ret == 0)
        ret = wc_ecc_init_ex(key, heap, devId);
    if (ret == 0) {
        XMEMCPY(key->label, label, (size_t)labelLen);
        key->labelLen = labelLen;
    }

    return ret;
}
#endif /* WOLF_PRIVATE_KEY_ID */

int wc_ecc_set_flags(ecc_key* key, word32 flags)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }
    key->flags |= flags;
    return 0;
}


static int wc_ecc_get_curve_order_bit_count(const ecc_set_type* dp)
{
    int err = MP_OKAY;
    int orderBits;
    DECLARE_CURVE_SPECS(1);

    ALLOC_CURVE_SPECS(1, err);
    if (err == MP_OKAY) {
        err = wc_ecc_curve_load(dp, &curve, ECC_CURVE_FIELD_ORDER);
    }

    if (err != 0) {
       FREE_CURVE_SPECS();
       return err;
    }
    orderBits = mp_count_bits(curve->order);

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();
    return orderBits;
}

#ifdef HAVE_ECC_SIGN

#ifndef NO_ASN


#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A) ||  \
    defined(PLUTON_CRYPTO_ECC) || defined(WOLFSSL_CRYPTOCELL) || \
    defined(WOLFSSL_SILABS_SE_ACCEL) || defined(WOLFSSL_KCAPI_ECC) || \
    defined(WOLFSSL_SE050) || defined(WOLFSSL_XILINX_CRYPT_VERSAL)
static int wc_ecc_sign_hash_hw(const byte* in, word32 inlen,
    mp_int* r, mp_int* s, byte* out, word32 *outlen, WC_RNG* rng,
    ecc_key* key)
{
    int err;
#ifdef PLUTON_CRYPTO_ECC
    if (key->devId != INVALID_DEVID) /* use hardware */
#endif
    {
    #if defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_ATECC508A) && \
        !defined(WOLFSSL_ATECC608A)
        CRYS_ECDSA_SignUserContext_t sigCtxTemp;
        word32 raw_sig_size = *outlen;
        word32 msgLenInBytes = inlen;
        CRYS_ECPKI_HASH_OpMode_t hash_mode;
    #endif
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
#ifdef WOLFSSL_SMALL_STACK
        byte* K = NULL;
        byte* incopy = NULL;
#else
        byte K[MAX_ECC_BYTES] = {0};
        byte incopy[MAX_ECC_BYTES] = {0};
#endif
#if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
        word32 Ksize;
#endif
#endif
        word32 keysize = (word32)key->dp->size;
    #ifdef PLUTON_CRYPTO_ECC
        word32 orderBits = wc_ecc_get_curve_order_bit_count(key->dp);
    #endif

    #ifndef WOLFSSL_KCAPI_ECC
        /* Check args */
        if (keysize > ECC_MAX_CRYPTO_HW_SIZE || *outlen < keysize*2) {
            return ECC_BAD_ARG_E;
        }
    #endif

    #if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
        /* Sign: Result is 32-bytes of R then 32-bytes of S */
        err = atmel_ecc_sign(key->slot, in, out);
        if (err != 0) {
           return err;
        }
    #elif defined(PLUTON_CRYPTO_ECC)
        {
            /* if the input is larger than curve order, we must truncate */
            if ((inlen * WOLFSSL_BIT_SIZE) > orderBits) {
               inlen = (orderBits + WOLFSSL_BIT_SIZE - 1) / WOLFSSL_BIT_SIZE;
            }

            /* perform ECC sign */
            word32 raw_sig_size = *outlen;
            err = Crypto_EccSign(in, inlen, out, &raw_sig_size);
            if (err != CRYPTO_RES_SUCCESS || raw_sig_size != keysize*2){
               return BAD_COND_E;
            }
        }
    #elif defined(WOLFSSL_SILABS_SE_ACCEL)
        err = silabs_ecc_sign_hash(in, inlen, out, outlen, key);
        if (err != 0) {
               return WC_HW_E;
        }
    #elif defined(WOLFSSL_CRYPTOCELL)
        /* truncate if hash is longer than key size */
        if (msgLenInBytes > keysize) {
            msgLenInBytes = keysize;
        }
        hash_mode = cc310_hashModeECC(msgLenInBytes);
        if (hash_mode == CRYS_ECPKI_HASH_OpModeLast) {
            (void)cc310_hashModeECC(keysize);
            /* Ignoring returned value */
            hash_mode = CRYS_ECPKI_HASH_SHA256_mode;

        }

        /* create signature from an input buffer using a private key*/
        err = CRYS_ECDSA_Sign(&wc_rndState,
                               wc_rndGenVectFunc,
                               &sigCtxTemp,
                               &key->ctx.privKey,
                               hash_mode,
                               (byte*)in,
                               msgLenInBytes,
                               out,
                               (uint32_t*)&raw_sig_size);

        if (err != SA_SILIB_RET_OK){
            WOLFSSL_MSG("CRYS_ECDSA_Sign failed");
            return err;
        }
    #elif defined(WOLFSSL_KCAPI_ECC)
        err = KcapiEcc_Sign(key, in, inlen, out, *outlen);
        if (err != MP_OKAY) {
            return err;
        }
        (void)rng;
    #elif defined(WOLFSSL_SE050)
        err = se050_ecc_sign_hash_ex(in, inlen, r, s, out, outlen, key);
        if (err != MP_OKAY) {
            return err;
        }
        (void)rng;
    #elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)

#ifdef WOLFSSL_SMALL_STACK
        K = (byte*)XMALLOC(keysize, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        incopy = (byte*)XMALLOC(inlen, key->heap, DYNAMIC_TYPE_HASH_TMP);
        if (K == NULL || incopy == NULL) {
            XFREE(incopy, key->heap, DYNAMIC_TYPE_HASH_TMP);
            XFREE(K, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
            return MEMORY_E;
        }
#else
        if (inlen > sizeof(incopy))
            return ECC_BAD_ARG_E;
#endif

#if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
        err = deterministic_sign_helper(in, inlen, key);
        if (err)
            return err;
        Ksize = mp_unsigned_bin_size(key->sign_k);
        if (Ksize > keysize) {
            err = BUFFER_E;
            goto error_out;
        }
        err = mp_to_unsigned_bin(key->sign_k, K);
        if (err)
            goto error_out;
        mp_reverse(K, Ksize);
#else
        err = wc_RNG_GenerateBlock(rng, K, keysize);
        if (err)
            goto error_out;
        /* Make sure that K is max. 521 bits */
        if (keysize == 66)
            K[65] &= 0x1;
#endif
        buf_reverse(incopy, in, inlen < keysize ? inlen : keysize);
        WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(incopy), keysize);
        WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(key->privKey), keysize);
        WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(K), keysize);

        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE(XIL_CAST_U64(out), keysize * 2);

        err = XSecure_EllipticGenerateSign(&(key->xSec.cinst),
                                           xil_curve_type[key->dp->id],
                                           XIL_CAST_U64(incopy), keysize,
                                           XIL_CAST_U64(key->privKey),
                                           XIL_CAST_U64(K),
                                           XIL_CAST_U64(out));
        if (err) {
            WOLFSSL_XIL_ERROR("Generate ECC signature failed", err);
            err = WC_HW_E;
        }

        WOLFSSL_XIL_DCACHE_INVALIDATE_RANGE(XIL_CAST_U64(out), keysize * 2);
        mp_reverse(&out[0], keysize);
        mp_reverse(&out[keysize], keysize);

error_out:
        ForceZero(K, MAX_ECC_BYTES);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(incopy, key->heap, DYNAMIC_TYPE_HASH_TMP);
        XFREE(K, key->heap, DYNAMIC_TYPE_PRIVATE_KEY);
#endif
        if (err) {
            ForceZero(out, keysize * 2);
            return err;
        }
    #endif /* HW-specific #if-#elif chain */

    #ifndef WOLFSSL_SE050
        /* Load R and S, SE050 does this in port layer */
        err = mp_read_unsigned_bin(r, &out[0], keysize);
        if (err != MP_OKAY) {
            return err;
        }
        err = mp_read_unsigned_bin(s, &out[keysize], keysize);
        if (err != MP_OKAY) {
            return err;
        }
    #endif

        /* Check for zeros */
        if (mp_iszero(r) || mp_iszero(s)) {
            return MP_ZERO_E;
        }
    }
#ifdef PLUTON_CRYPTO_ECC
    else {
        err = wc_ecc_sign_hash_ex(in, inlen, rng, key, r, s);
    }
#endif
    (void)rng;

    return err;
}
#endif /* WOLFSSL_ATECC508A || PLUTON_CRYPTO_ECC || WOLFSSL_CRYPTOCELL */

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
static int wc_ecc_sign_hash_async(const byte* in, word32 inlen, byte* out,
    word32 *outlen, WC_RNG* rng, ecc_key* key)
{
    int err;
    mp_int *r = NULL, *s = NULL;

    if (in == NULL || out == NULL || outlen == NULL || key == NULL ||
                                                                rng == NULL) {
        return ECC_BAD_ARG_E;
    }

    err = wc_ecc_alloc_async(key);
    if (err != 0) {
        return err;
    }
    r = key->r;
    s = key->s;

    switch (key->state) {
        case ECC_STATE_NONE:
        case ECC_STATE_SIGN_DO:
            key->state = ECC_STATE_SIGN_DO;

            if ((err = mp_init_multi(r, s, NULL, NULL, NULL, NULL)) != MP_OKAY){
                break;
            }

            err = wc_ecc_sign_hash_ex(in, inlen, rng, key, r, s);
            if (err < 0) {
                break;
            }

            FALL_THROUGH;

        case ECC_STATE_SIGN_ENCODE:
            key->state = ECC_STATE_SIGN_ENCODE;

            if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
                #if !defined(WOLFSSL_ASYNC_CRYPT_SW) && defined(HAVE_ECC_CDH)
                    DECLARE_CURVE_SPECS(1);
                    ALLOC_CURVE_SPECS(1, err);

                    /* get curve order */
                    err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ORDER);
                #endif

                #ifdef HAVE_CAVIUM_V
                    /* Nitrox requires r and s in sep buffer, so split it */
                    NitroxEccRsSplit(key, &r->raw, &s->raw);
                #endif
                #ifndef WOLFSSL_ASYNC_CRYPT_SW
                    /* only do this if not software, since it overwrites result */
                    wc_bigint_to_mp(&r->raw, r);
                    wc_bigint_to_mp(&s->raw, s);

                /* if using a curve with cofactor != 1 then reduce by mod order */
                #ifdef HAVE_ECC_CDH
                    /* if r is not less than order than reduce */
                    if (err == 0 && mp_count_bits(r) > mp_count_bits(curve->order)) {
                        err = mp_mod(r, curve->order, r);
                    }
                    wc_ecc_curve_free(curve);
                    FREE_CURVE_SPECS();
                #endif
                #endif /* !WOLFSSL_ASYNC_CRYPT_SW */
            }

            /* encoded with DSA header */
            if (err == 0) {
                err = StoreECC_DSA_Sig(out, outlen, r, s);
            }

            /* done with R/S */
            mp_clear(r);
            mp_clear(s);
            break;

        default:
            err = BAD_STATE_E;
            break;
    }

    /* if async pending then return and skip done cleanup below */
    if (err == WC_PENDING_E) {
        key->state++;
        return err;
    }

    /* cleanup */
    wc_ecc_free_async(key);
    key->state = ECC_STATE_NONE;

    return err;
}
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_ECC */

/**
 Sign a message digest
 in        The message digest to sign
 inlen     The length of the digest
 out       [out] The destination for the signature
 outlen    [in/out] The max size and resulting size of the signature
 key       A private ECC key
 return    MP_OKAY if successful
 */
WOLFSSL_ABI
int wc_ecc_sign_hash(const byte* in, word32 inlen, byte* out, word32 *outlen,
                     WC_RNG* rng, ecc_key* key)
{
    int err;
#if !defined(WOLFSSL_ASYNC_CRYPT) || !defined(WC_ASYNC_ENABLE_ECC)
    DECL_MP_INT_SIZE_DYN(r, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
    DECL_MP_INT_SIZE_DYN(s, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
#endif

    if (in == NULL || out == NULL || outlen == NULL || key == NULL) {
        return ECC_BAD_ARG_E;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (key->devId != INVALID_DEVID)
    #endif
    {
        err = wc_CryptoCb_EccSign(in, inlen, out, outlen, rng, key);
        #ifndef WOLF_CRYPTO_CB_ONLY_ECC
        if (err != CRYPTOCB_UNAVAILABLE)
            return err;
        /* fall-through when unavailable */
        #endif
        #ifdef WOLF_CRYPTO_CB_ONLY_ECC
        if (err == CRYPTOCB_UNAVAILABLE) {
            err = NO_VALID_DEVID;
        }
        #endif
    }
#endif

#ifndef WOLF_CRYPTO_CB_ONLY_ECC
    if (rng == NULL) {
        WOLFSSL_MSG("ECC sign RNG missing");
        return ECC_BAD_ARG_E;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    /* handle async cases */
    err = wc_ecc_sign_hash_async(in, inlen, out, outlen, rng, key);
#else

    NEW_MP_INT_SIZE(r, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (r == NULL)
        return MEMORY_E;
#endif
    NEW_MP_INT_SIZE(s, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (s == NULL) {
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return MEMORY_E;
    }
#endif

    err = INIT_MP_INT_SIZE(r, ECC_KEY_MAX_BITS(key));
    if (err != 0) {
        FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return err;
    }
    err = INIT_MP_INT_SIZE(s, ECC_KEY_MAX_BITS(key));
    if (err != 0) {
        FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return err;
    }

/* hardware crypto */
#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A) || \
    defined(PLUTON_CRYPTO_ECC) || defined(WOLFSSL_CRYPTOCELL) || \
    defined(WOLFSSL_SILABS_SE_ACCEL) || defined(WOLFSSL_KCAPI_ECC) || \
    defined(WOLFSSL_SE050) || defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    err = wc_ecc_sign_hash_hw(in, inlen, r, s, out, outlen, rng, key);
#else
    err = wc_ecc_sign_hash_ex(in, inlen, rng, key, r, s);
#endif
    if (err < 0) {
        mp_clear(r);
        mp_clear(s);
        FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return err;
    }

    /* encoded with DSA header */
    err = StoreECC_DSA_Sig(out, outlen, r, s);

    /* cleanup */
    mp_clear(r);
    mp_clear(s);

    FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
    FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
#endif /* WOLFSSL_ASYNC_CRYPT */
#else
    (void)rng;
    (void)inlen;
    (void)s;
    (void)r;
    (void)err;
#endif /* WOLF_CRYPTO_CB_ONLY_ECC */

    return err;
}
#endif /* !NO_ASN */

#if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
/* returns MP_OKAY on success */
static int deterministic_sign_helper(const byte* in, word32 inlen, ecc_key* key)
{
    int err = MP_OKAY;
    DECLARE_CURVE_SPECS(1);
    ALLOC_CURVE_SPECS(1, err);

    /* get curve order */
    if (err == MP_OKAY) {
        err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ORDER);
    }

    if (err == MP_OKAY) {
    #ifndef WOLFSSL_NO_MALLOC
        /* if key->sign_k is NULL then create a buffer for the mp_int
         * if not NULL then assume the user correctly set deterministic flag and
         *    that the key->sign_k holds a previously malloc'd mp_int buffer */
        if (key->sign_k == NULL) {
            key->sign_k = (mp_int*)XMALLOC(sizeof(mp_int), key->heap,
                                                            DYNAMIC_TYPE_ECC);
        }

        if (key->sign_k != NULL) {
            /* currently limiting to SHA256 for auto create */
            if (mp_init(key->sign_k) != MP_OKAY ||
                wc_ecc_gen_deterministic_k(in, inlen,
                        WC_HASH_TYPE_SHA256, key->k, key->sign_k,
                        curve->order, key->heap) != 0) {
                mp_free(key->sign_k);
                XFREE(key->sign_k, key->heap, DYNAMIC_TYPE_ECC);
                key->sign_k = NULL;
                err = ECC_PRIV_KEY_E;
            }
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            else {
                mp_memzero_add("deterministic_sign_helper sign_k", key->sign_k);
            }
        #endif
        }
        else {
            err = MEMORY_E;
        }
    #else
        key->sign_k_set = 0;
        /* currently limiting to SHA256 for auto create */
        if (wc_ecc_gen_deterministic_k(in, inlen, WC_HASH_TYPE_SHA256, key->k,
                key->sign_k, curve->order, key->heap) != 0) {
            err = ECC_PRIV_KEY_E;
        }
        else {
            key->sign_k_set = 1;
        }
    #endif
    }

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();
    return err;
}
#endif /* WOLFSSL_ECDSA_DETERMINISTIC_K ||
          WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT */

#if defined(WOLFSSL_STM32_PKA)
int wc_ecc_sign_hash_ex(const byte* in, word32 inlen, WC_RNG* rng,
                     ecc_key* key, mp_int *r, mp_int *s)
{
    return stm32_ecc_sign_hash_ex(in, inlen, rng, key, r, s);
}
#elif !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
      !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_KCAPI_ECC)
#ifndef WOLFSSL_SP_MATH
static int ecc_sign_hash_sw(ecc_key* key, ecc_key* pubkey, WC_RNG* rng,
                            ecc_curve_spec* curve, mp_int* e, mp_int* r,
                            mp_int* s)
{
    int err = MP_OKAY;
    int loop_check = 0;
    DECL_MP_INT_SIZE_DYN(b, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);

    NEW_MP_INT_SIZE(b, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (b == NULL)
        err = MEMORY_E;
#endif

    if (err == MP_OKAY) {
        err = INIT_MP_INT_SIZE(b, ECC_KEY_MAX_BITS(key));
    }

#ifdef WOLFSSL_CUSTOM_CURVES
    /* if custom curve, apply params to pubkey */
    if (err == MP_OKAY && key->idx == ECC_CUSTOM_IDX) {
        err = wc_ecc_set_custom_curve(pubkey, key->dp);
    }
#endif

    if (err == MP_OKAY) {
        /* Generate blinding value - non-zero value. */
        do {
            if (++loop_check > 64) {
                 err = RNG_FAILURE_E;
                 break;
            }

            err = wc_ecc_gen_k(rng, key->dp->size, b, curve->order);
        }
        while (err == MP_ZERO_E);
        loop_check = 0;
    }
#ifdef WOLFSSL_CHECK_MEM_ZERO
    if (err == MP_OKAY) {
        mp_memzero_add("ecc_sign_hash_sw b", b);
    }
#endif

    for (; err == MP_OKAY;) {
        if (++loop_check > 64) {
             err = RNG_FAILURE_E;
             break;
        }
#if defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP) || \
           defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
           defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
#ifndef WOLFSSL_NO_MALLOC
        if (key->sign_k != NULL)
#else
        if (key->sign_k_set)
#endif
        {
            if (loop_check > 1) {
               err = RNG_FAILURE_E;
               break;
            }

            /* use provided sign_k */
            err = mp_copy(key->sign_k, pubkey->k);
            if (err != MP_OKAY) break;

            /* free sign_k, so only used once */
            mp_forcezero(key->sign_k);
#ifndef WOLFSSL_NO_MALLOC
            mp_free(key->sign_k);
            XFREE(key->sign_k, key->heap, DYNAMIC_TYPE_ECC);
            key->sign_k = NULL;
#else
            key->sign_k_set = 0;
#endif
    #ifdef WOLFSSL_ECDSA_SET_K_ONE_LOOP
            loop_check = 64;
    #endif
    #if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
        defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
            if (key->deterministic == 1) {
                /* sign_k generated earlier in function for SP calls.
                 * Only go through the loop once and fail if error */
                loop_check = 64;
            }
    #endif

            /* compute public key based on provided "k" */
            err = ecc_make_pub_ex(pubkey, curve, NULL, rng);
        }
        else
#endif
        {
            err = _ecc_make_key_ex(rng, key->dp->size, pubkey, key->dp->id,
                    WC_ECC_FLAG_NONE);
        }
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        if (err == MP_OKAY) {
            mp_memzero_add("ecc_sign_hash_sw k", pubkey->k);
        }
    #endif
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* for async do blocking wait here */
        err = wc_AsyncWait(err, &pubkey->asyncDev, WC_ASYNC_FLAG_NONE);
    #endif
        if (err != MP_OKAY) break;

        /* find r = x1 mod n */
        err = mp_mod(pubkey->pubkey.x, curve->order, r);
        if (err != MP_OKAY) break;

        if (mp_iszero(r) == MP_NO) {
            mp_int* ep = pubkey->k;
            mp_int* kp = pubkey->k;
            mp_int* x  = key->k;

            /* find s = (e + xr)/k
                      = b.(e/k.b + x.r/k.b) */

            /* k' = k.b */
            err = mp_mulmod(pubkey->k, b, curve->order, kp);
            if (err != MP_OKAY) break;

            /* k' = 1/k.b
                  = 1/k' */
            err = mp_invmod(kp, curve->order, kp);
            if (err != MP_OKAY) break;

            /* s = x.r */
            err = mp_mulmod(x, r, curve->order, s);
            if (err != MP_OKAY) break;

            /* s = x.r/k.b
                 = k'.s */
            err = mp_mulmod(kp, s, curve->order, s);
            if (err != MP_OKAY) break;

            /* e' = e/k.b
                  = e.k' */
            err = mp_mulmod(kp, e, curve->order, ep);
            if (err != MP_OKAY) break;

            /* s = e/k.b + x.r/k.b = (e + x.r)/k.b
                 = e' + s */
            err = mp_addmod_ct(ep, s, curve->order, s);
            if (err != MP_OKAY) break;

            /* s = b.(e + x.r)/k.b = (e + x.r)/k
                 = b.s */
            err = mp_mulmod(s, b, curve->order, s);
            if (err != MP_OKAY) break;

            if (mp_iszero(s) == MP_NO) {
                /* sign successful */
                break;
            }
         }
     #ifndef ALT_ECC_SIZE
         mp_clear(pubkey->pubkey.x);
         mp_clear(pubkey->pubkey.y);
         mp_clear(pubkey->pubkey.z);
     #endif
         mp_forcezero(pubkey->k);
    }
    mp_forcezero(b);
    FREE_MP_INT_SIZE(b, key->heap, DYNAMIC_TYPE_ECC);
#if !defined(WOLFSSL_SMALL_STACK) && defined(WOLFSSL_CHECK_MEM_ZERO)
    mp_memzero_check(b);
#endif

    return err;
}
#endif

#ifdef WOLFSSL_HAVE_SP_ECC
static int ecc_sign_hash_sp(const byte* in, word32 inlen, WC_RNG* rng,
    ecc_key* key, mp_int *r, mp_int *s)
{
    if (key->idx != ECC_CUSTOM_IDX) {
    #if defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP) \
        || defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
           defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
        mp_int* sign_k = key->sign_k;
    #else
        mp_int* sign_k = NULL;
    #endif
    #if defined(WC_ECC_NONBLOCK) && defined(WC_ECC_NONBLOCK_ONLY)
        /* perform blocking call to non-blocking function */
        ecc_nb_ctx_t nb_ctx;
        XMEMSET(&nb_ctx, 0, sizeof(nb_ctx));
    #endif
    #ifndef WOLFSSL_SP_NO_256
        if (ecc_sets[key->idx].id == ECC_SECP256R1) {
        #ifdef WC_ECC_NONBLOCK
            #ifdef WC_ECC_NONBLOCK_ONLY
            int err;
            #endif
            if (key->nb_ctx) {
                return sp_ecc_sign_256_nb(&key->nb_ctx->sp_ctx, in, inlen, rng,
                    key->k, r, s, sign_k, key->heap);
            }
            #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_sign_256_nb(&nb_ctx.sp_ctx, in, inlen, rng,
                    key->k, r, s, sign_k, key->heap);
            } while (err == FP_WOULDBLOCK);
            return err;
            #endif
        #endif /* WC_ECC_NONBLOCK */
        #if !defined(WC_ECC_NONBLOCK) || (defined(WC_ECC_NONBLOCK) && !defined(WC_ECC_NONBLOCK_ONLY))
            {
                int ret;
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
                ret = sp_ecc_sign_256(in, inlen, rng, key->k, r, s, sign_k,
                                      key->heap);
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        #endif
        }
#ifdef SM2_SP_IMPL_AVAILABLE
        #ifdef WOLFSSL_SM2
        if (ecc_sets[key->idx].id == ECC_SM2P256V1) {
            return sp_ecc_sign_sm2_256(in, inlen, rng, &key->k, r, s, sign_k,
                key->heap);
        }
        #endif
#endif
    #endif
    #ifdef WOLFSSL_SP_384
        if (ecc_sets[key->idx].id == ECC_SECP384R1) {
        #ifdef WC_ECC_NONBLOCK
            #ifdef WC_ECC_NONBLOCK_ONLY
            int err;
            #endif
            if (key->nb_ctx) {
                return sp_ecc_sign_384_nb(&key->nb_ctx->sp_ctx, in, inlen, rng,
                    key->k, r, s, sign_k, key->heap);
            }
            #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_sign_384_nb(&nb_ctx.sp_ctx, in, inlen, rng,
                    key->k, r, s, sign_k, key->heap);
            } while (err == FP_WOULDBLOCK);
            return err;
            #endif
        #endif /* WC_ECC_NONBLOCK */
        #if !defined(WC_ECC_NONBLOCK) || (defined(WC_ECC_NONBLOCK) && !defined(WC_ECC_NONBLOCK_ONLY))
            {
                int ret;
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
                ret = sp_ecc_sign_384(in, inlen, rng, key->k, r, s, sign_k,
                                      key->heap);
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        #endif
        }
    #endif
    #ifdef WOLFSSL_SP_521
        if (ecc_sets[key->idx].id == ECC_SECP521R1) {
        #ifdef WC_ECC_NONBLOCK
            #ifdef WC_ECC_NONBLOCK_ONLY
            int err;
            #endif
            if (key->nb_ctx) {
                return sp_ecc_sign_521_nb(&key->nb_ctx->sp_ctx, in, inlen, rng,
                    key->k, r, s, sign_k, key->heap);
            }
            #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_sign_521_nb(&nb_ctx.sp_ctx, in, inlen, rng,
                    key->k, r, s, sign_k, key->heap);
            } while (err == FP_WOULDBLOCK);
            return err;
            #endif
        #endif /* WC_ECC_NONBLOCK */
        #if !defined(WC_ECC_NONBLOCK) || (defined(WC_ECC_NONBLOCK) && !defined(WC_ECC_NONBLOCK_ONLY))
            {
                int ret;
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
                ret = sp_ecc_sign_521(in, inlen, rng, key->k, r, s, sign_k,
                                      key->heap);
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        #endif
        }
    #endif
        (void)sign_k;
    }

    /* SP doesn't support curve. */
    return WC_KEY_SIZE_E;
}
#endif

/**
  Sign a message digest
  in        The message digest to sign
  inlen     The length of the digest
  key       A private ECC key
  r         [out] The destination for r component of the signature
  s         [out] The destination for s component of the signature
  return    MP_OKAY if successful
*/
int wc_ecc_sign_hash_ex(const byte* in, word32 inlen, WC_RNG* rng,
                     ecc_key* key, mp_int *r, mp_int *s)
{
   int    err = 0;
#if !defined(WOLFSSL_SP_MATH)
   mp_int* e;
#if !defined(WOLFSSL_ASYNC_CRYPT) || !defined(HAVE_CAVIUM_V)
   DECL_MP_INT_SIZE_DYN(e_lcl, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
#endif

#if defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT) || \
    (defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC) && \
    (defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA)))
   DECLARE_CURVE_SPECS(ECC_CURVE_FIELD_COUNT);
#else
   DECLARE_CURVE_SPECS(1);
#endif
#endif /* !WOLFSSL_SP_MATH */

   if (in == NULL || r == NULL || s == NULL || key == NULL || rng == NULL) {
       return ECC_BAD_ARG_E;
   }

   /* is this a private key? */
   if (key->type != ECC_PRIVATEKEY && key->type != ECC_PRIVATEKEY_ONLY) {
      return ECC_BAD_ARG_E;
   }

   /* is the IDX valid ?  */
   if (wc_ecc_is_valid_idx(key->idx) == 0 || key->dp == NULL) {
      return ECC_BAD_ARG_E;
   }

#if defined(WOLFSSL_SP_MATH)
    if (key->idx == ECC_CUSTOM_IDX || (1
    #ifndef WOLFSSL_SP_NO_256
         && ecc_sets[key->idx].id != ECC_SECP256R1
    #endif
    #ifdef WOLFSSL_SP_384
         && ecc_sets[key->idx].id != ECC_SECP384R1
    #endif
    #ifdef WOLFSSL_SP_521
         && ecc_sets[key->idx].id != ECC_SECP521R1
    #endif
        )) {
        return WC_KEY_SIZE_E;
    }
#endif

#if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
    /* generate deterministic 'k' value to be used either with SP or normal */
    if (key->deterministic == 1) {
        if (deterministic_sign_helper(in, inlen, key)) {
            WOLFSSL_MSG("Error generating deterministic k to sign");
            return ECC_PRIV_KEY_E;
        }
    }
#endif


#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC) && \
       defined(WOLFSSL_ASYNC_CRYPT_SW)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
        if (wc_AsyncSwInit(&key->asyncDev, ASYNC_SW_ECC_SIGN)) {
            WC_ASYNC_SW* sw = &key->asyncDev.sw;
            sw->eccSign.in = in;
            sw->eccSign.inSz = inlen;
            sw->eccSign.rng = rng;
            sw->eccSign.key = key;
            sw->eccSign.r = r;
            sw->eccSign.s = s;
            return WC_PENDING_E;
        }
    }
#endif

#if defined(WOLFSSL_HAVE_SP_ECC)
   err = ecc_sign_hash_sp(in, inlen, rng, key, r, s);
   if (err != WC_KEY_SIZE_E) {
       return err;
   }
#else
   (void)inlen;
#endif

#if !defined(WOLFSSL_SP_MATH)

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(HAVE_CAVIUM_V)
   err = wc_ecc_alloc_mpint(key, &key->e);
   if (err != 0) {
      return err;
   }
   e = key->e;
#else
   NEW_MP_INT_SIZE(e_lcl, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
   if (e_lcl == NULL) {
      return MEMORY_E;
   }
#endif
   e = e_lcl;
#endif

   /* get the hash and load it as a bignum into 'e' */
   /* init the bignums */
   if ((err = INIT_MP_INT_SIZE(e, ECC_KEY_MAX_BITS(key))) != MP_OKAY) {
      FREE_MP_INT_SIZE(e_lcl, key->heap, DYNAMIC_TYPE_ECC);
      return err;
   }

   /* load curve info */
#if defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
    ALLOC_CURVE_SPECS(ECC_CURVE_FIELD_COUNT, err);
    if (err == MP_OKAY)
        err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
#else
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC) && \
      (defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA))
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
        ALLOC_CURVE_SPECS(ECC_CURVE_FIELD_COUNT, err);
        if (err == MP_OKAY)
            err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
    }
    else
    #endif
    {
        ALLOC_CURVE_SPECS(1, err);
        if (err == MP_OKAY)
            err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ORDER);
    }
#endif

   /* load digest into e */
   if (err == MP_OKAY) {
       /* we may need to truncate if hash is longer than key size */
       word32 orderBits = (word32)mp_count_bits(curve->order);

       /* truncate down to byte size, may be all that's needed */
       if ((WOLFSSL_BIT_SIZE * inlen) > orderBits)
           inlen = (orderBits + WOLFSSL_BIT_SIZE - 1) / WOLFSSL_BIT_SIZE;
       err = mp_read_unsigned_bin(e, in, inlen);

       /* may still need bit truncation too */
       if (err == MP_OKAY && (WOLFSSL_BIT_SIZE * inlen) > orderBits)
           mp_rshb(e, (int)(WOLFSSL_BIT_SIZE - (orderBits & 0x7)));
   }

   /* make up a key and export the public copy */
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
   if ((err == MP_OKAY) && (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC)) {
   #if defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA)
   #ifdef HAVE_CAVIUM_V
       if (NitroxEccIsCurveSupported(key))
   #endif
       {
           word32 keySz = key->dp->size;
           mp_int* k;
       #ifdef HAVE_CAVIUM_V
           err = wc_ecc_alloc_mpint(key, &key->signK);
           if (err != 0)
              return err;
           k = key->signK;
       #else
           mp_int k_lcl;
           k = &k_lcl;
       #endif

           err = mp_init(k);

            /* make sure r and s are allocated */
       #ifdef HAVE_CAVIUM_V
           /* Nitrox V needs single buffer for R and S */
           if (err == MP_OKAY)
               err = wc_bigint_alloc(&key->r->raw, NitroxEccGetSize(key)*2);
           /* Nitrox V only needs Prime and Order */
           if (err == MP_OKAY)
               err = wc_ecc_curve_load(key->dp, &curve,
                    (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_ORDER));
       #else
           if (err == MP_OKAY)
               err = wc_bigint_alloc(&key->r->raw, key->dp->size);
           if (err == MP_OKAY)
               err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
       #endif
           if (err == MP_OKAY)
               err = wc_bigint_alloc(&key->s->raw, key->dp->size);

           /* load e and k */
           if (err == MP_OKAY)
               err = wc_mp_to_bigint_sz(e, &e->raw, keySz);
           if (err == MP_OKAY)
               err = wc_mp_to_bigint_sz(key->k, &key->k->raw, keySz);
           if (err == MP_OKAY)
               err = wc_ecc_gen_k(rng, key->dp->size, k, curve->order);
           if (err == MP_OKAY)
               err = wc_mp_to_bigint_sz(k, &k->raw, keySz);

       #ifdef HAVE_CAVIUM_V
           if (err == MP_OKAY)
               err = NitroxEcdsaSign(key, &e->raw, &key->k->raw, &k->raw,
                &r->raw, &s->raw, &curve->prime->raw, &curve->order->raw);
       #else
           if (err == MP_OKAY)
               err = IntelQaEcdsaSign(&key->asyncDev, &e->raw, &key->k->raw,
                  &k->raw, &r->raw, &s->raw, &curve->Af->raw, &curve->Bf->raw,
                  &curve->prime->raw, &curve->order->raw, &curve->Gx->raw,
                  &curve->Gy->raw);
       #endif

       #ifndef HAVE_CAVIUM_V
           mp_clear(e);
           mp_clear(k);
       #endif
           wc_ecc_curve_free(curve);
           FREE_CURVE_SPECS();

           return err;
       }
   #endif /* HAVE_CAVIUM_V || HAVE_INTEL_QA */
   }
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_ECC */

   if (err == MP_OKAY) {
   #ifdef WOLFSSL_SMALL_STACK
       ecc_key* pubkey;
   #else
       ecc_key  pubkey[1];
   #endif

   #ifdef WOLFSSL_SMALL_STACK
       pubkey = (ecc_key*)XMALLOC(sizeof(ecc_key), key->heap, DYNAMIC_TYPE_ECC);
       if (pubkey == NULL)
           err = MEMORY_E;
   #endif

       /* don't use async for key, since we don't support async return here */
       if (err == MP_OKAY) {
           err = wc_ecc_init_ex(pubkey, key->heap, INVALID_DEVID);
           if (err == MP_OKAY) {
              err = ecc_sign_hash_sw(key, pubkey, rng, curve, e, r, s);
              wc_ecc_free(pubkey);
           #ifdef WOLFSSL_SMALL_STACK
              XFREE(pubkey, key->heap, DYNAMIC_TYPE_ECC);
           #endif
           }
       }
   }

   mp_clear(e);
   wc_ecc_curve_free(curve);
   FREE_MP_INT_SIZE(e_lcl, key->heap, DYNAMIC_TYPE_ECC);
   FREE_CURVE_SPECS();
#endif /* !WOLFSSL_SP_MATH */

   return err;
}

#if defined(WOLFSSL_ECDSA_DETERMINISTIC_K) || \
    defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
/* helper function to do HMAC operations
 * returns 0 on success and updates "out" buffer
 */
static int _HMAC_K(byte* K, word32 KSz, byte* V, word32 VSz,
        const byte* h1, word32 h1Sz, byte* x, word32 xSz, byte* oct,
        byte* out, enum wc_HashType hashType, void* heap)
{
    Hmac hmac;
    int  ret, init;

    ret = init = wc_HmacInit(&hmac, heap, 0);
    if (ret == 0)
        ret = wc_HmacSetKey(&hmac, hashType, K, KSz);

    if (ret == 0)
        ret = wc_HmacUpdate(&hmac, V, VSz);

    if (ret == 0 && oct != NULL)
        ret = wc_HmacUpdate(&hmac, oct, 1);

    if (ret == 0)
        ret = wc_HmacUpdate(&hmac, x, xSz);

    if (ret == 0)
        ret = wc_HmacUpdate(&hmac, h1, h1Sz);

    if (ret == 0)
        ret = wc_HmacFinal(&hmac, out);

    if (init == 0)
        wc_HmacFree(&hmac);

    return ret;
}


/* Generates a deterministic key based of the message using RFC6979
 * @param  [in]   hash     Hash value to sign
 * @param  [in]   hashSz   Size of 'hash' buffer passed in
 * @param  [in]   hashType Type of hash to use with deterministic k gen, i.e.
 *                WC_HASH_TYPE_SHA256
 * @param  [in]   priv     Current ECC private key set
 * @param  [out]  k        An initialized mp_int to set the k value generated in
 * @param  [in]   order    ECC order parameter to use with generation
 * @return  0 on success.
 */
int wc_ecc_gen_deterministic_k(const byte* hash, word32 hashSz,
        enum wc_HashType hashType, mp_int* priv, mp_int* k, mp_int* order,
        void* heap)
{
    int ret = 0, qbits = 0;
#ifndef WOLFSSL_SMALL_STACK
    byte h1[MAX_ECC_BYTES];
    byte V[WC_MAX_DIGEST_SIZE];
    byte K[WC_MAX_DIGEST_SIZE];
    byte x[MAX_ECC_BYTES];
    mp_int z1[1];
#else
    byte *h1 = NULL;
    byte *V  = NULL;
    byte *K  = NULL;
    byte *x  = NULL;
    mp_int *z1 = NULL;
#endif
    word32 xSz, VSz, KSz, h1len, qLen;
    byte intOct;

    if (hash == NULL || k == NULL || order == NULL) {
        return BAD_FUNC_ARG;
    }

    if (hashSz > WC_MAX_DIGEST_SIZE) {
        WOLFSSL_MSG("hash size was too large!");
        return BAD_FUNC_ARG;
    }

    if (hashSz != WC_SHA256_DIGEST_SIZE) {
        WOLFSSL_MSG("Currently only SHA256 digest is supported");
        return BAD_FUNC_ARG;
    }

    if (mp_unsigned_bin_size(priv) > MAX_ECC_BYTES) {
        WOLFSSL_MSG("private key larger than max expected!");
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    h1 = (byte*)XMALLOC(MAX_ECC_BYTES, heap, DYNAMIC_TYPE_DIGEST);
    if (h1 == NULL) {
        ret = MEMORY_E;
    }

    if (ret == 0) {
        V = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, heap, DYNAMIC_TYPE_ECC_BUFFER);
        if (V == NULL)
            ret = MEMORY_E;
    }

    if (ret == 0) {
        K = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, heap, DYNAMIC_TYPE_ECC_BUFFER);
        if (K == NULL)
            ret = MEMORY_E;
    }

    if (ret == 0) {
        x = (byte*)XMALLOC(MAX_ECC_BYTES, heap, DYNAMIC_TYPE_PRIVATE_KEY);
        if (x == NULL)
            ret = MEMORY_E;
    }

    if (ret == 0) {
        z1 = (mp_int *)XMALLOC(sizeof(*z1), heap, DYNAMIC_TYPE_ECC_BUFFER);
        if (z1 == NULL)
            ret = MEMORY_E;
    }

    /* bail out if any error has been hit at this point */
    if (ret != 0) {
        if (x != NULL)
            XFREE(x, heap, DYNAMIC_TYPE_PRIVATE_KEY);
        if (K != NULL)
            XFREE(K, heap, DYNAMIC_TYPE_ECC_BUFFER);
        if (V != NULL)
            XFREE(V, heap, DYNAMIC_TYPE_ECC_BUFFER);
        if (h1 != NULL)
            XFREE(h1, heap, DYNAMIC_TYPE_DIGEST);
        return ret;
    }
#endif

    VSz = KSz = hashSz;
    qLen = xSz = h1len = (word32)mp_unsigned_bin_size(order);

    /* 3.2 b. Set V = 0x01 0x01 ... */
    XMEMSET(V, 0x01, VSz);

    /* 3.2 c. Set K = 0x00 0x00 ... */
    XMEMSET(K, 0x00, KSz);

    mp_init(z1); /* always init z1 and free z1 */
    ret = mp_to_unsigned_bin_len(priv, x, (int)qLen);
    if (ret == 0) {
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Add("wc_ecc_gen_deterministic_k x", x, qLen);
    #endif
        qbits = mp_count_bits(order);
        ret = mp_read_unsigned_bin(z1, hash, hashSz);
    }

    /* bits2octets on h1 */
    if (ret == 0) {
        XMEMSET(h1, 0, MAX_ECC_BYTES);

    #if !defined(WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT)
        /* mod reduce by order using conditional subtract
         * RFC6979 lists a variant that uses the hash directly instead of
         * doing bits2octets(H(m)), when variant macro is used avoid this
         * bits2octets operation */
        if (mp_cmp(z1, order) == MP_GT) {
            int z1Sz;

            mp_sub(z1, order, z1);
            z1Sz = mp_unsigned_bin_size(z1);
            if (z1Sz < 0 || z1Sz > MAX_ECC_BYTES) {
                ret = BUFFER_E;
            }
            else {
                ret = mp_to_unsigned_bin_len(z1, h1, h1len);
            }
        }
        else
    #endif
        {
            /* use original hash and keep leading 0's */
            mp_to_unsigned_bin_len(z1, h1, (int)h1len);
        }
    }
    mp_free(z1);

    /* 3.2 step d. K = HMAC_K(V || 0x00 || int2octests(x) || bits2octests(h1) */
    if (ret == 0) {
        intOct = 0x00;
        ret = _HMAC_K(K, KSz, V, VSz, h1, h1len, x, xSz, &intOct, K,
                hashType, heap);
    }

    /* 3.2 step e. V = HMAC_K(V) */
    if (ret == 0) {
        ret = _HMAC_K(K, KSz, V, VSz, NULL, 0, NULL, 0, NULL, V, hashType,
                heap);
    }


    /* 3.2 step f. K = HMAC_K(V || 0x01 || int2octests(x) || bits2octests(h1) */
    if (ret == 0) {
        intOct = 0x01;
        ret = _HMAC_K(K, KSz, V, VSz, h1, h1len, x, xSz, &intOct, K, hashType,
                heap);
    }

    /* 3.2 step g. V = HMAC_K(V) */
    if (ret == 0) {
        ret = _HMAC_K(K, KSz, V, VSz, NULL, 0, NULL, 0, NULL, V, hashType,
                heap);
    }

    /* 3.2 step h. loop through the next steps until a valid value is found */
    if (ret == 0 ) {
        int err;

        intOct = 0x00;
        do {
            xSz = 0; /* used as tLen */
            err = 0; /* start as good until generated k is tested */

            /* 3.2 step h.2 when tlen < qlen do V = HMAC_K(V); T = T || V */
            while (xSz < qLen) {
                ret = _HMAC_K(K, KSz, V, VSz, NULL, 0, NULL, 0, NULL, V,
                        hashType, heap);
                if (ret == 0) {
                    int sz;

                    sz = (int)MIN(qLen - xSz, (size_t)VSz);
                    XMEMCPY(x + xSz, V, (size_t)sz);
                    xSz += (word32)sz;
                }
                else {
                    break; /* error case */
                }
            }

            if (ret == 0) {
                mp_clear(k); /* 3.2 step h.1 clear T */
                ret = mp_read_unsigned_bin(k, x, xSz);
            }

            if ((ret == 0) && ((int)(xSz * WOLFSSL_BIT_SIZE) != qbits)) {
                /* handle odd case where shift of 'k' is needed with RFC 6979
                 *  k = bits2int(T) in section 3.2 h.3 */
                mp_rshb(k, ((int)xSz * WOLFSSL_BIT_SIZE) - qbits);
            }

            /* 3.2 step h.3 the key should be smaller than the order of base
             * point */
            if (ret == 0) {
                if (mp_cmp(k, order) != MP_LT) {
                    err = MP_VAL;
                } else if (mp_iszero(k) == MP_YES) {
                    /* no 0 key's */
                    err = MP_ZERO_E;
                }
            }

            /* 3.2 step h.3 if there was a problem with 'k' generated then try
             * again K = HMAC_K(V || 0x00) and V = HMAC_K(V) */
            if (ret == 0 && err != 0) {
                ret = _HMAC_K(K, KSz, V, VSz, NULL, 0, NULL, 0, &intOct, K,
                    hashType, heap);
                if (ret == 0) {
                    ret = _HMAC_K(K, KSz, V, VSz, NULL, 0, NULL, 0, NULL, V,
                    hashType, heap);
                }
            }
        } while (ret == 0 && err != 0);
    }

    ForceZero(x, MAX_ECC_BYTES);
#ifdef WOLFSSL_SMALL_STACK
    if (z1 != NULL)
        XFREE(z1, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (x != NULL)
        XFREE(x, heap, DYNAMIC_TYPE_PRIVATE_KEY);
    if (K != NULL)
        XFREE(K, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (V != NULL)
        XFREE(V, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (h1 != NULL)
        XFREE(h1, heap, DYNAMIC_TYPE_DIGEST);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
    wc_MemZero_Check(x, MAX_ECC_BYTES);
#endif

    return ret;
}


/* Sets the deterministic flag for 'k' generation with sign.
 * returns 0 on success
 */
int wc_ecc_set_deterministic(ecc_key* key, byte flag)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    key->deterministic = flag ? 1 : 0;
    return 0;
}
#endif /* end sign_ex and deterministic sign */


#if defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP)
int wc_ecc_sign_set_k(const byte* k, word32 klen, ecc_key* key)
{
    int ret = MP_OKAY;
    DECLARE_CURVE_SPECS(1);

    if (k == NULL || klen == 0 || key == NULL) {
        return BAD_FUNC_ARG;
    }

    ALLOC_CURVE_SPECS(1, ret);
    if (ret == MP_OKAY) {
        ret = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ORDER);
    }

    if (ret != 0) {
        FREE_CURVE_SPECS();
        return ret;
    }

#ifndef WOLFSSL_NO_MALLOC
    if (key->sign_k == NULL) {
        key->sign_k = (mp_int*)XMALLOC(sizeof(mp_int), key->heap,
                                                            DYNAMIC_TYPE_ECC);
        if (key->sign_k) {
            ret = mp_init(key->sign_k);
        }
        else {
            ret = MEMORY_E;
        }
    }
#endif

    if (ret == 0) {
        ret = mp_read_unsigned_bin(key->sign_k, k, klen);
    }
    if (ret == 0 && mp_cmp(key->sign_k, curve->order) != MP_LT) {
        ret = MP_VAL;
    }
#ifdef WOLFSSL_NO_MALLOC
    if (ret == 0) {
        key->sign_k_set = 1;
    }
#endif

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();
    return ret;
}
#endif /* WOLFSSL_ECDSA_SET_K || WOLFSSL_ECDSA_SET_K_ONE_LOOP */
#endif /* WOLFSSL_ATECC508A && WOLFSSL_CRYPTOCELL */

#endif /* !HAVE_ECC_SIGN */

#ifdef WOLFSSL_CUSTOM_CURVES
void wc_ecc_free_curve(const ecc_set_type* curve, void* heap)
{
#ifndef WOLFSSL_ECC_CURVE_STATIC
    if (curve->prime != NULL)
        XFREE((void*)curve->prime, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (curve->Af != NULL)
        XFREE((void*)curve->Af, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (curve->Bf != NULL)
        XFREE((void*)curve->Bf, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (curve->order != NULL)
        XFREE((void*)curve->order, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (curve->Gx != NULL)
        XFREE((void*)curve->Gx, heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (curve->Gy != NULL)
        XFREE((void*)curve->Gy, heap, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    XFREE((void*)curve, heap, DYNAMIC_TYPE_ECC_BUFFER);

    (void)heap;
}
#endif /* WOLFSSL_CUSTOM_CURVES */

/**
  Free an ECC key from memory
  key   The key you wish to free
*/
WOLFSSL_ABI
int wc_ecc_free(ecc_key* key)
{
    if (key == NULL) {
        return 0;
    }

#if defined(WOLFSSL_ECDSA_SET_K) || defined(WOLFSSL_ECDSA_SET_K_ONE_LOOP)
#ifndef WOLFSSL_NO_MALLOC
    if (key->sign_k != NULL)
#endif
    {
        mp_forcezero(key->sign_k);
        mp_free(key->sign_k);
#ifndef WOLFSSL_NO_MALLOC
        XFREE(key->sign_k, key->heap, DYNAMIC_TYPE_ECC);
#endif
    }
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    #ifdef WC_ASYNC_ENABLE_ECC
    wolfAsync_DevCtxFree(&key->asyncDev, WOLFSSL_ASYNC_MARKER_ECC);
    #endif
    wc_ecc_free_async(key);
#endif

#if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    /* free secure memory */
    if ((key->blackKey != CAAM_BLACK_KEY_CCM &&
         key->blackKey != CAAM_BLACK_KEY_ECB) && key->blackKey > 0) {
       caamFreePart(key->partNum);
    }
#endif

#ifdef WOLFSSL_SE050
    se050_ecc_free_key(key);
#endif

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
    atmel_ecc_free(key->slot);
    key->slot = ATECC_INVALID_SLOT;
#endif /* WOLFSSL_ATECC508A */

#ifdef WOLFSSL_KCAPI_ECC
    KcapiEcc_Free(key);
#endif

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    key->privKey = NULL;
    ForceZero(key->keyRaw, sizeof(key->keyRaw));
    ForceZero(&key->xSec, sizeof(key->xSec));
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    wc_MAXQ10XX_EccFree(key);
#endif

    mp_clear(key->pubkey.x);
    mp_clear(key->pubkey.y);
    mp_clear(key->pubkey.z);

#ifdef ALT_ECC_SIZE
    if (key->k)
#endif
        mp_forcezero(key->k);

#ifdef WOLFSSL_CUSTOM_CURVES
    if (key->deallocSet && key->dp != NULL)
        wc_ecc_free_curve(key->dp, key->heap);
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(key, sizeof(ecc_key));
#endif

    return 0;
}

#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SP_MATH) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC)
/* Handles add failure cases:
 *
 * Before add:
 *   Case 1: A is infinity
 *        -> Copy B into result.
 *   Case 2: B is infinity
 *        -> Copy A into result.
 *   Case 3: x and z are the same in A and B (same x value in affine)
 *     Case 3a: y values the same - same point
 *           -> Double instead of add.
 *     Case 3b: y values different - negative of the other when points on curve
 *           -> Need to set result to infinity.
 *
 * After add:
 *   Case 1: A and B are the same point (maybe different z)
 *           (Result was: x == y == z == 0)
 *        -> Need to double instead.
 *
 *   Case 2: A + B = <infinity> = 0.
 *           (Result was: z == 0, x and/or y not 0)
 *        -> Need to set result to infinity.
 */
int ecc_projective_add_point_safe(ecc_point* A, ecc_point* B, ecc_point* R,
    mp_int* a, mp_int* modulus, mp_digit mp, int* infinity)
{
    int err;

    if (mp_iszero(A->x) && mp_iszero(A->y)) {
        /* A is infinity. */
        err = wc_ecc_copy_point(B, R);
    }
    else if (mp_iszero(B->x) && mp_iszero(B->y)) {
        /* B is infinity. */
        err = wc_ecc_copy_point(A, R);
    }
    else if ((mp_cmp(A->x, B->x) == MP_EQ) && (mp_cmp(A->z, B->z) == MP_EQ)) {
        /* x ordinattes the same. */
        if (mp_cmp(A->y, B->y) == MP_EQ) {
            /* A = B */
            err = _ecc_projective_dbl_point(B, R, a, modulus, mp);
        }
        else {
            /* A = -B */
            err = mp_set(R->x, 0);
            if (err == MP_OKAY)
                err = mp_set(R->y, 0);
            if (err == MP_OKAY)
                err = mp_set(R->z, 1);
            if ((err == MP_OKAY) && (infinity != NULL))
                *infinity = 1;
        }
    }
    else {
        err = _ecc_projective_add_point(A, B, R, a, modulus, mp);
        if ((err == MP_OKAY) && mp_iszero(R->z)) {
            /* When all zero then should have done a double */
            if (mp_iszero(R->x) && mp_iszero(R->y)) {
                if (mp_iszero(B->z)) {
                    err = wc_ecc_copy_point(B, R);
                    if (err == MP_OKAY) {
                        err = mp_montgomery_calc_normalization(R->z, modulus);
                    }
                    if (err == MP_OKAY) {
                        err = _ecc_projective_dbl_point(R, R, a, modulus, mp);
                    }
                }
                else {
                    err = _ecc_projective_dbl_point(B, R, a, modulus, mp);
                }
            }
            /* When only Z zero then result is infinity */
            else {
                err = mp_set(R->x, 0);
                if (err == MP_OKAY)
                    err = mp_set(R->y, 0);
                if (err == MP_OKAY)
                    err = mp_set(R->z, 1);
                if ((err == MP_OKAY) && (infinity != NULL))
                    *infinity = 1;
            }
        }
    }

    return err;
}

/* Handles when P is the infinity point.
 *
 * Double infinity -> infinity.
 * Otherwise do normal double - which can't lead to infinity as odd order.
 */
int ecc_projective_dbl_point_safe(ecc_point *P, ecc_point *R, mp_int* a,
                                  mp_int* modulus, mp_digit mp)
{
    int err;

    if (mp_iszero(P->x) && mp_iszero(P->y)) {
        /* P is infinity. */
        err = wc_ecc_copy_point(P, R);
    }
    else {
        err = _ecc_projective_dbl_point(P, R, a, modulus, mp);
    }

    return err;
}
#endif /* !WOLFSSL_ATECC508A && !WOLFSSL_ATECC608A
          && !WOLFSSL_CRYPTOCELL && !WOLFSSL_SP_MATH */

#if !defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_ATECC508A) && \
    !defined(WOLFSSL_ATECC608A) && !defined(WOLFSSL_CRYPTOCELL) && \
    !defined(WOLFSSL_KCAPI_ECC) && !defined(WOLF_CRYPTO_CB_ONLY_ECC)
#ifdef ECC_SHAMIR

static int ecc_mont_norm_points(ecc_point* A, ecc_point* Am, ecc_point* B,
    ecc_point* Bm, mp_int* modulus, void* heap)
{
    int err = MP_OKAY;
    DECL_MP_INT_SIZE_DYN(mu, mp_bitsused(modulus), MAX_ECC_BITS_USE);

    (void)heap;

    NEW_MP_INT_SIZE(mu, mp_bitsused(modulus), heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (mu == NULL)
       err = MEMORY_E;
#endif
    if (err == MP_OKAY) {
        err = INIT_MP_INT_SIZE(mu, mp_bitsused(modulus));
    }
    if (err == MP_OKAY) {
        err = mp_montgomery_calc_normalization(mu, modulus);

        if (err == MP_OKAY) {
            /* copy ones ... */
            err = mp_mulmod(A->x, mu, modulus, Am->x);
        }

        if (err == MP_OKAY)
            err = mp_mulmod(A->y, mu, modulus, Am->y);
        if (err == MP_OKAY)
            err = mp_mulmod(A->z, mu, modulus, Am->z);

        if (err == MP_OKAY)
            err = mp_mulmod(B->x, mu, modulus, Bm->x);
        if (err == MP_OKAY)
            err = mp_mulmod(B->y, mu, modulus, Bm->y);
        if (err == MP_OKAY)
            err = mp_mulmod(B->z, mu, modulus, Bm->z);

        /* done with mu */
        mp_clear(mu);
    }

    FREE_MP_INT_SIZE(mu, heap, DYNAMIC_TYPE_ECC);

    return err;
}

/** Computes kA*A + kB*B = C using Shamir's Trick
  A        First point to multiply
  kA       What to multiple A by
  B        Second point to multiply
  kB       What to multiple B by
  C        [out] Destination point (can overlap with A or B)
  a        ECC curve parameter a
  modulus  Modulus for curve
  return MP_OKAY on success
*/
#ifdef FP_ECC
static int normal_ecc_mul2add(ecc_point* A, mp_int* kA,
                             ecc_point* B, mp_int* kB,
                             ecc_point* C, mp_int* a, mp_int* modulus,
                             void* heap)
#else
int ecc_mul2add(ecc_point* A, mp_int* kA,
                    ecc_point* B, mp_int* kB,
                    ecc_point* C, mp_int* a, mp_int* modulus,
                    void* heap)
#endif
{
#ifdef WOLFSSL_SMALL_STACK_CACHE
  ecc_key        *key = NULL;
#endif
#ifdef WOLFSSL_SMALL_STACK
  ecc_point**    precomp = NULL;
#else
  ecc_point*     precomp[SHAMIR_PRECOMP_SZ];
  #ifdef WOLFSSL_NO_MALLOC
  ecc_point      lcl_precomp[SHAMIR_PRECOMP_SZ];
  #endif
#endif
  unsigned int  bitbufA, bitbufB, lenA, lenB, len, nA, nB, nibble;
#ifdef WOLFSSL_NO_MALLOC
  unsigned char tA[ECC_BUFSIZE];
  unsigned char tB[ECC_BUFSIZE];
#else
  unsigned char* tA = NULL;
  unsigned char* tB = NULL;
#endif
  int            err = MP_OKAY, first, x, y;
  mp_digit       mp = 0;

  /* argchks */
  if (A == NULL || kA == NULL || B == NULL || kB == NULL || C == NULL ||
                                                         modulus == NULL) {
     return ECC_BAD_ARG_E;
  }

#ifndef WOLFSSL_NO_MALLOC
  /* allocate memory */
  tA = (unsigned char*)XMALLOC(ECC_BUFSIZE, heap, DYNAMIC_TYPE_ECC_BUFFER);
  if (tA == NULL) {
     return GEN_MEM_ERR;
  }
  tB = (unsigned char*)XMALLOC(ECC_BUFSIZE, heap, DYNAMIC_TYPE_ECC_BUFFER);
  if (tB == NULL) {
     XFREE(tA, heap, DYNAMIC_TYPE_ECC_BUFFER);
     return GEN_MEM_ERR;
  }
#endif

#ifdef WOLFSSL_SMALL_STACK_CACHE
  key = (ecc_key *)XMALLOC(sizeof(*key), heap, DYNAMIC_TYPE_ECC_BUFFER);
  if (key == NULL) {
     XFREE(tB, heap, DYNAMIC_TYPE_ECC_BUFFER);
     XFREE(tA, heap, DYNAMIC_TYPE_ECC_BUFFER);
     return GEN_MEM_ERR;
  }
#endif
#ifdef WOLFSSL_SMALL_STACK
  precomp = (ecc_point**)XMALLOC(sizeof(ecc_point*) * SHAMIR_PRECOMP_SZ, heap,
                                                       DYNAMIC_TYPE_ECC_BUFFER);
  if (precomp == NULL) {
     XFREE(tB, heap, DYNAMIC_TYPE_ECC_BUFFER);
     XFREE(tA, heap, DYNAMIC_TYPE_ECC_BUFFER);
  #ifdef WOLFSSL_SMALL_STACK_CACHE
     XFREE(key, heap, DYNAMIC_TYPE_ECC_BUFFER);
  #endif
     return GEN_MEM_ERR;
  }
#endif
#ifdef WOLFSSL_SMALL_STACK_CACHE
  key->t1 = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
  key->t2 = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
#ifdef ALT_ECC_SIZE
  key->x = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
  key->y = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
  key->z = (mp_int*)XMALLOC(sizeof(mp_int), heap, DYNAMIC_TYPE_ECC);
#endif

  if (key->t1 == NULL || key->t2 == NULL
#ifdef ALT_ECC_SIZE
     || key->x == NULL || key->y == NULL || key->z == NULL
#endif
  ) {
#ifdef ALT_ECC_SIZE
      XFREE(key->z, heap, DYNAMIC_TYPE_ECC);
      XFREE(key->y, heap, DYNAMIC_TYPE_ECC);
      XFREE(key->x, heap, DYNAMIC_TYPE_ECC);
#endif
      XFREE(key->t2, heap, DYNAMIC_TYPE_ECC);
      XFREE(key->t1, heap, DYNAMIC_TYPE_ECC);
      XFREE(precomp, heap, DYNAMIC_TYPE_ECC_BUFFER);
      XFREE(tB, heap, DYNAMIC_TYPE_ECC_BUFFER);
      XFREE(tA, heap, DYNAMIC_TYPE_ECC_BUFFER);
      XFREE(key, heap, DYNAMIC_TYPE_ECC_BUFFER);
      return MEMORY_E;
  }
  C->key = key;
#endif /* WOLFSSL_SMALL_STACK_CACHE */

  /* init variables */
  XMEMSET(tA, 0, ECC_BUFSIZE);
  XMEMSET(tB, 0, ECC_BUFSIZE);
#ifndef WOLFSSL_SMALL_STACK
  XMEMSET(precomp, 0, sizeof(precomp));
#else
  XMEMSET(precomp, 0, sizeof(ecc_point*) * SHAMIR_PRECOMP_SZ);
#endif
#ifdef WOLFSSL_CHECK_MEM_ZERO
  wc_MemZero_Add("ecc_mul2add tA", tA, ECC_BUFSIZE);
  wc_MemZero_Add("ecc_mul2add tB", tB, ECC_BUFSIZE);
#endif

  /* get sizes */
  lenA = (unsigned int)mp_unsigned_bin_size(kA);
  lenB = (unsigned int)mp_unsigned_bin_size(kB);
  len  = MAX(lenA, lenB);

  /* sanity check */
  if ((lenA > ECC_BUFSIZE) || (lenB > ECC_BUFSIZE)) {
    err = BAD_FUNC_ARG;
  }

  if (err == MP_OKAY) {
    /* extract and justify kA */
    err = mp_to_unsigned_bin(kA, (len - lenA) + tA);

    /* extract and justify kB */
    if (err == MP_OKAY)
        err = mp_to_unsigned_bin(kB, (len - lenB) + tB);

    /* allocate the table */
    if (err == MP_OKAY) {
        for (x = 0; x < SHAMIR_PRECOMP_SZ; x++) {
        #ifdef WOLFSSL_NO_MALLOC
            precomp[x] = &lcl_precomp[x];
        #endif
            err = wc_ecc_new_point_ex(&precomp[x], heap);
            if (err != MP_OKAY)
                break;
        #ifdef WOLFSSL_SMALL_STACK_CACHE
            precomp[x]->key = key;
        #endif
        }
    }
  }

  if (err == MP_OKAY)
    /* init montgomery reduction */
    err = mp_montgomery_setup(modulus, &mp);

  if (err == MP_OKAY) {
    err = ecc_mont_norm_points(A, precomp[1], B, precomp[1<<2], modulus, heap);
  }

  if (err == MP_OKAY) {
    /* precomp [i,0](A + B) table */
    err = ecc_projective_dbl_point_safe(precomp[1], precomp[2], a, modulus, mp);
  }
  if (err == MP_OKAY) {
    err = ecc_projective_add_point_safe(precomp[1], precomp[2], precomp[3],
                                                          a, modulus, mp, NULL);
  }

  if (err == MP_OKAY) {
    /* precomp [0,i](A + B) table */
    err = ecc_projective_dbl_point_safe(precomp[4], precomp[8], a, modulus, mp);
  }
  if (err == MP_OKAY) {
    err = ecc_projective_add_point_safe(precomp[4], precomp[8], precomp[12], a,
                                                             modulus, mp, NULL);
  }

  if (err == MP_OKAY) {
    /* precomp [i,j](A + B) table (i != 0, j != 0) */
    for (x = 1; x < 4; x++) {
      for (y = 1; y < 4; y++) {
        if (err == MP_OKAY) {
          err = ecc_projective_add_point_safe(precomp[x], precomp[(y<<2)],
                                                  precomp[x+(y<<2)], a, modulus,
                                                  mp, NULL);
        }
      }
    }
  }

  if (err == MP_OKAY) {
    nibble  = 3;
    first   = 1;
    bitbufA = tA[0];
    bitbufB = tB[0];

    /* for every byte of the multiplicands */
    for (x = 0; x < (int)len || nibble != 3; ) {
        /* grab a nibble */
        if (++nibble == 4) {
            if (x == (int)len) break;
            bitbufA = tA[x];
            bitbufB = tB[x];
            nibble  = 0;
            x++;
        }

        /* extract two bits from both, shift/update */
        nA = (bitbufA >> 6) & 0x03;
        nB = (bitbufB >> 6) & 0x03;
        bitbufA = (bitbufA << 2) & 0xFF;
        bitbufB = (bitbufB << 2) & 0xFF;

        /* if both zero, if first, continue */
        if ((nA == 0) && (nB == 0) && (first == 1)) {
            continue;
        }

        /* double twice, only if this isn't the first */
        if (first == 0) {
            /* double twice */
            if (err == MP_OKAY)
                err = ecc_projective_dbl_point_safe(C, C, a, modulus, mp);
            if (err == MP_OKAY)
                err = ecc_projective_dbl_point_safe(C, C, a, modulus, mp);
            else
                break;
        }

        /* if not both zero */
        if ((nA != 0) || (nB != 0)) {
            unsigned int i = nA + (nB<<2);
            if (first == 1) {
                /* if first, copy from table */
                first = 0;
                if (err == MP_OKAY)
                    err = mp_copy(precomp[i]->x, C->x);

                if (err == MP_OKAY)
                    err = mp_copy(precomp[i]->y, C->y);

                if (err == MP_OKAY)
                    err = mp_copy(precomp[i]->z, C->z);
                else
                    break;
            } else {
                /* if not first, add from table */
                if (err == MP_OKAY)
                    err = ecc_projective_add_point_safe(C, precomp[i],
                                                        C, a, modulus, mp,
                                                        &first);
                if (err != MP_OKAY)
                    break;
            }
        }
    }
  }

  /* reduce to affine */
  if (err == MP_OKAY)
    err = ecc_map(C, modulus, mp);

  /* clean up */
  for (x = 0; x < SHAMIR_PRECOMP_SZ; x++) {
     wc_ecc_del_point_ex(precomp[x], heap);
  }

  ForceZero(tA, ECC_BUFSIZE);
  ForceZero(tB, ECC_BUFSIZE);
#ifdef WOLFSSL_SMALL_STACK_CACHE
#ifdef ALT_ECC_SIZE
  XFREE(key->z, heap, DYNAMIC_TYPE_ECC);
  XFREE(key->y, heap, DYNAMIC_TYPE_ECC);
  XFREE(key->x, heap, DYNAMIC_TYPE_ECC);
#endif
  XFREE(key->t2, heap, DYNAMIC_TYPE_ECC);
  XFREE(key->t1, heap, DYNAMIC_TYPE_ECC);
  XFREE(key, heap, DYNAMIC_TYPE_ECC_BUFFER);
  C->key = NULL;
#endif
#ifdef WOLFSSL_SMALL_STACK
  XFREE(precomp, heap, DYNAMIC_TYPE_ECC_BUFFER);
#endif
#ifndef WOLFSSL_NO_MALLOC
  XFREE(tB, heap, DYNAMIC_TYPE_ECC_BUFFER);
  XFREE(tA, heap, DYNAMIC_TYPE_ECC_BUFFER);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
  wc_MemZero_Check(tB, ECC_BUFSIZE);
  wc_MemZero_Check(tA, ECC_BUFSIZE);
#endif
  return err;
}

#endif /* ECC_SHAMIR */
#endif /* (!WOLFSSL_SP_MATH && !WOLFSSL_ATECC508A && !WOLFSSL_ATECC608A &&
        * !WOLFSSL_CRYPTOCEL */


#ifdef HAVE_ECC_VERIFY
#ifndef NO_ASN
/* verify
 *
 * w  = s^-1 mod n
 * u1 = xw
 * u2 = rw
 * X = u1*G + u2*Q
 * v = X_x1 mod n
 * accept if v == r
 */

/**
 Verify an ECC signature
 sig         The signature to verify
 siglen      The length of the signature (octets)
 hash        The hash (message digest) that was signed
 hashlen     The length of the hash (octets)
 res         Result of signature, 1==valid, 0==invalid
 key         The corresponding public ECC key
 return      MP_OKAY if successful (even if the signature is not valid)
             Caller should check the *res value to determine if the signature
             is valid or invalid. Other negative values are returned on error.
 */
WOLFSSL_ABI
int wc_ecc_verify_hash(const byte* sig, word32 siglen, const byte* hash,
                       word32 hashlen, int* res, ecc_key* key)
{
    int err;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    mp_int *r = NULL, *s = NULL;
#else
    DECL_MP_INT_SIZE_DYN(r, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
    DECL_MP_INT_SIZE_DYN(s, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    int isPrivateKeyOnly = 0;
#endif

    if (sig == NULL || hash == NULL || res == NULL || key == NULL) {
        return ECC_BAD_ARG_E;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (key->devId != INVALID_DEVID)
    #endif
    {
        err = wc_CryptoCb_EccVerify(sig, siglen, hash, hashlen, res, key);
        #ifndef WOLF_CRYPTO_CB_ONLY_ECC
        if (err != CRYPTOCB_UNAVAILABLE)
            return err;
        /* fall-through when unavailable */
        #endif
        #ifdef WOLF_CRYPTO_CB_ONLY_ECC
        if (err == CRYPTOCB_UNAVAILABLE) {
            err = NO_VALID_DEVID;
        }
        #endif
    }
#endif

#ifndef WOLF_CRYPTO_CB_ONLY_ECC

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    err = wc_ecc_alloc_async(key);
    if (err != 0)
        return err;
    r = key->r;
    s = key->s;
#else
    NEW_MP_INT_SIZE(r, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (r == NULL)
        return MEMORY_E;
#endif
    NEW_MP_INT_SIZE(s, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
    if (s == NULL) {
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return MEMORY_E;
    }
#endif
    err = INIT_MP_INT_SIZE(r, ECC_KEY_MAX_BITS(key));
    if (err != 0) {
        FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return err;
    }
    err = INIT_MP_INT_SIZE(s, ECC_KEY_MAX_BITS(key));
    if (err != 0) {
        FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
        FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        return err;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    switch (key->state) {
        case ECC_STATE_NONE:
        case ECC_STATE_VERIFY_DECODE:
            key->state = ECC_STATE_VERIFY_DECODE;

            /* default to invalid signature */
            *res = 0;

            /* Decode ASN.1 ECDSA signature. */
        #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
            /* Note, DecodeECC_DSA_Sig() calls mp_init() on r and s.
             * If either of those don't allocate correctly, none of
             * the rest of this function will execute, and everything
             * gets cleaned up at the end. */
            err = DecodeECC_DSA_Sig(sig, siglen, r, s);
        #else
            /* r and s are initialized. */
            err = DecodeECC_DSA_Sig_Ex(sig, siglen, r, s, 0);
        #endif
            if (err < 0) {
                break;
            }
            FALL_THROUGH;

        case ECC_STATE_VERIFY_DO:
            key->state = ECC_STATE_VERIFY_DO;
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (key->type == ECC_PRIVATEKEY_ONLY) {
                isPrivateKeyOnly = 1;
            }
        #endif
            err = wc_ecc_verify_hash_ex(r, s, hash, hashlen, res, key);

        #ifndef WOLFSSL_ASYNC_CRYPT
            /* done with R/S */
            mp_clear(r);
            mp_clear(s);
            FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
            FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
        #ifdef MP_INT_SIZE_CHECK_NULL
            r = NULL;
            s = NULL;
        #endif
        #endif

            if (err < 0) {
                break;
            }
            FALL_THROUGH;

        case ECC_STATE_VERIFY_RES:
            key->state = ECC_STATE_VERIFY_RES;
            err = 0;
            break;

        default:
            err = BAD_STATE_E;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    /* if async pending then return and skip done cleanup below */
    if (err == WC_PENDING_E) {
        if (!isPrivateKeyOnly) /* do not advance state if doing make pub key */
            key->state++;
        return err;
    }
#endif

    /* cleanup */
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
    wc_ecc_free_async(key);
#else
    FREE_MP_INT_SIZE(s, key->heap, DYNAMIC_TYPE_ECC);
    FREE_MP_INT_SIZE(r, key->heap, DYNAMIC_TYPE_ECC);
#endif

    /* make sure required variables are reset */
    wc_ecc_reset(key);
#else
    (void)siglen;
    (void)hashlen;
    (void)s;
    (void)r;
    (void)err;
#endif /* WOLF_CRYPTO_CB_ONLY_ECC */

    return err;
}
#endif /* !NO_ASN */

#ifndef WOLF_CRYPTO_CB_ONLY_ECC

#if !defined(WOLFSSL_STM32_PKA) && !defined(WOLFSSL_PSOC6_CRYPTO) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC)
static int wc_ecc_check_r_s_range(ecc_key* key, mp_int* r, mp_int* s)
{
    int err = MP_OKAY;
    DECLARE_CURVE_SPECS(1);

    ALLOC_CURVE_SPECS(1, err);
    if (err == MP_OKAY) {
        err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ORDER);
    }
    if (err != 0) {
        FREE_CURVE_SPECS();
        return err;
    }

    if (mp_iszero(r) || mp_iszero(s)) {
        err = MP_ZERO_E;
    }
    if ((err == 0) && (mp_cmp(r, curve->order) != MP_LT)) {
        err = MP_VAL;
    }
    if ((err == 0) && (mp_cmp(s, curve->order) != MP_LT)) {
        err = MP_VAL;
    }

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();
    return err;
}
#endif /* !WOLFSSL_STM32_PKA && !WOLFSSL_PSOC6_CRYPTO */

#ifdef HAVE_ECC_VERIFY_HELPER
static int ecc_verify_hash_sp(mp_int *r, mp_int *s, const byte* hash,
    word32 hashlen, int* res, ecc_key* key)
{
    (void)r;
    (void)s;
    (void)hash;
    (void)hashlen;
    (void)res;
    (void)key;

#if defined(WOLFSSL_DSP) && !defined(FREESCALE_LTC_ECC)
  if (key->handle != -1) {
      return sp_dsp_ecc_verify_256(key->handle, hash, hashlen, key->pubkey.x,
        key->pubkey.y, key->pubkey.z, r, s, res, key->heap);
  }
  if (wolfSSL_GetHandleCbSet() == 1) {
      return sp_dsp_ecc_verify_256(0, hash, hashlen, key->pubkey.x,
        key->pubkey.y, key->pubkey.z, r, s, res, key->heap);
  }
#endif

#if defined(WOLFSSL_SP_MATH) && !defined(FREESCALE_LTC_ECC)
    if (key->idx == ECC_CUSTOM_IDX || (1
    #ifndef WOLFSSL_SP_NO_256
         && ecc_sets[key->idx].id != ECC_SECP256R1
    #endif
    #ifdef WOLFSSL_SP_384
         && ecc_sets[key->idx].id != ECC_SECP384R1
    #endif
    #ifdef WOLFSSL_SP_521
         && ecc_sets[key->idx].id != ECC_SECP521R1
    #endif
        )) {
        return WC_KEY_SIZE_E;
    }
#endif

#if defined(WOLFSSL_HAVE_SP_ECC)
    if (key->idx != ECC_CUSTOM_IDX) {
    #if defined(WC_ECC_NONBLOCK) && defined(WC_ECC_NONBLOCK_ONLY)
        /* perform blocking call to non-blocking function */
        ecc_nb_ctx_t nb_ctx;
        int err;
        XMEMSET(&nb_ctx, 0, sizeof(nb_ctx));
        err = NOT_COMPILED_IN; /* set default error */
    #endif
    #ifndef WOLFSSL_SP_NO_256
        if (ecc_sets[key->idx].id == ECC_SECP256R1) {
        #ifdef WC_ECC_NONBLOCK
            if (key->nb_ctx) {
                return sp_ecc_verify_256_nb(&key->nb_ctx->sp_ctx, hash, hashlen,
                    key->pubkey.x, key->pubkey.y, key->pubkey.z, r, s, res,
                    key->heap);
            }
            #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_verify_256_nb(&nb_ctx.sp_ctx, hash, hashlen,
                    key->pubkey.x, key->pubkey.y, key->pubkey.z, r, s, res,
                    key->heap);
            } while (err == FP_WOULDBLOCK);
            return err;
            #endif
        #endif /* WC_ECC_NONBLOCK */
        #if !defined(WC_ECC_NONBLOCK) || (defined(WC_ECC_NONBLOCK) && !defined(WC_ECC_NONBLOCK_ONLY))
            {
                int ret;
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
                ret = sp_ecc_verify_256(hash, hashlen, key->pubkey.x,
                    key->pubkey.y, key->pubkey.z, r, s, res, key->heap);
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        #endif
        }
#ifdef SM2_SP_IMPL_AVAILABLE
        #ifdef WOLFSSL_SM2
        if (ecc_sets[key->idx].id == ECC_SM2P256V1) {
            #if defined(FP_ECC_CONTROL) && !defined(WOLFSSL_DSP_BUILD)
            return sp_ecc_cache_verify_sm2_256(hash, hashlen, key->pubkey.x,
                key->pubkey.y, key->pubkey.z, r, s, res,
                sp_ecc_get_cache_entry_256(&(key->pubkey), ECC_SM2P256V1,
                                          key->fpIdx, key->fpBuild, key->heap),
                key->heap);
            #endif
            #if !defined(FP_ECC_CONTROL)
            return sp_ecc_verify_sm2_256(hash, hashlen, key->pubkey.x,
                key->pubkey.y, key->pubkey.z, r, s, res, key->heap);
            #endif
        }
        #endif
    #endif
#endif
    #ifdef WOLFSSL_SP_384
        if (ecc_sets[key->idx].id == ECC_SECP384R1) {
        #ifdef WC_ECC_NONBLOCK
            if (key->nb_ctx) {
                return sp_ecc_verify_384_nb(&key->nb_ctx->sp_ctx, hash, hashlen,
                    key->pubkey.x,  key->pubkey.y, key->pubkey.z, r, s, res,
                    key->heap);
            }
            #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_verify_384_nb(&nb_ctx.sp_ctx, hash, hashlen,
                    key->pubkey.x, key->pubkey.y, key->pubkey.z, r, s, res,
                    key->heap);
            } while (err == FP_WOULDBLOCK);
            return err;
            #endif
        #endif /* WC_ECC_NONBLOCK */
        #if !defined(WC_ECC_NONBLOCK) || (defined(WC_ECC_NONBLOCK) && !defined(WC_ECC_NONBLOCK_ONLY))
            {
                int ret;
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
                ret = sp_ecc_verify_384(hash, hashlen, key->pubkey.x,
                    key->pubkey.y, key->pubkey.z, r, s, res, key->heap);
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        #endif
        }
    #endif
    #ifdef WOLFSSL_SP_521
        if (ecc_sets[key->idx].id == ECC_SECP521R1) {
        #ifdef WC_ECC_NONBLOCK
            if (key->nb_ctx) {
                return sp_ecc_verify_521_nb(&key->nb_ctx->sp_ctx, hash, hashlen,
                    key->pubkey.x,  key->pubkey.y, key->pubkey.z, r, s, res,
                    key->heap);
            }
            #ifdef WC_ECC_NONBLOCK_ONLY
            do { /* perform blocking call to non-blocking function */
                err = sp_ecc_verify_521_nb(&nb_ctx.sp_ctx, hash, hashlen,
                    key->pubkey.x, key->pubkey.y, key->pubkey.z, r, s, res,
                    key->heap);
            } while (err == FP_WOULDBLOCK);
            return err;
            #endif
        #endif /* WC_ECC_NONBLOCK */
        #if !defined(WC_ECC_NONBLOCK) || (defined(WC_ECC_NONBLOCK) && !defined(WC_ECC_NONBLOCK_ONLY))
            {
                int ret;
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
                ret = sp_ecc_verify_521(hash, hashlen, key->pubkey.x,
                    key->pubkey.y, key->pubkey.z, r, s, res, key->heap);
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        #endif
        }
    #endif
    }
#endif

    return NOT_COMPILED_IN;
}

#if !defined(WOLFSSL_SP_MATH) || defined(FREESCALE_LTC_ECC)
static int ecc_verify_hash(mp_int *r, mp_int *s, const byte* hash,
    word32 hashlen, int* res, ecc_key* key, ecc_curve_spec* curve)
{
   int        err;
   ecc_point* mG = NULL;
   ecc_point* mQ = NULL;
#ifdef WOLFSSL_NO_MALLOC
   ecc_point  lcl_mG;
   ecc_point  lcl_mQ;
#endif
   DECL_MP_INT_SIZE_DYN(w, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
#if !defined(WOLFSSL_ASYNC_CRYPT) || !defined(HAVE_CAVIUM_V)
   DECL_MP_INT_SIZE_DYN(e_lcl, ECC_KEY_MAX_BITS(key), MAX_ECC_BITS_USE);
#endif
   mp_int*    e;
   mp_int*    v = NULL;      /* Will be w. */
   mp_int*    u1 = NULL;     /* Will be e. */
   mp_int*    u2 = NULL;     /* Will be w. */

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(HAVE_CAVIUM_V)
   err = wc_ecc_alloc_mpint(key, &key->e);
   if (err != 0) {
      return err;
   }
   e = key->e;

   err = mp_init(e);
#else
   NEW_MP_INT_SIZE(e_lcl, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
   if (e_lcl == NULL) {
       return MEMORY_E;
   }
#endif
   e = e_lcl;

   err = INIT_MP_INT_SIZE(e, ECC_KEY_MAX_BITS(key));
#endif /* WOLFSSL_ASYNC_CRYPT && HAVE_CAVIUM_V */
   if (err != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
   #if !defined(WOLFSSL_ASYNC_CRYPT) || !defined(HAVE_CAVIUM_V)
      XFREE(e_lcl, key->heap, DYNAMIC_TYPE_ECC);
   #endif
#endif
      return MEMORY_E;
   }

   /* read hash */
   if (err == MP_OKAY) {
       /* we may need to truncate if hash is longer than key size */
       unsigned int orderBits = (unsigned int)mp_count_bits(curve->order);

       /* truncate down to byte size, may be all that's needed */
       if ( (WOLFSSL_BIT_SIZE * hashlen) > orderBits)
           hashlen = (orderBits + WOLFSSL_BIT_SIZE - 1) / WOLFSSL_BIT_SIZE;
       err = mp_read_unsigned_bin(e, hash, hashlen);

       /* may still need bit truncation too */
       if (err == MP_OKAY && (WOLFSSL_BIT_SIZE * hashlen) > orderBits)
           mp_rshb(e, (int)(WOLFSSL_BIT_SIZE - (orderBits & 0x7)));
   }

   /* check for async hardware acceleration */
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
   if (err == MP_OKAY && key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
   #if defined(HAVE_CAVIUM_V) || defined(HAVE_INTEL_QA)
   #ifdef HAVE_CAVIUM_V
      if (NitroxEccIsCurveSupported(key))
   #endif
      {
          word32 keySz = (word32)key->dp->size;
          err = wc_mp_to_bigint_sz(e, &e->raw, keySz);
          if (err == MP_OKAY)
              err = wc_mp_to_bigint_sz(key->pubkey.x, &key->pubkey.x->raw, keySz);
          if (err == MP_OKAY)
              err = wc_mp_to_bigint_sz(key->pubkey.y, &key->pubkey.y->raw, keySz);
          if (err == MP_OKAY)
          #ifdef HAVE_CAVIUM_V
              err = NitroxEcdsaVerify(key, &e->raw, &key->pubkey.x->raw,
                    &key->pubkey.y->raw, &r->raw, &s->raw,
                    &curve->prime->raw, &curve->order->raw, res);
          #else
              err = IntelQaEcdsaVerify(&key->asyncDev, &e->raw, &key->pubkey.x->raw,
                    &key->pubkey.y->raw, &r->raw, &s->raw, &curve->Af->raw,
                    &curve->Bf->raw, &curve->prime->raw, &curve->order->raw,
                    &curve->Gx->raw, &curve->Gy->raw, res);
          #endif

      #ifndef HAVE_CAVIUM_V
          mp_clear(e);
      #endif

          return err;
      }
   #endif /* HAVE_CAVIUM_V || HAVE_INTEL_QA */
   }
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_ECC */

   NEW_MP_INT_SIZE(w, ECC_KEY_MAX_BITS(key), key->heap, DYNAMIC_TYPE_ECC);
#ifdef MP_INT_SIZE_CHECK_NULL
   if (w == NULL) {
       err = MEMORY_E;
   }
#endif

   if (err == MP_OKAY) {
       u1 = e;
       u2 = w;
       v = w;
   }
   if (err == MP_OKAY) {
       err = INIT_MP_INT_SIZE(w, ECC_KEY_MAX_BITS(key));
   }

   /* allocate points */
   if (err == MP_OKAY) {
   #ifdef WOLFSSL_NO_MALLOC
       mG = &lcl_mG;
   #endif
       err = wc_ecc_new_point_ex(&mG, key->heap);
   }
   if (err == MP_OKAY) {
   #ifdef WOLFSSL_NO_MALLOC
       mQ = &lcl_mQ;
   #endif
       err = wc_ecc_new_point_ex(&mQ, key->heap);
   }

   /*  w  = s^-1 mod n */
   if (err == MP_OKAY)
       err = mp_invmod(s, curve->order, w);

   /* u1 = ew */
   if (err == MP_OKAY)
       err = mp_mulmod(e, w, curve->order, u1);

   /* u2 = rw */
   if (err == MP_OKAY)
       err = mp_mulmod(r, w, curve->order, u2);

   /* find mG and mQ */
   if (err == MP_OKAY)
       err = mp_copy(curve->Gx, mG->x);
   if (err == MP_OKAY)
       err = mp_copy(curve->Gy, mG->y);
   if (err == MP_OKAY)
       err = mp_set(mG->z, 1);

   if (err == MP_OKAY)
       err = mp_copy(key->pubkey.x, mQ->x);
   if (err == MP_OKAY)
       err = mp_copy(key->pubkey.y, mQ->y);
   if (err == MP_OKAY)
       err = mp_copy(key->pubkey.z, mQ->z);

#if defined(FREESCALE_LTC_ECC)
   /* use PKHA to compute u1*mG + u2*mQ */
   if (err == MP_OKAY)
       err = wc_ecc_mulmod_ex(u1, mG, mG, curve->Af, curve->prime, 0, key->heap);
   if (err == MP_OKAY)
       err = wc_ecc_mulmod_ex(u2, mQ, mQ, curve->Af, curve->prime, 0, key->heap);
   if (err == MP_OKAY)
       err = wc_ecc_point_add(mG, mQ, mG, curve->prime);
#else
#ifndef ECC_SHAMIR
    if (err == MP_OKAY)
    {
        mp_digit mp = 0;

        if (!mp_iszero((MP_INT_SIZE*)u1)) {
            /* compute u1*mG + u2*mQ = mG */
            err = wc_ecc_mulmod_ex(u1, mG, mG, curve->Af, curve->prime, 0,
                                                                     key->heap);
            if (err == MP_OKAY) {
                err = wc_ecc_mulmod_ex(u2, mQ, mQ, curve->Af, curve->prime, 0,
                                                                     key->heap);
            }

            /* find the montgomery mp */
            if (err == MP_OKAY)
                err = mp_montgomery_setup(curve->prime, &mp);

            /* add them */
            if (err == MP_OKAY)
                err = ecc_projective_add_point_safe(mQ, mG, mG, curve->Af,
                                                        curve->prime, mp, NULL);
        }
        else {
            /* compute 0*mG + u2*mQ = mG */
            err = wc_ecc_mulmod_ex(u2, mQ, mG, curve->Af, curve->prime, 0,
                                                                     key->heap);
            /* find the montgomery mp */
            if (err == MP_OKAY)
                err = mp_montgomery_setup(curve->prime, &mp);
        }

        /* reduce */
        if (err == MP_OKAY)
            err = ecc_map(mG, curve->prime, mp);
    }
#else
    /* use Shamir's trick to compute u1*mG + u2*mQ using half the doubles */
    if (err == MP_OKAY) {
        err = ecc_mul2add(mG, u1, mQ, u2, mG, curve->Af, curve->prime,
                                                                     key->heap);
    }
#endif /* ECC_SHAMIR */
#endif /* FREESCALE_LTC_ECC */
   /* v = X_x1 mod n */
   if (err == MP_OKAY)
       err = mp_mod(mG->x, curve->order, v);

   /* does v == r */
   if (err == MP_OKAY) {
       if (mp_cmp(v, r) == MP_EQ)
           *res = 1;
   }

   /* cleanup */
   wc_ecc_del_point_ex(mG, key->heap);
   wc_ecc_del_point_ex(mQ, key->heap);

   mp_clear(e);
   mp_clear(w);
   FREE_MP_INT_SIZE(w, key->heap, DYNAMIC_TYPE_ECC);
#if !defined(WOLFSSL_ASYNC_CRYPT) || !defined(HAVE_CAVIUM_V)
   FREE_MP_INT_SIZE(e_lcl, key->heap, DYNAMIC_TYPE_ECC);
#endif

   return err;
}
#endif /* !WOLFSSL_SP_MATH || FREESCALE_LTC_ECC */
#endif /* HAVE_ECC_VERIFY_HELPER */

/**
   Verify an ECC signature
   r           The signature R component to verify
   s           The signature S component to verify
   hash        The hash (message digest) that was signed
   hashlen     The length of the hash (octets)
   res         Result of signature, 1==valid, 0==invalid
   key         The corresponding public ECC key
   return      MP_OKAY if successful (even if the signature is not valid)
               Caller should check the *res value to determine if the signature
               is valid or invalid. Other negative values are returned on error.
*/
int wc_ecc_verify_hash_ex(mp_int *r, mp_int *s, const byte* hash,
                    word32 hashlen, int* res, ecc_key* key)
{
#if defined(WOLFSSL_STM32_PKA)
    return stm32_ecc_verify_hash_ex(r, s, hash, hashlen, res, key);
#elif defined(WOLFSSL_PSOC6_CRYPTO)
    return psoc6_ecc_verify_hash_ex(r, s, hash, hashlen, res, key);
#else
   int           err;
   word32        keySz = 0;
#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
   byte sigRS[ATECC_KEY_SIZE*2];
#elif defined(WOLFSSL_CRYPTOCELL)
   byte sigRS[ECC_MAX_CRYPTO_HW_SIZE*2];
   CRYS_ECDSA_VerifyUserContext_t sigCtxTemp;
   word32 msgLenInBytes = hashlen;
   CRYS_ECPKI_HASH_OpMode_t hash_mode;
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
   byte sigRS[ECC_MAX_CRYPTO_HW_SIZE * 2];
#elif defined(WOLFSSL_KCAPI_ECC)
   byte sigRS[MAX_ECC_BYTES*2];
#elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)
   byte sigRS[ECC_MAX_CRYPTO_HW_SIZE * 2];
   byte hashcopy[ECC_MAX_CRYPTO_HW_SIZE] = {0};
#else
   int curveLoaded = 0;
   DECLARE_CURVE_SPECS(ECC_CURVE_FIELD_COUNT);
#endif

   if (r == NULL || s == NULL || hash == NULL || res == NULL || key == NULL)
       return ECC_BAD_ARG_E;

   /* default to invalid signature */
   *res = 0;

   /* is the IDX valid ?  */
   if (wc_ecc_is_valid_idx(key->idx) == 0 || key->dp == NULL) {
      return ECC_BAD_ARG_E;
   }

   err = wc_ecc_check_r_s_range(key, r, s);
   if (err != MP_OKAY) {
      return err;
   }

   keySz = (word32)key->dp->size;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC) && \
       defined(WOLFSSL_ASYNC_CRYPT_SW)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ECC) {
        if (wc_AsyncSwInit(&key->asyncDev, ASYNC_SW_ECC_VERIFY)) {
            WC_ASYNC_SW* sw = &key->asyncDev.sw;
            sw->eccVerify.r = r;
            sw->eccVerify.s = s;
            sw->eccVerify.hash = hash;
            sw->eccVerify.hashlen = hashlen;
            sw->eccVerify.stat = res;
            sw->eccVerify.key = key;
            return WC_PENDING_E;
        }
    }
#endif

#ifndef HAVE_ECC_VERIFY_HELPER

#ifndef WOLFSSL_SE050
    /* Extract R and S with front zero padding (if required),
     * SE050 does this in port layer  */
    XMEMSET(sigRS, 0, sizeof(sigRS));
    err = mp_to_unsigned_bin(r, sigRS +
                                (keySz - mp_unsigned_bin_size(r)));
    if (err != MP_OKAY) {
        return err;
    }
    err = mp_to_unsigned_bin(s, sigRS + keySz +
                                (keySz - mp_unsigned_bin_size(s)));
    if (err != MP_OKAY) {
        return err;
    }
#endif /* WOLFSSL_SE050 */

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
    err = atmel_ecc_verify(hash, sigRS, key->pubkey_raw, res);
    if (err != 0) {
       return err;
    }
    (void)hashlen;
#elif defined(WOLFSSL_CRYPTOCELL)

   /* truncate if hash is longer than key size */
   if (msgLenInBytes > keySz) {
       msgLenInBytes = keySz;
   }
   hash_mode = cc310_hashModeECC(msgLenInBytes);
   if (hash_mode == CRYS_ECPKI_HASH_OpModeLast) {
       /* hash_mode = */ cc310_hashModeECC(keySz);
       hash_mode = CRYS_ECPKI_HASH_SHA256_mode;
   }

   /* verify the signature using the public key */
   err = CRYS_ECDSA_Verify(&sigCtxTemp,
                           &key->ctx.pubKey,
                           hash_mode,
                           &sigRS[0],
                           keySz*2,
                           (byte*)hash,
                           msgLenInBytes);

   if (err == CRYS_ECDSA_VERIFY_INCONSISTENT_VERIFY_ERROR) {
       /* signature verification reported invalid signature. */
       *res = 0; /* Redundant, added for code clarity */
       err = MP_OKAY;
   }
   else if (err != SA_SILIB_RET_OK) {
       WOLFSSL_MSG("CRYS_ECDSA_Verify failed");
       return err;
   }
   else {
       /* valid signature. */
       *res = 1;
       err = MP_OKAY;
   }
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
   err = silabs_ecc_verify_hash(&sigRS[0], keySz * 2,
                                hash, hashlen,
                                res, key);
#elif defined(WOLFSSL_KCAPI_ECC)
    err = KcapiEcc_Verify(key, hash, hashlen, sigRS, keySz * 2);
    if (err == 0) {
        *res = 1;
    }
#elif defined(WOLFSSL_SE050)
    err = se050_ecc_verify_hash_ex(hash, hashlen, r, s, key, res);
#elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    if (hashlen > sizeof(hashcopy))
        return ECC_BAD_ARG_E;
    buf_reverse(hashcopy, hash, (hashlen < keySz) ? hashlen : keySz);
    mp_reverse(sigRS, keySz);
    mp_reverse(sigRS + keySz, keySz);
    WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(hashcopy), keySz);
    WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(key->keyRaw), keySz * 2);
    WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(sigRS), keySz * 2);

    err = XSecure_EllipticVerifySign(&(key->xSec.cinst),
                                     xil_curve_type[key->dp->id],
                                     XIL_CAST_U64(hashcopy), keySz,
                                     XIL_CAST_U64(key->keyRaw),
                                     XIL_CAST_U64(sigRS));

    if (err != XST_SUCCESS) {
        WOLFSSL_XIL_ERROR("Verify ECC signature failed", err);
        err = WC_HW_E;
    } else {
        *res = 1;
    }
#endif

#else
  /* checking if private key with no public part */
  if (key->type == ECC_PRIVATEKEY_ONLY) {
      WOLFSSL_MSG("Verify called with private key, generating public part");
      ALLOC_CURVE_SPECS(ECC_CURVE_FIELD_COUNT, err);
      if (err != MP_OKAY) {
          return err;
      }
      err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
      if (err != MP_OKAY) {
          FREE_CURVE_SPECS();
          return err;
      }
      err = ecc_make_pub_ex(key, curve, NULL, NULL);
      if (err != MP_OKAY) {
           WOLFSSL_MSG("Unable to extract public key");
           wc_ecc_curve_free(curve);
           FREE_CURVE_SPECS();
           return err;
      }
      curveLoaded = 1;
  }

  err = ecc_verify_hash_sp(r, s, hash, hashlen, res, key);
  if (err != NOT_COMPILED_IN) {
      if (curveLoaded) {
           wc_ecc_curve_free(curve);
           FREE_CURVE_SPECS();
      }
      return err;
  }

#if !defined(WOLFSSL_SP_MATH) || defined(FREESCALE_LTC_ECC)
   if (!curveLoaded) {
       err = 0; /* potential for NOT_COMPILED_IN error from SP attempt */
       ALLOC_CURVE_SPECS(ECC_CURVE_FIELD_COUNT, err);
       if (err != 0) {
          return err;
       }
       /* read in the specs for this curve */
       err = wc_ecc_curve_load(key->dp, &curve, ECC_CURVE_FIELD_ALL);
       if (err != 0) {
          FREE_CURVE_SPECS();
          return err;
       }
   }

   err = ecc_verify_hash(r, s, hash, hashlen, res, key, curve);
#endif /* !WOLFSSL_SP_MATH || FREESCALE_LTC_ECC */

   (void)curveLoaded;
   wc_ecc_curve_free(curve);
   FREE_CURVE_SPECS();
#endif /* HAVE_ECC_VERIFY_HELPER */

   (void)keySz;
   (void)hashlen;

   return err;
#endif /* WOLFSSL_STM32_PKA */
}
#endif /* WOLF_CRYPTO_CB_ONLY_ECC */
#endif /* HAVE_ECC_VERIFY */

#ifdef HAVE_ECC_KEY_IMPORT
/* import point from der
 * if shortKeySize != 0 then keysize is always (inLen-1)>>1 */
int wc_ecc_import_point_der_ex(const byte* in, word32 inLen,
                               const int curve_idx, ecc_point* point,
                               int shortKeySize)
{
    int err = 0;
#ifdef HAVE_COMP_KEY
    int compressed = 0;
#endif
    int keysize;
    byte pointType;

#ifndef HAVE_COMP_KEY
    (void)shortKeySize;
#endif

    if (in == NULL || point == NULL || (curve_idx < 0) ||
        (wc_ecc_is_valid_idx(curve_idx) == 0))
        return ECC_BAD_ARG_E;

    /* must be odd */
    if ((inLen & 1) == 0) {
        return ECC_BAD_ARG_E;
    }

    /* clear if previously allocated */
    mp_clear(point->x);
    mp_clear(point->y);
    mp_clear(point->z);

    /* init point */
#ifdef ALT_ECC_SIZE
    point->x = (mp_int*)&point->xyz[0];
    point->y = (mp_int*)&point->xyz[1];
    point->z = (mp_int*)&point->xyz[2];
    alt_fp_init(point->x);
    alt_fp_init(point->y);
    alt_fp_init(point->z);
#else
    err = mp_init_multi(point->x, point->y, point->z, NULL, NULL, NULL);
#endif
    if (err != MP_OKAY)
        return MEMORY_E;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    /* check for point type (4, 2, or 3) */
    pointType = in[0];
    if (pointType != ECC_POINT_UNCOMP && pointType != ECC_POINT_COMP_EVEN &&
                                         pointType != ECC_POINT_COMP_ODD) {
        err = ASN_PARSE_E;
    }

    if (pointType == ECC_POINT_COMP_EVEN || pointType == ECC_POINT_COMP_ODD) {
#ifdef HAVE_COMP_KEY
        compressed = 1;
#else
        err = NOT_COMPILED_IN;
#endif
    }

    /* adjust to skip first byte */
    inLen -= 1;
    in += 1;

    /* calculate key size based on inLen / 2 if uncompressed or shortKeySize
     * is true */
#ifdef HAVE_COMP_KEY
    keysize = (int)((compressed && !shortKeySize) ? inLen : inLen>>1);
#else
    keysize = (int)(inLen>>1);
#endif

    /* read data */
    if (err == MP_OKAY)
        err = mp_read_unsigned_bin(point->x, in, (word32)keysize);

#ifdef HAVE_COMP_KEY
    if (err == MP_OKAY && compressed == 1) {   /* build y */
    #if defined(WOLFSSL_HAVE_SP_ECC)
        #ifndef WOLFSSL_SP_NO_256
        if (curve_idx != ECC_CUSTOM_IDX &&
                                      ecc_sets[curve_idx].id == ECC_SECP256R1) {
            err = sp_ecc_uncompress_256(point->x, pointType, point->y);
        }
        else
#ifdef SM2_SP_IMPL_AVAILABLE
        #ifdef WOLFSSL_SM2
        if (curve_idx != ECC_CUSTOM_IDX &&
                                 ecc_sets[curve_idx->idx].id == ECC_SM2P256V1) {
            sp_ecc_uncompress_sm2_256(point->x, pointType, point->y);
        }
        else
        #endif
#endif
        #endif
        #ifdef WOLFSSL_SP_384
        if (curve_idx != ECC_CUSTOM_IDX &&
                                      ecc_sets[curve_idx].id == ECC_SECP384R1) {
            err = sp_ecc_uncompress_384(point->x, pointType, point->y);
        }
        else
        #endif
        #ifdef WOLFSSL_SP_521
        if (curve_idx != ECC_CUSTOM_IDX &&
                                      ecc_sets[curve_idx].id == ECC_SECP521R1) {
            err = sp_ecc_uncompress_521(point->x, pointType, point->y);
        }
        else
        #endif
    #endif
    #if !defined(WOLFSSL_SP_MATH)
        {
            int did_init = 0;
        #ifdef WOLFSSL_SMALL_STACK
            mp_int* t1 = NULL;
            mp_int* t2 = NULL;
        #else
            mp_int t1[1], t2[1];
        #endif
            DECLARE_CURVE_SPECS(3);

            ALLOC_CURVE_SPECS(3, err);

        #ifdef WOLFSSL_SMALL_STACK
            if (err == MP_OKAY) {
                t1 = (mp_int*)XMALLOC(sizeof(mp_int), NULL,
                                      DYNAMIC_TYPE_BIGINT);
                if (t1 == NULL) {
                    err = MEMORY_E;
                }
            }
            if (err == MP_OKAY) {
                t2 = (mp_int*)XMALLOC(sizeof(mp_int), NULL,
                                      DYNAMIC_TYPE_BIGINT);
                if (t2 == NULL) {
                    err = MEMORY_E;
                }
            }
        #endif

            if (err == MP_OKAY) {
                if (mp_init_multi(t1, t2, NULL, NULL, NULL, NULL) != MP_OKAY)
                    err = MEMORY_E;
                else
                    did_init = 1;
            }

            /* load curve info */
            if (err == MP_OKAY)
                err = wc_ecc_curve_load(&ecc_sets[curve_idx], &curve,
                    (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_AF |
                        ECC_CURVE_FIELD_BF));

        #if defined(WOLFSSL_CUSTOM_CURVES) && \
            defined(WOLFSSL_VALIDATE_ECC_IMPORT)
            /* validate prime is prime for custom curves */
            if (err == MP_OKAY && curve_idx == ECC_CUSTOM_IDX) {
                int isPrime = MP_NO;
                err = mp_prime_is_prime(curve->prime, 8, &isPrime);
                if (err == MP_OKAY && isPrime == MP_NO)
                    err = MP_VAL;
            }
        #endif

            /* compute x^3 */
            if (err == MP_OKAY)
                err = mp_sqr(point->x, t1);
            if (err == MP_OKAY)
                err = mp_mulmod(t1, point->x, curve->prime, t1);

            /* compute x^3 + a*x */
            if (err == MP_OKAY)
                err = mp_mulmod(curve->Af, point->x, curve->prime, t2);
            if (err == MP_OKAY)
                err = mp_add(t1, t2, t1);

            /* compute x^3 + a*x + b */
            if (err == MP_OKAY)
                err = mp_add(t1, curve->Bf, t1);

            /* compute sqrt(x^3 + a*x + b) */
            if (err == MP_OKAY)
                err = mp_sqrtmod_prime(t1, curve->prime, t2);

            /* adjust y */
            if (err == MP_OKAY) {
                if ((mp_isodd(t2) == MP_YES &&
                                            pointType == ECC_POINT_COMP_ODD) ||
                    (mp_isodd(t2) == MP_NO &&
                                            pointType == ECC_POINT_COMP_EVEN)) {
                    err = mp_mod(t2, curve->prime, point->y);
                }
                else {
                    err = mp_submod(curve->prime, t2, curve->prime, point->y);
                }
            }

            if (did_init) {
                mp_clear(t2);
                mp_clear(t1);
            }

        #ifdef WOLFSSL_SMALL_STACK
            if (t1 != NULL) {
                XFREE(t1, NULL, DYNAMIC_TYPE_BIGINT);
            }
            if (t2 != NULL) {
                XFREE(t2, NULL, DYNAMIC_TYPE_BIGINT);
            }
        #endif

            wc_ecc_curve_free(curve);
            FREE_CURVE_SPECS();
        }
    #else
        {
            err = WC_KEY_SIZE_E;
        }
    #endif
    }
#endif

    if (err == MP_OKAY) {
#ifdef HAVE_COMP_KEY
        if (compressed == 0)
#endif
            err = mp_read_unsigned_bin(point->y, in + keysize, (word32)keysize);
    }
    if (err == MP_OKAY)
        err = mp_set(point->z, 1);

    if (err != MP_OKAY) {
        mp_clear(point->x);
        mp_clear(point->y);
        mp_clear(point->z);
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}

/* function for backwards compatibility with previous implementations */
int wc_ecc_import_point_der(const byte* in, word32 inLen, const int curve_idx,
                            ecc_point* point)
{
    return wc_ecc_import_point_der_ex(in, inLen, curve_idx, point, 1);
}
#endif /* HAVE_ECC_KEY_IMPORT */

#ifdef HAVE_ECC_KEY_EXPORT
/* export point to der */

int wc_ecc_export_point_der_ex(const int curve_idx, ecc_point* point, byte* out,
                               word32* outLen, int compressed)
{
    if (compressed == 0)
        return wc_ecc_export_point_der(curve_idx, point, out, outLen);
#ifdef HAVE_COMP_KEY
    else
        return wc_ecc_export_point_der_compressed(curve_idx, point, out, outLen);
#else
    return NOT_COMPILED_IN;
#endif
}

int wc_ecc_export_point_der(const int curve_idx, ecc_point* point, byte* out,
                            word32* outLen)
{
    int    ret = MP_OKAY;
    word32 numlen;
#ifdef WOLFSSL_SMALL_STACK
    byte*  buf;
#else
    byte   buf[ECC_BUFSIZE];
#endif

    if ((curve_idx < 0) || (wc_ecc_is_valid_idx(curve_idx) == 0))
        return ECC_BAD_ARG_E;

    numlen = (word32)ecc_sets[curve_idx].size;

    /* return length needed only */
    if (point != NULL && out == NULL && outLen != NULL) {
        *outLen = 1 + 2*numlen;
        return LENGTH_ONLY_E;
    }

    if (point == NULL || out == NULL || outLen == NULL)
        return ECC_BAD_ARG_E;

    if (*outLen < (1 + 2*numlen)) {
        *outLen = 1 + 2*numlen;
        return BUFFER_E;
    }

    /* Sanity check the ordinates' sizes. */
    if (((word32)mp_unsigned_bin_size(point->x) > numlen) ||
        ((word32)mp_unsigned_bin_size(point->y) > numlen)) {
        return ECC_BAD_ARG_E;
    }

    /* store byte point type */
    out[0] = ECC_POINT_UNCOMP;

#ifdef WOLFSSL_SMALL_STACK
    buf = (byte*)XMALLOC(ECC_BUFSIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
    if (buf == NULL)
        return MEMORY_E;
#endif

    /* pad and store x */
    XMEMSET(buf, 0, ECC_BUFSIZE);
    ret = mp_to_unsigned_bin(point->x, buf +
        (numlen - (word32)mp_unsigned_bin_size(point->x)));
    if (ret != MP_OKAY)
        goto done;
    XMEMCPY(out+1, buf, numlen);

    /* pad and store y */
    XMEMSET(buf, 0, ECC_BUFSIZE);
    ret = mp_to_unsigned_bin(point->y, buf +
        (numlen - (word32)mp_unsigned_bin_size(point->y)));
    if (ret != MP_OKAY)
        goto done;
    XMEMCPY(out+1+numlen, buf, numlen);

    *outLen = 1 + 2*numlen;

done:
#ifdef WOLFSSL_SMALL_STACK
    XFREE(buf, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return ret;
}


/* export point to der */
#ifdef HAVE_COMP_KEY
int wc_ecc_export_point_der_compressed(const int curve_idx, ecc_point* point,
                                       byte* out, word32* outLen)
{
    int    ret = MP_OKAY;
    word32 numlen;
    word32 output_len;
#ifdef WOLFSSL_SMALL_STACK
    byte*  buf;
#else
    byte   buf[ECC_BUFSIZE];
#endif

    if ((curve_idx < 0) || (wc_ecc_is_valid_idx(curve_idx) == 0))
        return ECC_BAD_ARG_E;

    numlen = (word32)ecc_sets[curve_idx].size;
    output_len = 1 + numlen; /* y point type + x */

    /* return length needed only */
    if (point != NULL && out == NULL && outLen != NULL) {
        *outLen = output_len;
        return LENGTH_ONLY_E;
    }

    if (point == NULL || out == NULL || outLen == NULL)
        return ECC_BAD_ARG_E;


    if (*outLen < output_len) {
        *outLen = output_len;
        return BUFFER_E;
    }

    /* Sanity check the ordinate's size. */
    if ((word32)mp_unsigned_bin_size(point->x) > numlen) {
        return ECC_BAD_ARG_E;
    }

    /* store byte point type */
    out[0] = mp_isodd(point->y) == MP_YES ? ECC_POINT_COMP_ODD :
                                            ECC_POINT_COMP_EVEN;

#ifdef WOLFSSL_SMALL_STACK
    buf = (byte*)XMALLOC(ECC_BUFSIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
    if (buf == NULL)
        return MEMORY_E;
#endif

    /* pad and store x */
    XMEMSET(buf, 0, ECC_BUFSIZE);
    ret = mp_to_unsigned_bin(point->x, buf +
                             (numlen - (word32)mp_unsigned_bin_size(point->x)));
    if (ret != MP_OKAY)
        goto done;
    XMEMCPY(out+1, buf, numlen);

    *outLen = output_len;

done:
#ifdef WOLFSSL_SMALL_STACK
    XFREE(buf, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return ret;
}
#endif /* HAVE_COMP_KEY */

/* export public ECC key in ANSI X9.63 format */
WOLFSSL_ABI
int wc_ecc_export_x963(ecc_key* key, byte* out, word32* outLen)
{
   int    ret = MP_OKAY;
   word32 numlen;
#ifdef WOLFSSL_SMALL_STACK
   byte*  buf;
#else
   byte   buf[ECC_BUFSIZE];
#endif
   word32 pubxlen, pubylen;

   /* return length needed only */
   if (key != NULL && out == NULL && outLen != NULL) {
      /* if key hasn't been setup assume max bytes for size estimation */
      numlen = key->dp ? (word32)key->dp->size : MAX_ECC_BYTES;
      *outLen = 1 + 2 * numlen;
      return LENGTH_ONLY_E;
   }

   if (key == NULL || out == NULL || outLen == NULL)
      return ECC_BAD_ARG_E;

   if (key->type == ECC_PRIVATEKEY_ONLY)
       return ECC_PRIVATEONLY_E;

#if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    /* check if public key in secure memory */
    if (key->securePubKey > 0) {
        int keySz = wc_ecc_size(key);

        /* store byte point type */
        out[0] = ECC_POINT_UNCOMP;

        if (caamReadPartition((CAAM_ADDRESS)key->securePubKey, out+1, keySz*2) != 0)
            return WC_HW_E;

        *outLen = 1 + 2*keySz;
        return MP_OKAY;
    }
#endif

   if (key->type == 0 || wc_ecc_is_valid_idx(key->idx) == 0 || key->dp == NULL){
       return ECC_BAD_ARG_E;
   }

   numlen = (word32)key->dp->size;

    /* verify room in out buffer */
   if (*outLen < (1 + 2*numlen)) {
      *outLen = 1 + 2*numlen;
      return BUFFER_E;
   }

   /* verify public key length is less than key size */
   pubxlen = (word32)mp_unsigned_bin_size(key->pubkey.x);
   pubylen = (word32)mp_unsigned_bin_size(key->pubkey.y);
   if ((pubxlen > numlen) || (pubylen > numlen)) {
      WOLFSSL_MSG("Public key x/y invalid!");
      return BUFFER_E;
   }

   /* store byte point type */
   out[0] = ECC_POINT_UNCOMP;

#ifdef WOLFSSL_SMALL_STACK
   buf = (byte*)XMALLOC(ECC_BUFSIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   if (buf == NULL)
      return MEMORY_E;
#endif

   /* pad and store x */
   XMEMSET(buf, 0, ECC_BUFSIZE);
   ret = mp_to_unsigned_bin(key->pubkey.x, buf + (numlen - pubxlen));
   if (ret != MP_OKAY)
      goto done;
   XMEMCPY(out+1, buf, numlen);

   /* pad and store y */
   XMEMSET(buf, 0, ECC_BUFSIZE);
   ret = mp_to_unsigned_bin(key->pubkey.y, buf + (numlen - pubylen));
   if (ret != MP_OKAY)
      goto done;
   XMEMCPY(out+1+numlen, buf, numlen);

   *outLen = 1 + 2*numlen;

done:
#ifdef WOLFSSL_SMALL_STACK
   XFREE(buf, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

   return ret;
}


/* export public ECC key in ANSI X9.63 format, extended with
 * compression option */
WOLFSSL_ABI
int wc_ecc_export_x963_ex(ecc_key* key, byte* out, word32* outLen,
                          int compressed)
{
    if (compressed == 0)
        return wc_ecc_export_x963(key, out, outLen);
#ifdef HAVE_COMP_KEY
    else
        return wc_ecc_export_x963_compressed(key, out, outLen);
#else
    return NOT_COMPILED_IN;
#endif
}
#endif /* HAVE_ECC_KEY_EXPORT */


#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SE050) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC) && !defined(WOLFSSL_STM32_PKA)

/* is ecc point on curve described by dp ? */
static int _ecc_is_point(ecc_point* ecp, mp_int* a, mp_int* b, mp_int* prime)
{
#if !defined(WOLFSSL_SP_MATH)
   int err;
#ifdef WOLFSSL_SMALL_STACK
   mp_int* t1;
   mp_int* t2;
#else
   mp_int  t1[1], t2[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
   t1 = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (t1 == NULL)
       return MEMORY_E;
   t2 = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (t2 == NULL) {
       XFREE(t1, NULL, DYNAMIC_TYPE_ECC);
       return MEMORY_E;
   }
#endif

   if ((err = mp_init_multi(t1, t2, NULL, NULL, NULL, NULL)) != MP_OKAY) {
   #ifdef WOLFSSL_SMALL_STACK
      XFREE(t2, NULL, DYNAMIC_TYPE_ECC);
      XFREE(t1, NULL, DYNAMIC_TYPE_ECC);
   #endif
      return err;
   }

   SAVE_VECTOR_REGISTERS(err = _svr_ret;);

   /* compute y^2 */
   if (err == MP_OKAY)
       err = mp_sqr(ecp->y, t1);

   /* compute x^3 */
   if (err == MP_OKAY)
       err = mp_sqr(ecp->x, t2);
   if (err == MP_OKAY)
       err = mp_mod(t2, prime, t2);
   if (err == MP_OKAY)
       err = mp_mul(ecp->x, t2, t2);

   /* compute y^2 - x^3 */
   if (err == MP_OKAY)
       err = mp_submod(t1, t2, prime, t1);

   /* Determine if curve "a" should be used in calc */
#ifdef WOLFSSL_CUSTOM_CURVES
   if (err == MP_OKAY) {
      /* Use a and prime to determine if a == 3 */
      err = mp_set(t2, 0);
      if (err == MP_OKAY)
          err = mp_submod(prime, a, prime, t2);
   }
   if (err == MP_OKAY && mp_cmp_d(t2, 3) != MP_EQ) {
      /* compute y^2 - x^3 + a*x */
      if (err == MP_OKAY)
          err = mp_mulmod(t2, ecp->x, prime, t2);
      if (err == MP_OKAY)
          err = mp_addmod(t1, t2, prime, t1);
   }
   else
#endif /* WOLFSSL_CUSTOM_CURVES */
   {
      /* assumes "a" == 3 */
      (void)a;

      /* compute y^2 - x^3 + 3x */
      if (err == MP_OKAY)
          err = mp_add(t1, ecp->x, t1);
      if (err == MP_OKAY)
          err = mp_add(t1, ecp->x, t1);
      if (err == MP_OKAY)
          err = mp_add(t1, ecp->x, t1);
      if (err == MP_OKAY)
          err = mp_mod(t1, prime, t1);
  }

   /* adjust range (0, prime) */
   while (err == MP_OKAY && mp_isneg(t1)) {
      err = mp_add(t1, prime, t1);
   }
   while (err == MP_OKAY && mp_cmp(t1, prime) != MP_LT) {
      err = mp_sub(t1, prime, t1);
   }

   /* compare to b */
   if (err == MP_OKAY) {
       if (mp_cmp(t1, b) != MP_EQ) {
          err = IS_POINT_E;
       } else {
          err = MP_OKAY;
       }
   }

   mp_clear(t1);
   mp_clear(t2);

   RESTORE_VECTOR_REGISTERS();

#ifdef WOLFSSL_SMALL_STACK
   XFREE(t2, NULL, DYNAMIC_TYPE_ECC);
   XFREE(t1, NULL, DYNAMIC_TYPE_ECC);
#endif

   return err;
#else
   (void)a;
   (void)b;

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
   if (mp_count_bits(prime) == 256) {
#ifdef SM2_SP_IMPL_AVAILABLE
   #ifdef WOLFSSL_SM2
       if (!mp_is_bit_set(prime, 224)) {
           return sp_ecc_is_point_sm2_256(ecp->x, ecp->y);
       }
   #endif
#endif
       return sp_ecc_is_point_256(ecp->x, ecp->y);
   }
#endif
#ifdef WOLFSSL_SP_384
   if (mp_count_bits(prime) == 384) {
       return sp_ecc_is_point_384(ecp->x, ecp->y);
   }
#endif
#ifdef WOLFSSL_SP_521
   if (mp_count_bits(prime) == 521) {
       return sp_ecc_is_point_521(ecp->x, ecp->y);
   }
#endif
#else
   (void)ecp;
   (void)prime;
#endif
   return WC_KEY_SIZE_E;
#endif
}

int wc_ecc_is_point(ecc_point* ecp, mp_int* a, mp_int* b, mp_int* prime)
{
    int err = MP_OKAY;

    /* Validate parameters. */
    if ((ecp == NULL) || (a == NULL) || (b == NULL) || (prime == NULL)) {
        err = BAD_FUNC_ARG;
    }

    if (err == MP_OKAY) {
        /* x must be in the range [0, p-1] */
        if ((mp_cmp(ecp->x, prime) != MP_LT) || mp_isneg(ecp->x)) {
            err = ECC_OUT_OF_RANGE_E;
        }
    }

    if (err == MP_OKAY) {
        /* y must be in the range [0, p-1] */
        if ((mp_cmp(ecp->y, prime) != MP_LT) || mp_isneg(ecp->y)) {
            err = ECC_OUT_OF_RANGE_E;
        }
    }

    if (err == MP_OKAY) {
        /* z must be one, that is point must be in affine form. */
        if (!mp_isone(ecp->z)) {
            err = ECC_BAD_ARG_E;
        }
    }

    if (err == MP_OKAY) {
        /* Check x and y are valid for curve equation. */
        err = _ecc_is_point(ecp, a, b, prime);
    }

    return err;
}

#if (FIPS_VERSION_GE(5,0) || defined(WOLFSSL_VALIDATE_ECC_KEYGEN) || \
    (defined(WOLFSSL_VALIDATE_ECC_IMPORT) && !defined(WOLFSSL_SP_MATH))) && \
    !defined(WOLFSSL_KCAPI_ECC) || defined(WOLFSSL_CAAM)
/* validate privkey * generator == pubkey, 0 on success */
static int ecc_check_privkey_gen(ecc_key* key, mp_int* a, mp_int* prime)
{
    int        err;
    ecc_point* base = NULL;
    ecc_point* res  = NULL;
#ifdef WOLFSSL_NO_MALLOC
    ecc_point lcl_base;
    ecc_point lcl_res;
#endif
    DECLARE_CURVE_SPECS(3);

    if (key == NULL)
        return BAD_FUNC_ARG;

    ALLOC_CURVE_SPECS(3, err);

#ifdef WOLFSSL_NO_MALLOC
    res = &lcl_res;
#endif
    err = wc_ecc_new_point_ex(&res, key->heap);

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP256R1) {
        if (err == MP_OKAY) {
            err = sp_ecc_mulmod_base_256(key->k, res, 1, key->heap);
        }
    }
    else
#ifdef SM2_SP_IMPL_AVAILABLE
    #ifdef WOLFSSL_SM2
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SM2P256V1) {
        if (err == MP_OKAY) {
            err = sp_ecc_mulmod_base_sm2_256(&key->k, res, 1, key->heap);
        }
    }
    else
    #endif
#endif
#endif
#ifdef WOLFSSL_SP_384
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP384R1) {
        if (err == MP_OKAY) {
            err = sp_ecc_mulmod_base_384(key->k, res, 1, key->heap);
        }
    }
    else
#endif
#ifdef WOLFSSL_SP_521
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP521R1) {
        if (err == MP_OKAY) {
            err = sp_ecc_mulmod_base_521(key->k, res, 1, key->heap);
        }
    }
    else
#endif
#endif
    {
        if (err == MP_OKAY) {
        #ifdef WOLFSSL_NO_MALLOC
            base = &lcl_base;
        #endif
            err = wc_ecc_new_point_ex(&base, key->heap);
        }

        if (err == MP_OKAY) {
            /* load curve info */
            err = wc_ecc_curve_load(key->dp, &curve, (ECC_CURVE_FIELD_GX |
                                   ECC_CURVE_FIELD_GY | ECC_CURVE_FIELD_ORDER));
        }

        /* set up base generator */
        if (err == MP_OKAY)
            err = mp_copy(curve->Gx, base->x);
        if (err == MP_OKAY)
            err = mp_copy(curve->Gy, base->y);
        if (err == MP_OKAY)
            err = mp_set(base->z, 1);

#ifdef WOLFSSL_KCAPI_ECC
        if (err == MP_OKAY) {
            word32 pubkey_sz = (word32)key->dp->size*2;
            if (key->handle == NULL) {
                /* if handle loaded, then pubkey_raw already populated */
                err = KcapiEcc_LoadKey(key, key->pubkey_raw, &pubkey_sz, 1);
            }
            if (err == 0) {
                err = mp_read_unsigned_bin(res->x, key->pubkey_raw,
                                           pubkey_sz/2);
            }
            if (err == MP_OKAY) {
                err = mp_read_unsigned_bin(res->y,
                                           key->pubkey_raw + pubkey_sz/2,
                                           pubkey_sz/2);
            }
            if (err == MP_OKAY) {
                err = mp_set(res->z, 1);
            }
        }
        (void)a;
        (void)prime;
#else
#ifdef ECC_TIMING_RESISTANT
        if (err == MP_OKAY)
            err = wc_ecc_mulmod_ex2(key->k, base, res, a, prime, curve->order,
                                                        key->rng, 1, key->heap);
#else
        if (err == MP_OKAY)
            err = wc_ecc_mulmod_ex2(key->k, base, res, a, prime, curve->order,
                                                            NULL, 1, key->heap);
#endif
#endif /* WOLFSSL_KCAPI_ECC */
    }

    if (err == MP_OKAY) {
        /* compare result to public key */
        if (mp_cmp(res->x, key->pubkey.x) != MP_EQ ||
            mp_cmp(res->y, key->pubkey.y) != MP_EQ ||
            mp_cmp(res->z, key->pubkey.z) != MP_EQ) {
            /* didn't match */
            err = ECC_PRIV_KEY_E;
        }
    }

    wc_ecc_curve_free(curve);
    wc_ecc_del_point_ex(res, key->heap);
    wc_ecc_del_point_ex(base, key->heap);
    FREE_CURVE_SPECS();

    return err;
}
#endif /* FIPS_VERSION_GE(5,0) || WOLFSSL_VALIDATE_ECC_KEYGEN ||
        * (!WOLFSSL_SP_MATH && WOLFSSL_VALIDATE_ECC_IMPORT) */

#if (FIPS_VERSION_GE(5,0) || defined(WOLFSSL_VALIDATE_ECC_KEYGEN)) && \
    !defined(WOLFSSL_KCAPI_ECC)

/* check privkey generator helper, creates prime needed */
static int ecc_check_privkey_gen_helper(ecc_key* key)
{
    int    err;
#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A)
    DECLARE_CURVE_SPECS(2);
#endif

    if (key == NULL)
        return BAD_FUNC_ARG;

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
    /* Hardware based private key, so this operation is not supported */
    err = MP_OKAY; /* just report success */
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    /* Hardware based private key, so this operation is not supported */
    err = MP_OKAY; /* just report success */
#elif defined(WOLFSSL_KCAPI_ECC)
    /* Hardware based private key, so this operation is not supported */
    err = MP_OKAY; /* just report success */
#else
    err = MP_OKAY;
    ALLOC_CURVE_SPECS(2, err);

    /* load curve info */
    if (err == MP_OKAY)
        err = wc_ecc_curve_load(key->dp, &curve,
            (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_AF));

    if (err == MP_OKAY)
        err = ecc_check_privkey_gen(key, curve->Af, curve->prime);

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();

#endif /* WOLFSSL_ATECC508A */

    return err;
}

/* Performs a Pairwise Consistency Test on an ECC key pair. */
static int _ecc_pairwise_consistency_test(ecc_key* key, WC_RNG* rng)
{
    int err = 0;
    word32 flags = key->flags;

    /* If flags not set default to cofactor and dec/sign */
    if ((flags & (WC_ECC_FLAG_COFACTOR | WC_ECC_FLAG_DEC_SIGN)) == 0) {
        flags = (WC_ECC_FLAG_COFACTOR | WC_ECC_FLAG_DEC_SIGN);
    }

    if (flags & WC_ECC_FLAG_COFACTOR) {
        err = ecc_check_privkey_gen_helper(key);
    }

    if (!err && (flags & WC_ECC_FLAG_DEC_SIGN)) {
        byte* sig;
        byte* digest;
        word32 sigLen, digestLen;
        int dynRng = 0, res = 0;

        sigLen = (word32)wc_ecc_sig_size(key);
        digestLen = WC_SHA256_DIGEST_SIZE;
        sig = (byte*)XMALLOC(sigLen + digestLen, NULL, DYNAMIC_TYPE_ECC);
        if (sig == NULL)
            return MEMORY_E;
        digest = sig + sigLen;

        if (rng == NULL) {
            dynRng = 1;
            rng = wc_rng_new(NULL, 0, NULL);
            if (rng == NULL) {
                XFREE(sig, NULL, DYNAMIC_TYPE_ECC);
                return MEMORY_E;
            }
        }

        err = wc_RNG_GenerateBlock(rng, digest, digestLen);

        if (!err)
            err = wc_ecc_sign_hash(digest, WC_SHA256_DIGEST_SIZE, sig, &sigLen,
                    rng, key);
        if (!err)
            err = wc_ecc_verify_hash(sig, sigLen,
                    digest, WC_SHA256_DIGEST_SIZE, &res, key);

        if (res == 0)
            err = ECC_PCT_E;

        if (dynRng) {
            wc_rng_free(rng);
        }
        ForceZero(sig, sigLen + digestLen);
        XFREE(sig, NULL, DYNAMIC_TYPE_ECC);
    }
    (void)rng;

    if (err != 0)
        err = ECC_PCT_E;

    return err;
}
#endif /* (FIPS v5 or later || WOLFSSL_VALIDATE_ECC_KEYGEN) &&!WOLFSSL_KCAPI_ECC */

#ifdef HAVE_ECC_CHECK_PUBKEY_ORDER
/* validate order * pubkey = point at infinity, 0 on success */
static int ecc_check_pubkey_order(ecc_key* key, ecc_point* pubkey, mp_int* a,
        mp_int* prime, mp_int* order)
{
    ecc_point* inf = NULL;
#ifdef WOLFSSL_NO_MALLOC
    ecc_point  lcl_inf;
#endif
    int err;

    if (key == NULL)
        return BAD_FUNC_ARG;
   if (mp_count_bits(pubkey->x) > mp_count_bits(prime) ||
       mp_count_bits(pubkey->y) > mp_count_bits(prime) ||
       mp_count_bits(pubkey->z) > mp_count_bits(prime)) {
       return IS_POINT_E;
   }

#ifdef WOLFSSL_NO_MALLOC
    inf = &lcl_inf;
#endif
    err = wc_ecc_new_point_ex(&inf, key->heap);
    if (err == MP_OKAY) {
#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
        if (key->idx != ECC_CUSTOM_IDX &&
                                       ecc_sets[key->idx].id == ECC_SECP256R1) {
            err = sp_ecc_mulmod_256(order, pubkey, inf, 1, key->heap);
        }
        else
#ifdef SM2_SP_IMPL_AVAILABLE
    #ifdef WOLFSSL_SM2
        if (key->idx != ECC_CUSTOM_IDX &&
                                       ecc_sets[key->idx].id == ECC_SM2P256V1) {
            err = sp_ecc_mulmod_sm2_256(order, pubkey, inf, 1, key->heap);
        }
        else
    #endif
#endif
#endif
#ifdef WOLFSSL_SP_384
        if (key->idx != ECC_CUSTOM_IDX &&
                                       ecc_sets[key->idx].id == ECC_SECP384R1) {
            err = sp_ecc_mulmod_384(order, pubkey, inf, 1, key->heap);
        }
        else
#endif
#ifdef WOLFSSL_SP_521
        if (key->idx != ECC_CUSTOM_IDX &&
                                       ecc_sets[key->idx].id == ECC_SECP521R1) {
            err = sp_ecc_mulmod_521(order, pubkey, inf, 1, key->heap);
        }
        else
#endif
#endif
#if !defined(WOLFSSL_SP_MATH)
            err = wc_ecc_mulmod_ex(order, pubkey, inf, a, prime, 1, key->heap);
        if (err == MP_OKAY && !wc_ecc_point_is_at_infinity(inf))
            err = ECC_INF_E;
#else
        {
            (void)a;
            (void)prime;

            err = WC_KEY_SIZE_E;
        }
#endif
    }

    wc_ecc_del_point_ex(inf, key->heap);

    return err;
}
#endif /* !WOLFSSL_SP_MATH */

#endif /* !WOLFSSL_ATECC508A && !WOLFSSL_CRYPTOCELL*/

#ifdef OPENSSL_EXTRA
int wc_ecc_get_generator(ecc_point* ecp, int curve_idx)
{
    int err = MP_OKAY;
    DECLARE_CURVE_SPECS(2);

    if (!ecp || curve_idx < 0 || curve_idx > (int)(ECC_SET_COUNT-1))
        return BAD_FUNC_ARG;

    ALLOC_CURVE_SPECS(2, err);

    if (err == MP_OKAY)
        err = wc_ecc_curve_load(&ecc_sets[curve_idx], &curve,
                            (ECC_CURVE_FIELD_GX | ECC_CURVE_FIELD_GY));
    if (err == MP_OKAY)
        err = mp_copy(curve->Gx, ecp->x);
    if (err == MP_OKAY)
        err = mp_copy(curve->Gy, ecp->y);
    if (err == MP_OKAY)
        err = mp_set(ecp->z, 1);

    wc_ecc_curve_free(curve);
    FREE_CURVE_SPECS();

    return err;
}
#endif /* OPENSSLALL */


/* Validate the public key per SP 800-56Ar3 section 5.6.2.3.3,
 * ECC Full Public Key Validation Routine. If the parameter
 * partial is set, then it follows section 5.6.2.3.4, the ECC
 * Partial Public Key Validation Routine.
 * If the parameter priv is set, add in a few extra
 * checks on the bounds of the private key. */
static int _ecc_validate_public_key(ecc_key* key, int partial, int priv)
{
    int err = MP_OKAY;
#ifndef WOLFSSL_SP_MATH
#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_SILABS_SE_ACCEL) && \
    !defined(WOLFSSL_SE050) && !defined(WOLF_CRYPTO_CB_ONLY_ECC) && \
    !defined(WOLF_CRYPTO_CB_ONLY_ECC) && !defined(WOLFSSL_STM32_PKA)
    mp_int* b = NULL;
    #ifdef USE_ECC_B_PARAM
        DECLARE_CURVE_SPECS(4);
    #else
        #ifndef WOLFSSL_SMALL_STACK
            mp_int b_lcl;
        #endif
        DECLARE_CURVE_SPECS(3);
    #endif /* USE_ECC_B_PARAM */
#endif /* !WOLFSSL_ATECC508A && !WOLFSSL_ATECC608A &&
          !WOLFSSL_CRYPTOCELL && !WOLFSSL_SILABS_SE_ACCEL && !WOLFSSL_SE050 */
#endif /* !WOLFSSL_SP_MATH */

    ASSERT_SAVED_VECTOR_REGISTERS();

    if (key == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP256R1) {
        return sp_ecc_check_key_256(key->pubkey.x, key->pubkey.y,
            key->type == ECC_PRIVATEKEY ? key->k : NULL, key->heap);
    }
#ifdef SM2_SP_IMPL_AVAILABLE
#ifdef WOLFSSL_SM2
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SM2P256V1) {
        return sp_ecc_check_key_sm2_256(key->pubkey.x, key->pubkey.y
            key->type == ECC_PRIVATEKEY ? &key->k : NULL, key->heap);
    }
#endif
#endif
#endif
#ifdef WOLFSSL_SP_384
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP384R1) {
        return sp_ecc_check_key_384(key->pubkey.x, key->pubkey.y,
            key->type == ECC_PRIVATEKEY ? key->k : NULL, key->heap);
    }
#endif
#ifdef WOLFSSL_SP_521
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SECP521R1) {
        return sp_ecc_check_key_521(key->pubkey.x, key->pubkey.y,
            key->type == ECC_PRIVATEKEY ? key->k : NULL, key->heap);
    }
#endif
#if defined(WOLFSSL_SP_1024) && defined(WOLFCRYPT_HAVE_SAKKE)
    if (key->idx != ECC_CUSTOM_IDX && ecc_sets[key->idx].id == ECC_SAKKE_1) {
        return sp_ecc_check_key_1024(key->pubkey.x, key->pubkey.y,
            key->type == ECC_PRIVATEKEY ? key->k : NULL, key->heap);
    }
#endif
#endif

#ifndef WOLFSSL_SP_MATH
#ifndef HAVE_ECC_CHECK_PUBKEY_ORDER
    /* consider key check success on HW crypto
     * ex: ATECC508/608A, CryptoCell and Silabs
     *
     * consider key check success on Crypt Cb
     */
    err = MP_OKAY;

#else
    #ifdef USE_ECC_B_PARAM
        ALLOC_CURVE_SPECS(4, err);
    #else
        ALLOC_CURVE_SPECS(3, err);
        #ifndef WOLFSSL_SMALL_STACK
            b = &b_lcl;
        #else
            b = (mp_int*)XMALLOC(sizeof(mp_int), key->heap, DYNAMIC_TYPE_ECC);
            if (b == NULL) {
                FREE_CURVE_SPECS();
                return MEMORY_E;
            }
        #endif
        XMEMSET(b, 0, sizeof(mp_int));
    #endif

    #ifdef WOLFSSL_CAAM
    /* keys can be black encrypted ones which can not be checked like plain text
     * keys */
    if (key->blackKey > 0) {
        /* encrypted key was used */
        #ifdef WOLFSSL_SMALL_STACK
        XFREE(b, key->heap, DYNAMIC_TYPE_ECC);
        #endif
        FREE_CURVE_SPECS();
        return 0;
    }
    #endif

    /* SP 800-56Ar3, section 5.6.2.3.3, process step 1 */
    /* SP 800-56Ar3, section 5.6.2.3.4, process step 1 */
    /* pubkey point cannot be at infinity */
    if (wc_ecc_point_is_at_infinity(&key->pubkey)) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(b, key->heap, DYNAMIC_TYPE_ECC);
    #endif
        FREE_CURVE_SPECS();
        return ECC_INF_E;
    }

    /* load curve info */
    if (err == MP_OKAY)
        err = wc_ecc_curve_load(key->dp, &curve, (ECC_CURVE_FIELD_PRIME |
            ECC_CURVE_FIELD_AF | ECC_CURVE_FIELD_ORDER
#ifdef USE_ECC_B_PARAM
            | ECC_CURVE_FIELD_BF
#endif
    ));

#ifndef USE_ECC_B_PARAM
    /* load curve b parameter */
    if (err == MP_OKAY)
        err = mp_init(b);
    if (err == MP_OKAY)
        err = mp_read_radix(b, key->dp->Bf, MP_RADIX_HEX);
#else
    if (err == MP_OKAY)
        b = curve->Bf;
#endif

    /* SP 800-56Ar3, section 5.6.2.3.3, process step 2 */
    /* SP 800-56Ar3, section 5.6.2.3.4, process step 2 */
    /* Qx must be in the range [0, p-1] */
    if (err == MP_OKAY) {
        if ((mp_cmp(key->pubkey.x, curve->prime) != MP_LT) ||
                mp_isneg(key->pubkey.x)) {
            err = ECC_OUT_OF_RANGE_E;
        }
    }

    /* Qy must be in the range [0, p-1] */
    if (err == MP_OKAY) {
        if ((mp_cmp(key->pubkey.y, curve->prime) != MP_LT) ||
                mp_isneg(key->pubkey.y)) {
            err = ECC_OUT_OF_RANGE_E;
        }
    }

    /* SP 800-56Ar3, section 5.6.2.3.3, process step 3 */
    /* SP 800-56Ar3, section 5.6.2.3.4, process step 3 */
    /* make sure point is actually on curve */
    if (err == MP_OKAY)
        err = _ecc_is_point(&key->pubkey, curve->Af, b, curve->prime);

    if (!partial) {
        /* SP 800-56Ar3, section 5.6.2.3.3, process step 4 */
        /* pubkey * order must be at infinity */
        if (err == MP_OKAY)
            err = ecc_check_pubkey_order(key, &key->pubkey, curve->Af,
                    curve->prime, curve->order);
    }

    if (priv) {
        /* SP 800-56Ar3, section 5.6.2.1.2 */
        /* private keys must be in the range [1, n-1] */
        if ((err == MP_OKAY) && (key->type == ECC_PRIVATEKEY) &&
            (mp_iszero(key->k) || mp_isneg(key->k) ||
            (mp_cmp(key->k, curve->order) != MP_LT))
        #ifdef WOLFSSL_KCAPI_ECC
            && key->handle == NULL
        #endif
        ) {
            err = ECC_PRIV_KEY_E;
        }

    #if defined(WOLFSSL_VALIDATE_ECC_IMPORT) || defined(WOLFSSL_CAAM)
        /* SP 800-56Ar3, section 5.6.2.1.4, method (b) for ECC */
        /* private * base generator must equal pubkey */
        if (err == MP_OKAY && key->type == ECC_PRIVATEKEY)
            err = ecc_check_privkey_gen(key, curve->Af, curve->prime);
    #endif
    }

    wc_ecc_curve_free(curve);

#ifndef USE_ECC_B_PARAM
    mp_clear(b);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(b, key->heap, DYNAMIC_TYPE_ECC);
    #endif
#endif

    FREE_CURVE_SPECS();
#endif /* HAVE_ECC_CHECK_PUBKEY_ORDER */

#else
    err = WC_KEY_SIZE_E;
#endif /* !WOLFSSL_SP_MATH */
    (void)partial;
    (void)priv;
    return err;
}


/* perform sanity checks on ecc key validity, 0 on success */
WOLFSSL_ABI
int wc_ecc_check_key(ecc_key* key)
{
    int ret;
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = _ecc_validate_public_key(key, 0, 1);
    RESTORE_VECTOR_REGISTERS();
    return ret;
}


#ifdef HAVE_ECC_KEY_IMPORT
/* import public ECC key in ANSI X9.63 format */
int wc_ecc_import_x963_ex(const byte* in, word32 inLen, ecc_key* key,
                          int curve_id)
{
    int err = MP_OKAY;
#ifdef HAVE_COMP_KEY
    int compressed = 0;
#endif
    int keysize = 0;
    byte pointType;
#ifdef WOLFSSL_CRYPTOCELL
    const CRYS_ECPKI_Domain_t* pDomain;
    CRYS_ECPKI_BUILD_TempData_t tempBuff;
#endif
    if (in == NULL || key == NULL)
        return BAD_FUNC_ARG;

    /* must be odd */
    if ((inLen & 1) == 0) {
        return ECC_BAD_ARG_E;
    }

    /* make sure required variables are reset */
    wc_ecc_reset(key);

    /* init key */
    #ifdef ALT_ECC_SIZE
        key->pubkey.x = (mp_int*)&key->pubkey.xyz[0];
        key->pubkey.y = (mp_int*)&key->pubkey.xyz[1];
        key->pubkey.z = (mp_int*)&key->pubkey.xyz[2];
        alt_fp_init(key->pubkey.x);
        alt_fp_init(key->pubkey.y);
        alt_fp_init(key->pubkey.z);
        key->k = (mp_int*)key->ka;
        alt_fp_init(key->k);
    #else
        err = mp_init_multi(key->k,
                    key->pubkey.x, key->pubkey.y, key->pubkey.z, NULL, NULL);
    #endif
    if (err != MP_OKAY)
        return MEMORY_E;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    /* check for point type (4, 2, or 3) */
    pointType = in[0];
    if (pointType != ECC_POINT_UNCOMP && pointType != ECC_POINT_COMP_EVEN &&
                                         pointType != ECC_POINT_COMP_ODD) {
        err = ASN_PARSE_E;
    }

    if (pointType == ECC_POINT_COMP_EVEN || pointType == ECC_POINT_COMP_ODD) {
    #ifdef HAVE_COMP_KEY
        compressed = 1;
    #else
        err = NOT_COMPILED_IN;
    #endif
    }

    /* adjust to skip first byte */
    inLen -= 1;
    in += 1;

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
    /* For SECP256R1 only save raw public key for hardware */
    if (curve_id == ECC_SECP256R1 && inLen <= (word32)sizeof(key->pubkey_raw)) {
    #ifdef HAVE_COMP_KEY
        if (!compressed)
    #endif
            XMEMCPY(key->pubkey_raw, (byte*)in, inLen);
    }
#elif defined(WOLFSSL_KCAPI_ECC)
    XMEMCPY(key->pubkey_raw, (byte*)in, inLen);
#endif

    if (err == MP_OKAY) {
    #ifdef HAVE_COMP_KEY
        /* adjust inLen if compressed */
        if (compressed)
            inLen = inLen*2 + 1;  /* used uncompressed len */
    #endif

        /* determine key size */
        keysize = (int)(inLen>>1);
        err = wc_ecc_set_curve(key, keysize, curve_id);
        key->type = ECC_PUBLICKEY;
    }

    /* read data */
    if (err == MP_OKAY)
        err = mp_read_unsigned_bin(key->pubkey.x, in, (word32)keysize);

#ifdef HAVE_COMP_KEY
    if (err == MP_OKAY && compressed == 1) {   /* build y */
#if !defined(WOLFSSL_SP_MATH)
    #ifdef WOLFSSL_SMALL_STACK
        mp_int* t1 = NULL;
        mp_int* t2 = NULL;
    #else
        mp_int t1[1], t2[1];
    #endif
        int did_init = 0;

        DECLARE_CURVE_SPECS(3);
        ALLOC_CURVE_SPECS(3, err);

        #ifdef WOLFSSL_SMALL_STACK
        if (err == MP_OKAY) {
            t1 = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
            if (t1 == NULL) {
                err = MEMORY_E;
            }
        }
        if (err == MP_OKAY) {
            t2 = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
            if (t2 == NULL) {
                err = MEMORY_E;
            }
        }
        #endif
        if (err == MP_OKAY) {
            if (mp_init_multi(t1, t2, NULL, NULL, NULL, NULL) != MP_OKAY)
                err = MEMORY_E;
            else
                did_init = 1;
        }

        /* load curve info */
        if (err == MP_OKAY)
            err = wc_ecc_curve_load(key->dp, &curve,
                (ECC_CURVE_FIELD_PRIME | ECC_CURVE_FIELD_AF |
                 ECC_CURVE_FIELD_BF));

    #if defined(WOLFSSL_CUSTOM_CURVES) && \
        defined(WOLFSSL_VALIDATE_ECC_IMPORT)
        /* validate prime is prime for custom curves */
        if (err == MP_OKAY && key->idx == ECC_CUSTOM_IDX) {
            int isPrime = MP_NO;
            err = mp_prime_is_prime(curve->prime, 8, &isPrime);
            if (err == MP_OKAY && isPrime == MP_NO)
                err = MP_VAL;
        }
    #endif

        /* compute x^3 */
        if (err == MP_OKAY)
            err = mp_sqrmod(key->pubkey.x, curve->prime, t1);
        if (err == MP_OKAY)
            err = mp_mulmod(t1, key->pubkey.x, curve->prime, t1);

        /* compute x^3 + a*x */
        if (err == MP_OKAY)
            err = mp_mulmod(curve->Af, key->pubkey.x, curve->prime, t2);
        if (err == MP_OKAY)
            err = mp_add(t1, t2, t1);

        /* compute x^3 + a*x + b */
        if (err == MP_OKAY)
            err = mp_add(t1, curve->Bf, t1);

        /* compute sqrt(x^3 + a*x + b) */
        if (err == MP_OKAY)
            err = mp_sqrtmod_prime(t1, curve->prime, t2);

        /* adjust y */
        if (err == MP_OKAY) {
            if ((mp_isodd(t2) == MP_YES && pointType == ECC_POINT_COMP_ODD) ||
                (mp_isodd(t2) == MP_NO &&  pointType == ECC_POINT_COMP_EVEN)) {
                err = mp_mod(t2, curve->prime, t2);
            }
            else {
                err = mp_submod(curve->prime, t2, curve->prime, t2);
            }
            if (err == MP_OKAY)
                err = mp_copy(t2, key->pubkey.y);
        }

        if (did_init) {
            mp_clear(t2);
            mp_clear(t1);
        }
    #ifdef WOLFSSL_SMALL_STACK
        if (t1 != NULL) {
            XFREE(t1, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (t2 != NULL) {
            XFREE(t2, NULL, DYNAMIC_TYPE_BIGINT);
        }
    #endif

        wc_ecc_curve_free(curve);
        FREE_CURVE_SPECS();
#else
    #ifndef WOLFSSL_SP_NO_256
        if (key->dp->id == ECC_SECP256R1) {
            err = sp_ecc_uncompress_256(key->pubkey.x, pointType,
                key->pubkey.y);
        }
        else
#ifdef SM2_SP_IMPL_AVAILABLE
        #ifdef WOLFSSL_SM2
        if (key->dp->id == ECC_SM2P256V1) {
            sp_ecc_uncompress_sm2_256(key->pubkey.x, pointType, key->pubkey.y);
        }
        else
        #endif
#endif
    #endif
    #ifdef WOLFSSL_SP_384
        if (key->dp->id == ECC_SECP384R1) {
            err = sp_ecc_uncompress_384(key->pubkey.x, pointType,
                key->pubkey.y);
        }
        else
    #endif
    #ifdef WOLFSSL_SP_521
        if (key->dp->id == ECC_SECP521R1) {
            err = sp_ecc_uncompress_521(key->pubkey.x, pointType,
                key->pubkey.y);
        }
        else
    #endif
        {
            err = WC_KEY_SIZE_E;
        }
#endif
    }
#endif /* HAVE_COMP_KEY */

    if (err == MP_OKAY) {
    #ifdef HAVE_COMP_KEY
        if (compressed == 0)
    #endif
        {
            err = mp_read_unsigned_bin(key->pubkey.y, in + keysize,
                (word32)keysize);
        }
    }
    if (err == MP_OKAY)
        err = mp_set(key->pubkey.z, 1);

#ifdef WOLFSSL_CRYPTOCELL
    if (err == MP_OKAY) {
        pDomain = CRYS_ECPKI_GetEcDomain(cc310_mapCurve(key->dp->id));

        /* create public key from external key buffer */
        err = CRYS_ECPKI_BuildPublKeyFullCheck(pDomain,
                                               (byte*)in-1, /* re-adjust */
                                               inLen+1,     /* original input */
                                               &key->ctx.pubKey,
                                               &tempBuff);

        if (err != SA_SILIB_RET_OK){
            WOLFSSL_MSG("CRYS_ECPKI_BuildPublKeyFullCheck failed");
        }
    }
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    if (err == MP_OKAY)
        err = silabs_ecc_import(key, keysize, 1, 0);
#elif defined(WOLFSSL_SE050)
    if (err == MP_OKAY) {
        /* reset key ID, in case used before */
        key->keyId = 0;
        key->keyIdSet = 0;
    }
#elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    #ifndef HAVE_COMP_KEY
    if (err == MP_OKAY) {
    #else
    if (err == MP_OKAY && !compressed) {
    #endif
        buf_reverse(&key->keyRaw[0], &in[0], keysize);
        buf_reverse(&key->keyRaw[keysize], &in[keysize], keysize);
    }
#endif
#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    if (err == MP_OKAY)
        err = wc_ecc_check_key(key);
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    if (err == MP_OKAY) {
        err = wc_MAXQ10XX_EccSetKey(key, keysize);
    }
#endif

    if (err != MP_OKAY) {
        mp_clear(key->pubkey.x);
        mp_clear(key->pubkey.y);
        mp_clear(key->pubkey.z);
        mp_clear(key->k);
    }

    RESTORE_VECTOR_REGISTERS();

    return err;
}

WOLFSSL_ABI
int wc_ecc_import_x963(const byte* in, word32 inLen, ecc_key* key)
{
    return wc_ecc_import_x963_ex(in, inLen, key, ECC_CURVE_DEF);
}
#endif /* HAVE_ECC_KEY_IMPORT */

#ifdef HAVE_ECC_KEY_EXPORT

/* export ecc key to component form, d is optional if only exporting public
 * encType is WC_TYPE_UNSIGNED_BIN or WC_TYPE_HEX_STR
 * return MP_OKAY on success */
int wc_ecc_export_ex(ecc_key* key, byte* qx, word32* qxLen,
                 byte* qy, word32* qyLen, byte* d, word32* dLen, int encType)
{
    int err = 0;
    word32 keySz;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (wc_ecc_is_valid_idx(key->idx) == 0 || key->dp == NULL) {
        return ECC_BAD_ARG_E;
    }
    keySz = (word32)key->dp->size;

    /* private key, d */
    if (d != NULL) {
        if (dLen == NULL ||
            (key->type != ECC_PRIVATEKEY && key->type != ECC_PRIVATEKEY_ONLY))
            return BAD_FUNC_ARG;

    #if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
        /* Hardware cannot export private portion */
        return NOT_COMPILED_IN;
    #else
    #if defined(WOLFSSL_SECO_CAAM)
        if (key->blackKey > 0 && key->devId == WOLFSSL_SECO_DEVID) {
            /* Hardware cannot export private portion */
            WOLFSSL_MSG("Can not export private key from HSM");
            return NOT_COMPILED_IN;
        }
    #endif
    #if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
        if (key->blackKey == CAAM_BLACK_KEY_CCM) {
            if (*dLen < keySz + WC_CAAM_MAC_SZ) {
                *dLen = keySz + WC_CAAM_MAC_SZ;
                return BUFFER_E;
            }

            err = wc_export_int(key->k, d, dLen, keySz + WC_CAAM_MAC_SZ,
                encType);
            *dLen = keySz + WC_CAAM_MAC_SZ;
        }
        else if (encType == WC_TYPE_BLACK_KEY &&
                key->blackKey != CAAM_BLACK_KEY_ECB &&
                key->blackKey > 0) {
            if (*dLen < keySz + WC_CAAM_MAC_SZ) {
                *dLen = keySz + WC_CAAM_MAC_SZ;
                return BUFFER_E;
            }

            if (key->blackKey != CAAM_BLACK_KEY_CCM) {
                if (caamReadPartition(key->blackKey, d, keySz + WC_CAAM_MAC_SZ) != 0)
                    return WC_HW_E;
            }

            *dLen = keySz + WC_CAAM_MAC_SZ;
        }
        else
    #endif
        {
            err = wc_export_int(key->k, d, dLen, keySz, encType);
            if (err != MP_OKAY)
                return err;
        }
    #endif
    }

    /* public x component */
    if (qx != NULL) {
        if (qxLen == NULL || key->type == ECC_PRIVATEKEY_ONLY)
            return BAD_FUNC_ARG;

        err = wc_export_int(key->pubkey.x, qx, qxLen, keySz, encType);
        if (err != MP_OKAY)
            return err;
    }

    /* public y component */
    if (qy != NULL) {
        if (qyLen == NULL || key->type == ECC_PRIVATEKEY_ONLY)
            return BAD_FUNC_ARG;

        err = wc_export_int(key->pubkey.y, qy, qyLen, keySz, encType);
        if (err != MP_OKAY)
            return err;
    }

    return err;
}


/* export ecc private key only raw, outLen is in/out size as unsigned bin
   return MP_OKAY on success */
WOLFSSL_ABI
int wc_ecc_export_private_only(ecc_key* key, byte* out, word32* outLen)
{
    if (out == NULL || outLen == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    /* check if black key in secure memory */
    if ((key->blackKey != CAAM_BLACK_KEY_CCM &&
         key->blackKey != CAAM_BLACK_KEY_ECB) && key->blackKey > 0) {
        return wc_ecc_export_ex(key, NULL, NULL, NULL, NULL, out, outLen,
            WC_TYPE_BLACK_KEY);
    }
#endif

    return wc_ecc_export_ex(key, NULL, NULL, NULL, NULL, out, outLen,
        WC_TYPE_UNSIGNED_BIN);
}

/* export public key to raw elements including public (Qx,Qy) as unsigned bin
 * return MP_OKAY on success, negative on error */
int wc_ecc_export_public_raw(ecc_key* key, byte* qx, word32* qxLen,
                             byte* qy, word32* qyLen)
{
    if (qx == NULL || qxLen == NULL || qy == NULL || qyLen == NULL) {
        return BAD_FUNC_ARG;
    }

    return wc_ecc_export_ex(key, qx, qxLen, qy, qyLen, NULL, NULL,
        WC_TYPE_UNSIGNED_BIN);
}

/* export ecc key to raw elements including public (Qx,Qy) and
 *   private (d) as unsigned bin
 * return MP_OKAY on success, negative on error */
int wc_ecc_export_private_raw(ecc_key* key, byte* qx, word32* qxLen,
                              byte* qy, word32* qyLen, byte* d, word32* dLen)
{
    return wc_ecc_export_ex(key, qx, qxLen, qy, qyLen, d, dLen,
        WC_TYPE_UNSIGNED_BIN);
}

#endif /* HAVE_ECC_KEY_EXPORT */

#ifdef HAVE_ECC_KEY_IMPORT
/* import private key, public part optional if (pub) passed as NULL */
int wc_ecc_import_private_key_ex(const byte* priv, word32 privSz,
                                 const byte* pub, word32 pubSz, ecc_key* key,
                                 int curve_id)
{
    int ret;
#ifdef WOLFSSL_CRYPTOCELL
    const CRYS_ECPKI_Domain_t* pDomain;
#endif
    if (key == NULL || priv == NULL)
        return BAD_FUNC_ARG;

    /* public optional, NULL if only importing private */
    if (pub != NULL) {
    #ifndef NO_ASN
        word32 idx = 0;
        ret = wc_ecc_import_x963_ex(pub, pubSz, key, curve_id);
        if (ret < 0)
            ret = wc_EccPublicKeyDecode(pub, &idx, key, pubSz);
        key->type = ECC_PRIVATEKEY;
    #else
        (void)pubSz;
        ret = NOT_COMPILED_IN;
    #endif
    }
    else {
        /* make sure required variables are reset */
        wc_ecc_reset(key);

        /* set key size */
        ret = wc_ecc_set_curve(key, (int)privSz, curve_id);
        key->type = ECC_PRIVATEKEY_ONLY;
    }

    if (ret != 0)
        return ret;

#ifdef WOLFSSL_CRYPTOCELL
    pDomain = CRYS_ECPKI_GetEcDomain(cc310_mapCurve(key->dp->id));
    /* import private key - priv checked for NULL at top */
    if (priv[0] != '\0') {

        /* Create private key from external key buffer*/
        ret = CRYS_ECPKI_BuildPrivKey(pDomain,
                                      priv,
                                      privSz,
                                      &key->ctx.privKey);

        if (ret != SA_SILIB_RET_OK) {
            WOLFSSL_MSG("CRYS_ECPKI_BuildPrivKey failed");
            return ret;
        }

        ret = mp_read_unsigned_bin(key->k, priv, privSz);
    }
#elif defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
    if ((wc_ecc_size(key) + WC_CAAM_MAC_SZ) == (int)privSz) {
    #ifdef WOLFSSL_CAAM_BLACK_KEY_SM
        int part = caamFindUnusedPartition();
        if (part >= 0) {
            CAAM_ADDRESS vaddr = caamGetPartition(part, privSz*3);
            if (vaddr == 0) {
                WOLFSSL_MSG("Unable to get partition");
                return MEMORY_E;
            }

            key->partNum  = part;
            key->blackKey = (word32)vaddr;
            if (caamWriteToPartition(vaddr, priv, privSz) != 0)
                return WC_HW_E;

            if (pub != NULL) {
                /* +1 to account for x963 compressed bit */
                if (caamWriteToPartition(vaddr + privSz, pub + 1, pubSz - 1) != 0)
                    return WC_HW_E;
                key->securePubKey = (word32)vaddr + privSz;
            }
        }
        else {
            WOLFSSL_MSG("Unable to find an unused partition");
            return MEMORY_E;
        }
    #else
        key->blackKey = CAAM_BLACK_KEY_CCM;
        ret = mp_read_unsigned_bin(key->k, priv, privSz);
    #endif
    }
    else {
        key->blackKey = 0;
        ret = mp_read_unsigned_bin(key->k, priv, privSz);

        /* If using AES-ECB encrypted black keys check here if key is valid,
         * if not valid than assume is an encrypted key. A public key is needed
         * for testing validity. */
        if (key->devId == WOLFSSL_CAAM_DEVID && (
            wc_ecc_get_curve_id(key->idx) == ECC_SECP256R1 ||
            wc_ecc_get_curve_id(key->idx) == ECC_SECP384R1)) {
            if ((pub != NULL) && (ret == MP_OKAY) &&
                (_ecc_validate_public_key(key, 1, 1) != MP_OKAY)) {
                key->blackKey = CAAM_BLACK_KEY_ECB;
            }
            else if ((pub == NULL) && (ret == MP_OKAY)) {
                WOLFSSL_MSG("Assuming encrypted key with no public key to check");
                key->blackKey = CAAM_BLACK_KEY_ECB;
            }
            else {
                WOLFSSL_MSG("Importing key that is not a black key!");
            }
        }
    }
#else

#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
#endif

    ret = mp_read_unsigned_bin(key->k, priv, privSz);
#ifdef HAVE_WOLF_BIGINT
    if (ret == 0 &&
                  wc_bigint_from_unsigned_bin(&key->k->raw, priv, privSz) != 0) {
        mp_clear(key->k);
        ret = ASN_GETINT_E;
    }
#endif /* HAVE_WOLF_BIGINT */
#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    if (ret == 0) {
    #ifdef WOLFSSL_SMALL_STACK
        mp_int* order = NULL;
    #else
        mp_int order[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        order = (mp_int*)XMALLOC(sizeof(mp_int), key->heap, DYNAMIC_TYPE_ECC);
        if (order == NULL) {
            ret = MEMORY_E;
        }
    #endif

        if (ret == 0) {
            ret = mp_init(order);
        }
        if (ret == 0) {
            ret = mp_read_radix(order, key->dp->order, MP_RADIX_HEX);
        }
        if ((ret == 0) && (mp_cmp(key->k, order) != MP_LT)) {
            ret = ECC_PRIV_KEY_E;
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(order, key->heap, DYNAMIC_TYPE_ECC);
    #endif
    }
#endif /* WOLFSSL_VALIDATE_ECC_IMPORT */

#endif /* WOLFSSL_CRYPTOCELL */

#if defined(WOLFSSL_VALIDATE_ECC_IMPORT) && !defined(WOLFSSL_KCAPI_ECC)
    if ((pub != NULL) && (ret == MP_OKAY))
        /* public key needed to perform key validation */
        ret = _ecc_validate_public_key(key, 1, 1);

#endif

#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    RESTORE_VECTOR_REGISTERS();
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    if (ret == 0) {
        ret = wc_MAXQ10XX_EccSetKey(key, key->dp->size);
    }
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    if (ret == 0) {
        ret = silabs_ecc_import(key, key->dp->size, (pub != NULL), 1);
    }
#endif

    return ret;
}

/* ecc private key import, public key in ANSI X9.63 format, private raw */
WOLFSSL_ABI
int wc_ecc_import_private_key(const byte* priv, word32 privSz, const byte* pub,
                           word32 pubSz, ecc_key* key)
{
    return wc_ecc_import_private_key_ex(priv, privSz, pub, pubSz, key,
                                                                ECC_CURVE_DEF);
}
#endif /* HAVE_ECC_KEY_IMPORT */

#ifndef NO_ASN
/**
   Convert ECC R,S to signature
   r       R component of signature
   s       S component of signature
   out     DER-encoded ECDSA signature
   outlen  [in/out] output buffer size, output signature size
   return  MP_OKAY on success
*/
WOLFSSL_ABI
int wc_ecc_rs_to_sig(const char* r, const char* s, byte* out, word32* outlen)
{
    int err;
#ifdef WOLFSSL_SMALL_STACK
    mp_int* rtmp = NULL;
    mp_int* stmp = NULL;
#else
    mp_int  rtmp[1];
    mp_int  stmp[1];
#endif

    if (r == NULL || s == NULL || out == NULL || outlen == NULL)
        return ECC_BAD_ARG_E;

#ifdef WOLFSSL_SMALL_STACK
    rtmp = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
    if (rtmp == NULL)
        return MEMORY_E;
    stmp = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
    if (stmp == NULL) {
        XFREE(rtmp, NULL, DYNAMIC_TYPE_ECC);
        return MEMORY_E;
    }
#endif

    err = mp_init_multi(rtmp, stmp, NULL, NULL, NULL, NULL);
    if (err != MP_OKAY) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(stmp, NULL, DYNAMIC_TYPE_ECC);
        XFREE(rtmp, NULL, DYNAMIC_TYPE_ECC);
    #endif
        return err;
    }

    err = mp_read_radix(rtmp, r, MP_RADIX_HEX);
    if (err == MP_OKAY)
        err = mp_read_radix(stmp, s, MP_RADIX_HEX);

    if (err == MP_OKAY) {
        if (mp_iszero(rtmp) == MP_YES || mp_iszero(stmp) == MP_YES)
            err = MP_ZERO_E;
    }
    if (err == MP_OKAY) {
        if (mp_isneg(rtmp) == MP_YES || mp_isneg(stmp) == MP_YES) {
            err = MP_READ_E;
        }
    }

    /* convert mp_ints to ECDSA sig, initializes rtmp and stmp internally */
    if (err == MP_OKAY)
        err = StoreECC_DSA_Sig(out, outlen, rtmp, stmp);

    mp_clear(rtmp);
    mp_clear(stmp);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(stmp, NULL, DYNAMIC_TYPE_ECC);
    XFREE(rtmp, NULL, DYNAMIC_TYPE_ECC);
#endif

    return err;
}

/**
   Convert ECC R,S raw unsigned bin to signature
   r       R component of signature
   rSz     R size
   s       S component of signature
   sSz     S size
   out     DER-encoded ECDSA signature
   outlen  [in/out] output buffer size, output signature size
   return  MP_OKAY on success
*/
int wc_ecc_rs_raw_to_sig(const byte* r, word32 rSz, const byte* s, word32 sSz,
    byte* out, word32* outlen)
{
    if (r == NULL || s == NULL || out == NULL || outlen == NULL)
        return ECC_BAD_ARG_E;

    /* convert mp_ints to ECDSA sig, initializes rtmp and stmp internally */
    return StoreECC_DSA_Sig_Bin(out, outlen, r, rSz, s, sSz);
}

/**
   Convert ECC signature to R,S
   sig     DER-encoded ECDSA signature
   sigLen  length of signature in octets
   r       R component of signature
   rLen    [in/out] output "r" buffer size, output "r" size
   s       S component of signature
   sLen    [in/out] output "s" buffer size, output "s" size
   return  MP_OKAY on success, negative on error
*/
int wc_ecc_sig_to_rs(const byte* sig, word32 sigLen, byte* r, word32* rLen,
                     byte* s, word32* sLen)
{
    if (sig == NULL || r == NULL || rLen == NULL || s == NULL || sLen == NULL)
        return ECC_BAD_ARG_E;

    return DecodeECC_DSA_Sig_Bin(sig, sigLen, r, rLen, s, sLen);
}
#endif /* !NO_ASN */

#ifdef HAVE_ECC_KEY_IMPORT
static int wc_ecc_import_raw_private(ecc_key* key, const char* qx,
          const char* qy, const char* d, int curve_id, int encType)
{
    int err = MP_OKAY;
#if defined(WOLFSSL_CRYPTOCELL) && !defined(WOLFSSL_ATECC508A) && \
    !defined(WOLFSSL_ATECC608A)
    const CRYS_ECPKI_Domain_t* pDomain;
    CRYS_ECPKI_BUILD_TempData_t tempBuff;
    byte keyRaw[ECC_MAX_CRYPTO_HW_SIZE*2 + 1];
#endif

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A) || \
    defined(WOLFSSL_CRYPTOCELL)
    word32 keySz = 0;
#endif

    /* if d is NULL, only import as public key using Qx,Qy */
    if (key == NULL || qx == NULL || qy == NULL) {
        return BAD_FUNC_ARG;
    }

    /* make sure required variables are reset */
    wc_ecc_reset(key);

    /* set curve type and index */
    err = wc_ecc_set_curve(key, 0, curve_id);
    if (err != 0) {
        return err;
    }

    /* init key */
#ifdef ALT_ECC_SIZE
    key->pubkey.x = (mp_int*)&key->pubkey.xyz[0];
    key->pubkey.y = (mp_int*)&key->pubkey.xyz[1];
    key->pubkey.z = (mp_int*)&key->pubkey.xyz[2];
    alt_fp_init(key->pubkey.x);
    alt_fp_init(key->pubkey.y);
    alt_fp_init(key->pubkey.z);
    key->k = (mp_int*)key->ka;
    alt_fp_init(key->k);
#else
    err = mp_init_multi(key->k, key->pubkey.x, key->pubkey.y, key->pubkey.z,
                                                                  NULL, NULL);
#endif
    if (err != MP_OKAY)
        return MEMORY_E;

    /* read Qx */
    if (err == MP_OKAY) {
        if (encType == WC_TYPE_HEX_STR)
            err = mp_read_radix(key->pubkey.x, qx, MP_RADIX_HEX);
        else
            err = mp_read_unsigned_bin(key->pubkey.x, (const byte*)qx,
                (word32)key->dp->size);

        if (mp_isneg(key->pubkey.x)) {
            WOLFSSL_MSG("Invalid Qx");
            err = BAD_FUNC_ARG;
        }
        if (mp_unsigned_bin_size(key->pubkey.x) > key->dp->size) {
            err = BAD_FUNC_ARG;
        }
    }

    /* read Qy */
    if (err == MP_OKAY) {
        if (encType == WC_TYPE_HEX_STR)
            err = mp_read_radix(key->pubkey.y, qy, MP_RADIX_HEX);
        else
            err = mp_read_unsigned_bin(key->pubkey.y, (const byte*)qy,
                (word32)key->dp->size);

        if (mp_isneg(key->pubkey.y)) {
            WOLFSSL_MSG("Invalid Qy");
            err = BAD_FUNC_ARG;
        }
        if (mp_unsigned_bin_size(key->pubkey.y) > key->dp->size) {
            err = BAD_FUNC_ARG;
        }
    }

    if (err == MP_OKAY) {
        if (mp_iszero(key->pubkey.x) && mp_iszero(key->pubkey.y)) {
            WOLFSSL_MSG("Invalid Qx and Qy");
            err = ECC_INF_E;
        }
    }

    if (err == MP_OKAY)
        err = mp_set(key->pubkey.z, 1);

#if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
    /* For SECP256R1 only save raw public key for hardware */
    if (err == MP_OKAY && curve_id == ECC_SECP256R1) {
        keySz = key->dp->size;
        err = wc_export_int(key->pubkey.x, key->pubkey_raw,
            &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
        if (err == MP_OKAY)
            err = wc_export_int(key->pubkey.y, &key->pubkey_raw[keySz],
                &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
    }
#elif defined(WOLFSSL_CRYPTOCELL)
    if (err == MP_OKAY) {
        keyRaw[0] = ECC_POINT_UNCOMP;
        keySz = (word32)key->dp->size;
        err = wc_export_int(key->pubkey.x, &keyRaw[1], &keySz, keySz,
            WC_TYPE_UNSIGNED_BIN);
        if (err == MP_OKAY) {
            err = wc_export_int(key->pubkey.y, &keyRaw[1+keySz],
                &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
        }

        if (err == MP_OKAY) {
            pDomain = CRYS_ECPKI_GetEcDomain(cc310_mapCurve(key->dp->id));

            /* create public key from external key buffer */
            err = CRYS_ECPKI_BuildPublKeyFullCheck(pDomain,
                                                   keyRaw,
                                                   keySz*2 + 1,
                                                   &key->ctx.pubKey,
                                                   &tempBuff);
        }

        if (err != SA_SILIB_RET_OK){
            WOLFSSL_MSG("CRYS_ECPKI_BuildPublKeyFullCheck failed");
            return err;
        }
    }
#elif defined(WOLFSSL_KCAPI_ECC)
    if (err == MP_OKAY) {
        word32 keySz = key->dp->size;
        err = wc_export_int(key->pubkey.x, key->pubkey_raw,
            &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
        if (err == MP_OKAY) {
            err = wc_export_int(key->pubkey.y,
                &key->pubkey_raw[keySz], &keySz, keySz,
                WC_TYPE_UNSIGNED_BIN);
        }
    }
#elif defined(WOLFSSL_XILINX_CRYPT_VERSAL)
    if (err == MP_OKAY) {
        const word32 keySize = key->dp->size;
        word32 bufSize = sizeof(key->keyRaw);
        err = wc_export_int(key->pubkey.x, key->keyRaw, &bufSize, keySize,
                            WC_TYPE_UNSIGNED_BIN);
        if (err == MP_OKAY) {
            const word32 offset = bufSize;
            bufSize = sizeof(key->keyRaw) - offset;
            err = wc_export_int(key->pubkey.y, &key->keyRaw[offset], &bufSize,
                                keySize, WC_TYPE_UNSIGNED_BIN);
        }
        if (err == MP_OKAY) {
            mp_reverse(key->keyRaw, keySize);
            mp_reverse(&key->keyRaw[keySize], keySize);
            WOLFSSL_XIL_DCACHE_FLUSH_RANGE(XIL_CAST_U64(key->keyRaw),
                                           keySize * 2);
#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
            err = XSecure_EllipticValidateKey(&(key->xSec.cinst),
                                              xil_curve_type[key->dp->id],
                                              XIL_CAST_U64(key->keyRaw));
            if (err) {
                WOLFSSL_XIL_ERROR("Validation of ECC key failed", err);
                err = WC_HW_E;
            }
#endif
        }
    }
#endif

#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    SAVE_VECTOR_REGISTERS(return _svr_ret;);
#endif

    /* import private key */
    if (err == MP_OKAY) {
        if (d != NULL) {
        #if defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_ATECC608A)
            /* Hardware doesn't support loading private key */
            err = NOT_COMPILED_IN;

        #elif defined(WOLFSSL_CRYPTOCELL)
            key->type = ECC_PRIVATEKEY;

            if (encType == WC_TYPE_HEX_STR)
                err = mp_read_radix(key->k, d, MP_RADIX_HEX);
            else
                err = mp_read_unsigned_bin(key->k, (const byte*)d,
                    key->dp->size);
            if (err == MP_OKAY) {
                err = wc_export_int(key->k, &keyRaw[0], &keySz, keySz,
                    WC_TYPE_UNSIGNED_BIN);
            }

            if (err == MP_OKAY) {
                /* Create private key from external key buffer*/
                err = CRYS_ECPKI_BuildPrivKey(pDomain,
                                              keyRaw,
                                              keySz,
                                              &key->ctx.privKey);

                if (err != SA_SILIB_RET_OK){
                    WOLFSSL_MSG("CRYS_ECPKI_BuildPrivKey failed");
                    return err;
                }
            }

        #else
            key->type = ECC_PRIVATEKEY;
            if (encType == WC_TYPE_HEX_STR)
                err = mp_read_radix(key->k, d, MP_RADIX_HEX);
            else {
            #if defined(WOLFSSL_QNX_CAAM) || defined(WOLFSSL_IMXRT1170_CAAM)
                if (key->blackKey == CAAM_BLACK_KEY_CCM) {
                    err = mp_read_unsigned_bin(key->k, (const byte*)d,
                    key->dp->size + WC_CAAM_MAC_SZ);
                }
                else
            #endif /* WOLFSSL_QNX_CAAM */
                {
                    err = mp_read_unsigned_bin(key->k, (const byte*)d,
                        (word32)key->dp->size);
                }
            }
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
            if (err == MP_OKAY) {
                const word32 key_size = key->dp->size;
                word32 buf_size = key_size;
                err = wc_export_int(key->k, key->privKey,
                                    &buf_size, key_size, WC_TYPE_UNSIGNED_BIN);
                mp_reverse(key->privKey, key_size);
            }
#endif

        #endif /* #else-case of custom HW-specific implementations */
            if (mp_iszero(key->k) || mp_isneg(key->k)) {
                WOLFSSL_MSG("Invalid private key");
                err = BAD_FUNC_ARG;
            }
        } else {
            key->type = ECC_PUBLICKEY;
        }
    }

#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    if (err == MP_OKAY) {
        err = wc_ecc_check_key(key);
        if (err == IS_POINT_E && (mp_iszero(key->pubkey.x) ||
                                  mp_iszero(key->pubkey.y))) {
            err = BAD_FUNC_ARG;
        }
    }
#endif

#ifdef WOLFSSL_VALIDATE_ECC_IMPORT
    RESTORE_VECTOR_REGISTERS();
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    if (err == MP_OKAY) {
        err = wc_MAXQ10XX_EccSetKey(key, key->dp->size);
    }
#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    if (err == MP_OKAY) {
        err = silabs_ecc_import(key, key->dp->size, 1, (d != NULL));
    }
#endif

    if (err != MP_OKAY) {
        mp_clear(key->pubkey.x);
        mp_clear(key->pubkey.y);
        mp_clear(key->pubkey.z);
        mp_clear(key->k);
#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)
        ForceZero(key->keyRaw, sizeof(key->keyRaw));
#endif
    }

    return err;
}

/**
   Import raw ECC key
   key       The destination ecc_key structure
   qx        x component of the public key, as ASCII hex string
   qy        y component of the public key, as ASCII hex string
   d         private key, as ASCII hex string, optional if importing public
             key only
   dp        Custom ecc_set_type
   return    MP_OKAY on success
*/
int wc_ecc_import_raw_ex(ecc_key* key, const char* qx, const char* qy,
                   const char* d, int curve_id)
{
    return wc_ecc_import_raw_private(key, qx, qy, d, curve_id,
        WC_TYPE_HEX_STR);

}

/* Import x, y and optional private (d) as unsigned binary */
int wc_ecc_import_unsigned(ecc_key* key, const byte* qx, const byte* qy,
                   const byte* d, int curve_id)
{
    return wc_ecc_import_raw_private(key, (const char*)qx, (const char*)qy,
        (const char*)d, curve_id, WC_TYPE_UNSIGNED_BIN);
}

/**
   Import raw ECC key
   key       The destination ecc_key structure
   qx        x component of the public key, as ASCII hex string
   qy        y component of the public key, as ASCII hex string
   d         private key, as ASCII hex string, optional if importing public
             key only
   curveName ECC curve name, from ecc_sets[]
   return    MP_OKAY on success
*/
WOLFSSL_ABI
int wc_ecc_import_raw(ecc_key* key, const char* qx, const char* qy,
                   const char* d, const char* curveName)
{
    int err, x;

    /* if d is NULL, only import as public key using Qx,Qy */
    if (key == NULL || qx == NULL || qy == NULL || curveName == NULL) {
        return BAD_FUNC_ARG;
    }

    /* set curve type and index */
    for (x = 0; ecc_sets[x].size != 0; x++) {
        if (XSTRNCMP(ecc_sets[x].name, curveName,
                     XSTRLEN(curveName)) == 0) {
            break;
        }
    }

    if (ecc_sets[x].size == 0) {
        WOLFSSL_MSG("ecc_set curve name not found");
        err = ASN_PARSE_E;
    } else {
        return wc_ecc_import_raw_private(key, qx, qy, d, ecc_sets[x].id,
            WC_TYPE_HEX_STR);
    }

    return err;
}
#endif /* HAVE_ECC_KEY_IMPORT */

#if defined(HAVE_ECC_ENCRYPT) && !defined(WOLFSSL_ECIES_OLD)
/* public key size in octets */
static int ecc_public_key_size(ecc_key* key, word32* sz)
{
    if (key == NULL || key->dp == NULL)
        return BAD_FUNC_ARG;

    /* 'Uncompressed' | x | y */
    *sz = 1 + 2 * (word32)key->dp->size;

    return 0;
}
#endif

/* key size in octets */
WOLFSSL_ABI
int wc_ecc_size(ecc_key* key)
{
    if (key == NULL || key->dp == NULL)
        return 0;

    return key->dp->size;
}

/* maximum signature size based on key size */
WOLFSSL_ABI
int wc_ecc_sig_size_calc(int sz)
{
    int maxSigSz = 0;

    /* calculate based on key bits */
    /* maximum possible signature header size is 7 bytes plus 2 bytes padding */
    maxSigSz = (sz * 2) + SIG_HEADER_SZ + ECC_MAX_PAD_SZ;

    /* if total length is less than 128 + SEQ(1)+LEN(1) then subtract 1 */
    if (maxSigSz < (128 + 2)) {
        maxSigSz -= 1;
    }

    return maxSigSz;
}

/* maximum signature size based on actual key curve */
WOLFSSL_ABI
int wc_ecc_sig_size(const ecc_key* key)
{
    int maxSigSz;
    int orderBits, keySz;

    if (key == NULL || key->dp == NULL)
        return 0;

    /* the signature r and s will always be less than order */
    /* if the order MSB (top bit of byte) is set then ASN encoding needs
        extra byte for r and s, so add 2 */
    keySz = key->dp->size;
    orderBits = wc_ecc_get_curve_order_bit_count(key->dp);
    if (orderBits > keySz * 8) {
        keySz = (orderBits + 7) / 8;
    }
    /* maximum possible signature header size is 7 bytes */
    maxSigSz = (keySz * 2) + SIG_HEADER_SZ;
    if ((orderBits % 8) == 0) {
        /* MSB can be set, so add 2 */
        maxSigSz += ECC_MAX_PAD_SZ;
    }
    /* if total length is less than 128 + SEQ(1)+LEN(1) then subtract 1 */
    if (maxSigSz < (128 + 2)) {
        maxSigSz -= 1;
    }

    return maxSigSz;
}


#ifdef FP_ECC

/* fixed point ECC cache */
/* number of entries in the cache */
#ifndef FP_ENTRIES
    #define FP_ENTRIES 15
#endif

/* number of bits in LUT */
#ifndef FP_LUT
    #define FP_LUT     8U
#endif

#ifdef ECC_SHAMIR
    /* Sharmir requires a bigger LUT, TAO */
    #if (FP_LUT > 12) || (FP_LUT < 4)
        #error FP_LUT must be between 4 and 12 inclusively
    #endif
#else
    #if (FP_LUT > 12) || (FP_LUT < 2)
        #error FP_LUT must be between 2 and 12 inclusively
    #endif
#endif


#if !defined(WOLFSSL_SP_MATH)

/** Our FP cache */
typedef struct {
   ecc_point* g;               /* cached COPY of base point */
   ecc_point* LUT[1U<<FP_LUT]; /* fixed point lookup */
   int        LUT_set;         /* flag to determine if the LUT has been computed */
   mp_int     mu;              /* copy of the montgomery constant */
   int        lru_count;       /* amount of times this entry has been used */
   int        lock;            /* flag to indicate cache eviction */
                               /* permitted (0) or not (1) */
} fp_cache_t;

/* if HAVE_THREAD_LS this cache is per thread, no locking needed */
static THREAD_LS_T fp_cache_t fp_cache[FP_ENTRIES];

#ifndef HAVE_THREAD_LS
    static volatile int initMutex = 0;  /* prevent multiple mutex inits */
    static wolfSSL_Mutex ecc_fp_lock;
#endif /* HAVE_THREAD_LS */

/* simple table to help direct the generation of the LUT */
static const struct {
   int ham, terma, termb;
} lut_orders[] = {
   { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 }, { 2, 1, 2 }, { 1, 0, 0 }, { 2, 1, 4 }, { 2, 2, 4 }, { 3, 3, 4 },
   { 1, 0, 0 }, { 2, 1, 8 }, { 2, 2, 8 }, { 3, 3, 8 }, { 2, 4, 8 }, { 3, 5, 8 }, { 3, 6, 8 }, { 4, 7, 8 },
   { 1, 0, 0 }, { 2, 1, 16 }, { 2, 2, 16 }, { 3, 3, 16 }, { 2, 4, 16 }, { 3, 5, 16 }, { 3, 6, 16 }, { 4, 7, 16 },
   { 2, 8, 16 }, { 3, 9, 16 }, { 3, 10, 16 }, { 4, 11, 16 }, { 3, 12, 16 }, { 4, 13, 16 }, { 4, 14, 16 }, { 5, 15, 16 },
   { 1, 0, 0 }, { 2, 1, 32 }, { 2, 2, 32 }, { 3, 3, 32 }, { 2, 4, 32 }, { 3, 5, 32 }, { 3, 6, 32 }, { 4, 7, 32 },
   { 2, 8, 32 }, { 3, 9, 32 }, { 3, 10, 32 }, { 4, 11, 32 }, { 3, 12, 32 }, { 4, 13, 32 }, { 4, 14, 32 }, { 5, 15, 32 },
   { 2, 16, 32 }, { 3, 17, 32 }, { 3, 18, 32 }, { 4, 19, 32 }, { 3, 20, 32 }, { 4, 21, 32 }, { 4, 22, 32 }, { 5, 23, 32 },
   { 3, 24, 32 }, { 4, 25, 32 }, { 4, 26, 32 }, { 5, 27, 32 }, { 4, 28, 32 }, { 5, 29, 32 }, { 5, 30, 32 }, { 6, 31, 32 },
#if FP_LUT > 6
   { 1, 0, 0 }, { 2, 1, 64 }, { 2, 2, 64 }, { 3, 3, 64 }, { 2, 4, 64 }, { 3, 5, 64 }, { 3, 6, 64 }, { 4, 7, 64 },
   { 2, 8, 64 }, { 3, 9, 64 }, { 3, 10, 64 }, { 4, 11, 64 }, { 3, 12, 64 }, { 4, 13, 64 }, { 4, 14, 64 }, { 5, 15, 64 },
   { 2, 16, 64 }, { 3, 17, 64 }, { 3, 18, 64 }, { 4, 19, 64 }, { 3, 20, 64 }, { 4, 21, 64 }, { 4, 22, 64 }, { 5, 23, 64 },
   { 3, 24, 64 }, { 4, 25, 64 }, { 4, 26, 64 }, { 5, 27, 64 }, { 4, 28, 64 }, { 5, 29, 64 }, { 5, 30, 64 }, { 6, 31, 64 },
   { 2, 32, 64 }, { 3, 33, 64 }, { 3, 34, 64 }, { 4, 35, 64 }, { 3, 36, 64 }, { 4, 37, 64 }, { 4, 38, 64 }, { 5, 39, 64 },
   { 3, 40, 64 }, { 4, 41, 64 }, { 4, 42, 64 }, { 5, 43, 64 }, { 4, 44, 64 }, { 5, 45, 64 }, { 5, 46, 64 }, { 6, 47, 64 },
   { 3, 48, 64 }, { 4, 49, 64 }, { 4, 50, 64 }, { 5, 51, 64 }, { 4, 52, 64 }, { 5, 53, 64 }, { 5, 54, 64 }, { 6, 55, 64 },
   { 4, 56, 64 }, { 5, 57, 64 }, { 5, 58, 64 }, { 6, 59, 64 }, { 5, 60, 64 }, { 6, 61, 64 }, { 6, 62, 64 }, { 7, 63, 64 },
#if FP_LUT > 7
   { 1, 0, 0 }, { 2, 1, 128 }, { 2, 2, 128 }, { 3, 3, 128 }, { 2, 4, 128 }, { 3, 5, 128 }, { 3, 6, 128 }, { 4, 7, 128 },
   { 2, 8, 128 }, { 3, 9, 128 }, { 3, 10, 128 }, { 4, 11, 128 }, { 3, 12, 128 }, { 4, 13, 128 }, { 4, 14, 128 }, { 5, 15, 128 },
   { 2, 16, 128 }, { 3, 17, 128 }, { 3, 18, 128 }, { 4, 19, 128 }, { 3, 20, 128 }, { 4, 21, 128 }, { 4, 22, 128 }, { 5, 23, 128 },
   { 3, 24, 128 }, { 4, 25, 128 }, { 4, 26, 128 }, { 5, 27, 128 }, { 4, 28, 128 }, { 5, 29, 128 }, { 5, 30, 128 }, { 6, 31, 128 },
   { 2, 32, 128 }, { 3, 33, 128 }, { 3, 34, 128 }, { 4, 35, 128 }, { 3, 36, 128 }, { 4, 37, 128 }, { 4, 38, 128 }, { 5, 39, 128 },
   { 3, 40, 128 }, { 4, 41, 128 }, { 4, 42, 128 }, { 5, 43, 128 }, { 4, 44, 128 }, { 5, 45, 128 }, { 5, 46, 128 }, { 6, 47, 128 },
   { 3, 48, 128 }, { 4, 49, 128 }, { 4, 50, 128 }, { 5, 51, 128 }, { 4, 52, 128 }, { 5, 53, 128 }, { 5, 54, 128 }, { 6, 55, 128 },
   { 4, 56, 128 }, { 5, 57, 128 }, { 5, 58, 128 }, { 6, 59, 128 }, { 5, 60, 128 }, { 6, 61, 128 }, { 6, 62, 128 }, { 7, 63, 128 },
   { 2, 64, 128 }, { 3, 65, 128 }, { 3, 66, 128 }, { 4, 67, 128 }, { 3, 68, 128 }, { 4, 69, 128 }, { 4, 70, 128 }, { 5, 71, 128 },
   { 3, 72, 128 }, { 4, 73, 128 }, { 4, 74, 128 }, { 5, 75, 128 }, { 4, 76, 128 }, { 5, 77, 128 }, { 5, 78, 128 }, { 6, 79, 128 },
   { 3, 80, 128 }, { 4, 81, 128 }, { 4, 82, 128 }, { 5, 83, 128 }, { 4, 84, 128 }, { 5, 85, 128 }, { 5, 86, 128 }, { 6, 87, 128 },
   { 4, 88, 128 }, { 5, 89, 128 }, { 5, 90, 128 }, { 6, 91, 128 }, { 5, 92, 128 }, { 6, 93, 128 }, { 6, 94, 128 }, { 7, 95, 128 },
   { 3, 96, 128 }, { 4, 97, 128 }, { 4, 98, 128 }, { 5, 99, 128 }, { 4, 100, 128 }, { 5, 101, 128 }, { 5, 102, 128 }, { 6, 103, 128 },
   { 4, 104, 128 }, { 5, 105, 128 }, { 5, 106, 128 }, { 6, 107, 128 }, { 5, 108, 128 }, { 6, 109, 128 }, { 6, 110, 128 }, { 7, 111, 128 },
   { 4, 112, 128 }, { 5, 113, 128 }, { 5, 114, 128 }, { 6, 115, 128 }, { 5, 116, 128 }, { 6, 117, 128 }, { 6, 118, 128 }, { 7, 119, 128 },
   { 5, 120, 128 }, { 6, 121, 128 }, { 6, 122, 128 }, { 7, 123, 128 }, { 6, 124, 128 }, { 7, 125, 128 }, { 7, 126, 128 }, { 8, 127, 128 },
#if FP_LUT > 8
   { 1, 0, 0 }, { 2, 1, 256 }, { 2, 2, 256 }, { 3, 3, 256 }, { 2, 4, 256 }, { 3, 5, 256 }, { 3, 6, 256 }, { 4, 7, 256 },
   { 2, 8, 256 }, { 3, 9, 256 }, { 3, 10, 256 }, { 4, 11, 256 }, { 3, 12, 256 }, { 4, 13, 256 }, { 4, 14, 256 }, { 5, 15, 256 },
   { 2, 16, 256 }, { 3, 17, 256 }, { 3, 18, 256 }, { 4, 19, 256 }, { 3, 20, 256 }, { 4, 21, 256 }, { 4, 22, 256 }, { 5, 23, 256 },
   { 3, 24, 256 }, { 4, 25, 256 }, { 4, 26, 256 }, { 5, 27, 256 }, { 4, 28, 256 }, { 5, 29, 256 }, { 5, 30, 256 }, { 6, 31, 256 },
   { 2, 32, 256 }, { 3, 33, 256 }, { 3, 34, 256 }, { 4, 35, 256 }, { 3, 36, 256 }, { 4, 37, 256 }, { 4, 38, 256 }, { 5, 39, 256 },
   { 3, 40, 256 }, { 4, 41, 256 }, { 4, 42, 256 }, { 5, 43, 256 }, { 4, 44, 256 }, { 5, 45, 256 }, { 5, 46, 256 }, { 6, 47, 256 },
   { 3, 48, 256 }, { 4, 49, 256 }, { 4, 50, 256 }, { 5, 51, 256 }, { 4, 52, 256 }, { 5, 53, 256 }, { 5, 54, 256 }, { 6, 55, 256 },
   { 4, 56, 256 }, { 5, 57, 256 }, { 5, 58, 256 }, { 6, 59, 256 }, { 5, 60, 256 }, { 6, 61, 256 }, { 6, 62, 256 }, { 7, 63, 256 },
   { 2, 64, 256 }, { 3, 65, 256 }, { 3, 66, 256 }, { 4, 67, 256 }, { 3, 68, 256 }, { 4, 69, 256 }, { 4, 70, 256 }, { 5, 71, 256 },
   { 3, 72, 256 }, { 4, 73, 256 }, { 4, 74, 256 }, { 5, 75, 256 }, { 4, 76, 256 }, { 5, 77, 256 }, { 5, 78, 256 }, { 6, 79, 256 },
   { 3, 80, 256 }, { 4, 81, 256 }, { 4, 82, 256 }, { 5, 83, 256 }, { 4, 84, 256 }, { 5, 85, 256 }, { 5, 86, 256 }, { 6, 87, 256 },
   { 4, 88, 256 }, { 5, 89, 256 }, { 5, 90, 256 }, { 6, 91, 256 }, { 5, 92, 256 }, { 6, 93, 256 }, { 6, 94, 256 }, { 7, 95, 256 },
   { 3, 96, 256 }, { 4, 97, 256 }, { 4, 98, 256 }, { 5, 99, 256 }, { 4, 100, 256 }, { 5, 101, 256 }, { 5, 102, 256 }, { 6, 103, 256 },
   { 4, 104, 256 }, { 5, 105, 256 }, { 5, 106, 256 }, { 6, 107, 256 }, { 5, 108, 256 }, { 6, 109, 256 }, { 6, 110, 256 }, { 7, 111, 256 },
   { 4, 112, 256 }, { 5, 113, 256 }, { 5, 114, 256 }, { 6, 115, 256 }, { 5, 116, 256 }, { 6, 117, 256 }, { 6, 118, 256 }, { 7, 119, 256 },
   { 5, 120, 256 }, { 6, 121, 256 }, { 6, 122, 256 }, { 7, 123, 256 }, { 6, 124, 256 }, { 7, 125, 256 }, { 7, 126, 256 }, { 8, 127, 256 },
   { 2, 128, 256 }, { 3, 129, 256 }, { 3, 130, 256 }, { 4, 131, 256 }, { 3, 132, 256 }, { 4, 133, 256 }, { 4, 134, 256 }, { 5, 135, 256 },
   { 3, 136, 256 }, { 4, 137, 256 }, { 4, 138, 256 }, { 5, 139, 256 }, { 4, 140, 256 }, { 5, 141, 256 }, { 5, 142, 256 }, { 6, 143, 256 },
   { 3, 144, 256 }, { 4, 145, 256 }, { 4, 146, 256 }, { 5, 147, 256 }, { 4, 148, 256 }, { 5, 149, 256 }, { 5, 150, 256 }, { 6, 151, 256 },
   { 4, 152, 256 }, { 5, 153, 256 }, { 5, 154, 256 }, { 6, 155, 256 }, { 5, 156, 256 }, { 6, 157, 256 }, { 6, 158, 256 }, { 7, 159, 256 },
   { 3, 160, 256 }, { 4, 161, 256 }, { 4, 162, 256 }, { 5, 163, 256 }, { 4, 164, 256 }, { 5, 165, 256 }, { 5, 166, 256 }, { 6, 167, 256 },
   { 4, 168, 256 }, { 5, 169, 256 }, { 5, 170, 256 }, { 6, 171, 256 }, { 5, 172, 256 }, { 6, 173, 256 }, { 6, 174, 256 }, { 7, 175, 256 },
   { 4, 176, 256 }, { 5, 177, 256 }, { 5, 178, 256 }, { 6, 179, 256 }, { 5, 180, 256 }, { 6, 181, 256 }, { 6, 182, 256 }, { 7, 183, 256 },
   { 5, 184, 256 }, { 6, 185, 256 }, { 6, 186, 256 }, { 7, 187, 256 }, { 6, 188, 256 }, { 7, 189, 256 }, { 7, 190, 256 }, { 8, 191, 256 },
   { 3, 192, 256 }, { 4, 193, 256 }, { 4, 194, 256 }, { 5, 195, 256 }, { 4, 196, 256 }, { 5, 197, 256 }, { 5, 198, 256 }, { 6, 199, 256 },
   { 4, 200, 256 }, { 5, 201, 256 }, { 5, 202, 256 }, { 6, 203, 256 }, { 5, 204, 256 }, { 6, 205, 256 }, { 6, 206, 256 }, { 7, 207, 256 },
   { 4, 208, 256 }, { 5, 209, 256 }, { 5, 210, 256 }, { 6, 211, 256 }, { 5, 212, 256 }, { 6, 213, 256 }, { 6, 214, 256 }, { 7, 215, 256 },
   { 5, 216, 256 }, { 6, 217, 256 }, { 6, 218, 256 }, { 7, 219, 256 }, { 6, 220, 256 }, { 7, 221, 256 }, { 7, 222, 256 }, { 8, 223, 256 },
   { 4, 224, 256 }, { 5, 225, 256 }, { 5, 226, 256 }, { 6, 227, 256 }, { 5, 228, 256 }, { 6, 229, 256 }, { 6, 230, 256 }, { 7, 231, 256 },
   { 5, 232, 256 }, { 6, 233, 256 }, { 6, 234, 256 }, { 7, 235, 256 }, { 6, 236, 256 }, { 7, 237, 256 }, { 7, 238, 256 }, { 8, 239, 256 },
   { 5, 240, 256 }, { 6, 241, 256 }, { 6, 242, 256 }, { 7, 243, 256 }, { 6, 244, 256 }, { 7, 245, 256 }, { 7, 246, 256 }, { 8, 247, 256 },
   { 6, 248, 256 }, { 7, 249, 256 }, { 7, 250, 256 }, { 8, 251, 256 }, { 7, 252, 256 }, { 8, 253, 256 }, { 8, 254, 256 }, { 9, 255, 256 },
#if FP_LUT > 9
   { 1, 0, 0 }, { 2, 1, 512 }, { 2, 2, 512 }, { 3, 3, 512 }, { 2, 4, 512 }, { 3, 5, 512 }, { 3, 6, 512 }, { 4, 7, 512 },
   { 2, 8, 512 }, { 3, 9, 512 }, { 3, 10, 512 }, { 4, 11, 512 }, { 3, 12, 512 }, { 4, 13, 512 }, { 4, 14, 512 }, { 5, 15, 512 },
   { 2, 16, 512 }, { 3, 17, 512 }, { 3, 18, 512 }, { 4, 19, 512 }, { 3, 20, 512 }, { 4, 21, 512 }, { 4, 22, 512 }, { 5, 23, 512 },
   { 3, 24, 512 }, { 4, 25, 512 }, { 4, 26, 512 }, { 5, 27, 512 }, { 4, 28, 512 }, { 5, 29, 512 }, { 5, 30, 512 }, { 6, 31, 512 },
   { 2, 32, 512 }, { 3, 33, 512 }, { 3, 34, 512 }, { 4, 35, 512 }, { 3, 36, 512 }, { 4, 37, 512 }, { 4, 38, 512 }, { 5, 39, 512 },
   { 3, 40, 512 }, { 4, 41, 512 }, { 4, 42, 512 }, { 5, 43, 512 }, { 4, 44, 512 }, { 5, 45, 512 }, { 5, 46, 512 }, { 6, 47, 512 },
   { 3, 48, 512 }, { 4, 49, 512 }, { 4, 50, 512 }, { 5, 51, 512 }, { 4, 52, 512 }, { 5, 53, 512 }, { 5, 54, 512 }, { 6, 55, 512 },
   { 4, 56, 512 }, { 5, 57, 512 }, { 5, 58, 512 }, { 6, 59, 512 }, { 5, 60, 512 }, { 6, 61, 512 }, { 6, 62, 512 }, { 7, 63, 512 },
   { 2, 64, 512 }, { 3, 65, 512 }, { 3, 66, 512 }, { 4, 67, 512 }, { 3, 68, 512 }, { 4, 69, 512 }, { 4, 70, 512 }, { 5, 71, 512 },
   { 3, 72, 512 }, { 4, 73, 512 }, { 4, 74, 512 }, { 5, 75, 512 }, { 4, 76, 512 }, { 5, 77, 512 }, { 5, 78, 512 }, { 6, 79, 512 },
   { 3, 80, 512 }, { 4, 81, 512 }, { 4, 82, 512 }, { 5, 83, 512 }, { 4, 84, 512 }, { 5, 85, 512 }, { 5, 86, 512 }, { 6, 87, 512 },
   { 4, 88, 512 }, { 5, 89, 512 }, { 5, 90, 512 }, { 6, 91, 512 }, { 5, 92, 512 }, { 6, 93, 512 }, { 6, 94, 512 }, { 7, 95, 512 },
   { 3, 96, 512 }, { 4, 97, 512 }, { 4, 98, 512 }, { 5, 99, 512 }, { 4, 100, 512 }, { 5, 101, 512 }, { 5, 102, 512 }, { 6, 103, 512 },
   { 4, 104, 512 }, { 5, 105, 512 }, { 5, 106, 512 }, { 6, 107, 512 }, { 5, 108, 512 }, { 6, 109, 512 }, { 6, 110, 512 }, { 7, 111, 512 },
   { 4, 112, 512 }, { 5, 113, 512 }, { 5, 114, 512 }, { 6, 115, 512 }, { 5, 116, 512 }, { 6, 117, 512 }, { 6, 118, 512 }, { 7, 119, 512 },
   { 5, 120, 512 }, { 6, 121, 512 }, { 6, 122, 512 }, { 7, 123, 512 }, { 6, 124, 512 }, { 7, 125, 512 }, { 7, 126, 512 }, { 8, 127, 512 },
   { 2, 128, 512 }, { 3, 129, 512 }, { 3, 130, 512 }, { 4, 131, 512 }, { 3, 132, 512 }, { 4, 133, 512 }, { 4, 134, 512 }, { 5, 135, 512 },
   { 3, 136, 512 }, { 4, 137, 512 }, { 4, 138, 512 }, { 5, 139, 512 }, { 4, 140, 512 }, { 5, 141, 512 }, { 5, 142, 512 }, { 6, 143, 512 },
   { 3, 144, 512 }, { 4, 145, 512 }, { 4, 146, 512 }, { 5, 147, 512 }, { 4, 148, 512 }, { 5, 149, 512 }, { 5, 150, 512 }, { 6, 151, 512 },
   { 4, 152, 512 }, { 5, 153, 512 }, { 5, 154, 512 }, { 6, 155, 512 }, { 5, 156, 512 }, { 6, 157, 512 }, { 6, 158, 512 }, { 7, 159, 512 },
   { 3, 160, 512 }, { 4, 161, 512 }, { 4, 162, 512 }, { 5, 163, 512 }, { 4, 164, 512 }, { 5, 165, 512 }, { 5, 166, 512 }, { 6, 167, 512 },
   { 4, 168, 512 }, { 5, 169, 512 }, { 5, 170, 512 }, { 6, 171, 512 }, { 5, 172, 512 }, { 6, 173, 512 }, { 6, 174, 512 }, { 7, 175, 512 },
   { 4, 176, 512 }, { 5, 177, 512 }, { 5, 178, 512 }, { 6, 179, 512 }, { 5, 180, 512 }, { 6, 181, 512 }, { 6, 182, 512 }, { 7, 183, 512 },
   { 5, 184, 512 }, { 6, 185, 512 }, { 6, 186, 512 }, { 7, 187, 512 }, { 6, 188, 512 }, { 7, 189, 512 }, { 7, 190, 512 }, { 8, 191, 512 },
   { 3, 192, 512 }, { 4, 193, 512 }, { 4, 194, 512 }, { 5, 195, 512 }, { 4, 196, 512 }, { 5, 197, 512 }, { 5, 198, 512 }, { 6, 199, 512 },
   { 4, 200, 512 }, { 5, 201, 512 }, { 5, 202, 512 }, { 6, 203, 512 }, { 5, 204, 512 }, { 6, 205, 512 }, { 6, 206, 512 }, { 7, 207, 512 },
   { 4, 208, 512 }, { 5, 209, 512 }, { 5, 210, 512 }, { 6, 211, 512 }, { 5, 212, 512 }, { 6, 213, 512 }, { 6, 214, 512 }, { 7, 215, 512 },
   { 5, 216, 512 }, { 6, 217, 512 }, { 6, 218, 512 }, { 7, 219, 512 }, { 6, 220, 512 }, { 7, 221, 512 }, { 7, 222, 512 }, { 8, 223, 512 },
   { 4, 224, 512 }, { 5, 225, 512 }, { 5, 226, 512 }, { 6, 227, 512 }, { 5, 228, 512 }, { 6, 229, 512 }, { 6, 230, 512 }, { 7, 231, 512 },
   { 5, 232, 512 }, { 6, 233, 512 }, { 6, 234, 512 }, { 7, 235, 512 }, { 6, 236, 512 }, { 7, 237, 512 }, { 7, 238, 512 }, { 8, 239, 512 },
   { 5, 240, 512 }, { 6, 241, 512 }, { 6, 242, 512 }, { 7, 243, 512 }, { 6, 244, 512 }, { 7, 245, 512 }, { 7, 246, 512 }, { 8, 247, 512 },
   { 6, 248, 512 }, { 7, 249, 512 }, { 7, 250, 512 }, { 8, 251, 512 }, { 7, 252, 512 }, { 8, 253, 512 }, { 8, 254, 512 }, { 9, 255, 512 },
   { 2, 256, 512 }, { 3, 257, 512 }, { 3, 258, 512 }, { 4, 259, 512 }, { 3, 260, 512 }, { 4, 261, 512 }, { 4, 262, 512 }, { 5, 263, 512 },
   { 3, 264, 512 }, { 4, 265, 512 }, { 4, 266, 512 }, { 5, 267, 512 }, { 4, 268, 512 }, { 5, 269, 512 }, { 5, 270, 512 }, { 6, 271, 512 },
   { 3, 272, 512 }, { 4, 273, 512 }, { 4, 274, 512 }, { 5, 275, 512 }, { 4, 276, 512 }, { 5, 277, 512 }, { 5, 278, 512 }, { 6, 279, 512 },
   { 4, 280, 512 }, { 5, 281, 512 }, { 5, 282, 512 }, { 6, 283, 512 }, { 5, 284, 512 }, { 6, 285, 512 }, { 6, 286, 512 }, { 7, 287, 512 },
   { 3, 288, 512 }, { 4, 289, 512 }, { 4, 290, 512 }, { 5, 291, 512 }, { 4, 292, 512 }, { 5, 293, 512 }, { 5, 294, 512 }, { 6, 295, 512 },
   { 4, 296, 512 }, { 5, 297, 512 }, { 5, 298, 512 }, { 6, 299, 512 }, { 5, 300, 512 }, { 6, 301, 512 }, { 6, 302, 512 }, { 7, 303, 512 },
   { 4, 304, 512 }, { 5, 305, 512 }, { 5, 306, 512 }, { 6, 307, 512 }, { 5, 308, 512 }, { 6, 309, 512 }, { 6, 310, 512 }, { 7, 311, 512 },
   { 5, 312, 512 }, { 6, 313, 512 }, { 6, 314, 512 }, { 7, 315, 512 }, { 6, 316, 512 }, { 7, 317, 512 }, { 7, 318, 512 }, { 8, 319, 512 },
   { 3, 320, 512 }, { 4, 321, 512 }, { 4, 322, 512 }, { 5, 323, 512 }, { 4, 324, 512 }, { 5, 325, 512 }, { 5, 326, 512 }, { 6, 327, 512 },
   { 4, 328, 512 }, { 5, 329, 512 }, { 5, 330, 512 }, { 6, 331, 512 }, { 5, 332, 512 }, { 6, 333, 512 }, { 6, 334, 512 }, { 7, 335, 512 },
   { 4, 336, 512 }, { 5, 337, 512 }, { 5, 338, 512 }, { 6, 339, 512 }, { 5, 340, 512 }, { 6, 341, 512 }, { 6, 342, 512 }, { 7, 343, 512 },
   { 5, 344, 512 }, { 6, 345, 512 }, { 6, 346, 512 }, { 7, 347, 512 }, { 6, 348, 512 }, { 7, 349, 512 }, { 7, 350, 512 }, { 8, 351, 512 },
   { 4, 352, 512 }, { 5, 353, 512 }, { 5, 354, 512 }, { 6, 355, 512 }, { 5, 356, 512 }, { 6, 357, 512 }, { 6, 358, 512 }, { 7, 359, 512 },
   { 5, 360, 512 }, { 6, 361, 512 }, { 6, 362, 512 }, { 7, 363, 512 }, { 6, 364, 512 }, { 7, 365, 512 }, { 7, 366, 512 }, { 8, 367, 512 },
   { 5, 368, 512 }, { 6, 369, 512 }, { 6, 370, 512 }, { 7, 371, 512 }, { 6, 372, 512 }, { 7, 373, 512 }, { 7, 374, 512 }, { 8, 375, 512 },
   { 6, 376, 512 }, { 7, 377, 512 }, { 7, 378, 512 }, { 8, 379, 512 }, { 7, 380, 512 }, { 8, 381, 512 }, { 8, 382, 512 }, { 9, 383, 512 },
   { 3, 384, 512 }, { 4, 385, 512 }, { 4, 386, 512 }, { 5, 387, 512 }, { 4, 388, 512 }, { 5, 389, 512 }, { 5, 390, 512 }, { 6, 391, 512 },
   { 4, 392, 512 }, { 5, 393, 512 }, { 5, 394, 512 }, { 6, 395, 512 }, { 5, 396, 512 }, { 6, 397, 512 }, { 6, 398, 512 }, { 7, 399, 512 },
   { 4, 400, 512 }, { 5, 401, 512 }, { 5, 402, 512 }, { 6, 403, 512 }, { 5, 404, 512 }, { 6, 405, 512 }, { 6, 406, 512 }, { 7, 407, 512 },
   { 5, 408, 512 }, { 6, 409, 512 }, { 6, 410, 512 }, { 7, 411, 512 }, { 6, 412, 512 }, { 7, 413, 512 }, { 7, 414, 512 }, { 8, 415, 512 },
   { 4, 416, 512 }, { 5, 417, 512 }, { 5, 418, 512 }, { 6, 419, 512 }, { 5, 420, 512 }, { 6, 421, 512 }, { 6, 422, 512 }, { 7, 423, 512 },
   { 5, 424, 512 }, { 6, 425, 512 }, { 6, 426, 512 }, { 7, 427, 512 }, { 6, 428, 512 }, { 7, 429, 512 }, { 7, 430, 512 }, { 8, 431, 512 },
   { 5, 432, 512 }, { 6, 433, 512 }, { 6, 434, 512 }, { 7, 435, 512 }, { 6, 436, 512 }, { 7, 437, 512 }, { 7, 438, 512 }, { 8, 439, 512 },
   { 6, 440, 512 }, { 7, 441, 512 }, { 7, 442, 512 }, { 8, 443, 512 }, { 7, 444, 512 }, { 8, 445, 512 }, { 8, 446, 512 }, { 9, 447, 512 },
   { 4, 448, 512 }, { 5, 449, 512 }, { 5, 450, 512 }, { 6, 451, 512 }, { 5, 452, 512 }, { 6, 453, 512 }, { 6, 454, 512 }, { 7, 455, 512 },
   { 5, 456, 512 }, { 6, 457, 512 }, { 6, 458, 512 }, { 7, 459, 512 }, { 6, 460, 512 }, { 7, 461, 512 }, { 7, 462, 512 }, { 8, 463, 512 },
   { 5, 464, 512 }, { 6, 465, 512 }, { 6, 466, 512 }, { 7, 467, 512 }, { 6, 468, 512 }, { 7, 469, 512 }, { 7, 470, 512 }, { 8, 471, 512 },
   { 6, 472, 512 }, { 7, 473, 512 }, { 7, 474, 512 }, { 8, 475, 512 }, { 7, 476, 512 }, { 8, 477, 512 }, { 8, 478, 512 }, { 9, 479, 512 },
   { 5, 480, 512 }, { 6, 481, 512 }, { 6, 482, 512 }, { 7, 483, 512 }, { 6, 484, 512 }, { 7, 485, 512 }, { 7, 486, 512 }, { 8, 487, 512 },
   { 6, 488, 512 }, { 7, 489, 512 }, { 7, 490, 512 }, { 8, 491, 512 }, { 7, 492, 512 }, { 8, 493, 512 }, { 8, 494, 512 }, { 9, 495, 512 },
   { 6, 496, 512 }, { 7, 497, 512 }, { 7, 498, 512 }, { 8, 499, 512 }, { 7, 500, 512 }, { 8, 501, 512 }, { 8, 502, 512 }, { 9, 503, 512 },
   { 7, 504, 512 }, { 8, 505, 512 }, { 8, 506, 512 }, { 9, 507, 512 }, { 8, 508, 512 }, { 9, 509, 512 }, { 9, 510, 512 }, { 10, 511, 512 },
#if FP_LUT > 10
   { 1, 0, 0 }, { 2, 1, 1024 }, { 2, 2, 1024 }, { 3, 3, 1024 }, { 2, 4, 1024 }, { 3, 5, 1024 }, { 3, 6, 1024 }, { 4, 7, 1024 },
   { 2, 8, 1024 }, { 3, 9, 1024 }, { 3, 10, 1024 }, { 4, 11, 1024 }, { 3, 12, 1024 }, { 4, 13, 1024 }, { 4, 14, 1024 }, { 5, 15, 1024 },
   { 2, 16, 1024 }, { 3, 17, 1024 }, { 3, 18, 1024 }, { 4, 19, 1024 }, { 3, 20, 1024 }, { 4, 21, 1024 }, { 4, 22, 1024 }, { 5, 23, 1024 },
   { 3, 24, 1024 }, { 4, 25, 1024 }, { 4, 26, 1024 }, { 5, 27, 1024 }, { 4, 28, 1024 }, { 5, 29, 1024 }, { 5, 30, 1024 }, { 6, 31, 1024 },
   { 2, 32, 1024 }, { 3, 33, 1024 }, { 3, 34, 1024 }, { 4, 35, 1024 }, { 3, 36, 1024 }, { 4, 37, 1024 }, { 4, 38, 1024 }, { 5, 39, 1024 },
   { 3, 40, 1024 }, { 4, 41, 1024 }, { 4, 42, 1024 }, { 5, 43, 1024 }, { 4, 44, 1024 }, { 5, 45, 1024 }, { 5, 46, 1024 }, { 6, 47, 1024 },
   { 3, 48, 1024 }, { 4, 49, 1024 }, { 4, 50, 1024 }, { 5, 51, 1024 }, { 4, 52, 1024 }, { 5, 53, 1024 }, { 5, 54, 1024 }, { 6, 55, 1024 },
   { 4, 56, 1024 }, { 5, 57, 1024 }, { 5, 58, 1024 }, { 6, 59, 1024 }, { 5, 60, 1024 }, { 6, 61, 1024 }, { 6, 62, 1024 }, { 7, 63, 1024 },
   { 2, 64, 1024 }, { 3, 65, 1024 }, { 3, 66, 1024 }, { 4, 67, 1024 }, { 3, 68, 1024 }, { 4, 69, 1024 }, { 4, 70, 1024 }, { 5, 71, 1024 },
   { 3, 72, 1024 }, { 4, 73, 1024 }, { 4, 74, 1024 }, { 5, 75, 1024 }, { 4, 76, 1024 }, { 5, 77, 1024 }, { 5, 78, 1024 }, { 6, 79, 1024 },
   { 3, 80, 1024 }, { 4, 81, 1024 }, { 4, 82, 1024 }, { 5, 83, 1024 }, { 4, 84, 1024 }, { 5, 85, 1024 }, { 5, 86, 1024 }, { 6, 87, 1024 },
   { 4, 88, 1024 }, { 5, 89, 1024 }, { 5, 90, 1024 }, { 6, 91, 1024 }, { 5, 92, 1024 }, { 6, 93, 1024 }, { 6, 94, 1024 }, { 7, 95, 1024 },
   { 3, 96, 1024 }, { 4, 97, 1024 }, { 4, 98, 1024 }, { 5, 99, 1024 }, { 4, 100, 1024 }, { 5, 101, 1024 }, { 5, 102, 1024 }, { 6, 103, 1024 },
   { 4, 104, 1024 }, { 5, 105, 1024 }, { 5, 106, 1024 }, { 6, 107, 1024 }, { 5, 108, 1024 }, { 6, 109, 1024 }, { 6, 110, 1024 }, { 7, 111, 1024 },
   { 4, 112, 1024 }, { 5, 113, 1024 }, { 5, 114, 1024 }, { 6, 115, 1024 }, { 5, 116, 1024 }, { 6, 117, 1024 }, { 6, 118, 1024 }, { 7, 119, 1024 },
   { 5, 120, 1024 }, { 6, 121, 1024 }, { 6, 122, 1024 }, { 7, 123, 1024 }, { 6, 124, 1024 }, { 7, 125, 1024 }, { 7, 126, 1024 }, { 8, 127, 1024 },
   { 2, 128, 1024 }, { 3, 129, 1024 }, { 3, 130, 1024 }, { 4, 131, 1024 }, { 3, 132, 1024 }, { 4, 133, 1024 }, { 4, 134, 1024 }, { 5, 135, 1024 },
   { 3, 136, 1024 }, { 4, 137, 1024 }, { 4, 138, 1024 }, { 5, 139, 1024 }, { 4, 140, 1024 }, { 5, 141, 1024 }, { 5, 142, 1024 }, { 6, 143, 1024 },
   { 3, 144, 1024 }, { 4, 145, 1024 }, { 4, 146, 1024 }, { 5, 147, 1024 }, { 4, 148, 1024 }, { 5, 149, 1024 }, { 5, 150, 1024 }, { 6, 151, 1024 },
   { 4, 152, 1024 }, { 5, 153, 1024 }, { 5, 154, 1024 }, { 6, 155, 1024 }, { 5, 156, 1024 }, { 6, 157, 1024 }, { 6, 158, 1024 }, { 7, 159, 1024 },
   { 3, 160, 1024 }, { 4, 161, 1024 }, { 4, 162, 1024 }, { 5, 163, 1024 }, { 4, 164, 1024 }, { 5, 165, 1024 }, { 5, 166, 1024 }, { 6, 167, 1024 },
   { 4, 168, 1024 }, { 5, 169, 1024 }, { 5, 170, 1024 }, { 6, 171, 1024 }, { 5, 172, 1024 }, { 6, 173, 1024 }, { 6, 174, 1024 }, { 7, 175, 1024 },
   { 4, 176, 1024 }, { 5, 177, 1024 }, { 5, 178, 1024 }, { 6, 179, 1024 }, { 5, 180, 1024 }, { 6, 181, 1024 }, { 6, 182, 1024 }, { 7, 183, 1024 },
   { 5, 184, 1024 }, { 6, 185, 1024 }, { 6, 186, 1024 }, { 7, 187, 1024 }, { 6, 188, 1024 }, { 7, 189, 1024 }, { 7, 190, 1024 }, { 8, 191, 1024 },
   { 3, 192, 1024 }, { 4, 193, 1024 }, { 4, 194, 1024 }, { 5, 195, 1024 }, { 4, 196, 1024 }, { 5, 197, 1024 }, { 5, 198, 1024 }, { 6, 199, 1024 },
   { 4, 200, 1024 }, { 5, 201, 1024 }, { 5, 202, 1024 }, { 6, 203, 1024 }, { 5, 204, 1024 }, { 6, 205, 1024 }, { 6, 206, 1024 }, { 7, 207, 1024 },
   { 4, 208, 1024 }, { 5, 209, 1024 }, { 5, 210, 1024 }, { 6, 211, 1024 }, { 5, 212, 1024 }, { 6, 213, 1024 }, { 6, 214, 1024 }, { 7, 215, 1024 },
   { 5, 216, 1024 }, { 6, 217, 1024 }, { 6, 218, 1024 }, { 7, 219, 1024 }, { 6, 220, 1024 }, { 7, 221, 1024 }, { 7, 222, 1024 }, { 8, 223, 1024 },
   { 4, 224, 1024 }, { 5, 225, 1024 }, { 5, 226, 1024 }, { 6, 227, 1024 }, { 5, 228, 1024 }, { 6, 229, 1024 }, { 6, 230, 1024 }, { 7, 231, 1024 },
   { 5, 232, 1024 }, { 6, 233, 1024 }, { 6, 234, 1024 }, { 7, 235, 1024 }, { 6, 236, 1024 }, { 7, 237, 1024 }, { 7, 238, 1024 }, { 8, 239, 1024 },
   { 5, 240, 1024 }, { 6, 241, 1024 }, { 6, 242, 1024 }, { 7, 243, 1024 }, { 6, 244, 1024 }, { 7, 245, 1024 }, { 7, 246, 1024 }, { 8, 247, 1024 },
   { 6, 248, 1024 }, { 7, 249, 1024 }, { 7, 250, 1024 }, { 8, 251, 1024 }, { 7, 252, 1024 }, { 8, 253, 1024 }, { 8, 254, 1024 }, { 9, 255, 1024 },
   { 2, 256, 1024 }, { 3, 257, 1024 }, { 3, 258, 1024 }, { 4, 259, 1024 }, { 3, 260, 1024 }, { 4, 261, 1024 }, { 4, 262, 1024 }, { 5, 263, 1024 },
   { 3, 264, 1024 }, { 4, 265, 1024 }, { 4, 266, 1024 }, { 5, 267, 1024 }, { 4, 268, 1024 }, { 5, 269, 1024 }, { 5, 270, 1024 }, { 6, 271, 1024 },
   { 3, 272, 1024 }, { 4, 273, 1024 }, { 4, 274, 1024 }, { 5, 275, 1024 }, { 4, 276, 1024 }, { 5, 277, 1024 }, { 5, 278, 1024 }, { 6, 279, 1024 },
   { 4, 280, 1024 }, { 5, 281, 1024 }, { 5, 282, 1024 }, { 6, 283, 1024 }, { 5, 284, 1024 }, { 6, 285, 1024 }, { 6, 286, 1024 }, { 7, 287, 1024 },
   { 3, 288, 1024 }, { 4, 289, 1024 }, { 4, 290, 1024 }, { 5, 291, 1024 }, { 4, 292, 1024 }, { 5, 293, 1024 }, { 5, 294, 1024 }, { 6, 295, 1024 },
   { 4, 296, 1024 }, { 5, 297, 1024 }, { 5, 298, 1024 }, { 6, 299, 1024 }, { 5, 300, 1024 }, { 6, 301, 1024 }, { 6, 302, 1024 }, { 7, 303, 1024 },
   { 4, 304, 1024 }, { 5, 305, 1024 }, { 5, 306, 1024 }, { 6, 307, 1024 }, { 5, 308, 1024 }, { 6, 309, 1024 }, { 6, 310, 1024 }, { 7, 311, 1024 },
   { 5, 312, 1024 }, { 6, 313, 1024 }, { 6, 314, 1024 }, { 7, 315, 1024 }, { 6, 316, 1024 }, { 7, 317, 1024 }, { 7, 318, 1024 }, { 8, 319, 1024 },
   { 3, 320, 1024 }, { 4, 321, 1024 }, { 4, 322, 1024 }, { 5, 323, 1024 }, { 4, 324, 1024 }, { 5, 325, 1024 }, { 5, 326, 1024 }, { 6, 327, 1024 },
   { 4, 328, 1024 }, { 5, 329, 1024 }, { 5, 330, 1024 }, { 6, 331, 1024 }, { 5, 332, 1024 }, { 6, 333, 1024 }, { 6, 334, 1024 }, { 7, 335, 1024 },
   { 4, 336, 1024 }, { 5, 337, 1024 }, { 5, 338, 1024 }, { 6, 339, 1024 }, { 5, 340, 1024 }, { 6, 341, 1024 }, { 6, 342, 1024 }, { 7, 343, 1024 },
   { 5, 344, 1024 }, { 6, 345, 1024 }, { 6, 346, 1024 }, { 7, 347, 1024 }, { 6, 348, 1024 }, { 7, 349, 1024 }, { 7, 350, 1024 }, { 8, 351, 1024 },
   { 4, 352, 1024 }, { 5, 353, 1024 }, { 5, 354, 1024 }, { 6, 355, 1024 }, { 5, 356, 1024 }, { 6, 357, 1024 }, { 6, 358, 1024 }, { 7, 359, 1024 },
   { 5, 360, 1024 }, { 6, 361, 1024 }, { 6, 362, 1024 }, { 7, 363, 1024 }, { 6, 364, 1024 }, { 7, 365, 1024 }, { 7, 366, 1024 }, { 8, 367, 1024 },
   { 5, 368, 1024 }, { 6, 369, 1024 }, { 6, 370, 1024 }, { 7, 371, 1024 }, { 6, 372, 1024 }, { 7, 373, 1024 }, { 7, 374, 1024 }, { 8, 375, 1024 },
   { 6, 376, 1024 }, { 7, 377, 1024 }, { 7, 378, 1024 }, { 8, 379, 1024 }, { 7, 380, 1024 }, { 8, 381, 1024 }, { 8, 382, 1024 }, { 9, 383, 1024 },
   { 3, 384, 1024 }, { 4, 385, 1024 }, { 4, 386, 1024 }, { 5, 387, 1024 }, { 4, 388, 1024 }, { 5, 389, 1024 }, { 5, 390, 1024 }, { 6, 391, 1024 },
   { 4, 392, 1024 }, { 5, 393, 1024 }, { 5, 394, 1024 }, { 6, 395, 1024 }, { 5, 396, 1024 }, { 6, 397, 1024 }, { 6, 398, 1024 }, { 7, 399, 1024 },
   { 4, 400, 1024 }, { 5, 401, 1024 }, { 5, 402, 1024 }, { 6, 403, 1024 }, { 5, 404, 1024 }, { 6, 405, 1024 }, { 6, 406, 1024 }, { 7, 407, 1024 },
   { 5, 408, 1024 }, { 6, 409, 1024 }, { 6, 410, 1024 }, { 7, 411, 1024 }, { 6, 412, 1024 }, { 7, 413, 1024 }, { 7, 414, 1024 }, { 8, 415, 1024 },
   { 4, 416, 1024 }, { 5, 417, 1024 }, { 5, 418, 1024 }, { 6, 419, 1024 }, { 5, 420, 1024 }, { 6, 421, 1024 }, { 6, 422, 1024 }, { 7, 423, 1024 },
   { 5, 424, 1024 }, { 6, 425, 1024 }, { 6, 426, 1024 }, { 7, 427, 1024 }, { 6, 428, 1024 }, { 7, 429, 1024 }, { 7, 430, 1024 }, { 8, 431, 1024 },
   { 5, 432, 1024 }, { 6, 433, 1024 }, { 6, 434, 1024 }, { 7, 435, 1024 }, { 6, 436, 1024 }, { 7, 437, 1024 }, { 7, 438, 1024 }, { 8, 439, 1024 },
   { 6, 440, 1024 }, { 7, 441, 1024 }, { 7, 442, 1024 }, { 8, 443, 1024 }, { 7, 444, 1024 }, { 8, 445, 1024 }, { 8, 446, 1024 }, { 9, 447, 1024 },
   { 4, 448, 1024 }, { 5, 449, 1024 }, { 5, 450, 1024 }, { 6, 451, 1024 }, { 5, 452, 1024 }, { 6, 453, 1024 }, { 6, 454, 1024 }, { 7, 455, 1024 },
   { 5, 456, 1024 }, { 6, 457, 1024 }, { 6, 458, 1024 }, { 7, 459, 1024 }, { 6, 460, 1024 }, { 7, 461, 1024 }, { 7, 462, 1024 }, { 8, 463, 1024 },
   { 5, 464, 1024 }, { 6, 465, 1024 }, { 6, 466, 1024 }, { 7, 467, 1024 }, { 6, 468, 1024 }, { 7, 469, 1024 }, { 7, 470, 1024 }, { 8, 471, 1024 },
   { 6, 472, 1024 }, { 7, 473, 1024 }, { 7, 474, 1024 }, { 8, 475, 1024 }, { 7, 476, 1024 }, { 8, 477, 1024 }, { 8, 478, 1024 }, { 9, 479, 1024 },
   { 5, 480, 1024 }, { 6, 481, 1024 }, { 6, 482, 1024 }, { 7, 483, 1024 }, { 6, 484, 1024 }, { 7, 485, 1024 }, { 7, 486, 1024 }, { 8, 487, 1024 },
   { 6, 488, 1024 }, { 7, 489, 1024 }, { 7, 490, 1024 }, { 8, 491, 1024 }, { 7, 492, 1024 }, { 8, 493, 1024 }, { 8, 494, 1024 }, { 9, 495, 1024 },
   { 6, 496, 1024 }, { 7, 497, 1024 }, { 7, 498, 1024 }, { 8, 499, 1024 }, { 7, 500, 1024 }, { 8, 501, 1024 }, { 8, 502, 1024 }, { 9, 503, 1024 },
   { 7, 504, 1024 }, { 8, 505, 1024 }, { 8, 506, 1024 }, { 9, 507, 1024 }, { 8, 508, 1024 }, { 9, 509, 1024 }, { 9, 510, 1024 }, { 10, 511, 1024 },
   { 2, 512, 1024 }, { 3, 513, 1024 }, { 3, 514, 1024 }, { 4, 515, 1024 }, { 3, 516, 1024 }, { 4, 517, 1024 }, { 4, 518, 1024 }, { 5, 519, 1024 },
   { 3, 520, 1024 }, { 4, 521, 1024 }, { 4, 522, 1024 }, { 5, 523, 1024 }, { 4, 524, 1024 }, { 5, 525, 1024 }, { 5, 526, 1024 }, { 6, 527, 1024 },
   { 3, 528, 1024 }, { 4, 529, 1024 }, { 4, 530, 1024 }, { 5, 531, 1024 }, { 4, 532, 1024 }, { 5, 533, 1024 }, { 5, 534, 1024 }, { 6, 535, 1024 },
   { 4, 536, 1024 }, { 5, 537, 1024 }, { 5, 538, 1024 }, { 6, 539, 1024 }, { 5, 540, 1024 }, { 6, 541, 1024 }, { 6, 542, 1024 }, { 7, 543, 1024 },
   { 3, 544, 1024 }, { 4, 545, 1024 }, { 4, 546, 1024 }, { 5, 547, 1024 }, { 4, 548, 1024 }, { 5, 549, 1024 }, { 5, 550, 1024 }, { 6, 551, 1024 },
   { 4, 552, 1024 }, { 5, 553, 1024 }, { 5, 554, 1024 }, { 6, 555, 1024 }, { 5, 556, 1024 }, { 6, 557, 1024 }, { 6, 558, 1024 }, { 7, 559, 1024 },
   { 4, 560, 1024 }, { 5, 561, 1024 }, { 5, 562, 1024 }, { 6, 563, 1024 }, { 5, 564, 1024 }, { 6, 565, 1024 }, { 6, 566, 1024 }, { 7, 567, 1024 },
   { 5, 568, 1024 }, { 6, 569, 1024 }, { 6, 570, 1024 }, { 7, 571, 1024 }, { 6, 572, 1024 }, { 7, 573, 1024 }, { 7, 574, 1024 }, { 8, 575, 1024 },
   { 3, 576, 1024 }, { 4, 577, 1024 }, { 4, 578, 1024 }, { 5, 579, 1024 }, { 4, 580, 1024 }, { 5, 581, 1024 }, { 5, 582, 1024 }, { 6, 583, 1024 },
   { 4, 584, 1024 }, { 5, 585, 1024 }, { 5, 586, 1024 }, { 6, 587, 1024 }, { 5, 588, 1024 }, { 6, 589, 1024 }, { 6, 590, 1024 }, { 7, 591, 1024 },
   { 4, 592, 1024 }, { 5, 593, 1024 }, { 5, 594, 1024 }, { 6, 595, 1024 }, { 5, 596, 1024 }, { 6, 597, 1024 }, { 6, 598, 1024 }, { 7, 599, 1024 },
   { 5, 600, 1024 }, { 6, 601, 1024 }, { 6, 602, 1024 }, { 7, 603, 1024 }, { 6, 604, 1024 }, { 7, 605, 1024 }, { 7, 606, 1024 }, { 8, 607, 1024 },
   { 4, 608, 1024 }, { 5, 609, 1024 }, { 5, 610, 1024 }, { 6, 611, 1024 }, { 5, 612, 1024 }, { 6, 613, 1024 }, { 6, 614, 1024 }, { 7, 615, 1024 },
   { 5, 616, 1024 }, { 6, 617, 1024 }, { 6, 618, 1024 }, { 7, 619, 1024 }, { 6, 620, 1024 }, { 7, 621, 1024 }, { 7, 622, 1024 }, { 8, 623, 1024 },
   { 5, 624, 1024 }, { 6, 625, 1024 }, { 6, 626, 1024 }, { 7, 627, 1024 }, { 6, 628, 1024 }, { 7, 629, 1024 }, { 7, 630, 1024 }, { 8, 631, 1024 },
   { 6, 632, 1024 }, { 7, 633, 1024 }, { 7, 634, 1024 }, { 8, 635, 1024 }, { 7, 636, 1024 }, { 8, 637, 1024 }, { 8, 638, 1024 }, { 9, 639, 1024 },
   { 3, 640, 1024 }, { 4, 641, 1024 }, { 4, 642, 1024 }, { 5, 643, 1024 }, { 4, 644, 1024 }, { 5, 645, 1024 }, { 5, 646, 1024 }, { 6, 647, 1024 },
   { 4, 648, 1024 }, { 5, 649, 1024 }, { 5, 650, 1024 }, { 6, 651, 1024 }, { 5, 652, 1024 }, { 6, 653, 1024 }, { 6, 654, 1024 }, { 7, 655, 1024 },
   { 4, 656, 1024 }, { 5, 657, 1024 }, { 5, 658, 1024 }, { 6, 659, 1024 }, { 5, 660, 1024 }, { 6, 661, 1024 }, { 6, 662, 1024 }, { 7, 663, 1024 },
   { 5, 664, 1024 }, { 6, 665, 1024 }, { 6, 666, 1024 }, { 7, 667, 1024 }, { 6, 668, 1024 }, { 7, 669, 1024 }, { 7, 670, 1024 }, { 8, 671, 1024 },
   { 4, 672, 1024 }, { 5, 673, 1024 }, { 5, 674, 1024 }, { 6, 675, 1024 }, { 5, 676, 1024 }, { 6, 677, 1024 }, { 6, 678, 1024 }, { 7, 679, 1024 },
   { 5, 680, 1024 }, { 6, 681, 1024 }, { 6, 682, 1024 }, { 7, 683, 1024 }, { 6, 684, 1024 }, { 7, 685, 1024 }, { 7, 686, 1024 }, { 8, 687, 1024 },
   { 5, 688, 1024 }, { 6, 689, 1024 }, { 6, 690, 1024 }, { 7, 691, 1024 }, { 6, 692, 1024 }, { 7, 693, 1024 }, { 7, 694, 1024 }, { 8, 695, 1024 },
   { 6, 696, 1024 }, { 7, 697, 1024 }, { 7, 698, 1024 }, { 8, 699, 1024 }, { 7, 700, 1024 }, { 8, 701, 1024 }, { 8, 702, 1024 }, { 9, 703, 1024 },
   { 4, 704, 1024 }, { 5, 705, 1024 }, { 5, 706, 1024 }, { 6, 707, 1024 }, { 5, 708, 1024 }, { 6, 709, 1024 }, { 6, 710, 1024 }, { 7, 711, 1024 },
   { 5, 712, 1024 }, { 6, 713, 1024 }, { 6, 714, 1024 }, { 7, 715, 1024 }, { 6, 716, 1024 }, { 7, 717, 1024 }, { 7, 718, 1024 }, { 8, 719, 1024 },
   { 5, 720, 1024 }, { 6, 721, 1024 }, { 6, 722, 1024 }, { 7, 723, 1024 }, { 6, 724, 1024 }, { 7, 725, 1024 }, { 7, 726, 1024 }, { 8, 727, 1024 },
   { 6, 728, 1024 }, { 7, 729, 1024 }, { 7, 730, 1024 }, { 8, 731, 1024 }, { 7, 732, 1024 }, { 8, 733, 1024 }, { 8, 734, 1024 }, { 9, 735, 1024 },
   { 5, 736, 1024 }, { 6, 737, 1024 }, { 6, 738, 1024 }, { 7, 739, 1024 }, { 6, 740, 1024 }, { 7, 741, 1024 }, { 7, 742, 1024 }, { 8, 743, 1024 },
   { 6, 744, 1024 }, { 7, 745, 1024 }, { 7, 746, 1024 }, { 8, 747, 1024 }, { 7, 748, 1024 }, { 8, 749, 1024 }, { 8, 750, 1024 }, { 9, 751, 1024 },
   { 6, 752, 1024 }, { 7, 753, 1024 }, { 7, 754, 1024 }, { 8, 755, 1024 }, { 7, 756, 1024 }, { 8, 757, 1024 }, { 8, 758, 1024 }, { 9, 759, 1024 },
   { 7, 760, 1024 }, { 8, 761, 1024 }, { 8, 762, 1024 }, { 9, 763, 1024 }, { 8, 764, 1024 }, { 9, 765, 1024 }, { 9, 766, 1024 }, { 10, 767, 1024 },
   { 3, 768, 1024 }, { 4, 769, 1024 }, { 4, 770, 1024 }, { 5, 771, 1024 }, { 4, 772, 1024 }, { 5, 773, 1024 }, { 5, 774, 1024 }, { 6, 775, 1024 },
   { 4, 776, 1024 }, { 5, 777, 1024 }, { 5, 778, 1024 }, { 6, 779, 1024 }, { 5, 780, 1024 }, { 6, 781, 1024 }, { 6, 782, 1024 }, { 7, 783, 1024 },
   { 4, 784, 1024 }, { 5, 785, 1024 }, { 5, 786, 1024 }, { 6, 787, 1024 }, { 5, 788, 1024 }, { 6, 789, 1024 }, { 6, 790, 1024 }, { 7, 791, 1024 },
   { 5, 792, 1024 }, { 6, 793, 1024 }, { 6, 794, 1024 }, { 7, 795, 1024 }, { 6, 796, 1024 }, { 7, 797, 1024 }, { 7, 798, 1024 }, { 8, 799, 1024 },
   { 4, 800, 1024 }, { 5, 801, 1024 }, { 5, 802, 1024 }, { 6, 803, 1024 }, { 5, 804, 1024 }, { 6, 805, 1024 }, { 6, 806, 1024 }, { 7, 807, 1024 },
   { 5, 808, 1024 }, { 6, 809, 1024 }, { 6, 810, 1024 }, { 7, 811, 1024 }, { 6, 812, 1024 }, { 7, 813, 1024 }, { 7, 814, 1024 }, { 8, 815, 1024 },
   { 5, 816, 1024 }, { 6, 817, 1024 }, { 6, 818, 1024 }, { 7, 819, 1024 }, { 6, 820, 1024 }, { 7, 821, 1024 }, { 7, 822, 1024 }, { 8, 823, 1024 },
   { 6, 824, 1024 }, { 7, 825, 1024 }, { 7, 826, 1024 }, { 8, 827, 1024 }, { 7, 828, 1024 }, { 8, 829, 1024 }, { 8, 830, 1024 }, { 9, 831, 1024 },
   { 4, 832, 1024 }, { 5, 833, 1024 }, { 5, 834, 1024 }, { 6, 835, 1024 }, { 5, 836, 1024 }, { 6, 837, 1024 }, { 6, 838, 1024 }, { 7, 839, 1024 },
   { 5, 840, 1024 }, { 6, 841, 1024 }, { 6, 842, 1024 }, { 7, 843, 1024 }, { 6, 844, 1024 }, { 7, 845, 1024 }, { 7, 846, 1024 }, { 8, 847, 1024 },
   { 5, 848, 1024 }, { 6, 849, 1024 }, { 6, 850, 1024 }, { 7, 851, 1024 }, { 6, 852, 1024 }, { 7, 853, 1024 }, { 7, 854, 1024 }, { 8, 855, 1024 },
   { 6, 856, 1024 }, { 7, 857, 1024 }, { 7, 858, 1024 }, { 8, 859, 1024 }, { 7, 860, 1024 }, { 8, 861, 1024 }, { 8, 862, 1024 }, { 9, 863, 1024 },
   { 5, 864, 1024 }, { 6, 865, 1024 }, { 6, 866, 1024 }, { 7, 867, 1024 }, { 6, 868, 1024 }, { 7, 869, 1024 }, { 7, 870, 1024 }, { 8, 871, 1024 },
   { 6, 872, 1024 }, { 7, 873, 1024 }, { 7, 874, 1024 }, { 8, 875, 1024 }, { 7, 876, 1024 }, { 8, 877, 1024 }, { 8, 878, 1024 }, { 9, 879, 1024 },
   { 6, 880, 1024 }, { 7, 881, 1024 }, { 7, 882, 1024 }, { 8, 883, 1024 }, { 7, 884, 1024 }, { 8, 885, 1024 }, { 8, 886, 1024 }, { 9, 887, 1024 },
   { 7, 888, 1024 }, { 8, 889, 1024 }, { 8, 890, 1024 }, { 9, 891, 1024 }, { 8, 892, 1024 }, { 9, 893, 1024 }, { 9, 894, 1024 }, { 10, 895, 1024 },
   { 4, 896, 1024 }, { 5, 897, 1024 }, { 5, 898, 1024 }, { 6, 899, 1024 }, { 5, 900, 1024 }, { 6, 901, 1024 }, { 6, 902, 1024 }, { 7, 903, 1024 },
   { 5, 904, 1024 }, { 6, 905, 1024 }, { 6, 906, 1024 }, { 7, 907, 1024 }, { 6, 908, 1024 }, { 7, 909, 1024 }, { 7, 910, 1024 }, { 8, 911, 1024 },
   { 5, 912, 1024 }, { 6, 913, 1024 }, { 6, 914, 1024 }, { 7, 915, 1024 }, { 6, 916, 1024 }, { 7, 917, 1024 }, { 7, 918, 1024 }, { 8, 919, 1024 },
   { 6, 920, 1024 }, { 7, 921, 1024 }, { 7, 922, 1024 }, { 8, 923, 1024 }, { 7, 924, 1024 }, { 8, 925, 1024 }, { 8, 926, 1024 }, { 9, 927, 1024 },
   { 5, 928, 1024 }, { 6, 929, 1024 }, { 6, 930, 1024 }, { 7, 931, 1024 }, { 6, 932, 1024 }, { 7, 933, 1024 }, { 7, 934, 1024 }, { 8, 935, 1024 },
   { 6, 936, 1024 }, { 7, 937, 1024 }, { 7, 938, 1024 }, { 8, 939, 1024 }, { 7, 940, 1024 }, { 8, 941, 1024 }, { 8, 942, 1024 }, { 9, 943, 1024 },
   { 6, 944, 1024 }, { 7, 945, 1024 }, { 7, 946, 1024 }, { 8, 947, 1024 }, { 7, 948, 1024 }, { 8, 949, 1024 }, { 8, 950, 1024 }, { 9, 951, 1024 },
   { 7, 952, 1024 }, { 8, 953, 1024 }, { 8, 954, 1024 }, { 9, 955, 1024 }, { 8, 956, 1024 }, { 9, 957, 1024 }, { 9, 958, 1024 }, { 10, 959, 1024 },
   { 5, 960, 1024 }, { 6, 961, 1024 }, { 6, 962, 1024 }, { 7, 963, 1024 }, { 6, 964, 1024 }, { 7, 965, 1024 }, { 7, 966, 1024 }, { 8, 967, 1024 },
   { 6, 968, 1024 }, { 7, 969, 1024 }, { 7, 970, 1024 }, { 8, 971, 1024 }, { 7, 972, 1024 }, { 8, 973, 1024 }, { 8, 974, 1024 }, { 9, 975, 1024 },
   { 6, 976, 1024 }, { 7, 977, 1024 }, { 7, 978, 1024 }, { 8, 979, 1024 }, { 7, 980, 1024 }, { 8, 981, 1024 }, { 8, 982, 1024 }, { 9, 983, 1024 },
   { 7, 984, 1024 }, { 8, 985, 1024 }, { 8, 986, 1024 }, { 9, 987, 1024 }, { 8, 988, 1024 }, { 9, 989, 1024 }, { 9, 990, 1024 }, { 10, 991, 1024 },
   { 6, 992, 1024 }, { 7, 993, 1024 }, { 7, 994, 1024 }, { 8, 995, 1024 }, { 7, 996, 1024 }, { 8, 997, 1024 }, { 8, 998, 1024 }, { 9, 999, 1024 },
   { 7, 1000, 1024 }, { 8, 1001, 1024 }, { 8, 1002, 1024 }, { 9, 1003, 1024 }, { 8, 1004, 1024 }, { 9, 1005, 1024 }, { 9, 1006, 1024 }, { 10, 1007, 1024 },
   { 7, 1008, 1024 }, { 8, 1009, 1024 }, { 8, 1010, 1024 }, { 9, 1011, 1024 }, { 8, 1012, 1024 }, { 9, 1013, 1024 }, { 9, 1014, 1024 }, { 10, 1015, 1024 },
   { 8, 1016, 1024 }, { 9, 1017, 1024 }, { 9, 1018, 1024 }, { 10, 1019, 1024 }, { 9, 1020, 1024 }, { 10, 1021, 1024 }, { 10, 1022, 1024 }, { 11, 1023, 1024 },
#if FP_LUT > 11
   { 1, 0, 0 }, { 2, 1, 2048 }, { 2, 2, 2048 }, { 3, 3, 2048 }, { 2, 4, 2048 }, { 3, 5, 2048 }, { 3, 6, 2048 }, { 4, 7, 2048 },
   { 2, 8, 2048 }, { 3, 9, 2048 }, { 3, 10, 2048 }, { 4, 11, 2048 }, { 3, 12, 2048 }, { 4, 13, 2048 }, { 4, 14, 2048 }, { 5, 15, 2048 },
   { 2, 16, 2048 }, { 3, 17, 2048 }, { 3, 18, 2048 }, { 4, 19, 2048 }, { 3, 20, 2048 }, { 4, 21, 2048 }, { 4, 22, 2048 }, { 5, 23, 2048 },
   { 3, 24, 2048 }, { 4, 25, 2048 }, { 4, 26, 2048 }, { 5, 27, 2048 }, { 4, 28, 2048 }, { 5, 29, 2048 }, { 5, 30, 2048 }, { 6, 31, 2048 },
   { 2, 32, 2048 }, { 3, 33, 2048 }, { 3, 34, 2048 }, { 4, 35, 2048 }, { 3, 36, 2048 }, { 4, 37, 2048 }, { 4, 38, 2048 }, { 5, 39, 2048 },
   { 3, 40, 2048 }, { 4, 41, 2048 }, { 4, 42, 2048 }, { 5, 43, 2048 }, { 4, 44, 2048 }, { 5, 45, 2048 }, { 5, 46, 2048 }, { 6, 47, 2048 },
   { 3, 48, 2048 }, { 4, 49, 2048 }, { 4, 50, 2048 }, { 5, 51, 2048 }, { 4, 52, 2048 }, { 5, 53, 2048 }, { 5, 54, 2048 }, { 6, 55, 2048 },
   { 4, 56, 2048 }, { 5, 57, 2048 }, { 5, 58, 2048 }, { 6, 59, 2048 }, { 5, 60, 2048 }, { 6, 61, 2048 }, { 6, 62, 2048 }, { 7, 63, 2048 },
   { 2, 64, 2048 }, { 3, 65, 2048 }, { 3, 66, 2048 }, { 4, 67, 2048 }, { 3, 68, 2048 }, { 4, 69, 2048 }, { 4, 70, 2048 }, { 5, 71, 2048 },
   { 3, 72, 2048 }, { 4, 73, 2048 }, { 4, 74, 2048 }, { 5, 75, 2048 }, { 4, 76, 2048 }, { 5, 77, 2048 }, { 5, 78, 2048 }, { 6, 79, 2048 },
   { 3, 80, 2048 }, { 4, 81, 2048 }, { 4, 82, 2048 }, { 5, 83, 2048 }, { 4, 84, 2048 }, { 5, 85, 2048 }, { 5, 86, 2048 }, { 6, 87, 2048 },
   { 4, 88, 2048 }, { 5, 89, 2048 }, { 5, 90, 2048 }, { 6, 91, 2048 }, { 5, 92, 2048 }, { 6, 93, 2048 }, { 6, 94, 2048 }, { 7, 95, 2048 },
   { 3, 96, 2048 }, { 4, 97, 2048 }, { 4, 98, 2048 }, { 5, 99, 2048 }, { 4, 100, 2048 }, { 5, 101, 2048 }, { 5, 102, 2048 }, { 6, 103, 2048 },
   { 4, 104, 2048 }, { 5, 105, 2048 }, { 5, 106, 2048 }, { 6, 107, 2048 }, { 5, 108, 2048 }, { 6, 109, 2048 }, { 6, 110, 2048 }, { 7, 111, 2048 },
   { 4, 112, 2048 }, { 5, 113, 2048 }, { 5, 114, 2048 }, { 6, 115, 2048 }, { 5, 116, 2048 }, { 6, 117, 2048 }, { 6, 118, 2048 }, { 7, 119, 2048 },
   { 5, 120, 2048 }, { 6, 121, 2048 }, { 6, 122, 2048 }, { 7, 123, 2048 }, { 6, 124, 2048 }, { 7, 125, 2048 }, { 7, 126, 2048 }, { 8, 127, 2048 },
   { 2, 128, 2048 }, { 3, 129, 2048 }, { 3, 130, 2048 }, { 4, 131, 2048 }, { 3, 132, 2048 }, { 4, 133, 2048 }, { 4, 134, 2048 }, { 5, 135, 2048 },
   { 3, 136, 2048 }, { 4, 137, 2048 }, { 4, 138, 2048 }, { 5, 139, 2048 }, { 4, 140, 2048 }, { 5, 141, 2048 }, { 5, 142, 2048 }, { 6, 143, 2048 },
   { 3, 144, 2048 }, { 4, 145, 2048 }, { 4, 146, 2048 }, { 5, 147, 2048 }, { 4, 148, 2048 }, { 5, 149, 2048 }, { 5, 150, 2048 }, { 6, 151, 2048 },
   { 4, 152, 2048 }, { 5, 153, 2048 }, { 5, 154, 2048 }, { 6, 155, 2048 }, { 5, 156, 2048 }, { 6, 157, 2048 }, { 6, 158, 2048 }, { 7, 159, 2048 },
   { 3, 160, 2048 }, { 4, 161, 2048 }, { 4, 162, 2048 }, { 5, 163, 2048 }, { 4, 164, 2048 }, { 5, 165, 2048 }, { 5, 166, 2048 }, { 6, 167, 2048 },
   { 4, 168, 2048 }, { 5, 169, 2048 }, { 5, 170, 2048 }, { 6, 171, 2048 }, { 5, 172, 2048 }, { 6, 173, 2048 }, { 6, 174, 2048 }, { 7, 175, 2048 },
   { 4, 176, 2048 }, { 5, 177, 2048 }, { 5, 178, 2048 }, { 6, 179, 2048 }, { 5, 180, 2048 }, { 6, 181, 2048 }, { 6, 182, 2048 }, { 7, 183, 2048 },
   { 5, 184, 2048 }, { 6, 185, 2048 }, { 6, 186, 2048 }, { 7, 187, 2048 }, { 6, 188, 2048 }, { 7, 189, 2048 }, { 7, 190, 2048 }, { 8, 191, 2048 },
   { 3, 192, 2048 }, { 4, 193, 2048 }, { 4, 194, 2048 }, { 5, 195, 2048 }, { 4, 196, 2048 }, { 5, 197, 2048 }, { 5, 198, 2048 }, { 6, 199, 2048 },
   { 4, 200, 2048 }, { 5, 201, 2048 }, { 5, 202, 2048 }, { 6, 203, 2048 }, { 5, 204, 2048 }, { 6, 205, 2048 }, { 6, 206, 2048 }, { 7, 207, 2048 },
   { 4, 208, 2048 }, { 5, 209, 2048 }, { 5, 210, 2048 }, { 6, 211, 2048 }, { 5, 212, 2048 }, { 6, 213, 2048 }, { 6, 214, 2048 }, { 7, 215, 2048 },
   { 5, 216, 2048 }, { 6, 217, 2048 }, { 6, 218, 2048 }, { 7, 219, 2048 }, { 6, 220, 2048 }, { 7, 221, 2048 }, { 7, 222, 2048 }, { 8, 223, 2048 },
   { 4, 224, 2048 }, { 5, 225, 2048 }, { 5, 226, 2048 }, { 6, 227, 2048 }, { 5, 228, 2048 }, { 6, 229, 2048 }, { 6, 230, 2048 }, { 7, 231, 2048 },
   { 5, 232, 2048 }, { 6, 233, 2048 }, { 6, 234, 2048 }, { 7, 235, 2048 }, { 6, 236, 2048 }, { 7, 237, 2048 }, { 7, 238, 2048 }, { 8, 239, 2048 },
   { 5, 240, 2048 }, { 6, 241, 2048 }, { 6, 242, 2048 }, { 7, 243, 2048 }, { 6, 244, 2048 }, { 7, 245, 2048 }, { 7, 246, 2048 }, { 8, 247, 2048 },
   { 6, 248, 2048 }, { 7, 249, 2048 }, { 7, 250, 2048 }, { 8, 251, 2048 }, { 7, 252, 2048 }, { 8, 253, 2048 }, { 8, 254, 2048 }, { 9, 255, 2048 },
   { 2, 256, 2048 }, { 3, 257, 2048 }, { 3, 258, 2048 }, { 4, 259, 2048 }, { 3, 260, 2048 }, { 4, 261, 2048 }, { 4, 262, 2048 }, { 5, 263, 2048 },
   { 3, 264, 2048 }, { 4, 265, 2048 }, { 4, 266, 2048 }, { 5, 267, 2048 }, { 4, 268, 2048 }, { 5, 269, 2048 }, { 5, 270, 2048 }, { 6, 271, 2048 },
   { 3, 272, 2048 }, { 4, 273, 2048 }, { 4, 274, 2048 }, { 5, 275, 2048 }, { 4, 276, 2048 }, { 5, 277, 2048 }, { 5, 278, 2048 }, { 6, 279, 2048 },
   { 4, 280, 2048 }, { 5, 281, 2048 }, { 5, 282, 2048 }, { 6, 283, 2048 }, { 5, 284, 2048 }, { 6, 285, 2048 }, { 6, 286, 2048 }, { 7, 287, 2048 },
   { 3, 288, 2048 }, { 4, 289, 2048 }, { 4, 290, 2048 }, { 5, 291, 2048 }, { 4, 292, 2048 }, { 5, 293, 2048 }, { 5, 294, 2048 }, { 6, 295, 2048 },
   { 4, 296, 2048 }, { 5, 297, 2048 }, { 5, 298, 2048 }, { 6, 299, 2048 }, { 5, 300, 2048 }, { 6, 301, 2048 }, { 6, 302, 2048 }, { 7, 303, 2048 },
   { 4, 304, 2048 }, { 5, 305, 2048 }, { 5, 306, 2048 }, { 6, 307, 2048 }, { 5, 308, 2048 }, { 6, 309, 2048 }, { 6, 310, 2048 }, { 7, 311, 2048 },
   { 5, 312, 2048 }, { 6, 313, 2048 }, { 6, 314, 2048 }, { 7, 315, 2048 }, { 6, 316, 2048 }, { 7, 317, 2048 }, { 7, 318, 2048 }, { 8, 319, 2048 },
   { 3, 320, 2048 }, { 4, 321, 2048 }, { 4, 322, 2048 }, { 5, 323, 2048 }, { 4, 324, 2048 }, { 5, 325, 2048 }, { 5, 326, 2048 }, { 6, 327, 2048 },
   { 4, 328, 2048 }, { 5, 329, 2048 }, { 5, 330, 2048 }, { 6, 331, 2048 }, { 5, 332, 2048 }, { 6, 333, 2048 }, { 6, 334, 2048 }, { 7, 335, 2048 },
   { 4, 336, 2048 }, { 5, 337, 2048 }, { 5, 338, 2048 }, { 6, 339, 2048 }, { 5, 340, 2048 }, { 6, 341, 2048 }, { 6, 342, 2048 }, { 7, 343, 2048 },
   { 5, 344, 2048 }, { 6, 345, 2048 }, { 6, 346, 2048 }, { 7, 347, 2048 }, { 6, 348, 2048 }, { 7, 349, 2048 }, { 7, 350, 2048 }, { 8, 351, 2048 },
   { 4, 352, 2048 }, { 5, 353, 2048 }, { 5, 354, 2048 }, { 6, 355, 2048 }, { 5, 356, 2048 }, { 6, 357, 2048 }, { 6, 358, 2048 }, { 7, 359, 2048 },
   { 5, 360, 2048 }, { 6, 361, 2048 }, { 6, 362, 2048 }, { 7, 363, 2048 }, { 6, 364, 2048 }, { 7, 365, 2048 }, { 7, 366, 2048 }, { 8, 367, 2048 },
   { 5, 368, 2048 }, { 6, 369, 2048 }, { 6, 370, 2048 }, { 7, 371, 2048 }, { 6, 372, 2048 }, { 7, 373, 2048 }, { 7, 374, 2048 }, { 8, 375, 2048 },
   { 6, 376, 2048 }, { 7, 377, 2048 }, { 7, 378, 2048 }, { 8, 379, 2048 }, { 7, 380, 2048 }, { 8, 381, 2048 }, { 8, 382, 2048 }, { 9, 383, 2048 },
   { 3, 384, 2048 }, { 4, 385, 2048 }, { 4, 386, 2048 }, { 5, 387, 2048 }, { 4, 388, 2048 }, { 5, 389, 2048 }, { 5, 390, 2048 }, { 6, 391, 2048 },
   { 4, 392, 2048 }, { 5, 393, 2048 }, { 5, 394, 2048 }, { 6, 395, 2048 }, { 5, 396, 2048 }, { 6, 397, 2048 }, { 6, 398, 2048 }, { 7, 399, 2048 },
   { 4, 400, 2048 }, { 5, 401, 2048 }, { 5, 402, 2048 }, { 6, 403, 2048 }, { 5, 404, 2048 }, { 6, 405, 2048 }, { 6, 406, 2048 }, { 7, 407, 2048 },
   { 5, 408, 2048 }, { 6, 409, 2048 }, { 6, 410, 2048 }, { 7, 411, 2048 }, { 6, 412, 2048 }, { 7, 413, 2048 }, { 7, 414, 2048 }, { 8, 415, 2048 },
   { 4, 416, 2048 }, { 5, 417, 2048 }, { 5, 418, 2048 }, { 6, 419, 2048 }, { 5, 420, 2048 }, { 6, 421, 2048 }, { 6, 422, 2048 }, { 7, 423, 2048 },
   { 5, 424, 2048 }, { 6, 425, 2048 }, { 6, 426, 2048 }, { 7, 427, 2048 }, { 6, 428, 2048 }, { 7, 429, 2048 }, { 7, 430, 2048 }, { 8, 431, 2048 },
   { 5, 432, 2048 }, { 6, 433, 2048 }, { 6, 434, 2048 }, { 7, 435, 2048 }, { 6, 436, 2048 }, { 7, 437, 2048 }, { 7, 438, 2048 }, { 8, 439, 2048 },
   { 6, 440, 2048 }, { 7, 441, 2048 }, { 7, 442, 2048 }, { 8, 443, 2048 }, { 7, 444, 2048 }, { 8, 445, 2048 }, { 8, 446, 2048 }, { 9, 447, 2048 },
   { 4, 448, 2048 }, { 5, 449, 2048 }, { 5, 450, 2048 }, { 6, 451, 2048 }, { 5, 452, 2048 }, { 6, 453, 2048 }, { 6, 454, 2048 }, { 7, 455, 2048 },
   { 5, 456, 2048 }, { 6, 457, 2048 }, { 6, 458, 2048 }, { 7, 459, 2048 }, { 6, 460, 2048 }, { 7, 461, 2048 }, { 7, 462, 2048 }, { 8, 463, 2048 },
   { 5, 464, 2048 }, { 6, 465, 2048 }, { 6, 466, 2048 }, { 7, 467, 2048 }, { 6, 468, 2048 }, { 7, 469, 2048 }, { 7, 470, 2048 }, { 8, 471, 2048 },
   { 6, 472, 2048 }, { 7, 473, 2048 }, { 7, 474, 2048 }, { 8, 475, 2048 }, { 7, 476, 2048 }, { 8, 477, 2048 }, { 8, 478, 2048 }, { 9, 479, 2048 },
   { 5, 480, 2048 }, { 6, 481, 2048 }, { 6, 482, 2048 }, { 7, 483, 2048 }, { 6, 484, 2048 }, { 7, 485, 2048 }, { 7, 486, 2048 }, { 8, 487, 2048 },
   { 6, 488, 2048 }, { 7, 489, 2048 }, { 7, 490, 2048 }, { 8, 491, 2048 }, { 7, 492, 2048 }, { 8, 493, 2048 }, { 8, 494, 2048 }, { 9, 495, 2048 },
   { 6, 496, 2048 }, { 7, 497, 2048 }, { 7, 498, 2048 }, { 8, 499, 2048 }, { 7, 500, 2048 }, { 8, 501, 2048 }, { 8, 502, 2048 }, { 9, 503, 2048 },
   { 7, 504, 2048 }, { 8, 505, 2048 }, { 8, 506, 2048 }, { 9, 507, 2048 }, { 8, 508, 2048 }, { 9, 509, 2048 }, { 9, 510, 2048 }, { 10, 511, 2048 },
   { 2, 512, 2048 }, { 3, 513, 2048 }, { 3, 514, 2048 }, { 4, 515, 2048 }, { 3, 516, 2048 }, { 4, 517, 2048 }, { 4, 518, 2048 }, { 5, 519, 2048 },
   { 3, 520, 2048 }, { 4, 521, 2048 }, { 4, 522, 2048 }, { 5, 523, 2048 }, { 4, 524, 2048 }, { 5, 525, 2048 }, { 5, 526, 2048 }, { 6, 527, 2048 },
   { 3, 528, 2048 }, { 4, 529, 2048 }, { 4, 530, 2048 }, { 5, 531, 2048 }, { 4, 532, 2048 }, { 5, 533, 2048 }, { 5, 534, 2048 }, { 6, 535, 2048 },
   { 4, 536, 2048 }, { 5, 537, 2048 }, { 5, 538, 2048 }, { 6, 539, 2048 }, { 5, 540, 2048 }, { 6, 541, 2048 }, { 6, 542, 2048 }, { 7, 543, 2048 },
   { 3, 544, 2048 }, { 4, 545, 2048 }, { 4, 546, 2048 }, { 5, 547, 2048 }, { 4, 548, 2048 }, { 5, 549, 2048 }, { 5, 550, 2048 }, { 6, 551, 2048 },
   { 4, 552, 2048 }, { 5, 553, 2048 }, { 5, 554, 2048 }, { 6, 555, 2048 }, { 5, 556, 2048 }, { 6, 557, 2048 }, { 6, 558, 2048 }, { 7, 559, 2048 },
   { 4, 560, 2048 }, { 5, 561, 2048 }, { 5, 562, 2048 }, { 6, 563, 2048 }, { 5, 564, 2048 }, { 6, 565, 2048 }, { 6, 566, 2048 }, { 7, 567, 2048 },
   { 5, 568, 2048 }, { 6, 569, 2048 }, { 6, 570, 2048 }, { 7, 571, 2048 }, { 6, 572, 2048 }, { 7, 573, 2048 }, { 7, 574, 2048 }, { 8, 575, 2048 },
   { 3, 576, 2048 }, { 4, 577, 2048 }, { 4, 578, 2048 }, { 5, 579, 2048 }, { 4, 580, 2048 }, { 5, 581, 2048 }, { 5, 582, 2048 }, { 6, 583, 2048 },
   { 4, 584, 2048 }, { 5, 585, 2048 }, { 5, 586, 2048 }, { 6, 587, 2048 }, { 5, 588, 2048 }, { 6, 589, 2048 }, { 6, 590, 2048 }, { 7, 591, 2048 },
   { 4, 592, 2048 }, { 5, 593, 2048 }, { 5, 594, 2048 }, { 6, 595, 2048 }, { 5, 596, 2048 }, { 6, 597, 2048 }, { 6, 598, 2048 }, { 7, 599, 2048 },
   { 5, 600, 2048 }, { 6, 601, 2048 }, { 6, 602, 2048 }, { 7, 603, 2048 }, { 6, 604, 2048 }, { 7, 605, 2048 }, { 7, 606, 2048 }, { 8, 607, 2048 },
   { 4, 608, 2048 }, { 5, 609, 2048 }, { 5, 610, 2048 }, { 6, 611, 2048 }, { 5, 612, 2048 }, { 6, 613, 2048 }, { 6, 614, 2048 }, { 7, 615, 2048 },
   { 5, 616, 2048 }, { 6, 617, 2048 }, { 6, 618, 2048 }, { 7, 619, 2048 }, { 6, 620, 2048 }, { 7, 621, 2048 }, { 7, 622, 2048 }, { 8, 623, 2048 },
   { 5, 624, 2048 }, { 6, 625, 2048 }, { 6, 626, 2048 }, { 7, 627, 2048 }, { 6, 628, 2048 }, { 7, 629, 2048 }, { 7, 630, 2048 }, { 8, 631, 2048 },
   { 6, 632, 2048 }, { 7, 633, 2048 }, { 7, 634, 2048 }, { 8, 635, 2048 }, { 7, 636, 2048 }, { 8, 637, 2048 }, { 8, 638, 2048 }, { 9, 639, 2048 },
   { 3, 640, 2048 }, { 4, 641, 2048 }, { 4, 642, 2048 }, { 5, 643, 2048 }, { 4, 644, 2048 }, { 5, 645, 2048 }, { 5, 646, 2048 }, { 6, 647, 2048 },
   { 4, 648, 2048 }, { 5, 649, 2048 }, { 5, 650, 2048 }, { 6, 651, 2048 }, { 5, 652, 2048 }, { 6, 653, 2048 }, { 6, 654, 2048 }, { 7, 655, 2048 },
   { 4, 656, 2048 }, { 5, 657, 2048 }, { 5, 658, 2048 }, { 6, 659, 2048 }, { 5, 660, 2048 }, { 6, 661, 2048 }, { 6, 662, 2048 }, { 7, 663, 2048 },
   { 5, 664, 2048 }, { 6, 665, 2048 }, { 6, 666, 2048 }, { 7, 667, 2048 }, { 6, 668, 2048 }, { 7, 669, 2048 }, { 7, 670, 2048 }, { 8, 671, 2048 },
   { 4, 672, 2048 }, { 5, 673, 2048 }, { 5, 674, 2048 }, { 6, 675, 2048 }, { 5, 676, 2048 }, { 6, 677, 2048 }, { 6, 678, 2048 }, { 7, 679, 2048 },
   { 5, 680, 2048 }, { 6, 681, 2048 }, { 6, 682, 2048 }, { 7, 683, 2048 }, { 6, 684, 2048 }, { 7, 685, 2048 }, { 7, 686, 2048 }, { 8, 687, 2048 },
   { 5, 688, 2048 }, { 6, 689, 2048 }, { 6, 690, 2048 }, { 7, 691, 2048 }, { 6, 692, 2048 }, { 7, 693, 2048 }, { 7, 694, 2048 }, { 8, 695, 2048 },
   { 6, 696, 2048 }, { 7, 697, 2048 }, { 7, 698, 2048 }, { 8, 699, 2048 }, { 7, 700, 2048 }, { 8, 701, 2048 }, { 8, 702, 2048 }, { 9, 703, 2048 },
   { 4, 704, 2048 }, { 5, 705, 2048 }, { 5, 706, 2048 }, { 6, 707, 2048 }, { 5, 708, 2048 }, { 6, 709, 2048 }, { 6, 710, 2048 }, { 7, 711, 2048 },
   { 5, 712, 2048 }, { 6, 713, 2048 }, { 6, 714, 2048 }, { 7, 715, 2048 }, { 6, 716, 2048 }, { 7, 717, 2048 }, { 7, 718, 2048 }, { 8, 719, 2048 },
   { 5, 720, 2048 }, { 6, 721, 2048 }, { 6, 722, 2048 }, { 7, 723, 2048 }, { 6, 724, 2048 }, { 7, 725, 2048 }, { 7, 726, 2048 }, { 8, 727, 2048 },
   { 6, 728, 2048 }, { 7, 729, 2048 }, { 7, 730, 2048 }, { 8, 731, 2048 }, { 7, 732, 2048 }, { 8, 733, 2048 }, { 8, 734, 2048 }, { 9, 735, 2048 },
   { 5, 736, 2048 }, { 6, 737, 2048 }, { 6, 738, 2048 }, { 7, 739, 2048 }, { 6, 740, 2048 }, { 7, 741, 2048 }, { 7, 742, 2048 }, { 8, 743, 2048 },
   { 6, 744, 2048 }, { 7, 745, 2048 }, { 7, 746, 2048 }, { 8, 747, 2048 }, { 7, 748, 2048 }, { 8, 749, 2048 }, { 8, 750, 2048 }, { 9, 751, 2048 },
   { 6, 752, 2048 }, { 7, 753, 2048 }, { 7, 754, 2048 }, { 8, 755, 2048 }, { 7, 756, 2048 }, { 8, 757, 2048 }, { 8, 758, 2048 }, { 9, 759, 2048 },
   { 7, 760, 2048 }, { 8, 761, 2048 }, { 8, 762, 2048 }, { 9, 763, 2048 }, { 8, 764, 2048 }, { 9, 765, 2048 }, { 9, 766, 2048 }, { 10, 767, 2048 },
   { 3, 768, 2048 }, { 4, 769, 2048 }, { 4, 770, 2048 }, { 5, 771, 2048 }, { 4, 772, 2048 }, { 5, 773, 2048 }, { 5, 774, 2048 }, { 6, 775, 2048 },
   { 4, 776, 2048 }, { 5, 777, 2048 }, { 5, 778, 2048 }, { 6, 779, 2048 }, { 5, 780, 2048 }, { 6, 781, 2048 }, { 6, 782, 2048 }, { 7, 783, 2048 },
   { 4, 784, 2048 }, { 5, 785, 2048 }, { 5, 786, 2048 }, { 6, 787, 2048 }, { 5, 788, 2048 }, { 6, 789, 2048 }, { 6, 790, 2048 }, { 7, 791, 2048 },
   { 5, 792, 2048 }, { 6, 793, 2048 }, { 6, 794, 2048 }, { 7, 795, 2048 }, { 6, 796, 2048 }, { 7, 797, 2048 }, { 7, 798, 2048 }, { 8, 799, 2048 },
   { 4, 800, 2048 }, { 5, 801, 2048 }, { 5, 802, 2048 }, { 6, 803, 2048 }, { 5, 804, 2048 }, { 6, 805, 2048 }, { 6, 806, 2048 }, { 7, 807, 2048 },
   { 5, 808, 2048 }, { 6, 809, 2048 }, { 6, 810, 2048 }, { 7, 811, 2048 }, { 6, 812, 2048 }, { 7, 813, 2048 }, { 7, 814, 2048 }, { 8, 815, 2048 },
   { 5, 816, 2048 }, { 6, 817, 2048 }, { 6, 818, 2048 }, { 7, 819, 2048 }, { 6, 820, 2048 }, { 7, 821, 2048 }, { 7, 822, 2048 }, { 8, 823, 2048 },
   { 6, 824, 2048 }, { 7, 825, 2048 }, { 7, 826, 2048 }, { 8, 827, 2048 }, { 7, 828, 2048 }, { 8, 829, 2048 }, { 8, 830, 2048 }, { 9, 831, 2048 },
   { 4, 832, 2048 }, { 5, 833, 2048 }, { 5, 834, 2048 }, { 6, 835, 2048 }, { 5, 836, 2048 }, { 6, 837, 2048 }, { 6, 838, 2048 }, { 7, 839, 2048 },
   { 5, 840, 2048 }, { 6, 841, 2048 }, { 6, 842, 2048 }, { 7, 843, 2048 }, { 6, 844, 2048 }, { 7, 845, 2048 }, { 7, 846, 2048 }, { 8, 847, 2048 },
   { 5, 848, 2048 }, { 6, 849, 2048 }, { 6, 850, 2048 }, { 7, 851, 2048 }, { 6, 852, 2048 }, { 7, 853, 2048 }, { 7, 854, 2048 }, { 8, 855, 2048 },
   { 6, 856, 2048 }, { 7, 857, 2048 }, { 7, 858, 2048 }, { 8, 859, 2048 }, { 7, 860, 2048 }, { 8, 861, 2048 }, { 8, 862, 2048 }, { 9, 863, 2048 },
   { 5, 864, 2048 }, { 6, 865, 2048 }, { 6, 866, 2048 }, { 7, 867, 2048 }, { 6, 868, 2048 }, { 7, 869, 2048 }, { 7, 870, 2048 }, { 8, 871, 2048 },
   { 6, 872, 2048 }, { 7, 873, 2048 }, { 7, 874, 2048 }, { 8, 875, 2048 }, { 7, 876, 2048 }, { 8, 877, 2048 }, { 8, 878, 2048 }, { 9, 879, 2048 },
   { 6, 880, 2048 }, { 7, 881, 2048 }, { 7, 882, 2048 }, { 8, 883, 2048 }, { 7, 884, 2048 }, { 8, 885, 2048 }, { 8, 886, 2048 }, { 9, 887, 2048 },
   { 7, 888, 2048 }, { 8, 889, 2048 }, { 8, 890, 2048 }, { 9, 891, 2048 }, { 8, 892, 2048 }, { 9, 893, 2048 }, { 9, 894, 2048 }, { 10, 895, 2048 },
   { 4, 896, 2048 }, { 5, 897, 2048 }, { 5, 898, 2048 }, { 6, 899, 2048 }, { 5, 900, 2048 }, { 6, 901, 2048 }, { 6, 902, 2048 }, { 7, 903, 2048 },
   { 5, 904, 2048 }, { 6, 905, 2048 }, { 6, 906, 2048 }, { 7, 907, 2048 }, { 6, 908, 2048 }, { 7, 909, 2048 }, { 7, 910, 2048 }, { 8, 911, 2048 },
   { 5, 912, 2048 }, { 6, 913, 2048 }, { 6, 914, 2048 }, { 7, 915, 2048 }, { 6, 916, 2048 }, { 7, 917, 2048 }, { 7, 918, 2048 }, { 8, 919, 2048 },
   { 6, 920, 2048 }, { 7, 921, 2048 }, { 7, 922, 2048 }, { 8, 923, 2048 }, { 7, 924, 2048 }, { 8, 925, 2048 }, { 8, 926, 2048 }, { 9, 927, 2048 },
   { 5, 928, 2048 }, { 6, 929, 2048 }, { 6, 930, 2048 }, { 7, 931, 2048 }, { 6, 932, 2048 }, { 7, 933, 2048 }, { 7, 934, 2048 }, { 8, 935, 2048 },
   { 6, 936, 2048 }, { 7, 937, 2048 }, { 7, 938, 2048 }, { 8, 939, 2048 }, { 7, 940, 2048 }, { 8, 941, 2048 }, { 8, 942, 2048 }, { 9, 943, 2048 },
   { 6, 944, 2048 }, { 7, 945, 2048 }, { 7, 946, 2048 }, { 8, 947, 2048 }, { 7, 948, 2048 }, { 8, 949, 2048 }, { 8, 950, 2048 }, { 9, 951, 2048 },
   { 7, 952, 2048 }, { 8, 953, 2048 }, { 8, 954, 2048 }, { 9, 955, 2048 }, { 8, 956, 2048 }, { 9, 957, 2048 }, { 9, 958, 2048 }, { 10, 959, 2048 },
   { 5, 960, 2048 }, { 6, 961, 2048 }, { 6, 962, 2048 }, { 7, 963, 2048 }, { 6, 964, 2048 }, { 7, 965, 2048 }, { 7, 966, 2048 }, { 8, 967, 2048 },
   { 6, 968, 2048 }, { 7, 969, 2048 }, { 7, 970, 2048 }, { 8, 971, 2048 }, { 7, 972, 2048 }, { 8, 973, 2048 }, { 8, 974, 2048 }, { 9, 975, 2048 },
   { 6, 976, 2048 }, { 7, 977, 2048 }, { 7, 978, 2048 }, { 8, 979, 2048 }, { 7, 980, 2048 }, { 8, 981, 2048 }, { 8, 982, 2048 }, { 9, 983, 2048 },
   { 7, 984, 2048 }, { 8, 985, 2048 }, { 8, 986, 2048 }, { 9, 987, 2048 }, { 8, 988, 2048 }, { 9, 989, 2048 }, { 9, 990, 2048 }, { 10, 991, 2048 },
   { 6, 992, 2048 }, { 7, 993, 2048 }, { 7, 994, 2048 }, { 8, 995, 2048 }, { 7, 996, 2048 }, { 8, 997, 2048 }, { 8, 998, 2048 }, { 9, 999, 2048 },
   { 7, 1000, 2048 }, { 8, 1001, 2048 }, { 8, 1002, 2048 }, { 9, 1003, 2048 }, { 8, 1004, 2048 }, { 9, 1005, 2048 }, { 9, 1006, 2048 }, { 10, 1007, 2048 },
   { 7, 1008, 2048 }, { 8, 1009, 2048 }, { 8, 1010, 2048 }, { 9, 1011, 2048 }, { 8, 1012, 2048 }, { 9, 1013, 2048 }, { 9, 1014, 2048 }, { 10, 1015, 2048 },
   { 8, 1016, 2048 }, { 9, 1017, 2048 }, { 9, 1018, 2048 }, { 10, 1019, 2048 }, { 9, 1020, 2048 }, { 10, 1021, 2048 }, { 10, 1022, 2048 }, { 11, 1023, 2048 },
   { 2, 1024, 2048 }, { 3, 1025, 2048 }, { 3, 1026, 2048 }, { 4, 1027, 2048 }, { 3, 1028, 2048 }, { 4, 1029, 2048 }, { 4, 1030, 2048 }, { 5, 1031, 2048 },
   { 3, 1032, 2048 }, { 4, 1033, 2048 }, { 4, 1034, 2048 }, { 5, 1035, 2048 }, { 4, 1036, 2048 }, { 5, 1037, 2048 }, { 5, 1038, 2048 }, { 6, 1039, 2048 },
   { 3, 1040, 2048 }, { 4, 1041, 2048 }, { 4, 1042, 2048 }, { 5, 1043, 2048 }, { 4, 1044, 2048 }, { 5, 1045, 2048 }, { 5, 1046, 2048 }, { 6, 1047, 2048 },
   { 4, 1048, 2048 }, { 5, 1049, 2048 }, { 5, 1050, 2048 }, { 6, 1051, 2048 }, { 5, 1052, 2048 }, { 6, 1053, 2048 }, { 6, 1054, 2048 }, { 7, 1055, 2048 },
   { 3, 1056, 2048 }, { 4, 1057, 2048 }, { 4, 1058, 2048 }, { 5, 1059, 2048 }, { 4, 1060, 2048 }, { 5, 1061, 2048 }, { 5, 1062, 2048 }, { 6, 1063, 2048 },
   { 4, 1064, 2048 }, { 5, 1065, 2048 }, { 5, 1066, 2048 }, { 6, 1067, 2048 }, { 5, 1068, 2048 }, { 6, 1069, 2048 }, { 6, 1070, 2048 }, { 7, 1071, 2048 },
   { 4, 1072, 2048 }, { 5, 1073, 2048 }, { 5, 1074, 2048 }, { 6, 1075, 2048 }, { 5, 1076, 2048 }, { 6, 1077, 2048 }, { 6, 1078, 2048 }, { 7, 1079, 2048 },
   { 5, 1080, 2048 }, { 6, 1081, 2048 }, { 6, 1082, 2048 }, { 7, 1083, 2048 }, { 6, 1084, 2048 }, { 7, 1085, 2048 }, { 7, 1086, 2048 }, { 8, 1087, 2048 },
   { 3, 1088, 2048 }, { 4, 1089, 2048 }, { 4, 1090, 2048 }, { 5, 1091, 2048 }, { 4, 1092, 2048 }, { 5, 1093, 2048 }, { 5, 1094, 2048 }, { 6, 1095, 2048 },
   { 4, 1096, 2048 }, { 5, 1097, 2048 }, { 5, 1098, 2048 }, { 6, 1099, 2048 }, { 5, 1100, 2048 }, { 6, 1101, 2048 }, { 6, 1102, 2048 }, { 7, 1103, 2048 },
   { 4, 1104, 2048 }, { 5, 1105, 2048 }, { 5, 1106, 2048 }, { 6, 1107, 2048 }, { 5, 1108, 2048 }, { 6, 1109, 2048 }, { 6, 1110, 2048 }, { 7, 1111, 2048 },
   { 5, 1112, 2048 }, { 6, 1113, 2048 }, { 6, 1114, 2048 }, { 7, 1115, 2048 }, { 6, 1116, 2048 }, { 7, 1117, 2048 }, { 7, 1118, 2048 }, { 8, 1119, 2048 },
   { 4, 1120, 2048 }, { 5, 1121, 2048 }, { 5, 1122, 2048 }, { 6, 1123, 2048 }, { 5, 1124, 2048 }, { 6, 1125, 2048 }, { 6, 1126, 2048 }, { 7, 1127, 2048 },
   { 5, 1128, 2048 }, { 6, 1129, 2048 }, { 6, 1130, 2048 }, { 7, 1131, 2048 }, { 6, 1132, 2048 }, { 7, 1133, 2048 }, { 7, 1134, 2048 }, { 8, 1135, 2048 },
   { 5, 1136, 2048 }, { 6, 1137, 2048 }, { 6, 1138, 2048 }, { 7, 1139, 2048 }, { 6, 1140, 2048 }, { 7, 1141, 2048 }, { 7, 1142, 2048 }, { 8, 1143, 2048 },
   { 6, 1144, 2048 }, { 7, 1145, 2048 }, { 7, 1146, 2048 }, { 8, 1147, 2048 }, { 7, 1148, 2048 }, { 8, 1149, 2048 }, { 8, 1150, 2048 }, { 9, 1151, 2048 },
   { 3, 1152, 2048 }, { 4, 1153, 2048 }, { 4, 1154, 2048 }, { 5, 1155, 2048 }, { 4, 1156, 2048 }, { 5, 1157, 2048 }, { 5, 1158, 2048 }, { 6, 1159, 2048 },
   { 4, 1160, 2048 }, { 5, 1161, 2048 }, { 5, 1162, 2048 }, { 6, 1163, 2048 }, { 5, 1164, 2048 }, { 6, 1165, 2048 }, { 6, 1166, 2048 }, { 7, 1167, 2048 },
   { 4, 1168, 2048 }, { 5, 1169, 2048 }, { 5, 1170, 2048 }, { 6, 1171, 2048 }, { 5, 1172, 2048 }, { 6, 1173, 2048 }, { 6, 1174, 2048 }, { 7, 1175, 2048 },
   { 5, 1176, 2048 }, { 6, 1177, 2048 }, { 6, 1178, 2048 }, { 7, 1179, 2048 }, { 6, 1180, 2048 }, { 7, 1181, 2048 }, { 7, 1182, 2048 }, { 8, 1183, 2048 },
   { 4, 1184, 2048 }, { 5, 1185, 2048 }, { 5, 1186, 2048 }, { 6, 1187, 2048 }, { 5, 1188, 2048 }, { 6, 1189, 2048 }, { 6, 1190, 2048 }, { 7, 1191, 2048 },
   { 5, 1192, 2048 }, { 6, 1193, 2048 }, { 6, 1194, 2048 }, { 7, 1195, 2048 }, { 6, 1196, 2048 }, { 7, 1197, 2048 }, { 7, 1198, 2048 }, { 8, 1199, 2048 },
   { 5, 1200, 2048 }, { 6, 1201, 2048 }, { 6, 1202, 2048 }, { 7, 1203, 2048 }, { 6, 1204, 2048 }, { 7, 1205, 2048 }, { 7, 1206, 2048 }, { 8, 1207, 2048 },
   { 6, 1208, 2048 }, { 7, 1209, 2048 }, { 7, 1210, 2048 }, { 8, 1211, 2048 }, { 7, 1212, 2048 }, { 8, 1213, 2048 }, { 8, 1214, 2048 }, { 9, 1215, 2048 },
   { 4, 1216, 2048 }, { 5, 1217, 2048 }, { 5, 1218, 2048 }, { 6, 1219, 2048 }, { 5, 1220, 2048 }, { 6, 1221, 2048 }, { 6, 1222, 2048 }, { 7, 1223, 2048 },
   { 5, 1224, 2048 }, { 6, 1225, 2048 }, { 6, 1226, 2048 }, { 7, 1227, 2048 }, { 6, 1228, 2048 }, { 7, 1229, 2048 }, { 7, 1230, 2048 }, { 8, 1231, 2048 },
   { 5, 1232, 2048 }, { 6, 1233, 2048 }, { 6, 1234, 2048 }, { 7, 1235, 2048 }, { 6, 1236, 2048 }, { 7, 1237, 2048 }, { 7, 1238, 2048 }, { 8, 1239, 2048 },
   { 6, 1240, 2048 }, { 7, 1241, 2048 }, { 7, 1242, 2048 }, { 8, 1243, 2048 }, { 7, 1244, 2048 }, { 8, 1245, 2048 }, { 8, 1246, 2048 }, { 9, 1247, 2048 },
   { 5, 1248, 2048 }, { 6, 1249, 2048 }, { 6, 1250, 2048 }, { 7, 1251, 2048 }, { 6, 1252, 2048 }, { 7, 1253, 2048 }, { 7, 1254, 2048 }, { 8, 1255, 2048 },
   { 6, 1256, 2048 }, { 7, 1257, 2048 }, { 7, 1258, 2048 }, { 8, 1259, 2048 }, { 7, 1260, 2048 }, { 8, 1261, 2048 }, { 8, 1262, 2048 }, { 9, 1263, 2048 },
   { 6, 1264, 2048 }, { 7, 1265, 2048 }, { 7, 1266, 2048 }, { 8, 1267, 2048 }, { 7, 1268, 2048 }, { 8, 1269, 2048 }, { 8, 1270, 2048 }, { 9, 1271, 2048 },
   { 7, 1272, 2048 }, { 8, 1273, 2048 }, { 8, 1274, 2048 }, { 9, 1275, 2048 }, { 8, 1276, 2048 }, { 9, 1277, 2048 }, { 9, 1278, 2048 }, { 10, 1279, 2048 },
   { 3, 1280, 2048 }, { 4, 1281, 2048 }, { 4, 1282, 2048 }, { 5, 1283, 2048 }, { 4, 1284, 2048 }, { 5, 1285, 2048 }, { 5, 1286, 2048 }, { 6, 1287, 2048 },
   { 4, 1288, 2048 }, { 5, 1289, 2048 }, { 5, 1290, 2048 }, { 6, 1291, 2048 }, { 5, 1292, 2048 }, { 6, 1293, 2048 }, { 6, 1294, 2048 }, { 7, 1295, 2048 },
   { 4, 1296, 2048 }, { 5, 1297, 2048 }, { 5, 1298, 2048 }, { 6, 1299, 2048 }, { 5, 1300, 2048 }, { 6, 1301, 2048 }, { 6, 1302, 2048 }, { 7, 1303, 2048 },
   { 5, 1304, 2048 }, { 6, 1305, 2048 }, { 6, 1306, 2048 }, { 7, 1307, 2048 }, { 6, 1308, 2048 }, { 7, 1309, 2048 }, { 7, 1310, 2048 }, { 8, 1311, 2048 },
   { 4, 1312, 2048 }, { 5, 1313, 2048 }, { 5, 1314, 2048 }, { 6, 1315, 2048 }, { 5, 1316, 2048 }, { 6, 1317, 2048 }, { 6, 1318, 2048 }, { 7, 1319, 2048 },
   { 5, 1320, 2048 }, { 6, 1321, 2048 }, { 6, 1322, 2048 }, { 7, 1323, 2048 }, { 6, 1324, 2048 }, { 7, 1325, 2048 }, { 7, 1326, 2048 }, { 8, 1327, 2048 },
   { 5, 1328, 2048 }, { 6, 1329, 2048 }, { 6, 1330, 2048 }, { 7, 1331, 2048 }, { 6, 1332, 2048 }, { 7, 1333, 2048 }, { 7, 1334, 2048 }, { 8, 1335, 2048 },
   { 6, 1336, 2048 }, { 7, 1337, 2048 }, { 7, 1338, 2048 }, { 8, 1339, 2048 }, { 7, 1340, 2048 }, { 8, 1341, 2048 }, { 8, 1342, 2048 }, { 9, 1343, 2048 },
   { 4, 1344, 2048 }, { 5, 1345, 2048 }, { 5, 1346, 2048 }, { 6, 1347, 2048 }, { 5, 1348, 2048 }, { 6, 1349, 2048 }, { 6, 1350, 2048 }, { 7, 1351, 2048 },
   { 5, 1352, 2048 }, { 6, 1353, 2048 }, { 6, 1354, 2048 }, { 7, 1355, 2048 }, { 6, 1356, 2048 }, { 7, 1357, 2048 }, { 7, 1358, 2048 }, { 8, 1359, 2048 },
   { 5, 1360, 2048 }, { 6, 1361, 2048 }, { 6, 1362, 2048 }, { 7, 1363, 2048 }, { 6, 1364, 2048 }, { 7, 1365, 2048 }, { 7, 1366, 2048 }, { 8, 1367, 2048 },
   { 6, 1368, 2048 }, { 7, 1369, 2048 }, { 7, 1370, 2048 }, { 8, 1371, 2048 }, { 7, 1372, 2048 }, { 8, 1373, 2048 }, { 8, 1374, 2048 }, { 9, 1375, 2048 },
   { 5, 1376, 2048 }, { 6, 1377, 2048 }, { 6, 1378, 2048 }, { 7, 1379, 2048 }, { 6, 1380, 2048 }, { 7, 1381, 2048 }, { 7, 1382, 2048 }, { 8, 1383, 2048 },
   { 6, 1384, 2048 }, { 7, 1385, 2048 }, { 7, 1386, 2048 }, { 8, 1387, 2048 }, { 7, 1388, 2048 }, { 8, 1389, 2048 }, { 8, 1390, 2048 }, { 9, 1391, 2048 },
   { 6, 1392, 2048 }, { 7, 1393, 2048 }, { 7, 1394, 2048 }, { 8, 1395, 2048 }, { 7, 1396, 2048 }, { 8, 1397, 2048 }, { 8, 1398, 2048 }, { 9, 1399, 2048 },
   { 7, 1400, 2048 }, { 8, 1401, 2048 }, { 8, 1402, 2048 }, { 9, 1403, 2048 }, { 8, 1404, 2048 }, { 9, 1405, 2048 }, { 9, 1406, 2048 }, { 10, 1407, 2048 },
   { 4, 1408, 2048 }, { 5, 1409, 2048 }, { 5, 1410, 2048 }, { 6, 1411, 2048 }, { 5, 1412, 2048 }, { 6, 1413, 2048 }, { 6, 1414, 2048 }, { 7, 1415, 2048 },
   { 5, 1416, 2048 }, { 6, 1417, 2048 }, { 6, 1418, 2048 }, { 7, 1419, 2048 }, { 6, 1420, 2048 }, { 7, 1421, 2048 }, { 7, 1422, 2048 }, { 8, 1423, 2048 },
   { 5, 1424, 2048 }, { 6, 1425, 2048 }, { 6, 1426, 2048 }, { 7, 1427, 2048 }, { 6, 1428, 2048 }, { 7, 1429, 2048 }, { 7, 1430, 2048 }, { 8, 1431, 2048 },
   { 6, 1432, 2048 }, { 7, 1433, 2048 }, { 7, 1434, 2048 }, { 8, 1435, 2048 }, { 7, 1436, 2048 }, { 8, 1437, 2048 }, { 8, 1438, 2048 }, { 9, 1439, 2048 },
   { 5, 1440, 2048 }, { 6, 1441, 2048 }, { 6, 1442, 2048 }, { 7, 1443, 2048 }, { 6, 1444, 2048 }, { 7, 1445, 2048 }, { 7, 1446, 2048 }, { 8, 1447, 2048 },
   { 6, 1448, 2048 }, { 7, 1449, 2048 }, { 7, 1450, 2048 }, { 8, 1451, 2048 }, { 7, 1452, 2048 }, { 8, 1453, 2048 }, { 8, 1454, 2048 }, { 9, 1455, 2048 },
   { 6, 1456, 2048 }, { 7, 1457, 2048 }, { 7, 1458, 2048 }, { 8, 1459, 2048 }, { 7, 1460, 2048 }, { 8, 1461, 2048 }, { 8, 1462, 2048 }, { 9, 1463, 2048 },
   { 7, 1464, 2048 }, { 8, 1465, 2048 }, { 8, 1466, 2048 }, { 9, 1467, 2048 }, { 8, 1468, 2048 }, { 9, 1469, 2048 }, { 9, 1470, 2048 }, { 10, 1471, 2048 },
   { 5, 1472, 2048 }, { 6, 1473, 2048 }, { 6, 1474, 2048 }, { 7, 1475, 2048 }, { 6, 1476, 2048 }, { 7, 1477, 2048 }, { 7, 1478, 2048 }, { 8, 1479, 2048 },
   { 6, 1480, 2048 }, { 7, 1481, 2048 }, { 7, 1482, 2048 }, { 8, 1483, 2048 }, { 7, 1484, 2048 }, { 8, 1485, 2048 }, { 8, 1486, 2048 }, { 9, 1487, 2048 },
   { 6, 1488, 2048 }, { 7, 1489, 2048 }, { 7, 1490, 2048 }, { 8, 1491, 2048 }, { 7, 1492, 2048 }, { 8, 1493, 2048 }, { 8, 1494, 2048 }, { 9, 1495, 2048 },
   { 7, 1496, 2048 }, { 8, 1497, 2048 }, { 8, 1498, 2048 }, { 9, 1499, 2048 }, { 8, 1500, 2048 }, { 9, 1501, 2048 }, { 9, 1502, 2048 }, { 10, 1503, 2048 },
   { 6, 1504, 2048 }, { 7, 1505, 2048 }, { 7, 1506, 2048 }, { 8, 1507, 2048 }, { 7, 1508, 2048 }, { 8, 1509, 2048 }, { 8, 1510, 2048 }, { 9, 1511, 2048 },
   { 7, 1512, 2048 }, { 8, 1513, 2048 }, { 8, 1514, 2048 }, { 9, 1515, 2048 }, { 8, 1516, 2048 }, { 9, 1517, 2048 }, { 9, 1518, 2048 }, { 10, 1519, 2048 },
   { 7, 1520, 2048 }, { 8, 1521, 2048 }, { 8, 1522, 2048 }, { 9, 1523, 2048 }, { 8, 1524, 2048 }, { 9, 1525, 2048 }, { 9, 1526, 2048 }, { 10, 1527, 2048 },
   { 8, 1528, 2048 }, { 9, 1529, 2048 }, { 9, 1530, 2048 }, { 10, 1531, 2048 }, { 9, 1532, 2048 }, { 10, 1533, 2048 }, { 10, 1534, 2048 }, { 11, 1535, 2048 },
   { 3, 1536, 2048 }, { 4, 1537, 2048 }, { 4, 1538, 2048 }, { 5, 1539, 2048 }, { 4, 1540, 2048 }, { 5, 1541, 2048 }, { 5, 1542, 2048 }, { 6, 1543, 2048 },
   { 4, 1544, 2048 }, { 5, 1545, 2048 }, { 5, 1546, 2048 }, { 6, 1547, 2048 }, { 5, 1548, 2048 }, { 6, 1549, 2048 }, { 6, 1550, 2048 }, { 7, 1551, 2048 },
   { 4, 1552, 2048 }, { 5, 1553, 2048 }, { 5, 1554, 2048 }, { 6, 1555, 2048 }, { 5, 1556, 2048 }, { 6, 1557, 2048 }, { 6, 1558, 2048 }, { 7, 1559, 2048 },
   { 5, 1560, 2048 }, { 6, 1561, 2048 }, { 6, 1562, 2048 }, { 7, 1563, 2048 }, { 6, 1564, 2048 }, { 7, 1565, 2048 }, { 7, 1566, 2048 }, { 8, 1567, 2048 },
   { 4, 1568, 2048 }, { 5, 1569, 2048 }, { 5, 1570, 2048 }, { 6, 1571, 2048 }, { 5, 1572, 2048 }, { 6, 1573, 2048 }, { 6, 1574, 2048 }, { 7, 1575, 2048 },
   { 5, 1576, 2048 }, { 6, 1577, 2048 }, { 6, 1578, 2048 }, { 7, 1579, 2048 }, { 6, 1580, 2048 }, { 7, 1581, 2048 }, { 7, 1582, 2048 }, { 8, 1583, 2048 },
   { 5, 1584, 2048 }, { 6, 1585, 2048 }, { 6, 1586, 2048 }, { 7, 1587, 2048 }, { 6, 1588, 2048 }, { 7, 1589, 2048 }, { 7, 1590, 2048 }, { 8, 1591, 2048 },
   { 6, 1592, 2048 }, { 7, 1593, 2048 }, { 7, 1594, 2048 }, { 8, 1595, 2048 }, { 7, 1596, 2048 }, { 8, 1597, 2048 }, { 8, 1598, 2048 }, { 9, 1599, 2048 },
   { 4, 1600, 2048 }, { 5, 1601, 2048 }, { 5, 1602, 2048 }, { 6, 1603, 2048 }, { 5, 1604, 2048 }, { 6, 1605, 2048 }, { 6, 1606, 2048 }, { 7, 1607, 2048 },
   { 5, 1608, 2048 }, { 6, 1609, 2048 }, { 6, 1610, 2048 }, { 7, 1611, 2048 }, { 6, 1612, 2048 }, { 7, 1613, 2048 }, { 7, 1614, 2048 }, { 8, 1615, 2048 },
   { 5, 1616, 2048 }, { 6, 1617, 2048 }, { 6, 1618, 2048 }, { 7, 1619, 2048 }, { 6, 1620, 2048 }, { 7, 1621, 2048 }, { 7, 1622, 2048 }, { 8, 1623, 2048 },
   { 6, 1624, 2048 }, { 7, 1625, 2048 }, { 7, 1626, 2048 }, { 8, 1627, 2048 }, { 7, 1628, 2048 }, { 8, 1629, 2048 }, { 8, 1630, 2048 }, { 9, 1631, 2048 },
   { 5, 1632, 2048 }, { 6, 1633, 2048 }, { 6, 1634, 2048 }, { 7, 1635, 2048 }, { 6, 1636, 2048 }, { 7, 1637, 2048 }, { 7, 1638, 2048 }, { 8, 1639, 2048 },
   { 6, 1640, 2048 }, { 7, 1641, 2048 }, { 7, 1642, 2048 }, { 8, 1643, 2048 }, { 7, 1644, 2048 }, { 8, 1645, 2048 }, { 8, 1646, 2048 }, { 9, 1647, 2048 },
   { 6, 1648, 2048 }, { 7, 1649, 2048 }, { 7, 1650, 2048 }, { 8, 1651, 2048 }, { 7, 1652, 2048 }, { 8, 1653, 2048 }, { 8, 1654, 2048 }, { 9, 1655, 2048 },
   { 7, 1656, 2048 }, { 8, 1657, 2048 }, { 8, 1658, 2048 }, { 9, 1659, 2048 }, { 8, 1660, 2048 }, { 9, 1661, 2048 }, { 9, 1662, 2048 }, { 10, 1663, 2048 },
   { 4, 1664, 2048 }, { 5, 1665, 2048 }, { 5, 1666, 2048 }, { 6, 1667, 2048 }, { 5, 1668, 2048 }, { 6, 1669, 2048 }, { 6, 1670, 2048 }, { 7, 1671, 2048 },
   { 5, 1672, 2048 }, { 6, 1673, 2048 }, { 6, 1674, 2048 }, { 7, 1675, 2048 }, { 6, 1676, 2048 }, { 7, 1677, 2048 }, { 7, 1678, 2048 }, { 8, 1679, 2048 },
   { 5, 1680, 2048 }, { 6, 1681, 2048 }, { 6, 1682, 2048 }, { 7, 1683, 2048 }, { 6, 1684, 2048 }, { 7, 1685, 2048 }, { 7, 1686, 2048 }, { 8, 1687, 2048 },
   { 6, 1688, 2048 }, { 7, 1689, 2048 }, { 7, 1690, 2048 }, { 8, 1691, 2048 }, { 7, 1692, 2048 }, { 8, 1693, 2048 }, { 8, 1694, 2048 }, { 9, 1695, 2048 },
   { 5, 1696, 2048 }, { 6, 1697, 2048 }, { 6, 1698, 2048 }, { 7, 1699, 2048 }, { 6, 1700, 2048 }, { 7, 1701, 2048 }, { 7, 1702, 2048 }, { 8, 1703, 2048 },
   { 6, 1704, 2048 }, { 7, 1705, 2048 }, { 7, 1706, 2048 }, { 8, 1707, 2048 }, { 7, 1708, 2048 }, { 8, 1709, 2048 }, { 8, 1710, 2048 }, { 9, 1711, 2048 },
   { 6, 1712, 2048 }, { 7, 1713, 2048 }, { 7, 1714, 2048 }, { 8, 1715, 2048 }, { 7, 1716, 2048 }, { 8, 1717, 2048 }, { 8, 1718, 2048 }, { 9, 1719, 2048 },
   { 7, 1720, 2048 }, { 8, 1721, 2048 }, { 8, 1722, 2048 }, { 9, 1723, 2048 }, { 8, 1724, 2048 }, { 9, 1725, 2048 }, { 9, 1726, 2048 }, { 10, 1727, 2048 },
   { 5, 1728, 2048 }, { 6, 1729, 2048 }, { 6, 1730, 2048 }, { 7, 1731, 2048 }, { 6, 1732, 2048 }, { 7, 1733, 2048 }, { 7, 1734, 2048 }, { 8, 1735, 2048 },
   { 6, 1736, 2048 }, { 7, 1737, 2048 }, { 7, 1738, 2048 }, { 8, 1739, 2048 }, { 7, 1740, 2048 }, { 8, 1741, 2048 }, { 8, 1742, 2048 }, { 9, 1743, 2048 },
   { 6, 1744, 2048 }, { 7, 1745, 2048 }, { 7, 1746, 2048 }, { 8, 1747, 2048 }, { 7, 1748, 2048 }, { 8, 1749, 2048 }, { 8, 1750, 2048 }, { 9, 1751, 2048 },
   { 7, 1752, 2048 }, { 8, 1753, 2048 }, { 8, 1754, 2048 }, { 9, 1755, 2048 }, { 8, 1756, 2048 }, { 9, 1757, 2048 }, { 9, 1758, 2048 }, { 10, 1759, 2048 },
   { 6, 1760, 2048 }, { 7, 1761, 2048 }, { 7, 1762, 2048 }, { 8, 1763, 2048 }, { 7, 1764, 2048 }, { 8, 1765, 2048 }, { 8, 1766, 2048 }, { 9, 1767, 2048 },
   { 7, 1768, 2048 }, { 8, 1769, 2048 }, { 8, 1770, 2048 }, { 9, 1771, 2048 }, { 8, 1772, 2048 }, { 9, 1773, 2048 }, { 9, 1774, 2048 }, { 10, 1775, 2048 },
   { 7, 1776, 2048 }, { 8, 1777, 2048 }, { 8, 1778, 2048 }, { 9, 1779, 2048 }, { 8, 1780, 2048 }, { 9, 1781, 2048 }, { 9, 1782, 2048 }, { 10, 1783, 2048 },
   { 8, 1784, 2048 }, { 9, 1785, 2048 }, { 9, 1786, 2048 }, { 10, 1787, 2048 }, { 9, 1788, 2048 }, { 10, 1789, 2048 }, { 10, 1790, 2048 }, { 11, 1791, 2048 },
   { 4, 1792, 2048 }, { 5, 1793, 2048 }, { 5, 1794, 2048 }, { 6, 1795, 2048 }, { 5, 1796, 2048 }, { 6, 1797, 2048 }, { 6, 1798, 2048 }, { 7, 1799, 2048 },
   { 5, 1800, 2048 }, { 6, 1801, 2048 }, { 6, 1802, 2048 }, { 7, 1803, 2048 }, { 6, 1804, 2048 }, { 7, 1805, 2048 }, { 7, 1806, 2048 }, { 8, 1807, 2048 },
   { 5, 1808, 2048 }, { 6, 1809, 2048 }, { 6, 1810, 2048 }, { 7, 1811, 2048 }, { 6, 1812, 2048 }, { 7, 1813, 2048 }, { 7, 1814, 2048 }, { 8, 1815, 2048 },
   { 6, 1816, 2048 }, { 7, 1817, 2048 }, { 7, 1818, 2048 }, { 8, 1819, 2048 }, { 7, 1820, 2048 }, { 8, 1821, 2048 }, { 8, 1822, 2048 }, { 9, 1823, 2048 },
   { 5, 1824, 2048 }, { 6, 1825, 2048 }, { 6, 1826, 2048 }, { 7, 1827, 2048 }, { 6, 1828, 2048 }, { 7, 1829, 2048 }, { 7, 1830, 2048 }, { 8, 1831, 2048 },
   { 6, 1832, 2048 }, { 7, 1833, 2048 }, { 7, 1834, 2048 }, { 8, 1835, 2048 }, { 7, 1836, 2048 }, { 8, 1837, 2048 }, { 8, 1838, 2048 }, { 9, 1839, 2048 },
   { 6, 1840, 2048 }, { 7, 1841, 2048 }, { 7, 1842, 2048 }, { 8, 1843, 2048 }, { 7, 1844, 2048 }, { 8, 1845, 2048 }, { 8, 1846, 2048 }, { 9, 1847, 2048 },
   { 7, 1848, 2048 }, { 8, 1849, 2048 }, { 8, 1850, 2048 }, { 9, 1851, 2048 }, { 8, 1852, 2048 }, { 9, 1853, 2048 }, { 9, 1854, 2048 }, { 10, 1855, 2048 },
   { 5, 1856, 2048 }, { 6, 1857, 2048 }, { 6, 1858, 2048 }, { 7, 1859, 2048 }, { 6, 1860, 2048 }, { 7, 1861, 2048 }, { 7, 1862, 2048 }, { 8, 1863, 2048 },
   { 6, 1864, 2048 }, { 7, 1865, 2048 }, { 7, 1866, 2048 }, { 8, 1867, 2048 }, { 7, 1868, 2048 }, { 8, 1869, 2048 }, { 8, 1870, 2048 }, { 9, 1871, 2048 },
   { 6, 1872, 2048 }, { 7, 1873, 2048 }, { 7, 1874, 2048 }, { 8, 1875, 2048 }, { 7, 1876, 2048 }, { 8, 1877, 2048 }, { 8, 1878, 2048 }, { 9, 1879, 2048 },
   { 7, 1880, 2048 }, { 8, 1881, 2048 }, { 8, 1882, 2048 }, { 9, 1883, 2048 }, { 8, 1884, 2048 }, { 9, 1885, 2048 }, { 9, 1886, 2048 }, { 10, 1887, 2048 },
   { 6, 1888, 2048 }, { 7, 1889, 2048 }, { 7, 1890, 2048 }, { 8, 1891, 2048 }, { 7, 1892, 2048 }, { 8, 1893, 2048 }, { 8, 1894, 2048 }, { 9, 1895, 2048 },
   { 7, 1896, 2048 }, { 8, 1897, 2048 }, { 8, 1898, 2048 }, { 9, 1899, 2048 }, { 8, 1900, 2048 }, { 9, 1901, 2048 }, { 9, 1902, 2048 }, { 10, 1903, 2048 },
   { 7, 1904, 2048 }, { 8, 1905, 2048 }, { 8, 1906, 2048 }, { 9, 1907, 2048 }, { 8, 1908, 2048 }, { 9, 1909, 2048 }, { 9, 1910, 2048 }, { 10, 1911, 2048 },
   { 8, 1912, 2048 }, { 9, 1913, 2048 }, { 9, 1914, 2048 }, { 10, 1915, 2048 }, { 9, 1916, 2048 }, { 10, 1917, 2048 }, { 10, 1918, 2048 }, { 11, 1919, 2048 },
   { 5, 1920, 2048 }, { 6, 1921, 2048 }, { 6, 1922, 2048 }, { 7, 1923, 2048 }, { 6, 1924, 2048 }, { 7, 1925, 2048 }, { 7, 1926, 2048 }, { 8, 1927, 2048 },
   { 6, 1928, 2048 }, { 7, 1929, 2048 }, { 7, 1930, 2048 }, { 8, 1931, 2048 }, { 7, 1932, 2048 }, { 8, 1933, 2048 }, { 8, 1934, 2048 }, { 9, 1935, 2048 },
   { 6, 1936, 2048 }, { 7, 1937, 2048 }, { 7, 1938, 2048 }, { 8, 1939, 2048 }, { 7, 1940, 2048 }, { 8, 1941, 2048 }, { 8, 1942, 2048 }, { 9, 1943, 2048 },
   { 7, 1944, 2048 }, { 8, 1945, 2048 }, { 8, 1946, 2048 }, { 9, 1947, 2048 }, { 8, 1948, 2048 }, { 9, 1949, 2048 }, { 9, 1950, 2048 }, { 10, 1951, 2048 },
   { 6, 1952, 2048 }, { 7, 1953, 2048 }, { 7, 1954, 2048 }, { 8, 1955, 2048 }, { 7, 1956, 2048 }, { 8, 1957, 2048 }, { 8, 1958, 2048 }, { 9, 1959, 2048 },
   { 7, 1960, 2048 }, { 8, 1961, 2048 }, { 8, 1962, 2048 }, { 9, 1963, 2048 }, { 8, 1964, 2048 }, { 9, 1965, 2048 }, { 9, 1966, 2048 }, { 10, 1967, 2048 },
   { 7, 1968, 2048 }, { 8, 1969, 2048 }, { 8, 1970, 2048 }, { 9, 1971, 2048 }, { 8, 1972, 2048 }, { 9, 1973, 2048 }, { 9, 1974, 2048 }, { 10, 1975, 2048 },
   { 8, 1976, 2048 }, { 9, 1977, 2048 }, { 9, 1978, 2048 }, { 10, 1979, 2048 }, { 9, 1980, 2048 }, { 10, 1981, 2048 }, { 10, 1982, 2048 }, { 11, 1983, 2048 },
   { 6, 1984, 2048 }, { 7, 1985, 2048 }, { 7, 1986, 2048 }, { 8, 1987, 2048 }, { 7, 1988, 2048 }, { 8, 1989, 2048 }, { 8, 1990, 2048 }, { 9, 1991, 2048 },
   { 7, 1992, 2048 }, { 8, 1993, 2048 }, { 8, 1994, 2048 }, { 9, 1995, 2048 }, { 8, 1996, 2048 }, { 9, 1997, 2048 }, { 9, 1998, 2048 }, { 10, 1999, 2048 },
   { 7, 2000, 2048 }, { 8, 2001, 2048 }, { 8, 2002, 2048 }, { 9, 2003, 2048 }, { 8, 2004, 2048 }, { 9, 2005, 2048 }, { 9, 2006, 2048 }, { 10, 2007, 2048 },
   { 8, 2008, 2048 }, { 9, 2009, 2048 }, { 9, 2010, 2048 }, { 10, 2011, 2048 }, { 9, 2012, 2048 }, { 10, 2013, 2048 }, { 10, 2014, 2048 }, { 11, 2015, 2048 },
   { 7, 2016, 2048 }, { 8, 2017, 2048 }, { 8, 2018, 2048 }, { 9, 2019, 2048 }, { 8, 2020, 2048 }, { 9, 2021, 2048 }, { 9, 2022, 2048 }, { 10, 2023, 2048 },
   { 8, 2024, 2048 }, { 9, 2025, 2048 }, { 9, 2026, 2048 }, { 10, 2027, 2048 }, { 9, 2028, 2048 }, { 10, 2029, 2048 }, { 10, 2030, 2048 }, { 11, 2031, 2048 },
   { 8, 2032, 2048 }, { 9, 2033, 2048 }, { 9, 2034, 2048 }, { 10, 2035, 2048 }, { 9, 2036, 2048 }, { 10, 2037, 2048 }, { 10, 2038, 2048 }, { 11, 2039, 2048 },
   { 9, 2040, 2048 }, { 10, 2041, 2048 }, { 10, 2042, 2048 }, { 11, 2043, 2048 }, { 10, 2044, 2048 }, { 11, 2045, 2048 }, { 11, 2046, 2048 }, { 12, 2047, 2048 },
#endif
#endif
#endif
#endif
#endif
#endif
};


/* find a hole and free as required, return -1 if no hole found */
static int find_hole(void)
{
   int      x, y, z;
   for (z = -1, y = INT_MAX, x = 0; x < FP_ENTRIES; x++) {
       if (fp_cache[x].lru_count < y && fp_cache[x].lock == 0) {
          z = x;
          y = fp_cache[x].lru_count;
       }
   }

   /* decrease all */
   for (x = 0; x < FP_ENTRIES; x++) {
      if (fp_cache[x].lru_count > 3) {
         --(fp_cache[x].lru_count);
      }
   }

   /* free entry z */
   if (z >= 0 && fp_cache[z].g) {
      mp_clear(&fp_cache[z].mu);
      wc_ecc_del_point(fp_cache[z].g);
      fp_cache[z].g  = NULL;
      for (x = 0; x < (1<<FP_LUT); x++) {
         wc_ecc_del_point(fp_cache[z].LUT[x]);
         fp_cache[z].LUT[x] = NULL;
      }
      fp_cache[z].LUT_set = 0;
      fp_cache[z].lru_count = 0;
   }
   return z;
}

/* determine if a base is already in the cache and if so, where */
static int find_base(ecc_point* g)
{
   int x;
   for (x = 0; x < FP_ENTRIES; x++) {
      if (fp_cache[x].g != NULL &&
          mp_cmp(fp_cache[x].g->x, g->x) == MP_EQ &&
          mp_cmp(fp_cache[x].g->y, g->y) == MP_EQ &&
          mp_cmp(fp_cache[x].g->z, g->z) == MP_EQ) {
         break;
      }
   }
   if (x == FP_ENTRIES) {
      x = -1;
   }
   return x;
}

/* add a new base to the cache */
static int add_entry(int idx, ecc_point *g)
{
   unsigned x, y;

   /* allocate base and LUT */
   fp_cache[idx].g = wc_ecc_new_point();
   if (fp_cache[idx].g == NULL) {
      return GEN_MEM_ERR;
   }

   /* copy x and y */
   if ((mp_copy(g->x, fp_cache[idx].g->x) != MP_OKAY) ||
       (mp_copy(g->y, fp_cache[idx].g->y) != MP_OKAY) ||
       (mp_copy(g->z, fp_cache[idx].g->z) != MP_OKAY)) {
      wc_ecc_del_point(fp_cache[idx].g);
      fp_cache[idx].g = NULL;
      return GEN_MEM_ERR;
   }

   for (x = 0; x < (1U<<FP_LUT); x++) {
      fp_cache[idx].LUT[x] = wc_ecc_new_point();
      if (fp_cache[idx].LUT[x] == NULL) {
         for (y = 0; y < x; y++) {
            wc_ecc_del_point(fp_cache[idx].LUT[y]);
            fp_cache[idx].LUT[y] = NULL;
         }
         wc_ecc_del_point(fp_cache[idx].g);
         fp_cache[idx].g         = NULL;
         fp_cache[idx].lru_count = 0;
         return GEN_MEM_ERR;
      }
   }

   fp_cache[idx].LUT_set   = 0;
   fp_cache[idx].lru_count = 0;

   return MP_OKAY;
}
#endif

#if !defined(WOLFSSL_SP_MATH)
/* build the LUT by spacing the bits of the input by #modulus/FP_LUT bits apart
 *
 * The algorithm builds patterns in increasing bit order by first making all
 * single bit input patterns, then all two bit input patterns and so on
 */
static int build_lut(int idx, mp_int* a, mp_int* modulus, mp_digit mp,
    mp_int* mu)
{
   int err;
   unsigned x, y, bitlen, lut_gap;
#ifdef WOLFSSL_SMALL_STACK
   mp_int *tmp = NULL;
#else
   mp_int tmp[1];
#endif
   int infinity;

#ifdef WOLFSSL_SMALL_STACK
   if ((tmp = (mp_int *)XMALLOC(sizeof(*tmp), NULL, DYNAMIC_TYPE_ECC_BUFFER)) == NULL)
       return MEMORY_E;
#endif

   err = mp_init(tmp);
   if (err != MP_OKAY) {
       err = GEN_MEM_ERR;
       goto errout;
   }

   /* sanity check to make sure lut_order table is of correct size,
      should compile out to a NOP if true */
   if ((sizeof(lut_orders) / sizeof(lut_orders[0])) < (1U<<FP_LUT)) {
       err = BAD_FUNC_ARG;
       goto errout;
   }

   /* get bitlen and round up to next multiple of FP_LUT */
   bitlen  = (unsigned)mp_unsigned_bin_size(modulus) << 3;
   x       = bitlen % FP_LUT;
   if (x) {
       bitlen += FP_LUT - x;
   }
   lut_gap = bitlen / FP_LUT;

   /* init the mu */
   err = mp_init_copy(&fp_cache[idx].mu, mu);
   if (err != MP_OKAY)
       goto errout;

   /* copy base */
   if ((mp_mulmod(fp_cache[idx].g->x, mu, modulus,
                  fp_cache[idx].LUT[1]->x) != MP_OKAY) ||
       (mp_mulmod(fp_cache[idx].g->y, mu, modulus,
                  fp_cache[idx].LUT[1]->y) != MP_OKAY) ||
       (mp_mulmod(fp_cache[idx].g->z, mu, modulus,
                  fp_cache[idx].LUT[1]->z) != MP_OKAY)) {
       err = MP_MULMOD_E;
       goto errout;
   }

   /* make all single bit entries */
   for (x = 1; x < FP_LUT; x++) {
      if ((mp_copy(fp_cache[idx].LUT[1<<(x-1)]->x,
                   fp_cache[idx].LUT[1<<x]->x) != MP_OKAY) ||
          (mp_copy(fp_cache[idx].LUT[1<<(x-1)]->y,
                   fp_cache[idx].LUT[1<<x]->y) != MP_OKAY) ||
          (mp_copy(fp_cache[idx].LUT[1<<(x-1)]->z,
                   fp_cache[idx].LUT[1<<x]->z) != MP_OKAY)){
          err = MP_INIT_E;
          goto errout;
      } else {

         /* now double it bitlen/FP_LUT times */
         for (y = 0; y < lut_gap; y++) {
             if ((err = ecc_projective_dbl_point_safe(fp_cache[idx].LUT[1<<x],
                            fp_cache[idx].LUT[1<<x], a, modulus, mp)) != MP_OKAY) {
                 goto errout;
             }
         }
     }
  }

   /* now make all entries in increase order of hamming weight */
   for (x = 2; x <= FP_LUT; x++) {
       if (err != MP_OKAY)
           goto errout;
       for (y = 0; y < (1UL<<FP_LUT); y++) {
           if (lut_orders[y].ham != (int)x) continue;

           /* perform the add */
           if ((err = ecc_projective_add_point_safe(
                           fp_cache[idx].LUT[lut_orders[y].terma],
                           fp_cache[idx].LUT[lut_orders[y].termb],
                           fp_cache[idx].LUT[y], a, modulus, mp,
                           &infinity)) != MP_OKAY) {
               goto errout;
           }
       }
   }

   /* now map all entries back to affine space to make point addition faster */
   for (x = 1; x < (1UL<<FP_LUT); x++) {
       if (err != MP_OKAY)
           break;

       /* convert z to normal from montgomery */
       err = mp_montgomery_reduce(fp_cache[idx].LUT[x]->z, modulus, mp);

       /* invert it */
       if (err == MP_OKAY)
         err = mp_invmod(fp_cache[idx].LUT[x]->z, modulus,
                         fp_cache[idx].LUT[x]->z);

       if (err == MP_OKAY)
         /* now square it */
         err = mp_sqrmod(fp_cache[idx].LUT[x]->z, modulus, tmp);

       if (err == MP_OKAY)
         /* fix x */
         err = mp_mulmod(fp_cache[idx].LUT[x]->x, tmp, modulus,
                         fp_cache[idx].LUT[x]->x);

       if (err == MP_OKAY)
         /* get 1/z^3 */
         err = mp_mulmod(tmp, fp_cache[idx].LUT[x]->z, modulus, tmp);

       if (err == MP_OKAY)
         /* fix y */
         err = mp_mulmod(fp_cache[idx].LUT[x]->y, tmp, modulus,
                         fp_cache[idx].LUT[x]->y);

       if (err == MP_OKAY)
         /* free z */
         mp_clear(fp_cache[idx].LUT[x]->z);
   }

  errout:

   mp_clear(tmp);
#ifdef WOLFSSL_SMALL_STACK
   XFREE(tmp, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

   if (err == MP_OKAY) {
       fp_cache[idx].LUT_set = 1;
       return MP_OKAY;
   }

   /* err cleanup */
   for (y = 0; y < (1U<<FP_LUT); y++) {
      wc_ecc_del_point(fp_cache[idx].LUT[y]);
      fp_cache[idx].LUT[y] = NULL;
   }
   wc_ecc_del_point(fp_cache[idx].g);
   fp_cache[idx].g         = NULL;
   fp_cache[idx].LUT_set   = 0;
   fp_cache[idx].lru_count = 0;
   mp_clear(&fp_cache[idx].mu);

   return err;
}

/* perform a fixed point ECC mulmod */
static int accel_fp_mul(int idx, const mp_int* k, ecc_point *R, mp_int* a,
                        mp_int* modulus, mp_digit mp, int map)
{
#ifdef WOLFCRYPT_HAVE_SAKKE
    #define KB_SIZE 256
#else
    #define KB_SIZE 128
#endif

#ifdef WOLFSSL_SMALL_STACK
   unsigned char* kb = NULL;
   mp_int*        tk = NULL;
   mp_int*        order = NULL;
#else
   unsigned char kb[KB_SIZE];
   mp_int        tk[1];
   mp_int        order[1];
#endif
   int      x, err;
   unsigned y, z = 0, bitlen, bitpos, lut_gap;
   int first;
   int tk_zeroize = 0;

#ifdef WOLFSSL_SMALL_STACK
   tk = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (tk == NULL) {
      err = MEMORY_E; goto done;
   }
   order = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (order == NULL) {
      err = MEMORY_E; goto done;
   }
#endif

   if (mp_init_multi(tk, order, NULL, NULL, NULL, NULL) != MP_OKAY) {
       err = MP_INIT_E; goto done;
   }

   if ((err = mp_copy(k, tk)) != MP_OKAY)
       goto done;
   tk_zeroize = 1;

#ifdef WOLFSSL_CHECK_MEM_ZERO
   mp_memzero_add("accel_fp_mul tk", tk);
#endif

   /* if it's smaller than modulus we fine */
   if (mp_unsigned_bin_size(k) > mp_unsigned_bin_size(modulus)) {
      /* find order */
       y = (unsigned)mp_unsigned_bin_size(modulus);
      for (x = 0; ecc_sets[x].size; x++) {
         if (y <= (unsigned)ecc_sets[x].size) break;
      }

      /* back off if we are on the 521 bit curve */
      if (y == 66) --x;

      if ((err = mp_read_radix(order, ecc_sets[x].order,
                                                MP_RADIX_HEX)) != MP_OKAY) {
         goto done;
      }

      /* k must be less than modulus */
      if (mp_cmp(tk, order) != MP_LT) {
         if ((err = mp_mod(tk, order, tk)) != MP_OKAY) {
            goto done;
         }
      }
   }

   /* get bitlen and round up to next multiple of FP_LUT */
   bitlen  = (unsigned)mp_unsigned_bin_size(modulus) << 3;
   x       = bitlen % FP_LUT;
   if (x) {
      bitlen += FP_LUT - (unsigned)x;
   }
   lut_gap = bitlen / FP_LUT;

   /* get the k value */
   if (mp_unsigned_bin_size(tk) > (int)(KB_SIZE - 2)) {
      err = BUFFER_E; goto done;
   }

   /* store k */
#ifdef WOLFSSL_SMALL_STACK
   kb = (unsigned char*)XMALLOC(KB_SIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   if (kb == NULL) {
      err = MEMORY_E; goto done;
   }
#endif

   XMEMSET(kb, 0, KB_SIZE);
   if ((err = mp_to_unsigned_bin(tk, kb)) == MP_OKAY) {
   #ifdef WOLFSSL_CHECK_MEM_ZERO
      wc_MemZero_Add("accel_fp_mul kb", kb, KB_SIZE);
   #endif
      /* let's reverse kb so it's little endian */
      x = 0;
      y = (unsigned)mp_unsigned_bin_size(tk);
      if (y > 0) {
          y -= 1;
      }

      while ((unsigned)x < y) {
         z = kb[x]; kb[x] = kb[y]; kb[y] = (byte)z;
         ++x; --y;
      }

      /* at this point we can start, yipee */
      first = 1;
      for (x = (int)lut_gap-1; x >= 0; x--) {
          /* extract FP_LUT bits from kb spread out by lut_gap bits and offset
             by x bits from the start */
          bitpos = (unsigned)x;
          for (y = z = 0; y < FP_LUT; y++) {
             z |= ((kb[bitpos>>3] >> (bitpos&7)) & 1) << y;
             bitpos += lut_gap;  /* it's y*lut_gap + x, but here we can avoid
                                    the mult in each loop */
          }

          /* double if not first */
          if (!first) {
             if ((err = ecc_projective_dbl_point_safe(R, R, a, modulus,
                                                              mp)) != MP_OKAY) {
                break;
             }
          }

          /* add if not first, otherwise copy */
          if (!first && z) {
             if ((err = ecc_projective_add_point_safe(R, fp_cache[idx].LUT[z],
                                       R, a, modulus, mp, &first)) != MP_OKAY) {
                break;
             }
          } else if (z) {
             if ((mp_copy(fp_cache[idx].LUT[z]->x, R->x) != MP_OKAY) ||
                 (mp_copy(fp_cache[idx].LUT[z]->y, R->y) != MP_OKAY) ||
                 (mp_copy(&fp_cache[idx].mu,       R->z) != MP_OKAY)) {
                 err = GEN_MEM_ERR;
                 break;
             }
             first = 0;
          }
      }
   }

   if (err == MP_OKAY) {
      (void) z; /* Acknowledge the unused assignment */
      ForceZero(kb, KB_SIZE);

      /* map R back from projective space */
      if (map) {
         err = ecc_map(R, modulus, mp);
      } else {
         err = MP_OKAY;
      }
   }

done:
   /* cleanup */
   mp_clear(order);
   /* Ensure it was initialized. */
   if (tk_zeroize) {
       mp_forcezero(tk);
   }

#ifdef WOLFSSL_SMALL_STACK
   XFREE(kb, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   XFREE(order, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   XFREE(tk, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
   wc_MemZero_Check(kb, KB_SIZE);
   mp_memzero_check(tk);
#endif

#undef KB_SIZE

   return err;
}
#endif

#ifdef ECC_SHAMIR
#if !defined(WOLFSSL_SP_MATH)
/* perform a fixed point ECC mulmod */
static int accel_fp_mul2add(int idx1, int idx2,
                            mp_int* kA, mp_int* kB,
                            ecc_point *R, mp_int* a,
                            mp_int* modulus, mp_digit mp)
{
#define KB_SIZE 128

#ifdef WOLFSSL_SMALL_STACK
   unsigned char* kb[2] = {NULL, NULL};
   mp_int*        tka = NULL;
   mp_int*        tkb = NULL;
   mp_int*        order = NULL;
#else
   unsigned char kb[2][KB_SIZE];
   mp_int        tka[1];
   mp_int        tkb[1];
   mp_int        order[1];
#endif
   int      x, err;
   unsigned y, z, bitlen, bitpos, lut_gap, zA, zB;
   int first;

#ifdef WOLFSSL_SMALL_STACK
   tka = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (tka == NULL) {
      err = MEMORY_E; goto done;
   }
   tkb = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (tkb == NULL) {
      err = MEMORY_E; goto done;
   }
   order = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC);
   if (order == NULL) {
      err = MEMORY_E; goto done;
   }
#endif

   if (mp_init_multi(tka, tkb, order, NULL, NULL, NULL) != MP_OKAY) {
      err = MP_INIT_E; goto done;
   }

   /* if it's smaller than modulus we fine */
   if (mp_unsigned_bin_size(kA) > mp_unsigned_bin_size(modulus)) {
      /* find order */
      y = (unsigned)mp_unsigned_bin_size(modulus);
      for (x = 0; ecc_sets[x].size; x++) {
         if (y <= (unsigned)ecc_sets[x].size) break;
      }

      /* back off if we are on the 521 bit curve */
      if (y == 66) --x;

      if ((err = mp_read_radix(order, ecc_sets[x].order,
                                                MP_RADIX_HEX)) != MP_OKAY) {
         goto done;
      }

      /* kA must be less than modulus */
      if (mp_cmp(kA, order) != MP_LT) {
         if ((err = mp_mod(kA, order, tka)) != MP_OKAY) {
            goto done;
         }
      } else {
         if ((err = mp_copy(kA, tka)) != MP_OKAY) {
            goto done;
         }
      }
   } else {
      if ((err = mp_copy(kA, tka)) != MP_OKAY) {
         goto done;
      }
   }
#ifdef WOLFSSL_CHECK_MEM_ZERO
   mp_memzero_add("accel_fp_mul2add tka", tka);
#endif

   /* if it's smaller than modulus we fine */
   if (mp_unsigned_bin_size(kB) > mp_unsigned_bin_size(modulus)) {
      /* find order */
      y = (unsigned)mp_unsigned_bin_size(modulus);
      for (x = 0; ecc_sets[x].size; x++) {
         if (y <= (unsigned)ecc_sets[x].size) break;
      }

      /* back off if we are on the 521 bit curve */
      if (y == 66) --x;

      if ((err = mp_read_radix(order, ecc_sets[x].order,
                                                MP_RADIX_HEX)) != MP_OKAY) {
         goto done;
      }

      /* kB must be less than modulus */
      if (mp_cmp(kB, order) != MP_LT) {
         if ((err = mp_mod(kB, order, tkb)) != MP_OKAY) {
            goto done;
         }
      } else {
         if ((err = mp_copy(kB, tkb)) != MP_OKAY) {
            goto done;
         }
      }
   } else {
      if ((err = mp_copy(kB, tkb)) != MP_OKAY) {
         goto done;
      }
   }
#ifdef WOLFSSL_CHECK_MEM_ZERO
   mp_memzero_add("accel_fp_mul2add tkb", tkb);
#endif

   /* get bitlen and round up to next multiple of FP_LUT */
   bitlen  = (unsigned)mp_unsigned_bin_size(modulus) << 3;
   x       = bitlen % FP_LUT;
   if (x) {
      bitlen += FP_LUT - (unsigned)x;
   }
   lut_gap = bitlen / FP_LUT;

   /* get the k value */
   if ((mp_unsigned_bin_size(tka) > (int)(KB_SIZE - 2)) ||
       (mp_unsigned_bin_size(tkb) > (int)(KB_SIZE - 2))  ) {
      err = BUFFER_E; goto done;
   }

   /* store k */
#ifdef WOLFSSL_SMALL_STACK
   kb[0] = (unsigned char*)XMALLOC(KB_SIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   if (kb[0] == NULL) {
      err = MEMORY_E; goto done;
   }
#endif

   XMEMSET(kb[0], 0, KB_SIZE);
   if ((err = mp_to_unsigned_bin(tka, kb[0])) != MP_OKAY) {
      goto done;
   }
#ifdef WOLFSSL_CHECK_MEM_ZERO
   wc_MemZero_Add("accel_fp_mul2add kb[0]", kb[0], KB_SIZE);
#endif

   /* let's reverse kb so it's little endian */
   x = 0;
   y = (unsigned)mp_unsigned_bin_size(tka);
   if (y > 0) {
       y -= 1;
   }
   mp_clear(tka);
   while ((unsigned)x < y) {
      z = kb[0][x]; kb[0][x] = kb[0][y]; kb[0][y] = (byte)z;
      ++x; --y;
   }

   /* store b */
#ifdef WOLFSSL_SMALL_STACK
   kb[1] = (unsigned char*)XMALLOC(KB_SIZE, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   if (kb[1] == NULL) {
      err = MEMORY_E; goto done;
   }
#endif

   XMEMSET(kb[1], 0, KB_SIZE);
#ifdef WOLFSSL_CHECK_MEM_ZERO
   wc_MemZero_Add("accel_fp_mul2add kb[1]", kb[1], KB_SIZE);
#endif
   if ((err = mp_to_unsigned_bin(tkb, kb[1])) == MP_OKAY) {
      x = 0;
      y = (unsigned)mp_unsigned_bin_size(tkb);
      if (y > 0) {
          y -= 1;
      }

      while ((unsigned)x < y) {
         z = kb[1][x]; kb[1][x] = kb[1][y]; kb[1][y] = (byte)z;
         ++x; --y;
      }

      /* at this point we can start, yipee */
      first = 1;
      for (x = (int)lut_gap-1; x >= 0; x--) {
          /* extract FP_LUT bits from kb spread out by lut_gap bits and
             offset by x bits from the start */
          bitpos = (unsigned)x;
          for (y = zA = zB = 0; y < FP_LUT; y++) {
             zA |= ((kb[0][bitpos>>3] >> (bitpos&7)) & 1) << y;
             zB |= ((kb[1][bitpos>>3] >> (bitpos&7)) & 1) << y;
             bitpos += lut_gap;    /* it's y*lut_gap + x, but here we can avoid
                                      the mult in each loop */
          }

          /* double if not first */
          if (!first) {
             if ((err = ecc_projective_dbl_point_safe(R, R, a, modulus,
                                                              mp)) != MP_OKAY) {
                break;
             }

             /* add if not first, otherwise copy */
             if (zA) {
                if ((err = ecc_projective_add_point_safe(R,
                                             fp_cache[idx1].LUT[zA], R, a,
                                             modulus, mp, &first)) != MP_OKAY) {
                   break;
                }
             }

             if (zB) {
                if ((err = ecc_projective_add_point_safe(R,
                                             fp_cache[idx2].LUT[zB], R, a,
                                             modulus, mp, &first)) != MP_OKAY) {
                   break;
                }
             }
          } else {
             if (zA) {
                 if ((mp_copy(fp_cache[idx1].LUT[zA]->x, R->x) != MP_OKAY) ||
                     (mp_copy(fp_cache[idx1].LUT[zA]->y, R->y) != MP_OKAY) ||
                     (mp_copy(&fp_cache[idx1].mu,        R->z) != MP_OKAY)) {
                     err = GEN_MEM_ERR;
                     break;
                 }
                    first = 0;
             }
             if (zB && first == 0) {
                if ((err = ecc_projective_add_point_safe(R,
                                        fp_cache[idx2].LUT[zB], R, a,
                                        modulus, mp, &first)) != MP_OKAY){
                   break;
                }
             } else if (zB && first == 1) {
                 if ((mp_copy(fp_cache[idx2].LUT[zB]->x, R->x) != MP_OKAY) ||
                     (mp_copy(fp_cache[idx2].LUT[zB]->y, R->y) != MP_OKAY) ||
                     (mp_copy(&fp_cache[idx2].mu,        R->z) != MP_OKAY)) {
                     err = GEN_MEM_ERR;
                     break;
                 }
                    first = 0;
             }
          }
      }
   }

done:
   /* cleanup */
   mp_forcezero(tkb);
   mp_forcezero(tka);
   mp_clear(order);

#ifdef WOLFSSL_SMALL_STACK
   if (kb[0])
#endif
      ForceZero(kb[0], KB_SIZE);
#ifdef WOLFSSL_SMALL_STACK
   if (kb[1])
#endif
      ForceZero(kb[1], KB_SIZE);

#ifdef WOLFSSL_SMALL_STACK
   XFREE(kb[1], NULL, DYNAMIC_TYPE_ECC_BUFFER);
   XFREE(kb[0], NULL, DYNAMIC_TYPE_ECC_BUFFER);
   XFREE(order, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   XFREE(tkb, NULL, DYNAMIC_TYPE_ECC_BUFFER);
   XFREE(tka, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#elif defined(WOLFSSL_CHECK_MEM_ZERO)
   wc_MemZero_Check(kb[1], KB_SIZE);
   wc_MemZero_Check(kb[0], KB_SIZE);
   mp_memzero_check(tkb);
   mp_memzero_check(tka);
#endif

#undef KB_SIZE

    if (err != MP_OKAY)
        return err;

   return ecc_map(R, modulus, mp);
}


/** ECC Fixed Point mulmod global with heap hint used
  Computes kA*A + kB*B = C using Shamir's Trick
  A        First point to multiply
  kA       What to multiple A by
  B        Second point to multiply
  kB       What to multiple B by
  C        [out] Destination point (can overlap with A or B)
  a        ECC curve parameter a
  modulus  Modulus for curve
  return MP_OKAY on success
*/
int ecc_mul2add(ecc_point* A, mp_int* kA,
                ecc_point* B, mp_int* kB,
                ecc_point* C, mp_int* a, mp_int* modulus, void* heap)
{
   int  idx1 = -1, idx2 = -1, err, mpInit = 0;
   mp_digit mp;
#ifdef WOLFSSL_SMALL_STACK
   mp_int   *mu = (mp_int *)XMALLOC(sizeof *mu, NULL, DYNAMIC_TYPE_ECC_BUFFER);

   if (mu == NULL)
       return MP_MEM;
#else
   mp_int   mu[1];
#endif

   err = mp_init(mu);
   if (err != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
       XFREE(mu, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif
       return err;
   }

#ifndef HAVE_THREAD_LS
   if (initMutex == 0) { /* extra sanity check if wolfCrypt_Init not called */
        wc_InitMutex(&ecc_fp_lock);
        initMutex = 1;
   }

   if (wc_LockMutex(&ecc_fp_lock) != 0) {
#ifdef WOLFSSL_SMALL_STACK
       XFREE(mu, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif
      return BAD_MUTEX_E;
   }
#endif /* HAVE_THREAD_LS */

      SAVE_VECTOR_REGISTERS(err = _svr_ret;);

      /* find point */
      idx1 = find_base(A);

      /* no entry? */
      if (idx1 == -1) {
         /* find hole and add it */
         if ((idx1 = find_hole()) >= 0) {
            err = add_entry(idx1, A);
         }
      }
      if (err == MP_OKAY && idx1 != -1) {
         /* increment LRU */
         ++(fp_cache[idx1].lru_count);
      }

      if (err == MP_OKAY) {
        /* find point */
        idx2 = find_base(B);

        /* no entry? */
        if (idx2 == -1) {
           /* find hole and add it */
           if ((idx2 = find_hole()) >= 0)
              err = add_entry(idx2, B);
         }
      }

      if (err == MP_OKAY && idx2 != -1) {
         /* increment LRU */
         ++(fp_cache[idx2].lru_count);
      }

      if (err == MP_OKAY) {
        /* if it's >= 2 AND the LUT is not set build the LUT */
        if (idx1 >= 0 && fp_cache[idx1].lru_count >= 2 && !fp_cache[idx1].LUT_set) {
           /* compute mp */
           err = mp_montgomery_setup(modulus, &mp);

           if (err == MP_OKAY) {
             mpInit = 1;
             err = mp_montgomery_calc_normalization(mu, modulus);
           }

           if (err == MP_OKAY)
             /* build the LUT */
             err = build_lut(idx1, a, modulus, mp, mu);
        }
      }

      if (err == MP_OKAY) {
        /* if it's >= 2 AND the LUT is not set build the LUT */
        if (idx2 >= 0 && fp_cache[idx2].lru_count >= 2 && !fp_cache[idx2].LUT_set) {
           if (mpInit == 0) {
                /* compute mp */
                err = mp_montgomery_setup(modulus, &mp);
                if (err == MP_OKAY) {
                    mpInit = 1;
                    err = mp_montgomery_calc_normalization(mu, modulus);
                }
            }

            if (err == MP_OKAY)
              /* build the LUT */
              err = build_lut(idx2, a, modulus, mp, mu);
        }
      }


      if (err == MP_OKAY) {
        if (idx1 >=0 && idx2 >= 0 && fp_cache[idx1].LUT_set &&
                                     fp_cache[idx2].LUT_set) {
           if (mpInit == 0) {
              /* compute mp */
              err = mp_montgomery_setup(modulus, &mp);
           }
           if (err == MP_OKAY)
             err = accel_fp_mul2add(idx1, idx2, kA, kB, C, a, modulus, mp);
        } else {
           err = normal_ecc_mul2add(A, kA, B, kB, C, a, modulus, heap);
        }
      }

      RESTORE_VECTOR_REGISTERS();

#ifndef HAVE_THREAD_LS
    wc_UnLockMutex(&ecc_fp_lock);
#endif /* HAVE_THREAD_LS */
    mp_clear(mu);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(mu, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return err;
}
#endif
#endif /* ECC_SHAMIR */

/** ECC Fixed Point mulmod global
    k        The multiplicand
    G        Base point to multiply
    R        [out] Destination of product
    a        ECC curve parameter a
    modulus  The modulus for the curve
    map      [boolean] If non-zero maps the point back to affine coordinates,
             otherwise it's left in jacobian-montgomery form
    return MP_OKAY if successful
*/
int wc_ecc_mulmod_ex(const mp_int* k, ecc_point *G, ecc_point *R, mp_int* a,
    mp_int* modulus, int map, void* heap)
{
#if !defined(WOLFSSL_SP_MATH)
   int   idx, err = MP_OKAY;
   mp_digit mp;
#ifdef WOLFSSL_SMALL_STACK
   mp_int   *mu = NULL;
#else
   mp_int   mu[1];
#endif
   int      mpSetup = 0;
#ifndef HAVE_THREAD_LS
   int got_ecc_fp_lock = 0;
#endif

   if (k == NULL || G == NULL || R == NULL || a == NULL || modulus == NULL) {
       return ECC_BAD_ARG_E;
   }

   /* k can't have more bits than modulus count plus 1 */
   if (mp_count_bits(k) > mp_count_bits(modulus) + 1) {
      return ECC_OUT_OF_RANGE_E;
   }

#ifdef WOLFSSL_SMALL_STACK
   if ((mu = (mp_int *)XMALLOC(sizeof(*mu), NULL, DYNAMIC_TYPE_ECC_BUFFER)) == NULL)
       return MP_MEM;
#endif

   if (mp_init(mu) != MP_OKAY) {
       err = MP_INIT_E;
       goto out;
   }

#ifndef HAVE_THREAD_LS
   if (initMutex == 0) { /* extra sanity check if wolfCrypt_Init not called */
        wc_InitMutex(&ecc_fp_lock);
        initMutex = 1;
   }

   if (wc_LockMutex(&ecc_fp_lock) != 0) {
      err = BAD_MUTEX_E;
      goto out;
   }
   got_ecc_fp_lock = 1;
#endif /* HAVE_THREAD_LS */

      SAVE_VECTOR_REGISTERS(err = _svr_ret; goto out;);

      /* find point */
      idx = find_base(G);

      /* no entry? */
      if (idx == -1) {
         /* find hole and add it */
         idx = find_hole();

         if (idx >= 0)
            err = add_entry(idx, G);
      }
      if (err == MP_OKAY && idx >= 0) {
         /* increment LRU */
         ++(fp_cache[idx].lru_count);
      }


      if (err == MP_OKAY) {
        /* if it's 2 build the LUT, if it's higher just use the LUT */
        if (idx >= 0 && fp_cache[idx].lru_count >= 2 && !fp_cache[idx].LUT_set) {
           /* compute mp */
           err = mp_montgomery_setup(modulus, &mp);

           if (err == MP_OKAY) {
             /* compute mu */
             mpSetup = 1;
             err = mp_montgomery_calc_normalization(mu, modulus);
           }

           if (err == MP_OKAY)
             /* build the LUT */
             err = build_lut(idx, a, modulus, mp, mu);
        }
      }

      if (err == MP_OKAY) {
        if (idx >= 0 && fp_cache[idx].LUT_set) {
           if (mpSetup == 0) {
              /* compute mp */
              err = mp_montgomery_setup(modulus, &mp);
           }
           if (err == MP_OKAY)
             err = accel_fp_mul(idx, k, R, a, modulus, mp, map);
        } else {
           err = normal_ecc_mulmod(k, G, R, a, modulus, NULL, map, heap);
        }
      }

      RESTORE_VECTOR_REGISTERS();

  out:

#ifndef HAVE_THREAD_LS
    if (got_ecc_fp_lock)
        wc_UnLockMutex(&ecc_fp_lock);
#endif /* HAVE_THREAD_LS */
    mp_clear(mu);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(mu, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return err;

#else /* WOLFSSL_SP_MATH */

    if (k == NULL || G == NULL || R == NULL || a == NULL || modulus == NULL) {
        return ECC_BAD_ARG_E;
    }
    if (mp_count_bits(G->x) > mp_count_bits(modulus) ||
        mp_count_bits(G->y) > mp_count_bits(modulus) ||
        mp_count_bits(G->z) > mp_count_bits(modulus)) {
        return IS_POINT_E;
    }

#ifndef WOLFSSL_SP_NO_256
    if (mp_count_bits(modulus) == 256) {
        int ret;
        SAVE_VECTOR_REGISTERS(return _svr_ret);
#ifdef SM2_SP_IMPL_AVAILABLE
     #ifdef WOLFSSL_SM2
        if (!mp_is_bit_set(modulus, 224)) {
            ret = sp_ecc_mulmod_sm2_256(k, G, R, map, heap);
        }
        else
     #endif
#endif
        {
            ret = sp_ecc_mulmod_256(k, G, R, map, heap);
        }
        RESTORE_VECTOR_REGISTERS();
        return ret;
    }
#endif
#ifdef WOLFSSL_SP_384
    if (mp_count_bits(modulus) == 384) {
        int ret;
        SAVE_VECTOR_REGISTERS(return _svr_ret);
        ret = sp_ecc_mulmod_384(k, G, R, map, heap);
        RESTORE_VECTOR_REGISTERS();
        return ret;
    }
#endif
#ifdef WOLFSSL_SP_521
    if (mp_count_bits(modulus) == 521) {
        int ret;
        SAVE_VECTOR_REGISTERS(return _svr_ret);
        ret = sp_ecc_mulmod_521(k, G, R, map, heap);
        RESTORE_VECTOR_REGISTERS();
        return ret;
    }
#endif
    return WC_KEY_SIZE_E;
#endif /* WOLFSSL_SP_MATH */
}

/** ECC Fixed Point mulmod global
    k        The multiplicand
    G        Base point to multiply
    R        [out] Destination of product
    a        ECC curve parameter a
    modulus  The modulus for the curve
    map      [boolean] If non-zero maps the point back to affine coordinates,
             otherwise it's left in jacobian-montgomery form
    return MP_OKAY if successful
*/
int wc_ecc_mulmod_ex2(const mp_int* k, ecc_point *G, ecc_point *R, mp_int* a,
    mp_int* modulus, mp_int* order, WC_RNG* rng, int map, void* heap)
{
#if !defined(WOLFSSL_SP_MATH)
   int   idx, err = MP_OKAY;
   mp_digit mp;
#ifdef WOLFSSL_SMALL_STACK
   mp_int   *mu = NULL;
#else
   mp_int   mu[1];
#endif
   int      mpSetup = 0;
#ifndef HAVE_THREAD_LS
   int got_ecc_fp_lock = 0;
#endif

   if (k == NULL || G == NULL || R == NULL || a == NULL || modulus == NULL ||
                                                                order == NULL) {
       return ECC_BAD_ARG_E;
   }

   /* k can't have more bits than order */
   if (mp_count_bits(k) > mp_count_bits(order)) {
      return ECC_OUT_OF_RANGE_E;
   }

#ifdef WOLFSSL_SMALL_STACK
   if ((mu = (mp_int *)XMALLOC(sizeof(*mu), NULL, DYNAMIC_TYPE_ECC_BUFFER)) == NULL)
       return MP_MEM;
#endif

   if (mp_init(mu) != MP_OKAY) {
       err = MP_INIT_E;
       goto out;
   }

#ifndef HAVE_THREAD_LS
   if (initMutex == 0) { /* extra sanity check if wolfCrypt_Init not called */
        wc_InitMutex(&ecc_fp_lock);
        initMutex = 1;
   }

   if (wc_LockMutex(&ecc_fp_lock) != 0) {
      err = BAD_MUTEX_E;
      goto out;
   }
   got_ecc_fp_lock = 1;
#endif /* HAVE_THREAD_LS */

      SAVE_VECTOR_REGISTERS(err = _svr_ret; goto out;);

      /* find point */
      idx = find_base(G);

      /* no entry? */
      if (idx == -1) {
         /* find hole and add it */
         idx = find_hole();

         if (idx >= 0)
            err = add_entry(idx, G);
      }
      if (err == MP_OKAY && idx >= 0) {
         /* increment LRU */
         ++(fp_cache[idx].lru_count);
      }


      if (err == MP_OKAY) {
        /* if it's 2 build the LUT, if it's higher just use the LUT */
        if (idx >= 0 && fp_cache[idx].lru_count >= 2 && !fp_cache[idx].LUT_set) {
           /* compute mp */
           err = mp_montgomery_setup(modulus, &mp);

           if (err == MP_OKAY) {
             /* compute mu */
             mpSetup = 1;
             err = mp_montgomery_calc_normalization(mu, modulus);
           }

           if (err == MP_OKAY)
             /* build the LUT */
             err = build_lut(idx, a, modulus, mp, mu);
        }
      }

      if (err == MP_OKAY) {
        if (idx >= 0 && fp_cache[idx].LUT_set) {
           if (mpSetup == 0) {
              /* compute mp */
              err = mp_montgomery_setup(modulus, &mp);
           }
           if (err == MP_OKAY)
             err = accel_fp_mul(idx, k, R, a, modulus, mp, map);
        } else {
          err = normal_ecc_mulmod(k, G, R, a, modulus, rng, map, heap);
        }
      }

      RESTORE_VECTOR_REGISTERS();

  out:

#ifndef HAVE_THREAD_LS
    if (got_ecc_fp_lock)
        wc_UnLockMutex(&ecc_fp_lock);
#endif /* HAVE_THREAD_LS */
    mp_clear(mu);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(mu, NULL, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return err;

#else /* WOLFSSL_SP_MATH */

    (void)rng;

    if (k == NULL || G == NULL || R == NULL || a == NULL || modulus == NULL ||
                                                                order == NULL) {
        return ECC_BAD_ARG_E;
    }
    if (mp_count_bits(G->x) > mp_count_bits(modulus) ||
        mp_count_bits(G->y) > mp_count_bits(modulus) ||
        mp_count_bits(G->z) > mp_count_bits(modulus)) {
        return IS_POINT_E;
    }

#ifndef WOLFSSL_SP_NO_256
    if (mp_count_bits(modulus) == 256) {
        int ret;
        SAVE_VECTOR_REGISTERS(return _svr_ret);
#ifdef SM2_SP_IMPL_AVAILABLE
    #ifdef WOLFSSL_SM2
        if (!mp_is_bit_set(modulus, 224)) {
            ret = sp_ecc_mulmod_sm2_256(k, G, R, map, heap);
        }
        else
    #endif
#endif
        {
            ret = sp_ecc_mulmod_256(k, G, R, map, heap);
        }
        RESTORE_VECTOR_REGISTERS();
        return ret;
    }
#endif
#ifdef WOLFSSL_SP_384
    if (mp_count_bits(modulus) == 384) {
        int ret;
        SAVE_VECTOR_REGISTERS(return _svr_ret);
        ret = sp_ecc_mulmod_384(k, G, R, map, heap);
        RESTORE_VECTOR_REGISTERS();
        return ret;
    }
#endif
#ifdef WOLFSSL_SP_521
    if (mp_count_bits(modulus) == 521) {
        int ret;
        SAVE_VECTOR_REGISTERS(return _svr_ret);
        ret = sp_ecc_mulmod_521(k, G, R, map, heap);
        RESTORE_VECTOR_REGISTERS();
        return ret;
    }
#endif
    return WC_KEY_SIZE_E;
#endif /* WOLFSSL_SP_MATH */
}

#if !defined(WOLFSSL_SP_MATH)
/* helper function for freeing the cache ...
   must be called with the cache mutex locked */
static void wc_ecc_fp_free_cache(void)
{
   unsigned x, y;
   for (x = 0; x < FP_ENTRIES; x++) {
      if (fp_cache[x].g != NULL) {
         for (y = 0; y < (1U<<FP_LUT); y++) {
            wc_ecc_del_point(fp_cache[x].LUT[y]);
            fp_cache[x].LUT[y] = NULL;
         }
         wc_ecc_del_point(fp_cache[x].g);
         fp_cache[x].g         = NULL;
         mp_clear(&fp_cache[x].mu);
         fp_cache[x].LUT_set   = 0;
         fp_cache[x].lru_count = 0;
         fp_cache[x].lock = 0;
      }
   }
}
#endif


/** Init the Fixed Point cache */
void wc_ecc_fp_init(void)
{
#ifndef WOLFSSL_SP_MATH
#ifndef HAVE_THREAD_LS
   if (initMutex == 0) {
        wc_InitMutex(&ecc_fp_lock);
        initMutex = 1;
   }
#endif
#endif
}


/** Free the Fixed Point cache */
WOLFSSL_ABI
void wc_ecc_fp_free(void)
{
#if !defined(WOLFSSL_SP_MATH)
#ifndef HAVE_THREAD_LS
   if (initMutex == 0) { /* extra sanity check if wolfCrypt_Init not called */
        wc_InitMutex(&ecc_fp_lock);
        initMutex = 1;
   }

   if (wc_LockMutex(&ecc_fp_lock) == 0) {
#endif /* HAVE_THREAD_LS */

       wc_ecc_fp_free_cache();

#ifndef HAVE_THREAD_LS
       wc_UnLockMutex(&ecc_fp_lock);
       wc_FreeMutex(&ecc_fp_lock);
       initMutex = 0;
   }
#endif /* HAVE_THREAD_LS */
#endif
}


#endif /* FP_ECC */

int wc_ecc_set_rng(ecc_key* key, WC_RNG* rng)
{
    int err = 0;

#ifdef ECC_TIMING_RESISTANT
    if (key == NULL) {
        err = BAD_FUNC_ARG;
    }
    else {
        key->rng = rng;
    }
#else
    (void)key;
    (void)rng;
    /* report success, not an error if ECC_TIMING_RESISTANT is not defined */
#endif

    return err;
}

#ifdef HAVE_ECC_ENCRYPT


enum ecCliState {
    ecCLI_INIT      = 1,
    ecCLI_SALT_GET  = 2,
    ecCLI_SALT_SET  = 3,
    ecCLI_SENT_REQ  = 4,
    ecCLI_RECV_RESP = 5,
    ecCLI_BAD_STATE = 99
};

enum ecSrvState {
    ecSRV_INIT      = 1,
    ecSRV_SALT_GET  = 2,
    ecSRV_SALT_SET  = 3,
    ecSRV_RECV_REQ  = 4,
    ecSRV_SENT_RESP = 5,
    ecSRV_BAD_STATE = 99
};


struct ecEncCtx {
    const byte* kdfSalt;   /* optional salt for kdf */
    const byte* kdfInfo;   /* optional info for kdf */
    const byte* macSalt;   /* optional salt for mac */
    word32    kdfSaltSz;   /* size of kdfSalt */
    word32    kdfInfoSz;   /* size of kdfInfo */
    word32    macSaltSz;   /* size of macSalt */
    void*     heap;        /* heap hint for memory used */
    byte      clientSalt[EXCHANGE_SALT_SZ];  /* for msg exchange */
    byte      serverSalt[EXCHANGE_SALT_SZ];  /* for msg exchange */
    byte      encAlgo;     /* which encryption type */
    byte      kdfAlgo;     /* which key derivation function type */
    byte      macAlgo;     /* which mac function type */
    byte      protocol;    /* are we REQ_RESP client or server ? */
    byte      cliSt;       /* protocol state, for sanity checks */
    byte      srvSt;       /* protocol state, for sanity checks */
    WC_RNG*   rng;
};

/* optional set info, can be called before or after set_peer_salt */
int wc_ecc_ctx_set_algo(ecEncCtx* ctx, byte encAlgo, byte kdfAlgo, byte macAlgo)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->encAlgo = encAlgo;
    ctx->kdfAlgo = kdfAlgo;
    ctx->macAlgo = macAlgo;

    return 0;
}


const byte* wc_ecc_ctx_get_own_salt(ecEncCtx* ctx)
{
    if (ctx == NULL || ctx->protocol == 0)
        return NULL;

    if (ctx->protocol == REQ_RESP_CLIENT) {
        if (ctx->cliSt == ecCLI_INIT) {
            ctx->cliSt =  ecCLI_SALT_GET;
            return ctx->clientSalt;
        }
        else {
            ctx->cliSt = ecCLI_BAD_STATE;
            return NULL;
        }
    }
    else if (ctx->protocol == REQ_RESP_SERVER) {
        if (ctx->srvSt == ecSRV_INIT) {
            ctx->srvSt =  ecSRV_SALT_GET;
            return ctx->serverSalt;
        }
        else {
            ctx->srvSt = ecSRV_BAD_STATE;
            return NULL;
        }
    }

    return NULL;
}


/* optional set info, can be called before or after set_peer_salt */
int wc_ecc_ctx_set_info(ecEncCtx* ctx, const byte* info, int sz)
{
    if (ctx == NULL || info == 0 || sz < 0)
        return BAD_FUNC_ARG;

    ctx->kdfInfo   = info;
    ctx->kdfInfoSz = (word32)sz;

    return 0;
}


static const char* exchange_info = "Secure Message Exchange";

int wc_ecc_ctx_set_peer_salt(ecEncCtx* ctx, const byte* salt)
{
    byte tmp[EXCHANGE_SALT_SZ/2];
    int  halfSz = EXCHANGE_SALT_SZ/2;

    if (ctx == NULL || ctx->protocol == 0 || salt == NULL)
        return BAD_FUNC_ARG;

    if (ctx->protocol == REQ_RESP_CLIENT) {
        XMEMCPY(ctx->serverSalt, salt, EXCHANGE_SALT_SZ);
        if (ctx->cliSt == ecCLI_SALT_GET)
            ctx->cliSt =  ecCLI_SALT_SET;
        else {
            ctx->cliSt =  ecCLI_BAD_STATE;
            return BAD_STATE_E;
        }
    }
    else {
        XMEMCPY(ctx->clientSalt, salt, EXCHANGE_SALT_SZ);
        if (ctx->srvSt == ecSRV_SALT_GET)
            ctx->srvSt =  ecSRV_SALT_SET;
        else {
            ctx->srvSt =  ecSRV_BAD_STATE;
            return BAD_STATE_E;
        }
    }

    /* mix half and half */
    /* tmp stores 2nd half of client before overwrite */
    XMEMCPY(tmp, ctx->clientSalt + halfSz, (size_t)halfSz);
    XMEMCPY(ctx->clientSalt + halfSz, ctx->serverSalt, (size_t)halfSz);
    XMEMCPY(ctx->serverSalt, tmp, (size_t)halfSz);

    ctx->kdfSalt   = ctx->clientSalt;
    ctx->kdfSaltSz = EXCHANGE_SALT_SZ;

    ctx->macSalt   = ctx->serverSalt;
    ctx->macSaltSz = EXCHANGE_SALT_SZ;

    if (ctx->kdfInfo == NULL) {
        /* default info */
        ctx->kdfInfo   = (const byte*)exchange_info;
        ctx->kdfInfoSz = EXCHANGE_INFO_SZ;
    }

    return 0;
}

/* Set the salt pointer into context.
 *
 * @param  [in, out]  ctx   ECIES context object.
 * @param  [in]       salt  Salt to use with KDF.
 * @param  [in]       len   Length of salt in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when ctx is NULL or salt is NULL and len is not 0.
 */
int wc_ecc_ctx_set_kdf_salt(ecEncCtx* ctx, const byte* salt, word32 len)
{
    if (ctx == NULL || (salt == NULL && len != 0))
        return BAD_FUNC_ARG;

    ctx->kdfSalt   = salt;
    ctx->kdfSaltSz = len;

    if (ctx->protocol == REQ_RESP_CLIENT) {
        ctx->cliSt = ecCLI_SALT_SET;
    }
    else if (ctx->protocol == REQ_RESP_SERVER) {
        ctx->srvSt = ecSRV_SALT_SET;
    }

    return 0;
}

static int ecc_ctx_set_salt(ecEncCtx* ctx, int flags)
{
    byte* saltBuffer = NULL;

    if (ctx == NULL || flags == 0)
        return BAD_FUNC_ARG;

    saltBuffer = (flags == REQ_RESP_CLIENT) ? ctx->clientSalt : ctx->serverSalt;

    return wc_RNG_GenerateBlock(ctx->rng, saltBuffer, EXCHANGE_SALT_SZ);
}


static void ecc_ctx_init(ecEncCtx* ctx, int flags, WC_RNG* rng)
{
    if (ctx) {
        XMEMSET(ctx, 0, sizeof(ecEncCtx));

    #if !defined(NO_AES) && defined(HAVE_AES_CBC)
        #ifdef WOLFSSL_AES_128
            ctx->encAlgo  = ecAES_128_CBC;
        #else
            ctx->encAlgo  = ecAES_256_CBC;
        #endif
    #elif !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
        #ifdef WOLFSSL_AES_256
            ctx->encAlgo  = ecAES_256_CTR;
        #else
            ctx->encAlgo  = ecAES_128_CTR;
        #endif
    #else
        #error "No valid encryption algorithm for ECIES configured."
    #endif
        ctx->kdfAlgo  = ecHKDF_SHA256;
        ctx->macAlgo  = ecHMAC_SHA256;
        ctx->protocol = (byte)flags;
        ctx->rng      = rng;

        if (flags == REQ_RESP_CLIENT)
            ctx->cliSt = ecCLI_INIT;
        if (flags == REQ_RESP_SERVER)
            ctx->srvSt = ecSRV_INIT;
    }
}


/* allow ecc context reset so user doesn't have to init/free for reuse */
WOLFSSL_ABI
int wc_ecc_ctx_reset(ecEncCtx* ctx, WC_RNG* rng)
{
    if (ctx == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    ecc_ctx_init(ctx, ctx->protocol, rng);
    return ecc_ctx_set_salt(ctx, ctx->protocol);
}


ecEncCtx* wc_ecc_ctx_new_ex(int flags, WC_RNG* rng, void* heap)
{
    int       ret = 0;
    ecEncCtx* ctx = (ecEncCtx*)XMALLOC(sizeof(ecEncCtx), heap,
                                                              DYNAMIC_TYPE_ECC);

    if (ctx) {
        ctx->protocol = (byte)flags;
        ctx->heap     = heap;
    }

    ret = wc_ecc_ctx_reset(ctx, rng);
    if (ret != 0) {
        wc_ecc_ctx_free(ctx);
        ctx = NULL;
    }

    return ctx;
}


/* alloc/init and set defaults, return new Context  */
WOLFSSL_ABI
ecEncCtx* wc_ecc_ctx_new(int flags, WC_RNG* rng)
{
    return wc_ecc_ctx_new_ex(flags, rng, NULL);
}


/* free any resources, clear any keys */
WOLFSSL_ABI
void wc_ecc_ctx_free(ecEncCtx* ctx)
{
    if (ctx) {
        void* heap = ctx->heap;
        ForceZero(ctx, sizeof(ecEncCtx));
        XFREE(ctx, heap, DYNAMIC_TYPE_ECC);
        (void)heap;
    }
}

static int ecc_get_key_sizes(ecEncCtx* ctx, int* encKeySz, int* ivSz,
                             int* keysLen, word32* digestSz, word32* blockSz)
{
    if (ctx) {
        switch (ctx->encAlgo) {
        #if !defined(NO_AES) && defined(HAVE_AES_CBC)
            case ecAES_128_CBC:
                *encKeySz = KEY_SIZE_128;
                *ivSz     = IV_SIZE_128;
                *blockSz  = AES_BLOCK_SIZE;
                break;
            case ecAES_256_CBC:
                *encKeySz = KEY_SIZE_256;
                *ivSz     = IV_SIZE_128;
                *blockSz  = AES_BLOCK_SIZE;
                break;
        #endif
        #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
            case ecAES_128_CTR:
                *encKeySz = KEY_SIZE_128;
                *ivSz     = 12;
                *blockSz  = 1;
                break;
            case ecAES_256_CTR:
                *encKeySz = KEY_SIZE_256;
                *ivSz     = 12;
                *blockSz  = 1;
                break;
        #endif
            default:
                return BAD_FUNC_ARG;
        }

        switch (ctx->macAlgo) {
            case ecHMAC_SHA256:
                *digestSz = WC_SHA256_DIGEST_SIZE;
                break;
            default:
                return BAD_FUNC_ARG;
        }
    } else
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_ECIES_OLD
    *keysLen  = *encKeySz + *ivSz + (int)*digestSz;
#else
    *keysLen  = *encKeySz + (int)*digestSz;
#endif

    return 0;
}


/* ecc encrypt with shared secret run through kdf
   ctx holds non default algos and inputs
   msgSz should be the right size for encAlgo, i.e., already padded
   return 0 on success */
int wc_ecc_encrypt_ex(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
    word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx, int compressed)
{
    int          ret = 0;
    word32       blockSz = 0;
#ifndef WOLFSSL_ECIES_OLD
#ifndef WOLFSSL_ECIES_GEN_IV
    byte         iv[ECC_MAX_IV_SIZE];
#endif
    word32       pubKeySz = 0;
#endif
    word32       digestSz = 0;
    ecEncCtx     localCtx;
#ifdef WOLFSSL_SMALL_STACK
    byte*        sharedSecret;
    byte*        keys;
#else
#if defined(WOLFSSL_ECIES_OLD) || !defined(WOLFSSL_ECIES_ISO18033)
    byte         sharedSecret[ECC_MAXSIZE];  /* 521 max size */
#else
    byte         sharedSecret[ECC_MAXSIZE * 3 + 1]; /* Public key too */
#endif
    byte         keys[ECC_BUFSIZE];         /* max size */
#endif
#if defined(WOLFSSL_ECIES_OLD) || !defined(WOLFSSL_ECIES_ISO18033)
    word32       sharedSz = ECC_MAXSIZE;
#else
    /* 'Uncompressed' byte | public key x | public key y | secret */
    word32       sharedSz = 1 + ECC_MAXSIZE * 3;
#endif
    int          keysLen = 0;
    int          encKeySz = 0;
    int          ivSz = 0;
    int          offset = 0;         /* keys offset if doing msg exchange */
    byte*        encKey = NULL;
    byte*        encIv = NULL;
    byte*        macKey = NULL;

    if (privKey == NULL || pubKey == NULL || msg == NULL || out == NULL ||
                           outSz  == NULL)
        return BAD_FUNC_ARG;

    if (ctx == NULL) {  /* use defaults */
        ecc_ctx_init(&localCtx, 0, NULL);
        ctx = &localCtx;
    }

    ret = ecc_get_key_sizes(ctx, &encKeySz, &ivSz, &keysLen, &digestSz,
                            &blockSz);
    if (ret != 0)
        return ret;

#ifndef WOLFSSL_ECIES_OLD
    if (!compressed) {
        pubKeySz = 1 + (word32)wc_ecc_size(privKey) * 2;
    }
    else {
        pubKeySz = 1 + (word32)wc_ecc_size(privKey);
    }
#else
    (void) compressed; /* avoid unused parameter if WOLFSSL_ECIES_OLD is defined */
#endif

    if (ctx->protocol == REQ_RESP_SERVER) {
        offset = keysLen;
        keysLen *= 2;

        if (ctx->srvSt != ecSRV_RECV_REQ)
            return BAD_STATE_E;

        ctx->srvSt = ecSRV_BAD_STATE; /* we're done no more ops allowed */
    }
    else if (ctx->protocol == REQ_RESP_CLIENT) {
        if (ctx->cliSt != ecCLI_SALT_SET)
            return BAD_STATE_E;

        ctx->cliSt = ecCLI_SENT_REQ; /* only do this once */
    }

    if (keysLen > ECC_BUFSIZE) /* keys size */
        return BUFFER_E;

    if ((msgSz % blockSz) != 0)
        return BAD_PADDING_E;

#ifdef WOLFSSL_ECIES_OLD
    if (*outSz < (msgSz + digestSz))
        return BUFFER_E;
#elif defined(WOLFSSL_ECIES_GEN_IV)
    if (*outSz < (pubKeySz + ivSz + msgSz + digestSz))
        return BUFFER_E;
#else
    if (*outSz < (pubKeySz + msgSz + digestSz))
        return BUFFER_E;
#endif

#ifdef ECC_TIMING_RESISTANT
    if (ctx->rng != NULL && privKey->rng == NULL)
        privKey->rng = ctx->rng;
#endif

#ifndef WOLFSSL_ECIES_OLD
    if (privKey->type == ECC_PRIVATEKEY_ONLY) {
#ifdef ECC_TIMING_RESISTANT
        ret = wc_ecc_make_pub_ex(privKey, NULL, privKey->rng);
#else
        ret = wc_ecc_make_pub_ex(privKey, NULL, NULL);
#endif
        if (ret != 0)
            return ret;
    }
    ret = wc_ecc_export_x963_ex(privKey, out, &pubKeySz, compressed);
    if (ret != 0)
        return ret;
    out += pubKeySz;
#endif

#ifdef WOLFSSL_SMALL_STACK
    sharedSecret = (byte*)XMALLOC(sharedSz, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (sharedSecret == NULL)
        return MEMORY_E;

    keys = (byte*)XMALLOC(ECC_BUFSIZE, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (keys == NULL) {
        XFREE(sharedSecret, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
        return MEMORY_E;
    }
#endif

    SAVE_VECTOR_REGISTERS(ret = _svr_ret;);

#ifdef WOLFSSL_ECIES_ISO18033
    XMEMCPY(sharedSecret, out - pubKeySz, pubKeySz);
    sharedSz -= pubKeySz;
#endif

    do {
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
        ret = wc_AsyncWait(ret, &privKey->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
        if (ret != 0)
            break;
    #endif
    #ifndef WOLFSSL_ECIES_ISO18033
        ret = wc_ecc_shared_secret(privKey, pubKey, sharedSecret, &sharedSz);
    #else
        ret = wc_ecc_shared_secret(privKey, pubKey, sharedSecret + pubKeySz,
                                                                     &sharedSz);
    #endif
    } while (ret == WC_PENDING_E);
    if (ret == 0) {
    #ifdef WOLFSSL_ECIES_ISO18033
        /* KDF data is encoded public key and secret. */
        sharedSz += pubKeySz;
    #endif
        switch (ctx->kdfAlgo) {
            case ecHKDF_SHA256 :
                ret = wc_HKDF(WC_SHA256, sharedSecret, sharedSz, ctx->kdfSalt,
                           ctx->kdfSaltSz, ctx->kdfInfo, ctx->kdfInfoSz,
                           keys, (word32)keysLen);
                break;

            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (ret == 0) {
    #ifdef WOLFSSL_ECIES_OLD
        encKey = keys + offset;
        encIv  = encKey + encKeySz;
        macKey = encKey + encKeySz + ivSz;
    #elif defined(WOLFSSL_ECIES_GEN_IV)
        encKey = keys + offset;
        encIv  = out;
        out += ivSz;
        macKey = encKey + encKeySz;
        ret = wc_RNG_GenerateBlock(privKey->rng, encIv, ivSz);
    #else
        XMEMSET(iv, 0, (size_t)ivSz);
        encKey = keys + offset;
        encIv  = iv;
        macKey = encKey + encKeySz;
    #endif
    }

    if (ret == 0) {
       switch (ctx->encAlgo) {
            case ecAES_128_CBC:
            case ecAES_256_CBC:
            {
        #if !defined(NO_AES) && defined(HAVE_AES_CBC)
            #ifdef WOLFSSL_SMALL_STACK
                Aes *aes = (Aes *)XMALLOC(sizeof *aes, ctx->heap,
                                          DYNAMIC_TYPE_AES);
                if (aes == NULL) {
                    ret = MEMORY_E;
                    break;
                }
            #else
                Aes aes[1];
            #endif
                ret = wc_AesInit(aes, NULL, INVALID_DEVID);
                if (ret == 0) {
                    ret = wc_AesSetKey(aes, encKey, (word32)encKeySz, encIv,
                                                                AES_ENCRYPTION);
                    if (ret == 0) {
                        ret = wc_AesCbcEncrypt(aes, out, msg, msgSz);
                    #if defined(WOLFSSL_ASYNC_CRYPT) && \
                                                    defined(WC_ASYNC_ENABLE_AES)
                        ret = wc_AsyncWait(ret, &aes->asyncDev,
                                            WC_ASYNC_FLAG_NONE);
                    #endif
                    }
                    wc_AesFree(aes);
                }
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(aes, ctx->heap, DYNAMIC_TYPE_AES);
            #endif
        #else
                ret = NOT_COMPILED_IN;
        #endif
                break;
            }
            case ecAES_128_CTR:
            case ecAES_256_CTR:
            {
        #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
                byte ctr_iv[AES_BLOCK_SIZE];
            #ifndef WOLFSSL_SMALL_STACK
                Aes aes[1];
            #else
                Aes *aes = (Aes *)XMALLOC(sizeof *aes, ctx->heap,
                                            DYNAMIC_TYPE_AES);
                if (aes == NULL) {
                    ret = MEMORY_E;
                    break;
                }
            #endif

                /* Include 4 byte counter starting at all zeros. */
                XMEMCPY(ctr_iv, encIv, WOLFSSL_ECIES_GEN_IV_SIZE);
                XMEMSET(ctr_iv + WOLFSSL_ECIES_GEN_IV_SIZE, 0,
                    AES_BLOCK_SIZE - WOLFSSL_ECIES_GEN_IV_SIZE);

                ret = wc_AesInit(aes, NULL, INVALID_DEVID);
                if (ret == 0) {
                    ret = wc_AesSetKey(aes, encKey, (word32)encKeySz, ctr_iv,
                                                                AES_ENCRYPTION);
                    if (ret == 0) {
                        ret = wc_AesCtrEncrypt(aes, out, msg, msgSz);
                    #if defined(WOLFSSL_ASYNC_CRYPT) && \
                                                    defined(WC_ASYNC_ENABLE_AES)
                        ret = wc_AsyncWait(ret, &aes->asyncDev,
                                            WC_ASYNC_FLAG_NONE);
                    #endif
                    }
                    wc_AesFree(aes);
                }
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(aes, ctx->heap, DYNAMIC_TYPE_AES);
            #endif
        #else
                ret = NOT_COMPILED_IN;
        #endif
                break;
            }
            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (ret == 0) {
        switch (ctx->macAlgo) {
            case ecHMAC_SHA256:
            {
            #ifdef WOLFSSL_SMALL_STACK
                Hmac *hmac = (Hmac *)XMALLOC(sizeof *hmac, ctx->heap,
                                             DYNAMIC_TYPE_HMAC);
                if (hmac == NULL) {
                    ret = MEMORY_E;
                    break;
                }
            #else
                Hmac hmac[1];
            #endif
                ret = wc_HmacInit(hmac, NULL, INVALID_DEVID);
                if (ret == 0) {
                    ret = wc_HmacSetKey(hmac, WC_SHA256, macKey,
                                                         WC_SHA256_DIGEST_SIZE);
                    if (ret == 0) {
                    #if !defined(WOLFSSL_ECIES_GEN_IV)
                        ret = wc_HmacUpdate(hmac, out, msgSz);
                    #else
                        /* IV is before encrypted message. */
                        ret = wc_HmacUpdate(hmac, encIv, ivSz + msgSz);
                    #endif
                    }
                    if (ret == 0)
                        ret = wc_HmacUpdate(hmac, ctx->macSalt, ctx->macSaltSz);
                    if (ret == 0)
                        ret = wc_HmacFinal(hmac, out+msgSz);
                    wc_HmacFree(hmac);
                }
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(hmac, ctx->heap, DYNAMIC_TYPE_HMAC);
            #endif
                break;
            }

            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (ret == 0) {
#ifdef WOLFSSL_ECIES_OLD
        *outSz = msgSz + digestSz;
#elif defined(WOLFSSL_ECIES_GEN_IV)
        *outSz = pubKeySz + ivSz + msgSz + digestSz;
#else
        *outSz = pubKeySz + msgSz + digestSz;
#endif
    }

    RESTORE_VECTOR_REGISTERS();

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sharedSecret, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    XFREE(keys, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return ret;
}

/* ecc encrypt with shared secret run through kdf
   ctx holds non default algos and inputs
   msgSz should be the right size for encAlgo, i.e., already padded
   return 0 on success */
WOLFSSL_ABI
int wc_ecc_encrypt(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
                word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx)
{
    return wc_ecc_encrypt_ex(privKey, pubKey, msg, msgSz, out, outSz, ctx, 0);
}

/* ecc decrypt with shared secret run through kdf
   ctx holds non default algos and inputs
   return 0 on success */
WOLFSSL_ABI
int wc_ecc_decrypt(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
                word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx)
{
    int          ret = 0;
    word32       blockSz = 0;
#ifndef WOLFSSL_ECIES_OLD
#ifndef WOLFSSL_ECIES_GEN_IV
    byte         iv[ECC_MAX_IV_SIZE];
#endif
    word32       pubKeySz = 0;
#ifdef WOLFSSL_SMALL_STACK
    ecc_key*     peerKey = NULL;
#else
    ecc_key      peerKey[1];
#endif
#endif
    word32       digestSz = 0;
    ecEncCtx     localCtx;
#ifdef WOLFSSL_SMALL_STACK
    byte*        sharedSecret;
    byte*        keys;
#else
#if defined(WOLFSSL_ECIES_OLD) || !defined(WOLFSSL_ECIES_ISO18033)
    byte         sharedSecret[ECC_MAXSIZE];  /* 521 max size */
#else
    byte         sharedSecret[ECC_MAXSIZE * 3 + 1]; /* Public key too */
#endif
    byte         keys[ECC_BUFSIZE];         /* max size */
#endif
#if defined(WOLFSSL_ECIES_OLD) || !defined(WOLFSSL_ECIES_ISO18033)
    word32       sharedSz = ECC_MAXSIZE;
#else
    word32       sharedSz = ECC_MAXSIZE * 3 + 1;
#endif
    int          keysLen = 0;
    int          encKeySz = 0;
    int          ivSz = 0;
    int          offset = 0;       /* in case using msg exchange */
    byte*        encKey = NULL;
    const byte*  encIv = NULL;
    byte*        macKey = NULL;


    if (privKey == NULL || msg == NULL || out == NULL || outSz  == NULL)
        return BAD_FUNC_ARG;
#ifdef WOLFSSL_ECIES_OLD
    if (pubKey == NULL)
        return BAD_FUNC_ARG;
#endif

    if (ctx == NULL) {  /* use defaults */
        ecc_ctx_init(&localCtx, 0, NULL);
        ctx = &localCtx;
    }

    ret = ecc_get_key_sizes(ctx, &encKeySz, &ivSz, &keysLen, &digestSz,
                            &blockSz);
    if (ret != 0)
        return ret;

#ifndef WOLFSSL_ECIES_OLD
    ret = ecc_public_key_size(privKey, &pubKeySz);
    if (ret != 0)
        return ret;
#ifdef HAVE_COMP_KEY
    if ((msgSz > 1) && ((msg[0] == 0x02) || (msg[0] == 0x03))) {
        pubKeySz = (pubKeySz / 2) + 1;
    }
#endif /* HAVE_COMP_KEY */
#endif /* WOLFSSL_ECIES_OLD */

    if (ctx->protocol == REQ_RESP_CLIENT) {
        offset = keysLen;
        keysLen *= 2;

        if (ctx->cliSt != ecCLI_SENT_REQ)
            return BAD_STATE_E;

        ctx->cliSt = ecSRV_BAD_STATE; /* we're done no more ops allowed */
    }
    else if (ctx->protocol == REQ_RESP_SERVER) {
        if (ctx->srvSt != ecSRV_SALT_SET)
            return BAD_STATE_E;

        ctx->srvSt = ecSRV_RECV_REQ; /* only do this once */
    }

    if (keysLen > ECC_BUFSIZE) /* keys size */
        return BUFFER_E;

#ifdef WOLFSSL_ECIES_OLD
    if (((msgSz - digestSz) % blockSz) != 0)
        return BAD_PADDING_E;

    if (*outSz < (msgSz - digestSz))
        return BUFFER_E;
#elif defined(WOLFSSL_ECIES_GEN_IV)
    if (((msgSz - ivSz - digestSz - pubKeySz) % blockSz) != 0)
        return BAD_PADDING_E;

    if (msgSz < pubKeySz + ivSz + blockSz + digestSz)
        return BAD_FUNC_ARG;
    if (*outSz < (msgSz - ivSz - digestSz - pubKeySz))
        return BUFFER_E;
#else
    if (((msgSz - digestSz - pubKeySz) % blockSz) != 0)
        return BAD_PADDING_E;

    if (msgSz < pubKeySz + blockSz + digestSz)
        return BAD_FUNC_ARG;
    if (*outSz < (msgSz - digestSz - pubKeySz))
        return BUFFER_E;
#endif

#ifdef ECC_TIMING_RESISTANT
    if (ctx->rng != NULL && privKey->rng == NULL)
        privKey->rng = ctx->rng;
#endif

#ifdef WOLFSSL_SMALL_STACK
    sharedSecret = (byte*)XMALLOC(sharedSz, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (sharedSecret == NULL) {
    #ifndef WOLFSSL_ECIES_OLD
        if (pubKey == peerKey)
            wc_ecc_free(peerKey);
    #endif
        return MEMORY_E;
    }

    keys = (byte*)XMALLOC(ECC_BUFSIZE, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    if (keys == NULL) {
        XFREE(sharedSecret, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    #ifndef WOLFSSL_ECIES_OLD
        if (pubKey == peerKey)
            wc_ecc_free(peerKey);
    #endif
        return MEMORY_E;
    }
#endif

    SAVE_VECTOR_REGISTERS(ret = _svr_ret;);

#ifndef WOLFSSL_ECIES_OLD
    if (pubKey == NULL) {
#ifdef WOLFSSL_SMALL_STACK
        peerKey = (ecc_key*)XMALLOC(sizeof(*peerKey), ctx->heap,
                                                       DYNAMIC_TYPE_ECC_BUFFER);
        if (peerKey == NULL)
            ret = MEMORY_E;
#endif
        pubKey = peerKey;
    }
    else {
        /* if a public key was passed in we should free it here before init
         * and import */
        wc_ecc_free(pubKey);
    }
    if (ret == 0) {
        ret = wc_ecc_init_ex(pubKey, privKey->heap, INVALID_DEVID);
    }
    if (ret == 0) {
        ret = wc_ecc_import_x963_ex(msg, pubKeySz, pubKey, privKey->dp->id);
    }
    if (ret == 0) {
        /* Point is not MACed. */
        msg += pubKeySz;
        msgSz -= pubKeySz;
    }
#endif

    if (ret == 0) {
    #ifdef WOLFSSL_ECIES_ISO18033
        XMEMCPY(sharedSecret, msg - pubKeySz, pubKeySz);
        sharedSz -= pubKeySz;
    #endif

        do {
        #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ECC)
            ret = wc_AsyncWait(ret, &privKey->asyncDev,
                                                     WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif
        #ifndef WOLFSSL_ECIES_ISO18033
            ret = wc_ecc_shared_secret(privKey, pubKey, sharedSecret,
                                                                    &sharedSz);
        #else
            ret = wc_ecc_shared_secret(privKey, pubKey, sharedSecret +
                                                          pubKeySz, &sharedSz);
        #endif
        } while (ret == WC_PENDING_E);
    }
    if (ret == 0) {
    #ifdef WOLFSSL_ECIES_ISO18033
        /* KDF data is encoded public key and secret. */
        sharedSz += pubKeySz;
    #endif
        switch (ctx->kdfAlgo) {
            case ecHKDF_SHA256 :
                ret = wc_HKDF(WC_SHA256, sharedSecret, sharedSz, ctx->kdfSalt,
                           ctx->kdfSaltSz, ctx->kdfInfo, ctx->kdfInfoSz,
                           keys, (word32)keysLen);
                break;

            default:
                ret = BAD_FUNC_ARG;
                break;
         }
    }

    if (ret == 0) {
    #ifdef WOLFSSL_ECIES_OLD
        encKey = keys + offset;
        encIv  = encKey + encKeySz;
        macKey = encKey + encKeySz + ivSz;
    #elif defined(WOLFSSL_ECIES_GEN_IV)
        encKey = keys + offset;
        encIv  = msg;
        msg   += ivSz;
        msgSz -= ivSz;
        macKey = encKey + encKeySz;
    #else
        XMEMSET(iv, 0, (size_t)ivSz);
        encKey = keys + offset;
        encIv  = iv;
        macKey = encKey + encKeySz;
    #endif

        switch (ctx->macAlgo) {
            case ecHMAC_SHA256:
            {
                byte verify[WC_SHA256_DIGEST_SIZE];
            #ifdef WOLFSSL_SMALL_STACK
                Hmac *hmac = (Hmac *)XMALLOC(sizeof *hmac, ctx->heap,
                                             DYNAMIC_TYPE_HMAC);
                if (hmac == NULL) {
                    ret = MEMORY_E;
                    break;
                }
            #else
                Hmac hmac[1];
            #endif
                ret = wc_HmacInit(hmac, NULL, INVALID_DEVID);
                if (ret == 0) {
                    ret = wc_HmacSetKey(hmac, WC_SHA256, macKey,
                                                         WC_SHA256_DIGEST_SIZE);
                    if (ret == 0)
                    #if !defined(WOLFSSL_ECIES_GEN_IV)
                        ret = wc_HmacUpdate(hmac, msg, msgSz-digestSz);
                    #else
                        /* IV is before encrypted message. */
                        ret = wc_HmacUpdate(hmac, encIv, ivSz+msgSz-digestSz);
                    #endif
                    if (ret == 0)
                        ret = wc_HmacUpdate(hmac, ctx->macSalt, ctx->macSaltSz);

                    if (ret == 0)
                        ret = wc_HmacFinal(hmac, verify);
                    if ((ret == 0) && (XMEMCMP(verify, msg + msgSz - digestSz,
                                                              digestSz) != 0)) {
                        ret = -1;
                    }

                    wc_HmacFree(hmac);
                }
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(hmac, ctx->heap, DYNAMIC_TYPE_HMAC);
            #endif
                break;
            }

            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (ret == 0) {
        switch (ctx->encAlgo) {
        #if !defined(NO_AES) && defined(HAVE_AES_CBC)
            case ecAES_128_CBC:
            case ecAES_256_CBC:
            {
            #ifdef WOLFSSL_SMALL_STACK
                Aes *aes = (Aes *)XMALLOC(sizeof *aes, ctx->heap,
                                          DYNAMIC_TYPE_AES);
                if (aes == NULL) {
                    ret = MEMORY_E;
                    break;
                }
            #else
                Aes aes[1];
            #endif
                ret = wc_AesInit(aes, NULL, INVALID_DEVID);
                if (ret == 0) {
                    ret = wc_AesSetKey(aes, encKey, (word32)encKeySz, encIv,
                                                                AES_DECRYPTION);
                    if (ret == 0) {
                        ret = wc_AesCbcDecrypt(aes, out, msg, msgSz-digestSz);
                    #if defined(WOLFSSL_ASYNC_CRYPT) && \
                                                    defined(WC_ASYNC_ENABLE_AES)
                        ret = wc_AsyncWait(ret, &aes->asyncDev,
                                                            WC_ASYNC_FLAG_NONE);
                    #endif
                    }
                    wc_AesFree(aes);
                }
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(aes, ctx->heap, DYNAMIC_TYPE_AES);
            #endif
                break;
            }
        #endif
        #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
            case ecAES_128_CTR:
            case ecAES_256_CTR:
            {
            #ifdef WOLFSSL_SMALL_STACK
                Aes *aes = (Aes *)XMALLOC(sizeof *aes, ctx->heap,
                                          DYNAMIC_TYPE_AES);
                if (aes == NULL) {
                    ret = MEMORY_E;
                    break;
                }
             #else
                Aes aes[1];
             #endif
                ret = wc_AesInit(aes, NULL, INVALID_DEVID);
                if (ret == 0) {
                    byte ctr_iv[AES_BLOCK_SIZE];
                    /* Make a 16 byte IV from the bytes passed in. */
                    XMEMCPY(ctr_iv, encIv, WOLFSSL_ECIES_GEN_IV_SIZE);
                    XMEMSET(ctr_iv + WOLFSSL_ECIES_GEN_IV_SIZE, 0,
                        AES_BLOCK_SIZE - WOLFSSL_ECIES_GEN_IV_SIZE);
                    ret = wc_AesSetKey(aes, encKey, (word32)encKeySz, ctr_iv,
                                                                AES_ENCRYPTION);
                    if (ret == 0) {
                        ret = wc_AesCtrEncrypt(aes, out, msg, msgSz-digestSz);
                    #if defined(WOLFSSL_ASYNC_CRYPT) && \
                                                    defined(WC_ASYNC_ENABLE_AES)
                        ret = wc_AsyncWait(ret, &aes->asyncDev,
                                                            WC_ASYNC_FLAG_NONE);
                    #endif
                    }
                    wc_AesFree(aes);
                }
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(aes, ctx->heap, DYNAMIC_TYPE_AES);
            #endif
                break;
            }
        #endif
            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (ret == 0)
       *outSz = msgSz - digestSz;

    RESTORE_VECTOR_REGISTERS();

#ifndef WOLFSSL_ECIES_OLD
    if (pubKey == peerKey)
        wc_ecc_free(peerKey);
#endif
#ifdef WOLFSSL_SMALL_STACK
#ifndef WOLFSSL_ECIES_OLD
    if (peerKey != NULL) {
        XFREE(peerKey, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    }
#endif
    XFREE(sharedSecret, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
    XFREE(keys, ctx->heap, DYNAMIC_TYPE_ECC_BUFFER);
#endif

    return ret;
}


#endif /* HAVE_ECC_ENCRYPT */


#ifdef HAVE_COMP_KEY
#if !defined(WOLFSSL_ATECC508A) && !defined(WOLFSSL_ATECC608A) && \
    !defined(WOLFSSL_CRYPTOCELL)

#ifndef WOLFSSL_SP_MATH
/* computes the jacobi c = (a | n) (or Legendre if n is prime)
 */
int mp_jacobi(mp_int* a, mp_int* n, int* c)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int*  a1 = NULL;
    mp_int*  n1 = NULL;
#else
    mp_int   a1[1], n1[1];
#endif
    int      res;
    int      s = 1;
    int      k;
    mp_int*  t[2];
    mp_int*  ts;
    mp_digit residue;

    if (mp_isneg(a) == MP_YES) {
        return MP_VAL;
    }
    if (mp_isneg(n) == MP_YES) {
        return MP_VAL;
    }
    if (mp_iseven(n) == MP_YES) {
        return MP_VAL;
    }

#ifdef WOLFSSL_SMALL_STACK
    a1 = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (a1 == NULL) {
        return MP_MEM;
    }
    n1 = (mp_int*)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (n1 == NULL) {
        XFREE(a1, NULL, DYNAMIC_TYPE_BIGINT);
        return MP_MEM;
    }
#endif

    if ((res = mp_init_multi(a1, n1, NULL, NULL, NULL, NULL)) != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(a1, NULL, DYNAMIC_TYPE_BIGINT);
        XFREE(n1, NULL, DYNAMIC_TYPE_BIGINT);
#endif
        return res;
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    if ((res = mp_mod(a, n, a1)) != MP_OKAY) {
        goto done;
    }

    if ((res = mp_copy(n, n1)) != MP_OKAY) {
        goto done;
    }

    t[0] = a1;
    t[1] = n1;

    /* Keep reducing until first number is 0. */
    while (!mp_iszero(t[0])) {
        /* Divide by 2 until odd. */
        k = mp_cnt_lsb(t[0]);
        if (k > 0) {
            mp_rshb(t[0], k);

            /* Negate s each time we divide by 2 if t[1] mod 8 == 3 or 5.
             * Odd number of divides results in a negate.
             */
            residue = t[1]->dp[0] & 7;
            if ((k & 1) && ((residue == 3) || (residue == 5))) {
                s = -s;
            }
        }

        /* Swap t[0] and t[1]. */
        ts   = t[0];
        t[0] = t[1];
        t[1] = ts;

        /* Negate s if both numbers == 3 mod 4. */
        if (((t[0]->dp[0] & 3) == 3) && ((t[1]->dp[0] & 3) == 3)) {
             s = -s;
        }

        /* Reduce first number modulo second. */
        if ((k == 0) && (mp_count_bits(t[0]) == mp_count_bits(t[1]))) {
            res = mp_sub(t[0], t[1], t[0]);
        }
        else {
            res = mp_mod(t[0], t[1], t[0]);
        }
        if (res != MP_OKAY) {
            goto done;
        }
    }

    /* When the two numbers have divisors in common. */
    if (!mp_isone(t[1])) {
        s = 0;
    }
    *c = s;

done:

    RESTORE_VECTOR_REGISTERS();

    /* cleanup */
    mp_clear(n1);
    mp_clear(a1);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(a1, NULL, DYNAMIC_TYPE_BIGINT);
    XFREE(n1, NULL, DYNAMIC_TYPE_BIGINT);
#endif

  return res;
}


/* Solves the modular equation x^2 = n (mod p)
 * where prime number is greater than 2 (odd prime).
 * The result is returned in the third argument x
 * the function returns MP_OKAY on success, MP_VAL or another error on failure
 */
int mp_sqrtmod_prime(mp_int* n, mp_int* prime, mp_int* ret)
{
#ifdef SQRTMOD_USE_MOD_EXP
  int res;

  mp_int e;

  SAVE_VECTOR_REGISTERS(return _svr_ret;);

  res = mp_init(&e);
  if (res == MP_OKAY)
      res = mp_add_d(prime, 1, &e);
  if (res == MP_OKAY)
      res = mp_div_2d(&e, 2, &e, NULL);
  if (res == MP_OKAY)
      res = mp_exptmod(n, &e, prime, ret);

  mp_clear(&e);

  RESTORE_VECTOR_REGISTERS();

  return res;
#else
  int res, legendre, done = 0;
  mp_digit i;
#ifdef WOLFSSL_SMALL_STACK
  mp_int *t1 = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *C = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *Q = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *S = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *Z = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *M = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *T = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *R = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *N = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
  mp_int *two = (mp_int *)XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_ECC_BUFFER);
#else
  mp_int t1[1], C[1], Q[1], S[1], Z[1], M[1], T[1], R[1], N[1], two[1];
#endif

  SAVE_VECTOR_REGISTERS(res = _svr_ret; goto out;);

  if ((mp_init_multi(t1, C, Q, S, Z, M) != MP_OKAY) ||
      (mp_init_multi(T, R, N, two, NULL, NULL) != MP_OKAY)) {
    res = MP_INIT_E;
    goto out;
  }

#ifdef WOLFSSL_SMALL_STACK
  if ((t1 == NULL) ||
      (C == NULL) ||
      (Q == NULL) ||
      (S == NULL) ||
      (Z == NULL) ||
      (M == NULL) ||
      (T == NULL) ||
      (R == NULL) ||
      (N == NULL) ||
      (two == NULL)) {
    res = MP_MEM;
    goto out;
  }
#endif

  /* first handle the simple cases n = 0 or n = 1 */
  if (mp_cmp_d(n, 0) == MP_EQ) {
    mp_zero(ret);
    res = MP_OKAY;
    goto out;
  }
  if (mp_cmp_d(n, 1) == MP_EQ) {
    res = mp_set(ret, 1);
    goto out;
  }

  /* prime must be odd */
  if (mp_cmp_d(prime, 2) == MP_EQ) {
    res = MP_VAL;
    goto out;
  }

  /* reduce n to less than prime */
  res = mp_mod(n, prime, N);
  if (res != MP_OKAY) {
    goto out;
  }
  /* when N is zero, sqrt is zero */
  if (mp_iszero(N)) {
    mp_set(ret, 0);
    goto out;
  }

  /* is quadratic non-residue mod prime */
  if ((res = mp_jacobi(N, prime, &legendre)) != MP_OKAY) {
    goto out;
  }
  if (legendre == -1) {
    res = MP_VAL;
    goto out;
  }

  /* SPECIAL CASE: if prime mod 4 == 3
   * compute directly: res = n^(prime+1)/4 mod prime
   * Handbook of Applied Cryptography algorithm 3.36
   */
  res = mp_mod_d(prime, 4, &i);
  if (res == MP_OKAY && i == 3) {
    res = mp_add_d(prime, 1, t1);

    if (res == MP_OKAY)
      res = mp_div_2(t1, t1);
    if (res == MP_OKAY)
      res = mp_div_2(t1, t1);
    if (res == MP_OKAY)
      res = mp_exptmod(N, t1, prime, ret);

    done = 1;
  }

  /* NOW: TonelliShanks algorithm */
  if (res == MP_OKAY && done == 0) {

    /* factor out powers of 2 from prime-1, defining Q and S
    *                                      as: prime-1 = Q*2^S */
    /* Q = prime - 1 */
    res = mp_copy(prime, Q);
    if (res == MP_OKAY)
      res = mp_sub_d(Q, 1, Q);

    /* S = 0 */
    if (res == MP_OKAY)
      mp_zero(S);

    while (res == MP_OKAY && mp_iseven(Q) == MP_YES) {
      /* Q = Q / 2 */
      res = mp_div_2(Q, Q);

      /* S = S + 1 */
      if (res == MP_OKAY)
        res = mp_add_d(S, 1, S);
    }

    /* find a Z such that the Legendre symbol (Z|prime) == -1 */
    /* Z = 2 */
    if (res == MP_OKAY)
      res = mp_set_int(Z, 2);

    while (res == MP_OKAY) {
      res = mp_jacobi(Z, prime, &legendre);
      if (res == MP_OKAY && legendre == -1)
        break;

      /* Z = Z + 1 */
      if (res == MP_OKAY)
        res = mp_add_d(Z, 1, Z);
    }

    /* C = Z ^ Q mod prime */
    if (res == MP_OKAY)
      res = mp_exptmod(Z, Q, prime, C);

    /* t1 = (Q + 1) / 2 */
    if (res == MP_OKAY)
      res = mp_add_d(Q, 1, t1);
    if (res == MP_OKAY)
      res = mp_div_2(t1, t1);

    /* R = n ^ ((Q + 1) / 2) mod prime */
    if (res == MP_OKAY)
      res = mp_exptmod(N, t1, prime, R);

    /* T = n ^ Q mod prime */
    if (res == MP_OKAY)
      res = mp_exptmod(N, Q, prime, T);

    /* M = S */
    if (res == MP_OKAY)
      res = mp_copy(S, M);

    if (res == MP_OKAY)
      res = mp_set_int(two, 2);

    while (res == MP_OKAY && done == 0) {
      res = mp_copy(T, t1);

      /* reduce to 1 and count */
      i = 0;
      while (res == MP_OKAY) {
        if (mp_cmp_d(t1, 1) == MP_EQ)
            break;
        res = mp_exptmod(t1, two, prime, t1);
        if (res == MP_OKAY)
          i++;
      }
      if (res == MP_OKAY && i == 0) {
        res = mp_copy(R, ret);
        done = 1;
      }

      if (done == 0) {
        /* t1 = 2 ^ (M - i - 1) */
        if (res == MP_OKAY)
          res = mp_sub_d(M, i, t1);
        if (res == MP_OKAY)
          res = mp_sub_d(t1, 1, t1);
        if (res == MP_OKAY)
          res = mp_exptmod(two, t1, prime, t1);

        /* t1 = C ^ (2 ^ (M - i - 1)) mod prime */
        if (res == MP_OKAY)
          res = mp_exptmod(C, t1, prime, t1);

        /* C = (t1 * t1) mod prime */
        if (res == MP_OKAY)
          res = mp_sqrmod(t1, prime, C);

        /* R = (R * t1) mod prime */
        if (res == MP_OKAY)
          res = mp_mulmod(R, t1, prime, R);

        /* T = (T * C) mod prime */
        if (res == MP_OKAY)
          res = mp_mulmod(T, C, prime, T);

        /* M = i */
        if (res == MP_OKAY)
          res = mp_set(M, i);
      }
    }
  }

  out:

  RESTORE_VECTOR_REGISTERS();

#ifdef WOLFSSL_SMALL_STACK
  if (t1) {
    if (res != MP_INIT_E)
      mp_clear(t1);
    XFREE(t1, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (C) {
    if (res != MP_INIT_E)
      mp_clear(C);
    XFREE(C, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (Q) {
    if (res != MP_INIT_E)
      mp_clear(Q);
    XFREE(Q, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (S) {
    if (res != MP_INIT_E)
      mp_clear(S);
    XFREE(S, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (Z) {
    if (res != MP_INIT_E)
      mp_clear(Z);
    XFREE(Z, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (M) {
    if (res != MP_INIT_E)
      mp_clear(M);
    XFREE(M, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (T) {
    if (res != MP_INIT_E)
      mp_clear(T);
    XFREE(T, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (R) {
    if (res != MP_INIT_E)
      mp_clear(R);
    XFREE(R, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (N) {
    if (res != MP_INIT_E)
      mp_clear(N);
    XFREE(N, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
  if (two) {
    if (res != MP_INIT_E)
      mp_clear(two);
    XFREE(two, NULL, DYNAMIC_TYPE_ECC_BUFFER);
  }
#else
  if (res != MP_INIT_E) {
    mp_clear(t1);
    mp_clear(C);
    mp_clear(Q);
    mp_clear(S);
    mp_clear(Z);
    mp_clear(M);
    mp_clear(T);
    mp_clear(R);
    mp_clear(N);
    mp_clear(two);
  }
#endif

  return res;
#endif
}
#endif /* !WOLFSSL_SP_MATH */
#endif /* !WOLFSSL_ATECC508A && !WOLFSSL_ATECC608A && !WOLFSSL_CRYPTOCELL */

#ifdef HAVE_ECC_KEY_EXPORT
/* export public ECC key in ANSI X9.63 format compressed */
static int wc_ecc_export_x963_compressed(ecc_key* key, byte* out, word32* outLen)
{
   word32 numlen;
   int    ret = MP_OKAY;

   if (key == NULL || outLen == NULL)
       return BAD_FUNC_ARG;

   if (key->type == ECC_PRIVATEKEY_ONLY)
       return ECC_PRIVATEONLY_E;

   if (key->type == 0 || wc_ecc_is_valid_idx(key->idx) == 0 || key->dp == NULL){
       return ECC_BAD_ARG_E;
   }

   numlen = (word32)key->dp->size;

   if (*outLen < (1 + numlen)) {
      *outLen = 1 + numlen;
      return LENGTH_ONLY_E;
   }

   if (out == NULL)
       return BAD_FUNC_ARG;

   if (mp_unsigned_bin_size(key->pubkey.x) > (int)numlen)
       return ECC_BAD_ARG_E;

   /* store first byte */
   out[0] = mp_isodd(key->pubkey.y) == MP_YES ? ECC_POINT_COMP_ODD : ECC_POINT_COMP_EVEN;

   /* pad and store x */
   XMEMSET(out+1, 0, numlen);
   ret = mp_to_unsigned_bin(
       key->pubkey.x,
       out+1 + (numlen - (word32)mp_unsigned_bin_size(key->pubkey.x)));
   *outLen = 1 + numlen;

   return ret;
}
#endif /* HAVE_ECC_KEY_EXPORT */
#endif /* HAVE_COMP_KEY */


int wc_ecc_get_oid(word32 oidSum, const byte** oid, word32* oidSz)
{
    int x;

    if (oidSum == 0) {
        return BAD_FUNC_ARG;
    }

    /* find matching OID sum (based on encoded value) */
    for (x = 0; ecc_sets[x].size != 0; x++) {
        if (ecc_sets[x].oidSum == oidSum) {
            int ret;
        #ifdef HAVE_OID_ENCODING
            ret = 0;
            /* check cache */
            oid_cache_t* o = &ecc_oid_cache[x];
            if (o->oidSz == 0) {
                o->oidSz = sizeof(o->oid);
                ret = EncodeObjectId(ecc_sets[x].oid, ecc_sets[x].oidSz,
                                                            o->oid, &o->oidSz);
            }
            if (oidSz) {
                *oidSz = o->oidSz;
            }
            if (oid) {
                *oid = o->oid;
            }
            /* on success return curve id */
            if (ret == 0) {
                ret = ecc_sets[x].id;
            }
        #else
            if (oidSz) {
                *oidSz = ecc_sets[x].oidSz;
            }
            if (oid) {
                *oid = ecc_sets[x].oid;
            }
            ret = ecc_sets[x].id;
        #endif
            return ret;
        }
    }

    return NOT_COMPILED_IN;
}

#ifdef WOLFSSL_CUSTOM_CURVES
int wc_ecc_set_custom_curve(ecc_key* key, const ecc_set_type* dp)
{
    if (key == NULL || dp == NULL) {
        return BAD_FUNC_ARG;
    }

    key->idx = ECC_CUSTOM_IDX;
    key->dp = dp;

    return 0;
}
#endif /* WOLFSSL_CUSTOM_CURVES */

#if defined(HAVE_X963_KDF) && !defined(NO_HASH_WRAPPER)

static WC_INLINE void IncrementX963KdfCounter(byte* inOutCtr)
{
    int i;

    /* in network byte order so start at end and work back */
    for (i = 3; i >= 0; i--) {
        if (++inOutCtr[i])  /* we're done unless we overflow */
            return;
    }
}

/* ASN X9.63 Key Derivation Function (SEC1) */
int wc_X963_KDF(enum wc_HashType type, const byte* secret, word32 secretSz,
                const byte* sinfo, word32 sinfoSz, byte* out, word32 outSz)
{
    int ret;
    word32 digestSz, copySz, remaining = outSz;
    byte* outIdx;
    byte  counter[4];
    byte  tmp[WC_MAX_DIGEST_SIZE];

#ifdef WOLFSSL_SMALL_STACK
    wc_HashAlg* hash;
#else
    wc_HashAlg hash[1];
#endif

    if (secret == NULL || secretSz == 0 || out == NULL)
        return BAD_FUNC_ARG;

    /* X9.63 allowed algos only */
    if (type != WC_HASH_TYPE_SHA    && type != WC_HASH_TYPE_SHA224 &&
        type != WC_HASH_TYPE_SHA256 && type != WC_HASH_TYPE_SHA384 &&
        type != WC_HASH_TYPE_SHA512)
        return BAD_FUNC_ARG;

    ret = wc_HashGetDigestSize(type);
    if (ret < 0)
        return ret;
    digestSz = (word32)ret;

#ifdef WOLFSSL_SMALL_STACK
    hash = (wc_HashAlg*)XMALLOC(sizeof(wc_HashAlg), NULL,
                                DYNAMIC_TYPE_HASHES);
    if (hash == NULL)
        return MEMORY_E;
#endif

    ret = wc_HashInit(hash, type);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(hash, NULL, DYNAMIC_TYPE_HASHES);
#endif
        return ret;
    }

    outIdx = out;
    XMEMSET(counter, 0, sizeof(counter));

    while (remaining > 0) {

        IncrementX963KdfCounter(counter);

        ret = wc_HashUpdate(hash, type, secret, secretSz);
        if (ret != 0) {
            break;
        }

        ret = wc_HashUpdate(hash, type, counter, sizeof(counter));
        if (ret != 0) {
            break;
        }

        if (sinfo) {
            ret = wc_HashUpdate(hash, type, sinfo, sinfoSz);
            if (ret != 0) {
                break;
            }
        }

        ret = wc_HashFinal(hash, type, tmp);
        if (ret != 0) {
            break;
        }

        copySz = min(remaining, digestSz);
        XMEMCPY(outIdx, tmp, copySz);

        remaining -= copySz;
        outIdx += copySz;
    }

    wc_HashFree(hash, type);

#ifdef WOLFSSL_SMALL_STACK
     XFREE(hash, NULL, DYNAMIC_TYPE_HASHES);
#endif

    return ret;
}
#endif /* HAVE_X963_KDF && !NO_HASH_WRAPPER */

#ifdef WOLFSSL_SE050
/* Use specified hardware key ID with ecc_key operations. Unlike devId,
 * keyId is a word32, can be used for key IDs larger than an int.
 *
 * key    initialized ecc_key struct
 * keyId  hardware key ID which stores ECC key
 * flags  optional flags, currently unused
 *
 * Return 0 on success, negative on error */
int wc_ecc_use_key_id(ecc_key* key, word32 keyId, word32 flags)
{
    (void)flags;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    return se050_ecc_use_key_id(key, keyId);
}

/* Get hardware key ID associated with this ecc_key structure.
 *
 * key    initialized ecc_key struct
 * keyId  [OUT] output for key ID associated with this structure
 *
 * Returns 0 on success, negative on error.
 */
int wc_ecc_get_key_id(ecc_key* key, word32* keyId)
{
    if (key == NULL || keyId == NULL) {
        return BAD_FUNC_ARG;
    }

    return se050_ecc_get_key_id(key, keyId);
}
#endif /* WOLFSSL_SE050 */


#ifdef WC_ECC_NONBLOCK
/* Enable ECC support for non-blocking operations */
int wc_ecc_set_nonblock(ecc_key *key, ecc_nb_ctx_t* ctx)
{
    if (key) {
        if (ctx) {
            XMEMSET(ctx, 0, sizeof(ecc_nb_ctx_t));
        }
        key->nb_ctx = ctx;
    }
    return 0;
}
#endif /* WC_ECC_NONBLOCK */

#endif /* HAVE_ECC */
