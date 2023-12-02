/* dsa.c
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

#ifndef NO_DSA

#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/dsa.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef _MSC_VER
    /* disable for while(0) cases (MSVC bug) */
    #pragma warning(disable:4127)
#endif

int wc_InitDsaKey(DsaKey* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    key->type = -1;  /* haven't decided yet */
    key->heap = NULL;

    return mp_init_multi(
        /* public  alloc parts */
        &key->p,
        &key->q,
        &key->g,
        &key->y,

        /* private alloc parts */
        &key->x,
        NULL
    );
}


int wc_InitDsaKey_h(DsaKey* key, void* h)
{
    int ret = wc_InitDsaKey(key);
    if (ret == 0)
        key->heap = h;

    return ret;
}


void wc_FreeDsaKey(DsaKey* key)
{
    if (key == NULL)
        return;

    if (key->type == DSA_PRIVATE)
        mp_forcezero(&key->x);

    mp_clear(&key->x);
    mp_clear(&key->y);
    mp_clear(&key->g);
    mp_clear(&key->q);
    mp_clear(&key->p);
}


/* validate that (L,N) match allowed sizes from FIPS 186-4, Section 4.2.
 * modLen - represents L, the size of p (prime modulus) in bits
 * divLen - represents N, the size of q (prime divisor) in bits
 * return 0 on success, -1 on error */
static int CheckDsaLN(int modLen, int divLen)
{
    int ret = -1;

    switch (modLen) {
#ifdef WOLFSSL_DSA_768_MODULUS
        case 768:
#endif
        case 1024:
            if (divLen == 160)
                ret = 0;
            break;
        case 2048:
            if (divLen == 224 || divLen == 256)
                ret = 0;
            break;
        case 3072:
            if (divLen == 256)
                ret = 0;
            break;
        default:
            break;
    }

    return ret;
}


#ifdef WOLFSSL_KEY_GEN

/* Create DSA key pair (&dsa->x, &dsa->y)
 *
 * Based on NIST FIPS 186-4,
 * "B.1.1 Key Pair Generation Using Extra Random Bits"
 *
 * rng - pointer to initialized WC_RNG structure
 * dsa - pointer to initialized DsaKey structure, will hold generated key
 *
 * return 0 on success, negative on error */
