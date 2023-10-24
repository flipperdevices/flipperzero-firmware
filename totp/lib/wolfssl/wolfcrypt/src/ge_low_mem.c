/* ge_low_mem.c
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


 /* Based from Daniel Beer's public domain work. */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ED25519
#ifdef ED25519_SMALL /* use slower code that takes less memory */

#include <wolfssl/wolfcrypt/ge_operations.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

void ed25519_smult(ge_p3 *r, const ge_p3 *a, const byte *e);
void ed25519_add(ge_p3 *r, const ge_p3 *a, const ge_p3 *b);
void ed25519_double(ge_p3 *r, const ge_p3 *a);


static const byte ed25519_order[F25519_SIZE] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
    0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

/*Arithmetic modulo the group order mod = 2^252 +
 27742317777372353535851937790883648493 =
 7237005577332262213973186563042994240857116359379907606001950938285454250989 */

static const word32 mod[32] = {
    0xED,0xD3,0xF5,0x5C,0x1A,0x63,0x12,0x58,0xD6,0x9C,0xF7,0xA2,0xDE,0xF9,
    0xDE,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x10
};

static const word32 mu[33] = {
    0x1B,0x13,0x2C,0x0A,0xA3,0xE5,0x9C,0xED,0xA7,0x29,0x63,0x08,0x5D,0x21,
    0x06,0x21,0xEB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0x0F
};


int ge_compress_key(byte* out, const byte* xIn, const byte* yIn,
                        word32 keySz)
{
    byte tmp[F25519_SIZE];
    byte parity;
    byte pt[32];
    int     i;

    lm_copy(tmp, xIn);
    parity = (tmp[0] & 1) << 7;

    lm_copy(pt, yIn);
    pt[31] |= parity;

    for(i = 0; i < 32; i++) {
        out[32-i-1] = pt[i];
    }
    (void)keySz;
    return 0;
}


static word32 lt(word32 a,word32 b) /* 16-bit inputs */
{
  word32 x = a;
  x -= (unsigned int) b; /* 0..65535: no; 4294901761..4294967295: yes */
  x >>= 31; /* 0: no; 1: yes */
  return x;
}


/* Reduce coefficients of r before calling reduce_add_sub */
static void reduce_add_sub(word32 *r)
{
  word32 pb = 0;
  word32 b;
  word32 mask;
  int i;
  unsigned char t[32];

  for(i=0;i<32;i++)
  {
    pb += mod[i];
    b = lt(r[i],pb);
    t[i] = r[i]-pb+(b<<8);
    pb = b;
  }
  mask = b - 1;
  for(i=0;i<32;i++)
    r[i] ^= mask & (r[i] ^ t[i]);
}


/* Reduce coefficients of x before calling barrett_reduce */
static void barrett_reduce(word32* r, word32 x[64])
{
  /* See HAC, Alg. 14.42 */
  int i,j;
  word32 q2[66];
  word32 *q3 = q2 + 33;
  word32 r1[33];
  word32 r2[33];
  word32 carry;
  word32 pb = 0;

  for (i = 0;i < 66;++i) q2[i] = 0;
  for (i = 0;i < 33;++i) r2[i] = 0;

  for(i=0;i<33;i++)
    for(j=0;j<33;j++)
      if(i+j >= 31) q2[i+j] += mu[i]*x[j+31];
  carry = q2[31] >> 8;
  q2[32] += carry;
  carry = q2[32] >> 8;
  q2[33] += carry;

  for(i=0;i<33;i++)r1[i] = x[i];
  for(i=0;i<32;i++)
    for(j=0;j<33;j++)
      if(i+j < 33) r2[i+j] += mod[i]*q3[j];

  for(i=0;i<32;i++)
  {
    carry = r2[i] >> 8;
    r2[i+1] += carry;
    r2[i] &= 0xff;
  }

  for(i=0;i<32;i++)
  {
    word32 b;
    pb += r2[i];
    b = lt(r1[i],pb);
    r[i] = r1[i]-pb+(b<<8);
    pb = b;
  }

  /* XXX: Can it really happen that r<0?, See HAC, Alg 14.42, Step 3
   * r is an unsigned type.
   * If so: Handle  it here!
   */

  reduce_add_sub(r);
  reduce_add_sub(r);
}


