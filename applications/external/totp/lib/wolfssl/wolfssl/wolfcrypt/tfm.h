/* tfm.h
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
 * Based on public domain TomsFastMath 0.10 by Tom St Denis, tomstdenis@iahu.ca,
 * http://math.libtomcrypt.com
 */


/**
 *  Edited by Moises Guimaraes (moises.guimaraes@phoebus.com.br)
 *  to fit CyaSSL's needs.
 */

/*!
    \file wolfssl/wolfcrypt/tfm.h
*/

#ifndef WOLF_CRYPT_TFM_H
#define WOLF_CRYPT_TFM_H

#include <wolfssl/wolfcrypt/types.h>
#ifndef CHAR_BIT
    #include <limits.h>
#endif

#include <wolfssl/wolfcrypt/random.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_NO_ASM
   #undef  TFM_NO_ASM
   #define TFM_NO_ASM
#endif

#ifdef NO_64BIT
   #undef  NO_TFM_64BIT
   #define NO_TFM_64BIT
#endif

#ifndef NO_TFM_64BIT
/* autodetect x86-64 and make sure we are using 64-bit digits with x86-64 asm */
#if defined(__x86_64__)
   #if defined(TFM_X86) || defined(TFM_SSE2) || defined(TFM_ARM)
       #error x86-64 detected, x86-32/SSE2/ARM optimizations are not valid!
   #endif
   #if !defined(TFM_X86_64) && !defined(TFM_NO_ASM)
      #define TFM_X86_64
   #endif
#endif
#if defined(__aarch64__) && defined(__APPLE__)
    #if !defined(TFM_AARCH_64) && !defined(TFM_NO_ASM)
        #define TFM_AARCH_64
    #endif
#endif
#if defined(TFM_X86_64) || defined(TFM_AARCH_64)
    #if !defined(FP_64BIT)
       #define FP_64BIT
    #endif
#endif
/* use 64-bit digit even if not using asm on x86_64 */
#if defined(__x86_64__) && !defined(FP_64BIT)
    #define FP_64BIT
#endif
/* if intel compiler doesn't provide 128 bit type don't turn on 64bit */
#if defined(FP_64BIT) && defined(__INTEL_COMPILER) && !defined(HAVE___UINT128_T)
    #undef FP_64BIT
    #undef TFM_X86_64
#endif
#endif /* NO_TFM_64BIT */

/* try to detect x86-32 */
#if defined(__i386__) && !defined(TFM_SSE2)
   #if defined(TFM_X86_64) || defined(TFM_ARM)
       #error x86-32 detected, x86-64/ARM optimizations are not valid!
   #endif
   #if !defined(TFM_X86) && !defined(TFM_NO_ASM)
      #define TFM_X86
   #endif
#endif

/* make sure we're 32-bit for x86-32/sse/arm/ppc32 */
#if (defined(TFM_X86) || defined(TFM_SSE2) || defined(TFM_ARM) || defined(TFM_PPC32)) && defined(FP_64BIT)
   #warning x86-32, SSE2 and ARM, PPC32 optimizations require 32-bit digits (undefining)
   #undef FP_64BIT
#endif

/* multi asms? */
#ifdef TFM_X86
   #define TFM_ASM
#endif
#ifdef TFM_X86_64
   #ifdef TFM_ASM
      #error TFM_ASM already defined!
   #endif
   #define TFM_ASM
#endif
#ifdef TFM_SSE2
   #ifdef TFM_ASM
      #error TFM_ASM already defined!
   #endif
   #define TFM_ASM
#endif
#ifdef TFM_ARM
   #ifdef TFM_ASM
      #error TFM_ASM already defined!
   #endif
   #define TFM_ASM
#endif
#ifdef TFM_PPC32
   #ifdef TFM_ASM
      #error TFM_ASM already defined!
   #endif
   #define TFM_ASM
#endif
#ifdef TFM_PPC64
   #ifdef TFM_ASM
      #error TFM_ASM already defined!
   #endif
   #define TFM_ASM
#endif
#ifdef TFM_AVR32
   #ifdef TFM_ASM
      #error TFM_ASM already defined!
   #endif
   #define TFM_ASM
#endif

/* we want no asm? */
#ifdef TFM_NO_ASM
   #undef TFM_X86
   #undef TFM_X86_64
   #undef TFM_SSE2
   #undef TFM_ARM
   #undef TFM_PPC32
   #undef TFM_PPC64
   #undef TFM_AVR32
   #undef TFM_ASM
#endif

