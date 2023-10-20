/* ksdk_port.c
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
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/* If FREESCALE_LTC_TFM or FREESCALE_LTC_ECC */
#if defined(FREESCALE_LTC_TFM) || defined(FREESCALE_LTC_ECC)

#include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <stdint.h>

#define ERROR_OUT(res) { ret = (res); goto done; }

/* For debugging only - Enable this to do software tests of each operation */
/* #define ENABLE_NXPLTC_TESTS */
#ifdef ENABLE_NXPLTC_TESTS
static int doLtcTest = 0;
#endif

int ksdk_port_init(void)
{
#if defined(FREESCALE_LTC_TFM)
    LTC_Init(LTC0);
#endif

    return 0;
}

/* Reverse array in memory (in place) */
static void ltc_reverse_array(uint8_t *src, size_t src_len)
{
    unsigned int i;

    for (i = 0; i < src_len / 2; i++) {
        uint8_t tmp;

        tmp = src[i];
        src[i] = src[src_len - 1 - i];
        src[src_len - 1 - i] = tmp;
    }
}


#ifndef WOLFSSL_SP_MATH
/* same as mp_to_unsigned_bin() with mp_reverse() skipped */
static int mp_to_unsigned_lsb_bin(mp_int *a, unsigned char *b)
{
    int res;
    mp_int t;

    res = mp_init_copy(&t, a);
    if (res == MP_OKAY) {
        res = mp_to_unsigned_bin_at_pos(0, &t, b);
        if (res >= 0)
            res = 0;
    #ifndef USE_FAST_MATH
        mp_clear(&t);
    #endif
    }

    return res;
}
#endif

static int ltc_get_lsb_bin_from_mp_int(uint8_t *dst, mp_int *A, uint16_t *psz)
{
    int res;
    uint16_t sz;

    sz = mp_unsigned_bin_size(A);
#ifndef WOLFSSL_SP_MATH
    /* result is lsbyte at lowest addr as required by LTC */
    res = mp_to_unsigned_lsb_bin(A, dst);
#else
    res = mp_to_unsigned_bin(A, dst);
    if (res == MP_OKAY) {
        ltc_reverse_array(dst, sz); 
    }
#endif
    *psz = sz;
    return res;
}

/* LTC TFM */
#if defined(FREESCALE_LTC_TFM)

/* these function are used by wolfSSL upper layers (like RSA) */

