/* wolfmath.c
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

/* common functions between all math libraries */

/* HAVE_WOLF_BIGINT: Used with asynchronous crypto hardware where "raw" math
 *                   buffers are required.
 * NO_BIG_INT: Disable support for all multi-precision math libraries
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

/* in case user set USE_FAST_MATH there */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/wolfmath.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if !defined(NO_BIG_INT) || defined(WOLFSSL_SP_MATH)

#if !defined(WC_NO_CACHE_RESISTANT) && \
    ((defined(HAVE_ECC) && defined(ECC_TIMING_RESISTANT)) || \
     (defined(USE_FAST_MATH) && defined(TFM_TIMING_RESISTANT)))

    /* all off / all on pointer addresses for constant calculations */
    /* ecc.c uses same table */
    const wc_ptr_t wc_off_on_addr[2] =
    {
    #if defined(WC_64BIT_CPU)
        W64LIT(0x0000000000000000),
        W64LIT(0xffffffffffffffff)
    #elif defined(WC_16BIT_CPU)
        0x0000U,
        0xffffU
    #else
        /* 32 bit */
        0x00000000U,
        0xffffffffU
    #endif
    };
#endif


/* reverse an array, used for radix code */
void mp_reverse(unsigned char *s, int len)
{
    int ix, iy;

    if (s == NULL)
        return;

    ix = 0;
    iy = len - 1;
    while (ix < iy) {
        unsigned char t = s[ix];
        s[ix] = s[iy];
        s[iy] = t;
        ++ix;
        --iy;
    }
}

int get_digit_count(const mp_int* a)
{
    if (a == NULL)
        return 0;

    return (int)a->used;
}

mp_digit get_digit(const mp_int* a, int n)
{
    if (a == NULL)
        return 0;

    return (n < 0 || (unsigned int)n >= (unsigned int)a->used) ? 0 : a->dp[n];
}

#if defined(HAVE_ECC) || defined(WOLFSSL_MP_COND_COPY)
/* Conditionally copy a into b. Performed in constant time.
 *
 * a     MP integer to copy.
 * copy  On 1, copy a into b. on 0 leave b unchanged.
 * b     MP integer to copy into.
 * returns BAD_FUNC_ARG when a or b is NULL, MEMORY_E when growing b fails and
 *         MP_OKAY otherwise.
 */
int mp_cond_copy(mp_int* a, int copy, mp_int* b)
{
    int err = MP_OKAY;
#if defined(SP_WORD_SIZE) && SP_WORD_SIZE == 8
    unsigned int mask = (unsigned int)0 - copy;
#else
    mp_digit mask = (mp_digit)0 - (mp_digit)copy;
#endif

    if (a == NULL || b == NULL)
        err = BAD_FUNC_ARG;

    /* Ensure b has enough space to copy a into */
    if (err == MP_OKAY)
        err = mp_grow(b, (int)a->used + 1);
    if (err == MP_OKAY) {
    #if defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_SP_MATH_ALL)
        unsigned int i;
    #else
        int i;
    #endif
        /* When mask 0, b is unchanged2
         * When mask all set, b ^ b ^ a = a
         */
        /* Conditionally copy all digits and then number of used digits.
         * get_digit() returns 0 when index greater than available digit.
         */
        for (i = 0; i < a->used; i++) {
            b->dp[i] ^= (get_digit(a, (int)i) ^ get_digit(b, (int)i)) & mask;
        }
        for (; i < b->used; i++) {
            b->dp[i] ^= (get_digit(a, (int)i) ^ get_digit(b, (int)i)) & mask;
        }
        b->used ^= (a->used ^ b->used) & (unsigned int)mask;
#if (!defined(WOLFSSL_SP_MATH) && !defined(WOLFSSL_SP_MATH_ALL)) || \
    defined(WOLFSSL_SP_INT_NEGATIVE)
        b->sign ^= (a->sign ^ b->sign) & (unsigned int)mask;
#endif
    }

    return err;
}
#endif /* HAVE_ECC || WOLFSSL_MP_COND_COPY */


#ifndef WC_NO_RNG
int get_rand_digit(WC_RNG* rng, mp_digit* d)
{
    return wc_RNG_GenerateBlock(rng, (byte*)d, sizeof(mp_digit));
}