/* ECC helpers */
#ifdef TFM_ECC192
   #ifdef FP_64BIT
       #define TFM_MUL3
       #define TFM_SQR3
   #else
       #define TFM_MUL6
       #define TFM_SQR6
   #endif
#endif

#ifdef TFM_ECC224
   #ifdef FP_64BIT
       #define TFM_MUL4
       #define TFM_SQR4
   #else
       #define TFM_MUL7
       #define TFM_SQR7
   #endif
#endif

#ifdef TFM_ECC256
   #ifdef FP_64BIT
       #define TFM_MUL4
       #define TFM_SQR4
   #else
       #define TFM_MUL8
       #define TFM_SQR8
   #endif
#endif

#ifdef TFM_ECC384
   #ifdef FP_64BIT
       #define TFM_MUL6
       #define TFM_SQR6
   #else
       #define TFM_MUL12
       #define TFM_SQR12
   #endif
#endif

#ifdef TFM_ECC521
   #ifdef FP_64BIT
       #define TFM_MUL9
       #define TFM_SQR9
   #else
       #define TFM_MUL17
       #define TFM_SQR17
   #endif
#endif


/* allow user to define on fp_digit, fp_word types */
#ifndef WOLFSSL_BIGINT_TYPES

/* some default configurations.
 */
#if defined(WC_16BIT_CPU)
   typedef unsigned int    fp_digit;
   #define SIZEOF_FP_DIGIT 2
   typedef unsigned long   fp_word;
   typedef   signed long   fp_sword;
#elif defined(FP_64BIT)
   /* for GCC only on supported platforms */
   typedef unsigned long long fp_digit;   /* 64bit, 128 uses mode(TI) below */
   #define SIZEOF_FP_DIGIT 8
   typedef unsigned long      fp_word  __attribute__ ((mode(TI)));
   typedef   signed long      fp_sword __attribute__ ((mode(TI)));
#else

   #ifndef NO_TFM_64BIT
      #if defined(_MSC_VER) || defined(__BORLANDC__)
         typedef unsigned __int64   ulong64;
         typedef   signed __int64    long64;
      #else
         typedef unsigned long long ulong64;
         typedef   signed long long  long64;
      #endif
      typedef unsigned int       fp_digit;
      #define SIZEOF_FP_DIGIT 4
      typedef ulong64            fp_word;
      typedef long64             fp_sword;
      #define FP_32BIT
   #else
      /* some procs like coldfire prefer not to place multiply into 64bit type
         even though it exists */
      typedef unsigned short     fp_digit;
      #define SIZEOF_FP_DIGIT 2
      typedef unsigned int       fp_word;
      typedef   signed int       fp_sword;
   #endif
#endif

#endif /* WOLFSSL_BIGINT_TYPES */


/* # of digits this is */
#define DIGIT_BIT   ((CHAR_BIT) * SIZEOF_FP_DIGIT)

/* Max size of any number in bits.  Basically the largest size you will be
 * multiplying should be half [or smaller] of FP_MAX_SIZE-four_digit
 *
 * It defaults to 4096-bits [allowing multiplications up to 2048x2048 bits ]
 */


#ifndef FP_MAX_BITS
    #define FP_MAX_BITS           4096
#endif
#ifdef WOLFSSL_OPENSSH
    /* OpenSSH uses some BIG primes so we need to accommodate for that */
    #undef FP_MAX_BITS
    #define FP_MAX_BITS 16384
#endif
#define FP_MAX_SIZE           (FP_MAX_BITS+(8*DIGIT_BIT))

/* will this lib work? */
#if CHAR_BIT == 0
   #error CHAR_BIT must be nonzero
#endif
#if (CHAR_BIT & 7)
   #error CHAR_BIT must be a multiple of eight.
#endif
#if FP_MAX_BITS % CHAR_BIT
   #error FP_MAX_BITS must be a multiple of CHAR_BIT
#endif

#define FP_MASK    (fp_digit)(-1)
#define FP_DIGIT_MAX FP_MASK
#define FP_SIZE    (FP_MAX_SIZE/DIGIT_BIT)

#define FP_MAX_PRIME_SIZE (FP_MAX_BITS/(2*CHAR_BIT))
/* In terms of FP_MAX_BITS, it is double the size possible for a number
 * to allow for multiplication, divide that 2 out. Also divide by CHAR_BIT
 * to convert from bits to bytes. (Note, FP_PRIME_SIZE is the number of
 * values in the canned prime number list.) */

/* signs */
#define FP_ZPOS     0
#define FP_NEG      1