/* c = a * b */
int mp_mul(mp_int *A, mp_int *B, mp_int *C)
{
    int res = MP_OKAY;
    status_t status;
    int szA, szB;

#ifdef ENABLE_NXPLTC_TESTS
    mp_int t;
    mp_init(&t);
    if (doLtcTest)
        wolfcrypt_mp_mul(A, B, &t);
#endif

    szA = mp_unsigned_bin_size(A);
    szB = mp_unsigned_bin_size(B);

    /* if unsigned mul can fit into LTC PKHA let's use it, otherwise call 
     * software mul */
    if ((szA <= LTC_MAX_INT_BYTES / 2) && (szB <= LTC_MAX_INT_BYTES / 2)) {
        uint8_t *ptrA = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrB = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrN = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrC = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);

        /* unsigned multiply */
#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
        int neg = (A->sign == B->sign) ? MP_ZPOS : MP_NEG;
#endif

        if (ptrA && ptrB && ptrN && ptrC) {
            uint16_t sizeA, sizeB, sizeN, sizeC = 0;

            res = ltc_get_lsb_bin_from_mp_int(ptrA, A, &sizeA);
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrB, B, &sizeB);
            if (res == MP_OKAY) {
                sizeN = sizeA + sizeB;
                XMEMSET(ptrN, 0xFF, sizeN);
                XMEMSET(ptrC, 0, LTC_MAX_INT_BYTES);

                status = LTC_PKHA_ModMul(LTC_BASE,
                    ptrA, sizeA,  /* first integer */
                    ptrB, sizeB,  /* second integer */
                    ptrN, sizeN,  /* modulus */
                    ptrC, &sizeC, /* out */
                    kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
                    kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
                if (status == kStatus_Success) {
                    ltc_reverse_array(ptrC, sizeC);
                    res = mp_read_unsigned_bin(C, ptrC, sizeC);

#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
                    /* fix sign */
                    C->sign = neg;
#endif
                }
                else {
                    res = MP_VAL;
                }
            }
        }

        if (ptrA) {
            XFREE(ptrA, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrB) {
            XFREE(ptrB, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrN) {
            XFREE(ptrN, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrC) {
            XFREE(ptrC, NULL, DYNAMIC_TYPE_BIGINT);
        }
    }
    else {
#ifdef WOLFSSL_SP_MATH
        res = sp_mul(A, B, C);
#else
        res = wolfcrypt_mp_mul(A, B, C);
#endif
    }

#ifdef ENABLE_NXPLTC_TESTS
    /* compare hardware vs software */
    if (doLtcTest && mp_cmp(&t, C) != MP_EQ) {
        printf("mp_mul test fail!\n");

        mp_dump("C", C, 0);
        mp_dump("C soft", &t, 0);
    }
    mp_clear(&t);
#endif

    return res;
}

/* c = a mod b, 0 <= c < b  */
int mp_mod(mp_int *a, mp_int *b, mp_int *c)
{
    int res = MP_OKAY;
    int szA, szB;

#ifdef ENABLE_NXPLTC_TESTS
    mp_int t;
    mp_init(&t);
    if (doLtcTest)
        wolfcrypt_mp_mod(a, b, &t);
#endif

    szA = mp_unsigned_bin_size(a);
    szB = mp_unsigned_bin_size(b);
    if ((szA <= LTC_MAX_INT_BYTES) && (szB <= LTC_MAX_INT_BYTES)) {
        uint8_t *ptrA = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrB = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrC = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);

#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
        /* get sign for the result */
        int neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
#endif

        /* get remainder of unsigned a divided by unsigned b */
        if (ptrA && ptrB && ptrC) {
            uint16_t sizeA, sizeB, sizeC;

            res = ltc_get_lsb_bin_from_mp_int(ptrA, a, &sizeA);
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrB, b, &sizeB);
            if (res == MP_OKAY) {
                if (kStatus_Success ==
                    LTC_PKHA_ModRed(LTC_BASE, ptrA, sizeA, ptrB, sizeB, ptrC,
                        &sizeC, kLTC_PKHA_IntegerArith))
                {
                    ltc_reverse_array(ptrC, sizeC);
                    res = mp_read_unsigned_bin(c, ptrC, sizeC);
                
#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
                    /* fix sign */
                    c->sign = neg;
#endif
                }
                else {
                    res = MP_VAL;
                }
            }
        }
        else {
            res = MP_MEM;
        }

        if (ptrA) {
            XFREE(ptrA, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrB) {
            XFREE(ptrB, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrC) {
            XFREE(ptrC, NULL, DYNAMIC_TYPE_BIGINT);
        }
    }
    else {
#if defined(FREESCALE_LTC_TFM_RSA_4096_ENABLE)
        res = wolfcrypt_mp_mod(a, b, c);
#else
        res = NOT_COMPILED_IN;
#endif
    }

#ifdef ENABLE_NXPLTC_TESTS
    /* compare hardware vs software */
    if (doLtcTest && mp_cmp(&t, c) != MP_EQ) {
        printf("mp_mod test fail!\n");

        mp_dump("C", c, 0);
        mp_dump("C soft", &t, 0);
    }
    mp_clear(&t);
#endif

    return res;
}

/* c = 1/a (mod b) for odd b only */
int mp_invmod(mp_int *a, mp_int *b, mp_int *c)
{
    int res = MP_OKAY;
    int szA, szB;

#ifdef ENABLE_NXPLTC_TESTS
    mp_int t;
    mp_init(&t);
    if (doLtcTest)
        wolfcrypt_mp_invmod(a, b, &t);
#endif

    szA = mp_unsigned_bin_size(a);
    szB = mp_unsigned_bin_size(b);
    if ((szA <= LTC_MAX_INT_BYTES) && (szB <= LTC_MAX_INT_BYTES)) {
        uint8_t *ptrA = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrB = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);
        uint8_t *ptrC = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL,
            DYNAMIC_TYPE_BIGINT);

        if (ptrA && ptrB && ptrC) {
            uint16_t sizeA, sizeB, sizeC;

            res = ltc_get_lsb_bin_from_mp_int(ptrA, a, &sizeA);
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrB, b, &sizeB);
            
            /* if a >= b then reduce */
            /* TODO: Perhaps always do mod reduce depending on hardware performance */
            if (res == MP_OKAY && 
                        LTC_PKHA_CompareBigNum(ptrA, sizeA, ptrB, sizeB) >= 0) {
                if (LTC_PKHA_ModRed(LTC_BASE, ptrA, sizeA, ptrB, sizeB,
                    ptrA, &sizeA, kLTC_PKHA_IntegerArith) != kStatus_Success) {
                    res = MP_VAL;
                }
            }
            if (res == MP_OKAY) {
                if (LTC_PKHA_ModInv(LTC_BASE, ptrA, sizeA, ptrB, sizeB, ptrC,
                        &sizeC, kLTC_PKHA_IntegerArith) == kStatus_Success) {
                    ltc_reverse_array(ptrC, sizeC);
                    res = mp_read_unsigned_bin(c, ptrC, sizeC);


#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
                    c->sign = a->sign;
#endif
                }
                else {
                    res = MP_VAL;
                }
            }
        }
        else {
            res = MP_MEM;
        }

        if (ptrA) {
            XFREE(ptrA, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrB) {
            XFREE(ptrB, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrC) {
            XFREE(ptrC, NULL, DYNAMIC_TYPE_BIGINT);
        }
    }
    else {
#if defined(FREESCALE_LTC_TFM_RSA_4096_ENABLE)
        res = wolfcrypt_mp_invmod(a, b, c);
#else
        res = NOT_COMPILED_IN;
#endif
    }

#ifdef ENABLE_NXPLTC_TESTS
    /* compare hardware vs software */
    if (doLtcTest && mp_cmp(&t, c) != MP_EQ) {
        printf("mp_invmod test fail!\n");

        mp_dump("C", c, 0);
        mp_dump("C soft", &t, 0);
    }
    mp_clear(&t);
#endif

    return res;
}

/* d = a * b (mod c) */
/* with blinding enabled ptr a can equal ptr d */
int mp_mulmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
{
    int res = MP_OKAY;
    status_t status;
    int szA, szB, szC;
    
#ifdef ENABLE_NXPLTC_TESTS
    mp_int t;
    mp_init(&t);
    if (doLtcTest)
        wolfcrypt_mp_mulmod(a, b, c, &t);
#endif

    szA = mp_unsigned_bin_size(a);
    szB = mp_unsigned_bin_size(b);
    szC = mp_unsigned_bin_size(c);

    /* LTC hardware seems to have issue with ModMul with 511 bytes or more,
        so use software math */
    if ((szA + szB < LTC_MAX_INT_BYTES-1) && (szC <=  LTC_MAX_INT_BYTES))
    {
        uint8_t *ptrA, *ptrB, *ptrC, *ptrD;

        ptrA = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrB = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrC = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrD = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);

        if (ptrA && ptrB && ptrC && ptrD) {
            uint16_t sizeA, sizeB, sizeC, sizeD = 0;

            /* unsigned multiply */
#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
            int neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;
#endif

            /* Multiply A * B = D */
            res = ltc_get_lsb_bin_from_mp_int(ptrA, a, &sizeA);
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrB, b, &sizeB);
            if (res == MP_OKAY) {
                /* modulus C is all F's for integer multiply */
                sizeC = sizeA + sizeB;
                XMEMSET(ptrC, 0xFF, sizeC);
                XMEMSET(ptrD, 0, LTC_MAX_INT_BYTES);

                status = LTC_PKHA_ModMul(LTC_BASE,
                    ptrA, sizeA,  /* first integer */
                    ptrB, sizeB,  /* second integer */
                    ptrC, sizeC,  /* modulus */
                    ptrD, &sizeD, /* out */
                    kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
                    kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
                if (status != kStatus_Success)
                    res = MP_VAL;
            }
            /* load modulus */
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrC, c, &sizeC);
            /* perform D mod C = D */
            if (res == MP_OKAY) {
                status = LTC_PKHA_ModRed(LTC_BASE,
                    ptrD, sizeD,
                    ptrC, sizeC,
                    ptrD, &sizeD,
                    kLTC_PKHA_IntegerArith);
                if (status != kStatus_Success)
                    res = MP_VAL;
            }
            if (res == MP_OKAY) {
                ltc_reverse_array(ptrD, sizeD);
                res = mp_read_unsigned_bin(d, ptrD, sizeD);

#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
                /* fix sign */
                d->sign = neg;
#endif
            }
        }
        else {
            res = MP_MEM;
        }

        if (ptrA) {
            XFREE(ptrA, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrB) {
            XFREE(ptrB, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrC) {
            XFREE(ptrC, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrD) {
            XFREE(ptrD, NULL, DYNAMIC_TYPE_BIGINT);
        }
    }
    else {
#if defined(FREESCALE_LTC_TFM_RSA_4096_ENABLE)
        res = wolfcrypt_mp_mulmod(a, b, c, d);
#else
        res = NOT_COMPILED_IN;
#endif
    }

#ifdef ENABLE_NXPLTC_TESTS
    /* compare hardware vs software */
    if (doLtcTest && mp_cmp(&t, d) != MP_EQ) {
        printf("mp_mulmod test fail!\n");

        mp_dump("D", d, 0);
        mp_dump("D soft", &t, 0);
    }
    mp_clear(&t);
#endif

    return res;
}

/* Y = G^X mod P */
int ltc_mp_exptmod(mp_int *G, mp_int *X, mp_int *P, mp_int *Y, int useConstTime)
{
    int res = MP_OKAY;
    int szG, szX, szP;

#ifdef ENABLE_NXPLTC_TESTS
    mp_int t;
    mp_init(&t);
    if (doLtcTest)
        wolfcrypt_mp_exptmod(G, X, P, &t);
#endif

    szG = mp_unsigned_bin_size(G);
    szX = mp_unsigned_bin_size(X);
    szP = mp_unsigned_bin_size(P);

    if ((szG <= LTC_MAX_INT_BYTES) && 
        (szX <= LTC_MAX_INT_BYTES) && 
        (szP <= LTC_MAX_INT_BYTES))
    {
        uint16_t sizeG, sizeX, sizeP, sizeY;
        uint8_t *ptrG, *ptrX, *ptrP, *ptrY;

        ptrG = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrX = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrP = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrY = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        if (ptrG && ptrX && ptrP && ptrY) {
            res = ltc_get_lsb_bin_from_mp_int(ptrG, G, &sizeG);
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrX, X, &sizeX);
            if (res == MP_OKAY)
                res = ltc_get_lsb_bin_from_mp_int(ptrP, P, &sizeP);

            /* if G >= P then reduce */
            /* TODO: Perhaps always do mod reduce depending on hardware performance */
            if (res == MP_OKAY && 
                        LTC_PKHA_CompareBigNum(ptrG, sizeG, ptrP, sizeP) >= 0) {
                res = LTC_PKHA_ModRed(LTC_BASE, 
                    ptrG, sizeG,
                    ptrP, sizeP,
                    ptrG, &sizeG, kLTC_PKHA_IntegerArith);
                res = (res == kStatus_Success) ? MP_OKAY: MP_VAL;
            }
            if (res == MP_OKAY) {
                res = LTC_PKHA_ModExp(LTC_BASE,
                    ptrG, sizeG,  /* integer input */
                    ptrP, sizeP,  /* modulus */
                    ptrX, sizeX,  /* expenoent */
                    ptrY, &sizeY, /* out */
                    kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
                    useConstTime ? kLTC_PKHA_TimingEqualized :
                                   kLTC_PKHA_NoTimingEqualized);
                res = (res == kStatus_Success) ? MP_OKAY: MP_VAL;
            }
            if (res == MP_OKAY) {
                ltc_reverse_array(ptrY, sizeY);
                res = mp_read_unsigned_bin(Y, ptrY, sizeY);
            }
        }
        else {
            res = MP_MEM;
        }

        if (ptrY) {
            XFREE(ptrY, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrP) {
            XFREE(ptrP, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrX) {
            XFREE(ptrX, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrG) {
            XFREE(ptrG, NULL, DYNAMIC_TYPE_BIGINT);
        }        
    }
    else {
#if defined(FREESCALE_LTC_TFM_RSA_4096_ENABLE)
        res = wolfcrypt_mp_exptmod(G, X, P, Y);
#else
        res = NOT_COMPILED_IN;
#endif
    }

#ifdef ENABLE_NXPLTC_TESTS
    /* compare hardware vs software */
    if (doLtcTest && mp_cmp(&t, Y) != MP_EQ) {
        printf("mp_exptmod test fail!\n");

        mp_dump("Y", Y, 0);
        mp_dump("Y soft", &t, 0);
    }
    mp_clear(&t);
#endif

    return res;
}

int mp_exptmod(mp_int *G, mp_int *X, mp_int *P, mp_int *Y)
{
    return ltc_mp_exptmod(G, X, P, Y, 1);
}

int mp_exptmod_nct(mp_int * G, mp_int * X, mp_int * P, mp_int * Y)
{
    return ltc_mp_exptmod(G, X, P, Y, 0);
}

#if !defined(NO_DH) || !defined(NO_DSA) || !defined(NO_RSA) || \
    defined(WOLFSSL_KEY_GEN)
int mp_prime_is_prime_ex(mp_int* a, int t, int* result, WC_RNG* rng)
{
    int res = MP_OKAY;
    int szA;

#ifdef ENABLE_NXPLTC_TESTS
    int result_soft = 0;
    if (doLtcTest)
        mp_prime_is_prime_ex(a, t, &result_soft, rng);
#endif

    szA = mp_unsigned_bin_size(a);
    if (szA <= LTC_MAX_INT_BYTES) {
        uint16_t sizeA, sizeB;
        uint8_t *ptrA, *ptrB;

        sizeB = mp_count_bits(a);
        /* The base size is the number of bits / 8. One is added if the number
         * of bits isn't an even 8. */
        sizeB = (sizeB / 8) + ((sizeB % 8) ? 1 : 0);

        ptrA = (uint8_t*)XMALLOC(LTC_MAX_INT_BYTES, NULL, DYNAMIC_TYPE_BIGINT);
        ptrB = (uint8_t*)XMALLOC(sizeB, NULL, DYNAMIC_TYPE_BIGINT);
        if (ptrA == NULL || ptrB == NULL) {
            res = MEMORY_E;
        }

        if (res == MP_OKAY) {
        #ifndef WC_NO_RNG
            /* A NULL rng will return as bad function arg */
            res = wc_RNG_GenerateBlock(rng, ptrB, sizeB);
        #else
            res = NOT_COMPILED_IN;
        #endif
        }

        if (res == MP_OKAY) {
            res = ltc_get_lsb_bin_from_mp_int(ptrA, a, &sizeA);
        }
        if (res == MP_OKAY) {
            if (LTC_PKHA_PrimalityTest(LTC_BASE, 
                ptrB, sizeB,             /* seed */
                (uint8_t*)&t, sizeof(t), /* trials */
                ptrA, sizeA,             /* candidate */
                (bool*)result) != kStatus_Success) {
                res = MP_MEM;
            }
        }

        if (ptrB) {
            XFREE(ptrB, NULL, DYNAMIC_TYPE_BIGINT);
        }
        if (ptrA) {
            XFREE(ptrA, NULL, DYNAMIC_TYPE_BIGINT);
        }
    }
    else {
#if defined(FREESCALE_LTC_TFM_RSA_4096_ENABLE)
        res = wolfcrypt_mp_prime_is_prime_ex(a, t, result, rng);
#else
        res = NOT_COMPILED_IN;
#endif
    }

#ifdef ENABLE_NXPLTC_TESTS
    /* compare hardware vs software */
    if (doLtcTest && *result != result_soft) {
        printf("Fail! mp_prime_is_prime_ex %d != %d\n", *result, result_soft);
    }
#endif

    return res;
}

int mp_prime_is_prime(mp_int* a, int t, int* result)
{
    /* the NXP LTC prime check requires an RNG, so use software version */
    return wolfcrypt_mp_prime_is_prime_ex(a, t, result, NULL);
}
#endif /* !NO_RSA || !NO_DSA || !NO_DH || WOLFSSL_KEY_GEN */

#endif /* FREESCALE_LTC_TFM */


/* ECC */
#if defined(HAVE_ECC) && defined(FREESCALE_LTC_ECC)

/* convert from mp_int to LTC integer, as array of bytes of size sz.
 * if mp_int has less bytes than sz, add zero bytes at most significant byte 
 *   positions.
 * This is when for example modulus is 32 bytes (P-256 curve)
 * and mp_int has only 31 bytes, we add leading zeros
 * so that result array has 32 bytes, same as modulus (sz).
 */
static int ltc_get_from_mp_int(uint8_t *dst, mp_int *a, int sz)
{
    int res;
    int szbin;
    int offset;

    /* check how many bytes are in the mp_int */
    szbin = mp_unsigned_bin_size(a);

    /* compute offset from dst */
    offset = sz - szbin;
    if (offset < 0)
        offset = 0;
    if (offset > sz)
        offset = sz;

    /* add leading zeroes */
    if (offset)
        XMEMSET(dst, 0, offset);

    /* convert mp_int to array of bytes */
    res = mp_to_unsigned_bin(a, dst + offset);

    if (res == MP_OKAY) {
        /* reverse array for LTC direct use */
        ltc_reverse_array(dst, sz);
    }

    return res;
}

/* ECC specs in lsbyte at lowest address format for direct use by LTC PKHA 
 * driver functions */
#if defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)
#define ECC192
#endif
#if defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)
#define ECC224
#endif
#if !defined(NO_ECC256) || defined(HAVE_ALL_CURVES)
#define ECC256
#endif
#if defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)
#define ECC384
#endif

#ifdef ECC192
#ifndef NO_ECC_SECP
static const uint8_t ltc_secp192r1_modulus[24] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp192r1_r2modn[24] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp192r1_aCurveParam[24] = {
    0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp192r1_bCurveParam[24] = {
    0xB1, 0xB9, 0x46, 0xC1, 0xEC, 0xDE, 0xB8, 0xFE, 0x49, 0x30, 0x24, 0x72,
    0xAB, 0xE9, 0xA7, 0x0F, 0xE7, 0x80, 0x9C, 0xE5, 0x19, 0x05, 0x21, 0x64};
#endif
#ifdef HAVE_ECC_KOBLITZ
static const uint8_t ltc_secp192k1_modulus[24] = {
    0x37, 0xEE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp192k1_r2modn[24] = {
    0xD1, 0x4F, 0x3C, 0x01, 0x92, 0x23, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp192k1_aCurveParam[24] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp192k1_bCurveParam[24] = {
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
#endif /* ECC192 */

#ifdef ECC224
#ifndef NO_ECC_SECP
static const uint8_t ltc_secp224r1_modulus[28] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp224r1_r2modn[28] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp224r1_aCurveParam[28] = {
    0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp224r1_bCurveParam[28] = {
    0xB4, 0xFF, 0x55, 0x23, 0x43, 0x39, 0x0B, 0x27, 0xBA, 0xD8,
    0xBF, 0xD7, 0xB7, 0xB0, 0x44, 0x50, 0x56, 0x32, 0x41, 0xF5,
    0xAB, 0xB3, 0x04, 0x0C, 0x85, 0x0A, 0x05, 0xB4};
#endif
#ifdef HAVE_ECC_KOBLITZ
static const uint8_t ltc_secp224k1_modulus[28] = {
    0x6D, 0xE5, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp224k1_r2modn[28] = {
    0x69, 0x30, 0xC2, 0x02, 0x26, 0x35, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp224k1_aCurveParam[28] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp224k1_bCurveParam[28] = {
    0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
#endif /* ECC224 */

#ifdef ECC256
#ifndef NO_ECC_SECP
static const uint8_t ltc_secp256r1_modulus[32] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp256r1_r2modn[32] = { /* R^2 mod N */
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0x04, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp256r1_aCurveParam[32] = {
    0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp256r1_bCurveParam[32] = {
    0x4B, 0x60, 0xD2, 0x27, 0x3E, 0x3C, 0xCE, 0x3B, 0xF6, 0xB0, 0x53,
    0xCC, 0xB0, 0x06, 0x1D, 0x65, 0xBC, 0x86, 0x98, 0x76, 0x55, 0xBD,
    0xEB, 0xB3, 0xE7, 0x93, 0x3A, 0xAA, 0xD8, 0x35, 0xC6, 0x5A};
#endif
#ifdef HAVE_ECC_KOBLITZ
static const uint8_t ltc_secp256k1_modulus[32] = {
    0x2F, 0xFC, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const uint8_t ltc_secp256k1_r2modn[32] = { /* R^2 mod N */
    0xA1, 0x90, 0x0E, 0x00, 0xA2, 0x07, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp256k1_aCurveParam[32] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp256k1_bCurveParam[32] = {
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
#endif /* ECC256 */

#ifdef ECC384
#ifndef NO_ECC_SECP
static const uint8_t ltc_secp384r1_modulus[48] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t ltc_secp384r1_r2modn[48] = {
    0x01, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t ltc_secp384r1_aCurveParam[48] = {
    0xfc, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static const uint8_t ltc_secp384r1_bCurveParam[48] = {
    0xef, 0x2a, 0xec, 0xd3, 0xed, 0xc8, 0x85, 0x2a, 0x9d, 0xd1, 0x2e, 0x8a,
    0x8d, 0x39, 0x56, 0xc6, 0x5a, 0x87, 0x13, 0x50, 0x8f, 0x08, 0x14, 0x03,
    0x12, 0x41, 0x81, 0xfe, 0x6e, 0x9c, 0x1d, 0x18, 0x19, 0x2d, 0xf8, 0xe3,
    0x6b, 0x05, 0x8e, 0x98, 0xe4, 0xe7, 0x3e, 0xe2, 0xa7, 0x2f, 0x31, 0xb3};
#endif
#endif /* ECC384 */

static int ltc_get_ecc_specs(const uint8_t **modulus, const uint8_t **r2modn,
    const uint8_t **aCurveParam, const uint8_t **bCurveParam, const uint8_t *prime,
    int size)
{
    switch (size) {
#ifdef ECC256
    case 32:
#ifndef NO_ECC_SECP
        if (XMEMCMP(prime, ltc_secp256r1_modulus, size) == 0) {
            *modulus = ltc_secp256r1_modulus;
            *r2modn = ltc_secp256r1_r2modn;
            *aCurveParam = ltc_secp256r1_aCurveParam;
            *bCurveParam = ltc_secp256r1_bCurveParam;
            return 0;
        }
#endif
#ifdef HAVE_ECC_KOBLITZ
        if (XMEMCMP(prime, ltc_secp256k1_modulus, size) == 0) {
            *modulus = ltc_secp256k1_modulus;
            *r2modn = ltc_secp256k1_r2modn;
            *aCurveParam = ltc_secp256k1_aCurveParam;
            *bCurveParam = ltc_secp256k1_bCurveParam;
            return 0;
        }
#endif
        break;
#endif /* ECC256 */

#ifdef ECC224
    case 28:
#ifndef NO_ECC_SECP
        if (XMEMCMP(prime, ltc_secp224r1_modulus, size) == 0) {
            *modulus = ltc_secp224r1_modulus;
            *r2modn = ltc_secp224r1_r2modn;
            *aCurveParam = ltc_secp224r1_aCurveParam;
            *bCurveParam = ltc_secp224r1_bCurveParam;
            return 0;
        }
#endif
#ifdef HAVE_ECC_KOBLITZ
        if (XMEMCMP(prime, ltc_secp224k1_modulus, size) == 0) {
            *modulus = ltc_secp224k1_modulus;
            *r2modn = ltc_secp224k1_r2modn;
            *aCurveParam = ltc_secp224k1_aCurveParam;
            *bCurveParam = ltc_secp224k1_bCurveParam;
            return 0;
        }
#endif
        break;
#endif /* ECC224 */

#ifdef ECC192
    case 24:
#ifndef NO_ECC_SECP
        if (XMEMCMP(prime, ltc_secp192r1_modulus, size) == 0) {
            *modulus = ltc_secp192r1_modulus;
            *r2modn = ltc_secp192r1_r2modn;
            *aCurveParam = ltc_secp192r1_aCurveParam;
            *bCurveParam = ltc_secp192r1_bCurveParam;
            return 0;
        }
#endif
#ifdef HAVE_ECC_KOBLITZ
        if (XMEMCMP(prime, ltc_secp192k1_modulus, size) == 0) {
            *modulus = ltc_secp192k1_modulus;
            *r2modn = ltc_secp192k1_r2modn;
            *aCurveParam = ltc_secp192k1_aCurveParam;
            *bCurveParam = ltc_secp192k1_bCurveParam;
            return 0;
        }
#endif
        break;
#endif /* ECC192 */

#ifdef ECC384
    case 48:
#ifndef NO_ECC_SECP
        if (XMEMCMP(prime, ltc_secp384r1_modulus, size) == 0) {
            *modulus = ltc_secp384r1_modulus;
            *r2modn = ltc_secp384r1_r2modn;
            *aCurveParam = ltc_secp384r1_aCurveParam;
            *bCurveParam = ltc_secp384r1_bCurveParam;
            return 0;
        }
#endif
        break;
#endif /* ECC384 */

    default:
        break;
    }

    return -1;
}

/**
   Perform a point multiplication  (timing resistant)
   k    The scalar to multiply by
   G    The base point
   R    [out] Destination for kG
   modulus  The modulus of the field the ECC curve is in
   map      Boolean whether to map back to affine or not
            (1==map, 0 == leave in projective)
   return MP_OKAY on success
*/
int wc_ecc_mulmod_ex(const mp_int *k, ecc_point *G, ecc_point *R, mp_int* a,
    mp_int *modulus, int map, void* heap)
{
    ltc_pkha_ecc_point_t B;
    uint8_t size;
    int szModulus;
    int szkbin;
    bool point_of_infinity;
    status_t status;
    int res;

    uint8_t Gxbin[LTC_MAX_ECC_BITS / 8];
    uint8_t Gybin[LTC_MAX_ECC_BITS / 8];
    uint8_t kbin[LTC_MAX_INT_BYTES];

    const uint8_t *modbin;
    const uint8_t *aCurveParam;
    const uint8_t *bCurveParam;
    const uint8_t *r2modn;

    (void)a;
    (void)heap;

    if (k == NULL || G == NULL || R == NULL || modulus == NULL) {
        return ECC_BAD_ARG_E;
    }

    szModulus = mp_unsigned_bin_size(modulus);
    szkbin = mp_unsigned_bin_size(k);

    /* make sure LTC big number variable is large enough */
    if (szModulus > LTC_MAX_INT_BYTES / 2) {
        return MP_MEM;
    }

    res = ltc_get_from_mp_int(kbin, (mp_int*)k, szkbin);
    if (res == MP_OKAY)
        res = ltc_get_from_mp_int(Gxbin, G->x, szModulus);
    if (res == MP_OKAY)
        res = ltc_get_from_mp_int(Gybin, G->y, szModulus);
    if (res != MP_OKAY)
        return res;

    size = szModulus;

    /* find LTC friendly parameters for the selected curve */
    if (ltc_get_ecc_specs(&modbin, &r2modn, &aCurveParam, &bCurveParam,
                          (const uint8_t *)modulus->dp, size) != 0) {
        return ECC_BAD_ARG_E;
    }

    B.X = &Gxbin[0];
    B.Y = &Gybin[0];

    status = LTC_PKHA_ECC_PointMul(LTC_BASE, &B, kbin, szkbin, modbin, r2modn,
        aCurveParam, bCurveParam, size, kLTC_PKHA_TimingEqualized,
        kLTC_PKHA_IntegerArith, &B, &point_of_infinity);
    if (status != kStatus_Success) {
        return MP_VAL;
    }

    ltc_reverse_array(Gxbin, size);
    ltc_reverse_array(Gybin, size);
    res = mp_read_unsigned_bin(R->x, Gxbin, size);
    if (res == MP_OKAY) {
        res = mp_read_unsigned_bin(R->y, Gybin, size);
        /* if k is negative, we compute the multiplication with abs(-k)
         * with result (x, y) and modify the result to (x, -y)
         */
#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
      defined(WOLFSSL_SP_INT_NEGATIVE)
        R->y->sign = k->sign;
#endif
    }
    if (res == MP_OKAY)
        res = mp_set(R->z, 1);

    return res;
}

int wc_ecc_mulmod_ex2(const mp_int* k, ecc_point *G, ecc_point *R, mp_int* a,
                      mp_int* modulus, mp_int* order, WC_RNG* rng, int map,
                      void* heap)
{
    (void)order;
    (void)rng;
    return wc_ecc_mulmod_ex(k, G, R, a, modulus, map, heap);
}

int ecc_map_ex(ecc_point* P, mp_int* modulus, mp_digit mp, int ct)
{
    /* this is handled in hardware, so no projective mapping needed */
    (void)P;
    (void)modulus;
    (void)mp;
    (void)ct;
    return MP_OKAY;
}

int wc_ecc_point_add(ecc_point *mG, ecc_point *mQ, ecc_point *mR, mp_int *m)
{
    int res;
    ltc_pkha_ecc_point_t A, B;
    int size;
    status_t status;

    uint8_t Gxbin[LTC_MAX_ECC_BITS / 8];
    uint8_t Gybin[LTC_MAX_ECC_BITS / 8];
    uint8_t Qxbin[LTC_MAX_ECC_BITS / 8];
    uint8_t Qybin[LTC_MAX_ECC_BITS / 8];
    const uint8_t *modbin;
    const uint8_t *aCurveParam;
    const uint8_t *bCurveParam;
    const uint8_t *r2modn;

    size = mp_unsigned_bin_size(m);

    /* find LTC friendly parameters for the selected curve */
    if (ltc_get_ecc_specs(&modbin, &r2modn, &aCurveParam, &bCurveParam,
                          (const uint8_t *)m->dp, size) != 0) {
        res = ECC_BAD_ARG_E;
    }
    else {
        res = ltc_get_from_mp_int(Gxbin, mG->x, size);
        if (res == MP_OKAY)
            res = ltc_get_from_mp_int(Gybin, mG->y, size);
        if (res == MP_OKAY)
            res = ltc_get_from_mp_int(Qxbin, mQ->x, size);
        if (res == MP_OKAY)
            res = ltc_get_from_mp_int(Qybin, mQ->y, size);

        if (res != MP_OKAY)
            return res;

        A.X = Gxbin;
        A.Y = Gybin;

        B.X = Qxbin;
        B.Y = Qybin;

        status = LTC_PKHA_ECC_PointAdd(LTC_BASE, &A, &B, modbin, r2modn,
            aCurveParam, bCurveParam, size, kLTC_PKHA_IntegerArith, &A);
        if (status != kStatus_Success) {
            res = MP_VAL;
        }
        else {
            ltc_reverse_array(Gxbin, size);
            ltc_reverse_array(Gybin, size);
            res = mp_read_unsigned_bin(mR->x, Gxbin, size);
            if (res == MP_OKAY)
                res = mp_read_unsigned_bin(mR->y, Gybin, size);
            if (res == MP_OKAY)
                res = mp_set(mR->z, 1);
        }
    }
    return res;
}

#if defined(HAVE_ED25519) || defined(HAVE_CURVE25519)
/* Weierstrass parameters of prime 2^255 - 19 */
static const uint8_t curve25519_modbin[32] = {
    0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};
/* precomputed R2modN for the curve25519 */
static const uint8_t r2mod[32] = {
    0xa4, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* invThree = ModInv(3,curve25519_modbin) in LSB first */
static const uint8_t invThree[32] = {
    0x49, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

/*
 *
 * finds square root in finite field when modulus congruent to 5 modulo 8
 * this is fixed to curve25519 modulus 2^255 - 19 which is congruent to 
 * 5 modulo 8.
 *
 * This function solves equation: res^2 = a mod (2^255 - 19)
 *
p = prime
p % 8 must be 5

v = ModularArithmetic.powmod(2*a, (p-5)/8, p)
i = (2*a*v**2) % p
r1 = 1*a*v*(i - 1) % p
r2 = -1*a*v*(i - 1) % p
puts "Gy=0x#{r2.to_s(16)}"
 */
status_t LTC_PKHA_Prime25519SquareRootMod(const uint8_t *A, size_t sizeA,
    uint8_t *res, size_t *szRes, int sign)
{
    status_t status;
    const uint8_t curve25519_param[] = {
        0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f};
    uint8_t twoA[sizeof(curve25519_modbin)] = {0};
    uint8_t V[sizeof(curve25519_modbin)] = {0};
    uint8_t I[sizeof(curve25519_modbin)] = {0};
    uint8_t VV[sizeof(curve25519_modbin)] = {0};
    uint16_t szTwoA = 0;
    uint16_t szV = 0;
    uint16_t szVV = 0;
    uint16_t szI = 0;
    uint16_t szRes16 = 0;
    uint8_t one = 1;

    /* twoA = 2*A % p */
    status = LTC_PKHA_ModAdd(LTC_BASE, A, sizeA, A, sizeA, curve25519_modbin,
        sizeof(curve25519_modbin), twoA, &szTwoA, kLTC_PKHA_IntegerArith);

    /* V = ModularArithmetic.powmod(twoA, (p-5)/8, p) */
    if (status == kStatus_Success) {
        status =
            LTC_PKHA_ModExp(LTC_BASE, twoA, szTwoA, curve25519_modbin,
                sizeof(curve25519_modbin), curve25519_param,
                sizeof(curve25519_param), V, &szV, kLTC_PKHA_IntegerArith,
                kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* VV = V*V % p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, V, szV, V, szV, curve25519_modbin,
            sizeof(curve25519_modbin), VV, &szVV, kLTC_PKHA_IntegerArith,
            kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
            kLTC_PKHA_TimingEqualized);
    }

    /* I = twoA * VV = 2*A*V*V % p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, twoA, szTwoA, VV, szVV,
            curve25519_modbin, sizeof(curve25519_modbin), I, &szI,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* I = I - 1 */
    /* just temp for maximum integer - for non-modular subtract */
    XMEMSET(VV, 0xff, sizeof(VV));
    if (LTC_PKHA_CompareBigNum(I, szI, &one, sizeof(one)) >= 0) {
        if (status == kStatus_Success) {
            status = LTC_PKHA_ModSub1(LTC_BASE, I, szI, &one, sizeof(one),
                VV, sizeof(VV), I, &szI);
        }
    }
    else {
        if (status == kStatus_Success) {
            status = LTC_PKHA_ModSub1(LTC_BASE, curve25519_modbin,
                sizeof(curve25519_modbin), &one, sizeof(one), VV, sizeof(VV), I,
                &szI);
        }
    }

    /* res = a*v  mod p */
    status = LTC_PKHA_ModMul(LTC_BASE, A, sizeA, V, szV, curve25519_modbin,
        sizeof(curve25519_modbin), res, &szRes16, kLTC_PKHA_IntegerArith,
        kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
        kLTC_PKHA_TimingEqualized);

    /* res = res * (i-1) mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, res, szRes16, I, szI,
            curve25519_modbin, sizeof(curve25519_modbin), res, &szRes16,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* if X mod 2 != X_0 then we need the -X
     *
     * X mod 2 get from LSB bit0
     */
    if ((status == kStatus_Success) &&
        ((bool)sign != (bool)(res[0] & 0x01u)))
    {
        status = LTC_PKHA_ModSub1(LTC_BASE, curve25519_modbin,
            sizeof(curve25519_modbin), res, szRes16, VV, sizeof(VV), res,
            &szRes16); /* -a = p - a */
    }

    if (status == kStatus_Success) {
        *szRes = szRes16;
    }

    return status;
}
#endif /* HAVE_ED25519 || HAVE_CURVE25519 */


#ifdef HAVE_CURVE25519

/* for LTC we need Weierstrass format of curve25519 parameters
         * these two are base point X and Y.
         * in LSB first format (native for LTC)
         */
static const ECPoint ecBasePoint = {
   {0x5a, 0x24, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a},
   {0xd9, 0xd3, 0xce, 0x7e, 0xa2, 0xc5, 0xe9, 0x29, 0xb2, 0x61, 0x7c,
    0x6d, 0x7e, 0x4d, 0x3d, 0x92, 0x4c, 0xd1, 0x48, 0x77, 0x2c, 0xdd,
    0x1e, 0xe0, 0xb4, 0x86, 0xa0, 0xb8, 0xa1, 0x19, 0xae, 0x20},
};

const ECPoint *nxp_ltc_curve25519_GetBasePoint(void)
{
    return &ecBasePoint;
}

static const uint8_t curve25519_aCurveParam[CURVE25519_KEYSIZE] = {
    0x44, 0xa1, 0x14, 0x49, 0x98, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
    0xaa, 0xaa, 0xaa, 0xaa, 0x2a};

static const uint8_t curve25519_bCurveParam[CURVE25519_KEYSIZE] = {
    0x64, 0xc8, 0x10, 0x77, 0x9c, 0x5e, 0x0b, 0x26, 0xb4, 0x97, 0xd0,
    0x5e, 0x42, 0x7b, 0x09, 0xed,
    0x25, 0xb4, 0x97, 0xd0, 0x5e, 0x42, 0x7b, 0x09, 0xed, 0x25, 0xb4,
    0x97, 0xd0, 0x5e, 0x42, 0x7b};

/* transform a point on Montgomery curve to a point on Weierstrass curve */
status_t LTC_PKHA_Curve25519ToWeierstrass(
    const ltc_pkha_ecc_point_t *ltcPointIn,ltc_pkha_ecc_point_t *ltcPointOut)
{
    /* offset X point (in Montgomery) so that it becomes Weierstrass */
    const uint8_t offset[] = {
        0x51, 0x24, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
        0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a};
    uint16_t sizeRes = 0;
    status_t status;
    status = LTC_PKHA_ModAdd(LTC_BASE, ltcPointIn->X, CURVE25519_KEYSIZE,
        offset, sizeof(offset), curve25519_modbin, CURVE25519_KEYSIZE,
        ltcPointOut->X, &sizeRes, kLTC_PKHA_IntegerArith);

    if (status == kStatus_Success) {
        if (ltcPointOut->Y != ltcPointIn->Y) {
            XMEMCPY(ltcPointOut->Y, ltcPointIn->Y, CURVE25519_KEYSIZE);
        }
    }

    return status;
}

/* transform a point on Weierstrass curve to a point on Montgomery curve */
status_t LTC_PKHA_WeierstrassToCurve25519(
    const ltc_pkha_ecc_point_t *ltcPointIn, ltc_pkha_ecc_point_t *ltcPointOut)
{
    status_t status;
    uint16_t resultSize = 0;
    const uint8_t three = 0x03;

    status = LTC_PKHA_ModMul(LTC_BASE, &three, sizeof(three), ltcPointIn->X,
        CURVE25519_KEYSIZE, curve25519_modbin, CURVE25519_KEYSIZE,
        ltcPointOut->X, &resultSize, kLTC_PKHA_IntegerArith,
        kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
        kLTC_PKHA_TimingEqualized);

    if (status == kStatus_Success) {
        const uint8_t A[] = {0x06, 0x6d, 0x07};
        if (LTC_PKHA_CompareBigNum(ltcPointOut->X, resultSize, A, sizeof(A))) {
            status = LTC_PKHA_ModSub1(LTC_BASE, ltcPointOut->X, resultSize, A,
                sizeof(A), curve25519_modbin, CURVE25519_KEYSIZE,
                ltcPointOut->X, &resultSize);
        }
        else {
            status = LTC_PKHA_ModSub2(LTC_BASE, ltcPointOut->X, resultSize, A,
                sizeof(A), curve25519_modbin, CURVE25519_KEYSIZE,
                ltcPointOut->X, &resultSize);
        }
    }

    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, invThree, CURVE25519_KEYSIZE,
            ltcPointOut->X, resultSize, curve25519_modbin, CURVE25519_KEYSIZE,
            ltcPointOut->X, &resultSize, kLTC_PKHA_IntegerArith,
            kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
            kLTC_PKHA_TimingEqualized);
    }

    if (status == kStatus_Success) {
        if (ltcPointOut->Y != ltcPointIn->Y) {
            XMEMCPY(ltcPointOut->Y, ltcPointIn->Y, CURVE25519_KEYSIZE);
        }
    }

    return status;
}

/* Y = square root (X^3 + 486662*X^2 + X) */
status_t LTC_PKHA_Curve25519ComputeY(ltc_pkha_ecc_point_t *ltcPoint)
{
    uint8_t three = 3;
    const uint8_t A[] = {0x06, 0x6d, 0x07};
    uint8_t U[CURVE25519_KEYSIZE] = {0};
    uint8_t X2[CURVE25519_KEYSIZE] = {0};
    uint16_t sizeU = 0;
    uint16_t sizeX2 = 0;
    size_t szRes = 0;
    status_t status;

    /* X^3 */
    status = LTC_PKHA_ModExp(LTC_BASE, ltcPoint->X, CURVE25519_KEYSIZE,
        curve25519_modbin, CURVE25519_KEYSIZE, &three, 1, U, &sizeU,
        kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
        kLTC_PKHA_TimingEqualized);

    /* X^2 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, ltcPoint->X, CURVE25519_KEYSIZE,
            ltcPoint->X, CURVE25519_KEYSIZE, curve25519_modbin,
            CURVE25519_KEYSIZE, X2, &sizeX2, kLTC_PKHA_IntegerArith,
            kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
            kLTC_PKHA_TimingEqualized);
    }

    /* 486662*X^2 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, A, sizeof(A), X2, sizeX2,
            curve25519_modbin, CURVE25519_KEYSIZE, X2, &sizeX2,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* X^3 + 486662*X^2 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, U, sizeU, X2, sizeX2,
            curve25519_modbin, CURVE25519_KEYSIZE, U, &sizeU,
            kLTC_PKHA_IntegerArith);
    }

    /* U = X^3 + 486662*X^2 + X */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, U, sizeU, ltcPoint->X,
            CURVE25519_KEYSIZE, curve25519_modbin, CURVE25519_KEYSIZE, U,
            &sizeU, kLTC_PKHA_IntegerArith);
    }

    /* Y = modular square root of U (U is Y^2) */
    if (status == kStatus_Success) {
        status = LTC_PKHA_Prime25519SquareRootMod(U, sizeU, ltcPoint->Y,
            &szRes, 1);
    }

    return status;
}

/* Q = n*P */
/* if type is set, the input point p is in Montgomery curve coordinates,
    so there is a map to Weierstrass curve */
/* q output point is always in Montgomery curve coordinates */
int nxp_ltc_curve25519(ECPoint *q, const byte *n, const ECPoint *p,
    fsl_ltc_ecc_coordinate_system_t type)
{
    status_t status;
    ltc_pkha_ecc_point_t ltcPoint;
    ltc_pkha_ecc_point_t ltcPointOut;
    ECPoint pIn = {{0}};

    XMEMCPY(&pIn, p, sizeof(*p));
    ltcPoint.X = &pIn.point[0];
    ltcPoint.Y = &pIn.pointY[0];

    /* if input point P is on Curve25519 Montgomery curve, transform
        it to Weierstrass equivalent */
    if (type == kLTC_Curve25519) {
        LTC_PKHA_Curve25519ToWeierstrass(&ltcPoint, &ltcPoint);
    }

    ltcPointOut.X = &q->point[0];
    ltcPointOut.Y = &q->pointY[0];
    /* curve25519_modbin, r2mod, curve25519_aCurveParam, curve25519_bCurveParam
     * are Weierstrass equivalent with Curve25519 */
    status = LTC_PKHA_ECC_PointMul(LTC_BASE, &ltcPoint, n, CURVE25519_KEYSIZE,
        curve25519_modbin, r2mod, curve25519_aCurveParam,
        curve25519_bCurveParam, CURVE25519_KEYSIZE, kLTC_PKHA_TimingEqualized,
        kLTC_PKHA_IntegerArith, &ltcPointOut, NULL);

    /* now need to map from Weierstrass form to Montgomery form */
    if (status == kStatus_Success) {
        status = LTC_PKHA_WeierstrassToCurve25519(&ltcPointOut, &ltcPointOut);
    }

    return (status == kStatus_Success) ? 0 : IS_POINT_E;
}

#endif /* HAVE_CURVE25519 */


#ifdef HAVE_ED25519
/* a and d are Edwards curve parameters -1 and -121665/121666 prime is 2^255 - 19.
 *
 * https://en.wikipedia.org/wiki/Montgomery_curve#Equivalence_with_Edward_curves
 */

/* d parameter of ed25519 */
static const uint8_t d_coefEd25519[] = {
    0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75, 0xab, 0xd8, 0x41,
    0x41, 0x4d, 0x0a, 0x70, 0x00, 0x98, 0xe8, 0x79, 0x77, 0x79, 0x40,
    0xc7, 0x8c, 0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52};

/* Montgomery curve parameter A for a Montgomery curve equivalent with ed25519 */
static const uint8_t A_coefEd25519[] = {
    0x06, 0x6d, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Montgomery curve parameter B for a Montgomery curve equivalent with ed25519 */
static const uint8_t B_coefEd25519[] = {
    0xe5, 0x92, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};

/* these are pre-computed constants used in computations */

/* = 3*B */
static const uint8_t threeB_coefEd25519[] = {
    0xd5, 0xb8, 0xe9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};

/* = -A */
static const uint8_t minus_A_coefEd25519[] = {
    0xe7, 0x92, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};

/* = 1/B */
static const uint8_t invB_coefEd25519[] = {
0xc4, 0xa1, 0x29, 0x7b, 0x8d, 0x2c, 0x85, 0x22, 0xd5, 0x89, 0xaf,
    0xaf, 0x6c, 0xfd, 0xe3, 0xff, 0xd9, 0x85, 0x21, 0xa2, 0xe1, 0x2f,
    0xce, 0x1c, 0x63, 0x00, 0x24, 0x75, 0xc4, 0x24, 0x7f, 0x6b};

/* = 1/(3*B) */
static const uint8_t A_mul_invThreeB_coefEd25519[] = {
    0xb9, 0x3e, 0xe4, 0xad, 0xa1, 0x37, 0xa7, 0x93, 0x1c, 0xa4, 0x35,
    0xe0, 0x0c, 0x57, 0xbd, 0xaa, 0x6e, 0x51, 0x94, 0x3e, 0x14, 0xe0,
    0xcb, 0xec, 0xbd, 0xff, 0xe7, 0xb1, 0x27, 0x92, 0x00, 0x63};

/* Weierstrass curve parameter a for a Weierstrass curve equivalent with ed25519 */
static const uint8_t a_coefEd25519[] = {
    0x2d, 0x17, 0xbc, 0xf8, 0x8e, 0xe1, 0x71, 0xac, 0xf7, 0x2a, 0xa5,
    0x0c, 0x5d, 0xb6, 0xb8, 0x6b, 0xd6, 0x3d, 0x7b, 0x61, 0x0d, 0xe1,
    0x97, 0x31, 0xe6, 0xbe, 0xb9, 0xa5, 0xd3, 0xac, 0x4e, 0x5d};

/* Weierstrass curve parameter b for a Weierstrass curve equivalent with ed25519 */
static const uint8_t b_coefEd25519[] = {
    0xa4, 0xb2, 0x64, 0xf3, 0xc1, 0xeb, 0x04, 0x90, 0x32, 0xbc, 0x9f,
    0x6b, 0x97, 0x31, 0x48, 0xf5, 0xd5, 0x80, 0x57, 0x10, 0x06, 0xdb,
    0x0d, 0x55, 0xe0, 0xb3, 0xd0, 0xcf, 0x9b, 0xb2, 0x11, 0x1d};

/* Ed25519 basepoint B mapped to Weierstrass equivalent */
static uint8_t Wx_Ed25519[ED25519_KEY_SIZE] = {
    0x35, 0xef, 0x5a, 0x02, 0x9b, 0xc8, 0x55, 0xca, 0x9a, 0x7c, 0x61,
    0x0d, 0xdf, 0x3f, 0xc1, 0xa9, 0x18, 0x06, 0xc2, 0xf1, 0x02, 0x8f,
    0x0b, 0xf0, 0x39, 0x03, 0x2c, 0xd0, 0x0f, 0xdd, 0x78, 0x2a};
static uint8_t Wy_Ed25519[ED25519_KEY_SIZE] = {
    0x14, 0x1d, 0x2c, 0xf6, 0xf3, 0x30, 0x78, 0x9b, 0x65, 0x31, 0x71,
    0x80, 0x61, 0xd0, 0x6f, 0xcf, 0x23, 0x83, 0x79, 0x63, 0xa5, 0x3b,
    0x48, 0xbe, 0x2e, 0xa2, 0x1d, 0xc7, 0xa5, 0x44, 0xc6, 0x29};

static const ltc_pkha_ecc_point_t basepointEd25519 = {
    Wx_Ed25519, Wy_Ed25519,
};

const ltc_pkha_ecc_point_t *LTC_PKHA_Ed25519_BasePoint(void)
{
    return &basepointEd25519;
}

/* input point is on Weierstrass curve, typeOut determines the coordinates
    system of output point (either Weierstrass or Ed25519) */
status_t LTC_PKHA_Ed25519_PointMul(const ltc_pkha_ecc_point_t *ltcPointIn,
                                   const uint8_t *N,
                                   size_t sizeN,
                                   ltc_pkha_ecc_point_t *ltcPointOut,
                                   fsl_ltc_ecc_coordinate_system_t typeOut)
{
    uint16_t szN = (uint16_t)sizeN;
    status_t status;
    /* input on W, output in W, W parameters of ECC curve are Ed25519 curve
        parameters mapped to Weierstrass curve */
    status = LTC_PKHA_ECC_PointMul(LTC_BASE, ltcPointIn, N, szN,
        curve25519_modbin, r2mod, a_coefEd25519, b_coefEd25519,
        ED25519_KEY_SIZE, kLTC_PKHA_TimingEqualized, kLTC_PKHA_IntegerArith,
        ltcPointOut, NULL);

    /* Weierstrass coordinates to Ed25519 coordinates */
    if ((status == kStatus_Success) && (typeOut == kLTC_Ed25519)) {
        status = LTC_PKHA_WeierstrassToEd25519(ltcPointOut, ltcPointOut);
    }
    return status;
}

status_t LTC_PKHA_Ed25519ToWeierstrass(const ltc_pkha_ecc_point_t *ltcPointIn,
    ltc_pkha_ecc_point_t *ltcPointOut)
{
    status_t status;
    uint8_t Mx[ED25519_KEY_SIZE] = {0};
    uint8_t My[ED25519_KEY_SIZE] = {0};
    uint8_t temp[ED25519_KEY_SIZE] = {0};
    uint8_t temp2[ED25519_KEY_SIZE] = {0};
    const uint8_t max[32] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    const uint8_t *Ex;
    const uint8_t *Ey;
    uint8_t *Gx;
    uint8_t *Gy;
    uint16_t szMx = 0;
    uint16_t szGx = 0;
    uint16_t szMy = 0;
    uint16_t szGy = 0;
    uint16_t szTemp = 0;
    uint16_t szTemp2 = 0;
    uint8_t one = 1;

    Ex = ltcPointIn->X;
    Ey = ltcPointIn->Y;
    Gx = ltcPointOut->X;
    Gy = ltcPointOut->Y;
    /* # (Ex, Ey) on Ed (a_ed, d) to (x, y) on M (A,B)
    Mx = (1 + Ey) * ModularArithmetic.invert(1 - Ey, prime) % prime
    My = (1 + Ey) * ModularArithmetic.invert((1 - Ey)*Ex, prime) % prime */

    /* Gx = ((Mx * ModularArithmetic.invert(B, prime)) +
        (A * ModularArithmetic.invert(3*B, prime))) % prime
    Gy = (My * ModularArithmetic.invert(B, prime)) % prime */

    /* temp = 1 + Ey */
    status = LTC_PKHA_ModAdd(LTC_BASE, Ey, ED25519_KEY_SIZE, &one, sizeof(one),
        curve25519_modbin, sizeof(curve25519_modbin), temp, &szTemp,
        kLTC_PKHA_IntegerArith);

    /* temp2 = 1 - Ey = 1 + (p - Ey) */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModSub1(LTC_BASE, curve25519_modbin,
            sizeof(curve25519_modbin), Ey, ED25519_KEY_SIZE, max, sizeof(max),
            temp2, &szTemp2);
    }
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, temp2, szTemp2, &one, sizeof(one),
            curve25519_modbin, sizeof(curve25519_modbin), temp2, &szTemp2,
            kLTC_PKHA_IntegerArith);
    }

    /* Mx = ModInv(temp2,prime) */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModInv(LTC_BASE, temp2, szTemp2, curve25519_modbin,
            sizeof(curve25519_modbin), Mx, &szMx, kLTC_PKHA_IntegerArith);
    }

    /* Mx = Mx * temp */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, Mx, szMx, temp, szTemp,
            curve25519_modbin, ED25519_KEY_SIZE, Mx, &szMx,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* My = temp2 * Ex */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, Ex, ED25519_KEY_SIZE, temp2,
            szTemp2, curve25519_modbin, ED25519_KEY_SIZE, My, &szMy,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* My = ModInv(My, prime) */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModInv(LTC_BASE, My, szMy, curve25519_modbin,
            sizeof(curve25519_modbin), My, &szMy, kLTC_PKHA_IntegerArith);
    }
    /* My = My * temp */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, My, szMy, temp, szTemp,
            curve25519_modbin, ED25519_KEY_SIZE, My, &szMy,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* Gx = Mx * invB_coefEd25519 + A_mul_invThreeB_coefEd25519 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, Mx, szMx, invB_coefEd25519,
            sizeof(invB_coefEd25519), curve25519_modbin, ED25519_KEY_SIZE, Gx,
            &szGx, kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, Gx, szGx,
            A_mul_invThreeB_coefEd25519, sizeof(A_mul_invThreeB_coefEd25519),
            curve25519_modbin, sizeof(curve25519_modbin), Gx, &szGx,
            kLTC_PKHA_IntegerArith);
    }

    /* Gy = My * invB_coefEd25519 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, My, szMy, invB_coefEd25519,
            sizeof(invB_coefEd25519), curve25519_modbin, ED25519_KEY_SIZE, Gy,
            &szGy, kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    return status;
}

/*
# (Gx, Gy) on W to (Ex, Ey) on E
My = (B*Gy) % prime
Mx = ((3*B*Gx-A)*ModularArithmetic.invert(3, prime)) % prime
Ex = Mx*ModularArithmetic.invert(My, prime) % prime
Ey = (Mx - 1)*ModularArithmetic.invert(Mx + 1, prime) % prime
*/
status_t LTC_PKHA_WeierstrassToEd25519(const ltc_pkha_ecc_point_t *ltcPointIn,
    ltc_pkha_ecc_point_t *ltcPointOut)
{
    status_t status;
    uint8_t Mx[ED25519_KEY_SIZE] = {0};
    uint8_t My[ED25519_KEY_SIZE] = {0};
    uint8_t temp[ED25519_KEY_SIZE] = {0};
    const uint8_t *Gx;
    const uint8_t *Gy;
    uint8_t *Ex;
    uint8_t *Ey;
    uint16_t szMx = 0;
    uint16_t szEx = 0;
    uint16_t szMy = 0;
    uint16_t szEy = 0;
    uint16_t szTemp = 0;
    uint8_t one = 1;

    Gx = ltcPointIn->X;
    Gy = ltcPointIn->Y;
    Ex = ltcPointOut->X;
    Ey = ltcPointOut->Y;

    /* My = (B*Gy) % prime  */
    status = LTC_PKHA_ModMul(LTC_BASE, B_coefEd25519, sizeof(B_coefEd25519),
        Gy, ED25519_KEY_SIZE, curve25519_modbin, ED25519_KEY_SIZE, My, &szMy,
        kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
        kLTC_PKHA_TimingEqualized);

    /* temp = 3*B*Gx mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, threeB_coefEd25519,
            sizeof(threeB_coefEd25519), Gx, ED25519_KEY_SIZE, curve25519_modbin,
            ED25519_KEY_SIZE, temp, &szTemp, kLTC_PKHA_IntegerArith,
            kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
            kLTC_PKHA_TimingEqualized);
    }
    /* temp = (temp - A) mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, temp, szTemp, minus_A_coefEd25519,
            sizeof(minus_A_coefEd25519), curve25519_modbin,
            sizeof(curve25519_modbin), temp, &szTemp, kLTC_PKHA_IntegerArith);
    }
    /* Mx = (temp/3) mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, temp, szTemp, invThree,
            sizeof(invThree), curve25519_modbin, sizeof(curve25519_modbin), Mx,
             &szMx, kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
             kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }
    /* temp = 1/My mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModInv(LTC_BASE, My, szMy, curve25519_modbin,
            sizeof(curve25519_modbin), temp, &szTemp, kLTC_PKHA_IntegerArith);
    }
    /* Ex = Mx * temp mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, temp, szTemp, Mx, szMx,
            curve25519_modbin, sizeof(curve25519_modbin), Ex, &szEx,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    /* temp = Mx + 1 mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, Mx, szMx, &one, sizeof(one),
            curve25519_modbin, sizeof(curve25519_modbin), temp, &szTemp,
            kLTC_PKHA_IntegerArith);
    }
    /* temp = 1/temp mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModInv(LTC_BASE, temp, szTemp, curve25519_modbin,
            sizeof(curve25519_modbin), temp, &szTemp, kLTC_PKHA_IntegerArith);
    }
    /* Mx = (Mx - 1) mod p */
    if (status == kStatus_Success) {
        if (LTC_PKHA_CompareBigNum(Mx, szMx, &one, sizeof(one)) >= 0) {
            status = LTC_PKHA_ModSub1(LTC_BASE, Mx, szMx, &one, sizeof(one),
                curve25519_modbin, sizeof(curve25519_modbin), Mx, &szMx);
        }
        else {
            /* Mx is zero, so it is modulus, thus we do modulus - 1 */
            XMEMCPY(Mx, curve25519_modbin, sizeof(curve25519_modbin));
            Mx[0]--;
        }
    }
    /* Ey = Mx * temp mod p */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, temp, szTemp, Mx, szMx,
            curve25519_modbin, sizeof(curve25519_modbin), Ey, &szEy,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    return status;
}

status_t LTC_PKHA_Ed25519_PointDecompress(const uint8_t *pubkey,
    size_t pubKeySize, ltc_pkha_ecc_point_t *ltcPointOut)
{
    status_t status;
    const uint8_t one = 1;

    /* pubkey contains the Y coordinate and a sign of X
     */

    /* x^2 = ((y^2 - 1) / (d*y^2 +1)) mod p */

    /* decode Y from pubkey */
    XMEMCPY(ltcPointOut->Y, pubkey, pubKeySize);
    ltcPointOut->Y[pubKeySize - 1] &= ~0x80u;
    int sign = (int)(bool)(pubkey[pubKeySize - 1] & 0x80u);

    uint8_t U[ED25519_KEY_SIZE] = {0};
    uint8_t V[ED25519_KEY_SIZE] = {0};
    uint8_t *X = ltcPointOut->X;
    uint8_t *Y = ltcPointOut->Y;
    uint16_t szU = 0;
    uint16_t szV = 0;
    size_t szRes = 0;

    /* decode X from pubkey */

    /* U = y * y mod p */
    status = LTC_PKHA_ModMul(LTC_BASE, Y, ED25519_KEY_SIZE, Y,
        ED25519_KEY_SIZE, curve25519_modbin, ED25519_KEY_SIZE, U, &szU,
        kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
        kLTC_PKHA_TimingEqualized);
    XMEMCPY(V, U, szU);
    szV = szU;

    /* U = U - 1 = y^2 - 1 */
    if (status == kStatus_Success) {
        if (LTC_PKHA_CompareBigNum(U, szU, &one, sizeof(one)) >= 0) {
            status = LTC_PKHA_ModSub1(LTC_BASE, U, szU, &one, sizeof(one),
                curve25519_modbin, sizeof(curve25519_modbin), U, &szU);
        }
        else {
            /* U is zero, so it is modulus, thus we do modulus - 1 */
            XMEMCPY(U, curve25519_modbin, sizeof(curve25519_modbin));
            U[0]--;
        }
    }

    /* V = d*y*y + 1 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, V, szV, d_coefEd25519,
            ED25519_KEY_SIZE, curve25519_modbin, ED25519_KEY_SIZE, V, &szV,
            kLTC_PKHA_IntegerArith, kLTC_PKHA_NormalValue,
            kLTC_PKHA_NormalValue, kLTC_PKHA_TimingEqualized);
    }

    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, V, szV, &one, sizeof(one),
            curve25519_modbin, sizeof(curve25519_modbin), V, &szV,
            kLTC_PKHA_IntegerArith);
    }

    /* U = U / V (mod p) */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModInv(LTC_BASE, V, szV, curve25519_modbin,
            sizeof(curve25519_modbin), V, &szV, kLTC_PKHA_IntegerArith);
    }
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, V, szV, U, szU, curve25519_modbin,
            ED25519_KEY_SIZE, U, &szU, kLTC_PKHA_IntegerArith,
            kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
            kLTC_PKHA_TimingEqualized);
    }

    /* get square root */
    if (status == kStatus_Success) {
        status = LTC_PKHA_Prime25519SquareRootMod(U, szU, X, &szRes, sign);
    }

    return status;
}

/* LSByte first of Ed25519 parameter l = 2^252 + 
 *   27742317777372353535851937790883648493 */
static const uint8_t l_coefEdDSA[] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
    0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/
status_t LTC_PKHA_sc_reduce(uint8_t *a)
{
    uint16_t szA = 0;
    return LTC_PKHA_ModRed(LTC_BASE, a, 64, l_coefEdDSA, sizeof(l_coefEdDSA),
        a, &szA, kLTC_PKHA_IntegerArith);
}

/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/
status_t LTC_PKHA_sc_muladd(uint8_t *s, const uint8_t *a,
    const uint8_t *b, const uint8_t *c)
{
    uint16_t szS = 0;
    uint16_t szB = 0;
    uint8_t tempB[32] = {0};
    status_t status;

    /* Assume only b can be larger than modulus. It is called durind
     * wc_ed25519_sign_msg() where hram (=a) and nonce(=c)
     * have been reduced by LTC_PKHA_sc_reduce()
     * Thus reducing b only.
     */
    status = LTC_PKHA_ModRed(LTC_BASE, b, 32, l_coefEdDSA, sizeof(l_coefEdDSA),
        tempB, &szB, kLTC_PKHA_IntegerArith);

    if (status == kStatus_Success) {
        status = LTC_PKHA_ModMul(LTC_BASE, a, 32, tempB, szB, l_coefEdDSA,
            sizeof(l_coefEdDSA), s, &szS, kLTC_PKHA_IntegerArith,
            kLTC_PKHA_NormalValue, kLTC_PKHA_NormalValue,
            kLTC_PKHA_TimingEqualized);
    }

    if (status == kStatus_Success) {
        status = LTC_PKHA_ModAdd(LTC_BASE, s, szS, c, 32, l_coefEdDSA, 32, s,
            &szS, kLTC_PKHA_IntegerArith);
    }

    return status;
}

/*
r = a * A + b * B
where A is public key point, B is basepoint
where a = a[0]+256*a[1]+...+256^31 a[31].
and b = b[0]+256*b[1]+...+256^31 b[31].
B is the Ed25519 base point (x,4/5) with x positive.
*/
status_t LTC_PKHA_SignatureForVerify(uint8_t *rcheck, const unsigned char *a,
    const unsigned char *b, ed25519_key *key)
{
    /* To verify a signature on a message M, first split the signature
       into two 32-octet halves.  Decode the first half as a point R,
       and the second half as an integer s, in the range 0 <= s < q.  If
       the decoding fails, the signature is invalid. */

    /* Check the group equation 8s B = 8 R + 8k A. */

    /*
       Uses a fast single-signature verification SB = R + H(R,A,M)A becomes
       SB - H(R,A,M)A saving decompression of R
    */
    uint8_t X0[ED25519_PUB_KEY_SIZE] = {0};
    uint8_t X1[ED25519_PUB_KEY_SIZE] = {0};
    uint8_t Y0[ED25519_PUB_KEY_SIZE] = {0};
    uint8_t Y1[ED25519_PUB_KEY_SIZE] = {0};
    const uint8_t max[32] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    ltc_pkha_ecc_point_t ltc0;
    ltc_pkha_ecc_point_t ltc1;
    ltc_pkha_ecc_point_t pubKey;
    status_t status;

    /* The equality for the negative of a point P, in affine coordinates,
        is -P = -(x,y) = (x, -y) */
    uint16_t szY = 32;

    ltc0.X = X0;
    ltc1.X = X1;
    ltc0.Y = Y0;
    ltc1.Y = Y1;
    pubKey.X = key->pointX;
    pubKey.Y = key->pointY;

    /* ltc0 = b*B */
    status = LTC_PKHA_Ed25519_PointMul(LTC_PKHA_Ed25519_BasePoint(), b,
        ED25519_KEY_SIZE, &ltc0, kLTC_Weierstrass /* result in W */);

    /* ltc1 = a*A */
    if (status == kStatus_Success) {
        status = LTC_PKHA_Ed25519ToWeierstrass(&pubKey, &ltc1);
    }
    if (status == kStatus_Success) {
        status = LTC_PKHA_Ed25519_PointMul(&ltc1, a, ED25519_KEY_SIZE, &ltc1,
            kLTC_Weierstrass /* result in W */);
    }

    /* R = b*B - a*A */
    if (status == kStatus_Success) {
        status = LTC_PKHA_ModSub1(LTC_BASE, curve25519_modbin,
            sizeof(curve25519_modbin), ltc1.Y, szY, max, sizeof(max), ltc1.Y,
            &szY);
    }
    if (status == kStatus_Success) {
        status = LTC_PKHA_ECC_PointAdd(LTC_BASE, &ltc0, &ltc1,
            curve25519_modbin, r2mod, a_coefEd25519, b_coefEd25519,
            ED25519_KEY_SIZE, kLTC_PKHA_IntegerArith, &ltc0);
    }
    /* map to Ed25519 */
    if (status == kStatus_Success) {
        status = LTC_PKHA_WeierstrassToEd25519(&ltc0, &ltc0);
    }

    if (status == kStatus_Success) {
        if (((uint32_t)ltc0.X[0]) & 0x01u) {
            ltc0.Y[ED25519_KEY_SIZE - 1] |= 0x80u;
        }

        XMEMCPY(rcheck, ltc0.Y, ED25519_KEY_SIZE);
    }

    return status;
}

status_t LTC_PKHA_Ed25519_Compress(const ltc_pkha_ecc_point_t *ltcPointIn,
    uint8_t *p)
{
    /* compress */
    /* get sign of X per https://tools.ietf.org/html/draft-josefsson-eddsa-ed25519-02
     * To form the encoding of the point, copy the least
       significant bit of the x-coordinate to the most significant bit of
       the final octet
     */
    XMEMCPY(p, ltcPointIn->Y, ED25519_KEY_SIZE);
    if (((uint32_t)ltcPointIn->X[0]) & 0x01u) {
        p[ED25519_KEY_SIZE - 1] |= 0x80u;
    }
    return kStatus_Success;
}

#endif /* HAVE_ED25519 */
#endif /* FREESCALE_LTC_ECC */


#undef ERROR_OUT

#endif /* FREESCALE_LTC_TFM || FREESCALE_LTC_ECC */