void sc_reduce(unsigned char *x)
{
  int i;
  word32 t[64];
  word32 r[32];
  for(i=0;i<64;i++) t[i] = x[i];
  barrett_reduce(r, t);
  for(i=0;i<32;i++) x[i] = (r[i] & 0xFF);
}


void sc_muladd(byte* out, const byte* a, const byte* b, const byte* c)
{

    byte s[32];
    byte e[64];

    XMEMSET(e, 0, sizeof(e));
    XMEMCPY(e, b, 32);

    /* Obtain e */
    sc_reduce(e);

    /* Compute s = ze + k */
    fprime_mul(s, a, e, ed25519_order);
    fprime_add(s, c, ed25519_order);

    XMEMCPY(out, s, 32);
}


/* Base point is (numbers wrapped):
 *
 *     x = 151122213495354007725011514095885315114
 *         54012693041857206046113283949847762202
 *     y = 463168356949264781694283940034751631413
 *         07993866256225615783033603165251855960
 *
 * y is derived by transforming the original Montgomery base (u=9). x
 * is the corresponding positive coordinate for the new curve equation.
 * t is x*y.
 */
const ge_p3 ed25519_base = {
    {
        0x1a, 0xd5, 0x25, 0x8f, 0x60, 0x2d, 0x56, 0xc9,
        0xb2, 0xa7, 0x25, 0x95, 0x60, 0xc7, 0x2c, 0x69,
        0x5c, 0xdc, 0xd6, 0xfd, 0x31, 0xe2, 0xa4, 0xc0,
        0xfe, 0x53, 0x6e, 0xcd, 0xd3, 0x36, 0x69, 0x21
    },
    {
        0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66
    },
    {1, 0},
    {
        0xa3, 0xdd, 0xb7, 0xa5, 0xb3, 0x8a, 0xde, 0x6d,
        0xf5, 0x52, 0x51, 0x77, 0x80, 0x9f, 0xf0, 0x20,
        0x7d, 0xe3, 0xab, 0x64, 0x8e, 0x4e, 0xea, 0x66,
        0x65, 0x76, 0x8b, 0xd7, 0x0f, 0x5f, 0x87, 0x67
    },

};


const ge_p3 ed25519_neutral = {
    {0},
    {1, 0},
    {1, 0},
    {0},

};


static const byte ed25519_d[F25519_SIZE] = {
    0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75,
    0xab, 0xd8, 0x41, 0x41, 0x4d, 0x0a, 0x70, 0x00,
    0x98, 0xe8, 0x79, 0x77, 0x79, 0x40, 0xc7, 0x8c,
    0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52
};


/* k = 2d */
static const byte ed25519_k[F25519_SIZE] = {
    0x59, 0xf1, 0xb2, 0x26, 0x94, 0x9b, 0xd6, 0xeb,
    0x56, 0xb1, 0x83, 0x82, 0x9a, 0x14, 0xe0, 0x00,
    0x30, 0xd1, 0xf3, 0xee, 0xf2, 0x80, 0x8e, 0x19,
    0xe7, 0xfc, 0xdf, 0x56, 0xdc, 0xd9, 0x06, 0x24
};