/* return codes */
#define FP_OKAY      0
#define FP_VAL      (-1)
#define FP_MEM      (-2)
#define FP_NOT_INF  (-3)
#define FP_WOULDBLOCK (-4)

/* equalities */
#define FP_LT        (-1)   /* less than */
#define FP_EQ         0   /* equal to */
#define FP_GT         1   /* greater than */

/* replies */
#define FP_YES        1   /* yes response */
#define FP_NO         0   /* no response */


#ifdef WOLFSSL_SMALL_STACK
/*
 * Dynamic memory allocation of mp_int.
 */
/* Declare a dynamically allocated mp_int. */
#define DECL_MP_INT_SIZE(name, bits)                        \
    mp_int* name = NULL
/* Declare a dynamically allocated mp_int. */
#define DECL_MP_INT_SIZE_DYN(name, bits, max)               \
    mp_int* name = NULL
/* Allocate an mp_int of minimal size and zero out. */
#define NEW_MP_INT_SIZE(name, bits, heap, type)             \
do {                                                        \
    name = (mp_int*)XMALLOC(sizeof(mp_int), heap, type);    \
    if (name != NULL) {                                     \
        XMEMSET(name, 0, sizeof(mp_int));                   \
    }                                                       \
}                                                           \
while (0)
/* Dispose of dynamically allocated mp_int. */
#define FREE_MP_INT_SIZE(name, heap, type)      \
    XFREE(name, heap, type)
/* Must check for mp_int pointer for NULL. */
#define MP_INT_SIZE_CHECK_NULL
#else
/*
 * Static allocation of mp_int.
 */
/* Declare a statically allocated mp_int. */
#define DECL_MP_INT_SIZE(name, bits)            \
    mp_int name[1]
/* Declare a statically allocated mp_int. */
#define DECL_MP_INT_SIZE_DYN(name, bits, max)   \
    mp_int name[1]
/* Zero out mp_int of minimal size. */
#define NEW_MP_INT_SIZE(name, bits, heap, type) \
    XMEMSET(name, 0, sizeof(mp_int))
/* Dispose of static mp_int. */
#define FREE_MP_INT_SIZE(name, heap, type) WC_DO_NOTHING
#endif

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

/* a FP type */
typedef struct fp_int {
    int      used;
    int      sign;
#if defined(ALT_ECC_SIZE) || defined(HAVE_WOLF_BIGINT)
    int      size;
#endif
    fp_digit dp[FP_SIZE];

#ifdef HAVE_WOLF_BIGINT
    struct WC_BIGINT raw; /* unsigned binary (big endian) */
#endif
} fp_int;

/* Types */
typedef fp_digit mp_digit;
typedef fp_word  mp_word;
typedef fp_int   mp_int;


/* wolf big int and common functions */
#include <wolfssl/wolfcrypt/wolfmath.h>


/* externally define this symbol to ignore the default settings, useful for changing the build from the make process */
#ifndef TFM_ALREADY_SET

/* do we want the large set of small multiplications ?
   Enable these if you are going to be doing a lot of small (<= 16 digit) multiplications say in ECC
   Or if you're on a 64-bit machine doing RSA as a 1024-bit integer == 16 digits ;-)
 */
/* need to refactor the function */
/*#define TFM_SMALL_SET */

/* do we want huge code
   Enable these if you are doing 20, 24, 28, 32, 48, 64 digit multiplications (useful for RSA)
   Less important on 64-bit machines as 32 digits == 2048 bits
 */
#if 0
#define TFM_MUL3
#define TFM_MUL4
#define TFM_MUL6
#define TFM_MUL7
#define TFM_MUL8
#define TFM_MUL9
#define TFM_MUL12
#define TFM_MUL17
#endif
#ifdef TFM_HUGE_SET
#define TFM_MUL20
#define TFM_MUL24
#define TFM_MUL28
#define TFM_MUL32
#if (FP_MAX_BITS >= 6144) && defined(FP_64BIT)
    #define TFM_MUL48
#endif
#if (FP_MAX_BITS >= 8192) && defined(FP_64BIT)
    #define TFM_MUL64
#endif
#endif

#if 0
#define TFM_SQR3
#define TFM_SQR4
#define TFM_SQR6
#define TFM_SQR7
#define TFM_SQR8
#define TFM_SQR9
#define TFM_SQR12
#define TFM_SQR17
#endif
#ifdef TFM_HUGE_SET
#define TFM_SQR20
#define TFM_SQR24
#define TFM_SQR28
#define TFM_SQR32
#define TFM_SQR48
#define TFM_SQR64
#endif