#if defined(WC_RSA_BLINDING) || defined(WOLFCRYPT_HAVE_SAKKE)
int mp_rand(mp_int* a, int digits, WC_RNG* rng)
{
    int ret = 0;
    int cnt = digits * (int)sizeof(mp_digit);

    if (rng == NULL) {
        ret = MISSING_RNG_E;
    }
    else if (a == NULL || digits <= 0) {
        ret = BAD_FUNC_ARG;
    }

#ifdef USE_INTEGER_HEAP_MATH
    /* allocate space for digits */
    if (ret == MP_OKAY) {
        ret = mp_set_bit(a, digits * DIGIT_BIT - 1);
    }
#else
#if defined(WOLFSSL_SP_MATH) || defined(WOLFSSL_SP_MATH_ALL)
    if ((ret == MP_OKAY) && ((unsigned int)digits > a->size))
#else
    if ((ret == MP_OKAY) && (digits > FP_SIZE))
#endif
    {
        ret = BAD_FUNC_ARG;
    }
    if (ret == MP_OKAY) {
        a->used = (word32)digits;
    }
#endif
    /* fill the data with random bytes */
    if (ret == MP_OKAY) {
        ret = wc_RNG_GenerateBlock(rng, (byte*)a->dp, (word32)cnt);
    }
    if (ret == MP_OKAY) {
#ifdef USE_INTEGER_HEAP_MATH
        int i;
        /* Mask down each digit to only bits used */
        for (i = 0; i < a->used; i++) {
            a->dp[i] &= MP_MASK;
        }
#endif
        /* ensure top digit is not zero */
        while ((ret == MP_OKAY) && (a->dp[a->used - 1] == 0)) {
            ret = get_rand_digit(rng, &a->dp[a->used - 1]);
#ifdef USE_INTEGER_HEAP_MATH
            a->dp[a->used - 1] &= MP_MASK;
#endif
        }
    }

    return ret;
}
#endif /* WC_RSA_BLINDING || WOLFCRYPT_HAVE_SAKKE */
#endif /* !WC_NO_RNG */

#if defined(HAVE_ECC) || defined(WOLFSSL_EXPORT_INT)
/* export an mp_int as unsigned char or hex string
 * encType is WC_TYPE_UNSIGNED_BIN or WC_TYPE_HEX_STR
 * return MP_OKAY on success */
int wc_export_int(mp_int* mp, byte* buf, word32* len, word32 keySz,
    int encType)
{
    int err;

    if (mp == NULL || buf == NULL || len == NULL)
        return BAD_FUNC_ARG;

    if (encType == WC_TYPE_HEX_STR) {
        /* for WC_TYPE_HEX_STR the keySz is not used.
         * The size is computed via mp_radix_size and checked with len input */
    #ifdef WC_MP_TO_RADIX
        int size = 0;
        err = mp_radix_size(mp, MP_RADIX_HEX, &size);
        if (err == MP_OKAY) {
            /* make sure we can fit result */
            if (*len < (word32)size) {
                *len = (word32)size;
                return BUFFER_E;
            }
            *len = (word32)size;
            err = mp_tohex(mp, (char*)buf);
        }
    #else
        err = NOT_COMPILED_IN;
    #endif
    }
    else {
        /* for WC_TYPE_UNSIGNED_BIN keySz is used to zero pad.
         * The key size is always returned as the size */
        if (*len < keySz) {
            *len = keySz;
            return BUFFER_E;
        }
        *len = keySz;
        XMEMSET(buf, 0, *len);
        err = mp_to_unsigned_bin(mp, buf +
            (keySz - (word32)mp_unsigned_bin_size(mp)));
    }

    return err;
}
#endif

#ifdef HAVE_WOLF_BIGINT
void wc_bigint_init(WC_BIGINT* a)
{
    if (a != NULL) {
        a->buf = NULL;
        a->len = 0;
        a->heap = NULL;
    }
}

int wc_bigint_alloc(WC_BIGINT* a, word32 sz)
{
    int err = MP_OKAY;

    if (a == NULL)
        return BAD_FUNC_ARG;

    if (sz > 0) {
        if (a->buf && sz > a->len) {
            wc_bigint_free(a);
        }
        if (a->buf == NULL) {
            a->buf = (byte*)XMALLOC(sz, a->heap, DYNAMIC_TYPE_WOLF_BIGINT);
            if (a->buf == NULL) {
                err = MP_MEM;
            }
        }
        else {
            XMEMSET(a->buf, 0, sz);
        }
    }
    a->len = sz;

    return err;
}

/* assumes input is big endian format */
int wc_bigint_from_unsigned_bin(WC_BIGINT* a, const byte* in, word32 inlen)
{
    int err;

    if (a == NULL || in == NULL || inlen == 0)
        return BAD_FUNC_ARG;

    err = wc_bigint_alloc(a, inlen);
    if (err == 0) {
        XMEMCPY(a->buf, in, inlen);
    }

    return err;
}

int wc_bigint_to_unsigned_bin(WC_BIGINT* a, byte* out, word32* outlen)
{
    word32 sz;

    if (a == NULL || out == NULL || outlen == NULL || *outlen == 0)
        return BAD_FUNC_ARG;

    /* trim to fit into output buffer */
    sz = a->len;
    if (a->len > *outlen) {
        WOLFSSL_MSG("wc_bigint_export: Truncating output");
        sz = *outlen;
    }

    if (a->buf) {
        XMEMCPY(out, a->buf, sz);
    }

    *outlen = sz;

    return MP_OKAY;
}

void wc_bigint_zero(WC_BIGINT* a)
{
    if (a && a->buf) {
        ForceZero(a->buf, a->len);
    }
}

void wc_bigint_free(WC_BIGINT* a)
{
    if (a) {
        if (a->buf) {
          XFREE(a->buf, a->heap, DYNAMIC_TYPE_WOLF_BIGINT);
        }
        a->buf = NULL;
        a->len = 0;
    }
}