void ed25519_add(ge_p3 *r,
         const ge_p3 *p1, const ge_p3 *p2)
{
    /* Explicit formulas database: add-2008-hwcd-3
     *
     * source 2008 Hisil--Wong--Carter--Dawson,
     *     http://eprint.iacr.org/2008/522, Section 3.1
     * appliesto extended-1
     * parameter k
     * assume k = 2 d
     * compute A = (Y1-X1)(Y2-X2)
     * compute B = (Y1+X1)(Y2+X2)
     * compute C = T1 k T2
     * compute D = Z1 2 Z2
     * compute E = B - A
     * compute F = D - C
     * compute G = D + C
     * compute H = B + A
     * compute X3 = E F
     * compute Y3 = G H
     * compute T3 = E H
     * compute Z3 = F G
     */
    byte a[F25519_SIZE];
    byte b[F25519_SIZE];
    byte c[F25519_SIZE];
    byte d[F25519_SIZE];
    byte e[F25519_SIZE];
    byte f[F25519_SIZE];
    byte g[F25519_SIZE];
    byte h[F25519_SIZE];

    /* A = (Y1-X1)(Y2-X2) */
    lm_sub(c, p1->Y, p1->X);
    lm_sub(d, p2->Y, p2->X);
    fe_mul__distinct(a, c, d);

    /* B = (Y1+X1)(Y2+X2) */
    lm_add(c, p1->Y, p1->X);
    lm_add(d, p2->Y, p2->X);
    fe_mul__distinct(b, c, d);

    /* C = T1 k T2 */
    fe_mul__distinct(d, p1->T, p2->T);
    fe_mul__distinct(c, d, ed25519_k);

    /* D = Z1 2 Z2 */
    fe_mul__distinct(d, p1->Z, p2->Z);
    lm_add(d, d, d);

    /* E = B - A */
    lm_sub(e, b, a);

    /* F = D - C */
    lm_sub(f, d, c);

    /* G = D + C */
    lm_add(g, d, c);

    /* H = B + A */
    lm_add(h, b, a);

    /* X3 = E F */
    fe_mul__distinct(r->X, e, f);

    /* Y3 = G H */
    fe_mul__distinct(r->Y, g, h);

    /* T3 = E H */
    fe_mul__distinct(r->T, e, h);

    /* Z3 = F G */
    fe_mul__distinct(r->Z, f, g);
}


void ed25519_double(ge_p3 *r, const ge_p3 *p)
{
    /* Explicit formulas database: dbl-2008-hwcd
     *
     * source 2008 Hisil--Wong--Carter--Dawson,
     *     http://eprint.iacr.org/2008/522, Section 3.3
     * compute A = X1^2
     * compute B = Y1^2
     * compute C = 2 Z1^2
     * compute D = a A
     * compute E = (X1+Y1)^2-A-B
     * compute G = D + B
     * compute F = G - C
     * compute H = D - B
     * compute X3 = E F
     * compute Y3 = G H
     * compute T3 = E H
     * compute Z3 = F G
     */
    byte a[F25519_SIZE];
    byte b[F25519_SIZE];
    byte c[F25519_SIZE];
    byte e[F25519_SIZE];
    byte f[F25519_SIZE];
    byte g[F25519_SIZE];
    byte h[F25519_SIZE];

    /* A = X1^2 */
    fe_mul__distinct(a, p->X, p->X);

    /* B = Y1^2 */
    fe_mul__distinct(b, p->Y, p->Y);

    /* C = 2 Z1^2 */
    fe_mul__distinct(c, p->Z, p->Z);
    lm_add(c, c, c);

    /* D = a A (alter sign) */
    /* E = (X1+Y1)^2-A-B */
    lm_add(f, p->X, p->Y);
    fe_mul__distinct(e, f, f);
    lm_sub(e, e, a);
    lm_sub(e, e, b);

    /* G = D + B */
    lm_sub(g, b, a);

    /* F = G - C */
    lm_sub(f, g, c);

    /* H = D - B */
    lm_neg(h, b);
    lm_sub(h, h, a);

    /* X3 = E F */
    fe_mul__distinct(r->X, e, f);

    /* Y3 = G H */
    fe_mul__distinct(r->Y, g, h);

    /* T3 = E H */
    fe_mul__distinct(r->T, e, h);

    /* Z3 = F G */
    fe_mul__distinct(r->Z, f, g);
}