/* Optional math checks (enable WOLFSSL_DEBUG_MATH to print info) */
/* #define TFM_CHECK */

/* Is the target a P4 Prescott
 */
/* #define TFM_PRESCOTT */

/* Do we want timing resistant fp_exptmod() ?
 * This makes it slower but also timing invariant with respect to the exponent
 */
/* #define TFM_TIMING_RESISTANT */

#endif /* TFM_ALREADY_SET */

/* functions */

/* returns a TFM ident string useful for debugging... */
/*const char *fp_ident(void);*/

/* initialize [or zero] an fp int */
void fp_init(fp_int *a);
MP_API void fp_zero(fp_int *a);
MP_API void fp_clear(fp_int *a);
/* uses ForceZero to clear sensitive memory */
MP_API void fp_forcezero (fp_int * a);
MP_API void fp_free(fp_int* a);

/* zero/one/even/odd/neg/word ? */
#define fp_iszero(a) (((a)->used == 0) ? FP_YES : FP_NO)
#define fp_isone(a) \
    ((((a)->used == 1) && ((a)->dp[0] == 1) && ((a)->sign == FP_ZPOS)) \
                                                               ? FP_YES : FP_NO)
#define fp_iseven(a) \
    (((a)->used > 0 && (((a)->dp[0] & 1) == 0)) ? FP_YES : FP_NO)
#define fp_isodd(a)  \
    (((a)->used > 0  && (((a)->dp[0] & 1) == 1)) ? FP_YES : FP_NO)
#define fp_isneg(a)  (((a)->sign != FP_ZPOS) ? FP_YES : FP_NO)
#define fp_setneg(a) ((a)->sign = FP_NEG)
#define fp_isword(a, w) \
    (((((a)->used == 1) && ((a)->dp[0] == (w))) || \
                               (((w) == 0) && ((a)->used == 0))) ? FP_YES : FP_NO)
/* Number of bits used based on used field only. */
#define fp_bitsused(a)   ((a)->used * DIGIT_BIT)

/* set to a small digit */
void fp_set(fp_int *a, fp_digit b);
int  fp_set_int(fp_int *a, unsigned long b);

/* check if a bit is set */
int fp_is_bit_set(fp_int *a, fp_digit b);
/* set the b bit to 1 */
int fp_set_bit (fp_int * a, fp_digit b);

/* copy from a to b */
void fp_copy(const fp_int *a, fp_int *b);
void fp_init_copy(fp_int *a, fp_int *b);

/* clamp digits */
#define fp_clamp(a)   { while ((a)->used && (a)->dp[(a)->used-1] == 0) --((a)->used); (a)->sign = (a)->used ? (a)->sign : FP_ZPOS; }
#define mp_clamp(a)   fp_clamp(a)
#define mp_grow(a,s)  MP_OKAY

/* negate and absolute */
#define fp_neg(a, b)  { fp_copy(a, b); (b)->sign ^= 1; fp_clamp(b); }
#define fp_abs(a, b)  { fp_copy(a, b); (b)->sign  = 0; }

/* right shift x digits */
void fp_rshd(fp_int *a, int x);

/* right shift x bits */
void fp_rshb(fp_int *c, int x);

/* left shift x digits */
int fp_lshd(fp_int *a, int x);

/* signed comparison */
int fp_cmp(fp_int *a, fp_int *b);

/* unsigned comparison */
int fp_cmp_mag(fp_int *a, fp_int *b);

/* power of 2 operations */
void fp_div_2d(fp_int *a, int b, fp_int *c, fp_int *d);
void fp_mod_2d(fp_int *a, int b, fp_int *c);
int  fp_mul_2d(fp_int *a, int b, fp_int *c);
void fp_2expt (fp_int *a, int b);
int  fp_mul_2(fp_int *a, fp_int *b);
void fp_div_2(fp_int *a, fp_int *b);
/* c = a / 2 (mod b) - constant time (a < b and positive) */
int fp_div_2_mod_ct(fp_int *a, fp_int *b, fp_int *c);


/* Counts the number of lsbs which are zero before the first zero bit */
int fp_cnt_lsb(fp_int *a);

/* c = a + b */
int fp_add(fp_int *a, fp_int *b, fp_int *c);

/* c = a - b */
int fp_sub(fp_int *a, fp_int *b, fp_int *c);

/* c = a * b */
int fp_mul(fp_int *a, fp_int *b, fp_int *c);

/* b = a*a  */
int fp_sqr(fp_int *a, fp_int *b);