int wc_MakeDsaKey(WC_RNG *rng, DsaKey *dsa)
{
    byte* cBuf;
    int qSz, pSz, cSz, err;
#ifdef WOLFSSL_SMALL_STACK
    mp_int *tmpQ = NULL;
#else
    mp_int tmpQ[1];
#endif

    if (rng == NULL || dsa == NULL)
        return BAD_FUNC_ARG;

    qSz = mp_unsigned_bin_size(&dsa->q);
    pSz = mp_unsigned_bin_size(&dsa->p);

    /* verify (L,N) pair bit lengths */
    if (CheckDsaLN(pSz * WOLFSSL_BIT_SIZE, qSz * WOLFSSL_BIT_SIZE) != 0)
        return BAD_FUNC_ARG;

    /* generate extra 64 bits so that bias from mod function is negligible */
    cSz = qSz + (64 / WOLFSSL_BIT_SIZE);
    cBuf = (byte*)XMALLOC((size_t)cSz, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (cBuf == NULL) {
        return MEMORY_E;
    }

    SAVE_VECTOR_REGISTERS();

#ifdef WOLFSSL_SMALL_STACK
    if ((tmpQ = (mp_int *)XMALLOC(sizeof(*tmpQ), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL)
        err = MEMORY_E;
    else
        err = MP_OKAY;

    if (err == MP_OKAY)
#endif
        err = mp_init_multi(&dsa->x, &dsa->y, tmpQ, NULL, NULL, NULL);

    if (err == MP_OKAY) {
        do {
            /* Generate N+64 bits (c) from RNG into &dsa->x, making sure
             * result is positive.
             * Hash_DRBG uses SHA-256 which matches maximum
             * requested_security_strength of (L,N).
             */
            err = wc_RNG_GenerateBlock(rng, cBuf, (word32)cSz);
            if (err != MP_OKAY)
                break;
            err = mp_read_unsigned_bin(&dsa->x, cBuf, (word32)cSz);
            if (err != MP_OKAY)
                break;
        } while (mp_cmp_d(&dsa->x, 1) != MP_GT);
    }

    /* tmpQ = q - 1 */
    if (err == MP_OKAY)
        err = mp_copy(&dsa->q, tmpQ);

    if (err == MP_OKAY)
        err = mp_sub_d(tmpQ, 1, tmpQ);

    /* x = c mod (q-1), &dsa->x holds c */
    if (err == MP_OKAY)
        err = mp_mod(&dsa->x, tmpQ, &dsa->x);

    /* x = c mod (q-1) + 1 */
    if (err == MP_OKAY)
        err = mp_add_d(&dsa->x, 1, &dsa->x);

    /* public key : y = g^x mod p */
    if (err == MP_OKAY) {
        err = mp_exptmod_ex(&dsa->g, &dsa->x, (int)dsa->q.used, &dsa->p,
            &dsa->y);
    }

    if (err == MP_OKAY)
        dsa->type = DSA_PRIVATE;

    if (err != MP_OKAY) {
        mp_clear(&dsa->x);
        mp_clear(&dsa->y);
    }

    XFREE(cBuf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);

#ifdef WOLFSSL_SMALL_STACK
    if (tmpQ != NULL) {
        mp_clear(tmpQ);
        XFREE(tmpQ, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#else
    mp_clear(tmpQ);
#endif

    RESTORE_VECTOR_REGISTERS();

    return err;
}


/* modulus_size in bits */
int wc_MakeDsaParameters(WC_RNG *rng, int modulus_size, DsaKey *dsa)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *tmp = NULL, *tmp2 = NULL;
#else
    mp_int tmp[1], tmp2[1];
#endif
    int     err, msize, qsize,
            loop_check_prime = 0,
            check_prime = MP_NO;
    unsigned char   *buf;

    if (rng == NULL || dsa == NULL)
        return BAD_FUNC_ARG;

    /* set group size in bytes from modulus size
     * FIPS 186-4 defines valid values (1024, 160) (2048, 256) (3072, 256)
     */
    switch (modulus_size) {
#ifdef WOLFSSL_DSA_768_MODULUS
    /* This key length is insecure and only included for bind 9 testing */
        case 768:
#endif
        case 1024:
            qsize = 20;
            break;
        case 2048:
        case 3072:
            qsize = 32;
            break;
        default:
            return BAD_FUNC_ARG;
    }

    /* modulus size in bytes */
    msize = modulus_size / WOLFSSL_BIT_SIZE;

    /* allocate ram */
    buf = (unsigned char *)XMALLOC((size_t)(msize - qsize),
                                   dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL) {
        return MEMORY_E;
    }

    /* make a random string that will be multiplied against q */
    err = wc_RNG_GenerateBlock(rng, buf, (word32)(msize - qsize));
    if (err != MP_OKAY) {
        XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return err;
    }

    /* force magnitude */
    buf[0] |= 0xC0;

    /* force even */
    buf[msize - qsize - 1] &= (unsigned char)~1;

#ifdef WOLFSSL_SMALL_STACK
    if (((tmp = (mp_int *)XMALLOC(sizeof(*tmp), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL) ||
        ((tmp2 = (mp_int *)XMALLOC(sizeof(*tmp2), NULL, DYNAMIC_TYPE_WOLF_BIGINT)) == NULL))
        err = MEMORY_E;
    else
        err = MP_OKAY;

    if (err == MP_OKAY)
#endif
        err = mp_init_multi(tmp, tmp2, &dsa->p, &dsa->q, 0, 0);

    if (err == MP_OKAY)
        err = mp_read_unsigned_bin(tmp2, buf, (word32)(msize - qsize));

    /* make our prime q */
    if (err == MP_OKAY)
        err = mp_rand_prime(&dsa->q, qsize, rng, NULL);

    /* p = random * q */
    if (err == MP_OKAY)
        err = mp_mul(&dsa->q, tmp2, &dsa->p);

    /* p = random * q + 1, so q is a prime divisor of p-1 */
    if (err == MP_OKAY)
        err = mp_add_d(&dsa->p, 1, &dsa->p);

    /* tmp = 2q  */
    if (err == MP_OKAY)
        err = mp_add(&dsa->q, &dsa->q, tmp);

    if (err == MP_OKAY) {
        /* loop until p is prime */
        while (check_prime == MP_NO) {
            err = mp_prime_is_prime_ex(&dsa->p, 8, &check_prime, rng);
            if (err != MP_OKAY)
                break;
            if (check_prime != MP_YES) {
                /* p += 2q */
                err = mp_add(tmp, &dsa->p, &dsa->p);
                if (err != MP_OKAY)
                    break;
                loop_check_prime++;
            }
        }
    }

    /* tmp2 += (2*loop_check_prime)
     * to have p = (q * tmp2) + 1 prime
     */
    if (err == MP_OKAY) {
        if (loop_check_prime)
            err = mp_add_d(tmp2, 2 * (mp_digit)loop_check_prime, tmp2);
    }

    if (err == MP_OKAY)
        err = mp_init(&dsa->g);

    /* find a value g for which g^tmp2 != 1 */
    if (err == MP_OKAY)
        err = mp_set(&dsa->g, 1);

    if (err == MP_OKAY) {
        do {
            err = mp_add_d(&dsa->g, 1, &dsa->g);
            if (err != MP_OKAY)
                break;
            err = mp_exptmod(&dsa->g, tmp2, &dsa->p, tmp);
            if (err != MP_OKAY)
                break;
        } while (mp_cmp_d(tmp, 1) == MP_EQ);
    }

    /* at this point tmp generates a group of order q mod p */
    if (err == MP_OKAY) {
#ifndef USE_FAST_MATH
        /* Exchanging is quick when the data pointer can be copied. */
        err = mp_exch(tmp, &dsa->g);
#else
        err = mp_copy(tmp, &dsa->g);
#endif
    }

    XFREE(buf, dsa->heap, DYNAMIC_TYPE_TMP_BUFFER);

#ifdef WOLFSSL_SMALL_STACK
    if (tmp != NULL) {
        mp_clear(tmp);
        XFREE(tmp, NULL, DYNAMIC_TYPE_WOLF_BIGINT);
    }
    if (tmp2 != NULL) {
        mp_clear(tmp2);
        XFREE(tmp2, NULL, DYNAMIC_TYPE_WOLF_BIGINT);
    }
#else
    mp_clear(tmp);
    mp_clear(tmp2);
#endif
    if (err != MP_OKAY) {
        mp_clear(&dsa->q);
        mp_clear(&dsa->p);
        mp_clear(&dsa->g);
    }

    return err;
}
#endif /* WOLFSSL_KEY_GEN */


static int _DsaImportParamsRaw(DsaKey* dsa, const char* p, const char* q,
                          const char* g, int trusted, WC_RNG* rng)
{
    int err;
    int pSz, qSz;

    if (dsa == NULL || p == NULL || q == NULL || g == NULL)
        return BAD_FUNC_ARG;

    /* read p */
    err = mp_read_radix(&dsa->p, p, MP_RADIX_HEX);
    if (err == MP_OKAY && !trusted) {
        int isPrime = 1;
        if (rng == NULL)
            err = mp_prime_is_prime(&dsa->p, 8, &isPrime);
        else
            err = mp_prime_is_prime_ex(&dsa->p, 8, &isPrime, rng);

        if (err == MP_OKAY) {
            if (!isPrime)
                err = DH_CHECK_PUB_E;
        }
    }

    /* read q */
    if (err == MP_OKAY)
        err = mp_read_radix(&dsa->q, q, MP_RADIX_HEX);

    /* read g */
    if (err == MP_OKAY)
        err = mp_read_radix(&dsa->g, g, MP_RADIX_HEX);

    /* verify (L,N) pair bit lengths */
    pSz = mp_unsigned_bin_size(&dsa->p);
    qSz = mp_unsigned_bin_size(&dsa->q);

    if (CheckDsaLN(pSz * WOLFSSL_BIT_SIZE, qSz * WOLFSSL_BIT_SIZE) != 0) {
        WOLFSSL_MSG("Invalid DSA p or q parameter size");
        err = BAD_FUNC_ARG;
    }

    if (err != MP_OKAY) {
        mp_clear(&dsa->p);
        mp_clear(&dsa->q);
        mp_clear(&dsa->g);
    }

    return err;
}


/* Import raw DSA parameters into DsaKey structure for use with wc_MakeDsaKey(),
 * input parameters (p,q,g) should be represented as ASCII hex values.
 *
 * dsa  - pointer to initialized DsaKey structure
 * p    - DSA (p) parameter, ASCII hex string
 * pSz  - length of p
 * q    - DSA (q) parameter, ASCII hex string
 * qSz  - length of q
 * g    - DSA (g) parameter, ASCII hex string
 * gSz  - length of g
 *
 * returns 0 on success, negative upon failure
 */
int wc_DsaImportParamsRaw(DsaKey* dsa, const char* p, const char* q,
                          const char* g)
{
    return _DsaImportParamsRaw(dsa, p, q, g, 1, NULL);
}


/* Import raw DSA parameters into DsaKey structure for use with wc_MakeDsaKey(),
 * input parameters (p,q,g) should be represented as ASCII hex values. Check
 * that the p value is probably prime.
 *
 * dsa  - pointer to initialized DsaKey structure
 * p    - DSA (p) parameter, ASCII hex string
 * pSz  - length of p
 * q    - DSA (q) parameter, ASCII hex string
 * qSz  - length of q
 * g    - DSA (g) parameter, ASCII hex string
 * gSz  - length of g
 * trusted - trust that p is OK
 * rng  - random number generator for the prime test
 *
 * returns 0 on success, negative upon failure
 */
int wc_DsaImportParamsRawCheck(DsaKey* dsa, const char* p, const char* q,
                          const char* g, int trusted, WC_RNG* rng)
{
    return _DsaImportParamsRaw(dsa, p, q, g, trusted, rng);
}


/* Export raw DSA parameters from DsaKey structure
 *
 * dsa  - pointer to initialized DsaKey structure
 * p    - output location for DSA (p) parameter
 * pSz  - [IN/OUT] size of output buffer for p, size of p
 * q    - output location for DSA (q) parameter
 * qSz  - [IN/OUT] size of output buffer for q, size of q
 * g    - output location for DSA (g) parameter
 * gSz  - [IN/OUT] size of output buffer for g, size of g
 *
 * If p, q, and g pointers are all passed in as NULL, the function
 * will set pSz, qSz, and gSz to the required output buffer sizes for p,
 * q, and g. In this case, the function will return LENGTH_ONLY_E.
 *
 * returns 0 on success, negative upon failure
 */
int wc_DsaExportParamsRaw(DsaKey* dsa, byte* p, word32* pSz,
                          byte* q, word32* qSz, byte* g, word32* gSz)
{
    int err;
    word32 pLen, qLen, gLen;

    if (dsa == NULL || pSz == NULL || qSz == NULL || gSz == NULL)
        return BAD_FUNC_ARG;

    /* get required output buffer sizes */
    pLen = (word32)mp_unsigned_bin_size(&dsa->p);
    qLen = (word32)mp_unsigned_bin_size(&dsa->q);
    gLen = (word32)mp_unsigned_bin_size(&dsa->g);

    /* return buffer sizes and LENGTH_ONLY_E if buffers are NULL */
    if (p == NULL && q == NULL && g == NULL) {
        *pSz = pLen;
        *qSz = qLen;
        *gSz = gLen;
        return LENGTH_ONLY_E;
    }

    if (p == NULL || q == NULL || g == NULL)
        return BAD_FUNC_ARG;

    /* export p */
    if (*pSz < pLen) {
        WOLFSSL_MSG("Output buffer for DSA p parameter too small, "
                    "required size placed into pSz");
        *pSz = pLen;
        return BUFFER_E;
    }
    *pSz = pLen;
    err = mp_to_unsigned_bin(&dsa->p, p);

    /* export q */
    if (err == MP_OKAY) {
        if (*qSz < qLen) {
            WOLFSSL_MSG("Output buffer for DSA q parameter too small, "
                        "required size placed into qSz");
            *qSz = qLen;
            return BUFFER_E;
        }
        *qSz = qLen;
        err = mp_to_unsigned_bin(&dsa->q, q);
    }

    /* export g */
    if (err == MP_OKAY) {
        if (*gSz < gLen) {
            WOLFSSL_MSG("Output buffer for DSA g parameter too small, "
                        "required size placed into gSz");
            *gSz = gLen;
            return BUFFER_E;
        }
        *gSz = gLen;
        err = mp_to_unsigned_bin(&dsa->g, g);
    }

    return err;
}


/* Export raw DSA key (x, y) from DsaKey structure
 *
 * dsa  - pointer to initialized DsaKey structure
 * x    - output location for private key
 * xSz  - [IN/OUT] size of output buffer for x, size of x
 * y    - output location for public key
 * ySz  - [IN/OUT] size of output buffer for y, size of y
 *
 * If x and y pointers are all passed in as NULL, the function
 * will set xSz and ySz to the required output buffer sizes for x
 * and y. In this case, the function will return LENGTH_ONLY_E.
 *
 * returns 0 on success, negative upon failure
 */
int wc_DsaExportKeyRaw(DsaKey* dsa, byte* x, word32* xSz, byte* y, word32* ySz)
{
    int err;
    word32 xLen, yLen;

    if (dsa == NULL || xSz == NULL || ySz == NULL)
        return BAD_FUNC_ARG;

    /* get required output buffer sizes */
    xLen = (word32)mp_unsigned_bin_size(&dsa->x);
    yLen = (word32)mp_unsigned_bin_size(&dsa->y);

    /* return buffer sizes and LENGTH_ONLY_E if buffers are NULL */
    if (x == NULL && y == NULL) {
        *xSz = xLen;
        *ySz = yLen;
        return LENGTH_ONLY_E;
    }

    if (x == NULL || y == NULL)
        return BAD_FUNC_ARG;

    /* export x */
    if (*xSz < xLen) {
        WOLFSSL_MSG("Output buffer for DSA private key (x) too small, "
                    "required size placed into xSz");
        *xSz = xLen;
        return BUFFER_E;
    }
    *xSz = xLen;
    err = mp_to_unsigned_bin(&dsa->x, x);

    /* export y */
    if (err == MP_OKAY) {
        if (*ySz < yLen) {
            WOLFSSL_MSG("Output buffer to DSA public key (y) too small, "
                        "required size placed into ySz");
            *ySz = yLen;
            return BUFFER_E;
        }
        *ySz = yLen;
        err = mp_to_unsigned_bin(&dsa->y, y);
    }

    return err;
}

int wc_DsaSign(const byte* digest, byte* out, DsaKey* key, WC_RNG* rng)
{
    /* use sha1 by default for backwards compatibility */
    return wc_DsaSign_ex(digest, WC_SHA_DIGEST_SIZE, out, key, rng);
}

int wc_DsaSign_ex(const byte* digest, word32 digestSz, byte* out, DsaKey* key,
    WC_RNG* rng)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int  *k = NULL;
    mp_int  *kInv = NULL;
    mp_int  *r = NULL;
    mp_int  *s = NULL;
    mp_int  *H = NULL;
#ifndef WOLFSSL_MP_INVMOD_CONSTANT_TIME
    mp_int  *b = NULL;
#endif
    byte    *buffer = NULL;
#else
    mp_int  k[1], kInv[1], r[1], s[1], H[1];
#ifndef WOLFSSL_MP_INVMOD_CONSTANT_TIME
    mp_int  b[1];
#endif
    byte    buffer[DSA_MAX_HALF_SIZE];
#endif
    mp_int* qMinus1;
    int     ret = 0;
    word32  halfSz = 0;

    if (digest == NULL || out == NULL || key == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    do {
#ifdef WOLFSSL_SMALL_STACK
        k = (mp_int *)XMALLOC(sizeof *k, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        kInv = (mp_int *)XMALLOC(sizeof *kInv, key->heap,
                                 DYNAMIC_TYPE_TMP_BUFFER);
        r = (mp_int *)XMALLOC(sizeof *r, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        s = (mp_int *)XMALLOC(sizeof *s, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        H = (mp_int *)XMALLOC(sizeof *H, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
#ifndef WOLFSSL_MP_INVMOD_CONSTANT_TIME
        b = (mp_int *)XMALLOC(sizeof *b, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        buffer = (byte *)XMALLOC(DSA_MAX_HALF_SIZE, key->heap,
                                 DYNAMIC_TYPE_TMP_BUFFER);

        if ((k == NULL) ||
            (kInv == NULL) ||
            (r == NULL) ||
            (s == NULL) ||
            (H == NULL)
#ifndef WOLFSSL_MP_INVMOD_CONSTANT_TIME
            || (b == NULL)
#endif
            || (buffer == NULL)) {
            ret = MEMORY_E;
            break;
        }
#endif

#ifdef WOLFSSL_MP_INVMOD_CONSTANT_TIME
        if (mp_init_multi(k, kInv, r, s, H, 0) != MP_OKAY)
#else
            if (mp_init_multi(k, kInv, r, s, H, b) != MP_OKAY)
#endif
                {
                    ret = MP_INIT_E;
                    break;
                }

        halfSz = min(DSA_MAX_HALF_SIZE, (word32)mp_unsigned_bin_size(&key->q));
        /* NIST FIPS 186-4: Sections 4.1
         * q is a prime divisor where 2^(N-1) < q < 2^N and N is the bit length
         * of q.
         * To satisfy this constraint if N is 0 then q would still need to be
         * larger than 0.5, but since there is 0 bits in q it can not be any
         * value.
         */
        if (halfSz == 0) {
            ret = BAD_FUNC_ARG;
            break;
        }

        qMinus1 = kInv;

        /* NIST FIPS 186-4: B.2.2
         * Per-Message Secret Number Generation by Testing Candidates
         * Generate k in range [1, q-1].
         *   Check that k is less than q-1: range [0, q-2].
         *   Add 1 to k: range [1, q-1].
         */
        if (mp_sub_d(&key->q, 1, qMinus1)) {
            ret = MP_SUB_E;
            break;
        }

        /* if q-1 is 0 or smaller, k will never end up being less than it */
        if (mp_iszero(qMinus1) || mp_isneg(qMinus1)) {
            ret = BAD_FUNC_ARG;
            break;
        }

        do {
            /* Step 4: generate k */
            if ((ret = wc_RNG_GenerateBlock(rng, buffer, halfSz))) {
                break;
            }

            /* Step 5 */
            if (mp_read_unsigned_bin(k, buffer, halfSz) != MP_OKAY) {
                ret = MP_READ_E;
                break;
            }

            /* k is a random number and it should be less than q-1
             * if k greater than repeat
             */
            /* Step 6 */
        } while (mp_cmp(k, qMinus1) != MP_LT);

        if (ret != 0)
            break;

        /* Step 7 */
        if (mp_add_d(k, 1, k) != MP_OKAY) {
            ret = MP_MOD_E;
            break;
        }

#ifdef WOLFSSL_MP_INVMOD_CONSTANT_TIME
        /* inverse k mod q */
        if (mp_invmod(k, &key->q, kInv) != MP_OKAY) {
            ret = MP_INVMOD_E;
            break;
        }

        /* generate r, r = (g exp k mod p) mod q */
        if (mp_exptmod_ex(&key->g, k, key->q.used, &key->p, r) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
            break;
        }

        if (mp_mod(r, &key->q, r) != MP_OKAY) {
            ret = MP_MOD_E;
            break;
        }

        /* generate H from sha digest */
        if (mp_read_unsigned_bin(H, digest, digestSz) != MP_OKAY) {
            ret = MP_READ_E;
            break;
        }

        /* generate s, s = (kInv * (H + x*r)) % q */
        if (mp_mul(&key->x, r, s) != MP_OKAY) {
            ret = MP_MUL_E;
            break;
        }

        if (mp_add(s, H, s) != MP_OKAY) {
            ret = MP_ADD_E;
            break;
        }

        if (mp_mulmod(s, kInv, &key->q, s) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }
#else
        /* Blinding value
         * Generate b in range [1, q-1].
         */
        do {
            if ((ret = wc_RNG_GenerateBlock(rng, buffer, halfSz))) {
                break;
            }
            if (mp_read_unsigned_bin(b, buffer, halfSz) != MP_OKAY) {
                ret = MP_READ_E;
                break;
            }
        } while (mp_cmp(b, qMinus1) != MP_LT);

        if (ret != 0)
            break;

        if (mp_add_d(b, 1, b) != MP_OKAY) {
            ret = MP_MOD_E;
            break;
        }

        /* set H from sha digest */
        if (mp_read_unsigned_bin(H, digest, digestSz) != MP_OKAY) {
            ret = MP_READ_E;
            break;
        }

        /* generate r, r = (g exp k mod p) mod q */
        if (mp_exptmod_ex(&key->g, k, (int)key->q.used, &key->p, r) !=
                MP_OKAY) {
            ret = MP_EXPTMOD_E;
            break;
        }

        /* calculate s = (H + xr)/k = b.(H/k.b + x.r/k.b) */

        /* k = k.b */
        if (mp_mulmod(k, b, &key->q, k) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* kInv = 1/k.b mod q */
        if (mp_invmod(k, &key->q, kInv) != MP_OKAY) {
            ret = MP_INVMOD_E;
            break;
        }

        if (mp_mod(r, &key->q, r) != MP_OKAY) {
            ret = MP_MOD_E;
            break;
        }

        /* s = x.r */
        if (mp_mul(&key->x, r, s) != MP_OKAY) {
            ret = MP_MUL_E;
            break;
        }

        /* s = x.r/k.b */
        if (mp_mulmod(s, kInv, &key->q, s) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* H = H/k.b */
        if (mp_mulmod(H, kInv, &key->q, H) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* s = H/k.b + x.r/k.b = (H + x.r)/k.b */
        if (mp_add(s, H, s) != MP_OKAY) {
            ret = MP_ADD_E;
            break;
        }

        /* s = b.(e + x.r)/k.b = (e + x.r)/k */
        if (mp_mulmod(s, b, &key->q, s) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* s = (e + x.r)/k */
        if (mp_mod(s, &key->q, s) != MP_OKAY) {
            ret = MP_MOD_E;
            break;
        }
#endif

        /* detect zero r or s */
        if ((mp_iszero(r) == MP_YES) || (mp_iszero(s) == MP_YES)) {
            ret = MP_ZERO_E;
            break;
        }

        /* write out */
        {
            if (mp_to_unsigned_bin_len(r, out, (int)halfSz) != MP_OKAY)
                ret = MP_TO_E;
            else {
                out += halfSz;  /* advance to s in output */
                ret = mp_to_unsigned_bin_len(s, out, (int)halfSz);
            }
        }
    } while (0);

    RESTORE_VECTOR_REGISTERS();

#ifdef WOLFSSL_SMALL_STACK
    if (k) {
        if ((ret != MP_INIT_E) && (ret != MEMORY_E))
            mp_forcezero(k);
        XFREE(k, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (kInv) {
        if ((ret != MP_INIT_E) && (ret != MEMORY_E))
            mp_forcezero(kInv);
        XFREE(kInv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (r) {
        if ((ret != MP_INIT_E) && (ret != MEMORY_E))
            mp_clear(r);
        XFREE(r, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (s) {
        if ((ret != MP_INIT_E) && (ret != MEMORY_E))
            mp_clear(s);
        XFREE(s, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (H) {
        if ((ret != MP_INIT_E) && (ret != MEMORY_E))
            mp_clear(H);
        XFREE(H, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#ifndef WOLFSSL_MP_INVMOD_CONSTANT_TIME
    if (b) {
        if ((ret != MP_INIT_E) && (ret != MEMORY_E))
            mp_forcezero(b);
        XFREE(b, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif
    if (buffer) {
        ForceZero(buffer, halfSz);
        XFREE(buffer, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#else /* !WOLFSSL_SMALL_STACK */
    if (ret != MP_INIT_E) {
        ForceZero(buffer, halfSz);
        mp_forcezero(kInv);
        mp_forcezero(k);
#ifndef WOLFSSL_MP_INVMOD_CONSTANT_TIME
        mp_forcezero(b);
#endif
        mp_clear(H);
        mp_clear(s);
        mp_clear(r);
    }
#endif

    return ret;
}

int wc_DsaVerify(const byte* digest, const byte* sig, DsaKey* key, int* answer)
{
    /* use sha1 by default for backwards compatibility */
    return wc_DsaVerify_ex(digest, WC_SHA_DIGEST_SIZE, sig, key, answer);
}

int wc_DsaVerify_ex(const byte* digest, word32 digestSz, const byte* sig,
    DsaKey* key, int* answer)
{
#ifdef WOLFSSL_SMALL_STACK
    mp_int *w = NULL;
    mp_int *u1 = NULL;
    mp_int *u2 = NULL;
    mp_int *v = NULL;
    mp_int *r = NULL;
    mp_int *s = NULL;
#else
    mp_int w[1], u1[1], u2[1], v[1], r[1], s[1];
#endif
    int    ret = 0;
    int    qSz;

    if (digest == NULL || sig == NULL || key == NULL || answer == NULL)
        return BAD_FUNC_ARG;

    do {
#ifdef WOLFSSL_SMALL_STACK
        w = (mp_int *)XMALLOC(sizeof *w, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        u1 = (mp_int *)XMALLOC(sizeof *u1, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        u2 = (mp_int *)XMALLOC(sizeof *u2, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        v = (mp_int *)XMALLOC(sizeof *v, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        r = (mp_int *)XMALLOC(sizeof *r, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        s = (mp_int *)XMALLOC(sizeof *s, key->heap, DYNAMIC_TYPE_TMP_BUFFER);

        if ((w == NULL) ||
            (u1 == NULL) ||
            (u2 == NULL) ||
            (v == NULL) ||
            (r == NULL) ||
            (s == NULL)) {
            ret = MEMORY_E;
            break;
        }
#endif

        if (mp_init_multi(w, u1, u2, v, r, s) != MP_OKAY) {
            ret = MP_INIT_E;
            break;
        }

        qSz = mp_unsigned_bin_size(&key->q);
        if (qSz <= 0) {
            ret = BAD_FUNC_ARG;
            break;
        }

        /* set r and s from signature */
        if (mp_read_unsigned_bin(r, sig, (word32)qSz) != MP_OKAY ||
            mp_read_unsigned_bin(s, sig + qSz, (word32)qSz) != MP_OKAY) {
            ret = MP_READ_E;
            break;
        }

        /* sanity checks */
        if (mp_iszero(r) == MP_YES || mp_iszero(s) == MP_YES ||
            mp_cmp(r, &key->q) != MP_LT || mp_cmp(s, &key->q) != MP_LT) {
            ret = MP_ZERO_E;
            break;
        }

        /* put H into u1 from sha digest */
        if (mp_read_unsigned_bin(u1,digest, digestSz) != MP_OKAY) {
            ret = MP_READ_E;
            break;
        }

        /* w = s invmod q */
        if (mp_invmod(s, &key->q, w) != MP_OKAY) {
            ret = MP_INVMOD_E;
            break;
        }

        /* u1 = (H * w) % q */
        if (mp_mulmod(u1, w, &key->q, u1) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* u2 = (r * w) % q */
        if (mp_mulmod(r, w, &key->q, u2) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* verify v = ((g^u1 * y^u2) mod p) mod q */
        if (mp_exptmod(&key->g, u1, &key->p, u1) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
            break;
        }

        if (mp_exptmod(&key->y, u2, &key->p, u2) != MP_OKAY) {
            ret = MP_EXPTMOD_E;
            break;
        }

        if (mp_mulmod(u1, u2, &key->p, v) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        if (mp_mod(v, &key->q, v) != MP_OKAY) {
            ret = MP_MULMOD_E;
            break;
        }

        /* do they match */
        if (mp_cmp(r, v) == MP_EQ)
            *answer = 1;
        else
            *answer = 0;
    } while (0);

#ifdef WOLFSSL_SMALL_STACK
    if (s) {
        if (ret != MP_INIT_E)
            mp_clear(s);
        XFREE(s, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (r) {
        if (ret != MP_INIT_E)
            mp_clear(r);
        XFREE(r, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (u1) {
        if (ret != MP_INIT_E)
            mp_clear(u1);
        XFREE(u1, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (u2) {
        if (ret != MP_INIT_E)
            mp_clear(u2);
        XFREE(u2, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (w) {
        if (ret != MP_INIT_E)
            mp_clear(w);
        XFREE(w, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (v) {
        if (ret != MP_INIT_E)
            mp_clear(v);
        XFREE(v, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#else
    if (ret != MP_INIT_E) {
        mp_clear(s);
        mp_clear(r);
        mp_clear(u1);
        mp_clear(u2);
        mp_clear(w);
        mp_clear(v);
    }
#endif

    return ret;
}


#endif /* NO_DSA */