void ed25519_smult(ge_p3 *r_out, const ge_p3 *p, const byte *e)
{
    ge_p3 r;
    int   i;

    XMEMCPY(&r, &ed25519_neutral, sizeof(r));

    for (i = 255; i >= 0; i--) {
        const byte bit = (e[i >> 3] >> (i & 7)) & 1;
        ge_p3 s;

        ed25519_double(&r, &r);
        ed25519_add(&s, &r, p);

        fe_select(r.X, r.X, s.X, bit);
        fe_select(r.Y, r.Y, s.Y, bit);
        fe_select(r.Z, r.Z, s.Z, bit);
        fe_select(r.T, r.T, s.T, bit);
    }
    XMEMCPY(r_out, &r, sizeof(r));
}


void ge_scalarmult_base(ge_p3 *R,const unsigned char *nonce)
{
    ed25519_smult(R, &ed25519_base, nonce);
}


/* pack the point h into array s */
void ge_p3_tobytes(unsigned char *s,const ge_p3 *h)
{
    byte x[F25519_SIZE];
    byte y[F25519_SIZE];
    byte z1[F25519_SIZE];
    byte parity;

    fe_inv__distinct(z1, h->Z);
    fe_mul__distinct(x, h->X, z1);
    fe_mul__distinct(y, h->Y, z1);

    fe_normalize(x);
    fe_normalize(y);

    parity = (x[0] & 1) << 7;
    lm_copy(s, y);
    fe_normalize(s);
    s[31] |= parity;
}


/* pack the point h into array s */
void ge_tobytes(unsigned char *s,const ge_p2 *h)
{
    byte x[F25519_SIZE];
    byte y[F25519_SIZE];
    byte z1[F25519_SIZE];
    byte parity;

    fe_inv__distinct(z1, h->Z);
    fe_mul__distinct(x, h->X, z1);
    fe_mul__distinct(y, h->Y, z1);

    fe_normalize(x);
    fe_normalize(y);

    parity = (x[0] & 1) << 7;
    lm_copy(s, y);
    fe_normalize(s);
    s[31] |= parity;
}


/*
   Test if the public key can be uncompressed and negate it (-X,Y,Z,-T)
   return 0 on success
 */
int ge_frombytes_negate_vartime(ge_p3 *p,const unsigned char *s)
{

    byte parity;
    byte x[F25519_SIZE];
    byte y[F25519_SIZE];
    byte a[F25519_SIZE];
    byte b[F25519_SIZE];
    byte c[F25519_SIZE];
    int ret = 0;

    /* unpack the key s */
    parity = s[31] >> 7;
    lm_copy(y, s);
    y[31] &= 127;

    fe_mul__distinct(c, y, y);
    fe_mul__distinct(b, c, ed25519_d);
    lm_add(a, b, f25519_one);
    fe_inv__distinct(b, a);
    lm_sub(a, c, f25519_one);
    fe_mul__distinct(c, a, b);
    fe_sqrt(a, c);
    lm_neg(b, a);
    fe_select(x, a, b, (a[0] ^ parity) & 1);

    /* test that x^2 is equal to c */
    fe_mul__distinct(a, x, x);
    fe_normalize(a);
    fe_normalize(c);
    ret |= ConstantCompare(a, c, F25519_SIZE);

    /* project the key s onto p */
    lm_copy(p->X, x);
    lm_copy(p->Y, y);
    fe_load(p->Z, 1);
    fe_mul__distinct(p->T, x, y);

    /* negate, the point becomes (-X,Y,Z,-T) */
    lm_neg(p->X,p->X);
    lm_neg(p->T,p->T);

    return ret;
}


int ge_double_scalarmult_vartime(ge_p2* R, const unsigned char *h,
                                 const ge_p3 *inA,const unsigned char *sig)
{
    ge_p3 p, A;
    int ret = 0;

    XMEMCPY(&A, inA, sizeof(ge_p3));

    /* find SB */
    ed25519_smult(&p, &ed25519_base, sig);

    /* find H(R,A,M) * -A */
    ed25519_smult(&A, &A, h);

    /* SB + -H(R,A,M)A */
    ed25519_add(&A, &p, &A);

    lm_copy(R->X, A.X);
    lm_copy(R->Y, A.Y);
    lm_copy(R->Z, A.Z);

    return ret;
}

#endif /* ED25519_SMALL */
#endif /* HAVE_ED25519 */