/* a/b => cb + d == a */
int fp_div(fp_int *a, fp_int *b, fp_int *c, fp_int *d);

/* c = a mod b, 0 <= c < b  */
int fp_mod(fp_int *a, fp_int *b, fp_int *c);

/* compare against a single digit */
int fp_cmp_d(fp_int *a, fp_digit b);

/* c = a + b */
int fp_add_d(fp_int *a, fp_digit b, fp_int *c);

/* c = a - b */
int fp_sub_d(fp_int *a, fp_digit b, fp_int *c);

/* c = a * b */
int fp_mul_d(fp_int *a, fp_digit b, fp_int *c);

/* a/b => cb + d == a */
/*int fp_div_d(fp_int *a, fp_digit b, fp_int *c, fp_digit *d);*/

/* c = a mod b, 0 <= c < b  */
/*int fp_mod_d(fp_int *a, fp_digit b, fp_digit *c);*/

/* ---> number theory <--- */
/* d = a + b (mod c) */
/*int fp_addmod(fp_int *a, fp_int *b, fp_int *c, fp_int *d);*/

/* d = a - b (mod c) */
/*int fp_submod(fp_int *a, fp_int *b, fp_int *c, fp_int *d);*/

/* d = a * b (mod c) */
int fp_mulmod(fp_int *a, fp_int *b, fp_int *c, fp_int *d);

/* d = a - b (mod c) */
int fp_submod(fp_int *a, fp_int *b, fp_int *c, fp_int *d);

/* d = a + b (mod c) */
int fp_addmod(fp_int *a, fp_int *b, fp_int *c, fp_int *d);

/* d = a - b (mod c) - constant time (a < c and b < c) */
int fp_submod_ct(fp_int *a, fp_int *b, fp_int *c, fp_int *d);

/* d = a + b (mod c) - constant time (a < c and b < c) */
int fp_addmod_ct(fp_int *a, fp_int *b, fp_int *c, fp_int *d);

/* c = a * a (mod b) */
int fp_sqrmod(fp_int *a, fp_int *b, fp_int *c);

/* c = 1/a (mod b) */
int fp_invmod(fp_int *a, fp_int *b, fp_int *c);
int fp_invmod_mont_ct(fp_int *a, fp_int *b, fp_int *c, fp_digit mp);

/* c = (a, b) */
/*int fp_gcd(fp_int *a, fp_int *b, fp_int *c);*/

/* c = [a, b] */
/*int fp_lcm(fp_int *a, fp_int *b, fp_int *c);*/

/* setups the montgomery reduction */
int fp_montgomery_setup(fp_int *a, fp_digit *rho);

/* computes a = B**n mod b without division or multiplication useful for
 * normalizing numbers in a Montgomery system.
 */
int fp_montgomery_calc_normalization(fp_int *a, fp_int *b);

/* computes x/R == x (mod N) via Montgomery Reduction */
int fp_montgomery_reduce(fp_int *a, fp_int *m, fp_digit mp);
int fp_montgomery_reduce_ex(fp_int *a, fp_int *m, fp_digit mp, int ct);

/* d = a**b (mod c) */
int fp_exptmod(fp_int *G, fp_int *X, fp_int *P, fp_int *Y);
int fp_exptmod_ex(fp_int *G, fp_int *X, int minDigits, fp_int *P, fp_int *Y);
int fp_exptmod_nct(fp_int *G, fp_int *X, fp_int *P, fp_int *Y);

#ifdef WC_RSA_NONBLOCK

enum tfmExptModNbState {
  TFM_EXPTMOD_NB_INIT = 0,
  TFM_EXPTMOD_NB_MONT,
  TFM_EXPTMOD_NB_MONT_RED,
  TFM_EXPTMOD_NB_MONT_MUL,
  TFM_EXPTMOD_NB_MONT_MOD,
  TFM_EXPTMOD_NB_MONT_MODCHK,
  TFM_EXPTMOD_NB_NEXT,
  TFM_EXPTMOD_NB_MUL,
  TFM_EXPTMOD_NB_MUL_RED,
  TFM_EXPTMOD_NB_SQR,
  TFM_EXPTMOD_NB_SQR_RED,
  TFM_EXPTMOD_NB_RED,
  TFM_EXPTMOD_NB_COUNT /* last item for total state count only */
};

typedef struct {
#ifndef WC_NO_CACHE_RESISTANT
  fp_int   R[3];
#else
  fp_int   R[2];
#endif
  fp_digit buf;
  fp_digit mp;
  int bitcnt;
  int digidx;
  int y;
  int state; /* tfmExptModNbState */
#ifdef WC_RSA_NONBLOCK_TIME
  word32 maxBlockInst; /* maximum instructions to block */
  word32 totalInst;    /* tracks total instructions */
#endif
} exptModNb_t;

