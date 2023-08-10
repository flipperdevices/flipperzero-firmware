/* integer.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

/* in case user set USE_FAST_MATH there */
#include <wolfssl/wolfcrypt/settings.h>

#ifndef NO_BIG_INT

#if !defined(USE_FAST_MATH) && defined(USE_INTEGER_HEAP_MATH)

#ifndef WOLFSSL_SP_MATH

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/wolfmath.h>

#if defined(FREESCALE_LTC_TFM)
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif
#ifdef WOLFSSL_DEBUG_MATH
    #include <stdio.h>
#endif

#ifdef SHOW_GEN
    #ifndef NO_STDIO_FILESYSTEM
        #include <stdio.h>
    #endif
#endif

#if defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH)
#ifdef __cplusplus
    extern "C" {
#endif
WOLFSSL_LOCAL int sp_ModExp_1024(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_1536(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_2048(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_3072(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_4096(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
#ifdef __cplusplus
    } /* extern "C" */
#endif
#endif

/* reverse an array, used for radix code */
static void
bn_reverse (unsigned char *s, int len)
{
    int     ix, iy;
    unsigned char t;

    ix = 0;
    iy = len - 1;
    while (ix < iy) {
        t     = s[ix];
        s[ix] = s[iy];
        s[iy] = t;
        ++ix;
        --iy;
    }
}

/* math settings check */
word32 CheckRunTimeSettings(void)
{
    return CTC_SETTINGS;
}


/* handle up to 6 inits */
int mp_init_multi(mp_int* a, mp_int* b, mp_int* c, mp_int* d, mp_int* e,
                  mp_int* f)
{
    int res = MP_OKAY;

    if (a) XMEMSET(a, 0, sizeof(mp_int));
    if (b) XMEMSET(b, 0, sizeof(mp_int));
    if (c) XMEMSET(c, 0, sizeof(mp_int));
    if (d) XMEMSET(d, 0, sizeof(mp_int));
    if (e) XMEMSET(e, 0, sizeof(mp_int));
    if (f) XMEMSET(f, 0, sizeof(mp_int));

    if (a && ((res = mp_init(a)) != MP_OKAY))
        return res;

    if (b && ((res = mp_init(b)) != MP_OKAY)) {
        mp_clear(a);
        return res;
    }

    if (c && ((res = mp_init(c)) != MP_OKAY)) {
        mp_clear(a); mp_clear(b);
        return res;
    }

    if (d && ((res = mp_init(d)) != MP_OKAY)) {
        mp_clear(a); mp_clear(b); mp_clear(c);
        return res;
    }

    if (e && ((res = mp_init(e)) != MP_OKAY)) {
        mp_clear(a); mp_clear(b); mp_clear(c); mp_clear(d);
        return res;
    }

    if (f && ((res = mp_init(f)) != MP_OKAY)) {
        mp_clear(a); mp_clear(b); mp_clear(c); mp_clear(d); mp_clear(e);
        return res;
    }

    return res;
}


/* init a new mp_int */
int mp_init (mp_int * a)
{
  /* Safeguard against passing in a null pointer */
  if (a == NULL)
    return MP_VAL;

  /* defer allocation until mp_grow */
  a->dp = NULL;

  /* set the used to zero, allocated digits to the default precision
   * and sign to positive */
  a->used  = 0;
  a->alloc = 0;
  a->sign  = MP_ZPOS;
#ifdef HAVE_WOLF_BIGINT
  wc_bigint_init(&a->raw);
#endif

  return MP_OKAY;
}


/* clear one (frees)  */
void mp_clear (mp_int * a)
{
  int i;

  if (a == NULL)
      return;

  /* only do anything if a hasn't been freed previously */
#ifndef HAVE_WOLF_BIGINT
  /* When HAVE_WOLF_BIGINT then mp_free -> wc_bigint_free needs to be called
   * because a->raw->buf may be allocated even when a->dp == NULL. This is the
   * case for when a zero is loaded into the mp_int. */
  if (a->dp != NULL)
#endif
  {
    /* first zero the digits */
    for (i = 0; i < a->used; i++) {
        a->dp[i] = 0;
    }

    /* free ram */
    mp_free(a);

    /* reset members to make debugging easier */
    a->alloc = a->used = 0;
    a->sign  = MP_ZPOS;
  }
}

void mp_free (mp_int * a)
{
  /* only do anything if a hasn't been freed previously */
  if (a->dp != NULL) {
    /* free ram */
    XFREE(a->dp, 0, DYNAMIC_TYPE_BIGINT);
    a->dp    = NULL;
  }

#ifdef HAVE_WOLF_BIGINT
  wc_bigint_free(&a->raw);
#endif
}

void mp_forcezero(mp_int * a)
{
    if (a == NULL)
        return;

    /* only do anything if a hasn't been freed previously */
    if (a->dp != NULL) {
      /* force zero the used digits */
      ForceZero(a->dp, a->used * sizeof(mp_digit));
#ifdef HAVE_WOLF_BIGINT
      wc_bigint_zero(&a->raw);
#endif
      /* free ram */
      mp_free(a);

      /* reset members to make debugging easier */
      a->alloc = a->used = 0;
      a->sign  = MP_ZPOS;
    }

    a->sign = MP_ZPOS;
    a->used = 0;
}


/* get the size for an unsigned equivalent */
int mp_unsigned_bin_size (const mp_int * a)
{
  int     size = mp_count_bits (a);
  return (size / 8 + ((size & 7) != 0 ? 1 : 0));
}


/* returns the number of bits in an int */
int mp_count_bits (const mp_int * a)
{
  int     r;
  mp_digit q;

  /* shortcut */
  if (a->used == 0) {
    return 0;
  }

  /* get number of digits and add that */
  r = (a->used - 1) * DIGIT_BIT;

  /* take the last digit and count the bits in it */
  q = a->dp[a->used - 1];
  while (q > ((mp_digit) 0)) {
    ++r;
    q >>= ((mp_digit) 1);
  }
  return r;
}


int mp_leading_bit (mp_int * a)
{
    int c = mp_count_bits(a);

    if (c == 0) return 0;
    return (c % 8) == 0;
}

int mp_to_unsigned_bin_at_pos(int x, mp_int *t, unsigned char *b)
{
  int res = 0;
  while (mp_iszero(t) == MP_NO) {
#ifndef MP_8BIT
      b[x++] = (unsigned char) (t->dp[0] & 255);
#else
      b[x++] = (unsigned char) (t->dp[0] | ((t->dp[1] & 0x01) << 7));
#endif
    if ((res = mp_div_2d (t, 8, t, NULL)) != MP_OKAY) {
      return res;
    }
    res = x;
  }
  return res;
}

/* store in unsigned [big endian] format */
int mp_to_unsigned_bin (mp_int * a, unsigned char *b)
{
  int     x, res;
  mp_int  t;

  if ((res = mp_init_copy (&t, a)) != MP_OKAY) {
    return res;
  }

  x = mp_to_unsigned_bin_at_pos(0, &t, b);
  if (x < 0) {
    mp_clear(&t);
    return x;
  }

  bn_reverse (b, x);
  mp_clear (&t);
  return res;
}

int mp_to_unsigned_bin_len(mp_int * a, unsigned char *b, int c)
{
    int i, len;

    len = mp_unsigned_bin_size(a);

    if (len > c) {
      return MP_VAL;
    }

    /* pad front w/ zeros to match length */
    for (i = 0; i < c - len; i++) {
      b[i] = 0x00;
    }
    return mp_to_unsigned_bin(a, b + i);
}

/* creates "a" then copies b into it */
int mp_init_copy (mp_int * a, mp_int * b)
{
  int     res;

  if ((res = mp_init_size (a, b->used)) != MP_OKAY) {
    return res;
  }

  if((res = mp_copy (b, a)) != MP_OKAY) {
    mp_clear(a);
  }

  return res;
}


/* copy, b = a */
int mp_copy (const mp_int * a, mp_int * b)
{
  int     res, n;

  /* Safeguard against passing in a null pointer */
  if (a == NULL || b == NULL)
    return MP_VAL;

  /* if dst == src do nothing */
  if (a == b) {
    return MP_OKAY;
  }

  /* grow dest */
  if (b->alloc < a->used || b->alloc == 0) {
     if ((res = mp_grow (b, a->used)) != MP_OKAY) {
        return res;
     }
  }

  /* zero b and copy the parameters over */
  {
    mp_digit *tmpa, *tmpb;

    /* pointer aliases */

    /* source */
    tmpa = a->dp;

    /* destination */
    tmpb = b->dp;

    /* copy all the digits */
    for (n = 0; n < a->used; n++) {
      *tmpb++ = *tmpa++;
    }

    /* clear high digits */
    for (; n < b->used && b->dp; n++) {
      *tmpb++ = 0;
    }
  }

  /* copy used count and sign */
  b->used = a->used;
  b->sign = a->sign;
  return MP_OKAY;
}


/* grow as required */
int mp_grow (mp_int * a, int size)
{
  int     i;
  mp_digit *tmp;

  /* if the alloc size is smaller alloc more ram */
  if (a->alloc < size || size == 0) {
    /* ensure there are always at least MP_PREC digits extra on top */
    size += (MP_PREC * 2) - (size % MP_PREC);

    /* reallocate the array a->dp
     *
     * We store the return in a temporary variable
     * in case the operation failed we don't want
     * to overwrite the dp member of a.
     */
    tmp = (mp_digit *)XREALLOC (a->dp, sizeof (mp_digit) * size, NULL,
                                                           DYNAMIC_TYPE_BIGINT);
    if (tmp == NULL) {
      /* reallocation failed but "a" is still valid [can be freed] */
      return MP_MEM;
    }

    /* reallocation succeeded so set a->dp */
    a->dp = tmp;

    /* zero excess digits */
    i        = a->alloc;
    a->alloc = size;
    for (; i < a->alloc; i++) {
      a->dp[i] = 0;
    }
  }
  return MP_OKAY;
}


/* shift right by a certain bit count (store quotient in c, optional
   remainder in d) */
int mp_div_2d (mp_int * a, int b, mp_int * c, mp_int * d)
{
  int     D, res;
  mp_int  t;


  /* if the shift count is <= 0 then we do no work */
  if (b <= 0) {
    res = mp_copy (a, c);
    if (d != NULL) {
      mp_zero (d);
    }
    return res;
  }

  if ((res = mp_init (&t)) != MP_OKAY) {
    return res;
  }

  /* get the remainder */
  if (d != NULL) {
    if ((res = mp_mod_2d (a, b, &t)) != MP_OKAY) {
      mp_clear (&t);
      return res;
    }
  }

  /* copy */
  if ((res = mp_copy (a, c)) != MP_OKAY) {
    mp_clear (&t);
    return res;
  }

  /* shift by as many digits in the bit count */
  if (b >= (int)DIGIT_BIT) {
    mp_rshd (c, b / DIGIT_BIT);
  }

  /* shift any bit count < DIGIT_BIT */
  D = (b % DIGIT_BIT);
  if (D != 0) {
    mp_rshb(c, D);
  }
  mp_clamp (c);
  if (d != NULL) {
    mp_exch (&t, d);
  }
  mp_clear (&t);
  return MP_OKAY;
}


/* set to zero */
void mp_zero (mp_int * a)
{
  int       n;
  mp_digit *tmp;

  if (a == NULL)
      return;

  a->sign = MP_ZPOS;
  a->used = 0;

  tmp = a->dp;
  for (n = 0; tmp != NULL && n < a->alloc; n++) {
     *tmp++ = 0;
  }
}


/* trim unused digits
 *
 * This is used to ensure that leading zero digits are
 * trimmed and the leading "used" digit will be non-zero
 * Typically very fast.  Also fixes the sign if there
 * are no more leading digits
 */
void mp_clamp (mp_int * a)
{
  /* decrease used while the most significant digit is
   * zero.
   */
  while (a->used > 0 && a->dp[a->used - 1] == 0) {
    --(a->used);
  }

  /* reset the sign flag if used == 0 */
  if (a->used == 0) {
    a->sign = MP_ZPOS;
  }
}


/* swap the elements of two integers, for cases where you can't simply swap the
 * mp_int pointers around
 */
int mp_exch (mp_int * a, mp_int * b)
{
  mp_int  t;

  t  = *a;
  *a = *b;
  *b = t;
  return MP_OKAY;
}

int mp_cond_swap_ct_ex (mp_int * a, mp_int * b, int c, int m, mp_int * t)
{
    (void)c;
    (void)t;
    if (m == 1)
        mp_exch(a, b);
    return MP_OKAY;
}

int mp_cond_swap_ct (mp_int * a, mp_int * b, int c, int m)
{
    (void)c;
    if (m == 1)
        mp_exch(a, b);
    return MP_OKAY;
}


/* shift right a certain number of bits */
void mp_rshb (mp_int *c, int x)
{
    mp_digit *tmpc, mask, shift;
    mp_digit r, rr;
    mp_digit D = x;

    /* shifting by a negative number not supported, and shifting by
     * zero changes nothing.
     */
    if (x <= 0) return;

    /* shift digits first if needed */
    if (x >= DIGIT_BIT) {
        mp_rshd(c, x / DIGIT_BIT);
        /* recalculate number of bits to shift */
        D = x % DIGIT_BIT;
        /* check if any more shifting needed */
        if (D == 0) return;
    }

    /* zero shifted is always zero */
    if (mp_iszero(c)) return;

    /* mask */
    mask = (((mp_digit)1) << D) - 1;

    /* shift for lsb */
    shift = DIGIT_BIT - D;

    /* alias */
    tmpc = c->dp + (c->used - 1);

    /* carry */
    r = 0;
    for (x = c->used - 1; x >= 0; x--) {
      /* get the lower  bits of this word in a temp */
      rr = *tmpc & mask;

      /* shift the current word and mix in the carry bits from previous word */
      *tmpc = (*tmpc >> D) | (r << shift);
      --tmpc;

      /* set the carry to the carry bits of the current word found above */
      r = rr;
    }
    mp_clamp(c);
}


/* shift right a certain amount of digits */
void mp_rshd (mp_int * a, int b)
{
  int     x;

  /* if b <= 0 then ignore it */
  if (b <= 0) {
    return;
  }

  /* if b > used then simply zero it and return */
  if (a->used <= b) {
    mp_zero (a);
    return;
  }

  {
    mp_digit *bottom, *top;

    /* shift the digits down */

    /* bottom */
    bottom = a->dp;

    /* top [offset into digits] */
    top = a->dp + b;

    /* this is implemented as a sliding window where
     * the window is b-digits long and digits from
     * the top of the window are copied to the bottom
     *
     * e.g.

     b-2 | b-1 | b0 | b1 | b2 | ... | bb |   ---->
                 /\                   |      ---->
                  \-------------------/      ---->
     */
    for (x = 0; x < (a->used - b); x++) {
      *bottom++ = *top++;
    }

    /* zero the top digits */
    for (; x < a->used; x++) {
      *bottom++ = 0;
    }
  }

  /* remove excess digits */
  a->used -= b;
}


/* calc a value mod 2**b */
int mp_mod_2d (mp_int * a, int b, mp_int * c)
{
  int     x, res, bmax;

  /* if b is <= 0 then zero the int */
  if (b <= 0) {
    mp_zero (c);
    return MP_OKAY;
  }

  /* if the modulus is larger than the value than return */
  if (a->sign == MP_ZPOS && b >= (int) (a->used * DIGIT_BIT)) {
    res = mp_copy (a, c);
    return res;
  }

  /* copy */
  if ((res = mp_copy (a, c)) != MP_OKAY) {
    return res;
  }

  /* calculate number of digits in mod value */
  bmax = (b / DIGIT_BIT) + ((b % DIGIT_BIT) == 0 ? 0 : 1);
  /* zero digits above the last digit of the modulus */
  for (x = bmax; x < c->used; x++) {
    c->dp[x] = 0;
  }

  if (c->sign == MP_NEG) {
     mp_digit carry = 0;

     /* grow result to size of modulus */
     if ((res = mp_grow(c, bmax)) != MP_OKAY) {
         return res;
     }
     /* negate value */
     for (x = 0; x < c->used; x++) {
         mp_digit next = c->dp[x] > 0;
         c->dp[x] = ((mp_digit)0 - c->dp[x] - carry) & MP_MASK;
         carry |= next;
     }
     for (; x < bmax; x++) {
         c->dp[x] = ((mp_digit)0 - carry) & MP_MASK;
     }
     c->used = bmax;
     c->sign = MP_ZPOS;
  }

  /* clear the digit that is not completely outside/inside the modulus */
  x = DIGIT_BIT - (b % DIGIT_BIT);
  if (x != DIGIT_BIT) {
    c->dp[bmax - 1] &=
         ((mp_digit)~((mp_digit)0)) >> (x + ((sizeof(mp_digit)*8) - DIGIT_BIT));
  }
  mp_clamp (c);
  return MP_OKAY;
}


/* reads a unsigned char array, assumes the msb is stored first [big endian] */
int mp_read_unsigned_bin (mp_int * a, const unsigned char *b, int c)
{
  int     res;
  int     digits_needed;

  while (c > 0 && b[0] == 0) {
      c--;
      b++;
  }

  digits_needed = ((c * CHAR_BIT) + DIGIT_BIT - 1) / DIGIT_BIT;

  /* make sure there are enough digits available */
  if (a->alloc < digits_needed) {
     if ((res = mp_grow(a, digits_needed)) != MP_OKAY) {
        return res;
     }
  }

  /* zero the int */
  mp_zero (a);

  /* read the bytes in */
  while (c-- > 0) {
    if ((res = mp_mul_2d (a, 8, a)) != MP_OKAY) {
      return res;
    }

#ifndef MP_8BIT
      a->dp[0] |= *b++;
      if (a->used == 0)
          a->used = 1;
#else
      a->dp[0] = (*b & MP_MASK);
      a->dp[1] |= ((*b++ >> 7U) & 1);
      if (a->used == 0)
          a->used = 2;
#endif
  }
  mp_clamp (a);
  return MP_OKAY;
}


/* shift left by a certain bit count */
int mp_mul_2d (mp_int * a, int b, mp_int * c)
{
  mp_digit d;
  int      res;

  /* copy */
  if (a != c) {
     if ((res = mp_copy (a, c)) != MP_OKAY) {
       return res;
     }
  }

  if (c->alloc < (int)(c->used + b/DIGIT_BIT + 1)) {
     if ((res = mp_grow (c, c->used + b / DIGIT_BIT + 1)) != MP_OKAY) {
       return res;
     }
  }

  /* shift by as many digits in the bit count */
  if (b >= (int)DIGIT_BIT) {
    if ((res = mp_lshd (c, b / DIGIT_BIT)) != MP_OKAY) {
      return res;
    }
  }

  /* shift any bit count < DIGIT_BIT */
  d = (mp_digit) (b % DIGIT_BIT);
  if (d != 0) {
    mp_digit *tmpc, shift, mask, r, rr;
    int x;

    /* bitmask for carries */
    mask = (((mp_digit)1) << d) - 1;

    /* shift for msbs */
    shift = DIGIT_BIT - d;

    /* alias */
    tmpc = c->dp;

    /* carry */
    r    = 0;
    for (x = 0; x < c->used; x++) {
      /* get the higher bits of the current word */
      rr = (*tmpc >> shift) & mask;

      /* shift the current word and OR in the carry */
      *tmpc = (mp_digit)(((*tmpc << d) | r) & MP_MASK);
      ++tmpc;

      /* set the carry to the carry bits of the current word */
      r = rr;
    }

    /* set final carry */
    if (r != 0) {
       c->dp[(c->used)++] = r;
    }
  }
  mp_clamp (c);
  return MP_OKAY;
}


/* shift left a certain amount of digits */
int mp_lshd (mp_int * a, int b)
{
  int     x, res;

  /* if its less than zero return */
  if (b <= 0) {
    return MP_OKAY;
  }

  /* grow to fit the new digits */
  if (a->alloc < a->used + b) {
     if ((res = mp_grow (a, a->used + b)) != MP_OKAY) {
       return res;
     }
  }

  {
    mp_digit *top, *bottom;

    /* increment the used by the shift amount then copy upwards */
    a->used += b;

    /* top */
    top = a->dp + a->used - 1;

    /* base */
    bottom = a->dp + a->used - 1 - b;

    /* much like mp_rshd this is implemented using a sliding window
     * except the window goes the other way around.  Copying from
     * the bottom to the top.  see bn_mp_rshd.c for more info.
     */
    for (x = a->used - 1; x >= b; x--) {
      *top-- = *bottom--;
    }

    /* zero the lower digits */
    top = a->dp;
    for (x = 0; x < b; x++) {
      *top++ = 0;
    }
  }
  return MP_OKAY;
}


/* this is a shell function that calls either the normal or Montgomery
 * exptmod functions.  Originally the call to the montgomery code was
 * embedded in the normal function but that wasted a lot of stack space
 * for nothing (since 99% of the time the Montgomery code would be called)
 */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y)
#else
    int mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y) /* //NOLINT(misc-no-recursion) */
#endif
{
  int dr;

  /* modulus P must be positive */
  if (mp_iszero(P) || P->sign == MP_NEG) {
     return MP_VAL;
  }
  if (mp_isone(P)) {
     return mp_set(Y, 0);
  }
  if (mp_iszero(X)) {
     return mp_set(Y, 1);
  }
  if (mp_iszero(G)) {
     return mp_set(Y, 0);
  }

  /* if exponent X is negative we have to recurse */
  if (X->sign == MP_NEG) {
#ifdef BN_MP_INVMOD_C
     mp_int tmpG, tmpX;
     int err;

     /* first compute 1/G mod P */
     if ((err = mp_init(&tmpG)) != MP_OKAY) {
        return err;
     }
     if ((err = mp_invmod(G, P, &tmpG)) != MP_OKAY) {
        mp_clear(&tmpG);
        return err;
     }

     /* now get |X| */
     if ((err = mp_init(&tmpX)) != MP_OKAY) {
        mp_clear(&tmpG);
        return err;
     }
     if ((err = mp_abs(X, &tmpX)) != MP_OKAY) {
        mp_clear(&tmpG);
        mp_clear(&tmpX);
        return err;
     }

     /* and now compute (1/G)**|X| instead of G**X [X < 0] */
     err = mp_exptmod(&tmpG, &tmpX, P, Y);
     mp_clear(&tmpG);
     mp_clear(&tmpX);
     return err;
#else
     /* no invmod */
     return MP_VAL;
#endif
  }

#ifdef BN_MP_EXPTMOD_BASE_2
  if (G->used == 1 && G->dp[0] == 2 && mp_isodd(P) == MP_YES) {
    return mp_exptmod_base_2(X, P, Y);
  }
#endif

/* modified diminished radix reduction */
#if defined(BN_MP_REDUCE_IS_2K_L_C) && defined(BN_MP_REDUCE_2K_L_C) && \
  defined(BN_S_MP_EXPTMOD_C)
  if (mp_reduce_is_2k_l(P) == MP_YES) {
     return s_mp_exptmod(G, X, P, Y, 1);
  }
#endif

#ifdef BN_MP_DR_IS_MODULUS_C
  /* is it a DR modulus? */
  dr = mp_dr_is_modulus(P);
#else
  /* default to no */
  dr = 0;
#endif

  (void)dr;

#ifdef BN_MP_REDUCE_IS_2K_C
  /* if not, is it a unrestricted DR modulus? */
  if (dr == 0) {
     dr = mp_reduce_is_2k(P) << 1;
  }
#endif

  /* if the modulus is odd use the montgomery method, or use other known */
#ifdef BN_MP_EXPTMOD_FAST_C
  if (mp_isodd (P) == MP_YES || dr !=  0) {
    return mp_exptmod_fast (G, X, P, Y, dr);
  } else {
#endif
#ifdef BN_S_MP_EXPTMOD_C
    /* otherwise use the generic Barrett reduction technique */
    return s_mp_exptmod (G, X, P, Y, 0);
#else
    /* no exptmod for evens */
    return MP_VAL;
#endif
#ifdef BN_MP_EXPTMOD_FAST_C
  }
#endif
}

int mp_exptmod_ex (mp_int * G, mp_int * X, int digits, mp_int * P, mp_int * Y)
{
    (void)digits;
    return mp_exptmod(G, X, P, Y);
}

/* b = |a|
 *
 * Simple function copies the input and fixes the sign to positive
 */
int mp_abs (mp_int * a, mp_int * b)
{
  int     res;

  /* copy a to b */
  if (a != b) {
     if ((res = mp_copy (a, b)) != MP_OKAY) {
       return res;
     }
  }

  /* force the sign of b to positive */
  b->sign = MP_ZPOS;

  return MP_OKAY;
}


/* hac 14.61, pp608 */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_invmod(mp_int * a, mp_int * b, mp_int * c)
#else
int mp_invmod (mp_int * a, mp_int * b, mp_int * c)
#endif
{
  /* b cannot be negative or zero, and can not divide by 0 (1/a mod b) */
  if (b->sign == MP_NEG || mp_iszero(b) == MP_YES || mp_iszero(a) == MP_YES) {
    return MP_VAL;
  }

#ifdef BN_FAST_MP_INVMOD_C
  /* if the modulus is odd we can use a faster routine instead */
  if ((mp_isodd(b) == MP_YES) && (mp_cmp_d(b, 1) != MP_EQ)) {
    return fast_mp_invmod (a, b, c);
  }
#endif

#ifdef BN_MP_INVMOD_SLOW_C
  return mp_invmod_slow(a, b, c);
#else
  return MP_VAL;
#endif
}


/* computes the modular inverse via binary extended euclidean algorithm,
 * that is c = 1/a mod b
 *
 * Based on slow invmod except this is optimized for the case where b is
 * odd as per HAC Note 14.64 on pp. 610
 */
int fast_mp_invmod (mp_int * a, mp_int * b, mp_int * c)
{
  mp_int  x, y, u, v, B, D;
  int     res, loop_check = 0;

  /* 2. [modified] b must be odd   */
  if (mp_iseven (b) == MP_YES) {
    return MP_VAL;
  }

  /* init all our temps */
  if ((res = mp_init_multi(&x, &y, &u, &v, &B, &D)) != MP_OKAY) {
     return res;
  }

  /* x == modulus, y == value to invert */
  if ((res = mp_copy (b, &x)) != MP_OKAY) {
    goto LBL_ERR;
  }

  /* we need y = |a| */
  if ((res = mp_mod (a, b, &y)) != MP_OKAY) {
    goto LBL_ERR;
  }

  if (mp_iszero (&y) == MP_YES) {
    /* invmod doesn't exist for this a and b */
    res = MP_VAL;
    goto LBL_ERR;
  }

  /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
  if ((res = mp_copy (&x, &u)) != MP_OKAY) {
    goto LBL_ERR;
  }
  if ((res = mp_copy (&y, &v)) != MP_OKAY) {
    goto LBL_ERR;
  }
  if ((res = mp_set (&D, 1)) != MP_OKAY) {
    goto LBL_ERR;
  }

top:
  /* 4.  while u is even do */
  while (mp_iseven (&u) == MP_YES) {
    /* 4.1 u = u/2 */
    if ((res = mp_div_2 (&u, &u)) != MP_OKAY) {
      goto LBL_ERR;
    }
    /* 4.2 if B is odd then */
    if (mp_isodd (&B) == MP_YES) {
      if ((res = mp_sub (&B, &x, &B)) != MP_OKAY) {
        goto LBL_ERR;
      }
    }
    /* B = B/2 */
    if ((res = mp_div_2 (&B, &B)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }

  /* 5.  while v is even do */
  while (mp_iseven (&v) == MP_YES) {
    /* 5.1 v = v/2 */
    if ((res = mp_div_2 (&v, &v)) != MP_OKAY) {
      goto LBL_ERR;
    }
    /* 5.2 if D is odd then */
    if (mp_isodd (&D) == MP_YES) {
      /* D = (D-x)/2 */
      if ((res = mp_sub (&D, &x, &D)) != MP_OKAY) {
        goto LBL_ERR;
      }
    }
    /* D = D/2 */
    if ((res = mp_div_2 (&D, &D)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }

  /* 6.  if u >= v then */
  if (mp_cmp (&u, &v) != MP_LT) {
    /* u = u - v, B = B - D */
    if ((res = mp_sub (&u, &v, &u)) != MP_OKAY) {
      goto LBL_ERR;
    }

    if ((res = mp_sub (&B, &D, &B)) != MP_OKAY) {
      goto LBL_ERR;
    }
  } else {
    /* v - v - u, D = D - B */
    if ((res = mp_sub (&v, &u, &v)) != MP_OKAY) {
      goto LBL_ERR;
    }

    if ((res = mp_sub (&D, &B, &D)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }

  /* if not zero goto step 4 */
  if (mp_iszero (&u) == MP_NO) {
    if (++loop_check > MAX_INVMOD_SZ) {
        res = MP_VAL;
        goto LBL_ERR;
    }
    goto top;
  }

  /* now a = C, b = D, gcd == g*v */

  /* if v != 1 then there is no inverse */
  if (mp_cmp_d (&v, 1) != MP_EQ) {
    res = MP_VAL;
    goto LBL_ERR;
  }

  /* b is now the inverse */
  while (D.sign == MP_NEG) {
    if ((res = mp_add (&D, b, &D)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }
  /* too big */
  while (mp_cmp_mag(&D, b) != MP_LT) {
      if ((res = mp_sub(&D, b, &D)) != MP_OKAY) {
         goto LBL_ERR;
      }
  }
  mp_exch (&D, c);
  res = MP_OKAY;

LBL_ERR:mp_clear(&x);
        mp_clear(&y);
        mp_clear(&u);
        mp_clear(&v);
        mp_clear(&B);
        mp_clear(&D);
  return res;
}


/* hac 14.61, pp608 */
int mp_invmod_slow (mp_int * a, mp_int * b, mp_int * c)
{
  mp_int  x, y, u, v, A, B, C, D;
  int     res;

  /* b cannot be negative */
  if (b->sign == MP_NEG || mp_iszero(b) == MP_YES) {
    return MP_VAL;
  }

  /* init temps */
  if ((res = mp_init_multi(&x, &y, &u, &v,
                           &A, &B)) != MP_OKAY) {
    return res;
  }

  /* init rest of tmps temps */
  if ((res = mp_init_multi(&C, &D, 0, 0, 0, 0)) != MP_OKAY) {
    mp_clear(&x);
    mp_clear(&y);
    mp_clear(&u);
    mp_clear(&v);
    mp_clear(&A);
    mp_clear(&B);
    return res;
  }

  /* x = a, y = b */
  if ((res = mp_mod(a, b, &x)) != MP_OKAY) {
    goto LBL_ERR;
  }

  if (mp_iszero (&x) == MP_YES) {
    /* invmod doesn't exist for this a and b */
    res = MP_VAL;
    goto LBL_ERR;
  }

  if (mp_isone(&x)) {
    res = mp_set(c, 1);
    goto LBL_ERR;
  }
  if ((res = mp_copy (b, &y)) != MP_OKAY) {
    goto LBL_ERR;
  }

  /* 2. [modified] if x,y are both even then return an error! */
  if (mp_iseven (&x) == MP_YES && mp_iseven (&y) == MP_YES) {
    res = MP_VAL;
    goto LBL_ERR;
  }

  /* 3. u=x, v=y, A=1, B=0, C=0,D=1 */
  if ((res = mp_copy (&x, &u)) != MP_OKAY) {
    goto LBL_ERR;
  }
  if ((res = mp_copy (&y, &v)) != MP_OKAY) {
    goto LBL_ERR;
  }
  if ((res = mp_set (&A, 1)) != MP_OKAY) {
    goto LBL_ERR;
  }
  if ((res = mp_set (&D, 1)) != MP_OKAY) {
    goto LBL_ERR;
  }

top:
  /* 4.  while u is even do */
  while (mp_iseven (&u) == MP_YES) {
    /* 4.1 u = u/2 */
    if ((res = mp_div_2 (&u, &u)) != MP_OKAY) {
      goto LBL_ERR;
    }
    /* 4.2 if A or B is odd then */
    if (mp_isodd (&A) == MP_YES || mp_isodd (&B) == MP_YES) {
      /* A = (A+y)/2, B = (B-x)/2 */
      if ((res = mp_add (&A, &y, &A)) != MP_OKAY) {
        goto LBL_ERR;
      }
      if ((res = mp_sub (&B, &x, &B)) != MP_OKAY) {
        goto LBL_ERR;
      }
    }
    /* A = A/2, B = B/2 */
    if ((res = mp_div_2 (&A, &A)) != MP_OKAY) {
      goto LBL_ERR;
    }
    if ((res = mp_div_2 (&B, &B)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }

  /* 5.  while v is even do */
  while (mp_iseven (&v) == MP_YES) {
    /* 5.1 v = v/2 */
    if ((res = mp_div_2 (&v, &v)) != MP_OKAY) {
      goto LBL_ERR;
    }
    /* 5.2 if C or D is odd then */
    if (mp_isodd (&C) == MP_YES || mp_isodd (&D) == MP_YES) {
      /* C = (C+y)/2, D = (D-x)/2 */
      if ((res = mp_add (&C, &y, &C)) != MP_OKAY) {
        goto LBL_ERR;
      }
      if ((res = mp_sub (&D, &x, &D)) != MP_OKAY) {
        goto LBL_ERR;
      }
    }
    /* C = C/2, D = D/2 */
    if ((res = mp_div_2 (&C, &C)) != MP_OKAY) {
      goto LBL_ERR;
    }
    if ((res = mp_div_2 (&D, &D)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }

  /* 6.  if u >= v then */
  if (mp_cmp (&u, &v) != MP_LT) {
    /* u = u - v, A = A - C, B = B - D */
    if ((res = mp_sub (&u, &v, &u)) != MP_OKAY) {
      goto LBL_ERR;
    }

    if ((res = mp_sub (&A, &C, &A)) != MP_OKAY) {
      goto LBL_ERR;
    }

    if ((res = mp_sub (&B, &D, &B)) != MP_OKAY) {
      goto LBL_ERR;
    }
  } else {
    /* v - v - u, C = C - A, D = D - B */
    if ((res = mp_sub (&v, &u, &v)) != MP_OKAY) {
      goto LBL_ERR;
    }

    if ((res = mp_sub (&C, &A, &C)) != MP_OKAY) {
      goto LBL_ERR;
    }

    if ((res = mp_sub (&D, &B, &D)) != MP_OKAY) {
      goto LBL_ERR;
    }
  }

  /* if not zero goto step 4 */
  if (mp_iszero (&u) == MP_NO)
    goto top;

  /* now a = C, b = D, gcd == g*v */

  /* if v != 1 then there is no inverse */
  if (mp_cmp_d (&v, 1) != MP_EQ) {
    res = MP_VAL;
    goto LBL_ERR;
  }

  /* if its too low */
  while (mp_cmp_d(&C, 0) == MP_LT) {
      if ((res = mp_add(&C, b, &C)) != MP_OKAY) {
         goto LBL_ERR;
      }
  }

  /* too big */
  while (mp_cmp_mag(&C, b) != MP_LT) {
      if ((res = mp_sub(&C, b, &C)) != MP_OKAY) {
         goto LBL_ERR;
      }
  }

  /* C is now the inverse */
  mp_exch (&C, c);
  res = MP_OKAY;
LBL_ERR:mp_clear(&x);
        mp_clear(&y);
        mp_clear(&u);
        mp_clear(&v);
        mp_clear(&A);
        mp_clear(&B);
        mp_clear(&C);
        mp_clear(&D);
  return res;
}


/* compare magnitude of two ints (unsigned) */
int mp_cmp_mag (mp_int * a, mp_int * b)
{
  int     n;
  mp_digit *tmpa, *tmpb;

  /* compare based on # of non-zero digits */
  if (a->used > b->used) {
    return MP_GT;
  }

  if (a->used < b->used) {
    return MP_LT;
  }

  if (a->used == 0)
      return MP_EQ;

  /* alias for a */
  tmpa = a->dp + (a->used - 1);

  /* alias for b */
  tmpb = b->dp + (a->used - 1);

  /* compare based on digits  */
  for (n = 0; n < a->used; ++n, --tmpa, --tmpb) {
    if (*tmpa > *tmpb) {
      return MP_GT;
    }

    if (*tmpa < *tmpb) {
      return MP_LT;
    }
  }
  return MP_EQ;
}


/* compare two ints (signed)*/
int mp_cmp (mp_int * a, mp_int * b)
{
  /* compare based on sign */
  if (a->sign != b->sign) {
     if (a->sign == MP_NEG) {
        return MP_LT;
     } else {
        return MP_GT;
     }
  }

  /* compare digits */
  if (a->sign == MP_NEG) {
     /* if negative compare opposite direction */
     return mp_cmp_mag(b, a);
  } else {
     return mp_cmp_mag(a, b);
  }
}


/* compare a digit */
int mp_cmp_d(mp_int * a, mp_digit b)
{
  /* special case for zero*/
  if (a->used == 0 && b == 0)
    return MP_EQ;

  /* compare based on sign */
  if ((b && a->used == 0) || a->sign == MP_NEG) {
    return MP_LT;
  }

  /* compare based on magnitude */
  if (a->used > 1) {
    return MP_GT;
  }

  /* compare the only digit of a to b */
  if (a->dp[0] > b) {
    return MP_GT;
  } else if (a->dp[0] < b) {
    return MP_LT;
  } else {
    return MP_EQ;
  }
}


/* set to a digit */
int mp_set (mp_int * a, mp_digit b)
{
  int res;
  mp_zero (a);
  res = mp_grow (a, 1);
  if (res == MP_OKAY) {
    a->dp[0] = (mp_digit)(b & MP_MASK);
    a->used  = (a->dp[0] != 0) ? 1 : 0;
  }
  return res;
}

/* check if a bit is set */
int mp_is_bit_set (mp_int *a, mp_digit b)
{
    mp_digit i = b / DIGIT_BIT;  /* word index */
    mp_digit s = b % DIGIT_BIT;  /* bit index */

    if ((mp_digit)a->used <= i) {
        /* no words available at that bit count */
        return 0;
    }

    /* get word and shift bit to check down to index 0 */
    return (int)((a->dp[i] >> s) & (mp_digit)1);
}

/* c = a mod b, 0 <= c < b */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_mod(mp_int * a, mp_int * b, mp_int * c)
#else
int mp_mod (mp_int * a, mp_int * b, mp_int * c)
#endif
{
  mp_int  t;
  int     res;

  if ((res = mp_init_size (&t, b->used)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_div (a, b, NULL, &t)) != MP_OKAY) {
    mp_clear (&t);
    return res;
  }

  if ((mp_iszero(&t) != MP_NO) || (t.sign == b->sign)) {
    res = MP_OKAY;
    mp_exch (&t, c);
  } else {
    res = mp_add (b, &t, c);
  }

  mp_clear (&t);
  return res;
}


/* slower bit-bang division... also smaller */
int mp_div(mp_int * a, mp_int * b, mp_int * c, mp_int * d)
{
   mp_int ta, tb, tq, q;
   int    res, n, n2;

  /* is divisor zero ? */
  if (mp_iszero (b) == MP_YES) {
    return MP_VAL;
  }

  /* if a < b then q=0, r = a */
  if (mp_cmp_mag (a, b) == MP_LT) {
    if (d != NULL) {
      res = mp_copy (a, d);
    } else {
      res = MP_OKAY;
    }
    if (c != NULL) {
      mp_zero (c);
    }
    return res;
  }

  /* init our temps */
  if ((res = mp_init_multi(&ta, &tb, &tq, &q, 0, 0)) != MP_OKAY) {
     return res;
  }

  if ((res = mp_set(&tq, 1)) != MP_OKAY) {
     return res;
  }
  n = mp_count_bits(a) - mp_count_bits(b);
  if (((res = mp_abs(a, &ta)) != MP_OKAY) ||
      ((res = mp_abs(b, &tb)) != MP_OKAY) ||
      ((res = mp_mul_2d(&tb, n, &tb)) != MP_OKAY) ||
      ((res = mp_mul_2d(&tq, n, &tq)) != MP_OKAY)) {
      goto LBL_ERR;
  }

  while (n-- >= 0) {
     if (mp_cmp(&tb, &ta) != MP_GT) {
        if (((res = mp_sub(&ta, &tb, &ta)) != MP_OKAY) ||
            ((res = mp_add(&q, &tq, &q)) != MP_OKAY)) {
           goto LBL_ERR;
        }
     }
     if (((res = mp_div_2d(&tb, 1, &tb, NULL)) != MP_OKAY) ||
         ((res = mp_div_2d(&tq, 1, &tq, NULL)) != MP_OKAY)) {
           goto LBL_ERR;
     }
  }

  /* now q == quotient and ta == remainder */
  n  = a->sign;
  n2 = (a->sign == b->sign ? MP_ZPOS : MP_NEG);
  if (c != NULL) {
     mp_exch(c, &q);
     c->sign  = (mp_iszero(c) == MP_YES) ? MP_ZPOS : n2;
  }
  if (d != NULL) {
     mp_exch(d, &ta);
     d->sign = (mp_iszero(d) == MP_YES) ? MP_ZPOS : n;
  }
LBL_ERR:
   mp_clear(&ta);
   mp_clear(&tb);
   mp_clear(&tq);
   mp_clear(&q);
   return res;
}


/* b = a/2 */
int mp_div_2(mp_int * a, mp_int * b)
{
  int     x, res, oldused;

  /* copy */
  if (b->alloc < a->used) {
    if ((res = mp_grow (b, a->used)) != MP_OKAY) {
      return res;
    }
  }

  oldused = b->used;
  b->used = a->used;
  {
    mp_digit r, rr, *tmpa, *tmpb;

    /* source alias */
    tmpa = a->dp + b->used - 1;

    /* dest alias */
    tmpb = b->dp + b->used - 1;

    /* carry */
    r = 0;
    for (x = b->used - 1; x >= 0; x--) {
      /* get the carry for the next iteration */
      rr = *tmpa & 1;

      /* shift the current digit, add in carry and store */
      *tmpb-- = (*tmpa-- >> 1) | (r << (DIGIT_BIT - 1));

      /* forward carry to next iteration */
      r = rr;
    }

    /* zero excess digits */
    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  b->sign = a->sign;
  mp_clamp (b);
  return MP_OKAY;
}

/* c = a / 2 (mod b) - constant time (a < b and positive) */
int mp_div_2_mod_ct(mp_int *a, mp_int *b, mp_int *c)
{
    int res;

    if (mp_isodd(a)) {
        res = mp_add(a, b, c);
        if (res == MP_OKAY) {
            res = mp_div_2(c, c);
        }
    }
    else {
        res = mp_div_2(a, c);
    }

    return res;
}


/* high level addition (handles signs) */
int mp_add (mp_int * a, mp_int * b, mp_int * c)
{
  int sa, sb, res;

  /* get sign of both inputs */
  sa = a->sign;
  sb = b->sign;

  /* handle two cases, not four */
  if (sa == sb) {
    /* both positive or both negative */
    /* add their magnitudes, copy the sign */
    c->sign = sa;
    res = s_mp_add (a, b, c);
  } else {
    /* one positive, the other negative */
    /* subtract the one with the greater magnitude from */
    /* the one of the lesser magnitude.  The result gets */
    /* the sign of the one with the greater magnitude. */
    if (mp_cmp_mag (a, b) == MP_LT) {
      c->sign = sb;
      res = s_mp_sub (b, a, c);
    } else {
      c->sign = sa;
      res = s_mp_sub (a, b, c);
    }
  }
  return res;
}


/* low level addition, based on HAC pp.594, Algorithm 14.7 */
int s_mp_add (mp_int * a, mp_int * b, mp_int * c)
{
  mp_int *x;
  int     olduse, res, min_ab, max_ab;

  /* find sizes, we let |a| <= |b| which means we have to sort
   * them.  "x" will point to the input with the most digits
   */
  if (a->used > b->used) {
    min_ab = b->used;
    max_ab = a->used;
    x = a;
  } else {
    min_ab = a->used;
    max_ab = b->used;
    x = b;
  }

  /* init result */
  if (c->dp == NULL || c->alloc < max_ab + 1) {
    if ((res = mp_grow (c, max_ab + 1)) != MP_OKAY) {
      return res;
    }
  }

  /* get old used digit count and set new one */
  olduse = c->used;
  c->used = max_ab + 1;

  {
    mp_digit u, *tmpa, *tmpb, *tmpc;
    int i;

    /* alias for digit pointers */

    /* first input */
    tmpa = a->dp;

    /* second input */
    tmpb = b->dp;

    /* destination */
    tmpc = c->dp;

    /* zero the carry */
    u = 0;
    for (i = 0; i < min_ab; i++) {
      /* Compute the sum at one digit, T[i] = A[i] + B[i] + U */
      *tmpc = *tmpa++ + *tmpb++ + u;

      /* U = carry bit of T[i] */
      u = *tmpc >> ((mp_digit)DIGIT_BIT);

      /* take away carry bit from T[i] */
      *tmpc++ &= MP_MASK;
    }

    /* now copy higher words if any, that is in A+B
     * if A or B has more digits add those in
     */
    if (min_ab != max_ab) {
      for (; i < max_ab; i++) {
        /* T[i] = X[i] + U */
          *tmpc = x->dp[i] + u;

        /* U = carry bit of T[i] */
        u = *tmpc >> ((mp_digit)DIGIT_BIT);

        /* take away carry bit from T[i] */
        *tmpc++ &= MP_MASK;
      }
    }

    /* add carry */
    *tmpc++ = u;

    /* clear digits above olduse */
    for (i = c->used; i < olduse; i++) {
      *tmpc++ = 0;
    }
  }

  mp_clamp (c);
  return MP_OKAY;
}


/* low level subtraction (assumes |a| > |b|), HAC pp.595 Algorithm 14.9 */
int s_mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
  int     olduse, res, min_b, max_a;

  /* find sizes */
  min_b = b->used;
  max_a = a->used;

  /* init result */
  if (c->alloc < max_a) {
    if ((res = mp_grow (c, max_a)) != MP_OKAY) {
      return res;
    }
  }

  /* sanity check on destination */
  if (c->dp == NULL)
     return MP_VAL;

  olduse = c->used;
  c->used = max_a;

  {
    mp_digit u, *tmpa, *tmpb, *tmpc;
    int i;

    /* alias for digit pointers */
    tmpa = a->dp;
    tmpb = b->dp;
    tmpc = c->dp;

    /* set carry to zero */
    u = 0;
    for (i = 0; i < min_b; i++) {
      /* T[i] = A[i] - B[i] - U */
      *tmpc = *tmpa++ - *tmpb++ - u;

      /* U = carry bit of T[i]
       * Note this saves performing an AND operation since
       * if a carry does occur it will propagate all the way to the
       * MSB.  As a result a single shift is enough to get the carry
       */
      u = *tmpc >> ((mp_digit)(CHAR_BIT * sizeof (mp_digit) - 1));

      /* Clear carry from T[i] */
      *tmpc++ &= MP_MASK;
    }

    /* now copy higher words if any, e.g. if A has more digits than B  */
    for (; i < max_a; i++) {
      /* T[i] = A[i] - U */
      *tmpc = *tmpa++ - u;

      /* U = carry bit of T[i] */
      u = *tmpc >> ((mp_digit)(CHAR_BIT * sizeof (mp_digit) - 1));

      /* Clear carry from T[i] */
      *tmpc++ &= MP_MASK;
    }

    /* clear digits above used (since we may not have grown result above) */
    for (i = c->used; i < olduse; i++) {
      *tmpc++ = 0;
    }
  }

  mp_clamp (c);
  return MP_OKAY;
}


/* high level subtraction (handles signs) */
int mp_sub (mp_int * a, mp_int * b, mp_int * c)
{
  int     sa, sb, res;

  sa = a->sign;
  sb = b->sign;

  if (sa != sb) {
    /* subtract a negative from a positive, OR */
    /* subtract a positive from a negative. */
    /* In either case, ADD their magnitudes, */
    /* and use the sign of the first number. */
    c->sign = sa;
    res = s_mp_add (a, b, c);
  } else {
    /* subtract a positive from a positive, OR */
    /* subtract a negative from a negative. */
    /* First, take the difference between their */
    /* magnitudes, then... */
    if (mp_cmp_mag (a, b) != MP_LT) {
      /* Copy the sign from the first */
      c->sign = sa;
      /* The first has a larger or equal magnitude */
      res = s_mp_sub (a, b, c);
    } else {
      /* The result has the *opposite* sign from */
      /* the first number. */
      c->sign = (sa == MP_ZPOS) ? MP_NEG : MP_ZPOS;
      /* The second has a larger magnitude */
      res = s_mp_sub (b, a, c);
    }
  }
  return res;
}


/* determines if reduce_2k_l can be used */
int mp_reduce_is_2k_l(mp_int *a)
{
   int ix, iy;

   if (a->used == 0) {
      return MP_NO;
   } else if (a->used == 1) {
      return MP_YES;
   } else if (a->used > 1) {
      /* if more than half of the digits are -1 we're sold */
      for (iy = ix = 0; ix < a->used; ix++) {
          if (a->dp[ix] == MP_MASK) {
              ++iy;
          }
      }
      return (iy >= (a->used/2)) ? MP_YES : MP_NO;

   }
   return MP_NO;
}


/* determines if mp_reduce_2k can be used */
int mp_reduce_is_2k(mp_int *a)
{
   int ix, iy, iw;
   mp_digit iz;

   if (a->used == 0) {
      return MP_NO;
   } else if (a->used == 1) {
      return MP_YES;
   } else if (a->used > 1) {
      iy = mp_count_bits(a);
      iz = 1;
      iw = 1;

      /* Test every bit from the second digit up, must be 1 */
      for (ix = DIGIT_BIT; ix < iy; ix++) {
          if ((a->dp[iw] & iz) == 0) {
             return MP_NO;
          }
          iz <<= 1;
          if (iz > (mp_digit)MP_MASK) {
             ++iw;
             iz = 1;
          }
      }
   }
   return MP_YES;
}


/* determines if a number is a valid DR modulus */
int mp_dr_is_modulus(mp_int *a)
{
   int ix;

   /* must be at least two digits */
   if (a->used < 2) {
      return 0;
   }

   /* must be of the form b**k - a [a <= b] so all
    * but the first digit must be equal to -1 (mod b).
    */
   for (ix = 1; ix < a->used; ix++) {
       if (a->dp[ix] != MP_MASK) {
          return 0;
       }
   }
   return 1;
}

/* computes Y == G**X mod P, HAC pp.616, Algorithm 14.85
 *
 * Uses a left-to-right k-ary sliding window to compute the modular
 * exponentiation.
 * The value of k changes based on the size of the exponent.
 *
 * Uses Montgomery or Diminished Radix reduction [whichever appropriate]
 */

#ifdef MP_LOW_MEM
   #define TAB_SIZE 32
#else
   #define TAB_SIZE 256
#endif

int mp_exptmod_fast (mp_int * G, mp_int * X, mp_int * P, mp_int * Y,
                     int redmode)
{
  mp_int res;
  mp_digit buf, mp;
  int     err, bitbuf, bitcpy, bitcnt, mode, digidx, x, y, winsize;
#ifdef WOLFSSL_SMALL_STACK
  mp_int* M;
#else
  mp_int M[TAB_SIZE];
#endif
  /* use a pointer to the reduction algorithm.  This allows us to use
   * one of many reduction algorithms without modding the guts of
   * the code with if statements everywhere.
   */
  int     (*redux)(mp_int*,mp_int*,mp_digit) = NULL;

#ifdef WOLFSSL_SMALL_STACK
  M = (mp_int*) XMALLOC(sizeof(mp_int) * TAB_SIZE, NULL,
                                                       DYNAMIC_TYPE_BIGINT);
  if (M == NULL)
    return MP_MEM;
#endif

  /* find window size */
  x = mp_count_bits (X);
  if (x <= 7) {
    winsize = 2;
  } else if (x <= 36) {
    winsize = 3;
  } else if (x <= 140) {
    winsize = 4;
  } else if (x <= 450) {
    winsize = 5;
  } else if (x <= 1303) {
    winsize = 6;
  } else if (x <= 3529) {
    winsize = 7;
  } else {
    winsize = 8;
  }

#ifdef MP_LOW_MEM
  if (winsize > 5) {
     winsize = 5;
  }
#endif

  /* init M array */
  /* init first cell */
  if ((err = mp_init_size(&M[1], P->alloc)) != MP_OKAY) {
#ifdef WOLFSSL_SMALL_STACK
     XFREE(M, NULL, DYNAMIC_TYPE_BIGINT);
#endif

     return err;
  }

  /* now init the second half of the array */
  for (x = 1<<(winsize-1); x < (1 << winsize); x++) {
    if ((err = mp_init_size(&M[x], P->alloc)) != MP_OKAY) {
      for (y = 1<<(winsize-1); y < x; y++) {
        mp_clear (&M[y]);
      }
      mp_clear(&M[1]);

#ifdef WOLFSSL_SMALL_STACK
      XFREE(M, NULL, DYNAMIC_TYPE_BIGINT);
#endif

      return err;
    }
  }

  /* determine and setup reduction code */
  if (redmode == 0) {
#ifdef BN_MP_MONTGOMERY_SETUP_C
     /* now setup montgomery  */
     if ((err = mp_montgomery_setup (P, &mp)) != MP_OKAY) {
        goto LBL_M;
     }
#else
     err = MP_VAL;
     goto LBL_M;
#endif

     /* automatically pick the comba one if available (saves quite a few
        calls/ifs) */
#ifdef BN_FAST_MP_MONTGOMERY_REDUCE_C
     if (((P->used * 2 + 1) < (int)MP_WARRAY) &&
          P->used < (1L << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
        redux = fast_mp_montgomery_reduce;
     } else
#endif
     {
#ifdef BN_MP_MONTGOMERY_REDUCE_C
        /* use slower baseline Montgomery method */
        redux = mp_montgomery_reduce;
#endif
     }
  } else if (redmode == 1) {
#if defined(BN_MP_DR_SETUP_C) && defined(BN_MP_DR_REDUCE_C)
     /* setup DR reduction for moduli of the form B**k - b */
     mp_dr_setup(P, &mp);
     redux = mp_dr_reduce;
#endif
  } else {
#if defined(BN_MP_REDUCE_2K_SETUP_C) && defined(BN_MP_REDUCE_2K_C)
     /* setup DR reduction for moduli of the form 2**k - b */
     if ((err = mp_reduce_2k_setup(P, &mp)) != MP_OKAY) {
        goto LBL_M;
     }
     /* mp of zero is not usable */
     if (mp != 0) {
         redux = mp_reduce_2k;
     }
#endif
  }

  if (redux == NULL) {
     err = MP_VAL;
     goto LBL_M;
  }

  /* setup result */
  if ((err = mp_init_size (&res, P->alloc)) != MP_OKAY) {
    goto LBL_M;
  }

  /* create M table
   *

   *
   * The first half of the table is not computed though accept for M[0] and M[1]
   */

  if (redmode == 0) {
#ifdef BN_MP_MONTGOMERY_CALC_NORMALIZATION_C
     /* now we need R mod m */
     if ((err = mp_montgomery_calc_normalization (&res, P)) != MP_OKAY) {
       goto LBL_RES;
     }

     /* now set M[1] to G * R mod m */
     if ((err = mp_mulmod (G, &res, P, &M[1])) != MP_OKAY) {
       goto LBL_RES;
     }
#else
     err = MP_VAL;
     goto LBL_RES;
#endif
  } else {
     if ((err = mp_set(&res, 1)) != MP_OKAY) {
        goto LBL_RES;
     }
     if ((err = mp_mod(G, P, &M[1])) != MP_OKAY) {
        goto LBL_RES;
     }
  }

  /* compute the value at M[1<<(winsize-1)] by squaring M[1] (winsize-1) times*/
  if ((err = mp_copy (&M[1], &M[(mp_digit)(1 << (winsize - 1))])) != MP_OKAY) {
    goto LBL_RES;
  }

  for (x = 0; x < (winsize - 1); x++) {
    if ((err = mp_sqr (&M[(mp_digit)(1 << (winsize - 1))],
                       &M[(mp_digit)(1 << (winsize - 1))])) != MP_OKAY) {
      goto LBL_RES;
    }
    if ((err = redux (&M[(mp_digit)(1 << (winsize - 1))], P, mp)) != MP_OKAY) {
      goto LBL_RES;
    }
  }

  /* create upper table */
  for (x = (1 << (winsize - 1)) + 1; x < (1 << winsize); x++) {
    if ((err = mp_mul (&M[x - 1], &M[1], &M[x])) != MP_OKAY) {
      goto LBL_RES;
    }
    if ((err = redux (&M[x], P, mp)) != MP_OKAY) {
      goto LBL_RES;
    }
  }

  /* set initial mode and bit cnt */
  mode   = 0;
  bitcnt = 1;
  buf    = 0;
  digidx = X->used - 1;
  bitcpy = 0;
  bitbuf = 0;

  for (;;) {
    /* grab next digit as required */
    if (--bitcnt == 0) {
      /* if digidx == -1 we are out of digits so break */
      if (digidx == -1) {
        break;
      }
      /* read next digit and reset bitcnt */
      buf    = X->dp[digidx--];
      bitcnt = (int)DIGIT_BIT;
    }

    /* grab the next msb from the exponent */
    y     = (int)(buf >> (DIGIT_BIT - 1)) & 1;
    buf <<= (mp_digit)1;

    /* if the bit is zero and mode == 0 then we ignore it
     * These represent the leading zero bits before the first 1 bit
     * in the exponent.  Technically this opt is not required but it
     * does lower the # of trivial squaring/reductions used
     */
    if (mode == 0 && y == 0) {
      continue;
    }

    /* if the bit is zero and mode == 1 then we square */
    if (mode == 1 && y == 0) {
      if ((err = mp_sqr (&res, &res)) != MP_OKAY) {
        goto LBL_RES;
      }
      if ((err = redux (&res, P, mp)) != MP_OKAY) {
        goto LBL_RES;
      }
      continue;
    }

    /* else we add it to the window */
    bitbuf |= (y << (winsize - ++bitcpy));
    mode    = 2;

    if (bitcpy == winsize) {
      /* ok window is filled so square as required and multiply  */
      /* square first */
      for (x = 0; x < winsize; x++) {
        if ((err = mp_sqr (&res, &res)) != MP_OKAY) {
          goto LBL_RES;
        }
        if ((err = redux (&res, P, mp)) != MP_OKAY) {
          goto LBL_RES;
        }
      }

      /* then multiply */
      if ((err = mp_mul (&res, &M[bitbuf], &res)) != MP_OKAY) {
        goto LBL_RES;
      }
      if ((err = redux (&res, P, mp)) != MP_OKAY) {
        goto LBL_RES;
      }

      /* empty window and reset */
      bitcpy = 0;
      bitbuf = 0;
      mode   = 1;
    }
  }

  /* if bits remain then square/multiply */
  if (mode == 2 && bitcpy > 0) {
    /* square then multiply if the bit is set */
    for (x = 0; x < bitcpy; x++) {
      if ((err = mp_sqr (&res, &res)) != MP_OKAY) {
        goto LBL_RES;
      }
      if ((err = redux (&res, P, mp)) != MP_OKAY) {
        goto LBL_RES;
      }

      /* get next bit of the window */
      bitbuf <<= 1;
      if ((bitbuf & (1 << winsize)) != 0) {
        /* then multiply */
        if ((err = mp_mul (&res, &M[1], &res)) != MP_OKAY) {
          goto LBL_RES;
        }
        if ((err = redux (&res, P, mp)) != MP_OKAY) {
          goto LBL_RES;
        }
      }
    }
  }

  if (redmode == 0) {
     /* fixup result if Montgomery reduction is used
      * recall that any value in a Montgomery system is
      * actually multiplied by R mod n.  So we have
      * to reduce one more time to cancel out the factor
      * of R.
      */
     if ((err = redux(&res, P, mp)) != MP_OKAY) {
       goto LBL_RES;
     }
  }

  /* swap res with Y */
  mp_exch (&res, Y);
  err = MP_OKAY;
LBL_RES:mp_clear (&res);
LBL_M:
  mp_clear(&M[1]);
  for (x = 1<<(winsize-1); x < (1 << winsize); x++) {
    mp_clear (&M[x]);
  }

#ifdef WOLFSSL_SMALL_STACK
  XFREE(M, NULL, DYNAMIC_TYPE_BIGINT);
#endif

  return err;
}

#ifdef BN_MP_EXPTMOD_BASE_2
#if DIGIT_BIT < 16
    #define WINSIZE    3
#elif DIGIT_BIT < 32
    #define WINSIZE    4
#elif DIGIT_BIT < 64
    #define WINSIZE    5
#elif DIGIT_BIT < 128
    #define WINSIZE    6
#endif
int mp_exptmod_base_2(mp_int * X, mp_int * P, mp_int * Y)
{
  mp_digit buf, mp;
  int      err = MP_OKAY, bitbuf, bitcpy, bitcnt, digidx, x, y;
  mp_int   res[1];
  int     (*redux)(mp_int*,mp_int*,mp_digit) = NULL;

  /* automatically pick the comba one if available (saves quite a few
     calls/ifs) */
#ifdef BN_FAST_MP_MONTGOMERY_REDUCE_C
  if (((P->used * 2 + 1) < (int)MP_WARRAY) &&
       P->used < (1L << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
     redux = fast_mp_montgomery_reduce;
  } else
#endif
#ifdef BN_MP_MONTGOMERY_REDUCE_C
  {
     /* use slower baseline Montgomery method */
     redux = mp_montgomery_reduce;
  }
#endif

  if (redux == NULL) {
      return MP_VAL;
  }

  /* now setup montgomery  */
  if ((err = mp_montgomery_setup(P, &mp)) != MP_OKAY) {
     goto LBL_M;
  }

  /* setup result */
  if ((err = mp_init(res)) != MP_OKAY) {
     goto LBL_M;
  }

  /* now we need R mod m */
  if ((err = mp_montgomery_calc_normalization(res, P)) != MP_OKAY) {
     goto LBL_RES;
  }

  /* Get the top bits left over after taking WINSIZE bits starting at the
   * least-significant.
   */
  digidx = X->used - 1;
  bitcpy = (X->used * DIGIT_BIT) % WINSIZE;
  if (bitcpy > 0) {
     bitcnt = (int)DIGIT_BIT - bitcpy;
     buf    = X->dp[digidx--];
     bitbuf = (int)(buf >> bitcnt);
     /* Multiply montgomery representation of 1 by 2 ^ top */
     err = mp_mul_2d(res, bitbuf, res);
     if (err != MP_OKAY) {
        goto LBL_RES;
     }
     err = mp_mod(res, P, res);
     if (err != MP_OKAY) {
        goto LBL_RES;
     }
     /* Move out bits used */
     buf  <<= bitcpy;
     bitcnt++;
  }
  else {
     bitcnt = 1;
     buf    = 0;
  }

  /* empty window and reset  */
  bitbuf = 0;
  bitcpy = 0;

  for (;;) {
    /* grab next digit as required */
    if (--bitcnt == 0) {
      /* if digidx == -1 we are out of digits so break */
      if (digidx == -1) {
        break;
      }
      /* read next digit and reset bitcnt */
      buf    = X->dp[digidx--];
      bitcnt = (int)DIGIT_BIT;
    }

    /* grab the next msb from the exponent */
    y       = (int)(buf >> (DIGIT_BIT - 1)) & 1;
    buf   <<= (mp_digit)1;
    /* add bit to the window */
    bitbuf |= (y << (WINSIZE - ++bitcpy));

    if (bitcpy == WINSIZE) {
      /* ok window is filled so square as required and multiply  */
      /* square first */
      for (x = 0; x < WINSIZE; x++) {
        err = mp_sqr(res, res);
        if (err != MP_OKAY) {
          goto LBL_RES;
        }
        err = (*redux)(res, P, mp);
        if (err != MP_OKAY) {
          goto LBL_RES;
        }
      }

      /* then multiply by 2^bitbuf */
      err = mp_mul_2d(res, bitbuf, res);
      if (err != MP_OKAY) {
         goto LBL_RES;
      }
      err = mp_mod(res, P, res);
      if (err != MP_OKAY) {
         goto LBL_RES;
      }

      /* empty window and reset */
      bitcpy = 0;
      bitbuf = 0;
    }
  }

  /* fixup result if Montgomery reduction is used
   * recall that any value in a Montgomery system is
   * actually multiplied by R mod n.  So we have
   * to reduce one more time to cancel out the factor
   * of R.
   */
  err = (*redux)(res, P, mp);
  if (err != MP_OKAY) {
     goto LBL_RES;
  }

  /* swap res with Y */
  err = mp_copy(res, Y);

LBL_RES:mp_clear (res);
LBL_M:
  return err;
}

#undef WINSIZE
#endif /* BN_MP_EXPTMOD_BASE_2 */


/* setups the montgomery reduction stuff */
int mp_montgomery_setup (mp_int * n, mp_digit * rho)
{
  mp_digit x, b;

/* fast inversion mod 2**k
 *
 * Based on the fact that
 *
 * XA = 1 (mod 2**n)  =>  (X(2-XA)) A = 1 (mod 2**2n)
 *                    =>  2*X*A - X*X*A*A = 1
 *                    =>  2*(1) - (1)     = 1
 */
  b = n->dp[0];

  if ((b & 1) == 0) {
    return MP_VAL;
  }

  x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
  x *= 2 - b * x;               /* here x*a==1 mod 2**8 */
#if !defined(MP_8BIT)
  x *= 2 - b * x;               /* here x*a==1 mod 2**16 */
#endif
#if defined(MP_64BIT) || !(defined(MP_8BIT) || defined(MP_16BIT))
  x *= 2 - b * x;               /* here x*a==1 mod 2**32 */
#endif
#ifdef MP_64BIT
  x *= 2 - b * x;               /* here x*a==1 mod 2**64 */
#endif

  /* rho = -1/m mod b */
  /* TAO, switched mp_word casts to mp_digit to shut up compiler */
  *rho = (mp_digit)((((mp_digit)1 << ((mp_digit) DIGIT_BIT)) - x) & MP_MASK);

  return MP_OKAY;
}


/* computes xR**-1 == x (mod N) via Montgomery Reduction
 *
 * This is an optimized implementation of montgomery_reduce
 * which uses the comba method to quickly calculate the columns of the
 * reduction.
 *
 * Based on Algorithm 14.32 on pp.601 of HAC.
*/
int fast_mp_montgomery_reduce (mp_int * x, mp_int * n, mp_digit rho)
{
  int     ix, res, olduse;
#ifdef WOLFSSL_SMALL_STACK
  mp_word* W;    /* uses dynamic memory and slower */
#else
  mp_word W[MP_WARRAY];
#endif

  /* get old used count */
  olduse = x->used;

  /* grow a as required */
  if (x->alloc < n->used + 1) {
    if ((res = mp_grow (x, n->used + 1)) != MP_OKAY) {
      return res;
    }
  }

#ifdef WOLFSSL_SMALL_STACK
  W = (mp_word*)XMALLOC(sizeof(mp_word) * (n->used * 2 + 2), NULL,
    DYNAMIC_TYPE_BIGINT);
  if (W == NULL)
    return MP_MEM;
#endif

  XMEMSET(W, 0, sizeof(mp_word) * (n->used * 2 + 2));

  /* first we have to get the digits of the input into
   * an array of double precision words W[...]
   */
  {
    mp_word *_W;
    mp_digit *tmpx;

    /* alias for the W[] array */
    _W   = W;

    /* alias for the digits of  x*/
    tmpx = x->dp;

    /* copy the digits of a into W[0..a->used-1] */
    for (ix = 0; ix < x->used; ix++) {
      *_W++ = *tmpx++;
    }
  }

  /* now we proceed to zero successive digits
   * from the least significant upwards
   */
  for (ix = 0; ix < n->used; ix++) {
    /* mu = ai * m' mod b
     *
     * We avoid a double precision multiplication (which isn't required)
     * by casting the value down to a mp_digit.  Note this requires
     * that W[ix-1] have  the carry cleared (see after the inner loop)
     */
    mp_digit mu;
    mu = (mp_digit) (((W[ix] & MP_MASK) * rho) & MP_MASK);

    /* a = a + mu * m * b**i
     *
     * This is computed in place and on the fly.  The multiplication
     * by b**i is handled by offsetting which columns the results
     * are added to.
     *
     * Note the comba method normally doesn't handle carries in the
     * inner loop In this case we fix the carry from the previous
     * column since the Montgomery reduction requires digits of the
     * result (so far) [see above] to work.  This is
     * handled by fixing up one carry after the inner loop.  The
     * carry fixups are done in order so after these loops the
     * first m->used words of W[] have the carries fixed
     */
    {
      int iy;
      mp_digit *tmpn;
      mp_word *_W;

      /* alias for the digits of the modulus */
      tmpn = n->dp;

      /* Alias for the columns set by an offset of ix */
      _W = W + ix;

      /* inner loop */
      for (iy = 0; iy < n->used; iy++) {
          *_W++ += ((mp_word)mu) * ((mp_word)*tmpn++);
      }
    }

    /* now fix carry for next digit, W[ix+1] */
    W[ix + 1] += W[ix] >> ((mp_word) DIGIT_BIT);
  }

  /* now we have to propagate the carries and
   * shift the words downward [all those least
   * significant digits we zeroed].
   */
  {
    mp_digit *tmpx;
    mp_word *_W, *_W1;

    /* nox fix rest of carries */

    /* alias for current word */
    _W1 = W + ix;

    /* alias for next word, where the carry goes */
    _W = W + ++ix;

    for (; ix <= n->used * 2 + 1; ix++) {
      *_W++ += *_W1++ >> ((mp_word) DIGIT_BIT);
    }

    /* copy out, A = A/b**n
     *
     * The result is A/b**n but instead of converting from an
     * array of mp_word to mp_digit than calling mp_rshd
     * we just copy them in the right order
     */

    /* alias for destination word */
    tmpx = x->dp;

    /* alias for shifted double precision result */
    _W = W + n->used;

    for (ix = 0; ix < n->used + 1; ix++) {
      *tmpx++ = (mp_digit)(*_W++ & ((mp_word) MP_MASK));
    }

    /* zero olduse digits, if the input a was larger than
     * m->used+1 we'll have to clear the digits
     */
    for (; ix < olduse; ix++) {
      *tmpx++ = 0;
    }
  }

  /* set the max used and clamp */
  x->used = n->used + 1;
  mp_clamp (x);

#ifdef WOLFSSL_SMALL_STACK
  XFREE(W, NULL, DYNAMIC_TYPE_BIGINT);
#endif

  /* if A >= m then A = A - m */
  if (mp_cmp_mag (x, n) != MP_LT) {
    return s_mp_sub (x, n, x);
  }
  return MP_OKAY;
}


/* computes xR**-1 == x (mod N) via Montgomery Reduction */
int mp_montgomery_reduce (mp_int * x, mp_int * n, mp_digit rho)
{
  int     ix, res, digs;
  mp_digit mu;

  /* can the fast reduction [comba] method be used?
   *
   * Note that unlike in mul you're safely allowed *less*
   * than the available columns [255 per default] since carries
   * are fixed up in the inner loop.
   */
  digs = n->used * 2 + 1;
  if ((digs < (int)MP_WARRAY) &&
      n->used <
      (1L << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
    return fast_mp_montgomery_reduce (x, n, rho);
  }

  /* grow the input as required */
  if (x->alloc < digs) {
    if ((res = mp_grow (x, digs)) != MP_OKAY) {
      return res;
    }
  }
  x->used = digs;

  for (ix = 0; ix < n->used; ix++) {
    /* mu = ai * rho mod b
     *
     * The value of rho must be precalculated via
     * montgomery_setup() such that
     * it equals -1/n0 mod b this allows the
     * following inner loop to reduce the
     * input one digit at a time
     */
    mu = (mp_digit) (((mp_word)x->dp[ix]) * ((mp_word)rho) & MP_MASK);

    /* a = a + mu * m * b**i */
    {
      int iy;
      mp_digit *tmpn, *tmpx, u;
      mp_word r;

      /* alias for digits of the modulus */
      tmpn = n->dp;

      /* alias for the digits of x [the input] */
      tmpx = x->dp + ix;

      /* set the carry to zero */
      u = 0;

      /* Multiply and add in place */
      for (iy = 0; iy < n->used; iy++) {
        /* compute product and sum */
        r       = ((mp_word)mu) * ((mp_word)*tmpn++) +
                  ((mp_word) u) + ((mp_word) * tmpx);

        /* get carry */
        u       = (mp_digit)(r >> ((mp_word) DIGIT_BIT));

        /* fix digit */
        *tmpx++ = (mp_digit)(r & ((mp_word) MP_MASK));
      }
      /* At this point the ix'th digit of x should be zero */


      /* propagate carries upwards as required*/
      while (u) {
        *tmpx   += u;
        u        = *tmpx >> DIGIT_BIT;
        *tmpx++ &= MP_MASK;
      }
    }
  }

  /* at this point the n.used'th least
   * significant digits of x are all zero
   * which means we can shift x to the
   * right by n.used digits and the
   * residue is unchanged.
   */

  /* x = x/b**n.used */
  mp_clamp(x);
  mp_rshd (x, n->used);

  /* if x >= n then x = x - n */
  if (mp_cmp_mag (x, n) != MP_LT) {
    return s_mp_sub (x, n, x);
  }

  return MP_OKAY;
}


/* determines the setup value */
void mp_dr_setup(mp_int *a, mp_digit *d)
{
   /* the casts are required if DIGIT_BIT is one less than
    * the number of bits in a mp_digit [e.g. DIGIT_BIT==31]
    */
   *d = (mp_digit)((((mp_word)1) << ((mp_word)DIGIT_BIT)) -
        ((mp_word)a->dp[0]));
}


/* reduce "x" in place modulo "n" using the Diminished Radix algorithm.
 *
 * Based on algorithm from the paper
 *
 * "Generating Efficient Primes for Discrete Log Cryptosystems"
 *                 Chae Hoon Lim, Pil Joong Lee,
 *          POSTECH Information Research Laboratories
 *
 * The modulus must be of a special format [see manual]
 *
 * Has been modified to use algorithm 7.10 from the LTM book instead
 *
 * Input x must be in the range 0 <= x <= (n-1)**2
 */
int mp_dr_reduce (mp_int * x, mp_int * n, mp_digit k)
{
  int      err, i, m;
  mp_word  r;
  mp_digit mu, *tmpx1, *tmpx2;

  /* m = digits in modulus */
  m = n->used;

  /* ensure that "x" has at least 2m digits */
  if (x->alloc < m + m) {
    if ((err = mp_grow (x, m + m)) != MP_OKAY) {
      return err;
    }
  }

/* top of loop, this is where the code resumes if
 * another reduction pass is required.
 */
top:
  /* aliases for digits */
  /* alias for lower half of x */
  tmpx1 = x->dp;

  /* alias for upper half of x, or x/B**m */
  tmpx2 = x->dp + m;

  /* set carry to zero */
  mu = 0;

  /* compute (x mod B**m) + k * [x/B**m] inline and inplace */
  for (i = 0; i < m; i++) {
      r         = ((mp_word)*tmpx2++) * ((mp_word)k) + *tmpx1 + mu;
      *tmpx1++  = (mp_digit)(r & MP_MASK);
      mu        = (mp_digit)(r >> ((mp_word)DIGIT_BIT));
  }

  /* set final carry */
  *tmpx1++ = mu;

  /* zero words above m */
  for (i = m + 1; i < x->used; i++) {
      *tmpx1++ = 0;
  }

  /* clamp, sub and return */
  mp_clamp (x);

  /* if x >= n then subtract and reduce again
   * Each successive "recursion" makes the input smaller and smaller.
   */
  if (mp_cmp_mag (x, n) != MP_LT) {
    if ((err = s_mp_sub(x, n, x)) != MP_OKAY) {
        return err;
    }
    goto top;
  }
  return MP_OKAY;
}


/* reduces a modulo n where n is of the form 2**p - d */
int mp_reduce_2k(mp_int *a, mp_int *n, mp_digit d)
{
   mp_int q;
   int    p, res;

   if ((res = mp_init(&q)) != MP_OKAY) {
      return res;
   }

   p = mp_count_bits(n);
top:
   /* q = a/2**p, a = a mod 2**p */
   if ((res = mp_div_2d(a, p, &q, a)) != MP_OKAY) {
      goto ERR;
   }

   if (d != 1) {
      /* q = q * d */
      if ((res = mp_mul_d(&q, d, &q)) != MP_OKAY) {
         goto ERR;
      }
   }

   /* a = a + q */
   if ((res = s_mp_add(a, &q, a)) != MP_OKAY) {
      goto ERR;
   }

   if (mp_cmp_mag(a, n) != MP_LT) {
      if ((res = s_mp_sub(a, n, a)) != MP_OKAY) {
         goto ERR;
      }
      goto top;
   }

ERR:
   mp_clear(&q);
   return res;
}


/* determines the setup value */
int mp_reduce_2k_setup(mp_int *a, mp_digit *d)
{
   int res, p;
   mp_int tmp;

   if ((res = mp_init(&tmp)) != MP_OKAY) {
      return res;
   }

   p = mp_count_bits(a);
   if ((res = mp_2expt(&tmp, p)) != MP_OKAY) {
      mp_clear(&tmp);
      return res;
   }

   if ((res = s_mp_sub(&tmp, a, &tmp)) != MP_OKAY) {
      mp_clear(&tmp);
      return res;
   }

   *d = tmp.dp[0];
   mp_clear(&tmp);
   return MP_OKAY;
}


/* set the b bit of a */
int mp_set_bit (mp_int * a, int b)
{
    int i = b / DIGIT_BIT, res;

    /*
     * Require:
     *  bit index b >= 0
     *  a->alloc == a->used == 0 if a->dp == NULL
     */
    if (b < 0 || (a->dp == NULL && (a->alloc != 0 || a->used != 0)))
        return MP_VAL;

    if (a->dp == NULL || a->used < (int)(i + 1)) {
        /* grow a to accommodate the single bit */
        if ((res = mp_grow (a, i + 1)) != MP_OKAY) {
            return res;
        }

        /* set the used count of where the bit will go */
        a->used = (int)(i + 1);
    }

    /* put the single bit in its place */
    a->dp[i] |= ((mp_digit)1) << (b % DIGIT_BIT);

    return MP_OKAY;
}

/* computes a = 2**b
 *
 * Simple algorithm which zeros the int, set the required bit
 */
int mp_2expt (mp_int * a, int b)
{
    /* zero a as per default */
    mp_zero (a);

    return mp_set_bit(a, b);
}

/* multiply by a digit */
int mp_mul_d (mp_int * a, mp_digit b, mp_int * c)
{
  mp_digit u, *tmpa, *tmpc;
  mp_word  r;
  int      ix, res, olduse;

  /* make sure c is big enough to hold a*b */
  if (c->dp == NULL || c->alloc < a->used + 1) {
    if ((res = mp_grow (c, a->used + 1)) != MP_OKAY) {
      return res;
    }
  }

  /* get the original destinations used count */
  olduse = c->used;

  /* set the sign */
  c->sign = a->sign;

  /* alias for a->dp [source] */
  tmpa = a->dp;

  /* alias for c->dp [dest] */
  tmpc = c->dp;

  /* zero carry */
  u = 0;

  /* compute columns */
  for (ix = 0; ix < a->used; ix++) {
    /* compute product and carry sum for this term */
    r       = ((mp_word) u) + ((mp_word)*tmpa++) * ((mp_word)b);

    /* mask off higher bits to get a single digit */
    *tmpc++ = (mp_digit) (r & ((mp_word) MP_MASK));

    /* send carry into next iteration */
    u       = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
  }

  /* store final carry [if any] and increment ix offset  */
  *tmpc++ = u;
  ++ix;

  /* now zero digits above the top */
  while (ix++ < olduse) {
     *tmpc++ = 0;
  }

  /* set used count */
  c->used = a->used + 1;
  mp_clamp(c);

  return MP_OKAY;
}


/* d = a * b (mod c) */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_mulmod(mp_int *a, mp_int *b, mp_int *c, mp_int *d)
#else
int mp_mulmod (mp_int * a, mp_int * b, mp_int * c, mp_int * d)
#endif
{
  int     res;
  mp_int  t;

  if ((res = mp_init_size (&t, c->used)) != MP_OKAY) {
    return res;
  }

  res = mp_mul (a, b, &t);
  if (res == MP_OKAY) {
      res = mp_mod (&t, c, d);
  }

  mp_clear (&t);
  return res;
}


/* d = a - b (mod c) */
int mp_submod(mp_int* a, mp_int* b, mp_int* c, mp_int* d)
{
  int     res;
  mp_int  t;

  if ((res = mp_init (&t)) != MP_OKAY) {
    return res;
  }

  res = mp_sub (a, b, &t);
  if (res == MP_OKAY) {
      res = mp_mod (&t, c, d);
  }

  mp_clear (&t);

  return res;
}

/* d = a + b (mod c) */
int mp_addmod(mp_int* a, mp_int* b, mp_int* c, mp_int* d)
{
   int     res;
   mp_int  t;

   if ((res = mp_init (&t)) != MP_OKAY) {
     return res;
   }

   res = mp_add (a, b, &t);
   if (res == MP_OKAY) {
       res = mp_mod (&t, c, d);
   }

   mp_clear (&t);

   return res;
}

/* d = a - b (mod c) - a < c and b < c and positive */
int mp_submod_ct(mp_int* a, mp_int* b, mp_int* c, mp_int* d)
{
    int res;

    res = mp_sub(a, b, d);
    if (res == MP_OKAY && mp_isneg(d)) {
        res = mp_add(d, c, d);
    }

    return res;
}

/* d = a + b (mod c) - a < c and b < c and positive */
int mp_addmod_ct(mp_int* a, mp_int* b, mp_int* c, mp_int* d)
{
    int res;

    res = mp_add(a, b, d);
    if (res == MP_OKAY && mp_cmp(d, c) != MP_LT) {
        res = mp_sub(d, c, d);
    }

    return res;
}

/* computes b = a*a */
int mp_sqr (mp_int * a, mp_int * b)
{
  int     res;

  {
#ifdef BN_FAST_S_MP_SQR_C
    /* can we use the fast comba multiplier? */
    if ((a->used * 2 + 1) < (int)MP_WARRAY &&
         a->used <
         (1 << (sizeof(mp_word) * CHAR_BIT - 2*DIGIT_BIT - 1))) {
      res = fast_s_mp_sqr (a, b);
    } else
#endif
#ifdef BN_S_MP_SQR_C
      res = s_mp_sqr (a, b);
#else
      res = MP_VAL;
#endif
  }
  b->sign = MP_ZPOS;
  return res;
}


/* high level multiplication (handles sign) */
#if defined(FREESCALE_LTC_TFM)
int wolfcrypt_mp_mul(mp_int *a, mp_int *b, mp_int *c)
#else
int mp_mul (mp_int * a, mp_int * b, mp_int * c)
#endif
{
  int     res, neg;
  neg = (a->sign == b->sign) ? MP_ZPOS : MP_NEG;

  {
#ifdef BN_FAST_S_MP_MUL_DIGS_C
    /* can we use the fast multiplier?
     *
     * The fast multiplier can be used if the output will
     * have less than MP_WARRAY digits and the number of
     * digits won't affect carry propagation
     */
    int     digs = a->used + b->used + 1;

    if ((digs < (int)MP_WARRAY) &&
        MIN(a->used, b->used) <=
        (1L << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
      res = fast_s_mp_mul_digs (a, b, c, digs);
    } else
#endif
#ifdef BN_S_MP_MUL_DIGS_C
      res = s_mp_mul (a, b, c); /* uses s_mp_mul_digs */
#else
      res = MP_VAL;
#endif

  }
  c->sign = (c->used > 0) ? neg : MP_ZPOS;
  return res;
}


/* b = a*2 */
int mp_mul_2(mp_int * a, mp_int * b)
{
  int     x, res, oldused;

  /* grow to accommodate result */
  if (b->alloc < a->used + 1) {
    if ((res = mp_grow (b, a->used + 1)) != MP_OKAY) {
      return res;
    }
  }

  oldused = b->used;
  b->used = a->used;

  {
    mp_digit r, rr, *tmpa, *tmpb;

    /* alias for source */
    tmpa = a->dp;

    /* alias for dest */
    tmpb = b->dp;

    /* carry */
    r = 0;
    for (x = 0; x < a->used; x++) {

      /* get what will be the *next* carry bit from the
       * MSB of the current digit
       */
      rr = *tmpa >> ((mp_digit)(DIGIT_BIT - 1));

      /* now shift up this digit, add in the carry [from the previous] */
      *tmpb++ = (mp_digit)(((*tmpa++ << ((mp_digit)1)) | r) & MP_MASK);

      /* copy the carry that would be from the source
       * digit into the next iteration
       */
      r = rr;
    }

    /* new leading digit? */
    if (r != 0) {
      /* add a MSB which is always 1 at this point */
      *tmpb = 1;
      ++(b->used);
    }

    /* now zero any excess digits on the destination
     * that we didn't write to
     */
    tmpb = b->dp + b->used;
    for (x = b->used; x < oldused; x++) {
      *tmpb++ = 0;
    }
  }
  b->sign = a->sign;
  return MP_OKAY;
}


/* divide by three (based on routine from MPI and the GMP manual) */
int mp_div_3 (mp_int * a, mp_int *c, mp_digit * d)
{
  mp_int   q;
  mp_word  w, t;
  mp_digit b;
  int      res, ix;

  /* b = 2**DIGIT_BIT / 3 */
  b = (mp_digit) ( (((mp_word)1) << ((mp_word)DIGIT_BIT)) / ((mp_word)3) );

  if ((res = mp_init_size(&q, a->used)) != MP_OKAY) {
     return res;
  }

  q.used = a->used;
  q.sign = a->sign;
  w = 0;
  for (ix = a->used - 1; ix >= 0; ix--) {
     w = (w << ((mp_word)DIGIT_BIT)) | ((mp_word)a->dp[ix]);

     if (w >= 3) {
        /* multiply w by [1/3] */
        t = (w * ((mp_word)b)) >> ((mp_word)DIGIT_BIT);

        /* now subtract 3 * [w/3] from w, to get the remainder */
        w -= t+t+t;

        /* fixup the remainder as required since
         * the optimization is not exact.
         */
        while (w >= 3) {
           t += 1;
           w -= 3;
        }
      } else {
        t = 0;
      }
      q.dp[ix] = (mp_digit)t;
  }

  /* [optional] store the remainder */
  if (d != NULL) {
     *d = (mp_digit)w;
  }

  /* [optional] store the quotient */
  if (c != NULL) {
     mp_clamp(&q);
     mp_exch(&q, c);
  }
  mp_clear(&q);

  return res;
}


/* init an mp_init for a given size */
int mp_init_size (mp_int * a, int size)
{
  int x;

  /* pad size so there are always extra digits */
  size += (MP_PREC * 2) - (size % MP_PREC);

  /* alloc mem */
  a->dp = (mp_digit *)XMALLOC (sizeof (mp_digit) * size, NULL,
                                      DYNAMIC_TYPE_BIGINT);
  if (a->dp == NULL) {
    return MP_MEM;
  }

  /* set the members */
  a->used  = 0;
  a->alloc = size;
  a->sign  = MP_ZPOS;
#ifdef HAVE_WOLF_BIGINT
  wc_bigint_init(&a->raw);
#endif

  /* zero the digits */
  for (x = 0; x < size; x++) {
      a->dp[x] = 0;
  }

  return MP_OKAY;
}


/* the list of squaring...
 * you do like mult except the offset of the tmpx [one that
 * starts closer to zero] can't equal the offset of tmpy.
 * So basically you set up iy like before then you min it with
 * (ty-tx) so that it never happens.  You double all those
 * you add in the inner loop

After that loop you do the squares and add them in.
*/

int fast_s_mp_sqr (mp_int * a, mp_int * b)
{
  int       olduse, res, pa, ix, iz;
#ifdef WOLFSSL_SMALL_STACK
  mp_digit* W;    /* uses dynamic memory and slower */
#else
  mp_digit W[MP_WARRAY];
#endif
  mp_digit  *tmpx;
  mp_word   W1;

  /* grow the destination as required */
  pa = a->used + a->used;
  if (b->alloc < pa) {
    if ((res = mp_grow (b, pa)) != MP_OKAY) {
      return res;
    }
  }

  if (pa > (int)MP_WARRAY)
    return MP_RANGE;  /* TAO range check */

  if (pa == 0) {
    /* Nothing to do. Zero result and return. */
    mp_zero(b);
    return MP_OKAY;
  }

#ifdef WOLFSSL_SMALL_STACK
  W = (mp_digit*)XMALLOC(sizeof(mp_digit) * pa, NULL, DYNAMIC_TYPE_BIGINT);
  if (W == NULL)
    return MP_MEM;
#endif

  /* number of output digits to produce */
  W1 = 0;
  for (ix = 0; ix < pa; ix++) {
      int      tx, ty, iy;
      mp_word  _W;
      mp_digit *tmpy;

      /* clear counter */
      _W = 0;

      /* get offsets into the two bignums */
      ty = MIN(a->used-1, ix);
      tx = ix - ty;

      /* setup temp aliases */
      tmpx = a->dp + tx;
      tmpy = a->dp + ty;

      /* this is the number of times the loop will iterate, essentially
         while (tx++ < a->used && ty-- >= 0) { ... }
       */
      iy = MIN(a->used-tx, ty+1);

      /* now for squaring tx can never equal ty
       * we halve the distance since they approach at a rate of 2x
       * and we have to round because odd cases need to be executed
       */
      iy = MIN(iy, (ty-tx+1)>>1);

      /* execute loop */
      for (iz = 0; iz < iy; iz++) {
         _W += ((mp_word)*tmpx++)*((mp_word)*tmpy--);
      }

      /* double the inner product and add carry */
      _W = _W + _W + W1;

      /* even columns have the square term in them */
      if ((ix&1) == 0) {
         _W += ((mp_word)a->dp[ix>>1])*((mp_word)a->dp[ix>>1]);
      }

      /* store it */
      W[ix] = (mp_digit)(_W & MP_MASK);

      /* make next carry */
      W1 = _W >> ((mp_word)DIGIT_BIT);
  }

  /* setup dest */
  olduse  = b->used;
  b->used = a->used+a->used;

  {
    mp_digit *tmpb;
    tmpb = b->dp;
    for (ix = 0; ix < pa; ix++) {
      *tmpb++ = (mp_digit)(W[ix] & MP_MASK);
    }

    /* clear unused digits [that existed in the old copy of c] */
    for (; ix < olduse; ix++) {
      *tmpb++ = 0;
    }
  }
  mp_clamp (b);

#ifdef WOLFSSL_SMALL_STACK
  XFREE(W, NULL, DYNAMIC_TYPE_BIGINT);
#endif

  return MP_OKAY;
}


/* Fast (comba) multiplier
 *
 * This is the fast column-array [comba] multiplier.  It is
 * designed to compute the columns of the product first
 * then handle the carries afterwards.  This has the effect
 * of making the nested loops that compute the columns very
 * simple and schedulable on super-scalar processors.
 *
 * This has been modified to produce a variable number of
 * digits of output so if say only a half-product is required
 * you don't have to compute the upper half (a feature
 * required for fast Barrett reduction).
 *
 * Based on Algorithm 14.12 on pp.595 of HAC.
 *
 */
int fast_s_mp_mul_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
  int     olduse, res, pa, ix, iz;
#ifdef WOLFSSL_SMALL_STACK
  mp_digit* W;    /* uses dynamic memory and slower */
#else
  mp_digit W[MP_WARRAY];
#endif
  mp_word  _W;

  /* grow the destination as required */
  if (c->alloc < digs) {
    if ((res = mp_grow (c, digs)) != MP_OKAY) {
      return res;
    }
  }

  /* number of output digits to produce */
  pa = MIN(digs, a->used + b->used);
  if (pa > (int)MP_WARRAY)
    return MP_RANGE;  /* TAO range check */

  if (pa == 0) {
    /* Nothing to do. Zero result and return. */
    mp_zero(c);
    return MP_OKAY;
  }

#ifdef WOLFSSL_SMALL_STACK
  W = (mp_digit*)XMALLOC(sizeof(mp_digit) * pa, NULL, DYNAMIC_TYPE_BIGINT);
  if (W == NULL)
    return MP_MEM;
#endif

  /* clear the carry */
  _W = 0;
  for (ix = 0; ix < pa; ix++) {
      int      tx, ty;
      int      iy;
      mp_digit *tmpx, *tmpy;

      if ((a->used > 0) && (b->used > 0)) {
          /* get offsets into the two bignums */
          ty = MIN(b->used-1, ix);
          tx = ix - ty;

          /* setup temp aliases */
          tmpx = a->dp + tx;
          tmpy = b->dp + ty;

          /* this is the number of times the loop will iterate, essentially
             while (tx++ < a->used && ty-- >= 0) { ... }
          */
          iy = MIN(a->used-tx, ty+1);

          /* execute loop */
          for (iz = 0; iz < iy; ++iz) {
              _W += ((mp_word)*tmpx++)*((mp_word)*tmpy--);

          }
      }

      /* store term */
      W[ix] = (mp_digit)(((mp_digit)_W) & MP_MASK);

      /* make next carry */
      _W = _W >> ((mp_word)DIGIT_BIT);
 }

  /* setup dest */
  olduse  = c->used;
  c->used = pa;

  {
    mp_digit *tmpc;
    tmpc = c->dp;
    for (ix = 0; ix < pa; ix++) { /* JRB, +1 could read uninitialized data */
      /* now extract the previous digit [below the carry] */
      *tmpc++ = W[ix];
    }

    /* clear unused digits [that existed in the old copy of c] */
    for (; ix < olduse; ix++) {
      *tmpc++ = 0;
    }
  }
  mp_clamp (c);

#ifdef WOLFSSL_SMALL_STACK
  XFREE(W, NULL, DYNAMIC_TYPE_BIGINT);
#endif

  return MP_OKAY;
}


/* low level squaring, b = a*a, HAC pp.596-597, Algorithm 14.16 */
int s_mp_sqr (mp_int * a, mp_int * b)
{
  mp_int  t;
  int     res, ix, iy, pa;
  mp_word r;
  mp_digit u, tmpx, *tmpt;

  pa = a->used;
  if ((res = mp_init_size (&t, 2*pa + 1)) != MP_OKAY) {
    return res;
  }

  /* default used is maximum possible size */
  t.used = 2*pa + 1;

  for (ix = 0; ix < pa; ix++) {
    /* first calculate the digit at 2*ix */
    /* calculate double precision result */
    r = ((mp_word) t.dp[2*ix]) +
        ((mp_word)a->dp[ix])*((mp_word)a->dp[ix]);

    /* store lower part in result */
    t.dp[ix+ix] = (mp_digit) (r & ((mp_word) MP_MASK));

    /* get the carry */
    u           = (mp_digit)(r >> ((mp_word) DIGIT_BIT));

    /* left hand side of A[ix] * A[iy] */
    tmpx        = a->dp[ix];

    /* alias for where to store the results */
    tmpt        = t.dp + (2*ix + 1);

    for (iy = ix + 1; iy < pa; iy++) {
      /* first calculate the product */
      r       = ((mp_word)tmpx) * ((mp_word)a->dp[iy]);

      /* now calculate the double precision result, note we use
       * addition instead of *2 since it's easier to optimize
       */
      r       = ((mp_word) *tmpt) + r + r + ((mp_word) u);

      /* store lower part */
      *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));

      /* get carry */
      u       = (mp_digit)(r >> ((mp_word) DIGIT_BIT));
    }
    /* propagate upwards */
    while (u != ((mp_digit) 0)) {
      r       = ((mp_word) *tmpt) + ((mp_word) u);
      *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));
      u       = (mp_digit)(r >> ((mp_word) DIGIT_BIT));
    }
  }

  mp_clamp (&t);
  mp_exch (&t, b);
  mp_clear (&t);
  return MP_OKAY;
}


/* multiplies |a| * |b| and only computes up to digs digits of result
 * HAC pp. 595, Algorithm 14.12  Modified so you can control how
 * many digits of output are created.
 */
int s_mp_mul_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
  mp_int  t;
  int     res, pa, pb, ix, iy;
  mp_digit u;
  mp_word r;
  mp_digit tmpx, *tmpt, *tmpy;

  /* can we use the fast multiplier? */
  if ((digs < (int)MP_WARRAY) &&
      MIN (a->used, b->used) <
          (1L << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
    return fast_s_mp_mul_digs (a, b, c, digs);
  }

  if ((res = mp_init_size (&t, digs)) != MP_OKAY) {
    return res;
  }
  t.used = digs;

  /* compute the digits of the product directly */
  pa = a->used;
  for (ix = 0; ix < pa; ix++) {
    /* set the carry to zero */
    u = 0;

    /* limit ourselves to making digs digits of output */
    pb = MIN (b->used, digs - ix);

    /* setup some aliases */
    /* copy of the digit from a used within the nested loop */
    tmpx = a->dp[ix];

    /* an alias for the destination shifted ix places */
    tmpt = t.dp + ix;

    /* an alias for the digits of b */
    tmpy = b->dp;

    /* compute the columns of the output and propagate the carry */
    for (iy = 0; iy < pb; iy++) {
      /* compute the column as a mp_word */
      r       = ((mp_word)*tmpt) +
                ((mp_word)tmpx) * ((mp_word)*tmpy++) +
                ((mp_word) u);

      /* the new column is the lower part of the result */
      *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));

      /* get the carry word from the result */
      u       = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
    }
    /* set carry if it is placed below digs */
    if (ix + iy < digs) {
      *tmpt = u;
    }
  }

  mp_clamp (&t);
  mp_exch (&t, c);

  mp_clear (&t);
  return MP_OKAY;
}


/*
 * shifts with subtractions when the result is greater than b.
 *
 * The method is slightly modified to shift B unconditionally up to just under
 * the leading bit of b.  This saves a lot of multiple precision shifting.
 */
int mp_montgomery_calc_normalization (mp_int * a, mp_int * b)
{
  int     x, bits, res;

  /* how many bits of last digit does b use */
  bits = mp_count_bits (b) % DIGIT_BIT;

  if (b->used > 1) {
     if ((res = mp_2expt (a, (b->used - 1) * DIGIT_BIT + bits - 1))
         != MP_OKAY) {
        return res;
     }
  } else {
     if ((res = mp_set(a, 1)) != MP_OKAY) {
        return res;
     }
     bits = 1;
  }

  /* now compute C = A * B mod b */
  for (x = bits - 1; x < (int)DIGIT_BIT; x++) {
    if ((res = mp_mul_2 (a, a)) != MP_OKAY) {
      return res;
    }
    if (mp_cmp_mag (a, b) != MP_LT) {
      if ((res = s_mp_sub (a, b, a)) != MP_OKAY) {
        return res;
      }
    }
  }

  return MP_OKAY;
}


#ifdef MP_LOW_MEM
   #define TAB_SIZE 32
#else
   #define TAB_SIZE 256
#endif

int s_mp_exptmod (mp_int * G, mp_int * X, mp_int * P, mp_int * Y, int redmode)
{
  mp_int  M[TAB_SIZE], res, mu;
  mp_digit buf;
  int     err, bitbuf, bitcpy, bitcnt, mode, digidx, x, y, winsize;
  int (*redux)(mp_int*,mp_int*,mp_int*);

  /* find window size */
  x = mp_count_bits (X);
  if (x <= 7) {
    winsize = 2;
  } else if (x <= 36) {
    winsize = 3;
  } else if (x <= 140) {
    winsize = 4;
  } else if (x <= 450) {
    winsize = 5;
  } else if (x <= 1303) {
    winsize = 6;
  } else if (x <= 3529) {
    winsize = 7;
  } else {
    winsize = 8;
  }

#ifdef MP_LOW_MEM
    if (winsize > 5) {
       winsize = 5;
    }
#endif

  /* init M array */
  /* init first cell */
  if ((err = mp_init(&M[1])) != MP_OKAY) {
     return err;
  }

  /* now init the second half of the array */
  for (x = 1<<(winsize-1); x < (1 << winsize); x++) {
    if ((err = mp_init(&M[x])) != MP_OKAY) {
      for (y = 1<<(winsize-1); y < x; y++) {
        mp_clear (&M[y]);
      }
      mp_clear(&M[1]);
      return err;
    }
  }

  /* create mu, used for Barrett reduction */
  if ((err = mp_init (&mu)) != MP_OKAY) {
    goto LBL_M;
  }

  if (redmode == 0) {
     if ((err = mp_reduce_setup (&mu, P)) != MP_OKAY) {
        goto LBL_MU;
     }
     redux = mp_reduce;
  } else {
     if ((err = mp_reduce_2k_setup_l (P, &mu)) != MP_OKAY) {
        goto LBL_MU;
     }
     redux = mp_reduce_2k_l;
  }

  /* create M table
   *
   * The M table contains powers of the base,
   * e.g. M[x] = G**x mod P
   *
   * The first half of the table is not
   * computed though accept for M[0] and M[1]
   */
  if ((err = mp_mod (G, P, &M[1])) != MP_OKAY) {
    goto LBL_MU;
  }

  /* compute the value at M[1<<(winsize-1)] by squaring
   * M[1] (winsize-1) times
   */
  if ((err = mp_copy (&M[1], &M[(mp_digit)(1 << (winsize - 1))])) != MP_OKAY) {
    goto LBL_MU;
  }

  for (x = 0; x < (winsize - 1); x++) {
    /* square it */
    if ((err = mp_sqr (&M[(mp_digit)(1 << (winsize - 1))],
                       &M[(mp_digit)(1 << (winsize - 1))])) != MP_OKAY) {
      goto LBL_MU;
    }

    /* reduce modulo P */
    if ((err = redux (&M[(mp_digit)(1 << (winsize - 1))], P, &mu)) != MP_OKAY) {
      goto LBL_MU;
    }
  }

  /* create upper table, that is M[x] = M[x-1] * M[1] (mod P)
   * for x = (2**(winsize - 1) + 1) to (2**winsize - 1)
   */
  for (x = (1 << (winsize - 1)) + 1; x < (1 << winsize); x++) {
    if ((err = mp_mul (&M[x - 1], &M[1], &M[x])) != MP_OKAY) {
      goto LBL_MU;
    }
    if ((err = redux (&M[x], P, &mu)) != MP_OKAY) {
      goto LBL_MU;
    }
  }

  /* setup result */
  if ((err = mp_init (&res)) != MP_OKAY) {
    goto LBL_MU;
  }
  if ((err = mp_set (&res, 1)) != MP_OKAY) {
    goto LBL_MU;
  }

  /* set initial mode and bit cnt */
  mode   = 0;
  bitcnt = 1;
  buf    = 0;
  digidx = X->used - 1;
  bitcpy = 0;
  bitbuf = 0;

  for (;;) {
    /* grab next digit as required */
    if (--bitcnt == 0) {
      /* if digidx == -1 we are out of digits */
      if (digidx == -1) {
        break;
      }
      /* read next digit and reset the bitcnt */
      buf    = X->dp[digidx--];
      bitcnt = (int) DIGIT_BIT;
    }

    /* grab the next msb from the exponent */
    y     = (int)(buf >> (mp_digit)(DIGIT_BIT - 1)) & 1;
    buf <<= (mp_digit)1;

    /* if the bit is zero and mode == 0 then we ignore it
     * These represent the leading zero bits before the first 1 bit
     * in the exponent.  Technically this opt is not required but it
     * does lower the # of trivial squaring/reductions used
     */
    if (mode == 0 && y == 0) {
      continue;
    }

    /* if the bit is zero and mode == 1 then we square */
    if (mode == 1 && y == 0) {
      if ((err = mp_sqr (&res, &res)) != MP_OKAY) {
        goto LBL_RES;
      }
      if ((err = redux (&res, P, &mu)) != MP_OKAY) {
        goto LBL_RES;
      }
      continue;
    }

    /* else we add it to the window */
    bitbuf |= (y << (winsize - ++bitcpy));
    mode    = 2;

    if (bitcpy == winsize) {
      /* ok window is filled so square as required and multiply  */
      /* square first */
      for (x = 0; x < winsize; x++) {
        if ((err = mp_sqr (&res, &res)) != MP_OKAY) {
          goto LBL_RES;
        }
        if ((err = redux (&res, P, &mu)) != MP_OKAY) {
          goto LBL_RES;
        }
      }

      /* then multiply */
      if ((err = mp_mul (&res, &M[bitbuf], &res)) != MP_OKAY) {
        goto LBL_RES;
      }
      if ((err = redux (&res, P, &mu)) != MP_OKAY) {
        goto LBL_RES;
      }

      /* empty window and reset */
      bitcpy = 0;
      bitbuf = 0;
      mode   = 1;
    }
  }

  /* if bits remain then square/multiply */
  if (mode == 2 && bitcpy > 0) {
    /* square then multiply if the bit is set */
    for (x = 0; x < bitcpy; x++) {
      if ((err = mp_sqr (&res, &res)) != MP_OKAY) {
        goto LBL_RES;
      }
      if ((err = redux (&res, P, &mu)) != MP_OKAY) {
        goto LBL_RES;
      }

      bitbuf <<= 1;
      if ((bitbuf & (1 << winsize)) != 0) {
        /* then multiply */
        if ((err = mp_mul (&res, &M[1], &res)) != MP_OKAY) {
          goto LBL_RES;
        }
        if ((err = redux (&res, P, &mu)) != MP_OKAY) {
          goto LBL_RES;
        }
      }
    }
  }

  mp_exch (&res, Y);
  err = MP_OKAY;
LBL_RES:mp_clear (&res);
LBL_MU:mp_clear (&mu);
LBL_M:
  mp_clear(&M[1]);
  for (x = 1<<(winsize-1); x < (1 << winsize); x++) {
    mp_clear (&M[x]);
  }
  return err;
}


/* pre-calculate the value required for Barrett reduction
 * For a given modulus "b" it calculates the value required in "a"
 */
int mp_reduce_setup (mp_int * a, mp_int * b)
{
  int     res;

  if ((res = mp_2expt (a, b->used * 2 * DIGIT_BIT)) != MP_OKAY) {
    return res;
  }
  return mp_div (a, b, a, NULL);
}


/* reduces x mod m, assumes 0 < x < m**2, mu is
 * precomputed via mp_reduce_setup.
 * From HAC pp.604 Algorithm 14.42
 */
int mp_reduce (mp_int * x, mp_int * m, mp_int * mu)
{
  mp_int  q;
  int     res, um = m->used;

  /* q = x */
  if ((res = mp_init_copy (&q, x)) != MP_OKAY) {
    return res;
  }

  /* q1 = x / b**(k-1)  */
  mp_rshd (&q, um - 1);

  /* according to HAC this optimization is ok */
  if (((mp_word) um) > (((mp_digit)1) << (DIGIT_BIT - 1))) {
    if ((res = mp_mul (&q, mu, &q)) != MP_OKAY) {
      goto CLEANUP;
    }
  } else {
#ifdef BN_S_MP_MUL_HIGH_DIGS_C
    if ((res = s_mp_mul_high_digs (&q, mu, &q, um)) != MP_OKAY) {
      goto CLEANUP;
    }
#elif defined(BN_FAST_S_MP_MUL_HIGH_DIGS_C)
    if ((res = fast_s_mp_mul_high_digs (&q, mu, &q, um)) != MP_OKAY) {
      goto CLEANUP;
    }
#else
    {
      res = MP_VAL;
      goto CLEANUP;
    }
#endif
  }

  /* q3 = q2 / b**(k+1) */
  mp_rshd (&q, um + 1);

  /* x = x mod b**(k+1), quick (no division) */
  if ((res = mp_mod_2d (x, DIGIT_BIT * (um + 1), x)) != MP_OKAY) {
    goto CLEANUP;
  }

  /* q = q * m mod b**(k+1), quick (no division) */
  if ((res = s_mp_mul_digs (&q, m, &q, um + 1)) != MP_OKAY) {
    goto CLEANUP;
  }

  /* x = x - q */
  if ((res = mp_sub (x, &q, x)) != MP_OKAY) {
    goto CLEANUP;
  }

  /* If x < 0, add b**(k+1) to it */
  if (mp_cmp_d (x, 0) == MP_LT) {
    if ((res = mp_set (&q, 1)) != MP_OKAY)
        goto CLEANUP;
    if ((res = mp_lshd (&q, um + 1)) != MP_OKAY)
      goto CLEANUP;
    if ((res = mp_add (x, &q, x)) != MP_OKAY)
      goto CLEANUP;
  }

  /* Back off if it's too big */
  while (mp_cmp (x, m) != MP_LT) {
    if ((res = s_mp_sub (x, m, x)) != MP_OKAY) {
      goto CLEANUP;
    }
  }

CLEANUP:
  mp_clear (&q);

  return res;
}


/* reduces a modulo n where n is of the form 2**p - d
   This differs from reduce_2k since "d" can be larger
   than a single digit.
*/
int mp_reduce_2k_l(mp_int *a, mp_int *n, mp_int *d)
{
   mp_int q;
   int    p, res;

   if ((res = mp_init(&q)) != MP_OKAY) {
      return res;
   }

   p = mp_count_bits(n);
top:
   /* q = a/2**p, a = a mod 2**p */
   if ((res = mp_div_2d(a, p, &q, a)) != MP_OKAY) {
      goto ERR;
   }

   /* q = q * d */
   if ((res = mp_mul(&q, d, &q)) != MP_OKAY) {
      goto ERR;
   }

   /* a = a + q */
   if ((res = s_mp_add(a, &q, a)) != MP_OKAY) {
      goto ERR;
   }

   if (mp_cmp_mag(a, n) != MP_LT) {
      if ((res = s_mp_sub(a, n, a)) != MP_OKAY) {
         goto ERR;
      }
      goto top;
   }

ERR:
   mp_clear(&q);
   return res;
}


/* determines the setup value */
int mp_reduce_2k_setup_l(mp_int *a, mp_int *d)
{
   int    res;
   mp_int tmp;

   if ((res = mp_init(&tmp)) != MP_OKAY) {
      return res;
   }

   if ((res = mp_2expt(&tmp, mp_count_bits(a))) != MP_OKAY) {
      goto ERR;
   }

   if ((res = s_mp_sub(&tmp, a, d)) != MP_OKAY) {
      goto ERR;
   }

ERR:
   mp_clear(&tmp);
   return res;
}


/* multiplies |a| * |b| and does not compute the lower digs digits
 * [meant to get the higher part of the product]
 */
int s_mp_mul_high_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
  mp_int  t;
  int     res, pa, pb, ix, iy;
  mp_digit u;
  mp_word r;
  mp_digit tmpx, *tmpt, *tmpy;

  /* can we use the fast multiplier? */
#ifdef BN_FAST_S_MP_MUL_HIGH_DIGS_C
  if (((a->used + b->used + 1) < (int)MP_WARRAY)
      && MIN (a->used, b->used) <
      (1L << ((CHAR_BIT * sizeof (mp_word)) - (2 * DIGIT_BIT)))) {
    return fast_s_mp_mul_high_digs (a, b, c, digs);
  }
#endif

  if ((res = mp_init_size (&t, a->used + b->used + 1)) != MP_OKAY) {
    return res;
  }
  t.used = a->used + b->used + 1;

  pa = a->used;
  pb = b->used;
  for (ix = 0; ix < pa && a->dp; ix++) {
    /* clear the carry */
    u = 0;

    /* left hand side of A[ix] * B[iy] */
    tmpx = a->dp[ix];

    /* alias to the address of where the digits will be stored */
    tmpt = &(t.dp[digs]);

    /* alias for where to read the right hand side from */
    tmpy = b->dp + (digs - ix);

    for (iy = digs - ix; iy < pb; iy++) {
      /* calculate the double precision result */
      r       = ((mp_word)*tmpt) +
                ((mp_word)tmpx) * ((mp_word)*tmpy++) +
                ((mp_word) u);

      /* get the lower part */
      *tmpt++ = (mp_digit) (r & ((mp_word) MP_MASK));

      /* carry the carry */
      u       = (mp_digit) (r >> ((mp_word) DIGIT_BIT));
    }
    *tmpt = u;
  }
  mp_clamp (&t);
  mp_exch (&t, c);
  mp_clear (&t);
  return MP_OKAY;
}


/* this is a modified version of fast_s_mul_digs that only produces
 * output digits *above* digs.  See the comments for fast_s_mul_digs
 * to see how it works.
 *
 * This is used in the Barrett reduction since for one of the multiplications
 * only the higher digits were needed.  This essentially halves the work.
 *
 * Based on Algorithm 14.12 on pp.595 of HAC.
 */
int fast_s_mp_mul_high_digs (mp_int * a, mp_int * b, mp_int * c, int digs)
{
  int     olduse, res, pa, ix, iz;
#ifdef WOLFSSL_SMALL_STACK
  mp_digit* W;    /* uses dynamic memory and slower */
#else
  mp_digit W[MP_WARRAY];
#endif
  mp_word  _W;

  if (a->dp == NULL) { /* JRB, avoid reading uninitialized values */
      return MP_VAL;
  }

  /* grow the destination as required */
  pa = a->used + b->used;
  if (c->alloc < pa) {
    if ((res = mp_grow (c, pa)) != MP_OKAY) {
      return res;
    }
  }

  if (pa > (int)MP_WARRAY)
    return MP_RANGE;  /* TAO range check */

#ifdef WOLFSSL_SMALL_STACK
  W = (mp_digit*)XMALLOC(sizeof(mp_digit) * pa, NULL, DYNAMIC_TYPE_BIGINT);
  if (W == NULL)
    return MP_MEM;
#endif

  /* number of output digits to produce */
  _W = 0;
  for (ix = digs; ix < pa; ix++) { /* JRB, have a->dp check at top of function*/
      int      tx, ty, iy;
      mp_digit *tmpx, *tmpy;

      /* get offsets into the two bignums */
      ty = MIN(b->used-1, ix);
      tx = ix - ty;

      /* setup temp aliases */
      tmpx = a->dp + tx;
      tmpy = b->dp + ty;

      /* this is the number of times the loop will iterate, essentially its
         while (tx++ < a->used && ty-- >= 0) { ... }
       */
      iy = MIN(a->used-tx, ty+1);

      /* execute loop */
      for (iz = 0; iz < iy; iz++) {
         _W += ((mp_word)*tmpx++)*((mp_word)*tmpy--);
      }

      /* store term */
      W[ix] = (mp_digit)(((mp_digit)_W) & MP_MASK);

      /* make next carry */
      _W = _W >> ((mp_word)DIGIT_BIT);
  }

  /* setup dest */
  olduse  = c->used;
  c->used = pa;

  {
    mp_digit *tmpc;

    tmpc = c->dp + digs;
    for (ix = digs; ix < pa; ix++) {   /* TAO, <= could potentially overwrite */
      /* now extract the previous digit [below the carry] */
      *tmpc++ = W[ix];
    }

    /* clear unused digits [that existed in the old copy of c] */
    for (; ix < olduse; ix++) {
      *tmpc++ = 0;
    }
  }
  mp_clamp (c);

#ifdef WOLFSSL_SMALL_STACK
  XFREE(W, NULL, DYNAMIC_TYPE_BIGINT);
#endif

  return MP_OKAY;
}


#ifndef MP_SET_CHUNK_BITS
    #define MP_SET_CHUNK_BITS 4
#endif
int mp_set_int (mp_int * a, unsigned long b)
{
  int x, res;

  /* use direct mp_set if b is less than mp_digit max */
  if (b < MP_DIGIT_MAX) {
    return mp_set (a, (mp_digit)b);
  }

  mp_zero (a);

  /* set chunk bits at a time */
  for (x = 0; x < (int)(sizeof(b) * 8) / MP_SET_CHUNK_BITS; x++) {
    /* shift the number up chunk bits */
    if ((res = mp_mul_2d (a, MP_SET_CHUNK_BITS, a)) != MP_OKAY) {
      return res;
    }

    /* OR in the top bits of the source */
    a->dp[0] |= (b >> ((sizeof(b) * 8) - MP_SET_CHUNK_BITS)) &
                                  ((1 << MP_SET_CHUNK_BITS) - 1);

    /* shift the source up to the next chunk bits */
    b <<= MP_SET_CHUNK_BITS;

    /* ensure that digits are not clamped off */
    a->used += 1;
  }
  mp_clamp (a);
  return MP_OKAY;
}


#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_ECC) || !defined(NO_RSA) || \
    !defined(NO_DSA) | !defined(NO_DH)

/* c = a * a (mod b) */
int mp_sqrmod (mp_int * a, mp_int * b, mp_int * c)
{
  int     res;
  mp_int  t;

  if ((res = mp_init (&t)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_sqr (a, &t)) != MP_OKAY) {
    mp_clear (&t);
    return res;
  }
  res = mp_mod (&t, b, c);
  mp_clear (&t);
  return res;
}

#endif


#if defined(HAVE_ECC) || !defined(NO_PWDBASED) || defined(WOLFSSL_SNIFFER) || \
    defined(WOLFSSL_HAVE_WOLFSCEP) || defined(WOLFSSL_KEY_GEN) || \
    defined(OPENSSL_EXTRA) || defined(WC_RSA_BLINDING) || \
    (!defined(NO_RSA) && !defined(NO_RSA_BOUNDS_CHECK))

/* single digit addition */
int mp_add_d (mp_int* a, mp_digit b, mp_int* c) /* //NOLINT(misc-no-recursion) */
{
  int     res, ix, oldused;
  mp_digit *tmpa, *tmpc, mu;

  if (b > MP_DIGIT_MAX) return MP_VAL;

  /* grow c as required */
  if (c->alloc < a->used + 1) {
     if ((res = mp_grow(c, a->used + 1)) != MP_OKAY) {
        return res;
     }
  }

  /* if a is negative and |a| >= b, call c = |a| - b */
  if (a->sign == MP_NEG && (a->used > 1 || a->dp[0] >= b)) {
     /* temporarily fix sign of a */
     a->sign = MP_ZPOS;

     /* c = |a| - b */
     res = mp_sub_d(a, b, c);

     /* fix sign  */
     a->sign = c->sign = MP_NEG;

     /* clamp */
     mp_clamp(c);

     return res;
  }

  /* old number of used digits in c */
  oldused = c->used;

  /* sign always positive */
  c->sign = MP_ZPOS;

  /* source alias */
  tmpa    = a->dp;

  /* destination alias */
  tmpc    = c->dp;

  if (tmpa == NULL || tmpc == NULL) {
    return MP_MEM;
  }

  /* if a is positive */
  if (a->sign == MP_ZPOS) {
     /* add digit, after this we're propagating
      * the carry.
      */
     *tmpc   = *tmpa++ + b;
     mu      = *tmpc >> DIGIT_BIT;
     *tmpc++ &= MP_MASK;

     /* now handle rest of the digits */
     for (ix = 1; ix < a->used; ix++) {
        *tmpc   = *tmpa++ + mu;
        mu      = *tmpc >> DIGIT_BIT;
        *tmpc++ &= MP_MASK;
     }
     /* set final carry */
     if (ix < c->alloc) {
        ix++;
        *tmpc++  = mu;
     }

     /* setup size */
     c->used = a->used + 1;
  } else {
     /* a was negative and |a| < b */
     c->used  = 1;

     /* the result is a single digit */
     if (a->used == 1) {
        *tmpc++  =  b - a->dp[0];
     } else {
        *tmpc++  =  b;
     }

     /* setup count so the clearing of oldused
      * can fall through correctly
      */
     ix       = 1;
  }

  /* now zero to oldused */
  while (ix++ < oldused) {
     *tmpc++ = 0;
  }
  mp_clamp(c);

  return MP_OKAY;
}


/* single digit subtraction */
int mp_sub_d (mp_int * a, mp_digit b, mp_int * c) /* //NOLINT(misc-no-recursion) */
{
  mp_digit *tmpa, *tmpc, mu;
  int       res, ix, oldused;

  if (b > MP_MASK) return MP_VAL;

  /* grow c as required */
  if (c->alloc < a->used + 1) {
     if ((res = mp_grow(c, a->used + 1)) != MP_OKAY) {
        return res;
     }
  }

  /* if a is negative just do an unsigned
   * addition [with fudged signs]
   */
  if (a->sign == MP_NEG) {
     a->sign = MP_ZPOS;
     res     = mp_add_d(a, b, c);
     a->sign = c->sign = MP_NEG;

     /* clamp */
     mp_clamp(c);

     return res;
  }

  /* setup regs */
  oldused = c->used;
  tmpa    = a->dp;
  tmpc    = c->dp;

  if (tmpa == NULL || tmpc == NULL) {
    return MP_MEM;
  }

  /* if a <= b simply fix the single digit */
  if ((a->used == 1 && a->dp[0] <= b) || a->used == 0) {
     if (a->used == 1) {
        *tmpc++ = b - *tmpa;
     } else {
        *tmpc++ = b;
     }
     ix      = 1;

     /* negative/1digit */
     c->sign = MP_NEG;
     c->used = 1;
  } else {
     /* positive/size */
     c->sign = MP_ZPOS;
     c->used = a->used;

     /* subtract first digit */
     *tmpc    = *tmpa++ - b;
     mu       = *tmpc >> (sizeof(mp_digit) * CHAR_BIT - 1);
     *tmpc++ &= MP_MASK;

     /* handle rest of the digits */
     for (ix = 1; ix < a->used; ix++) {
        *tmpc    = *tmpa++ - mu;
        mu       = *tmpc >> (sizeof(mp_digit) * CHAR_BIT - 1);
        *tmpc++ &= MP_MASK;
     }
  }

  /* zero excess digits */
  while (ix++ < oldused) {
     *tmpc++ = 0;
  }
  mp_clamp(c);
  return MP_OKAY;
}

#endif /* defined(HAVE_ECC) || !defined(NO_PWDBASED) */


#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || defined(HAVE_ECC) || \
    defined(DEBUG_WOLFSSL) || !defined(NO_RSA) || !defined(NO_DSA) || \
    !defined(NO_DH) || defined(WC_MP_TO_RADIX)

static const int lnz[16] = {
   4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/* Counts the number of lsbs which are zero before the first zero bit */
int mp_cnt_lsb(mp_int *a)
{
    int x;
    mp_digit q = 0, qq;

    /* easy out */
    if (mp_iszero(a) == MP_YES) {
        return 0;
    }

    /* scan lower digits until non-zero */
    for (x = 0; x < a->used && a->dp[x] == 0; x++) {}
    if (a->dp)
        q = a->dp[x];
    x *= DIGIT_BIT;

    /* now scan this digit until a 1 is found */
    if ((q & 1) == 0) {
        do {
            qq  = q & 15;
            x  += lnz[qq];
            q >>= 4;
        } while (qq == 0);
    }
    return x;
}




static int s_is_power_of_two(mp_digit b, int *p)
{
   int x;

   /* fast return if no power of two */
   if ((b==0) || (b & (b-1))) {
      return 0;
   }

   for (x = 0; x < DIGIT_BIT; x++) {
      if (b == (((mp_digit)1)<<x)) {
         *p = x;
         return 1;
      }
   }
   return 0;
}

/* single digit division (based on routine from MPI) */
static int mp_div_d (mp_int * a, mp_digit b, mp_int * c, mp_digit * d)
{
  mp_int  q;
  mp_word w;
  mp_digit t;
  int     res = MP_OKAY, ix;

  /* cannot divide by zero */
  if (b == 0) {
     return MP_VAL;
  }

  /* quick outs */
  if (b == 1 || mp_iszero(a) == MP_YES) {
     if (d != NULL) {
        *d = 0;
     }
     if (c != NULL) {
        return mp_copy(a, c);
     }
     return MP_OKAY;
  }

  /* power of two ? */
  if (s_is_power_of_two(b, &ix) == 1) {
     if (d != NULL) {
        *d = a->dp[0] & ((((mp_digit)1)<<ix) - 1);
     }
     if (c != NULL) {
        return mp_div_2d(a, ix, c, NULL);
     }
     return MP_OKAY;
  }

#ifdef BN_MP_DIV_3_C
  /* three? */
  if (b == 3) {
     return mp_div_3(a, c, d);
  }
#endif

  /* no easy answer [c'est la vie].  Just division */
  if (c != NULL) {
      if ((res = mp_init_size(&q, a->used)) != MP_OKAY) {
         return res;
      }

      q.used = a->used;
      q.sign = a->sign;
  }
  else {
      if ((res = mp_init(&q)) != MP_OKAY) {
         return res;
      }
  }


  w = 0;
  for (ix = a->used - 1; ix >= 0; ix--) {
     w = (w << ((mp_word)DIGIT_BIT)) | ((mp_word)a->dp[ix]);

     if (w >= b) {
#ifdef WOLFSSL_LINUXKM
        t = (mp_digit)w;
        /* Linux kernel macro for in-place 64 bit integer division. */
        do_div(t, b);
#else
        t = (mp_digit)(w / b);
#endif
        w -= ((mp_word)t) * ((mp_word)b);
      } else {
        t = 0;
      }
      if (c != NULL)
        q.dp[ix] = (mp_digit)t;
  }

  if (d != NULL) {
     *d = (mp_digit)w;
  }

  if (c != NULL) {
     mp_clamp(&q);
     mp_exch(&q, c);
  }
  mp_clear(&q);

  return res;
}


int mp_mod_d (mp_int * a, mp_digit b, mp_digit * c)
{
  return mp_div_d(a, b, NULL, c);
}

#endif /* WOLFSSL_KEY_GEN || HAVE_COMP_KEY || HAVE_ECC || DEBUG_WOLFSSL */

#if (defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA)) || !defined(NO_DH) || !defined(NO_DSA)

const FLASH_QUALIFIER mp_digit ltm_prime_tab[PRIME_SIZE] = {
  0x0002, 0x0003, 0x0005, 0x0007, 0x000B, 0x000D, 0x0011, 0x0013,
  0x0017, 0x001D, 0x001F, 0x0025, 0x0029, 0x002B, 0x002F, 0x0035,
  0x003B, 0x003D, 0x0043, 0x0047, 0x0049, 0x004F, 0x0053, 0x0059,
  0x0061, 0x0065, 0x0067, 0x006B, 0x006D, 0x0071, 0x007F,
#ifndef MP_8BIT
  0x0083,
  0x0089, 0x008B, 0x0095, 0x0097, 0x009D, 0x00A3, 0x00A7, 0x00AD,
  0x00B3, 0x00B5, 0x00BF, 0x00C1, 0x00C5, 0x00C7, 0x00D3, 0x00DF,
  0x00E3, 0x00E5, 0x00E9, 0x00EF, 0x00F1, 0x00FB, 0x0101, 0x0107,
  0x010D, 0x010F, 0x0115, 0x0119, 0x011B, 0x0125, 0x0133, 0x0137,

  0x0139, 0x013D, 0x014B, 0x0151, 0x015B, 0x015D, 0x0161, 0x0167,
  0x016F, 0x0175, 0x017B, 0x017F, 0x0185, 0x018D, 0x0191, 0x0199,
  0x01A3, 0x01A5, 0x01AF, 0x01B1, 0x01B7, 0x01BB, 0x01C1, 0x01C9,
  0x01CD, 0x01CF, 0x01D3, 0x01DF, 0x01E7, 0x01EB, 0x01F3, 0x01F7,
  0x01FD, 0x0209, 0x020B, 0x021D, 0x0223, 0x022D, 0x0233, 0x0239,
  0x023B, 0x0241, 0x024B, 0x0251, 0x0257, 0x0259, 0x025F, 0x0265,
  0x0269, 0x026B, 0x0277, 0x0281, 0x0283, 0x0287, 0x028D, 0x0293,
  0x0295, 0x02A1, 0x02A5, 0x02AB, 0x02B3, 0x02BD, 0x02C5, 0x02CF,

  0x02D7, 0x02DD, 0x02E3, 0x02E7, 0x02EF, 0x02F5, 0x02F9, 0x0301,
  0x0305, 0x0313, 0x031D, 0x0329, 0x032B, 0x0335, 0x0337, 0x033B,
  0x033D, 0x0347, 0x0355, 0x0359, 0x035B, 0x035F, 0x036D, 0x0371,
  0x0373, 0x0377, 0x038B, 0x038F, 0x0397, 0x03A1, 0x03A9, 0x03AD,
  0x03B3, 0x03B9, 0x03C7, 0x03CB, 0x03D1, 0x03D7, 0x03DF, 0x03E5,
  0x03F1, 0x03F5, 0x03FB, 0x03FD, 0x0407, 0x0409, 0x040F, 0x0419,
  0x041B, 0x0425, 0x0427, 0x042D, 0x043F, 0x0443, 0x0445, 0x0449,
  0x044F, 0x0455, 0x045D, 0x0463, 0x0469, 0x047F, 0x0481, 0x048B,

  0x0493, 0x049D, 0x04A3, 0x04A9, 0x04B1, 0x04BD, 0x04C1, 0x04C7,
  0x04CD, 0x04CF, 0x04D5, 0x04E1, 0x04EB, 0x04FD, 0x04FF, 0x0503,
  0x0509, 0x050B, 0x0511, 0x0515, 0x0517, 0x051B, 0x0527, 0x0529,
  0x052F, 0x0551, 0x0557, 0x055D, 0x0565, 0x0577, 0x0581, 0x058F,
  0x0593, 0x0595, 0x0599, 0x059F, 0x05A7, 0x05AB, 0x05AD, 0x05B3,
  0x05BF, 0x05C9, 0x05CB, 0x05CF, 0x05D1, 0x05D5, 0x05DB, 0x05E7,
  0x05F3, 0x05FB, 0x0607, 0x060D, 0x0611, 0x0617, 0x061F, 0x0623,
  0x062B, 0x062F, 0x063D, 0x0641, 0x0647, 0x0649, 0x064D, 0x0653
#endif
};


/* Miller-Rabin test of "a" to the base of "b" as described in
 * HAC pp. 139 Algorithm 4.24
 *
 * Sets result to 0 if definitely composite or 1 if probably prime.
 * Randomly the chance of error is no more than 1/4 and often
 * very much lower.
 */
static int mp_prime_miller_rabin (mp_int * a, mp_int * b, int *result)
{
  mp_int  n1, y, r;
  int     s, j, err;

  /* default */
  *result = MP_NO;

  /* ensure b > 1 */
  if (mp_cmp_d(b, 1) != MP_GT) {
     return MP_VAL;
  }

  /* get n1 = a - 1 */
  if ((err = mp_init_copy (&n1, a)) != MP_OKAY) {
    return err;
  }
  if ((err = mp_sub_d (&n1, 1, &n1)) != MP_OKAY) {
    goto LBL_N1;
  }

  /* set 2**s * r = n1 */
  if ((err = mp_init_copy (&r, &n1)) != MP_OKAY) {
    goto LBL_N1;
  }

  /* count the number of least significant bits
   * which are zero
   */
  s = mp_cnt_lsb(&r);

  /* now divide n - 1 by 2**s */
  if ((err = mp_div_2d (&r, s, &r, NULL)) != MP_OKAY) {
    goto LBL_R;
  }

  /* compute y = b**r mod a */
  if ((err = mp_init (&y)) != MP_OKAY) {
    goto LBL_R;
  }
#if defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH)
#ifndef WOLFSSL_SP_NO_2048
  if (mp_count_bits(a) == 1024 && mp_isodd(a))
      err = sp_ModExp_1024(b, &r, a, &y);
  else if (mp_count_bits(a) == 2048 && mp_isodd(a))
      err = sp_ModExp_2048(b, &r, a, &y);
  else
#endif
#ifndef WOLFSSL_SP_NO_3072
  if (mp_count_bits(a) == 1536 && mp_isodd(a))
      err = sp_ModExp_1536(b, &r, a, &y);
  else if (mp_count_bits(a) == 3072 && mp_isodd(a))
      err = sp_ModExp_3072(b, &r, a, &y);
  else
#endif
#ifdef WOLFSSL_SP_4096
  if (mp_count_bits(a) == 4096 && mp_isodd(a))
      err = sp_ModExp_4096(b, &r, a, &y);
  else
#endif
#endif
      err = mp_exptmod (b, &r, a, &y);
  if (err != MP_OKAY)
      goto LBL_Y;

  /* if y != 1 and y != n1 do */
  if (mp_cmp_d (&y, 1) != MP_EQ && mp_cmp (&y, &n1) != MP_EQ) {
    j = 1;
    /* while j <= s-1 and y != n1 */
    while ((j <= (s - 1)) && mp_cmp (&y, &n1) != MP_EQ) {
      if ((err = mp_sqrmod (&y, a, &y)) != MP_OKAY) {
         goto LBL_Y;
      }

      /* if y == 1 then composite */
      if (mp_cmp_d (&y, 1) == MP_EQ) {
         goto LBL_Y;
      }

      ++j;
    }

    /* if y != n1 then composite */
    if (mp_cmp (&y, &n1) != MP_EQ) {
      goto LBL_Y;
    }
  }

  /* probably prime now */
  *result = MP_YES;
LBL_Y:mp_clear (&y);
LBL_R:mp_clear (&r);
LBL_N1:mp_clear (&n1);
  return err;
}


/* determines if an integers is divisible by one
 * of the first PRIME_SIZE primes or not
 *
 * sets result to 0 if not, 1 if yes
 */
static int mp_prime_is_divisible (mp_int * a, int *result)
{
  int     err, ix;
  mp_digit res;

  /* default to not */
  *result = MP_NO;

  for (ix = 0; ix < PRIME_SIZE; ix++) {
    /* what is a mod LBL_prime_tab[ix] */
    if ((err = mp_mod_d (a, ltm_prime_tab[ix], &res)) != MP_OKAY) {
      return err;
    }

    /* is the residue zero? */
    if (res == 0) {
      *result = MP_YES;
      return MP_OKAY;
    }
  }

  return MP_OKAY;
}

/*
 * Sets result to 1 if probably prime, 0 otherwise
 */
int mp_prime_is_prime (mp_int * a, int t, int *result)
{
  mp_int  b;
  int     ix, err, res;

  /* default to no */
  *result = MP_NO;

  /* valid value of t? */
  if (t <= 0 || t > PRIME_SIZE) {
    return MP_VAL;
  }

  if (mp_isone(a)) {
      *result = MP_NO;
      return MP_OKAY;
  }

  /* is the input equal to one of the primes in the table? */
  for (ix = 0; ix < PRIME_SIZE; ix++) {
      if (mp_cmp_d(a, ltm_prime_tab[ix]) == MP_EQ) {
         *result = MP_YES;
         return MP_OKAY;
      }
  }

  /* first perform trial division */
  if ((err = mp_prime_is_divisible (a, &res)) != MP_OKAY) {
    return err;
  }

  /* return if it was trivially divisible */
  if (res == MP_YES) {
    return MP_OKAY;
  }

  /* now perform the miller-rabin rounds */
  if ((err = mp_init (&b)) != MP_OKAY) {
    return err;
  }

  for (ix = 0; ix < t; ix++) {
    /* set the prime */
    if ((err = mp_set (&b, ltm_prime_tab[ix])) != MP_OKAY) {
        goto LBL_B;
    }

    if ((err = mp_prime_miller_rabin (a, &b, &res)) != MP_OKAY) {
      goto LBL_B;
    }

    if (res == MP_NO) {
      goto LBL_B;
    }
  }

  /* passed the test */
  *result = MP_YES;
LBL_B:mp_clear (&b);
  return err;
}


/*
 * Sets result to 1 if probably prime, 0 otherwise
 */
int mp_prime_is_prime_ex (mp_int * a, int t, int *result, WC_RNG *rng)
{
  mp_int  b, c;
  int     ix, err, res;
  byte*   base = NULL;
  word32  bitSz = 0;
  word32  baseSz = 0;

  /* default to no */
  *result = MP_NO;

  /* valid value of t? */
  if (t <= 0 || t > PRIME_SIZE) {
    return MP_VAL;
  }

  if (a->sign == MP_NEG) {
    return MP_VAL;
  }

  if (mp_isone(a)) {
    *result = MP_NO;
    return MP_OKAY;
  }

  /* is the input equal to one of the primes in the table? */
  for (ix = 0; ix < PRIME_SIZE; ix++) {
      if (mp_cmp_d(a, ltm_prime_tab[ix]) == MP_EQ) {
         *result = MP_YES;
         return MP_OKAY;
      }
  }

  /* first perform trial division */
  if ((err = mp_prime_is_divisible (a, &res)) != MP_OKAY) {
    return err;
  }

  /* return if it was trivially divisible */
  if (res == MP_YES) {
    return MP_OKAY;
  }

  /* now perform the miller-rabin rounds */
  if ((err = mp_init (&b)) != MP_OKAY) {
    return err;
  }
  if ((err = mp_init (&c)) != MP_OKAY) {
      mp_clear(&b);
    return err;
  }

  bitSz = mp_count_bits(a);
  baseSz = (bitSz / 8) + ((bitSz % 8) ? 1 : 0);
  bitSz %= 8;

  base = (byte*)XMALLOC(baseSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (base == NULL) {
      err = MP_MEM;
      goto LBL_B;
  }

  if ((err = mp_sub_d(a, 2, &c)) != MP_OKAY) {
      goto LBL_B;
  }

 /* now do a miller rabin with up to t random numbers, this should
  * give a (1/4)^t chance of a false prime. */
  for (ix = 0; ix < t; ix++) {
    /* Set a test candidate. */
    if ((err = wc_RNG_GenerateBlock(rng, base, baseSz)) != 0) {
        goto LBL_B;
    }

    /* Clear bits higher than those in a. */
    if (bitSz > 0) {
        base[0] &= (1 << bitSz) - 1;
    }

    if ((err = mp_read_unsigned_bin(&b, base, baseSz)) != MP_OKAY) {
        goto LBL_B;
    }

    if (mp_cmp_d(&b, 2) != MP_GT || mp_cmp(&b, &c) != MP_LT) {
        ix--;
        continue;
    }

    if ((err = mp_prime_miller_rabin (a, &b, &res)) != MP_OKAY) {
      goto LBL_B;
    }

    if (res == MP_NO) {
      goto LBL_B;
    }
  }

  /* passed the test */
  *result = MP_YES;
LBL_B:mp_clear (&b);
      mp_clear (&c);
      XFREE(base, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  return err;
}

#endif /* (WOLFSSL_KEY_GEN && !NO_RSA) || !NO_DH || !NO_DSA */

#if defined(WOLFSSL_KEY_GEN) && (!defined(NO_DH) || !defined(NO_DSA))

static const int USE_BBS = 1;

int mp_rand_prime(mp_int* a, int len, WC_RNG* rng, void* heap)
{
    int   err, res, type;
    byte* buf;

    if (a == NULL || rng == NULL)
        return MP_VAL;

    /* get type */
    if (len < 0) {
        type = USE_BBS;
        len = -len;
    } else {
        type = 0;
    }

    /* allow sizes between 2 and 512 bytes for a prime size */
    if (len < 2 || len > 512) {
        return MP_VAL;
    }

    /* allocate buffer to work with */
    buf = (byte*)XMALLOC(len, heap, DYNAMIC_TYPE_RSA);
    if (buf == NULL) {
        return MP_MEM;
    }
    XMEMSET(buf, 0, len);

    do {
#ifdef SHOW_GEN
        printf(".");
        fflush(stdout);
#endif
        /* generate value */
        err = wc_RNG_GenerateBlock(rng, buf, len);
        if (err != 0) {
            XFREE(buf, heap, DYNAMIC_TYPE_RSA);
            return err;
        }

        /* munge bits */
        buf[0]     |= 0x80 | 0x40;
        buf[len-1] |= 0x01 | ((type & USE_BBS) ? 0x02 : 0x00);

        /* load value */
        if ((err = mp_read_unsigned_bin(a, buf, len)) != MP_OKAY) {
            XFREE(buf, heap, DYNAMIC_TYPE_RSA);
            return err;
        }

        /* test */
        /* Running Miller-Rabin up to 3 times gives us a 2^{-80} chance
         * of a 1024-bit candidate being a false positive, when it is our
         * prime candidate. (Note 4.49 of Handbook of Applied Cryptography.)
         * Using 8 because we've always used 8. */
        if ((err = mp_prime_is_prime_ex(a, 8, &res, rng)) != MP_OKAY) {
            XFREE(buf, heap, DYNAMIC_TYPE_RSA);
            return err;
        }
    } while (res == MP_NO);

    XMEMSET(buf, 0, len);
    XFREE(buf, heap, DYNAMIC_TYPE_RSA);

    return MP_OKAY;
}

#endif

#if defined(WOLFSSL_KEY_GEN)

/* computes least common multiple as |a*b|/(a, b) */
int mp_lcm (mp_int * a, mp_int * b, mp_int * c)
{
  int     res;
  mp_int  t1, t2;

  /* LCM of 0 and any number is undefined as 0 is not in the set of values
   * being used. */
  if (mp_iszero (a) == MP_YES || mp_iszero (b) == MP_YES) {
    return MP_VAL;
  }

  if ((res = mp_init_multi (&t1, &t2, NULL, NULL, NULL, NULL)) != MP_OKAY) {
    return res;
  }

  /* t1 = get the GCD of the two inputs */
  if ((res = mp_gcd (a, b, &t1)) != MP_OKAY) {
    goto LBL_T;
  }

  /* divide the smallest by the GCD */
  if (mp_cmp_mag(a, b) == MP_LT) {
     /* store quotient in t2 such that t2 * b is the LCM */
     if ((res = mp_div(a, &t1, &t2, NULL)) != MP_OKAY) {
        goto LBL_T;
     }
     res = mp_mul(b, &t2, c);
  } else {
     /* store quotient in t2 such that t2 * a is the LCM */
     if ((res = mp_div(b, &t1, &t2, NULL)) != MP_OKAY) {
        goto LBL_T;
     }
     res = mp_mul(a, &t2, c);
  }

  /* fix the sign to positive */
  c->sign = MP_ZPOS;

LBL_T:
  mp_clear(&t1);
  mp_clear(&t2);
  return res;
}



/* Greatest Common Divisor using the binary method */
int mp_gcd (mp_int * a, mp_int * b, mp_int * c)
{
    mp_int  u, v;
    int     k, u_lsb, v_lsb, res;

    /* either zero than gcd is the largest */
    if (mp_iszero (a) == MP_YES) {
        /* GCD of 0 and 0 is undefined as all integers divide 0. */
        if (mp_iszero (b) == MP_YES) {
           return MP_VAL;
        }
        return mp_abs (b, c);
    }
    if (mp_iszero (b) == MP_YES) {
        return mp_abs (a, c);
    }

    /* get copies of a and b we can modify */
    if ((res = mp_init_copy (&u, a)) != MP_OKAY) {
        return res;
    }

    if ((res = mp_init_copy (&v, b)) != MP_OKAY) {
        goto LBL_U;
    }

    /* must be positive for the remainder of the algorithm */
    u.sign = v.sign = MP_ZPOS;

    /* B1.  Find the common power of two for u and v */
    u_lsb = mp_cnt_lsb(&u);
    v_lsb = mp_cnt_lsb(&v);
    k     = MIN(u_lsb, v_lsb);

    if (k > 0) {
        /* divide the power of two out */
        if ((res = mp_div_2d(&u, k, &u, NULL)) != MP_OKAY) {
            goto LBL_V;
        }

        if ((res = mp_div_2d(&v, k, &v, NULL)) != MP_OKAY) {
            goto LBL_V;
        }
    }

    /* divide any remaining factors of two out */
    if (u_lsb != k) {
        if ((res = mp_div_2d(&u, u_lsb - k, &u, NULL)) != MP_OKAY) {
            goto LBL_V;
        }
    }

    if (v_lsb != k) {
        if ((res = mp_div_2d(&v, v_lsb - k, &v, NULL)) != MP_OKAY) {
            goto LBL_V;
        }
    }

    while (mp_iszero(&v) == MP_NO) {
        /* make sure v is the largest */
        if (mp_cmp_mag(&u, &v) == MP_GT) {
            /* swap u and v to make sure v is >= u */
            mp_exch(&u, &v);
        }

        /* subtract smallest from largest */
        if ((res = s_mp_sub(&v, &u, &v)) != MP_OKAY) {
            goto LBL_V;
        }

        /* Divide out all factors of two */
        if ((res = mp_div_2d(&v, mp_cnt_lsb(&v), &v, NULL)) != MP_OKAY) {
            goto LBL_V;
        }
    }

    /* multiply by 2**k which we divided out at the beginning */
    if ((res = mp_mul_2d (&u, k, c)) != MP_OKAY) {
        goto LBL_V;
    }
    c->sign = MP_ZPOS;
    res = MP_OKAY;
LBL_V:mp_clear (&v);
LBL_U:mp_clear (&u);
    return res;
}

#endif /* WOLFSSL_KEY_GEN */


#if !defined(NO_DSA) || defined(HAVE_ECC) || defined(WOLFSSL_KEY_GEN) || \
    defined(HAVE_COMP_KEY) || defined(WOLFSSL_DEBUG_MATH) || \
    defined(DEBUG_WOLFSSL) || defined(OPENSSL_EXTRA) || defined(WC_MP_TO_RADIX)

/* chars used in radix conversions */
const char *mp_s_rmap = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz+/";
#endif

#if !defined(NO_DSA) || defined(HAVE_ECC) || defined(OPENSSL_EXTRA)
/* read a string [ASCII] in a given radix */
int mp_read_radix (mp_int * a, const char *str, int radix)
{
  int     y, res, neg;
  char    ch;

  /* zero the digit bignum */
  mp_zero(a);

  /* make sure the radix is ok */
  if (radix < MP_RADIX_BIN || radix > MP_RADIX_MAX) {
    return MP_VAL;
  }

  /* if the leading digit is a
   * minus set the sign to negative.
   */
  if (*str == '-') {
    ++str;
    neg = MP_NEG;
  } else {
    neg = MP_ZPOS;
  }

  /* set the integer to the default of zero */
  mp_zero (a);

  /* process each digit of the string */
  while (*str != '\0') {
    /* if the radix <= 36 the conversion is case insensitive
     * this allows numbers like 1AB and 1ab to represent the same  value
     * [e.g. in hex]
     */
    ch = (radix <= 36) ? (char)XTOUPPER((unsigned char)*str) : *str;
    for (y = 0; y < 64; y++) {
      if (ch == mp_s_rmap[y]) {
         break;
      }
    }

    /* if the char was found in the map
     * and is less than the given radix add it
     * to the number, otherwise exit the loop.
     */
    if (y < radix) {
      if ((res = mp_mul_d (a, (mp_digit) radix, a)) != MP_OKAY) {
         mp_zero(a);
         return res;
      }
      if ((res = mp_add_d (a, (mp_digit) y, a)) != MP_OKAY) {
         mp_zero(a);
         return res;
      }
    } else {
      break;
    }
    ++str;
  }

  /* if digit in isn't null term, then invalid character was found */
  if (*str != '\0') {
     mp_zero (a);
     return MP_VAL;
  }

  /* set the sign only if a != 0 */
  if (mp_iszero(a) != MP_YES) {
     a->sign = neg;
  }
  return MP_OKAY;
}
#endif /* !defined(NO_DSA) || defined(HAVE_ECC) */

#ifdef WC_MP_TO_RADIX

/* returns size of ASCII representation */
int mp_radix_size (mp_int *a, int radix, int *size)
{
    int     res, digs;
    mp_int  t;
    mp_digit d;

    *size = 0;

    /* special case for binary */
    if (radix == MP_RADIX_BIN) {
        *size = mp_count_bits(a);
        if (*size == 0)
          *size = 1;
        *size += (a->sign == MP_NEG ? 1 : 0) + 1; /* "-" sign + null term */
        return MP_OKAY;
    }

    /* make sure the radix is in range */
    if (radix < MP_RADIX_BIN || radix > MP_RADIX_MAX) {
        return MP_VAL;
    }

    if (mp_iszero(a) == MP_YES) {
#ifndef WC_DISABLE_RADIX_ZERO_PAD
        if (radix == 16)
            *size = 3;
        else
#endif
            *size = 2;
        return MP_OKAY;
    }

    /* digs is the digit count */
    digs = 0;

    /* init a copy of the input */
    if ((res = mp_init_copy (&t, a)) != MP_OKAY) {
        return res;
    }

    /* force temp to positive */
    t.sign = MP_ZPOS;

    /* fetch out all of the digits */
    while (mp_iszero (&t) == MP_NO) {
        if ((res = mp_div_d (&t, (mp_digit) radix, &t, &d)) != MP_OKAY) {
            mp_clear (&t);
            return res;
        }
        ++digs;
    }
    mp_clear (&t);

#ifndef WC_DISABLE_RADIX_ZERO_PAD
    /* For hexadecimal output, add zero padding when number of digits is odd */
    if ((digs & 1) && (radix == 16)) {
        ++digs;
    }
#endif

    /* if it's negative add one for the sign */
    if (a->sign == MP_NEG) {
        ++digs;
    }

    /* return digs + 1, the 1 is for the NULL byte that would be required. */
    *size = digs + 1;
    return MP_OKAY;
}

/* stores a bignum as a ASCII string in a given radix (2..64) */
int mp_toradix (mp_int *a, char *str, int radix)
{
    int     res, digs;
    mp_int  t;
    mp_digit d;
    char   *_s = str;

    /* check range of the radix */
    if (radix < MP_RADIX_BIN || radix > MP_RADIX_MAX) {
        return MP_VAL;
    }

    /* quick out if its zero */
    if (mp_iszero(a) == MP_YES) {
#ifndef WC_DISABLE_RADIX_ZERO_PAD
        if (radix == 16) {
            *str++ = '0';
        }
#endif
        *str++ = '0';
        *str = '\0';
        return MP_OKAY;
    }

    if ((res = mp_init_copy (&t, a)) != MP_OKAY) {
        return res;
    }

    /* if it is negative output a - */
    if (t.sign == MP_NEG) {
        ++_s;
        *str++ = '-';
        t.sign = MP_ZPOS;
    }

    digs = 0;
    while (mp_iszero (&t) == MP_NO) {
        if ((res = mp_div_d (&t, (mp_digit) radix, &t, &d)) != MP_OKAY) {
            mp_clear (&t);
            return res;
        }
        *str++ = mp_s_rmap[d];
        ++digs;
    }
#ifndef WC_DISABLE_RADIX_ZERO_PAD
    /* For hexadecimal output, add zero padding when number of digits is odd */
    if ((digs & 1) && (radix == 16)) {
        *str++ = mp_s_rmap[0];
        ++digs;
    }
#endif
    /* reverse the digits of the string.  In this case _s points
     * to the first digit [excluding the sign] of the number]
     */
    bn_reverse ((unsigned char *)_s, digs);

    /* append a NULL so the string is properly terminated */
    *str = '\0';

    mp_clear (&t);
    return MP_OKAY;
}

#ifdef WOLFSSL_DEBUG_MATH
void mp_dump(const char* desc, mp_int* a, byte verbose)
{
  char *buffer;
  int size = a->alloc;

  buffer = (char*)XMALLOC(size * sizeof(mp_digit) * 2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
  if (buffer == NULL) {
    return;
  }

  printf("%s: ptr=%p, used=%d, sign=%d, size=%d, mpd=%d\n",
    desc, a, a->used, a->sign, size, (int)sizeof(mp_digit));

  mp_tohex(a, buffer);
  printf("  %s\n  ", buffer);

  if (verbose) {
    int i;
    for(i=0; i<a->alloc * (int)sizeof(mp_digit); i++) {
      printf("%02x ", *(((byte*)a->dp) + i));
    }
    printf("\n");
  }

  XFREE(buffer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
}
#endif /* WOLFSSL_DEBUG_MATH */

#endif /* WC_MP_TO_RADIX */

#endif /* WOLFSSL_SP_MATH */

#endif /* !USE_FAST_MATH && USE_INTEGER_HEAP_MATH */

#endif /* NO_BIG_INT */
