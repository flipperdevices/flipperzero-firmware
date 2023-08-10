/* integer.h
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


/*
 * Based on public domain LibTomMath 0.38 by Tom St Denis, tomstdenis@iahu.ca,
 * http://math.libtomcrypt.com
 */


#ifndef WOLF_CRYPT_INTEGER_H
#define WOLF_CRYPT_INTEGER_H

/* may optionally use SP math all or fast math instead. The heap math requires
 * realloc and is not timing resistant. The SP math all is recommended for new
 * designs.
 */

#ifndef USE_INTEGER_HEAP_MATH

    /* Some platforms (like FIPS) may only include integer.h for math. */
    /* Handle variations of fast math, integer and sp math */
    #include <wolfssl/wolfcrypt/wolfmath.h>

#else

#include <wolfssl/wolfcrypt/random.h>

#ifndef CHAR_BIT
    #if defined(WOLFSSL_LINUXKM)
        #include <linux/limits.h>
    #else
        #include <limits.h>
    #endif
#endif

#include <wolfssl/wolfcrypt/mpi_class.h>


#ifdef __cplusplus
extern "C" {

/* C++ compilers don't like assigning void * to mp_digit * */
#define  OPT_CAST(x)  (x *)

#elif defined(_SH3)

/* SuperH SH3 compiler doesn't like assigning voi* to mp_digit* */
#define  OPT_CAST(x)  (x *)

#else

/* C on the other hand doesn't care */
#define  OPT_CAST(x) /* null expansion */

#endif /* __cplusplus */


/* detect 64-bit mode if possible */
#if (defined(__x86_64__) || defined(__aarch64__)) && !(defined (_MSC_VER) && defined(__clang__))
   #if !(defined(MP_64BIT) && defined(MP_16BIT) && defined(MP_8BIT))
      #define MP_64BIT
   #endif
#endif
/* if intel compiler doesn't provide 128 bit type don't turn on 64bit */
#if defined(MP_64BIT) && defined(__INTEL_COMPILER) && !defined(HAVE___UINT128_T)
    #undef MP_64BIT
#endif


/* allow user to define on mp_digit, mp_word, DIGIT_BIT types */
#ifndef WOLFSSL_BIGINT_TYPES

/* some default configurations.
 *
 * A "mp_digit" must be able to hold DIGIT_BIT + 1 bits
 * A "mp_word" must be able to hold 2*DIGIT_BIT + 1 bits
 *
 * At the very least a mp_digit must be able to hold 7 bits
 * [any size beyond that is ok provided it doesn't overflow the data type]
 */
#ifdef MP_8BIT
   /* 8-bit */
   typedef unsigned char      mp_digit;
   typedef unsigned short     mp_word;
   /* don't define DIGIT_BIT, so its calculated below */
#elif defined(MP_16BIT)
   /* 16-bit */
   typedef unsigned int       mp_digit;
   typedef unsigned long      mp_word;
   /* don't define DIGIT_BIT, so its calculated below */
#elif defined(NO_64BIT)
   /* 32-bit forced to 16-bit */
   typedef unsigned short     mp_digit;
   typedef unsigned int       mp_word;
   #define DIGIT_BIT          12
#elif defined(MP_64BIT)
   /* 64-bit */
   /* for GCC only on supported platforms */
   typedef unsigned long long mp_digit;  /* 64 bit type, 128 uses mode(TI) */
   typedef unsigned long      mp_word __attribute__ ((mode(TI)));
   #define DIGIT_BIT          60
#else
   /* 32-bit default case */

   #if defined(_MSC_VER) || defined(__BORLANDC__)
      typedef unsigned __int64   ulong64;
   #else
      typedef unsigned long long ulong64;
   #endif

   typedef unsigned int       mp_digit;  /* long could be 64 now, changed TAO */
   typedef ulong64            mp_word;

   #ifdef MP_31BIT
      /* this is an extension that uses 31-bit digits */
      #define DIGIT_BIT          31
   #else
      /* default case is 28-bit digits, defines MP_28BIT as a handy test macro */
      #define DIGIT_BIT          28
      #define MP_28BIT
   #endif
#endif

#endif /* WOLFSSL_BIGINT_TYPES */

/* otherwise the bits per digit is calculated automatically from the size of
   a mp_digit */
#ifndef DIGIT_BIT
   #define DIGIT_BIT ((int)((CHAR_BIT * sizeof(mp_digit) - 1)))
      /* bits per digit */
#endif

#define MP_DIGIT_BIT     DIGIT_BIT
#define MP_MASK          ((((mp_digit)1)<<((mp_digit)DIGIT_BIT))-((mp_digit)1))
#define MP_DIGIT_MAX     MP_MASK

/* equalities */
#define MP_LT       (-1)  /* less than */
#define MP_EQ         0   /* equal to */
#define MP_GT         1   /* greater than */

#define MP_ZPOS       0   /* positive integer */
#define MP_NEG        1   /* negative */

#define MP_OKAY       0   /* ok result */
#define MP_MEM       (-2) /* out of mem */
#define MP_VAL       (-3) /* invalid input */
#define MP_NOT_INF   (-4) /* point not at infinity */
#define MP_RANGE      MP_NOT_INF

#define MP_YES        1   /* yes response */
#define MP_NO         0   /* no response */

/* Primality generation flags */
#define LTM_PRIME_BBS      0x0001 /* BBS style prime */
#define LTM_PRIME_SAFE     0x0002 /* Safe prime (p-1)/2 == prime */
#define LTM_PRIME_2MSB_ON  0x0008 /* force 2nd MSB to 1 */

typedef int           mp_err;

/* define this to use lower memory usage routines (exptmods mostly) */
#define MP_LOW_MEM

/* default precision */
#ifndef MP_PREC
   #ifndef MP_LOW_MEM
      #define MP_PREC                 32     /* default digits of precision */
   #else
      #define MP_PREC                 1      /* default digits of precision */
   #endif
#endif

/* size of comba arrays, should be at least 2 * 2**(BITS_PER_WORD -
   BITS_PER_DIGIT*2) */
#define MP_WARRAY  ((mp_word)1 << (sizeof(mp_word) * CHAR_BIT - 2 * DIGIT_BIT + 1))

/* No point in dynamically allocating mp_int when it is very small.
 * The dp field will grow and shrink dynamically.
 */
/* Declare a statically allocated mp_int. */
#define DECL_MP_INT_SIZE(name, bits)    \
    mp_int name[1]
/* Declare statically allocated mp_int. */
#define DECL_MP_INT_SIZE_DYN(name, bits, max) \
    mp_int name[1]
/* Zero out mp_int of minimal size. */
#define NEW_MP_INT_SIZE(name, bits, heap, type) \
    XMEMSET(name, 0, sizeof(mp_int))
/* Dispose of static mp_int. */
#define FREE_MP_INT_SIZE(name, heap, type) WC_DO_NOTHING
/* Initialize an mp_int. */
#define INIT_MP_INT_SIZE(name, bits) \
    mp_init(name)
/* Type to cast to when using size marcos. */
#define MP_INT_SIZE     mp_int

#ifdef HAVE_WOLF_BIGINT
    /* raw big integer */
    typedef struct WC_BIGINT {
        byte*   buf;
        word32  len;
        void*   heap;
    } WC_BIGINT;
    #define WOLF_BIGINT_DEFINED
#endif

/* the mp_int structure */
typedef struct mp_int {
    int used, alloc, sign;
    mp_digit *dp;

#ifdef HAVE_WOLF_BIGINT
    struct WC_BIGINT raw; /* unsigned binary (big endian) */
#endif
} mp_int;

/* wolf big int and common functions */
#include <wolfssl/wolfcrypt/wolfmath.h>


/* callback for mp_prime_random, should fill dst with random bytes and return
   how many read [up to len] */
typedef int ltm_prime_callback(unsigned char *dst, int len, void *dat);


#define USED(m)    ((m)->used)
#define DIGIT(m,k) ((m)->dp[(k)])
#define SIGN(m)    ((m)->sign)


/* ---> Basic Manipulations <--- */
#define mp_iszero(a) (((a)->used == 0) ? MP_YES : MP_NO)
#define mp_isone(a) \
    (((((a)->used == 1)) && ((a)->dp[0] == 1u) && ((a)->sign == MP_ZPOS)) \
                                                               ? MP_YES : MP_NO)
#define mp_iseven(a) \
    (((a)->used > 0 && (((a)->dp[0] & 1u) == 0u)) ? MP_YES : MP_NO)
#define mp_isodd(a) \
    (((a)->used > 0 && (((a)->dp[0] & 1u) == 1u)) ? MP_YES : MP_NO)
#define mp_isneg(a)  (((a)->sign != MP_ZPOS) ? MP_YES : MP_NO)
#define mp_setneg(a) ((a)->sign = MP_NEG)
#define mp_isword(a, w) \
    ((((a)->used == 1) && ((a)->dp[0] == (w))) || (((w) == 0) && ((a)->used == 0)) \
                                                               ? MP_YES : MP_NO)
/* Number of bits used based on used field only. */
#define mp_bitsused(a)   ((a)->used * DIGIT_BIT)

/* number of primes */
#ifdef MP_8BIT
   #define PRIME_SIZE      31
#else
   #define PRIME_SIZE      256
#endif

#ifndef MAX_INVMOD_SZ
    #if defined(WOLFSSL_MYSQL_COMPATIBLE)
        #define MAX_INVMOD_SZ 8192
    #else
        #define MAX_INVMOD_SZ 4096
    #endif
#endif

#define mp_prime_random(a, t, size, bbs, cb, dat) \
   mp_prime_random_ex(a, t, ((size) * 8) + 1, ((bbs)==1)?LTM_PRIME_BBS:0, cb, dat)

#define mp_read_mag(mp, str, len) mp_read_unsigned_bin((mp), (str), (len))
#define mp_mag_size(mp)           mp_unsigned_bin_size(mp)
#define mp_tomag(mp, str)         mp_to_unsigned_bin((mp), (str))

#define MP_RADIX_BIN  2
#define MP_RADIX_OCT  8
#define MP_RADIX_DEC  10
#define MP_RADIX_HEX  16
#define MP_RADIX_MAX  64

#define mp_tobinary(M, S)  mp_toradix((M), (S), MP_RADIX_BIN)
#define mp_tooctal(M, S)   mp_toradix((M), (S), MP_RADIX_OCT)
#define mp_todecimal(M, S) mp_toradix((M), (S), MP_RADIX_DEC)
#define mp_tohex(M, S)     mp_toradix((M), (S), MP_RADIX_HEX)

#define s_mp_mul(a, b, c) s_mp_mul_digs(a, b, c, (a)->used + (b)->used + 1)

#if defined(HAVE_ECC) || defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || \
    defined(WOLFSSL_DEBUG_MATH) || defined(DEBUG_WOLFSSL)
extern const char *mp_s_rmap;
#endif

/* 6 functions needed by Rsa */
MP_API int  mp_init (mp_int * a);
MP_API void mp_clear (mp_int * a);
MP_API void mp_free (mp_int * a);
MP_API void mp_forcezero(mp_int * a);
MP_API int  mp_unsigned_bin_size(const mp_int * a);
MP_API int  mp_read_unsigned_bin (mp_int * a, const unsigned char *b, int c);
MP_API int  mp_to_unsigned_bin_at_pos(int x, mp_int *t, unsigned char *b);
MP_API int  mp_to_unsigned_bin (mp_int * a, unsigned char *b);
MP_API int  mp_to_unsigned_bin_len(mp_int * a, unsigned char *b, int c);
MP_API int  mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y);
MP_API int  mp_exptmod_ex (mp_int * G, mp_int * X, int digits, mp_int * P,
                           mp_int * Y);