#ifdef WC_RSA_NONBLOCK_TIME
enum {
  TFM_EXPTMOD_NB_STOP = 0,     /* stop and return FP_WOULDBLOCK */
  TFM_EXPTMOD_NB_CONTINUE = 1, /* keep blocking */
};
#endif

/* non-blocking version of timing resistant fp_exptmod function */
/* supports cache resistance */
int fp_exptmod_nb(exptModNb_t* nb, fp_int* G, fp_int* X, fp_int* P, fp_int* Y);

#endif /* WC_RSA_NONBLOCK */

/* primality stuff */

/* perform a Miller-Rabin test of a to the base b and store result in "result" */
/*void fp_prime_miller_rabin (fp_int * a, fp_int * b, int *result);*/

#define FP_PRIME_SIZE      256
/* 256 trial divisions + 8 Miller-Rabins, returns FP_YES if probable prime  */
/*int fp_isprime(fp_int *a);*/
/* extended version of fp_isprime, do 't' Miller-Rabins instead of only 8 */
/*int fp_isprime_ex(fp_int *a, int t, int* result);*/

/* Primality generation flags */
/*#define TFM_PRIME_BBS      0x0001 */ /* BBS style prime */
/*#define TFM_PRIME_SAFE     0x0002 */ /* Safe prime (p-1)/2 == prime */
/*#define TFM_PRIME_2MSB_OFF 0x0004 */ /* force 2nd MSB to 0 */
/*#define TFM_PRIME_2MSB_ON  0x0008 */ /* force 2nd MSB to 1 */

/* callback for fp_prime_random, should fill dst with random bytes and return how many read [up to len] */
/*typedef int tfm_prime_callback(unsigned char *dst, int len, void *dat);*/

/*#define fp_prime_random(a, t, size, bbs, cb, dat) fp_prime_random_ex(a, t, ((size) * 8) + 1, (bbs==1)?TFM_PRIME_BBS:0, cb, dat)*/

/*int fp_prime_random_ex(fp_int *a, int t, int size, int flags, tfm_prime_callback cb, void *dat);*/

/* radix conversions */
int fp_count_bits(const fp_int *a);
int fp_leading_bit(fp_int *a);

int fp_unsigned_bin_size(const fp_int *a);
int fp_read_unsigned_bin(fp_int *a, const unsigned char *b, int c);
int fp_to_unsigned_bin(fp_int *a, unsigned char *b);
int fp_to_unsigned_bin_len(fp_int *a, unsigned char *b, int c);
int fp_to_unsigned_bin_at_pos(int x, fp_int *t, unsigned char *b);

/*int fp_read_radix(fp_int *a, char *str, int radix);*/
/*int fp_toradix(fp_int *a, char *str, int radix);*/
/*int fp_toradix_n(fp_int * a, char *str, int radix, int maxlen);*/


/* VARIOUS LOW LEVEL STUFFS */
int  s_fp_add(fp_int *a, fp_int *b, fp_int *c);
void s_fp_sub(fp_int *a, fp_int *b, fp_int *c);

int  fp_mul_comba(fp_int *a, fp_int *b, fp_int *c);