/* sz: make sure the buffer is at least that size and zero padded.
 *     A `sz == 0` will use the size of `src`.
 *     The calculated sz is stored into dst->len in `wc_bigint_alloc`.
 */
int wc_mp_to_bigint_sz(mp_int* src, WC_BIGINT* dst, word32 sz)
{
    int err;
    word32 x;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    /* get size of source */
    x = mp_unsigned_bin_size(src);
    if (sz < x)
        sz = x;

    /* make sure destination is allocated and large enough */
    err = wc_bigint_alloc(dst, sz);
    if (err == MP_OKAY && sz > 0) {
        /* leading zero pad */
        word32 y = sz - x;
        XMEMSET(dst->buf, 0, y);

        /* export src as unsigned bin to destination buf */
        err = mp_to_unsigned_bin(src, dst->buf + y);
    }

    return err;
}

int wc_mp_to_bigint(mp_int* src, WC_BIGINT* dst)
{
    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    return wc_mp_to_bigint_sz(src, dst, 0);
}

int wc_bigint_to_mp(WC_BIGINT* src, mp_int* dst)
{
    int err;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    if (src->buf == NULL)
        return BAD_FUNC_ARG;

    err = mp_read_unsigned_bin(dst, src->buf, src->len);
    wc_bigint_free(src);

    return err;
}
#endif /* HAVE_WOLF_BIGINT */

#endif /* !NO_BIG_INT || WOLFSSL_SP_MATH */

#ifdef HAVE_WC_INTROSPECTION
const char *wc_GetMathInfo(void)
{
    return
        "\tMulti-Precision: "
    #ifdef WOLFSSL_SP_MATH_ALL
        "Wolf(SP)"
        #ifdef WOLFSSL_SP_NO_DYN_STACK
            " no-dyn-stack"
        #endif
        " word-size=" WC_STRINGIFY(SP_WORD_SIZE)
        " bits=" WC_STRINGIFY(SP_INT_BITS)
        " sp_int.c"
    #elif defined(USE_FAST_MATH)
        "Fast"
        " max-bits=" WC_STRINGIFY(FP_MAX_BITS)
        #ifndef TFM_TIMING_RESISTANT
        " not-constant-time"
        #endif
        " tfm.c"
    #elif defined(USE_INTEGER_HEAP_MATH)
        "Heap"
        " not-constant-time"
        " integer.c"
    #elif defined(NO_BIG_INT) || defined(WOLFSSL_SP_MATH)
        "Disabled"
    #else
        "Unknown"
    #endif

    #if defined(WOLFSSL_HAVE_SP_ECC) || defined(WOLFSSL_HAVE_SP_DH) || \
        defined(WOLFSSL_HAVE_SP_RSA)
         "\n\tSingle Precision:"
        #ifdef WOLFSSL_HAVE_SP_ECC
            " ecc"
            #ifndef WOLFSSL_SP_NO_256
                " 256"
            #endif
            #ifdef WOLFSSL_SP_384
                " 384"
            #endif
            #ifdef WOLFSSL_SP_521
                " 521"
            #endif
        #endif
        #if defined(WOLFSSL_HAVE_SP_RSA) && defined(WOLFSSL_HAVE_SP_DH)
            " rsa/dh"
        #elif defined(WOLFSSL_HAVE_SP_RSA)
            " rsa"
        #elif defined(WOLFSSL_HAVE_SP_DH)
            " dh"
        #endif
        #ifndef WOLFSSL_SP_NO_2048
            " 2048"
        #endif
        #ifndef WOLFSSL_SP_NO_3072
            " 3072"
        #endif
        #ifdef WOLFSSL_SP_4096
            " 4096"
        #endif
        #ifdef WOLFSSL_SP_ASM
            " asm"
        #endif

        #if !defined(WOLFSSL_SP_ASM)
            #if defined(SP_WORD_SIZE) && SP_WORD_SIZE == 32
            " sp_c32.c"
            #else
            " sp_c64.c"
            #endif
        #elif defined(WOLFSSL_SP_ARM32_ASM)
            " sp_arm32.c"
        #elif defined(WOLFSSL_SP_ARM64_ASM)
            " sp_arm64.c"
        #elif defined(WOLFSSL_SP_ARM_THUMB_ASM)
            " sp_armthumb.c"
        #elif defined(WOLFSSL_SP_ARM_CORTEX_M_ASM)
            " sp_cortexm.c"
        #elif defined(WOLFSSL_SP_X86_64_ASM)
            " sp_x86_64.c"
        #else
            " sp_[arch].c"
        #endif
    #endif

    /* other SP math options */
    #if defined(WOLFSSL_SP_MATH_ALL) || defined(WOLFSSL_HAVE_SP_ECC) || \
        defined(WOLFSSL_HAVE_SP_DH) || defined(WOLFSSL_HAVE_SP_RSA)
        #ifdef WOLFSSL_SP_SMALL
            " small"
        #endif
        #ifdef WOLFSSL_SP_NO_MALLOC
            " no-malloc"
        #endif
    #endif
    ;
}
#endif /* HAVE_WC_INTROSPECTION */