/* end functions needed by Rsa */

/* functions added to support above needed, removed TOOM and KARATSUBA */
MP_API int  mp_count_bits (const mp_int * a);
MP_API int  mp_leading_bit (mp_int * a);
MP_API int  mp_init_copy (mp_int * a, mp_int * b);
MP_API int  mp_copy (const mp_int * a, mp_int * b);
MP_API int  mp_grow (mp_int * a, int size);
MP_API int  mp_div_2d (mp_int * a, int b, mp_int * c, mp_int * d);
MP_API void mp_zero (mp_int * a);
MP_API void mp_clamp (mp_int * a);
MP_API int  mp_exch (mp_int * a, mp_int * b);
MP_API int  mp_cond_swap_ct_ex (mp_int * a, mp_int * b, int c, int m,
                                mp_int * t);
MP_API int  mp_cond_swap_ct (mp_int * a, mp_int * b, int c, int m);
MP_API void mp_rshd (mp_int * a, int b);
MP_API void mp_rshb (mp_int * a, int b);
MP_API int  mp_mod_2d (mp_int * a, int b, mp_int * c);
MP_API int  mp_mul_2d (mp_int * a, int b, mp_int * c);
MP_API int  mp_lshd (mp_int * a, int b);
MP_API int  mp_abs (mp_int * a, mp_int * b);
MP_API int  mp_invmod (mp_int * a, mp_int * b, mp_int * c);
int  fast_mp_invmod (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_invmod_slow (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_cmp_mag (mp_int * a, mp_int * b);
MP_API int  mp_cmp (mp_int * a, mp_int * b);
MP_API int  mp_cmp_d(mp_int * a, mp_digit b);
MP_API int  mp_set (mp_int * a, mp_digit b);
MP_API int  mp_is_bit_set (mp_int * a, mp_digit b);
MP_API int  mp_mod (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_div(mp_int * a, mp_int * b, mp_int * c, mp_int * d);
MP_API int  mp_div_2(mp_int * a, mp_int * b);
MP_API int  mp_div_2_mod_ct (mp_int* a, mp_int* b, mp_int* c);
MP_API int  mp_add (mp_int * a, mp_int * b, mp_int * c);
int  s_mp_add (mp_int * a, mp_int * b, mp_int * c);
int  s_mp_sub (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_sub (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_reduce_is_2k_l(mp_int *a);
MP_API int  mp_reduce_is_2k(mp_int *a);
MP_API int  mp_dr_is_modulus(mp_int *a);
MP_API int  mp_exptmod_fast (mp_int * G, mp_int * X, mp_int * P, mp_int * Y,
                             int redmode);
MP_API int  mp_exptmod_base_2 (mp_int * X, mp_int * P, mp_int * Y);
#define mp_exptmod_nct(G,X,P,Y)    mp_exptmod_fast(G,X,P,Y,0)
MP_API int  mp_montgomery_setup (mp_int * n, mp_digit * rho);
int  fast_mp_montgomery_reduce (mp_int * x, mp_int * n, mp_digit rho);
MP_API int  mp_montgomery_reduce (mp_int * x, mp_int * n, mp_digit rho);
#define mp_montgomery_reduce_ex(x, n, rho, ct) mp_montgomery_reduce (x, n, rho)
MP_API void mp_dr_setup(mp_int *a, mp_digit *d);
MP_API int  mp_dr_reduce (mp_int * x, mp_int * n, mp_digit k);
MP_API int  mp_reduce_2k(mp_int *a, mp_int *n, mp_digit d);
int  fast_s_mp_mul_high_digs (mp_int * a, mp_int * b, mp_int * c, int digs);
int  s_mp_mul_high_digs (mp_int * a, mp_int * b, mp_int * c, int digs);
MP_API int  mp_reduce_2k_setup_l(mp_int *a, mp_int *d);
MP_API int  mp_reduce_2k_l(mp_int *a, mp_int *n, mp_int *d);
MP_API int  mp_reduce (mp_int * x, mp_int * m, mp_int * mu);
MP_API int  mp_reduce_setup (mp_int * a, mp_int * b);
int  s_mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y, int redmode);
MP_API int  mp_montgomery_calc_normalization (mp_int * a, mp_int * b);
int  s_mp_mul_digs (mp_int * a, mp_int * b, mp_int * c, int digs);
int  s_mp_sqr (mp_int * a, mp_int * b);
int  fast_s_mp_mul_digs (mp_int * a, mp_int * b, mp_int * c, int digs);
int  fast_s_mp_sqr (mp_int * a, mp_int * b);
MP_API int  mp_init_size (mp_int * a, int size);
MP_API int  mp_div_3 (mp_int * a, mp_int *c, mp_digit * d);
MP_API int  mp_mul_2(mp_int * a, mp_int * b);
MP_API int  mp_mul (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_sqr (mp_int * a, mp_int * b);
MP_API int  mp_mulmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d);
MP_API int  mp_submod (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_addmod (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_submod_ct (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_addmod_ct (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_mul_d (mp_int * a, mp_digit b, mp_int * c);
MP_API int  mp_2expt (mp_int * a, int b);
MP_API int  mp_set_bit (mp_int * a, int b);
MP_API int  mp_reduce_2k_setup(mp_int *a, mp_digit *d);
MP_API int  mp_add_d (mp_int* a, mp_digit b, mp_int* c);
MP_API int  mp_set_int (mp_int * a, unsigned long b);
MP_API int  mp_sub_d (mp_int * a, mp_digit b, mp_int * c);
/* end support added functions */

/* added */
MP_API int mp_init_multi(mp_int* a, mp_int* b, mp_int* c, mp_int* d, mp_int* e,
                         mp_int* f);
MP_API int mp_toradix (mp_int *a, char *str, int radix);
MP_API int mp_radix_size (mp_int * a, int radix, int *size);

#ifdef WOLFSSL_DEBUG_MATH
    MP_API void mp_dump(const char* desc, mp_int* a, byte verbose);
#else
    #define mp_dump(desc, a, verbose) WC_DO_NOTHING
#endif

#if defined(HAVE_ECC) || defined(WOLFSSL_KEY_GEN) || !defined(NO_RSA) || \
    !defined(NO_DSA) || !defined(NO_DH)
    MP_API int mp_sqrmod(mp_int* a, mp_int* b, mp_int* c);
#endif
#if !defined(NO_DSA) || defined(HAVE_ECC) || defined(OPENSSL_EXTRA)
    MP_API int mp_read_radix(mp_int* a, const char* str, int radix);
#endif

#if defined(WOLFSSL_KEY_GEN) || !defined(NO_RSA) || !defined(NO_DSA) || !defined(NO_DH)
    MP_API int mp_prime_is_prime (mp_int * a, int t, int *result);
    MP_API int mp_prime_is_prime_ex (mp_int * a, int t, int *result, WC_RNG* rng);
#endif /* WOLFSSL_KEY_GEN NO_RSA NO_DSA NO_DH */
#ifdef WOLFSSL_KEY_GEN
    MP_API int mp_gcd (mp_int * a, mp_int * b, mp_int * c);
    MP_API int mp_lcm (mp_int * a, mp_int * b, mp_int * c);
    MP_API int mp_rand_prime(mp_int* a, int len, WC_RNG* rng, void* heap);
#endif

MP_API int mp_cnt_lsb(mp_int *a);
MP_API int mp_mod_d(mp_int* a, mp_digit b, mp_digit* c);


#ifdef __cplusplus
   }
#endif


#endif /* USE_INTEGER_HEAP_MATH */

#endif  /* WOLF_CRYPT_INTEGER_H */