int  fp_mul_comba_small(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba3(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba4(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba6(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba7(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba8(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba9(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba12(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba17(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba20(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba24(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba28(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba32(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba48(fp_int *a, fp_int *b, fp_int *c);
int  fp_mul_comba64(fp_int *a, fp_int *b, fp_int *c);
int  fp_sqr_comba(fp_int *a, fp_int *b);
int  fp_sqr_comba_small(fp_int *a, fp_int *b);
int  fp_sqr_comba3(fp_int *a, fp_int *b);
int  fp_sqr_comba4(fp_int *a, fp_int *b);
int  fp_sqr_comba6(fp_int *a, fp_int *b);
int  fp_sqr_comba7(fp_int *a, fp_int *b);
int  fp_sqr_comba8(fp_int *a, fp_int *b);
int  fp_sqr_comba9(fp_int *a, fp_int *b);
int  fp_sqr_comba12(fp_int *a, fp_int *b);
int  fp_sqr_comba17(fp_int *a, fp_int *b);
int  fp_sqr_comba20(fp_int *a, fp_int *b);
int  fp_sqr_comba24(fp_int *a, fp_int *b);
int  fp_sqr_comba28(fp_int *a, fp_int *b);
int  fp_sqr_comba32(fp_int *a, fp_int *b);
int  fp_sqr_comba48(fp_int *a, fp_int *b);
int  fp_sqr_comba64(fp_int *a, fp_int *b);


/**
 * Used by wolfSSL
 */

/* Constants */
#define MP_LT   FP_LT   /* less than    */
#define MP_EQ   FP_EQ   /* equal to     */
#define MP_GT   FP_GT   /* greater than */
#define MP_VAL  FP_VAL  /* invalid */
#define MP_MEM  FP_MEM  /* memory error */
#define MP_NOT_INF FP_NOT_INF /* point not at infinity */
#define MP_OKAY FP_OKAY /* ok result    */
#define MP_NO   FP_NO   /* yes/no result */
#define MP_YES  FP_YES  /* yes/no result */
#define MP_ZPOS FP_ZPOS
#define MP_NEG  FP_NEG
#define MP_MASK FP_MASK

/* Prototypes */
#define mp_zero(a)      fp_zero(a)
#define mp_isone(a)     fp_isone(a)
#define mp_iseven(a)    fp_iseven(a)
#define mp_isneg(a)     fp_isneg(a)
#define mp_setneg(a)    fp_setneg(a)
#define mp_isword(a, w) fp_isword(a, w)
#define mp_bitsused(a)  fp_bitsused(a)

#define MP_RADIX_BIN  2
#define MP_RADIX_OCT  8
#define MP_RADIX_DEC  10
#define MP_RADIX_HEX  16
#define MP_RADIX_MAX  64

#define mp_tobinary(M, S)  mp_toradix((M), (S), MP_RADIX_BIN)
#define mp_tooctal(M, S)   mp_toradix((M), (S), MP_RADIX_OCT)
#define mp_todecimal(M, S) mp_toradix((M), (S), MP_RADIX_DEC)
#define mp_tohex(M, S)     mp_toradix((M), (S), MP_RADIX_HEX)

MP_API int  mp_init (mp_int * a);
MP_API int  mp_init_copy(fp_int * a, fp_int * b);
MP_API void mp_clear (mp_int * a);
MP_API void mp_free (mp_int * a);
MP_API void mp_forcezero (mp_int * a);
MP_API int  mp_init_multi(mp_int* a, mp_int* b, mp_int* c, mp_int* d, mp_int* e,
                         mp_int* f);

MP_API int  mp_add (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_sub (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_add_d (mp_int * a, mp_digit b, mp_int * c);

MP_API int  mp_mul (mp_int * a, mp_int * b, mp_int * c);
MP_API int  mp_mul_d (mp_int * a, mp_digit b, mp_int * c);
MP_API int  mp_mulmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d);
MP_API int  mp_submod (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_addmod (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_submod_ct (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_addmod_ct (mp_int* a, mp_int* b, mp_int* c, mp_int* d);
MP_API int  mp_mod(mp_int *a, mp_int *b, mp_int *c);
MP_API int  mp_invmod(mp_int *a, mp_int *b, mp_int *c);
MP_API int  mp_invmod_mont_ct(mp_int *a, mp_int *b, mp_int *c, fp_digit mp);
MP_API int  mp_exptmod (mp_int * g, mp_int * x, mp_int * p, mp_int * y);
MP_API int  mp_exptmod_ex (mp_int * g, mp_int * x, int minDigits, mp_int * p,
                           mp_int * y);
MP_API int  mp_exptmod_nct (mp_int * g, mp_int * x, mp_int * p, mp_int * y);
MP_API int  mp_mul_2d(mp_int *a, int b, mp_int *c);
MP_API int  mp_2expt(mp_int* a, int b);

MP_API int  mp_div(mp_int * a, mp_int * b, mp_int * c, mp_int * d);

MP_API int  mp_cmp(mp_int *a, mp_int *b);
MP_API int  mp_cmp_d(mp_int *a, mp_digit b);

MP_API int  mp_unsigned_bin_size(const mp_int * a);
MP_API int  mp_read_unsigned_bin (mp_int * a, const unsigned char *b, int c);
MP_API int  mp_to_unsigned_bin_at_pos(int x, mp_int *t, unsigned char *b);
MP_API int  mp_to_unsigned_bin (mp_int * a, unsigned char *b);
MP_API int  mp_to_unsigned_bin_len(mp_int * a, unsigned char *b, int c);

MP_API int  mp_sub_d(fp_int *a, fp_digit b, fp_int *c);
MP_API int  mp_copy(const fp_int* a, fp_int* b);
MP_API int  mp_isodd(mp_int* a);
MP_API int  mp_iszero(mp_int* a);
MP_API int  mp_count_bits(const mp_int *a);
MP_API int  mp_leading_bit(mp_int *a);
MP_API int  mp_set_int(mp_int *a, unsigned long b);
MP_API int  mp_is_bit_set (mp_int * a, mp_digit b);
MP_API int  mp_set_bit (mp_int * a, mp_digit b);
MP_API void mp_rshb(mp_int *a, int x);
MP_API void mp_rshd(mp_int *a, int x);
MP_API int mp_toradix (mp_int *a, char *str, int radix);
MP_API int mp_radix_size (mp_int * a, int radix, int *size);

#ifdef WOLFSSL_DEBUG_MATH
    MP_API void mp_dump(const char* desc, mp_int* a, byte verbose);
#else
    #define mp_dump(desc, a, verbose) WC_DO_NOTHING
#endif

#if defined(OPENSSL_EXTRA) || !defined(NO_DSA) || defined(HAVE_ECC)
    MP_API int mp_read_radix(mp_int* a, const char* str, int radix);
#endif

#ifdef HAVE_ECC
    MP_API int mp_sqr(fp_int *a, fp_int *b);
    MP_API int mp_montgomery_reduce(fp_int *a, fp_int *m, fp_digit mp);
    MP_API int mp_montgomery_reduce_ex(fp_int *a, fp_int *m, fp_digit mp,
                                       int ct);
    MP_API int mp_montgomery_setup(fp_int *a, fp_digit *rho);
    MP_API int mp_div_2(fp_int * a, fp_int * b);
    MP_API int mp_div_2_mod_ct(mp_int *a, mp_int *b, mp_int *c);
#endif

#if defined(HAVE_ECC) || !defined(NO_RSA) || !defined(NO_DSA) || \
    defined(WOLFSSL_KEY_GEN)
    MP_API int mp_set(fp_int *a, fp_digit b);
#endif

#if defined(HAVE_ECC) || defined(WOLFSSL_KEY_GEN) || !defined(NO_RSA) || \
    !defined(NO_DSA) || !defined(NO_DH)
    MP_API int mp_sqrmod(mp_int* a, mp_int* b, mp_int* c);
    MP_API int mp_montgomery_calc_normalization(mp_int *a, mp_int *b);
#endif

#if !defined(NO_DH) || !defined(NO_DSA) || !defined(NO_RSA) || defined(WOLFSSL_KEY_GEN)
MP_API int  mp_prime_is_prime(mp_int* a, int t, int* result);
MP_API int  mp_prime_is_prime_ex(mp_int* a, int t, int* result, WC_RNG* rng);
#endif /* !NO_DH || !NO_DSA || !NO_RSA || WOLFSSL_KEY_GEN */
#ifdef WOLFSSL_KEY_GEN
MP_API int  mp_gcd(fp_int *a, fp_int *b, fp_int *c);
MP_API int  mp_lcm(fp_int *a, fp_int *b, fp_int *c);
MP_API int  mp_rand_prime(mp_int* a, int len, WC_RNG* rng, void* heap);
MP_API int  mp_exch(mp_int *a, mp_int *b);
#endif /* WOLFSSL_KEY_GEN */
MP_API int  mp_cond_swap_ct_ex(mp_int* a, mp_int* b, int c, int m, mp_int* t);
MP_API int  mp_cond_swap_ct(mp_int* a, mp_int* b, int c, int m);

MP_API int  mp_cnt_lsb(fp_int *a);
MP_API int  mp_div_2d(fp_int *a, int b, fp_int *c, fp_int *d);
MP_API int  mp_mod_d(fp_int* a, fp_digit b, fp_digit* c);
MP_API int  mp_lshd (mp_int * a, int b);
MP_API int  mp_abs(mp_int* a, mp_int* b);

WOLFSSL_API word32 CheckRunTimeFastMath(void);

#ifdef WOLFSSL_CHECK_MEM_ZERO
void mp_memzero_add(const char* name, mp_int* a);
void mp_memzero_check(mp_int* a);
#endif

/* If user uses RSA, DH, DSA, or ECC math lib directly then fast math FP_SIZE
   must match, return 1 if a match otherwise 0 */
#define CheckFastMathSettings() (FP_SIZE == CheckRunTimeFastMath())


#ifdef __cplusplus
   }
#endif

#endif  /* WOLF_CRYPT_TFM_H */

