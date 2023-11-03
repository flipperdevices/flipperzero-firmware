/* ge_448.c
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

/* Based On Daniel J Bernstein's ed25519 Public Domain ref10 work.
 * Small implementation based on Daniel Beer's ed25519 public domain work.
 * Reworked for ed448 by Sean Parkinson.
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ED448

#include <wolfssl/wolfcrypt/ge_448.h>
#include <wolfssl/wolfcrypt/ed448.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/*
sc means scalar.
ge means group element.

Here the group is the set of pairs (x,y) of field elements (see ge_448.h)
satisfying -x^2 + y^2 = 1 + d x^2y^2
where d = -39081

Representations:
  ge448_p2 (projective) :  (X:Y:Z) satisfying x=X/Z, y=Y/Z
  ge448_precomp (affine):  (X:Y)
*/


#ifdef ED448_SMALL

/* Base point of ed448 */
static const ge448_p2 ed448_base = {
    { 0x5e, 0xc0, 0x0c, 0xc7, 0x2b, 0xa8, 0x26, 0x26, 0x8e, 0x93, 0x00, 0x8b,
      0xe1, 0x80, 0x3b, 0x43, 0x11, 0x65, 0xb6, 0x2a, 0xf7, 0x1a, 0xae, 0x12,
      0x64, 0xa4, 0xd3, 0xa3, 0x24, 0xe3, 0x6d, 0xea, 0x67, 0x17, 0x0f, 0x47,
      0x70, 0x65, 0x14, 0x9e, 0xda, 0x36, 0xbf, 0x22, 0xa6, 0x15, 0x1d, 0x22,
      0xed, 0x0d, 0xed, 0x6b, 0xc6, 0x70, 0x19, 0x4f },
    { 0x14, 0xfa, 0x30, 0xf2, 0x5b, 0x79, 0x08, 0x98, 0xad, 0xc8, 0xd7, 0x4e,
      0x2c, 0x13, 0xbd, 0xfd, 0xc4, 0x39, 0x7c, 0xe6, 0x1c, 0xff, 0xd3, 0x3a,
      0xd7, 0xc2, 0xa0, 0x05, 0x1e, 0x9c, 0x78, 0x87, 0x40, 0x98, 0xa3, 0x6c,
      0x73, 0x73, 0xea, 0x4b, 0x62, 0xc7, 0xc9, 0x56, 0x37, 0x20, 0x76, 0x88,
      0x24, 0xbc, 0xb6, 0x6e, 0x71, 0x46, 0x3f, 0x69 },
    { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

/* Part of order of ed448 that needs tp be multiplied when reducing */
static const word8 ed448_order_mul[56] = {
    0x0d, 0xbb, 0xa7, 0x54, 0x6d, 0x3d, 0x87, 0xdc, 0xaa, 0x70, 0x3a, 0x72,
    0x8d, 0x3d, 0x93, 0xde, 0x6f, 0xc9, 0x29, 0x51, 0xb6, 0x24, 0xb1, 0x3b,
    0x16, 0xdc, 0x35, 0x83,
};

/* Reduce scalar mod the order of the curve.
 * Scalar Will be 114 bytes.
 *
 * b  [in]  Scalar to reduce.
 */
void sc448_reduce(byte* b)
{
    int i, j;
    word32 t[114];
    word8 o;

    for (i = 0; i < 86; i++) {
        t[i] = b[i];
    }
    for (i = 0; i < 58; i++) {
        for (j = 0; j < 28; j++)
            t[i+j] += b[i+56] * ((word32)ed448_order_mul[j] << 2);
        t[i+56] = 0;
    }
    for (i = 54; i < 87; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    for (i = 0; i < 31; i++) {
        for (j = 0; j < 28; j++)
            t[i+j] += t[i+56] * ((word32)ed448_order_mul[j] << 2);
        t[i+56] = 0;
    }
    for (i = 54; i < 60; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 28; j++)
            t[i+j] += t[i+56] * ((word32)ed448_order_mul[j] << 2);
        t[i+56] = 0;
    }
    for (i = 0; i < 55; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    o = t[55] >> 6;
    t[55] &= 0x3f;
    for (j = 0; j < 28; j++)
        t[j] += o * (word32)ed448_order_mul[j];
    for (i = 0; i < 55; i++) {
        t[i+1] += t[i] >> 8;
        b[i] = t[i] & 0xff;
    }
    b[i] = t[i] & 0xff;
    b[i+1] = 0;
}

/* Multiply a by b and add d. r = (a * b + d) mod order
 *
 * r  [in]  Scalar to hold result.
 * a  [in]  Scalar to multiply.
 * b  [in]  Scalar to multiply.
 * d  [in]  Scalar to add to multiplicative result.
 */
void sc448_muladd(byte* r, const byte* a, const byte* b, const byte* d)
{
    int i, j;
    word32 t[112];
    word8 o;

    /* a * b + d */
    for (i = 0; i < 56; i++)
        t[i] = d[i];
    for (i = 0; i < 56; i++) {
        for (j = 0; j < 56; j++)
            t[i+j] += (sword16)a[i] * b[j];
        t[i+56] = 0;
    }

    for (i = 0; i < 111; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    for (i = 0; i < 56; i++) {
        for (j = 0; j < 28; j++)
            t[i+j] += t[i+56] * ((word32)ed448_order_mul[j] << 2);
        t[i+56] = 0;
    }
    for (i = 54; i < 85; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    for (i = 0; i < 29; i++) {
        for (j = 0; j < 28; j++)
            t[i+j] += t[i+56] * ((word32)ed448_order_mul[j] << 2);
        t[i+56] = 0;
    }
    for (i = 54; i < 58; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 28; j++)
            t[i+j] += t[i+56] * ((word32)ed448_order_mul[j] << 2);
        t[i+56] = 0;
    }
    for (i = 0; i < 55; i++) {
        t[i+1] += t[i] >> 8;
        t[i] &= 0xff;
    }
    o = t[55] >> 6;
    t[55] &= 0x3f;
    for (j = 0; j < 28; j++)
        t[j] += o * (word32)ed448_order_mul[j];
    for (i = 0; i < 55; i++) {
        t[i+1] += t[i] >> 8;
        r[i] = t[i] & 0xff;
    }
    r[i] = t[i] & 0xff;
    r[i+1] = 0;
}

/* Double the point on the Twisted Edwards curve. r = 2.p
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to double.
 */
static WC_INLINE void ge448_dbl(ge448_p2 *r,const ge448_p2 *p)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];

    fe448_add(t0, p->X, p->Y);    /* t0 = B1 = X1+Y1     */
    fe448_reduce(t0);
    fe448_sqr(t0, t0);            /* t0 = B = (X1+Y1)^2  */
    fe448_sqr(r->X, p->X);        /* r->X = C = X1^2     */
    fe448_sqr(r->Y, p->Y);        /* r->Y = D = Y1^2     */
    fe448_add(t1, r->X, r->Y);    /* t1 = E = C+D        */
    fe448_reduce(t1);
    fe448_sub(r->Y, r->X, r->Y);  /* r->Y = Y31 = C-D    */
    fe448_sqr(r->Z, p->Z);        /* r->Z = H = Z1^2     */
    fe448_add(r->Z, r->Z, r->Z);  /* r->Z = J1 = 2*H     */
    fe448_sub(r->Z, t1, r->Z);    /* r->Z = J = E-2*H    */
    fe448_reduce(r->Z);
    fe448_sub(r->X, t0, t1);      /* r->X = X31 = B-E    */
    fe448_mul(r->X, r->X, r->Z);  /* r->X = X3 = (B-E)*J */
    fe448_mul(r->Y, r->Y, t1);    /* r->Y = Y3 = E*(C-D) */
    fe448_mul(r->Z, t1, r->Z);    /* r->Z = Z3 = E*J     */
}

/* Add two point on the Twisted Edwards curve. r = p + q
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to add.
 * q  [in]  Point to add.
 */
static WC_INLINE void ge448_add(ge448_p2* r, const ge448_p2* p,
                                const ge448_p2* q)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];
    ge448 t2[GE448_WORDS];
    ge448 t3[GE448_WORDS];
    ge448 t4[GE448_WORDS];

    fe448_mul(t1, p->X, q->X);      /* t1 = C = X1*X2             */
    fe448_mul(t2, p->Y, q->Y);      /* t2 = D = Y1*Y2             */
    fe448_mul(t3, t1, t2);          /* t3 = E1 = C*D              */
    fe448_mul39081(t3, t3);         /* t3 = E = d*C*D             */
    fe448_mul(r->Z, p->Z, q->Z);    /* r->Z = A = Z1*Z2           */
    fe448_sqr(t0, r->Z);            /* t0 = B = A^2               */
    fe448_add(t4, t0, t3);          /* t4 = F = B-(-E)            */
    fe448_sub(t0, t0, t3);          /* t0 = G = B+(-E)            */
    fe448_reduce(t0);
    fe448_add(r->X, p->X, p->Y);    /* r->X = H1 = X1+Y1          */
    fe448_reduce(r->X);
    fe448_add(r->Y, q->X, q->Y);    /* r->Y = H2 = X2+Y2          */
    fe448_reduce(r->Y);
    fe448_mul(r->X, r->X, r->Y);    /* r->X = H = (X1+Y1)*(X2+Y2) */
    fe448_sub(r->X, r->X, t1);      /* r->X = X31 = H-C           */
    fe448_sub(r->X, r->X, t2);      /* r->X = X32 = H-C-D         */
    fe448_reduce(r->X);
    fe448_mul(r->X, r->X, t4);      /* r->X = X33 = F*(H-C-D)     */
    fe448_mul(r->X, r->X, r->Z);    /* r->X = X3 = A*F*(H-C-D)    */
    fe448_sub(r->Y, t2, t1);        /* r->Y = Y31 = D-C           */
    fe448_reduce(r->Y);
    fe448_mul(r->Y, r->Y, t0);      /* r->Y = Y32 = G*(D-C)       */
    fe448_mul(r->Y, r->Y, r->Z);    /* r->Y = Y3 = A*F*(D-C)      */
    fe448_mul(r->Z, t4, t0);        /* r->Z = Z3 = F*G            */
}

/* Convert point to byte array assuming projective ordinates.
 *
 * b  [in]  Array of bytes to hold compressed point.
 * p  [in]  Point to convert.
 */
void ge448_to_bytes(byte *s, const ge448_p2 *h)
{
    ge448 recip[56];
    ge448 x[56];

    fe448_invert(recip, h->Z);
    fe448_mul(x, h->X, recip);
    fe448_mul(s, h->Y, recip);
    fe448_norm(x);
    fe448_norm(s);
    s[56] = (x[0] & 1) << 7;
}

/* Compress the point to y-ordinate and negative bit.
 *
 * out    [in]  Array of bytes to hold compressed key.
 * xIn    [in]  The x-ordinate.
 * yIn    [in]  The y-ordinate.
 */
int ge448_compress_key(byte* out, const byte* xIn, const byte* yIn)
{
    ge448 x[56];

    fe448_copy(x, xIn);
    fe448_copy(out, yIn);
    fe448_norm(x);
    fe448_norm(out);
    out[56] = (x[0] & 1) << 7;

    return 0;
}

/* Perform a scalar multiplication of the a point. r = p * base
 *
 * r  [in]  Point to hold result.
 * a  [in]  Scalar to multiply by.
 */
static void ge448_scalarmult(ge448_p2* h, const ge448_p2* p, const byte* a)
{
    ge448_p2 r;
    ge448_p2 s;
    int i;

    XMEMSET(&r, 0, sizeof(r));
    r.Y[0] = 1;
    r.Z[0] = 1;

    for (i = 447; i >= 0; i--) {
        const byte bit = (a[i >> 3] >> (i & 7)) & 1;

        ge448_dbl(&r, &r);
        ge448_add(&s, &r, p);

        fe448_cmov(r.X, s.X, bit);
        fe448_cmov(r.Y, s.Y, bit);
        fe448_cmov(r.Z, s.Z, bit);
    }

    XMEMCPY(h, &r, sizeof(r));
}

/* Perform a scalar multiplication of the base point. r = a * base
 *
 * r  [in]  Point to hold result.
 * a  [in]  Scalar to multiply by.
 */
void ge448_scalarmult_base(ge448_p2* h, const byte* a)
{
    ge448_scalarmult(h, &ed448_base, a);
}

/* Perform a scalar multplication of the base point and public point.
 *   r = a * p + b * base
 * Uses a sliding window of 5 bits.
 * Not constant time.
 *
 * r  [in]  Point to hold result.
 * a  [in]  Scalar to multiply by.
 */
int ge448_double_scalarmult_vartime(ge448_p2 *r, const byte *a,
                                    const ge448_p2 *A, const byte *b)
{
    ge448_p2 t;

    ge448_scalarmult(&t, &ed448_base, b);
    ge448_scalarmult(r, A, a);
    ge448_add(r, r, &t);

    return 0;
}

/* Convert compressed point to negative of affine point.
 * Calculates x from the y and the negative bit.
 * Not constant time.
 *
 * r  [in]  Uncompressed point.
 * b  [in]  Array of bytes representing point.
 * returns 0 on success and -1 on failure.
 */
int ge448_from_bytes_negate_vartime(ge448_p2 *r, const byte *b)
{
    int   ret = 0;
    ge448 u[GE448_WORDS];
    ge448 v[GE448_WORDS];
    ge448 u3[GE448_WORDS];
    ge448 vxx[GE448_WORDS];
    ge448 check[GE448_WORDS];

    fe448_copy(r->Y, b);
    XMEMSET(r->Z, 0, sizeof(r->Z));
    r->Z[0] = 1;
    fe448_sqr(u, r->Y);                /* u = y^2                      */
    fe448_mul39081(v, u);              /* v = 39081.y^2                */
    fe448_sub(u, u, r->Z);             /* u = y^2-1                    */
    fe448_add(v, v, r->Z);             /* v = 39081.y^2-1              */
    fe448_neg(v, v);                   /* v = -39081.y^2-1 = d.y^2-1   */

    fe448_sqr(r->X, v);                /* x = v^2                      */
    fe448_mul(r->X, r->X, v);          /* x = v^3                      */
    fe448_sqr(u3, u);                  /* x = u^2.v^3                  */
    fe448_mul(r->X, r->X, u3);         /* x = u^2.v^3                  */
    fe448_mul(u3, u3, u);              /* u3 = u^3                     */
    fe448_mul(r->X, r->X, u3);         /* x = u^5.v^3                  */

    fe448_pow_2_446_222_1(r->X, r->X); /* x = (u^5.v^3)^((q-3)/4)      */
    fe448_mul(r->X, r->X, u3);         /* x = u^3(u^5.v^3)^((q-3)/4)   */
    fe448_mul(r->X, r->X, v);          /* x = u^3.v(u^5.v^3)^((q-3)/4) */

    fe448_sqr(vxx, r->X);
    fe448_mul(vxx, vxx, v);
    fe448_sub(check, vxx, u);          /* check = v.x^2-u              */
    fe448_norm(check);
    fe448_norm(r->X);
    fe448_norm(r->Y);
    /* Note; vx^2+u is NOT correct. */
    if (fe448_isnonzero(check)) {
        ret = -1;
    }

    /* Calculating negative of point in bytes - negate only if X is correct. */
    if ((r->X[0] & 1) == (b[56] >> 7)) {
        fe448_neg(r->X, r->X);
    }

    return ret;
}

#else /* !ED448_SMALL */

#if defined(CURVED448_128BIT)

/* Reduce scalar mod the order of the curve.
 * Scalar Will be 114 bytes.
 *
 * b  [in]  Scalar to reduce.
 */
void sc448_reduce(byte* b)
{
    word64 d[8];
    word128 t[17];
    word128 c;
    word64 o;

    /* Load from bytes */
    t[ 0] =  ((sword64) (b[ 0]) <<  0)
          |  ((sword64) (b[ 1]) <<  8)
          |  ((sword64) (b[ 2]) << 16)
          |  ((sword64) (b[ 3]) << 24)
          |  ((sword64) (b[ 4]) << 32)
          |  ((sword64) (b[ 5]) << 40)
          |  ((sword64) (b[ 6]) << 48);
    t[ 1] =  ((sword64) (b[ 7]) <<  0)
          |  ((sword64) (b[ 8]) <<  8)
          |  ((sword64) (b[ 9]) << 16)
          |  ((sword64) (b[10]) << 24)
          |  ((sword64) (b[11]) << 32)
          |  ((sword64) (b[12]) << 40)
          |  ((sword64) (b[13]) << 48);
    t[ 2] =  ((sword64) (b[14]) <<  0)
          |  ((sword64) (b[15]) <<  8)
          |  ((sword64) (b[16]) << 16)
          |  ((sword64) (b[17]) << 24)
          |  ((sword64) (b[18]) << 32)
          |  ((sword64) (b[19]) << 40)
          |  ((sword64) (b[20]) << 48);
    t[ 3] =  ((sword64) (b[21]) <<  0)
          |  ((sword64) (b[22]) <<  8)
          |  ((sword64) (b[23]) << 16)
          |  ((sword64) (b[24]) << 24)
          |  ((sword64) (b[25]) << 32)
          |  ((sword64) (b[26]) << 40)
          |  ((sword64) (b[27]) << 48);
    t[ 4] =  ((sword64) (b[28]) <<  0)
          |  ((sword64) (b[29]) <<  8)
          |  ((sword64) (b[30]) << 16)
          |  ((sword64) (b[31]) << 24)
          |  ((sword64) (b[32]) << 32)
          |  ((sword64) (b[33]) << 40)
          |  ((sword64) (b[34]) << 48);
    t[ 5] =  ((sword64) (b[35]) <<  0)
          |  ((sword64) (b[36]) <<  8)
          |  ((sword64) (b[37]) << 16)
          |  ((sword64) (b[38]) << 24)
          |  ((sword64) (b[39]) << 32)
          |  ((sword64) (b[40]) << 40)
          |  ((sword64) (b[41]) << 48);
    t[ 6] =  ((sword64) (b[42]) <<  0)
          |  ((sword64) (b[43]) <<  8)
          |  ((sword64) (b[44]) << 16)
          |  ((sword64) (b[45]) << 24)
          |  ((sword64) (b[46]) << 32)
          |  ((sword64) (b[47]) << 40)
          |  ((sword64) (b[48]) << 48);
    t[ 7] =  ((sword64) (b[49]) <<  0)
          |  ((sword64) (b[50]) <<  8)
          |  ((sword64) (b[51]) << 16)
          |  ((sword64) (b[52]) << 24)
          |  ((sword64) (b[53]) << 32)
          |  ((sword64) (b[54]) << 40)
          |  ((sword64) (b[55]) << 48);
    t[ 8] =  ((sword64) (b[56]) <<  0)
          |  ((sword64) (b[57]) <<  8)
          |  ((sword64) (b[58]) << 16)
          |  ((sword64) (b[59]) << 24)
          |  ((sword64) (b[60]) << 32)
          |  ((sword64) (b[61]) << 40)
          |  ((sword64) (b[62]) << 48);
    t[ 9] =  ((sword64) (b[63]) <<  0)
          |  ((sword64) (b[64]) <<  8)
          |  ((sword64) (b[65]) << 16)
          |  ((sword64) (b[66]) << 24)
          |  ((sword64) (b[67]) << 32)
          |  ((sword64) (b[68]) << 40)
          |  ((sword64) (b[69]) << 48);
    t[10] =  ((sword64) (b[70]) <<  0)
          |  ((sword64) (b[71]) <<  8)
          |  ((sword64) (b[72]) << 16)
          |  ((sword64) (b[73]) << 24)
          |  ((sword64) (b[74]) << 32)
          |  ((sword64) (b[75]) << 40)
          |  ((sword64) (b[76]) << 48);
    t[11] =  ((sword64) (b[77]) <<  0)
          |  ((sword64) (b[78]) <<  8)
          |  ((sword64) (b[79]) << 16)
          |  ((sword64) (b[80]) << 24)
          |  ((sword64) (b[81]) << 32)
          |  ((sword64) (b[82]) << 40)
          |  ((sword64) (b[83]) << 48);
    t[12] =  ((sword64) (b[84]) <<  0)
          |  ((sword64) (b[85]) <<  8)
          |  ((sword64) (b[86]) << 16)
          |  ((sword64) (b[87]) << 24)
          |  ((sword64) (b[88]) << 32)
          |  ((sword64) (b[89]) << 40)
          |  ((sword64) (b[90]) << 48);
    t[13] =  ((sword64) (b[91]) <<  0)
          |  ((sword64) (b[92]) <<  8)
          |  ((sword64) (b[93]) << 16)
          |  ((sword64) (b[94]) << 24)
          |  ((sword64) (b[95]) << 32)
          |  ((sword64) (b[96]) << 40)
          |  ((sword64) (b[97]) << 48);
    t[14] =  ((sword64) (b[98]) <<  0)
          |  ((sword64) (b[99]) <<  8)
          |  ((sword64) (b[100]) << 16)
          |  ((sword64) (b[101]) << 24)
          |  ((sword64) (b[102]) << 32)
          |  ((sword64) (b[103]) << 40)
          |  ((sword64) (b[104]) << 48);
    t[15] =  ((sword64) (b[105]) <<  0)
          |  ((sword64) (b[106]) <<  8)
          |  ((sword64) (b[107]) << 16)
          |  ((sword64) (b[108]) << 24)
          |  ((sword64) (b[109]) << 32)
          |  ((sword64) (b[110]) << 40)
          |  ((sword64) (b[111]) << 48);
    t[16] =  ((sword64) (b[112]) <<  0)
          |  ((sword64) (b[113]) <<  8);

    /* Mod curve order */
    /* 2^446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d */
    /* Mod top half of extra words */
    t[ 4] += (sword128)0x21cf5b5529eec34L * t[12];
    t[ 5] += (sword128)0x0f635c8e9c2ab70L * t[12];
    t[ 6] += (sword128)0x2d944a725bf7a4cL * t[12];
    t[ 7] += (sword128)0x20cd77058eec490L * t[12];
    t[ 5] += (sword128)0x21cf5b5529eec34L * t[13];
    t[ 6] += (sword128)0x0f635c8e9c2ab70L * t[13];
    t[ 7] += (sword128)0x2d944a725bf7a4cL * t[13];
    t[ 8] += (sword128)0x20cd77058eec490L * t[13];
    t[ 6] += (sword128)0x21cf5b5529eec34L * t[14];
    t[ 7] += (sword128)0x0f635c8e9c2ab70L * t[14];
    t[ 8] += (sword128)0x2d944a725bf7a4cL * t[14];
    t[ 9] += (sword128)0x20cd77058eec490L * t[14];
    t[ 7] += (sword128)0x21cf5b5529eec34L * t[15];
    t[ 8] += (sword128)0x0f635c8e9c2ab70L * t[15];
    t[ 9] += (sword128)0x2d944a725bf7a4cL * t[15];
    t[10] += (sword128)0x20cd77058eec490L * t[15];
    t[ 8] += (sword128)0x21cf5b5529eec34L * t[16];
    t[ 9] += (sword128)0x0f635c8e9c2ab70L * t[16];
    t[10] += (sword128)0x2d944a725bf7a4cL * t[16];
    t[11] += (sword128)0x20cd77058eec490L * t[16];
    t[12]  = 0;
    /* Propagate carries */
    c = t[ 4] >> 56; t[ 5] += c; t[ 4] = t[ 4] & 0xffffffffffffff;
    c = t[ 5] >> 56; t[ 6] += c; t[ 5] = t[ 5] & 0xffffffffffffff;
    c = t[ 6] >> 56; t[ 7] += c; t[ 6] = t[ 6] & 0xffffffffffffff;
    c = t[ 7] >> 56; t[ 8] += c; t[ 7] = t[ 7] & 0xffffffffffffff;
    c = t[ 8] >> 56; t[ 9] += c; t[ 8] = t[ 8] & 0xffffffffffffff;
    c = t[ 9] >> 56; t[10] += c; t[ 9] = t[ 9] & 0xffffffffffffff;
    c = t[10] >> 56; t[11] += c; t[10] = t[10] & 0xffffffffffffff;
    c = t[11] >> 56; t[12] += c; t[11] = t[11] & 0xffffffffffffff;
    /* Mod bottom half of extra words */
    t[ 0] += (sword128)0x21cf5b5529eec34L * t[ 8];
    t[ 1] += (sword128)0x0f635c8e9c2ab70L * t[ 8];
    t[ 2] += (sword128)0x2d944a725bf7a4cL * t[ 8];
    t[ 3] += (sword128)0x20cd77058eec490L * t[ 8];
    t[ 1] += (sword128)0x21cf5b5529eec34L * t[ 9];
    t[ 2] += (sword128)0x0f635c8e9c2ab70L * t[ 9];
    t[ 3] += (sword128)0x2d944a725bf7a4cL * t[ 9];
    t[ 4] += (sword128)0x20cd77058eec490L * t[ 9];
    t[ 2] += (sword128)0x21cf5b5529eec34L * t[10];
    t[ 3] += (sword128)0x0f635c8e9c2ab70L * t[10];
    t[ 4] += (sword128)0x2d944a725bf7a4cL * t[10];
    t[ 5] += (sword128)0x20cd77058eec490L * t[10];
    t[ 3] += (sword128)0x21cf5b5529eec34L * t[11];
    t[ 4] += (sword128)0x0f635c8e9c2ab70L * t[11];
    t[ 5] += (sword128)0x2d944a725bf7a4cL * t[11];
    t[ 6] += (sword128)0x20cd77058eec490L * t[11];
    t[ 4] += (sword128)0x21cf5b5529eec34L * t[12];
    t[ 5] += (sword128)0x0f635c8e9c2ab70L * t[12];
    t[ 6] += (sword128)0x2d944a725bf7a4cL * t[12];
    t[ 7] += (sword128)0x20cd77058eec490L * t[12];
    t[ 8]  = 0;
    /* Propagate carries */
    c = t[ 0] >> 56; t[ 1] += c; t[ 0] = t[ 0] & 0xffffffffffffff;
    c = t[ 1] >> 56; t[ 2] += c; t[ 1] = t[ 1] & 0xffffffffffffff;
    c = t[ 2] >> 56; t[ 3] += c; t[ 2] = t[ 2] & 0xffffffffffffff;
    c = t[ 3] >> 56; t[ 4] += c; t[ 3] = t[ 3] & 0xffffffffffffff;
    c = t[ 4] >> 56; t[ 5] += c; t[ 4] = t[ 4] & 0xffffffffffffff;
    c = t[ 5] >> 56; t[ 6] += c; t[ 5] = t[ 5] & 0xffffffffffffff;
    c = t[ 6] >> 56; t[ 7] += c; t[ 6] = t[ 6] & 0xffffffffffffff;
    c = t[ 7] >> 56; t[ 8] += c; t[ 7] = t[ 7] & 0xffffffffffffff;
    t[ 0] += (sword128)0x21cf5b5529eec34L * t[ 8];
    t[ 1] += (sword128)0x0f635c8e9c2ab70L * t[ 8];
    t[ 2] += (sword128)0x2d944a725bf7a4cL * t[ 8];
    t[ 3] += (sword128)0x20cd77058eec490L * t[ 8];
    /* Propagate carries */
    c = t[ 0] >> 56; t[ 1] += c; d[ 0] = (sword64)(t[ 0] & 0xffffffffffffff);
    c = t[ 1] >> 56; t[ 2] += c; d[ 1] = (sword64)(t[ 1] & 0xffffffffffffff);
    c = t[ 2] >> 56; t[ 3] += c; d[ 2] = (sword64)(t[ 2] & 0xffffffffffffff);
    c = t[ 3] >> 56; t[ 4] += c; d[ 3] = (sword64)(t[ 3] & 0xffffffffffffff);
    c = t[ 4] >> 56; t[ 5] += c; d[ 4] = (sword64)(t[ 4] & 0xffffffffffffff);
    c = t[ 5] >> 56; t[ 6] += c; d[ 5] = (sword64)(t[ 5] & 0xffffffffffffff);
    c = t[ 6] >> 56; t[ 7] += c; d[ 6] = (sword64)(t[ 6] & 0xffffffffffffff);
    d[ 7] = (word64)t[7];
    /* Mod bits over 56 in last word */
    o = d[7] >> 54; d[ 7] &= 0x3fffffffffffff;
    d[ 0] += 0x873d6d54a7bb0dL * o;
    d[ 1] += 0x3d8d723a70aadcL * o;
    d[ 2] += 0xb65129c96fde93L * o;
    d[ 3] += 0x8335dc163bb124L * o;
    /* Propagate carries */
    o = d[ 0] >> 56; d[ 1] += o; d[ 0] = d[ 0] & 0xffffffffffffff;
    o = d[ 1] >> 56; d[ 2] += o; d[ 1] = d[ 1] & 0xffffffffffffff;
    o = d[ 2] >> 56; d[ 3] += o; d[ 2] = d[ 2] & 0xffffffffffffff;
    o = d[ 3] >> 56; d[ 4] += o; d[ 3] = d[ 3] & 0xffffffffffffff;
    o = d[ 4] >> 56; d[ 5] += o; d[ 4] = d[ 4] & 0xffffffffffffff;
    o = d[ 5] >> 56; d[ 6] += o; d[ 5] = d[ 5] & 0xffffffffffffff;
    o = d[ 6] >> 56; d[ 7] += o; d[ 6] = d[ 6] & 0xffffffffffffff;

    /* Convert to bytes */
    b[ 0] = (byte)(d[0 ] >>  0);
    b[ 1] = (byte)(d[0 ] >>  8);
    b[ 2] = (byte)(d[0 ] >> 16);
    b[ 3] = (byte)(d[0 ] >> 24);
    b[ 4] = (byte)(d[0 ] >> 32);
    b[ 5] = (byte)(d[0 ] >> 40);
    b[ 6] = (byte)(d[0 ] >> 48);
    b[ 7] = (byte)(d[1 ] >>  0);
    b[ 8] = (byte)(d[1 ] >>  8);
    b[ 9] = (byte)(d[1 ] >> 16);
    b[10] = (byte)(d[1 ] >> 24);
    b[11] = (byte)(d[1 ] >> 32);
    b[12] = (byte)(d[1 ] >> 40);
    b[13] = (byte)(d[1 ] >> 48);
    b[14] = (byte)(d[2 ] >>  0);
    b[15] = (byte)(d[2 ] >>  8);
    b[16] = (byte)(d[2 ] >> 16);
    b[17] = (byte)(d[2 ] >> 24);
    b[18] = (byte)(d[2 ] >> 32);
    b[19] = (byte)(d[2 ] >> 40);
    b[20] = (byte)(d[2 ] >> 48);
    b[21] = (byte)(d[3 ] >>  0);
    b[22] = (byte)(d[3 ] >>  8);
    b[23] = (byte)(d[3 ] >> 16);
    b[24] = (byte)(d[3 ] >> 24);
    b[25] = (byte)(d[3 ] >> 32);
    b[26] = (byte)(d[3 ] >> 40);
    b[27] = (byte)(d[3 ] >> 48);
    b[28] = (byte)(d[4 ] >>  0);
    b[29] = (byte)(d[4 ] >>  8);
    b[30] = (byte)(d[4 ] >> 16);
    b[31] = (byte)(d[4 ] >> 24);
    b[32] = (byte)(d[4 ] >> 32);
    b[33] = (byte)(d[4 ] >> 40);
    b[34] = (byte)(d[4 ] >> 48);
    b[35] = (byte)(d[5 ] >>  0);
    b[36] = (byte)(d[5 ] >>  8);
    b[37] = (byte)(d[5 ] >> 16);
    b[38] = (byte)(d[5 ] >> 24);
    b[39] = (byte)(d[5 ] >> 32);
    b[40] = (byte)(d[5 ] >> 40);
    b[41] = (byte)(d[5 ] >> 48);
    b[42] = (byte)(d[6 ] >>  0);
    b[43] = (byte)(d[6 ] >>  8);
    b[44] = (byte)(d[6 ] >> 16);
    b[45] = (byte)(d[6 ] >> 24);
    b[46] = (byte)(d[6 ] >> 32);
    b[47] = (byte)(d[6 ] >> 40);
    b[48] = (byte)(d[6 ] >> 48);
    b[49] = (byte)(d[7 ] >>  0);
    b[50] = (byte)(d[7 ] >>  8);
    b[51] = (byte)(d[7 ] >> 16);
    b[52] = (byte)(d[7 ] >> 24);
    b[53] = (byte)(d[7 ] >> 32);
    b[54] = (byte)(d[7 ] >> 40);
    b[55] = (byte)(d[7 ] >> 48);
    b[56] = 0;
}

/* Multiply a by b and add d. r = (a * b + d) mod order
 *
 * r  [in]  Scalar to hold result.
 * a  [in]  Scalar to multiply.
 * b  [in]  Scalar to multiply.
 * d  [in]  Scalar to add to multiplicative result.
 */
void sc448_muladd(byte* r, const byte* a, const byte* b, const byte* d)
{
    word64 ad[8], bd[8], dd[8], rd[8];
    word128 t[16];
    word128 c;
    word64 o;

    /* Load from bytes */
    ad[ 0] =  ((sword64) (a[ 0]) <<  0)
           |  ((sword64) (a[ 1]) <<  8)
           |  ((sword64) (a[ 2]) << 16)
           |  ((sword64) (a[ 3]) << 24)
           |  ((sword64) (a[ 4]) << 32)
           |  ((sword64) (a[ 5]) << 40)
           |  ((sword64) (a[ 6]) << 48);
    ad[ 1] =  ((sword64) (a[ 7]) <<  0)
           |  ((sword64) (a[ 8]) <<  8)
           |  ((sword64) (a[ 9]) << 16)
           |  ((sword64) (a[10]) << 24)
           |  ((sword64) (a[11]) << 32)
           |  ((sword64) (a[12]) << 40)
           |  ((sword64) (a[13]) << 48);
    ad[ 2] =  ((sword64) (a[14]) <<  0)
           |  ((sword64) (a[15]) <<  8)
           |  ((sword64) (a[16]) << 16)
           |  ((sword64) (a[17]) << 24)
           |  ((sword64) (a[18]) << 32)
           |  ((sword64) (a[19]) << 40)
           |  ((sword64) (a[20]) << 48);
    ad[ 3] =  ((sword64) (a[21]) <<  0)
           |  ((sword64) (a[22]) <<  8)
           |  ((sword64) (a[23]) << 16)
           |  ((sword64) (a[24]) << 24)
           |  ((sword64) (a[25]) << 32)
           |  ((sword64) (a[26]) << 40)
           |  ((sword64) (a[27]) << 48);
    ad[ 4] =  ((sword64) (a[28]) <<  0)
           |  ((sword64) (a[29]) <<  8)
           |  ((sword64) (a[30]) << 16)
           |  ((sword64) (a[31]) << 24)
           |  ((sword64) (a[32]) << 32)
           |  ((sword64) (a[33]) << 40)
           |  ((sword64) (a[34]) << 48);
    ad[ 5] =  ((sword64) (a[35]) <<  0)
           |  ((sword64) (a[36]) <<  8)
           |  ((sword64) (a[37]) << 16)
           |  ((sword64) (a[38]) << 24)
           |  ((sword64) (a[39]) << 32)
           |  ((sword64) (a[40]) << 40)
           |  ((sword64) (a[41]) << 48);
    ad[ 6] =  ((sword64) (a[42]) <<  0)
           |  ((sword64) (a[43]) <<  8)
           |  ((sword64) (a[44]) << 16)
           |  ((sword64) (a[45]) << 24)
           |  ((sword64) (a[46]) << 32)
           |  ((sword64) (a[47]) << 40)
           |  ((sword64) (a[48]) << 48);
    ad[ 7] =  ((sword64) (a[49]) <<  0)
           |  ((sword64) (a[50]) <<  8)
           |  ((sword64) (a[51]) << 16)
           |  ((sword64) (a[52]) << 24)
           |  ((sword64) (a[53]) << 32)
           |  ((sword64) (a[54]) << 40)
           |  ((sword64) (a[55]) << 48);
    /* Load from bytes */
    bd[ 0] =  ((sword64) (b[ 0]) <<  0)
           |  ((sword64) (b[ 1]) <<  8)
           |  ((sword64) (b[ 2]) << 16)
           |  ((sword64) (b[ 3]) << 24)
           |  ((sword64) (b[ 4]) << 32)
           |  ((sword64) (b[ 5]) << 40)
           |  ((sword64) (b[ 6]) << 48);
    bd[ 1] =  ((sword64) (b[ 7]) <<  0)
           |  ((sword64) (b[ 8]) <<  8)
           |  ((sword64) (b[ 9]) << 16)
           |  ((sword64) (b[10]) << 24)
           |  ((sword64) (b[11]) << 32)
           |  ((sword64) (b[12]) << 40)
           |  ((sword64) (b[13]) << 48);
    bd[ 2] =  ((sword64) (b[14]) <<  0)
           |  ((sword64) (b[15]) <<  8)
           |  ((sword64) (b[16]) << 16)
           |  ((sword64) (b[17]) << 24)
           |  ((sword64) (b[18]) << 32)
           |  ((sword64) (b[19]) << 40)
           |  ((sword64) (b[20]) << 48);
    bd[ 3] =  ((sword64) (b[21]) <<  0)
           |  ((sword64) (b[22]) <<  8)
           |  ((sword64) (b[23]) << 16)
           |  ((sword64) (b[24]) << 24)
           |  ((sword64) (b[25]) << 32)
           |  ((sword64) (b[26]) << 40)
           |  ((sword64) (b[27]) << 48);
    bd[ 4] =  ((sword64) (b[28]) <<  0)
           |  ((sword64) (b[29]) <<  8)
           |  ((sword64) (b[30]) << 16)
           |  ((sword64) (b[31]) << 24)
           |  ((sword64) (b[32]) << 32)
           |  ((sword64) (b[33]) << 40)
           |  ((sword64) (b[34]) << 48);
    bd[ 5] =  ((sword64) (b[35]) <<  0)
           |  ((sword64) (b[36]) <<  8)
           |  ((sword64) (b[37]) << 16)
           |  ((sword64) (b[38]) << 24)
           |  ((sword64) (b[39]) << 32)
           |  ((sword64) (b[40]) << 40)
           |  ((sword64) (b[41]) << 48);
    bd[ 6] =  ((sword64) (b[42]) <<  0)
           |  ((sword64) (b[43]) <<  8)
           |  ((sword64) (b[44]) << 16)
           |  ((sword64) (b[45]) << 24)
           |  ((sword64) (b[46]) << 32)
           |  ((sword64) (b[47]) << 40)
           |  ((sword64) (b[48]) << 48);
    bd[ 7] =  ((sword64) (b[49]) <<  0)
           |  ((sword64) (b[50]) <<  8)
           |  ((sword64) (b[51]) << 16)
           |  ((sword64) (b[52]) << 24)
           |  ((sword64) (b[53]) << 32)
           |  ((sword64) (b[54]) << 40)
           |  ((sword64) (b[55]) << 48);
    /* Load from bytes */
    dd[ 0] =  ((sword64) (d[ 0]) <<  0)
           |  ((sword64) (d[ 1]) <<  8)
           |  ((sword64) (d[ 2]) << 16)
           |  ((sword64) (d[ 3]) << 24)
           |  ((sword64) (d[ 4]) << 32)
           |  ((sword64) (d[ 5]) << 40)
           |  ((sword64) (d[ 6]) << 48);
    dd[ 1] =  ((sword64) (d[ 7]) <<  0)
           |  ((sword64) (d[ 8]) <<  8)
           |  ((sword64) (d[ 9]) << 16)
           |  ((sword64) (d[10]) << 24)
           |  ((sword64) (d[11]) << 32)
           |  ((sword64) (d[12]) << 40)
           |  ((sword64) (d[13]) << 48);
    dd[ 2] =  ((sword64) (d[14]) <<  0)
           |  ((sword64) (d[15]) <<  8)
           |  ((sword64) (d[16]) << 16)
           |  ((sword64) (d[17]) << 24)
           |  ((sword64) (d[18]) << 32)
           |  ((sword64) (d[19]) << 40)
           |  ((sword64) (d[20]) << 48);
    dd[ 3] =  ((sword64) (d[21]) <<  0)
           |  ((sword64) (d[22]) <<  8)
           |  ((sword64) (d[23]) << 16)
           |  ((sword64) (d[24]) << 24)
           |  ((sword64) (d[25]) << 32)
           |  ((sword64) (d[26]) << 40)
           |  ((sword64) (d[27]) << 48);
    dd[ 4] =  ((sword64) (d[28]) <<  0)
           |  ((sword64) (d[29]) <<  8)
           |  ((sword64) (d[30]) << 16)
           |  ((sword64) (d[31]) << 24)
           |  ((sword64) (d[32]) << 32)
           |  ((sword64) (d[33]) << 40)
           |  ((sword64) (d[34]) << 48);
    dd[ 5] =  ((sword64) (d[35]) <<  0)
           |  ((sword64) (d[36]) <<  8)
           |  ((sword64) (d[37]) << 16)
           |  ((sword64) (d[38]) << 24)
           |  ((sword64) (d[39]) << 32)
           |  ((sword64) (d[40]) << 40)
           |  ((sword64) (d[41]) << 48);
    dd[ 6] =  ((sword64) (d[42]) <<  0)
           |  ((sword64) (d[43]) <<  8)
           |  ((sword64) (d[44]) << 16)
           |  ((sword64) (d[45]) << 24)
           |  ((sword64) (d[46]) << 32)
           |  ((sword64) (d[47]) << 40)
           |  ((sword64) (d[48]) << 48);
    dd[ 7] =  ((sword64) (d[49]) <<  0)
           |  ((sword64) (d[50]) <<  8)
           |  ((sword64) (d[51]) << 16)
           |  ((sword64) (d[52]) << 24)
           |  ((sword64) (d[53]) << 32)
           |  ((sword64) (d[54]) << 40)
           |  ((sword64) (d[55]) << 48);

    /* a * b + d */
    t[ 0] = (word128)dd[ 0] + (sword128)ad[ 0] * bd[ 0];
    t[ 1] = (word128)dd[ 1] + (sword128)ad[ 0] * bd[ 1]
                            + (sword128)ad[ 1] * bd[ 0];
    t[ 2] = (word128)dd[ 2] + (sword128)ad[ 0] * bd[ 2]
                            + (sword128)ad[ 1] * bd[ 1]
                            + (sword128)ad[ 2] * bd[ 0];
    t[ 3] = (word128)dd[ 3] + (sword128)ad[ 0] * bd[ 3]
                            + (sword128)ad[ 1] * bd[ 2]
                            + (sword128)ad[ 2] * bd[ 1]
                            + (sword128)ad[ 3] * bd[ 0];
    t[ 4] = (word128)dd[ 4] + (sword128)ad[ 0] * bd[ 4]
                            + (sword128)ad[ 1] * bd[ 3]
                            + (sword128)ad[ 2] * bd[ 2]
                            + (sword128)ad[ 3] * bd[ 1]
                            + (sword128)ad[ 4] * bd[ 0];
    t[ 5] = (word128)dd[ 5] + (sword128)ad[ 0] * bd[ 5]
                            + (sword128)ad[ 1] * bd[ 4]
                            + (sword128)ad[ 2] * bd[ 3]
                            + (sword128)ad[ 3] * bd[ 2]
                            + (sword128)ad[ 4] * bd[ 1]
                            + (sword128)ad[ 5] * bd[ 0];
    t[ 6] = (word128)dd[ 6] + (sword128)ad[ 0] * bd[ 6]
                            + (sword128)ad[ 1] * bd[ 5]
                            + (sword128)ad[ 2] * bd[ 4]
                            + (sword128)ad[ 3] * bd[ 3]
                            + (sword128)ad[ 4] * bd[ 2]
                            + (sword128)ad[ 5] * bd[ 1]
                            + (sword128)ad[ 6] * bd[ 0];
    t[ 7] = (word128)dd[ 7] + (sword128)ad[ 0] * bd[ 7]
                            + (sword128)ad[ 1] * bd[ 6]
                            + (sword128)ad[ 2] * bd[ 5]
                            + (sword128)ad[ 3] * bd[ 4]
                            + (sword128)ad[ 4] * bd[ 3]
                            + (sword128)ad[ 5] * bd[ 2]
                            + (sword128)ad[ 6] * bd[ 1]
                            + (sword128)ad[ 7] * bd[ 0];
    t[ 8] = (word128)          (sword128)ad[ 1] * bd[ 7]
                            + (sword128)ad[ 2] * bd[ 6]
                            + (sword128)ad[ 3] * bd[ 5]
                            + (sword128)ad[ 4] * bd[ 4]
                            + (sword128)ad[ 5] * bd[ 3]
                            + (sword128)ad[ 6] * bd[ 2]
                            + (sword128)ad[ 7] * bd[ 1];
    t[ 9] = (word128)          (sword128)ad[ 2] * bd[ 7]
                            + (sword128)ad[ 3] * bd[ 6]
                            + (sword128)ad[ 4] * bd[ 5]
                            + (sword128)ad[ 5] * bd[ 4]
                            + (sword128)ad[ 6] * bd[ 3]
                            + (sword128)ad[ 7] * bd[ 2];
    t[10] = (word128)          (sword128)ad[ 3] * bd[ 7]
                            + (sword128)ad[ 4] * bd[ 6]
                            + (sword128)ad[ 5] * bd[ 5]
                            + (sword128)ad[ 6] * bd[ 4]
                            + (sword128)ad[ 7] * bd[ 3];
    t[11] = (word128)          (sword128)ad[ 4] * bd[ 7]
                            + (sword128)ad[ 5] * bd[ 6]
                            + (sword128)ad[ 6] * bd[ 5]
                            + (sword128)ad[ 7] * bd[ 4];
    t[12] = (word128)          (sword128)ad[ 5] * bd[ 7]
                            + (sword128)ad[ 6] * bd[ 6]
                            + (sword128)ad[ 7] * bd[ 5];
    t[13] = (word128)          (sword128)ad[ 6] * bd[ 7]
                            + (sword128)ad[ 7] * bd[ 6];
    t[14] = (word128)          (sword128)ad[ 7] * bd[ 7];
    t[15] = 0;

    /* Mod curve order */
    /* 2^446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d */
    /* Propagate carries */
    c = t[ 0] >> 56; t[ 1] += c; t[ 0] = t[ 0] & 0xffffffffffffff;
    c = t[ 1] >> 56; t[ 2] += c; t[ 1] = t[ 1] & 0xffffffffffffff;
    c = t[ 2] >> 56; t[ 3] += c; t[ 2] = t[ 2] & 0xffffffffffffff;
    c = t[ 3] >> 56; t[ 4] += c; t[ 3] = t[ 3] & 0xffffffffffffff;
    c = t[ 4] >> 56; t[ 5] += c; t[ 4] = t[ 4] & 0xffffffffffffff;
    c = t[ 5] >> 56; t[ 6] += c; t[ 5] = t[ 5] & 0xffffffffffffff;
    c = t[ 6] >> 56; t[ 7] += c; t[ 6] = t[ 6] & 0xffffffffffffff;
    c = t[ 7] >> 56; t[ 8] += c; t[ 7] = t[ 7] & 0xffffffffffffff;
    c = t[ 8] >> 56; t[ 9] += c; t[ 8] = t[ 8] & 0xffffffffffffff;
    c = t[ 9] >> 56; t[10] += c; t[ 9] = t[ 9] & 0xffffffffffffff;
    c = t[10] >> 56; t[11] += c; t[10] = t[10] & 0xffffffffffffff;
    c = t[11] >> 56; t[12] += c; t[11] = t[11] & 0xffffffffffffff;
    c = t[12] >> 56; t[13] += c; t[12] = t[12] & 0xffffffffffffff;
    c = t[13] >> 56; t[14] += c; t[13] = t[13] & 0xffffffffffffff;
    c = t[14] >> 56; t[15] += c; t[14] = t[14] & 0xffffffffffffff;
    /* Mod top half of extra words */
    t[ 4] += (sword128)0x21cf5b5529eec34L * t[12];
    t[ 5] += (sword128)0x0f635c8e9c2ab70L * t[12];
    t[ 6] += (sword128)0x2d944a725bf7a4cL * t[12];
    t[ 7] += (sword128)0x20cd77058eec490L * t[12];
    t[ 5] += (sword128)0x21cf5b5529eec34L * t[13];
    t[ 6] += (sword128)0x0f635c8e9c2ab70L * t[13];
    t[ 7] += (sword128)0x2d944a725bf7a4cL * t[13];
    t[ 8] += (sword128)0x20cd77058eec490L * t[13];
    t[ 6] += (sword128)0x21cf5b5529eec34L * t[14];
    t[ 7] += (sword128)0x0f635c8e9c2ab70L * t[14];
    t[ 8] += (sword128)0x2d944a725bf7a4cL * t[14];
    t[ 9] += (sword128)0x20cd77058eec490L * t[14];
    t[ 7] += (sword128)0x21cf5b5529eec34L * t[15];
    t[ 8] += (sword128)0x0f635c8e9c2ab70L * t[15];
    t[ 9] += (sword128)0x2d944a725bf7a4cL * t[15];
    t[10] += (sword128)0x20cd77058eec490L * t[15];
    /* Propagate carries */
    c = t[ 4] >> 56; t[ 5] += c; t[ 4] = t[ 4] & 0xffffffffffffff;
    c = t[ 5] >> 56; t[ 6] += c; t[ 5] = t[ 5] & 0xffffffffffffff;
    c = t[ 6] >> 56; t[ 7] += c; t[ 6] = t[ 6] & 0xffffffffffffff;
    c = t[ 7] >> 56; t[ 8] += c; t[ 7] = t[ 7] & 0xffffffffffffff;
    c = t[ 8] >> 56; t[ 9] += c; t[ 8] = t[ 8] & 0xffffffffffffff;
    c = t[ 9] >> 56; t[10] += c; t[ 9] = t[ 9] & 0xffffffffffffff;
    c = t[10] >> 56; t[11] += c; t[10] = t[10] & 0xffffffffffffff;
    /* Mod bottom half of extra words */
    t[ 0] += (sword128)0x21cf5b5529eec34L * t[ 8];
    t[ 1] += (sword128)0x0f635c8e9c2ab70L * t[ 8];
    t[ 2] += (sword128)0x2d944a725bf7a4cL * t[ 8];
    t[ 3] += (sword128)0x20cd77058eec490L * t[ 8];
    t[ 1] += (sword128)0x21cf5b5529eec34L * t[ 9];
    t[ 2] += (sword128)0x0f635c8e9c2ab70L * t[ 9];
    t[ 3] += (sword128)0x2d944a725bf7a4cL * t[ 9];
    t[ 4] += (sword128)0x20cd77058eec490L * t[ 9];
    t[ 2] += (sword128)0x21cf5b5529eec34L * t[10];
    t[ 3] += (sword128)0x0f635c8e9c2ab70L * t[10];
    t[ 4] += (sword128)0x2d944a725bf7a4cL * t[10];
    t[ 5] += (sword128)0x20cd77058eec490L * t[10];
    t[ 3] += (sword128)0x21cf5b5529eec34L * t[11];
    t[ 4] += (sword128)0x0f635c8e9c2ab70L * t[11];
    t[ 5] += (sword128)0x2d944a725bf7a4cL * t[11];
    t[ 6] += (sword128)0x20cd77058eec490L * t[11];
    /* Propagate carries */
    c = t[ 0] >> 56; t[ 1] += c; rd[ 0] = (sword64)(t[ 0] & 0xffffffffffffff);
    c = t[ 1] >> 56; t[ 2] += c; rd[ 1] = (sword64)(t[ 1] & 0xffffffffffffff);
    c = t[ 2] >> 56; t[ 3] += c; rd[ 2] = (sword64)(t[ 2] & 0xffffffffffffff);
    c = t[ 3] >> 56; t[ 4] += c; rd[ 3] = (sword64)(t[ 3] & 0xffffffffffffff);
    c = t[ 4] >> 56; t[ 5] += c; rd[ 4] = (sword64)(t[ 4] & 0xffffffffffffff);
    c = t[ 5] >> 56; t[ 6] += c; rd[ 5] = (sword64)(t[ 5] & 0xffffffffffffff);
    c = t[ 6] >> 56; t[ 7] += c; rd[ 6] = (sword64)(t[ 6] & 0xffffffffffffff);
    rd[ 7] = (word64)t[7];
    /* Mod bits over 56 in last word */
    o = rd[7] >> 54; rd[ 7] &= 0x3fffffffffffff;
    rd[ 0] += 0x873d6d54a7bb0dL * o;
    rd[ 1] += 0x3d8d723a70aadcL * o;
    rd[ 2] += 0xb65129c96fde93L * o;
    rd[ 3] += 0x8335dc163bb124L * o;
    /* Propagate carries */
    o = rd[ 0] >> 56; rd[ 1] += o; rd[ 0] = rd[ 0] & 0xffffffffffffff;
    o = rd[ 1] >> 56; rd[ 2] += o; rd[ 1] = rd[ 1] & 0xffffffffffffff;
    o = rd[ 2] >> 56; rd[ 3] += o; rd[ 2] = rd[ 2] & 0xffffffffffffff;
    o = rd[ 3] >> 56; rd[ 4] += o; rd[ 3] = rd[ 3] & 0xffffffffffffff;
    o = rd[ 4] >> 56; rd[ 5] += o; rd[ 4] = rd[ 4] & 0xffffffffffffff;
    o = rd[ 5] >> 56; rd[ 6] += o; rd[ 5] = rd[ 5] & 0xffffffffffffff;
    o = rd[ 6] >> 56; rd[ 7] += o; rd[ 6] = rd[ 6] & 0xffffffffffffff;

    /* Convert to bytes */
    r[ 0] = (byte)(rd[0 ] >>  0);
    r[ 1] = (byte)(rd[0 ] >>  8);
    r[ 2] = (byte)(rd[0 ] >> 16);
    r[ 3] = (byte)(rd[0 ] >> 24);
    r[ 4] = (byte)(rd[0 ] >> 32);
    r[ 5] = (byte)(rd[0 ] >> 40);
    r[ 6] = (byte)(rd[0 ] >> 48);
    r[ 7] = (byte)(rd[1 ] >>  0);
    r[ 8] = (byte)(rd[1 ] >>  8);
    r[ 9] = (byte)(rd[1 ] >> 16);
    r[10] = (byte)(rd[1 ] >> 24);
    r[11] = (byte)(rd[1 ] >> 32);
    r[12] = (byte)(rd[1 ] >> 40);
    r[13] = (byte)(rd[1 ] >> 48);
    r[14] = (byte)(rd[2 ] >>  0);
    r[15] = (byte)(rd[2 ] >>  8);
    r[16] = (byte)(rd[2 ] >> 16);
    r[17] = (byte)(rd[2 ] >> 24);
    r[18] = (byte)(rd[2 ] >> 32);
    r[19] = (byte)(rd[2 ] >> 40);
    r[20] = (byte)(rd[2 ] >> 48);
    r[21] = (byte)(rd[3 ] >>  0);
    r[22] = (byte)(rd[3 ] >>  8);
    r[23] = (byte)(rd[3 ] >> 16);
    r[24] = (byte)(rd[3 ] >> 24);
    r[25] = (byte)(rd[3 ] >> 32);
    r[26] = (byte)(rd[3 ] >> 40);
    r[27] = (byte)(rd[3 ] >> 48);
    r[28] = (byte)(rd[4 ] >>  0);
    r[29] = (byte)(rd[4 ] >>  8);
    r[30] = (byte)(rd[4 ] >> 16);
    r[31] = (byte)(rd[4 ] >> 24);
    r[32] = (byte)(rd[4 ] >> 32);
    r[33] = (byte)(rd[4 ] >> 40);
    r[34] = (byte)(rd[4 ] >> 48);
    r[35] = (byte)(rd[5 ] >>  0);
    r[36] = (byte)(rd[5 ] >>  8);
    r[37] = (byte)(rd[5 ] >> 16);
    r[38] = (byte)(rd[5 ] >> 24);
    r[39] = (byte)(rd[5 ] >> 32);
    r[40] = (byte)(rd[5 ] >> 40);
    r[41] = (byte)(rd[5 ] >> 48);
    r[42] = (byte)(rd[6 ] >>  0);
    r[43] = (byte)(rd[6 ] >>  8);
    r[44] = (byte)(rd[6 ] >> 16);
    r[45] = (byte)(rd[6 ] >> 24);
    r[46] = (byte)(rd[6 ] >> 32);
    r[47] = (byte)(rd[6 ] >> 40);
    r[48] = (byte)(rd[6 ] >> 48);
    r[49] = (byte)(rd[7 ] >>  0);
    r[50] = (byte)(rd[7 ] >>  8);
    r[51] = (byte)(rd[7 ] >> 16);
    r[52] = (byte)(rd[7 ] >> 24);
    r[53] = (byte)(rd[7 ] >> 32);
    r[54] = (byte)(rd[7 ] >> 40);
    r[55] = (byte)(rd[7 ] >> 48);
    r[56] = 0;
}

/* Precomputed multiples of the base point. */
static const ge448_precomp base[58][8] = {
{
    {
        { 0x26a82bc70cc05eL, 0x80e18b00938e26L, 0xf72ab66511433bL,
          0xa3d3a46412ae1aL, 0x0f1767ea6de324L, 0x36da9e14657047L,
          0xed221d15a622bfL, 0x4f1970c66bed0dL },
        { 0x08795bf230fa14L, 0x132c4ed7c8ad98L, 0x1ce67c39c4fdbdL,
          0x05a0c2d73ad3ffL, 0xa3984087789c1eL, 0xc7624bea73736cL,
          0x248876203756c9L, 0x693f46716eb6bcL }
    },
    {
        { 0x55555555555555L, 0x55555555555555L, 0x55555555555555L,
          0x55555555555555L, 0xaaaaaaaaaaaaa9L, 0xaaaaaaaaaaaaaaL,
          0xaaaaaaaaaaaaaaL, 0xaaaaaaaaaaaaaaL },
        { 0xeafbcdea9386edL, 0xb2bed1cda06bdaL, 0x833a2a3098bbbcL,
          0x8ad8c4b80d6565L, 0x884dd7b7e36d72L, 0xc2b0036ed7a035L,
          0x8db359d6205086L, 0xae05e9634ad704L }
    },
    {
        { 0x28173286ff2f8fL, 0xb769465da85757L, 0xf7f6271fd6e862L,
          0x4a3fcfe8daa9cbL, 0xda82c7e2ba077aL, 0x943332241b8b8cL,
          0x6455bd64316cb6L, 0x0865886b9108afL },
        { 0x22ac13588ed6fcL, 0x9a68fed02dafb8L, 0x1bdb6767f0bffaL,
          0xec4e1d58bb3a33L, 0x56c3b9fce43c82L, 0xa6449a4a8d9523L,
          0xf706cbda7ad43aL, 0xe005a8dbd5125cL }
    },
    {
        { 0xce42ac48ba7f30L, 0xe1798949e120e2L, 0xf1515dd8ba21aeL,
          0x70c74cc301b7bdL, 0x0891c693fda4beL, 0x29ea255a09cf4eL,
          0x2c1419a17226f9L, 0x49dcbc5c6c0cceL },
        { 0xe236f86de51839L, 0x44285d0d4f5b32L, 0x7ea1ca9472b5d4L,
          0x7b8a5bc1c0d8f9L, 0x57d845c90dc322L, 0x1b979cb7c02f04L,
          0x27164b33a5de02L, 0xd49077e4accde5L }
    },
    {
        { 0xa99d1092030034L, 0x2d8cefc6f950d0L, 0x7a920c3c96f07bL,
          0x958812808bc0d5L, 0x62ada756d761e8L, 0x0def80cbcf7285L,
          0x0e2ba7601eedb5L, 0x7a9f9335a48dcbL },
        { 0xb4731472f435ebL, 0x5512881f225443L, 0xee59d2b33c5840L,
          0xb698017127d7a4L, 0xb18fced86551f7L, 0x0ade260ca1823aL,
          0xd3b9109ce4fd58L, 0xadfd751a2517edL }
    },
    {
        { 0x7fd7652abef79cL, 0x6c20a07443a878L, 0x5c1840d12a7109L,
          0x4a06e4a876451cL, 0x3bed0b4ad95f65L, 0x25d2e673fb0260L,
          0x2e00349aebd971L, 0x54523e04498b72L },
        { 0xea5d1da07c7bccL, 0xcce776938ea98cL, 0x80284e861d2b3eL,
          0x48de76b6e1ff1bL, 0x7b121869c58522L, 0xbfd053a2765a1aL,
          0x2d743ec056c667L, 0x3f99b9cd8ab61cL }
    },
    {
        { 0xdf9567ceb5eaf7L, 0x110a6b478ac7d7L, 0x2d335014706e0bL,
          0x0df9c7b0b5a209L, 0xba4223d568e684L, 0xd78af2d8c3719bL,
          0x77467b9a5291b6L, 0x079748e5c89befL },
        { 0xe20d3fadac377fL, 0x34e866972b5c09L, 0xd8687a3c40bbb7L,
          0x7b3946fd2f84c9L, 0xd00e40ca78f50eL, 0xb87594417e7179L,
          0x9c7373bcb23583L, 0x7ddeda3c90fd69L }
    },
    {
        { 0x2538a67153bde0L, 0x223aca9406b696L, 0xf9080dc1ad713eL,
          0x6c4cb47d816a64L, 0xbc285685dc8b97L, 0xd97b037c08e2d7L,
          0x5b63fb45d0e66bL, 0xd1f1bc5520e8a3L },
        { 0x4eb873ce69e09bL, 0x1663164bc8ee45L, 0x08f7003ba8d89fL,
          0x4b98ead386ad82L, 0xa4b93b7bd94c7bL, 0x46ba408c6b38b3L,
          0xdae87d1f3574ffL, 0xc7564f4e9bea9bL }
    },
},
{
    {
        { 0x2e4fdb25bfac1cL, 0xf0d79aaf5f3bcaL, 0xe756b0d20fb7ccL,
          0xe3696beb39609aL, 0xa019fc35a5ab58L, 0xa2b24853b281ddL,
          0xe3e2be761ac0a2L, 0xf19c34feb56730L },
        { 0x2d25ce8a30241eL, 0xf5661eab73d7a1L, 0x4611ed0daac9f4L,
          0xd5442344ced72cL, 0xce78f52e92e985L, 0x6fe5dd44da4aadL,
          0xfcaddc61d363ceL, 0x3beb69cc9111bfL }
    },
    {
        { 0xd2e7660940ebc9L, 0xe032018b17bbe0L, 0xad4939175c0575L,
          0xdd0b14721c7f34L, 0x52c2ba43e147e0L, 0x7dd03c60ee8973L,
          0x5472e8decf2754L, 0x17a1cd1d6482bbL },
        { 0xdd43b848128b3fL, 0xf0cae34ea7dd25L, 0x81ca99fff07df2L,
          0x1c8959792ebbdcL, 0x45c7a6872155e6L, 0x907a50e39ddd08L,
          0xbe398c2bb2d89bL, 0x38063f91b3b536L }
    },
    {
        { 0x149fafbf843b23L, 0x00ab582ac7f22aL, 0xa3b981bf2f4d4cL,
          0x2ce1a654341a22L, 0x68a40747c03b63L, 0x63206a212f2cf8L,
          0xc9961d35149741L, 0xfb85430bc7099eL },
        { 0x9c9107290a9e59L, 0x734e94a06de367L, 0x5cf3cbedb99214L,
          0xc6bce3245b1fb9L, 0x1a82abedd7be0dL, 0xf74976aede7d1cL,
          0x7025b7c21503bdL, 0xf7894910d096abL }
    },
    {
        { 0x6bd48bb555a41bL, 0xfbdd0d067de206L, 0x98bc477dd6dfd1L,
          0x1d0693b3e40b8aL, 0x6e15563da32ae4L, 0x0194a20fcebaa2L,
          0xda116150980a93L, 0x8e119200109cecL },
        { 0x8ea0552ffb9726L, 0xeba50a4047e44bL, 0xc050d2460ddf76L,
          0xe009204ac690e0L, 0x47b86399b18edcL, 0x2f5b76ac77f23fL,
          0x4296c240792905L, 0x73f6b4a06f6dc7L }
    },
    {
        { 0xb6ef9ea3b10cadL, 0x312843df7c8fceL, 0x5bdcd528bedf86L,
          0x2889059f6dd823L, 0x04578e908bfde0L, 0x3245df3123e2e5L,
          0xbf461d57ee9e3aL, 0xddec2d46f94cebL },
        { 0x21b43b9145768fL, 0xe79a8f9dae962aL, 0xff1972bcbb043fL,
          0xe3dcf6d239649bL, 0xed592bdc533b85L, 0x14ff94fdbe22d0L,
          0x6c4eb87f1d8e22L, 0xd8d4c71d18cf6dL }
    },
    {
        { 0xcda666c8d96345L, 0x9ecaa25836cd21L, 0x6e885bd984606eL,
          0x1dd5fef804f054L, 0x9dfff6b6959ae4L, 0x99b9cf8c9b55ccL,
          0xb4716b062b9b80L, 0x13ec87c554b128L },
        { 0xe696d1f75aacc2L, 0xf78c99387fc5ffL, 0x76c09473809d42L,
          0x99ce62db618fa8L, 0x35e3e022f53341L, 0x62fc1ac0db6c5eL,
          0xa1fb8e600d8b47L, 0x0bc107058f0d1eL }
    },
    {
        { 0x1f4526916da513L, 0x1f2fc04f5cf341L, 0xae9208664d23e0L,
          0x4e33082da8a113L, 0x2688ec61cfc085L, 0x6f2e8de6e5327fL,
          0x2070db3b4e48a8L, 0xd6626973240adeL },
        { 0xa6b317ffbd997bL, 0x9fa1b5649e26bdL, 0xcbf0d258cba0f3L,
          0x4a7791b17b4745L, 0x25f555b5c9e190L, 0x7cd3940923ec4cL,
          0x16f4c6ae98f1b6L, 0x7962116bcd4e0fL }
    },
    {
        { 0x8d58fa302491e3L, 0x7cf76c67ab3898L, 0xbc2f657647ebc7L,
          0x5f4bfe0d25f5a3L, 0x503f478d69505dL, 0x4a889fc3fb6645L,
          0x33e1bc1fa86b18L, 0xabb234f5508dd8L },
        { 0x5348e1b9a05b48L, 0x57ac5f164dc858L, 0x21f4d38ec8a2d3L,
          0x5ec6d3ca3a3e9dL, 0xcd4062e560a0b8L, 0x49b74f73433f59L,
          0xefd9d87cab14e3L, 0x858ce7feb964f5L }
    },
},
{
    {
        { 0x7577254eb731b4L, 0x9fff1fb4e2397eL, 0x749b145c821715L,
          0x40619fe2e65e67L, 0x57b82812e618d8L, 0x063186c707b83eL,
          0xcfc80cb31b24a2L, 0xcca6185ac75169L },
        { 0x6539f44b255818L, 0x5895da00368bceL, 0x841a30917c7482L,
          0x85469e1b1a9c9eL, 0x05664c0e4f7d9dL, 0x8a063187b35cc0L,
          0x214763aa0e9b0aL, 0x1bd872c4b26ac2L }
    },
    {
        { 0x3578f97a93762bL, 0x434f69a72d52bcL, 0xddcca4022cb565L,
          0xa7d1e41ff20544L, 0x823475d8a66588L, 0x9fc97c799d7bafL,
          0x15542f1660e421L, 0xa7d1f60843faf6L },
        { 0xbbfaab54063cccL, 0x3ad9bada49855aL, 0xffd5f1c5bddbfeL,
          0x0e419c2ae87e59L, 0xdce6ed6f89956bL, 0xf047c21ccd8951L,
          0x6ed4a1ba83c991L, 0x85af86e2d28e0aL }
    },
    {
        { 0x04433c49ed48a8L, 0xeffa8580bc375dL, 0xfb0e1b2fa6e3b5L,
          0x51483a2a1aaddaL, 0x733448df8b2ea8L, 0xaa0513cf639f0cL,
          0x6bc61a3a23bf84L, 0x3e64f68dc2430dL },
        { 0x51bf502c5876b1L, 0x6b833751c0dd2aL, 0xe597be1342914fL,
          0x43d5ab0f8e632cL, 0x2696715d62587bL, 0xe87d20aed34f24L,
          0x25b7e14e18baf7L, 0xf5eb753e22e084L }
    },
    {
        { 0x51da71724d8295L, 0xd478e4318d1340L, 0xacf94f42cf7f66L,
          0x230d7d13760711L, 0x078a66a5abc626L, 0xd78b0bd6b5f6daL,
          0x23a971396d1d0bL, 0x87623d64bd960fL },
        { 0x0841a9977db53fL, 0x23c1a53f4d03eeL, 0x2f62c2e1f95df1L,
          0xd1e2ec1116f4e7L, 0x896d2fe34811a9L, 0xad65e2bec8096eL,
          0x09d36f9b1744a6L, 0x564bac7ff5ddf7L }
    },
    {
        { 0x48b41e2c3f77cbL, 0x52276730968938L, 0xff1b899fd9b452L,
          0x67cf3bf2e03908L, 0x3731d90248a6fbL, 0xd800a05256598fL,
          0x347d2f2bdc8530L, 0xc72a3007ad08a1L },
        { 0x5e5be741d65f73L, 0x183d4ae4206eadL, 0xcb50c1cade4013L,
          0x39db43d3102483L, 0x0eb49fa70d6325L, 0xa18f6a2c1f02b9L,
          0x3e6fe30dbf5e66L, 0xac4eeb93a82aa5L }
    },
    {
        { 0x295affd3613d47L, 0x7b7e68ab56f343L, 0x980629692b173bL,
          0x937061ebad35fbL, 0x25019785c21eeaL, 0xe92721b787a746L,
          0x463c46c3651631L, 0x6da4b5dc6f2d5aL },
        { 0xcb67cc16e6d18cL, 0x1b30d520010588L, 0x1bb6ea6db1d1e8L,
          0x9c6308aad11474L, 0xc3167413d19b1cL, 0xf2e84d7be4fb79L,
          0xeccb873e050f77L, 0xf7c8d80cc2bf86L }
    },
    {
        { 0x16fe2e17ab20e5L, 0x274deadecf3a92L, 0x9f434870972f67L,
          0x9a65a454605751L, 0x9351f07b8980b2L, 0x412962b0eb08a5L,
          0xb8c9bfd733f440L, 0xac2cd641ca250fL },
        { 0x68cdd0f2ba7d26L, 0xd3d2a4a4e0beeaL, 0x50135c19f4a258L,
          0xb475e53f0d02e4L, 0x432d8c6589283aL, 0x29141bfa0a2b6cL,
          0xd7379ec13704bcL, 0x831562c52459bfL }
    },
    {
        { 0x676b366eeec506L, 0xdd6cad545da557L, 0x9de39cb77057d2L,
          0x388c5fedf05bf1L, 0x6e55650dfb1f03L, 0xdbceffa52126c9L,
          0xe4d187b3a4a220L, 0xac914f9eb27020L },
        { 0x3f4ab98d2e5f30L, 0x6ae97dadd94451L, 0x64af6950d80981L,
          0x36b4b90f2aa2ceL, 0x6adcd7a18fcf59L, 0x3ddfe6dc116c81L,
          0x661072b549b9e3L, 0xd9e3134ec4584dL }
    },
},
{
    {
        { 0x6e46707a1e400cL, 0xcdc990b551e806L, 0xfa512513a07724L,
          0x500553f1b3e4f5L, 0x67e8b58ef4dac3L, 0x958349f2cb4cc7L,
          0x948b4ed7f9143cL, 0xe646d092b7822bL },
        { 0xd185dd52bc3c26L, 0x34ba16ec837fc9L, 0x516d4ba5a788b7L,
          0x72f2de756142b0L, 0x5846f61f445b3dL, 0xdaec5c9f4631a1L,
          0xa10b18d169ea9bL, 0x85d2998af6751bL }
    },
    {
        { 0xda0cac443ddf31L, 0x0966e171860911L, 0x9c3a7173cba600L,
          0x5781880571f895L, 0x5e2a927737ac21L, 0x8a461486c253fbL,
          0xe801cf595ee626L, 0x271166a5f84fc0L },
        { 0x306937fba856bdL, 0x80cb179be80a43L, 0x70393b2ffb5980L,
          0xa8e4a1c660fc64L, 0x5078abfc0d5c98L, 0x62ba530fbd31ffL,
          0xda608449e51b88L, 0xdb6ecb0355ae15L }
    },
    {
        { 0xbcbb6ea23c5d49L, 0x08906ba87959bcL, 0x61cc0880991665L,
          0x21d6b41d90d13cL, 0x0c27ac1d03afe9L, 0x159995f5cfea52L,
          0x4057e20bdfe220L, 0xdd1b349cbdf058L },
        { 0x0cd66262e37159L, 0x8cea8e43eb0d17L, 0x553af085bce7f0L,
          0xb94cb5f5b6511dL, 0x7b8d3a550e0330L, 0x415911057ab7e7L,
          0x320820e6aa886fL, 0x130d4d6c5b6b81L }
    },
    {
        { 0x2f98059c7bb2edL, 0x33ebf4ca49bdfbL, 0x04c72a1b0a675bL,
          0x94f9ea4adb6c14L, 0x03376d8cf728c0L, 0x5c059d34c6eb6aL,
          0x0178408eb8da48L, 0x8bf607b2956817L },
        { 0x7ad2822ceb3d28L, 0xd07a40337ae653L, 0xbc68739c1e46b2L,
          0x15d7cca9154ba9L, 0x6b97103a26617dL, 0xa610314b2e0d28L,
          0x52a08bafd4d363L, 0x80c2638c7dc2afL }
    },
    {
        { 0x0cde7ef3187140L, 0x93b92ca4b70acdL, 0x5696e507a79cdcL,
          0x73cc9728eaab66L, 0x6b8c5b68f1b0c7L, 0xb39a3184f7e0b1L,
          0x72cfb0d376108aL, 0x0c53efc98536a7L },
        { 0x03b52a824c2f1eL, 0x717132e6399b78L, 0x31ebd25349a85dL,
          0x265ee811a200d4L, 0x0b1aad2407d7adL, 0x9a9ebc894d2962L,
          0x994e6cd41171d9L, 0x09178d86c8fa83L }
    },
    {
        { 0x7d1d238a2593a1L, 0x863e93ab38fb19L, 0xd23a4cce7712a9L,
          0x7477b1327efcd5L, 0x3ba69ff1392f6cL, 0x63e0c32f7bb5a5L,
          0x20412c0026effdL, 0xd3ee8e4ef424abL },
        { 0x14c0b2d64e5174L, 0x2a611f2e58c47bL, 0xaa58a06c1e8635L,
          0x1870c3ecf17034L, 0xb0d5e3483f1bf3L, 0xb19905c16c7eb3L,
          0xbf85d626efa4caL, 0xfd16b2f180f92bL }
    },
    {
        { 0xc0431af3adcb48L, 0xc9a7a8dba90496L, 0xd765a163895294L,
          0xb02a41a551de70L, 0xb71b261749b8a1L, 0x0dfa89ec6f3e47L,
          0x392c0d80f5d9ceL, 0x43c59d831aee3cL },
        { 0x94bfb6d4d76f49L, 0xe8f5b8227d68a5L, 0x78ae1d9630fd08L,
          0x1379029ce1bdaeL, 0x9689da066715dcL, 0x5d4cb24d3278c7L,
          0x77c98339e84fbcL, 0xc8478dcea1048cL }
    },
    {
        { 0xe4b8f31770d2baL, 0x744f65242ea095L, 0xd06e090036f138L,
          0xd3a3d5b3b078caL, 0xc7ae54178b8417L, 0xad6c5d4c738fd7L,
          0x61789844676454L, 0xfbf34235d9a392L },
        { 0x8e451a7fff772fL, 0x8605bb75ffbeadL, 0x6f75cc1930d59fL,
          0xd4f47558f3f460L, 0xefd2d796700c8aL, 0xceb462a2406421L,
          0x8ed0f979dfe8f1L, 0x0280bf1d1d7600L }
    },
},
{
    {
        { 0x761c219dd9a54dL, 0x1127fcb86a39c0L, 0x7d0e4f04c9beddL,
          0x27c017a4d976b6L, 0x800c973da042cfL, 0xe7419af2593f11L,
          0xbd49448ae67960L, 0xd3b60b7744fd85L },
        { 0x5e74ed961676feL, 0x7383ef339af627L, 0x34407e05e62df7L,
          0xb0534618bf3196L, 0xd6b7184583b407L, 0xe3d068555011beL,
          0x94083d02124b52L, 0xa908324f780aafL }
    },
    {
        { 0xb27af1a73ec9c3L, 0xb66ad9f70fa725L, 0x07724f58cf73e4L,
          0xc3fcd579949358L, 0x06efb79da0cc01L, 0x1e977d210597c9L,
          0xcd732be703e8d6L, 0x6fd29bf6d0b69eL },
        { 0xca658ac667128eL, 0xca0036ac7872b3L, 0xc9698585355837L,
          0x59f3be8075cf1cL, 0x9f1b9b03809a11L, 0x6881ced9733871L,
          0x8cda0fbe902a5fL, 0x4d8c69b4e3871eL }
    },
    {
        { 0x5c3bd07ddee82fL, 0xe52dd312f9723bL, 0xcf8761174f1be8L,
          0xd9ecbd835f8657L, 0x4f77393fbfea17L, 0xec9579fd78fe2cL,
          0x320de920fb0450L, 0xbfc9b8d95d9c47L },
        { 0x818bd425e1b4c3L, 0x0e0c41c40e2c78L, 0x0f7ce9abccb0d0L,
          0xc7e9fa45ef81fbL, 0x2561d6f73574adL, 0xa2d8d99d2efb0bL,
          0xcf8f316e96cd0aL, 0x088f0f14964807L }
    },
    {
        { 0x0a8498945d5a19L, 0x47ab39c6c2131fL, 0x5c02824f3fc35dL,
          0x3be77c89ee8127L, 0xa8491b7c90b80aL, 0x5397631a28aa93L,
          0x54d6e816c0b344L, 0x22878be876d0e4L },
        { 0xeecb8a46db3bf6L, 0x340f29554577a3L, 0xa7798689a00f85L,
          0x98465d74bb9147L, 0x9532d7dda3c736L, 0x6d574f17504b20L,
          0x6e356f4d86e435L, 0x70c2e8d4533887L }
    },
    {
        { 0xdce5a0ad293980L, 0x32d7210069010eL, 0x64af59f06deaaaL,
          0xd6b43c459239e4L, 0x74bf2559199c29L, 0x3efff4111e1e2bL,
          0x1aa7b5ecb0f8d8L, 0x9baa22b989e395L },
        { 0xf78db807b33ac1L, 0x05a3b4354ce80aL, 0x371defc7bc8e12L,
          0x63305a01224610L, 0x028b1ae6d697efL, 0x7aba39c1cd8051L,
          0x76ed7a928ee4b4L, 0x31bd02a7f99901L }
    },
    {
        { 0xf9dab7af075566L, 0x84e29a5f56f18bL, 0x3a4c45af64e56dL,
          0xcf3644a6a7302dL, 0xfb40808156b658L, 0xf33ef9cf96be52L,
          0xfe92038caa2f08L, 0xcfaf2e3b261894L },
        { 0xf2a0dbc224ce3fL, 0xed05009592eb27L, 0x501743f95889d0L,
          0xa88a47877c95c2L, 0x86755fbdd63da9L, 0x9024acfc7ee828L,
          0x634b020f38113bL, 0x3c5aacc6056e64L }
    },
    {
        { 0xe03ff3aa2ef760L, 0x3b95767b1c3bacL, 0x51ce6aa940d754L,
          0x7cbac3f47a9a3dL, 0xa864ac434f8d1aL, 0x1eff3f280dbd47L,
          0xd8ab6607ebd5caL, 0xc4df5c405b07edL },
        { 0x3dc92dfa4f095bL, 0x5ae36a57cdbd9aL, 0x7ff29737891e04L,
          0x37c03130a5fe7bL, 0x210d7b0aa6e35eL, 0x6edfb53bf200d8L,
          0x787b68d84afb85L, 0x9b5c49b72c6de3L }
    },
    {
        { 0x51857164010f4eL, 0xe0b144b0536ebeL, 0xacabb14887d663L,
          0xac1caededf584fL, 0xb43fb8faf175a3L, 0x310b6d5f992a3cL,
          0xf2c4aa285178a4L, 0x69c99698bd56bfL },
        { 0x73d6372a4d972eL, 0x3d5bb2e9583803L, 0x7bf7d18d891581L,
          0xa5ce5d7568a34aL, 0x670b4331f45c81L, 0x97265a71f96910L,
          0xdb14eb3b07c1eaL, 0xdf008eafed447cL }
    },
},
{
    {
        { 0x0379f5a00c2f10L, 0xb320b4fd350285L, 0x74e560e8efdd7dL,
          0xf2f017ef46a140L, 0x2ced1a60f34624L, 0x7c4b4e3ca08ec9L,
          0xdffc2a15d8bc6bL, 0xcc8f3f3527b007L },
        { 0x59f8ac4861fe83L, 0x8d48d2cd03144cL, 0xa8457d2bfa6dceL,
          0xd7ed333677c136L, 0xcb8e219c228e18L, 0x5f70bc916ab1e4L,
          0x2ae3a3d3780370L, 0x9f3365488f17adL }
    },
    {
        { 0xeab0710960e4bbL, 0xc668a78ab9cfd3L, 0x2e85553b0ef946L,
          0xa43c4b98df5df3L, 0x0ecd5593cb3646L, 0x6f543c418dbe71L,
          0xee7edaaf59818bL, 0xc44e8d290911c1L },
        { 0xafb38b1269b509L, 0x9e2737c52afe2cL, 0x5b2ef02ccfa664L,
          0x1e0aeace1cc58bL, 0x37a57e95ea134eL, 0xc9c465a83b9fc2L,
          0x4b9e8c76e3eccaL, 0xca07dbe9bdbab5L }
    },
    {
        { 0xd297f3cb0d7807L, 0xee441a5f59ce61L, 0x728553bb2db844L,
          0x90f87e5640e9e0L, 0xaa72cbfcb76dffL, 0x065c6864012d57L,
          0xd5ee88f9678b44L, 0x3d74b852177603L },
        { 0x3f9c947748b68eL, 0x03856d98f44d44L, 0xde34b84462426cL,
          0xc16d1bb845ab29L, 0x9df6217d2e18deL, 0xec6d219b154643L,
          0x22a8ec32ee0f8fL, 0x632ad3891c5175L }
    },
    {
        { 0x19d9d236869267L, 0x628df94fe5532aL, 0x458d76c6dc9a01L,
          0x405fe6c2cc39c8L, 0x7dddc67f3a04baL, 0xfee630312500c7L,
          0x580b6f0a50e9deL, 0xfb5918a6090604L },
        { 0xd7159253af6b2dL, 0x83d62d61c7d1ecL, 0x94398c185858c4L,
          0x94643dc14bfb64L, 0x758fa38af7db80L, 0xe2d7d93a8a1557L,
          0xa569e853562af1L, 0xd226bdd84346aaL }
    },
    {
        { 0xc2d0a5ed0ccd20L, 0xeb9adb85dbc0cfL, 0xe0a29ee26d7e88L,
          0x8bb39f884a8e98L, 0x511f1c137396eaL, 0xbc9ec5ac8b2fb3L,
          0x299d81c090e5bcL, 0xe1dfe344cdd587L },
        { 0x80f61f45e465b7L, 0x5699c531bad59eL, 0x85e92e4b79ff92L,
          0x1e64fce9db244cL, 0x3748574a22097dL, 0xe2aa6b9efff24eL,
          0xb951be70a10bc6L, 0x66853269067a1cL }
    },
    {
        { 0xf716ddfa6114d3L, 0x9e515f5037ec1fL, 0x773454144944a6L,
          0x1540c4caba97ccL, 0xe41e5488b54bb7L, 0x4363156cae37bcL,
          0xc384eaff3d2ce8L, 0x72a4f454c58ba4L },
        { 0x0ceb530dcaf3fcL, 0x72d536578dcdbbL, 0x9b44084c6320faL,
          0x6262d34eb74c70L, 0x8abac85608e6dcL, 0x82a526410dd38dL,
          0xbc39911a819b8dL, 0xbda15fe03ad0d9L }
    },
    {
        { 0xadbf587f9dc60bL, 0xf9d814f7d846d2L, 0xccdd241b77bde0L,
          0x89cb6d72242f50L, 0x95c0e3ee6360a8L, 0x7c7dd5adf49713L,
          0x68e0e4957d5814L, 0x3aa097d0c16571L },
        { 0xb56b672267d03aL, 0x4f557088c44af4L, 0x67c49e7f3252a5L,
          0x871d6cfc94a469L, 0x57ae99801fbfaaL, 0x5c0e48f48a5d8eL,
          0xe9bf9c85e240b9L, 0xa41018999d41caL }
    },
    {
        { 0x6beb0c7b2889b4L, 0x78b7f899455370L, 0xd43421447ca364L,
          0xdd9d2da9f21e5bL, 0xa0c7c180a7e4aaL, 0x022c0d4da1660cL,
          0xe1f5c165a57002L, 0x51c7c9e518f68fL },
        { 0x6d521b62586502L, 0xa0f2cb3183ec1bL, 0x578b4e0caa5e16L,
          0x7bd4fbd764997fL, 0x7ec56c364b1804L, 0xb75a2540ee08e4L,
          0x6bf74a6dc19080L, 0x6ec793d97d6e59L }
    },
},
{
    {
        { 0x16789d60a4beb9L, 0x512b2cd9b9c801L, 0xf8b6d108c7bb9cL,
          0xd85651e9ebdc8cL, 0xc9450829ba971aL, 0x852d9ea7e1cf78L,
          0x6a45e350af01e2L, 0xe6cdadf6151dcfL },
        { 0xc454bb42b8c01bL, 0x59e0c493d54cd2L, 0x8e1e686454d608L,
          0x0dbae4bd8c6103L, 0xa5603a16c18b18L, 0x227a6b23369093L,
          0xf1e89295f3de1cL, 0x42f0b588ab63c5L }
    },
    {
        { 0xf1974cc5b596d8L, 0xee8093f44719f0L, 0x40ba933f6f5b54L,
          0xd6e53652f3d654L, 0x9aeb83526d73b8L, 0x50ed5350776382L,
          0x3be47d6ad43875L, 0x21d56dfc786e48L },
        { 0x8a75e18b73bb39L, 0x9eba84cf265a78L, 0x7c02a4d2e772e7L,
          0xf7df6d44c1ecd2L, 0xa8d9ea06cef71bL, 0x86e8f91cae3b68L,
          0x2fd141199efefaL, 0x0b36ab2214e6f6L }
    },
    {
        { 0xd79065cbdce61cL, 0xcb562ffdecb229L, 0xef5d3d14600849L,
          0x348b31b1d23ac8L, 0xb2ea69915c36b8L, 0x268683d4822836L,
          0x083edbec6f0b7dL, 0xaf4f39d1a7821cL },
        { 0x23be6e84e64841L, 0xe9e246365bf791L, 0xa3208ac02bfd7cL,
          0x231989cd01357dL, 0x79b8aad6422ab4L, 0x57d2b7e91b8564L,
          0x28ebbcc8c04421L, 0xdc787d87d09c05L }
    },
    {
        { 0xeb99f626c7bed5L, 0x326b15f39cd0e8L, 0xd9d53dcd860615L,
          0xdf636e71bf4205L, 0x1eaa0bf0752209L, 0x17ce69a4744abbL,
          0x474572df3ea2fbL, 0xc4f6f73224a7f3L },
        { 0x7ed86ad63081b4L, 0xcd4cdc74a20afbL, 0x7563831b301b2eL,
          0x5b4d2b1e038699L, 0xa15d1fa802a15fL, 0x6687aaf13e9172L,
          0x3eccd36ba6da90L, 0x34e829d7474e83L }
    },
    {
        { 0x4cea19b19c9b27L, 0xa14c37a5f52523L, 0x248b16d726625cL,
          0x8c40f9f6cabc21L, 0x918470c32a5c65L, 0x314056b2a98d5bL,
          0x6c974cf34a0714L, 0x0c8f8a94f6314aL },
        { 0x484455770bccfdL, 0xf5835db740c9fdL, 0x12e59b5a21407cL,
          0xbe338e0db1689dL, 0x5a50ce9dd5e915L, 0xb1780e9ef99f39L,
          0x1262b55ee4d833L, 0x4be3f2289c5340L }
    },
    {
        { 0xbb99b906c4b858L, 0xa7724d1550ca53L, 0x7d31f5a826962eL,
          0xf239322a5804daL, 0x3e113200275048L, 0xcbb1bb83ee4cb6L,
          0xdb865251331191L, 0xb7caf9e7d1d903L },
        { 0x06e3b0577d7a9dL, 0x7a132b0b3bbbf5L, 0xd61fbc57c50575L,
          0x393f712af4b646L, 0xef77972cb7efe9L, 0x20e6d5d5ea4995L,
          0x0ac23d4fbbe4c6L, 0x8456617c807f2aL }
    },
    {
        { 0x4995fb35396143L, 0xa8b4bd1b99dc46L, 0x2293e8e4150064L,
          0x2f77d4922a3545L, 0xe866b03b2192c4L, 0x58b01f05e0aa38L,
          0xe406b232ed246bL, 0x447edb3ed60974L },
        { 0xf541b338869703L, 0x6959fe0383420aL, 0xd6b39db4be4e48L,
          0x048f3b4b5714efL, 0x68b49685d9e4b8L, 0xbda8e6c2177963L,
          0x5094e35c4211feL, 0xea591c32d46d1aL }
    },
    {
        { 0x3a768ff2fef780L, 0x4218d2832970c6L, 0xce598e4ec6da17L,
          0xf675645fbb126aL, 0xb04c23f0427617L, 0xc9f93fbe4fce74L,
          0x44a414b3c91b00L, 0x4d982f31d3b3ccL },
        { 0xb1d40e8b24cce0L, 0x5a21c07133e73dL, 0x6e9358e0bb589dL,
          0x39cfb172399844L, 0x83f7647166080eL, 0xcfe7bf8450b468L,
          0x2a288f71e8434fL, 0xd39f1e521a81e3L }
    },
},
{
    {
        { 0x78c6f13528af6fL, 0x0001fe294b74d9L, 0xae7742501aab44L,
          0x7cbe937ef0039cL, 0xaf3e4f00fa2a67L, 0xe28175fda1378eL,
          0x72adeed8ccd90eL, 0x16a8ce100af22fL },
        { 0x69fae17cbf63ddL, 0x67861729e39e26L, 0xe92b3d5f827a18L,
          0x4d75e418403682L, 0x01a4fd99056a79L, 0x89efb2d20008f5L,
          0xa2f6918b78ff15L, 0xf41c870a3437f5L }
    },
    {
        { 0xc840ae57be353cL, 0x465a5eb3fb2691L, 0x34a89f07eba833L,
          0xf620896013346eL, 0x563b5f0e875df2L, 0x5f7fc8bfbc44ceL,
          0x22fcb5acfedf9dL, 0x7cf68d47dc691bL },
        { 0x37f7c2d76a103fL, 0x728a128fd87b7dL, 0x7db2ad8ccf2132L,
          0xa4c13feb100e63L, 0xcd28a517b511d5L, 0xb910280721ca5cL,
          0xec1305fd84bd52L, 0xb9646422729791L }
    },
    {
        { 0x83fccdf5bc7462L, 0x01f3ddad6f012fL, 0x57f11713a6a87cL,
          0xedb47ceff403acL, 0x6c184e5baab073L, 0x5b17c7d6f0d6a1L,
          0x45a4c4f3ef2c91L, 0x26c3f7e86a8f41L },
        { 0x81a6db0b646514L, 0xf84059fca8b9aeL, 0xd73dab69f02305L,
          0x0de3faec4b7c6cL, 0x18abb88696df2fL, 0x45dd1b975d7740L,
          0x3aeccc69ee35bcL, 0x478252eb029f88L }
    },
    {
        { 0x66bf85b8b2ce15L, 0x1175425335709dL, 0x00169ef8123874L,
          0xfd3c18c9b89868L, 0xb3612f9775204eL, 0x4b8d09dc2cd510L,
          0xafa12e614559adL, 0x1ddaa889657493L },
        { 0x87d700b1e77a08L, 0xaf4cf2f14d2e71L, 0xe00835dbf90c94L,
          0xb16a6ec6dc8429L, 0x02a7210f8a4d92L, 0x5a5ab403d0c48dL,
          0x0052b3ab5b9beaL, 0x6242739e138f89L }
    },
    {
        { 0x7c215d316b2819L, 0xdacb65efeb9d7aL, 0xc3c569ed833423L,
          0xbc08435886a058L, 0x132c4db7e5cb61L, 0x6373a279422affL,
          0x43b9d7efca9fc4L, 0xe3319a5dbe465fL },
        { 0x51d36870b39da7L, 0xcb6d7984b75492L, 0x77eb272eadd87aL,
          0xf2fb47de0d3f6cL, 0x807fd86f9f791cL, 0xf01086b975e885L,
          0xf9314b5b6a3604L, 0x8cd453867be852L }
    },
    {
        { 0x7c1e6b3858f79bL, 0xf0477c4938caf9L, 0xb311bbf3e88c44L,
          0x9234c091e3a3c1L, 0x531af2b95a1d4dL, 0xf3cc969b8d1c64L,
          0x6f3c328b51e78dL, 0x5a1bd6c34e8881L },
        { 0x2e312393a9336fL, 0x020f0cc5ced897L, 0x4b45d7b5fab121L,
          0x8068b1c1841210L, 0x1bd85fc8349170L, 0xfe816d80f97fe5L,
          0x108981814b84fcL, 0x1d4fabbb93cd48L }
    },
    {
        { 0x1f11d45aef599eL, 0x8d91243b09c58aL, 0xd2eec7bd08c3c3L,
          0x5a6039b3b02793L, 0xb27fed58fb2c00L, 0xb5de44de8acf5eL,
          0x2c3e0cd6e6c698L, 0x2f96ed4777180dL },
        { 0x67de8bf96d0e36L, 0xd36a2b6c9b6d65L, 0x8df5d37637d59cL,
          0x951899fc8d9878L, 0x0fa090db13fcf8L, 0xa5270811f5c7b4L,
          0x56a6560513a37aL, 0xc6f553014dc1feL }
    },
    {
        { 0x7f6def794945d6L, 0x2f52fe38cc8832L, 0x0228ad9a812ff5L,
          0xcd282e5bb8478aL, 0xa0bc9afbe91b07L, 0x0360cdc11165e2L,
          0xb5240fd7b857e4L, 0x67f1665fa36b08L },
        { 0x84ce588ad2c93fL, 0x94db722e8ff4c0L, 0xad2edbb489c8a3L,
          0x6b2d5b87e5f278L, 0x0265e58d1d0798L, 0xd2c9f264c5589eL,
          0xde81f094e4074dL, 0xc539595303089fL }
    },
},
{
    {
        { 0x183492f83e882cL, 0x4d58203b5e6c12L, 0x1ac96c3efec20bL,
          0xabd5a5be1cd15eL, 0x7e1e242cbbb14bL, 0x9f03f45d0543b3L,
          0xc94bc47d678158L, 0x7917be0a446cadL },
        { 0x53f2be29b37394L, 0x0cb0a6c064cc76L, 0x3a857bcfba3da3L,
          0xac86bc580fcb49L, 0x9d5336e30ab146L, 0xafb093d5bc1270L,
          0x996689de5c3b6eL, 0x55189faea076baL }
    },
    {
        { 0x99ef986646ce03L, 0xa155f8130e6100L, 0x75bef1729b6b07L,
          0xc46f08e1de077bL, 0xf52fdc57ed0526L, 0xe09d98961a299aL,
          0x95273297b8e93aL, 0x11255b50acd185L },
        { 0x57919db4a6acddL, 0x708a5784451d74L, 0x5b0bd01283f7b3L,
          0xe82f40cc3d9260L, 0x2ab96ec82bbdc2L, 0x921f680c164d87L,
          0xf0f7883c17a6a9L, 0xc366478382a001L }
    },
    {
        { 0x5c9aa072e40791L, 0xf0b72d6a0776bfL, 0x445f9b2eaa50dcL,
          0xa929fa96bda47fL, 0x539dc713bbfc49L, 0x4f16dd0006a78bL,
          0x331ba3deef39c7L, 0xbfa0a24c34157cL },
        { 0x0220beb6a3b482L, 0x3164d4d6c43885L, 0xa03bb5dacdea23L,
          0xd6b8b5a9d8f450L, 0xd218e65bd208feL, 0x43948ed35c476fL,
          0x29a0dd80a2ed2bL, 0xa6ccf3325295b7L }
    },
    {
        { 0xf68f15fac38939L, 0xb3dd5a2f8010c1L, 0xf7ac290a35f141L,
          0xdc8f3b27388574L, 0x7ec3de1e95fed2L, 0xc625451257ac7dL,
          0x66fc33e664e55aL, 0xd3968d34832ba5L },
        { 0x980291bc026448L, 0xfcb212524da4a5L, 0xbca7df4827a360L,
          0xfcc395c85ca63bL, 0xcf566ec8e9f733L, 0x835ee9bd465f70L,
          0xe66d111372f916L, 0xc066cf904d9211L }
    },
    {
        { 0xb9763a38b48818L, 0xa6d23cc4288f96L, 0xe27fcf5ed3a229L,
          0x6aebf9cabaff00L, 0xf3375038131cd1L, 0x13ad41dffabd58L,
          0x1bee6af861c83bL, 0x274fe969c142e7L },
        { 0x70ebcc99b84b5bL, 0xe1a57d78191cfcL, 0x46ccd06cbf00b8L,
          0xc233e8eefe402dL, 0xb4ab215beebeb3L, 0xb7424eabd14e7bL,
          0x351259aa679578L, 0x6d6d01e471d684L }
    },
    {
        { 0x755c465815ae38L, 0xadc3e85611db56L, 0x633999b188dd50L,
          0xfdf7509c12d907L, 0x25bcfde238b6afL, 0x50d705d397f5e7L,
          0xb65f60b944c974L, 0x8867fc327ac325L },
        { 0x2edc4413763effL, 0x892c0b3341fb63L, 0xb34b83ab3a7f28L,
          0x9aa106d15c2f18L, 0x720bbc61bb2277L, 0x637f72a5cfaefdL,
          0xf57db6ef43e565L, 0xceb7c67b58e772L }
    },
    {
        { 0x2793da56ecc1deL, 0x4e1097438f31b2L, 0x4229b4f8781267L,
          0xe5d2272dec04a1L, 0x6abb463ec17cffL, 0x28aaa7e0cbb048L,
          0x41dc081d22ef85L, 0xcbc361e5e63d0fL },
        { 0xb78aafcad5dbaaL, 0x0111505fc1edc3L, 0x63ed66d92c7bfaL,
          0x2982284e468919L, 0x30f1f21b8c0d8cL, 0xf0567472685093L,
          0x0e085b6f03dd0fL, 0xa8c8db85581e66L }
    },
    {
        { 0x42009a6264ad0cL, 0x13bf2b8593bef4L, 0x1d111905d4e8b1L,
          0xfe3e940ef7bddcL, 0xa012275624e62cL, 0xcb659241d6d3ccL,
          0xc7bcc70edb7ab6L, 0xff9fafbb750b1cL },
        { 0xf65df297fea84bL, 0x17c84a890b0e02L, 0xa92a859301e821L,
          0xbee8cb2fb480d1L, 0x7010b8c59c604eL, 0x47bf3f4e803c43L,
          0xd64514247b3fffL, 0xc4c5dcb9f0da13L }
    },
},
{
    {
        { 0x8af700cb5253b3L, 0x31ca605206957aL, 0x25744393eafdcdL,
          0x2ba5ae1d3ae15eL, 0x710b7385b82579L, 0x145ab57112b95aL,
          0x4b133a038c55c5L, 0xf7559c92a16fefL },
        { 0x70c3e68d9ba896L, 0x475dd32c33d07aL, 0xe084e473a41e40L,
          0xddc9382fd2e706L, 0x34b727579510bdL, 0x5e78a69a5f901eL,
          0x429dfd7dcfb823L, 0x1d9dc18014f0a3L }
    },
    {
        { 0x364fcdfaf403d7L, 0xd9ea4ffb7d7b34L, 0x21a3426cbb1dacL,
          0xfa51052143b4f5L, 0x2bca0736df2409L, 0x7e6985a8ad7285L,
          0x3a1a9d04aaa27fL, 0x1a815e19fc0c6cL },
        { 0xfab6147bb65bb3L, 0xa36dc0d33ced0bL, 0x26a88592062d78L,
          0x343861728a5fb7L, 0xe82da254ebb1adL, 0x70f5071d05aa11L,
          0x0b7f847adaac48L, 0xeb812bc93cb269L }
    },
    {
        { 0xcb317ccf7cacccL, 0xd3410d9cf85098L, 0xca68c8d7f078d7L,
          0xfe9e812b782efcL, 0x32e7c0f5f544b5L, 0x44fe95a3a7b7f2L,
          0xf4f1543e91327bL, 0x27d118d76645edL },
        { 0x690547cd7abc2cL, 0xf64680fb53c8afL, 0xbe0cbe079ea989L,
          0x6cf0ccea91af28L, 0xa3b85a29daa2f9L, 0xd4b663c91faed0L,
          0x782c7b7a8b20baL, 0xf494fafb8d98ceL }
    },
    {
        { 0x080c0d7002f55aL, 0xf4f8f142d6d9ddL, 0xb326229382f025L,
          0x58fd0b5ad28c20L, 0x704b9928d06a15L, 0xf4545d97fbd8e4L,
          0xc32fa63ed55581L, 0x3ab793601ac0fdL },
        { 0x13ece526099fd1L, 0x776dba89c79178L, 0x8d28212ce26c45L,
          0x09fddaf60d739cL, 0xf9931eda84826eL, 0x6e73d90b29439eL,
          0x94cfefc9095e61L, 0x3050d16802f474L }
    },
    {
        { 0x0898f8f9f6394bL, 0x48b8cea88b0e91L, 0x4bc99254c1b362L,
          0xe3fccb4827d9ecL, 0x5d4cf9ad950d6aL, 0xa16f1ef39b5b38L,
          0x3c76d1d620f288L, 0x9fdd059e119390L },
        { 0x7b5de9efb5edf8L, 0x3e290b9769d14eL, 0x4df3a916bd10b5L,
          0xae99bca82f8f7bL, 0x5481d5dc9524afL, 0xf112e4f69504f1L,
          0xb048f0951931ecL, 0xbff876a18f51b1L }
    },
    {
        { 0x932e2a746c1c37L, 0x903ad529aea4c1L, 0x717ac918f161f2L,
          0xa57d197f425e2aL, 0xae89dac7f39e0eL, 0x91655c0baa2a58L,
          0xe3dc28654836ddL, 0xb5f0baaa9ec9e6L },
        { 0xf7c4662bdbda04L, 0xbe5393b51059c0L, 0xb16d552dd95b0fL,
          0xde495b31b3bd96L, 0xb2a6e02c0206c5L, 0x045cc09014d3a9L,
          0xf66a3152a2f490L, 0x208c108c5dea05L }
    },
    {
        { 0x6e38b6865237eaL, 0x93a13039f27fc6L, 0x9a6d510a95068aL,
          0x6fbf216e7c9e54L, 0x7824290571ac1dL, 0x8cb23ba91c2a0cL,
          0x611202ec7e434dL, 0x8f901bf76058b4L },
        { 0xef0ac050849588L, 0xe0d2ddedd31804L, 0xaf5417ceb2ca81L,
          0x420ac065d1a509L, 0x46e345e9683bb6L, 0x6daf635f613f7fL,
          0xc9e829148a9576L, 0x5f9f1d1176d147L }
    },
    {
        { 0xd24ae1d77e9709L, 0x77751dc0047b8aL, 0xe325334c6a1593L,
          0x9baf962671f86aL, 0x425af6ac29a15eL, 0x31086002796e33L,
          0xb6ea78cfc253a5L, 0x4c733e0afae0eaL },
        { 0x4b7443a97c99b9L, 0xc14e9e450203a6L, 0xd1bb51552680baL,
          0xa56a3efd55533aL, 0xa66e38c169e1a0L, 0xb3e4df9eed7da0L,
          0x022c937ddce3d9L, 0x8552089f6e36b4L }
    },
},
{
    {
        { 0x8e4bf95f5cc82eL, 0x2ad80c3c3ed6c9L, 0xf2e5b2cc9045e1L,
          0x42c906559b06d4L, 0xc1f73797b43b84L, 0x1710dbf72d7992L,
          0xe98cf47767b41cL, 0xe713fce7bfb9e9L },
        { 0x9f54ae99fa5134L, 0x3002fd8de40d0eL, 0xdc282b79311334L,
          0x5519810bfeb360L, 0x31539c70f96ffeL, 0x04eacc0d27777bL,
          0x59824108ff5053L, 0x598236632b67adL }
    },
    {
        { 0x6eb45546bea5c2L, 0x82cfae0d509a33L, 0x6a69bd8394bb59L,
          0x1880d8d5770ee1L, 0x63518447dacf9eL, 0x5b1ecc5f02b891L,
          0xeb7d900b6c9a5aL, 0xdab8a768897da8L },
        { 0x28c7be598851a6L, 0x0101d4f4d73c3bL, 0x3c2569c5084996L,
          0xb9bc911280bde0L, 0x513a22acd0d4f9L, 0xdf2986d2a15f3bL,
          0x231c28f2aa4943L, 0x29623ad0333870L }
    },
    {
        { 0x2ceb1784084416L, 0x924cf1c49516cdL, 0x76536c04be856fL,
          0x11b59cd47a265bL, 0x720dc844999494L, 0x910f794007b795L,
          0x8434e142d3df83L, 0x8f53878bd478d3L },
        { 0xd9b072eaeb9c2fL, 0x16f87eafd8a29fL, 0x8c42f9b2fd0de1L,
          0x916721e0e816efL, 0x2ecb47018bde37L, 0xcde3b7a2375da2L,
          0x30d0657ef94281L, 0x51054565cd7af8L }
    },
    {
        { 0x7230b334bdced3L, 0x0c6a3e10838569L, 0xf19c9ece3493b8L,
          0xf2759270d97c57L, 0xf14181e0c862ebL, 0xfd3bac132c72bcL,
          0x620563ff3be362L, 0x672ccaf47283b7L },
        { 0x191e3fa2b7bf16L, 0xf838633520dad7L, 0xd3dde553629d87L,
          0x14d8836af86ebeL, 0x3db7dfb221b2ceL, 0x3872abb0aed72aL,
          0xb60de528c665b7L, 0x89c259644982cbL }
    },
    {
        { 0x799a2de4dbba25L, 0xd818aaea42715eL, 0xbc88f4df55c362L,
          0x142a163713c9aeL, 0x411e8eefbfb33fL, 0x34b46296bb684aL,
          0x4344becdc81817L, 0xcc9573d17f9d46L },
        { 0xf85f8bcff38a7dL, 0xa14bf730caf117L, 0x126874f4ba6429L,
          0xcc9bf22aa5db97L, 0x62b56df6aba827L, 0xfee1cb89c9772aL,
          0xe36838f177e541L, 0x698815dadd438fL }
    },
    {
        { 0xc9fd89438ed1adL, 0x73cd79d7b6a601L, 0x2210e6205e8d20L,
          0x72384ac3592af5L, 0x5ccc079763d07eL, 0x2f31a4aa5f79ebL,
          0x693f4ed2945a95L, 0xc7120178056fdcL },
        { 0x361ecd2df4b09aL, 0xa5644eab7d929aL, 0x34abc0b3fabe9aL,
          0x1a2473ce942a8cL, 0xe00c9246454bc3L, 0xab324bcdff7366L,
          0xe1412f121b8f99L, 0x970b572e33551eL }
    },
    {
        { 0x6ca4cacbd0a6b5L, 0x5584787921d654L, 0x18e5253c809bdaL,
          0x01b32c3f0cbe5eL, 0xb9aa7540f987ddL, 0x628f4bb6dfa4dbL,
          0x0255f0b891890bL, 0x25b7df4874e590L },
        { 0xbded3188ed5f95L, 0x9dc428dca93023L, 0xc68f25abccf520L,
          0xc4f3764e616e6cL, 0xd9a57f1a1d9993L, 0xd1964a5533431bL,
          0x06cd77f02ab6d0L, 0xa66079103e52e0L }
    },
    {
        { 0xab088645f72700L, 0xf77b2ff0a1a44eL, 0x43ebdd8c2a24b5L,
          0xa6d67114f564d7L, 0x495df63f414160L, 0xf5bacd776f6de6L,
          0x3011aff7c2b43dL, 0xbb1e64c3241928L },
        { 0xf70c5725034073L, 0x891c62a68f1e97L, 0xed8eb2eb22e374L,
          0xd3a53e97dbcc2fL, 0x1d06281dc8f220L, 0x9eef48face4393L,
          0x96014f5d2abecdL, 0x1da7e092653cebL }
    },
},
{
    {
        { 0x7593318d00bc94L, 0x586f3c6c7262a2L, 0xea68f52958ad31L,
          0x6707fccd4e8bedL, 0xb7e35d6cb3f9ceL, 0x2cbb6f7f4b1be8L,
          0xa5352687b41aeeL, 0x1d77845f7b39b8L },
        { 0xb1f3995eaf9554L, 0x3250f70fe9e7d4L, 0x62e5d1ba00c23cL,
          0x5e422f5c10e3bfL, 0x7a18039c25cec4L, 0xb4e66a17cc4d5bL,
          0xad7c5f636d0e0cL, 0x9f40b12a4cf347L }
    },
    {
        { 0x697f88251e3696L, 0xc89bc40ab0a648L, 0x8f261a59785804L,
          0x4c7f900b51a2bdL, 0xd00e7af8a2dfcfL, 0xf9c534db642aebL,
          0xea2a79fb63df0eL, 0x392a69af2f64a4L },
        { 0x0c0f01cc331b6cL, 0x414bf2e6a5edb5L, 0xfe5ed815068391L,
          0x0a8078d62fbc34L, 0x78a438254bca98L, 0xf7a49ae3d727c7L,
          0x96c1de1ab4dffeL, 0x45901f73b9440aL }
    },
    {
        { 0x3f1189facfe46eL, 0xdca6f464467443L, 0xac385422eb5bcfL,
          0xb02dce9906bf72L, 0xdd8cdacfe1d454L, 0xc26f04c65f7218L,
          0xb4748596ea145dL, 0xc53dc6b5bdb315L },
        { 0xbe5be749ad7197L, 0x627e91918b5eccL, 0x57c889c9ea405dL,
          0x2e5650c1a5360bL, 0x42290df1b30b27L, 0x4a071575242687L,
          0x553ed1fd379133L, 0xb9d7a0701db019L }
    },
    {
        { 0xcfe551c56597dcL, 0x81af92a925ebd6L, 0x83efe16f4e8d57L,
          0x61bb4311f640d3L, 0xf80440f78b414aL, 0x72f3c636c9e3b4L,
          0xb55f43a6a03c66L, 0x47a9dede417037L },
        { 0x1a7e287dbb612bL, 0x895c3c7dbb9220L, 0xd50c86e6c04764L,
          0xed5269853cf7caL, 0xc78d799f74af55L, 0xb2ba0f2b969ff2L,
          0x06d48151c6530bL, 0x764a1fe165a575L }
    },
    {
        { 0x4383a3bc1b5eceL, 0x0563c8854ff148L, 0x9a452795af796eL,
          0xffba7c088e9953L, 0xfe9fb5eb6a3001L, 0x795098825b6b19L,
          0x67c899ad81be5eL, 0xc89ac8d2f9d29bL },
        { 0x7c76ba329ab8f7L, 0xb2a18c96e40f74L, 0x1b5056e3864d9bL,
          0xdfa503d9b582b8L, 0xfb035197c9c68eL, 0xdc501316b3c22bL,
          0x38ab231a6c96ffL, 0x4ea527c8cb1c10L }
    },
    {
        { 0xd632f20c05b4edL, 0xe0199fab2a032dL, 0x373295626812d7L,
          0x2aed855013df13L, 0x92ca24b39f96acL, 0x620273dbb9751aL,
          0x5d0d21ef7437a1L, 0x9de2a43077de56L },
        { 0x0569b1211a4674L, 0xfc3923e89c3989L, 0x3d127042c5c770L,
          0x0072b9084e8c37L, 0x7178d4dac39f9aL, 0x5f8292f778d345L,
          0x9e5bf0f77c7307L, 0x7691610c3a20f5L }
    },
    {
        { 0x7c4ead5705fe96L, 0x377ec35c8e464cL, 0x3e5b9907689954L,
          0xc0f6949a2d31eaL, 0x839d395c580671L, 0x2f347a6b215b09L,
          0xfdcfa33683df83L, 0x6e12cc26af39a8L },
        { 0xae46ec813a3bd2L, 0x03a7d3b59366f8L, 0xe2029d5b87aed4L,
          0xbdc4e43fe1b83dL, 0x768437cdb8a1a8L, 0xe47acc3ea0dd7fL,
          0x550e0cc62a0af4L, 0xcaf2cbc1a20962L }
    },
    {
        { 0x5a784f7f28a78fL, 0x952a9b507e9724L, 0x8ac5e411bab7a3L,
          0x1251e3fb7bc1e1L, 0xe360f82dc15e22L, 0x3ac72da95213f5L,
          0x65ee9ba4dcd47bL, 0xdfeab7b3af5952L },
        { 0x34c5c8026fd3c6L, 0xd977b08f3ac7eeL, 0x003bd017dba2f6L,
          0xcfc5cf8ac98c8dL, 0x05eb6040e46922L, 0xc248b17faa9352L,
          0xfa41c0f395c7a7L, 0x29931d4b71ee44L }
    },
},
{
    {
        { 0xac087bb07861c5L, 0x3bd37db5ae8240L, 0x94c68ecf94518fL,
          0xd32a378ff88a5bL, 0x42c8aaf9b441d1L, 0x089db70fc07f12L,
          0x211c386d3d4455L, 0x1db9af7546b158L },
        { 0xdfd1b6551bc927L, 0x69c04930733df4L, 0xdc72cd42aeb586L,
          0xeebdace823aa13L, 0x51b3b3c56ad643L, 0xb983a99d4e0426L,
          0xa1e5b6c69c4eccL, 0x37cd38245e6668L }
    },
    {
        { 0x158ce6d9f73aeaL, 0x36a774914ff475L, 0x0d4e424dc0b018L,
          0xc2c44483946f09L, 0x7a7de3ffacda62L, 0x49a19e6b486709L,
          0x65094d8db61da7L, 0x09edfd98f5ee87L },
        { 0xe460fcfb37226dL, 0x3b9d03969bf470L, 0x3d4d511247ca22L,
          0xc7248d6c782cb1L, 0x91189a000ad293L, 0x1244942e8abe75L,
          0x9f88d12bf52cdbL, 0x368463ebbbcadfL }
    },
    {
        { 0x419e4b38074f45L, 0xd3f8e2e0771c83L, 0xd2743b42e68d34L,
          0xc68b7dbb116a00L, 0xfad2cf7d84cc37L, 0xcfd27c0b7a0f4dL,
          0x3b9e23f190e587L, 0x7bab499751ca9eL },
        { 0x3270861a8f12eeL, 0xee1f38d31b36d5L, 0x748bb31e4c0eedL,
          0x9be5c9b110ebadL, 0x728660bc8b6cb6L, 0x7bc9df793d914aL,
          0x73a4f2cc88c859L, 0xbe4a2fdb4e7f0eL }
    },
    {
        { 0xe566ff8a450e77L, 0xb0b40066a13abaL, 0x483a510cd7dc90L,
          0xb1a20135fa9cccL, 0xeb0b631a80e67cL, 0x7c34e1f020801aL,
          0x0257dc8f4e447cL, 0x7abe7d174c6f0fL },
        { 0xf115a3ab19a576L, 0x8f0474a064ca0eL, 0x999bb6b351f99bL,
          0x855254b773edc3L, 0x49f6c2f427d717L, 0x9f682532e0cef2L,
          0x1fe126c2ee34f5L, 0x1ec2cae80150f7L }
    },
    {
        { 0x862c5afc005b7aL, 0x61adea7ec4ef17L, 0xf885fd3007b446L,
          0x25c129d9b0e30eL, 0xbc10f25feec7e0L, 0x3901ac4df79ee1L,
          0xad49db7fe9e19fL, 0xc8624d9360d050L },
        { 0xc74a576bf3260bL, 0xbde80248c010c2L, 0xf15532909b6977L,
          0x6a5a82ed52dcf8L, 0x4fbf59d29b9dfcL, 0x337d049c7b730cL,
          0xb3deac63a89cd4L, 0x1e07595ad2f2ebL }
    },
    {
        { 0xa0b0a4d3b7c84eL, 0xf132c378cf2b00L, 0x192814beaaa8ecL,
          0xe7929f97b4b5dfL, 0xf08a68e42d0ab7L, 0x814afb17b60cddL,
          0x78c348c7d9c160L, 0xf8a948844db217L },
        { 0xcdefd88eaa2578L, 0xf717f56bd0e260L, 0x7754e131694d02L,
          0x1254c14181dbd8L, 0x0dacdd26e5f312L, 0xb8abdfbcef87bfL,
          0xb985972e74e2eaL, 0x1717621002b424L }
    },
    {
        { 0x92cc75e162df70L, 0x1e20c0618ee849L, 0xc036b4626aa590L,
          0x31be67e4da5155L, 0x04911b5f7213b0L, 0x39261d7bb2e72eL,
          0x9e844665c015a3L, 0x2f59fc0298ae67L },
        { 0xa3ea7ba1701fccL, 0x87a5fa90ebd651L, 0xa607ed4301d7b1L,
          0xbd4ec5f3b2e271L, 0x732a1a2dc4180fL, 0xbe15d82feaa8c1L,
          0x103670266f2f3fL, 0xccfd3979e79ce8L }
    },
    {
        { 0x82ab83570a54adL, 0x5c1dee8e3bec75L, 0xf583ff454b556bL,
          0x9220199f461e60L, 0xdf61ca887fc4e7L, 0x6641fd20776dadL,
          0x00c6edd8edd061L, 0xaf9b14255f7e87L },
        { 0x73f15e49bbe3ecL, 0xdd3b788f8bc1faL, 0xb24cc071b8ff86L,
          0x6c260d241be58bL, 0xec1c4e36b10adaL, 0xf6b42097fdb985L,
          0x0d0ac85d47c212L, 0x967191c07d78d1L }
    },
},
{
    {
        { 0x3b11638843d0f3L, 0x4b89297f27f10eL, 0x477236e863ba2aL,
          0x1949622add280cL, 0x7cd523504da757L, 0xe0e99d279e4ff7L,
          0xb4ef894537da41L, 0xc55dde45a24ff1L },
        { 0x18d8e21b587521L, 0x8010b5d3777833L, 0x4af522dd3a54c8L,
          0x7cd476b4c0ac13L, 0x4587e614099f67L, 0x494d0ed605ee64L,
          0x3218ba2cc80903L, 0x5ff56aa0b2e169L }
    },
    {
        { 0x51ec94e3a06c69L, 0xa26d7be5e65c52L, 0x156f113d44ee96L,
          0x70f0968bf5b9b4L, 0x9b7e4695f5332dL, 0x36c295f6703829L,
          0x1522690d04f492L, 0xcf35ca4728043bL },
        { 0xf9ca3e1190a7c3L, 0x53d2413f971b07L, 0xae596529c48b49L,
          0x74672b8fefff5cL, 0x0a3018ba7643b0L, 0x51919e83e9b0a8L,
          0x89ad33dc932fb5L, 0x52a4419643e687L }
    },
    {
        { 0x7778990d2d0acdL, 0x3bdbcce487fdf1L, 0xdc413ca2b03dd2L,
          0x278755b9a2b7d0L, 0x4ebb8b535ddd7fL, 0x0465152bcbdb92L,
          0x34f22d6671d051L, 0x1ba04c787192b9L },
        { 0xb1693f483560c1L, 0xe08a5937d174e9L, 0x47ffdc464dc9afL,
          0x1123596ce8126cL, 0x632d95f1124628L, 0x66287abfee7c76L,
          0xb40fe60c552332L, 0x3f11729e304e1eL }
    },
    {
        { 0x97a6ea05030a8cL, 0x692419809c27b2L, 0x3308501ac9dd5dL,
          0x9fed7fabe73fdcL, 0xea555440535286L, 0xc7c07ab6c9b832L,
          0x178c882c51b967L, 0x6fa0c6986ee075L },
        { 0xbaa4a15b8b5c4aL, 0xf83c0ea3130c0aL, 0xcf8624b2800331L,
          0xade85cd7ccbcb8L, 0x971d7f6f08445dL, 0xfd480b76a546dcL,
          0xdc15a38c93761cL, 0xc4c495c9d04631L }
    },
    {
        { 0x5f4cee89470efeL, 0x9fe896188d93adL, 0x24783b3f4e49ceL,
          0x1bc7ed752ffb3eL, 0xa3abe6a6d81e17L, 0xd6bb8b47a333c3L,
          0x3485c0b10a3527L, 0x7cddc9c31a9d10L },
        { 0x0c78112c38ca37L, 0x10e249ddd2f8d8L, 0x72c88ccc511911L,
          0x4d75b5a29a6c84L, 0xc74b267a227b1eL, 0x698390cf8e35adL,
          0x8f27edfe98d230L, 0xec922f26bdc7f4L }
    },
    {
        { 0xac34023fc32e11L, 0xe0ae2f547200d1L, 0xa7c7492bd98c82L,
          0x3910b687b02154L, 0x6fdd06ce28ab6dL, 0xd3a7e49d98b012L,
          0x4c1c82b9f54207L, 0xef5bbe645c176fL },
        { 0x3d17960d3e71ebL, 0x90d7e84080e70cL, 0x83e6438bff5d9eL,
          0x1877e1f535d85cL, 0x931ed6efbb69ccL, 0xcf962651247848L,
          0x76d618b750da4eL, 0xc076708717fbf6L }
    },
    {
        { 0x80a5ac5eec5126L, 0x6d05dd13379c80L, 0x514b0892336d32L,
          0x586c0066725137L, 0xab2365a574f954L, 0x3c89ea0ac7d356L,
          0xf1f2edd27460baL, 0xf200ddbab9870fL },
        { 0xc8f1b2ca35e885L, 0x5d22f86e6e7550L, 0x24b9a409554615L,
          0xcb41107616314fL, 0xca752f0c976a11L, 0x3e2f839a08291aL,
          0x0cff22ff2c420eL, 0xafd603e82b9747L }
    },
    {
        { 0xaddeddc810a3daL, 0x78b6c2dd3a87bfL, 0xbc7020bde3a04cL,
          0x47ab9739b6d045L, 0x3b046d60959358L, 0x0f953e7509ee3eL,
          0x803dc8669fc61bL, 0xcceaec0893c8d4L },
        { 0x21f8c40b048a45L, 0xb535073fcaea8aL, 0xe712c3590e360bL,
          0x5d0f3f48403338L, 0xe0ea26c7207f2dL, 0x20f6b57ffd9e05L,
          0xb97d68e4788b00L, 0xb1215541889cceL }
    },
},
{
    {
        { 0x0079817464238eL, 0x21103020d381caL, 0x1cc4c6ed9f01b5L,
          0x5e35dc55a131b1L, 0xb61848d06944ebL, 0x83792a029631a3L,
          0xbe1017fafca0ddL, 0x70aaa01782fcbbL },
        { 0xc63b7a099945e7L, 0xe9164ecc4486c1L, 0xb133e35885f2c1L,
          0x186f0d3c99ae02L, 0x2fca4922bf53e6L, 0xf922aa248a02bcL,
          0x4fe64900dd3dcaL, 0xe8c313ff6a8207L }
    },
    {
        { 0xc5b358397caf1eL, 0xa001922922a4b6L, 0x67e36bedf07c95L,
          0xabaa0aeb2f4f34L, 0x66dc926dedc333L, 0x82021c438ec5b3L,
          0x82b4f2600ab176L, 0x1b7c22e69c45afL },
        { 0x07b0dbe0924ad9L, 0xe030936a407ddeL, 0x66e1ce926ccd06L,
          0xb50c108e3505a9L, 0x8b921e1da98f51L, 0x449ca1a20cf7c7L,
          0xadb80c7e67d079L, 0x205aa54834372dL }
    },
    {
        { 0x1482b4819bf847L, 0xd6c16ab5906f0fL, 0x323fb1723ad060L,
          0x0346389c832be7L, 0xe71b2d82ee45bfL, 0x761c37dfb22276L,
          0xa9b33345d70be2L, 0x81a06565a0627aL },
        { 0x337750399a6282L, 0xafc8d2ed0436f0L, 0x22f71d3c53342fL,
          0x66ca56d8939ad3L, 0x15a919230e09baL, 0x261091ea6de890L,
          0x609d700e78f2d5L, 0x8aa52ee8eaaf78L }
    },
    {
        { 0xa398788ce76258L, 0x3031d07494b975L, 0x4a6d652043dfe2L,
          0xdb1a849b4401ecL, 0xf81ebbbce8bbccL, 0x937dd4716efe9eL,
          0x9c19350ef85eccL, 0x260d932214273bL },
        { 0x1d7e21e77bf1a3L, 0x199d689a544eb7L, 0x9da594194ced50L,
          0x71a60be8a0aeaaL, 0x183a0ae26d3b51L, 0x49f176a8df9728L,
          0x744376e3230674L, 0xb2cb21ae25541cL }
    },
    {
        { 0x7a721589a0071fL, 0xe19dd29e7d2a6bL, 0x3deb34e55113f0L,
          0xef1f8ebede573bL, 0xa8f7ff95665e37L, 0xa2c21eaf2d7777L,
          0x1387afa91e2e39L, 0x04057b97db68f6L },
        { 0x8b9d5ae1c241f7L, 0x689588a8e75993L, 0x79585b45c0e2d4L,
          0xba1ef167b64974L, 0x72685bc1c08a75L, 0xf0a5814d572eddL,
          0x71464a35ab0e70L, 0xc93c92b339aea7L }
    },
    {
        { 0x1917e2a5b8a87dL, 0xea5db763a82756L, 0x5bba2fb6420e2bL,
          0x5cc0501019372aL, 0xb1ef8beccc5efdL, 0xaf06393f49c57dL,
          0x3ab1adf87a0bc4L, 0x2ee4cca34fe6b6L },
        { 0xd1606686b8ba9bL, 0xef137d97efec13L, 0x7b6046550abb76L,
          0xb40ec2bf753a00L, 0x696ed22eaf8f1dL, 0x398c91fd8ba3d8L,
          0x11f203437db313L, 0xe1ec33bfe5079eL }
    },
    {
        { 0x8a10c00bdc81f0L, 0x5f392566fe8e05L, 0xa595dab14a368eL,
          0x32b318138cec6bL, 0xd77afde1b00d00L, 0x3c979284d9923dL,
          0x78f0e7a76e13ddL, 0x5ee8e59bf75675L },
        { 0x49ec89391b130cL, 0x9416182a47a441L, 0x54555b576e2ce8L,
          0xcbdd2fd349c40bL, 0x10ae7379392bbeL, 0x270b1112e2dab0L,
          0x5cb7712af293f4L, 0xfc22a33d6095c6L }
    },
    {
        { 0xdcb5bbd0f15878L, 0xbcf27adb6bba48L, 0x979913e7b70ebaL,
          0x4c0f34b158578aL, 0x53f59a76ed6088L, 0x19b3b2c75b0fc2L,
          0xad628dc0153f3cL, 0x5195a2bcec1607L },
        { 0x95f8b84dfe0f7aL, 0x935c6b0152920bL, 0x25f9e314da1056L,
          0x4910a94b28c229L, 0x54b03b48ee4d6eL, 0xc991fc3694e3edL,
          0x68c4c26dbe5709L, 0xc9cfce463d7657L }
    },
},
{
    {
        { 0x21c9227f52a44eL, 0x7f105a2e85bfbdL, 0x887781f6268fc2L,
          0x56ee808a2d7e35L, 0x14f9de52d3930fL, 0x4a4e356dcb561aL,
          0x87362267f95598L, 0x211c3425f34151L },
        { 0x8fcb75b0eaf9cbL, 0xcc9edf93d60ce2L, 0x54412c9a5fe627L,
          0x6036a72842dd09L, 0x71ce668a6c6099L, 0x02b30d75386764L,
          0xb69bed36f18e23L, 0x124c9b1d1de9f4L }
    },
    {
        { 0xe8f8d95e69b531L, 0xe1e115eaff1049L, 0x9087cd1eddea0cL,
          0x8ed55a57449916L, 0x8009f547808404L, 0x990f21617fea55L,
          0x68ba624fe8ecf9L, 0x8ac295056d1f47L },
        { 0x3257887529dfb0L, 0xc4a613f244c080L, 0xabb1ac028672faL,
          0xb2915c531eb291L, 0x6e368ca8fababaL, 0x6b8c2591fde498L,
          0x67724a1f2a548cL, 0x6b3b7e8f90409bL }
    },
    {
        { 0x5415003fae20aaL, 0x95858a985df5ceL, 0x42bc9870ac6beeL,
          0x8d843c539ea1a9L, 0x5de200cb571043L, 0x084fcd51741a33L,
          0xe1ca20c0009d1cL, 0x0271d28e957e6dL },
        { 0x84cbf809e3be55L, 0xc804dda1c578c6L, 0xea85489409a93aL,
          0x64a450a972021dL, 0xc6a2161e681312L, 0x280bff965bc111L,
          0xd358a4b0f8526fL, 0xd967be8953a3abL }
    },
    {
        { 0x4c5e6157dd066cL, 0x37afd33634c8d4L, 0xa3ac88a42d8b87L,
          0x9681e9b938b607L, 0x7a286ab37fe4c8L, 0xdeee5742494245L,
          0x184b9d36af75a8L, 0x20f696a3670c04L },
        { 0x1340adfa39e8b9L, 0x03c19290850b2eL, 0x435ebd42c0e1efL,
          0x49de18b142ee9bL, 0xb440b273f116f2L, 0xd94e9fa2214463L,
          0x1b0ddd36311543L, 0x1ae042a991ba3cL }
    },
    {
        { 0xbc322f85bb47aaL, 0x9e2562554a5845L, 0x96b65ae21115f3L,
          0x46fbed4bb5757bL, 0x18aec4f4c42dceL, 0xc59caf68d801f0L,
          0x91894631205521L, 0x66bd8e089feb7aL },
        { 0x39ebe95c529ee7L, 0x28d89928eadb99L, 0x6058c786927544L,
          0x877e7a5d3808ecL, 0x8f651111c52eafL, 0xfb59812ae221cdL,
          0x22289c6f890391L, 0xa97695b4966e92L }
    },
    {
        { 0xf0a91226ff10f0L, 0x49a931ba2a65c8L, 0x3fcebbcb1d3cb0L,
          0x70eb79bca9685fL, 0x82520b5ab38cb6L, 0xccf991b76304c3L,
          0x575aab1af8b07cL, 0xec8166a5ed5efbL },
        { 0xddc5698c8689b1L, 0x227c949b2e78d7L, 0x61323218e07d91L,
          0x658a11d22cfd62L, 0x908fb44004dd5fL, 0xe3d14f090d21b1L,
          0x6f3db9da6a1639L, 0x09d86c0333a525L }
    },
    {
        { 0xd83eaf06f043f7L, 0x88ab648b52d5f6L, 0x67c664d57144d7L,
          0x55d7644eafc8b5L, 0x1c89f20cceb291L, 0x51aec7b831ac47L,
          0x51172fa6148854L, 0x8fabf7ef6d7bfeL },
        { 0x5910316477ee27L, 0x5f299dd20fe61eL, 0x48079a842826abL,
          0xf4a83ba22591faL, 0x8fac66055482ecL, 0x48fd5f16b65b3bL,
          0x4288a7c9fd9e19L, 0x27db8199377894L }
    },
    {
        { 0x2936ee47fd9dd6L, 0xcce5f0e9ec87c6L, 0x15a50e3db6e3b4L,
          0x61df105ad701c8L, 0x3601add1dff1f7L, 0xb761e06e8a16e1L,
          0x4341e021af3f91L, 0x9156a4a933fa3fL },
        { 0x9dc46ae54bc01dL, 0x605577a64eb910L, 0x22b99f85a59a99L,
          0xab2dbaf0a229d8L, 0xa8bfb656599364L, 0x39ed4a5e94ebf0L,
          0x7b46a1e0dbb23eL, 0x117b1958751422L }
    },
},
{
    {
        { 0xd19e8fd423bddfL, 0x9d77042387ef59L, 0x315cbdd849590aL,
          0xfdc637c7866c1eL, 0x72be83d03515a6L, 0xd44a4a00376780L,
          0x3b9613119e0c2bL, 0x023aca37b1a689L },
        { 0xf5f368782282eaL, 0x44710898a8b5c7L, 0xcd2f00a17a3066L,
          0x754e11281ed681L, 0x9c6c70c0bfcefdL, 0xd6aced03b6f29bL,
          0xe443d562817a2aL, 0xe590ef4e7c0012L }
    },
    {
        { 0xc2f96763e62e2aL, 0x661816eb2daa26L, 0x3515fd2dd5f512L,
          0xdc36e2756b6e75L, 0x0bdde4674cc658L, 0x102908600e7644L,
          0xfdf00451694a09L, 0x454bcb6ceac169L },
        { 0xf4c92ab6481eb6L, 0x8b77afa09750e7L, 0xe6f42316362d6dL,
          0x0d45deef53a3aeL, 0xdac7aacd7dcf98L, 0x628cb7f125ec4aL,
          0x41e8a20aec0320L, 0x7418c7eea2e35bL }
    },
    {
        { 0x4d649abdf40519L, 0x8cb22d43525833L, 0x15f6d137a5333fL,
          0x8c3991b72c23eeL, 0x248b9a50cd44a3L, 0x6b4c4e0ccc1a75L,
          0x3221efb15c99a9L, 0x236d5040a9c504L },
        { 0x401c7fbd559100L, 0xcf0e07507c524dL, 0x39647c034a9275L,
          0x2355422f7e8683L, 0x3e0a16eb3ae670L, 0x1c83bcbad61b7fL,
          0x491bcb19ca6cbeL, 0xe668dc45e29458L }
    },
    {
        { 0xe44c65b219379eL, 0x211381bbb607eeL, 0xd4c7428b7bc6dbL,
          0xba62a03b76a2e8L, 0xe1729c98bb0b31L, 0x3caeb50c6bbc10L,
          0x6c66727b0187aaL, 0xbf9d2f0fb90dcfL },
        { 0xec693501184dc6L, 0xd58d2a32698eb5L, 0xb366d8da316b07L,
          0xe1e39bb251c017L, 0xbe44ba9adb157fL, 0xbaa9a9a8a8b06cL,
          0xd0f46356e473e1L, 0xd25a8f61d681c6L }
    },
    {
        { 0xba39d5fcb102c7L, 0x66eba21d8aa1ebL, 0xcc2591a697fbf4L,
          0x5adb5792317f54L, 0xa01ae71f76c6f9L, 0x2c525de5042705L,
          0xc8f42724f4479fL, 0x26ab54ae6d7a5bL },
        { 0xda217b5dc28106L, 0xc7cadeaeb2ae6aL, 0x0b1609453ea3b2L,
          0xcddcc1ccc6111bL, 0x5c47affa7a7bebL, 0xf9931bd0e52dabL,
          0x5231835c6dcf96L, 0x7095bdef27ea4eL }
    },
    {
        { 0xee8adaec33b4e2L, 0x300665163ceb44L, 0xf1476fb880b086L,
          0x07033289569ce8L, 0x2cabf9a238b595L, 0x85017bc26c8158L,
          0x420b5b568d5144L, 0xa9f5f1ef9c696fL },
        { 0x1409c3ac8fec5aL, 0x541516f28e9579L, 0x06573f70e1f446L,
          0x3e3c7062311b96L, 0x0033f1a3c2ffd8L, 0x8e808fcca6711cL,
          0x716752d07aef98L, 0x5e53e9a92525b3L }
    },
    {
        { 0xce98a425a1c29fL, 0xaa703483ca6dc9L, 0xe77d822edfa48bL,
          0xd2e3455068abcaL, 0xb456e81482cfcaL, 0xc5aa9817fbfb08L,
          0x8979f258243194L, 0x727f2172cd043dL },
        { 0x7cca616aa53923L, 0x387c5aee9bcb72L, 0x0173fd437580bbL,
          0xdd7795b75fc0d9L, 0x47d1c37345deaeL, 0x2eb5d7fb0d1c03L,
          0xf7a1b92958f002L, 0x7365cf48f61b67L }
    },
    {
        { 0x4b22c3b562a5edL, 0x711216f5c7cd07L, 0x51f72c49ba0648L,
          0xc10d0930de9e6fL, 0xaca479bfda63baL, 0x4722a55af532b0L,
          0x8d59eb77236f39L, 0x5cad8744465c34L },
        { 0xa2119e5722b0c1L, 0xb670264f343ea4L, 0x6910f02c19f387L,
          0xcfec5bc0381fbaL, 0x5f5de0d52c0a1dL, 0x4e474d56378cb6L,
          0x2fc802727e2ba3L, 0xa215da3159b541L }
    },
},
{
    {
        { 0xed535858499895L, 0xa0aefd565c998dL, 0x210d8502d5a561L,
          0xc2cc23ca2cd9d6L, 0x2371d46c4d297eL, 0x88b2143d18d441L,
          0xbebdad9043993dL, 0x6ba91e7ad5f28dL },
        { 0xc2bb3f13a731f4L, 0xd35cfac5d0d5c3L, 0x995099835ac427L,
          0x8938bb55458adbL, 0x0bd738cab26f3bL, 0x56db3d5a28cd8dL,
          0x87eb95fa1d8b4bL, 0xd6700efe7f3b4bL }
    },
    {
        { 0x962c920ea1e57bL, 0xd3be37e6dded6dL, 0xf499b622c96a73L,
          0x3eaf7b46c99752L, 0xa310c89025590bL, 0x535aa4a721db23L,
          0x56ab57819714a0L, 0xeecb4fad4048c1L },
        { 0x7b79ec4470c466L, 0xc4e8f2e1383ceeL, 0x0f5d7765750c45L,
          0xa3b3bc3725527dL, 0x2f5deb66d00cceL, 0x5d5a0f495a8d81L,
          0x50a442ee02b824L, 0xafb04462a11628L }
    },
    {
        { 0x72b67bc0c613deL, 0x0150d4be6f0b24L, 0x847854e8ed289dL,
          0xe08292fa320f88L, 0xd5b6da329c6160L, 0x2a48e2d4fb9d06L,
          0x55d9e412de087cL, 0x65683b54f02100L },
        { 0x4dc8c2ea8886c6L, 0xe966dd220d6114L, 0x99745eba57af97L,
          0x23a9a71b854725L, 0x8effe05621a047L, 0xf16d284049a4beL,
          0x95828c25b0660fL, 0xd5b69ba56e96b0L }
    },
    {
        { 0x0b5b4244ffa0b8L, 0x0585b45096cc5eL, 0x413e1aef505d37L,
          0xe5652a30c7ab8dL, 0xab32fb72990120L, 0x6b8b16e3f09368L,
          0xbf9fadbefe128eL, 0x85f366b14b7671L },
        { 0xcb2f294090608dL, 0x25e2769ac3045fL, 0x069c4f06131904L,
          0x1c57cf1329a779L, 0x72fe0d5b7cace7L, 0x04d9f430897a45L,
          0xbaf32f6359a645L, 0x0fa854ffa7485aL }
    },
    {
        { 0xae3533c5f56f60L, 0x9773bbb0ad9360L, 0x769b34a38fbe6bL,
          0xb5ba8e9ffb0c00L, 0xa93931875472e4L, 0x12cac92ce5f30fL,
          0x514fc06a9e7dbcL, 0xd7ca86558b4734L },
        { 0xd101ff365a730bL, 0x92da451abe70e9L, 0xfb5f94aef7bf4bL,
          0x8c3ef4c1d56c7bL, 0xb0857668435c10L, 0x7fbbbdae7ed4ccL,
          0x1da6eaf24f372fL, 0x0ab2c1f59b8ae3L }
    },
    {
        { 0x63a1a78f10a4b9L, 0xbb5278d0c7e510L, 0x97b224ef874142L,
          0x0a9ff52b2517b1L, 0x1b5a485c5cd920L, 0x1a8e2eba1823b9L,
          0x2b088c00e914a8L, 0xe5ec3adcf13432L },
        { 0x0d6ab3e6e7e253L, 0x9f0f5cd6f18458L, 0x839a744f459a6dL,
          0xb4b4f941eb15f7L, 0xe0313acc72cb14L, 0x58ee933b20472dL,
          0x5f73d7a872543eL, 0xb1700c5501f067L }
    },
    {
        { 0xb70428e085f67fL, 0x5441d5143cabe5L, 0x4d0e8c2e0a6055L,
          0x8d39a080882e4fL, 0x615bb32c1cb39dL, 0x113f18df7a1642L,
          0xbab8cf5250681fL, 0x3017ba2677b72aL },
        { 0xcd2b6e95a3a876L, 0x04765012035a69L, 0x31d6440efa2ea0L,
          0xde8f8d156874d5L, 0xcbc71cd0199d4aL, 0xc546b61e7f2170L,
          0x4e57e4e112c4c3L, 0x58955a8d1622baL }
    },
    {
        { 0x0064cd704e2f6fL, 0xe9d458de0edd38L, 0xeb1a5977e0a5c8L,
          0xe322ece01fc0a8L, 0x8b9d1661032a19L, 0x3e7b539a89de94L,
          0xfa30262001c754L, 0xe33de4ddb588f6L },
        { 0x4dafbdb954eb94L, 0xbb436480584c1bL, 0x622c93e5dbe29bL,
          0x968f9e3f57b931L, 0x98f03be0f6453bL, 0xb0ecc7f08f696cL,
          0x5af55f4a505335L, 0x028533efb3fa9bL }
    },
},
{
    {
        { 0x3bc8e6827e8d86L, 0x4e43b3063f105aL, 0x5301b7d4981250L,
          0x8b0a75e9f72fa8L, 0x88f59db357348cL, 0x5f0ebb1ec4208eL,
          0x4712561c043d3bL, 0x9e5ded0c806b97L },
        { 0xf9bd0a62121d09L, 0x1759ecbe337cd1L, 0xd1acc0ee945542L,
          0x3683febbd2f63aL, 0x44f1bccda5dfe9L, 0xa3606c9707f22fL,
          0x45ef0642d96ca5L, 0xfc3107d9022df9L }
    },
    {
        { 0xe81320b44be755L, 0xdf213d55c7c761L, 0xf43d2d5b4e5db9L,
          0x3bcfd828dedcd2L, 0xdf368a6d37a9ecL, 0xfef20aef475a77L,
          0x22f5894162c064L, 0x956bc660142a7dL },
        { 0xaaa10e27daec78L, 0x3cb9b72b6e9a78L, 0xa740bade383f72L,
          0xc31b4017759007L, 0xdada964a7afc50L, 0x6bf062cfd3d11fL,
          0x9470d535db3679L, 0x339447303abf13L }
    },
    {
        { 0x533f44046e5d7fL, 0xd1793e349048c8L, 0x59e11501929b94L,
          0xcddbbcb8364134L, 0x795c794582774fL, 0x114dfc4e03081aL,
          0x541ef68ef54042L, 0x159295b23f18cdL },
        { 0xfb7e2ba48a2c8cL, 0xe2d4572bb6d116L, 0x7bb0b22d750b53L,
          0xc58888cd142ee8L, 0xd11537a90c9e2dL, 0x77d5858d02eb9eL,
          0x1fa4c75d444a79L, 0xf19b2d3d58a68dL }
    },
    {
        { 0x37e5b73eb8b90fL, 0x3737f7a3f2a963L, 0x87913fa9de35e0L,
          0xec7f9928731eddL, 0x6e6259e219491eL, 0xb2148a04de236cL,
          0x89700e8fdd309bL, 0x9ce51e49f0bf80L },
        { 0xe7ec421301f17bL, 0xa4b570a3bc5f4fL, 0xc2b1b2a1285ee2L,
          0x5e86bc8c53db73L, 0xb65fceaf24fa90L, 0x9e74c5608ab024L,
          0x5c8003df9ed877L, 0xa632e9e4a2cbbcL }
    },
    {
        { 0x32a4546c91c8b5L, 0xc122b5ac969363L, 0xbbbec5e3648b3aL,
          0xd5a365e25143b0L, 0xcf3e46454157ceL, 0x9712f04f9bab64L,
          0xc12d43a04b4008L, 0x51932d72edf1c7L },
        { 0xaef1655b2f8470L, 0xaa8e3f36c24aceL, 0x7da75da6b4e761L,
          0xd371827b90bca2L, 0x84db4500afb45cL, 0xae12045ef46b5dL,
          0x91639a5d962f98L, 0x669cbe672f2ac0L }
    },
    {
        { 0x851bb3183a4356L, 0x7d436bf9a1bf15L, 0x46a3f0e120b378L,
          0x9302abc3f5b357L, 0x1e0672693fef53L, 0xb12f4a95fd2ee9L,
          0x94a884c7de9433L, 0x2645234a6f2874L },
        { 0x6fb56f5cdb8dfaL, 0x4a17dfc9e0ee4eL, 0xe269d8383ab01eL,
          0xda932dab77c10fL, 0x463af0c0321243L, 0xbe1d68216fc8a3L,
          0x2eae3ea48b39e3L, 0x94230213b03e7bL }
    },
    {
        { 0xaeb507cb22f28aL, 0xa77458b49a6b44L, 0x232ed5ac03dc17L,
          0x79dfc169c61ac6L, 0x7c48be9cd71b93L, 0x983d68ac429cd9L,
          0x7709c4798ae2c8L, 0xe4765c0a5df075L },
        { 0x23c4deb3367f33L, 0xbdf2b7e37d72a7L, 0xbaab5c70af2d26L,
          0xd609f7ffd026abL, 0x23b72b2541b039L, 0x8d06bac83be852L,
          0x911d4a9cb23d1cL, 0xeae815cfb0dbd7L }
    },
    {
        { 0x487c35c2c33481L, 0xffab636b6136dbL, 0xccd4daea3d3aa4L,
          0x87149bbc3704e0L, 0x9de8119c0e8396L, 0xd49357a58e7ca6L,
          0x68789181562d75L, 0xc7453815ab1fadL },
        { 0x0f1579802c9b91L, 0x7ffc3f0b1ddde5L, 0xa01d5e06aae50dL,
          0x6a97e65e279873L, 0x4bcf42fb5b1b41L, 0x1c6410f32f5982L,
          0xd4f760050701c8L, 0xff02663873b90dL }
    },
},
{
    {
        { 0xdc53ea2e5b2de2L, 0x94b352d38acecbL, 0x37d960b0d9d5e5L,
          0xabd868f90bd997L, 0x781668f35a7376L, 0x043d59710118bfL,
          0xd4da719f57928aL, 0x01942f6983e46cL },
        { 0xab97fc8728bd76L, 0x825956b4b5c1c5L, 0x202809fc82a104L,
          0xdb63e9cc8e3132L, 0xa41c701c2181afL, 0xd28018043e066aL,
          0xc734e4124044ceL, 0x4d9ab23505193cL }
    },
    {
        { 0x0bcd42af9f0c3fL, 0xda21a46b94a218L, 0xe55243c0ffc788L,
          0x318aae647a5551L, 0x8c2938b79af9cbL, 0x5d15232ec1dce5L,
          0x3d310ba8ad2e5cL, 0xd3d972494f792aL },
        { 0xdeb4ca112a9553L, 0x2f1ed04eb54d9dL, 0xaa9c9cf69fb7a1L,
          0xeb73c3a54dcd3aL, 0xee3eddcf5f201fL, 0x35f9e1cba7d234L,
          0x1d1d04cd2e242fL, 0x48df9d80df7515L }
    },
    {
        { 0x4ecc77da81dd9aL, 0xa6ac4bb03aa015L, 0x7645842bbc4fedL,
          0x9ae34cd9d6cf52L, 0xf8ff0335917e0bL, 0x7c9da37c2cc175L,
          0x1e74dccaaacfbeL, 0xa8f2df07999af8L },
        { 0xd06c4ea102a466L, 0x2156e87ae190ddL, 0xc95db8aec4a863L,
          0x49edffd244a6feL, 0x110fae6904f81eL, 0xbaa3e50a1cd104L,
          0x5bd38a20478b65L, 0x2b57d05daefbccL }
    },
    {
        { 0x1ce92ba86f4534L, 0xb2a8592414f5e3L, 0xdd7a4c69979436L,
          0x7599aff3f0add7L, 0xe0ce4d3e2d4f64L, 0x74475cc401a29fL,
          0xaef6541a2377d9L, 0x54048f53f917b6L },
        { 0x1b86b2205312ecL, 0x779ba2231493cbL, 0xc718369aac9320L,
          0xeab01a8617fce4L, 0x17b1f10f7187faL, 0xe68eda0a1aca46L,
          0x61033fe2586342L, 0xfc14e790b6ca43L }
    },
    {
        { 0x9f2231913d2491L, 0x66bdb537997202L, 0x0bafb0c4617f34L,
          0x5917831f3bb7b3L, 0x6feb2a6b45bddbL, 0x08662b30202c19L,
          0x0bc2b5705852f6L, 0x2c00fd491818c2L },
        { 0xca7672cda37dacL, 0xfe4c04c5a30865L, 0x5f1399f322e92aL,
          0xe7d67ea25b1bebL, 0xe08b014dce7f68L, 0x24df52af2f2b3cL,
          0x2028b23750ecd1L, 0x9b25d4bc810a45L }
    },
    {
        { 0xa35b7157a9d799L, 0x6da1eb301f9c99L, 0x33ef91ce363ba8L,
          0x21c0e2ece140daL, 0xb0b11bf158cd84L, 0x6a8744293da438L,
          0x924f10d3db585bL, 0xf5ddd7310c6159L },
        { 0xb72dcb86a74c21L, 0x6d14198cc8f79fL, 0x99f4b6c9c5a8d6L,
          0x063968890e135cL, 0x330edb883f6385L, 0xe1a5a6b9079675L,
          0x6e37fa8b8f5fe0L, 0x60e2fd961dca1eL }
    },
    {
        { 0xc6cb40366c395eL, 0x03b21a7b51d0f1L, 0xbc478a5e693181L,
          0x0017c2fc6cff33L, 0x740a5b839d8d1eL, 0x3968d664d9ec6dL,
          0xfd53738b0ef1b0L, 0x73ca8fd1ed0a04L },
        { 0x4ace93875ab371L, 0xd602936ddad7e9L, 0x1f5424a750bcc2L,
          0xfe09b3668c7a17L, 0x165f7de58341ecL, 0x95b825a6ce61e5L,
          0x9d31e1966c83c4L, 0x65b3e08cc5887bL }
    },
    {
        { 0xd37e93221482d1L, 0x9af659708b6380L, 0x279426a7d61e4bL,
          0x80dd0ec80997adL, 0x7239b0dd5b76d4L, 0x92e6c73e76c098L,
          0xeeb2321eab3e1dL, 0xa69c4a7eb1a910L },
        { 0x46d6aa7833d9aeL, 0x3ee6957572b0feL, 0x44ccbedcdb3d97L,
          0x342f29dcbea01bL, 0x0d518c58926876L, 0xaaabae75585d2cL,
          0xc548c77e008f58L, 0x819e2fa21fab2cL }
    },
},
{
    {
        { 0x468e149c16e981L, 0x286c7909ddbb7cL, 0x2a92d47db7a38aL,
          0xde614e68a27cb2L, 0x8dc8822e5b0ab6L, 0x38441aecf48565L,
          0x11ed5c9089435bL, 0x238928682d0d31L },
        { 0xc6698d472f2f31L, 0x295242c56d76afL, 0x4099205eba563bL,
          0xae7de5a3ab7384L, 0xccdf127d0ed86cL, 0xb9b6d5b965c3c3L,
          0xe351a8f2c31ad7L, 0xa761dd8ac12f13L }
    },
    {
        { 0xda115ddf171ab7L, 0x2de17b1401f93dL, 0x95019ca40964b4L,
          0x169d1f465ba3c3L, 0x534a0070090d08L, 0x805c5e282bf410L,
          0x15dfe1165f8d90L, 0x827a416ca72456L },
        { 0x5af888433a36c4L, 0x8bfa54cd8ee604L, 0x08fd1419ce290fL,
          0x2db5e8c287b3a6L, 0xe5be98103cdad2L, 0x155b874bf810b9L,
          0x2ae42de670f473L, 0x22185847f74657L }
    },
    {
        { 0x54b2a5023ffa43L, 0xcf87b16a24d919L, 0x1ff540263524e8L,
          0x73c94e056d1e54L, 0x76515523899fb5L, 0x13a721418723bfL,
          0x39afbdd3561517L, 0x49b790a9f2862eL },
        { 0xc8c1f4f527d2ceL, 0x1997aec7609bb7L, 0x583ad8002a3400L,
          0xac2374e4f79706L, 0xbf1f9a821b7183L, 0x06158ab6600fe0L,
          0xfcc9b2ebd56751L, 0xe1de5acddaaec7L }
    },
    {
        { 0x230baa1788fdabL, 0xf30860a7d04597L, 0xa2c7ece99f4caaL,
          0xbd39f106ad065eL, 0xfd92f5d3bef7bdL, 0x6069fad96d2203L,
          0xbff38cac4d9e0dL, 0x419a0171fda313L },
        { 0x5d77fd8572f035L, 0x5af99f2b282b40L, 0x7257d3b23facffL,
          0xf2ee22358c90afL, 0xcc2687d9b6a52aL, 0x140892c302430eL,
          0xa934d5e3ec4f38L, 0xc087d7c3bd18beL }
    },
    {
        { 0x7e94138a2c5ed7L, 0xbc8ceef53610bfL, 0xe89356bd86f803L,
          0x9a3a3805a55330L, 0xe894aba11ad648L, 0x2e68fbaba95918L,
          0x643e2bafcad344L, 0x0dd025661640aaL },
        { 0xc02e479e25cbddL, 0xd78c4d813a1b3fL, 0xa6dae8fcca9692L,
          0x3dd91e9e5de8a0L, 0x78ae0ce764ea36L, 0xb4ad99985dbc5eL,
          0x967ff23e82a169L, 0xaeb26ecbaee1fcL }
    },
    {
        { 0x8c502559a6f90cL, 0x56e7abe0ea374aL, 0x675c72256413b2L,
          0xd3fc17e946753fL, 0x28c4e1fe235f7cL, 0xe209bcdb028eb0L,
          0x7d0f93a489fe88L, 0xb966a2e063706aL },
        { 0xb6c228c4a30319L, 0x6868efeca6d674L, 0x0610a70057311aL,
          0x0808112bad7f89L, 0x2a2462c1dd6181L, 0x52ed9feb58e88aL,
          0xbbff16f33821a2L, 0xda53e9617f882aL }
    },
    {
        { 0xb6ffca38c30e5dL, 0xa90f9915c905f5L, 0x72fb200d753e88L,
          0xe509d4c7256c6aL, 0x369e552d866500L, 0xee4b7e033cf8aeL,
          0x280d954efcf6ebL, 0x5b275d3d557f0eL },
        { 0xeb17211b5cecf8L, 0xd6ad50fbdb2f8dL, 0x2478c7b35e04b7L,
          0x97e7143ac73bd3L, 0x09d6ede4817e24L, 0x68fea712c405e1L,
          0x34adbc905f67a1L, 0xd20ab7073edf99L }
    },
    {
        { 0xe116a96569f191L, 0xb3f0bce4d6e29aL, 0x30b9e1af51dbabL,
          0x1dd36f3346d276L, 0x83151030749a27L, 0x242f148ab47f70L,
          0xe8a5bcf5585681L, 0x8b801845ed79baL },
        { 0xa4042fd3894ad1L, 0x82f781d2b88bc6L, 0x2d34cacbe4c397L,
          0x8731aeadd99c9fL, 0x0f95498ef1d382L, 0xcaba2e1dd0bbc9L,
          0x78889e954064e8L, 0x8cd9c9761a8ab9L }
    },
},
{
    {
        { 0xf31f53ffa0459eL, 0xf8742a1315cd6bL, 0xabe2f50ae64e97L,
          0xbd787419b9da48L, 0x4521a3351e526eL, 0xfa05935e10ba45L,
          0x5c947e1e8f903cL, 0x0aa47d15a754eeL },
        { 0xb2849efd814825L, 0x9c2a5d25c9968dL, 0x24dbb2604e634cL,
          0x33f3a4cdb38194L, 0xe04f609c8a2b6bL, 0xcaefd8eabbbfdbL,
          0x683119a404498bL, 0x24ab7a98b21cbdL }
    },
    {
        { 0x6f1326921fa2ddL, 0xd79e61cc10a4bcL, 0xac4b3ce4bd6d46L,
          0x52459b6bd3f37bL, 0xce0f0a3a396966L, 0x050d1d5a1ed488L,
          0x1b9c403e0b17faL, 0xee1abd004a2e66L },
        { 0x97065c35cf3e3bL, 0x6513d5fbe33441L, 0xcd3463479047aeL,
          0x45cbb1cfd22df1L, 0x7a173ae967b17cL, 0x75f5ba72223cdaL,
          0xe3d12dbefe0a73L, 0x3b7f94dfd7adcfL }
    },
    {
        { 0xd596a13f1e9b7dL, 0x04f5bdd6734e0cL, 0x18b694f8be163aL,
          0x15620c7d959fa3L, 0x65fc2c553d2a3bL, 0xd44a364c4d36f2L,
          0xc8b421f268ceabL, 0x564139abfe2bd4L },
        { 0xb52461019d4633L, 0x5ab3f886346934L, 0x96691fe9819422L,
          0xdfdec898b39b82L, 0x84b1c7997cfb27L, 0xe59a98d4d6d004L,
          0x5e5d0c612c350fL, 0xb431220d415774L }
    },
    {
        { 0x3d0ca736aae0a2L, 0x7b1991f48c2d8cL, 0x00ae8565cdae72L,
          0xdbb6ca0bd55128L, 0x3c2ab2a45c82bfL, 0xea5a55979545caL,
          0xeba9a26d5927d0L, 0xb52e40183257fcL },
        { 0x55ed517ca9650aL, 0xbdaa081e3ebff2L, 0x8cf7ce49f8831bL,
          0x1d0b5bd6e3b8d3L, 0xa314a9fd8fc869L, 0x07f2079b892babL,
          0xb700dbfa0cc9d9L, 0x7105a086dc0a39L }
    },
    {
        { 0x0c7e05d8c7d901L, 0xa7ff681af3182bL, 0xb88e3caf9a0d06L,
          0xfe20a12c343b7fL, 0x9f0257703251f9L, 0xf225dedc40c5ebL,
          0x50e0cecb208ea7L, 0x5b250f0e6eeb65L },
        { 0x807a1534806b6eL, 0xded120afa94139L, 0x237ddc749366fbL,
          0xdd3674e5a34bcbL, 0xef6cdff9c4a61dL, 0x036194bb2fb896L,
          0x38659539528cd9L, 0x0723c596936a52L }
    },
    {
        { 0x1f84cd5e17719dL, 0x545939bc73b394L, 0xefbf3c583e84e7L,
          0x6cc46f1f77fd66L, 0xa629f591383ab8L, 0x9177ffacd35cd2L,
          0x039187f9dd411bL, 0xa9cf1cf7b7eea8L },
        { 0xa3b105aac47e5dL, 0xa755bead0a9da4L, 0x50cfbae73da15eL,
          0x9456cbc60b628cL, 0x7ffc3629b7a910L, 0x30b5924cd6d6a4L,
          0x198629f0b04ab6L, 0xc74609c624dea9L }
    },
    {
        { 0x27d4d77af12fa6L, 0xdd8a216690aeb2L, 0xe48fc02fe24417L,
          0x1970403720e17eL, 0x95013fdce37b42L, 0x06817d2de4bd9bL,
          0xc5863e763d0ba2L, 0xa1bafc0a556f5dL },
        { 0xf28ec7b410a78aL, 0x0dcac420a01a63L, 0xfcd3fa4b5bce11L,
          0x054d7e5d278b89L, 0x5195db85ce49e3L, 0x4c0b1672c73d96L,
          0xd94307720a1bdbL, 0x66fa8b359c77a7L }
    },
    {
        { 0xb9e93aed7462feL, 0xbfe54b218dde4fL, 0xaabb5283dbb08eL,
          0x8c367020e5fc45L, 0x35028888e69be3L, 0x6d2efc1c12a11dL,
          0xfce5cebf265e30L, 0x58c8bb35742c7eL },
        { 0x32e89dcccf7fa0L, 0xa811f33dd020a4L, 0xa10d6205129fe5L,
          0x3841c88e4ed29bL, 0xf3303a9d8b1ea6L, 0xa9a0cad1781f58L,
          0x4502b388f3ef0bL, 0x2b7587e74c6d35L }
    },
},
{
    {
        { 0xc6eaea123ae7cdL, 0xa1884d473c0caaL, 0x901e76fef1ea88L,
          0xdb9935ca14269dL, 0xe8b2486947f1deL, 0x4ad56f4a657588L,
          0xe7680542913fb1L, 0x2abff5d37600daL },
        { 0xa814813a81a797L, 0x63e76a446acb69L, 0xb1038394ab8277L,
          0x587de349d8e759L, 0xdfaeb8dddf62dfL, 0x24fe1cf9239d49L,
          0x7de7409e130d1cL, 0x3ecfef9581d070L }
    },
    {
        { 0x8d177a0f87c72dL, 0xae7e5818c6d1deL, 0x0077b5f8cece85L,
          0x382483832d2187L, 0x49d8b156db2bd2L, 0xe9e5513c8d85b9L,
          0x63c410ce05c53fL, 0xceaf2fbd86f752L },
        { 0x0b432fe93806c5L, 0x18eb15d3d06c75L, 0xcaad82612cfc02L,
          0x581e0401e2d045L, 0xd573cb595edcfdL, 0xce71948dbc66e3L,
          0xcf68721acc14eaL, 0xf68bea26cac4dcL }
    },
    {
        { 0xd8576afcb74da2L, 0x8771c29c433f46L, 0x7315af6e2f5b8eL,
          0xc195481ba33928L, 0xb77dcc22fb1f94L, 0xcb3e57ca610f75L,
          0xeb2a92753907dfL, 0x916f14923eff95L },
        { 0xbb378e4b6cd291L, 0xa2a5e2b2f13ce1L, 0xa8a0e60bcd00b0L,
          0x5902741682b75aL, 0xa0882c93f65a77L, 0x2069f75c93cfffL,
          0x1ede40570c0cb9L, 0x13840c90d526c4L }
    },
    {
        { 0xdc2caaa03ced48L, 0x2079219a0315beL, 0xca493563b1f642L,
          0x0202dc7b0665f2L, 0xe5d6bbdb7a5238L, 0x36fbd5e26eab32L,
          0xb3988f1f5819b4L, 0x5b15dc84aa4d69L },
        { 0xa52feed54e5c24L, 0x927471be91a797L, 0xd119bfdd57f677L,
          0xde38f7b78e4c4fL, 0xa7af516b150bc3L, 0x403b21e26b76c2L,
          0x589067d92300dcL, 0x04e406a066802aL }
    },
    {
        { 0x28e7d09a9ca9bbL, 0xaa84fd5fccf4a0L, 0xdbe9fb8635b7edL,
          0x9ede3f5d56fc7cL, 0xa4b5031b01cb29L, 0x584299d7f93703L,
          0xbd28868b6fe825L, 0x1d385d48b9c2d9L },
        { 0x6606f4a822be80L, 0xb5a0165626d0fdL, 0x9920a2014568adL,
          0x7d430f41c6d174L, 0xc243e16e02e9e9L, 0x367f1d2a6bd649L,
          0x693910071b8c36L, 0x2ede1314de2984L }
    },
    {
        { 0xdc781875beec32L, 0x1fff0cca525ff4L, 0x6e86425676df34L,
          0x2b4e8a63f638e1L, 0xc4991d29b1e59fL, 0x399d0011589717L,
          0x406464ebe041cdL, 0x901cb3d9e65bb0L },
        { 0xf5f4572fb42307L, 0xf81b3b0f1b7307L, 0x8fb695cf2094d1L,
          0x7db4792db56f7bL, 0x36836d55a794e0L, 0x2da477b09bc879L,
          0x1cdfadb1887c40L, 0x65dc6c2f2699b6L }
    },
    {
        { 0x36f9f214737972L, 0x48f0c8b7a387b0L, 0xa156ed339a1d24L,
          0x375293a0fed268L, 0xf679f487ff75cbL, 0xd15a00f1cc9e62L,
          0x92a7dc722c3877L, 0xe9870636fb0ed4L },
        { 0xfd8e59c16f5f3cL, 0x375732eaeeb48eL, 0x2dd9213ca1ab42L,
          0xcb062099ffcceaL, 0xfc611f6b23edfdL, 0x271634999b060eL,
          0xb938b5d820de8aL, 0x138f6e7eb49a32L }
    },
    {
        { 0x7feda63e485f70L, 0x646380aeb27b2cL, 0xcf8fe32c4511c7L,
          0x2c68e1eff9406aL, 0xa9f2fd920b6020L, 0x1c98fc63b3e465L,
          0xb8dac3593e53aaL, 0x2fb47b6a750e96L },
        { 0xea373ef1950bb3L, 0x81566944ac7aecL, 0x8d6b3c2b55b931L,
          0x5d13f2db62ef7dL, 0x4647f2aab9182bL, 0x8f56c5a33bf07cL,
          0xc5ab284b35a221L, 0x0747ab75a46a6bL }
    },
},
{
    {
        { 0x5b9236c86b85c5L, 0x5967a0dc482448L, 0x397c9557df6ae0L,
          0xf83ee1c5378f2bL, 0xf82df656e05dd1L, 0x4c424f619d7c8bL,
          0xa612550a6d5f2aL, 0xfe8482a63c3ebfL },
        { 0xcb8d4030142c82L, 0x08b06623679e6cL, 0x3ea51463eca5eeL,
          0x089eb3b1370500L, 0xcbfb19c5a0d306L, 0x2f6858842a65bbL,
          0xe3e1db5e51e119L, 0x2c150e7110895eL }
    },
    {
        { 0xf323488f6d4c4cL, 0x5fc931f63b87e2L, 0x8867da035c759fL,
          0xb6f1eff9746d4cL, 0x8a8172d990be0aL, 0x1113eee5c407b4L,
          0xd80dacf378ed8aL, 0x99b57cf3fa7fd1L },
        { 0xf5bb6d95176405L, 0x6b8963a92e83b5L, 0xac55b6b8a7ef8dL,
          0xe73fa126c1fbf0L, 0xdb3756060148dfL, 0x72f1a98f3f1fbaL,
          0x1f71d0aea550f2L, 0xc3ea4f09544a87L }
    },
    {
        { 0x5b09da24322bf3L, 0x2a573d561264e1L, 0x93cb2e1803acc4L,
          0x397b4fbe502fc6L, 0xddfb21239e0ebcL, 0xeccd8f5bbcbc57L,
          0x49d3bed4663788L, 0x37192aa1218df9L },
        { 0x8a05bc92ffa3c6L, 0xc38c28123ebf4dL, 0xc80d547fe343a8L,
          0xa8d5a5b6c63516L, 0xc5d8ce18d8fa6bL, 0xeb5e87224a87c0L,
          0x9806e9e75bfa23L, 0x11f0889689469aL }
    },
    {
        { 0x81005f68e75666L, 0xb84d861d349505L, 0xe0832829f321eaL,
          0xb751d7acfa33a1L, 0x793cf6f067c550L, 0x073a6b21027e56L,
          0x53f40ee66a6012L, 0x70bfaa8c210fa9L },
        { 0x1518e39e4b5998L, 0x8f0b53024b8d9cL, 0xd91c281afdf923L,
          0xc5cfb2824e3f69L, 0x63a529a870871fL, 0x3d3e8872128dadL,
          0xed658dccb30cceL, 0xf9373b9afb7baeL }
    },
    {
        { 0x22d4dbede58ed2L, 0x4fefc1d03f8789L, 0x6b0a1fe344817fL,
          0x96bef40a56b0b2L, 0x32684eeda249faL, 0x8298864524a91bL,
          0xa958baf0c736a1L, 0xd033a7def2f3e5L },
        { 0x5be3edc43f4d6aL, 0x326a39d9c89abbL, 0x90c44f755d997aL,
          0x20581066e966c2L, 0xdbae4906548038L, 0xac7bc97d473fc1L,
          0xb34488b4b2603aL, 0x27aea275e9bb98L }
    },
    {
        { 0xa59e7281b88773L, 0xe2f05d40c241f6L, 0xa56229e4e75749L,
          0x8f00c0b1b10705L, 0x855994619394d3L, 0x0d7e352aaf5e32L,
          0x526c462787b8eaL, 0x89297d9a179d48L },
        { 0xeff17e6ef43892L, 0x17091eb221f841L, 0x82f5eb34a4b848L,
          0x6bea4778eb7b76L, 0x21f227176c536cL, 0xd9ef2c896c81bbL,
          0x7c2754654bf4d3L, 0x9dd4662d7c28c8L }
    },
    {
        { 0xe7fff0020e1a6bL, 0x26a35c6a08d467L, 0xb3c773d3248c91L,
          0xa646615ba7d935L, 0xa91f453b0d26faL, 0xdcf9c3460c6d32L,
          0x63668619e3e3dcL, 0x3012813f30f3e2L },
        { 0xac6623dc2fc61aL, 0x108dc252bfd2ffL, 0xd7f5c0d231d6eaL,
          0xa904f9aad1107eL, 0x46941c20d1e9c8L, 0xe5b6451c810cf2L,
          0xaba8e674f511d1L, 0x5b4b94f08373feL }
    },
    {
        { 0x002d4e2849c230L, 0x9bed0efd8ba391L, 0x745e0c0828e319L,
          0xcd40907ca58de2L, 0x2c87ab11abaa4aL, 0x3c17a97db64391L,
          0x36b184e86c72d2L, 0xb03d202485f7aaL },
        { 0x2b6b79bde24abaL, 0xdcb78542325fb2L, 0xf5d1db966ebae2L,
          0x35a4d5b903840aL, 0x7afeb09190e9daL, 0x1818f6a35c1792L,
          0x90091fa3faa269L, 0xc4ccff62570235L }
    },
},
{
    {
        { 0xa177619ec85940L, 0xfca24db7ef7eeeL, 0xb2450f37a90c11L,
          0x29d256ddbf4f85L, 0x920c8d051316c3L, 0x2f7f7ba04474daL,
          0x308117f2ec9a0bL, 0xd0a231ad0d2085L },
        { 0xf3288fc7ab641dL, 0xc68bade9f4fa32L, 0x768f014bbf8253L,
          0x5eff260c0a33f0L, 0xc71b4536bb93ceL, 0xa71d045680697fL,
          0xb62444cce72bc3L, 0x11f03e8d1379f3L }
    },
    {
        { 0x1f54789c16df92L, 0x874c642e3ed142L, 0x6699f60fa2a9f1L,
          0xbd1b8d33fecfc1L, 0x59682d58a3d953L, 0xf17c0214a36b81L,
          0xeb9621d181a666L, 0x7c2c3ab3cf1ad8L },
        { 0xe6888c3e529f7cL, 0x197b66ab355315L, 0x63b558a83e31acL,
          0x4aa7bc5891c68eL, 0xc17d989592e360L, 0xc750a291363666L,
          0x0d534704909ac0L, 0xd6d02724594a10L }
    },
    {
        { 0x35c541b3fbb635L, 0x50016d05982afaL, 0x58ebce496b0ca0L,
          0xb940027577ea56L, 0xf29d305e38480fL, 0x43705b0ebd6a2cL,
          0x0e4acdae90c639L, 0xbe94a29f56e05eL },
        { 0xc61f4a030659adL, 0x39074adc402211L, 0xfe0d8d551b621dL,
          0x2d02e8dd1d5222L, 0x05ece3c46c2683L, 0xf70705ac689d41L,
          0xe3caf444d837bfL, 0xfda058475ba6d0L }
    },
    {
        { 0x1098163cb7d458L, 0x12b645ff5ba834L, 0x70a318128af72cL,
          0x5f4727ef32e5ddL, 0x7cbae1510a21b4L, 0xa80bf806785389L,
          0x9827402b8f93b7L, 0xe385f8208349daL },
        { 0x2d054619589f6eL, 0x6aa5b26e7c0191L, 0xe79ae12bd5574dL,
          0x5d13f914148e61L, 0x7b2be0f13716ffL, 0x82b0fe680bb81fL,
          0x697633c3e2569cL, 0x6c1f083873f8b3L }
    },
    {
        { 0x6e26d850be1674L, 0xe4e47f6ab8044fL, 0xfdf46e882fc434L,
          0x639ae2cc89cadcL, 0x2244a524b85bdcL, 0xb1e4790b7cf4eaL,
          0x51dce037e0bb8fL, 0xdd143352716ceeL },
        { 0x1c049b48e8841dL, 0x6bf26dcb97c621L, 0x21d6255ba01178L,
          0x477258a8e4f0e4L, 0xf5e437e68f8ef1L, 0xd118fbc8b03e1eL,
          0x3d6bc51e1c91b3L, 0xa259486d5b6907L }
    },
    {
        { 0x4159cfc7b6f5dcL, 0x05a52b3493694aL, 0xeeb511c83b8883L,
          0x19d79e42b06400L, 0x8e503a2738f37eL, 0xa30e5795a94ad9L,
          0x3981c75262618dL, 0x06b6c692dcba19L },
        { 0xd7242ee4d1b051L, 0x6274ccb3b350c4L, 0x66df0bbf540019L,
          0x4d66be65ae12d5L, 0xcea29601049cbaL, 0x40473398df84b3L,
          0x7d6c96b75a31c8L, 0xbb80159874174cL }
    },
    {
        { 0xf0f7be059f1aa4L, 0x798f39adcff451L, 0x96763ff8014e1eL,
          0x03987a809cc5ecL, 0x4919656893650aL, 0x92e8eef75e24dfL,
          0x54e97cde89d639L, 0x8081d067682cc0L },
        { 0xb9ef41aa8ceb71L, 0xb8173a4a4d7aaaL, 0x93d81b1c54ee10L,
          0xabe180570a445aL, 0xac0ff9764d569dL, 0x86946b23e570beL,
          0x8e11dd24180641L, 0x3d0b33c99f67dcL }
    },
    {
        { 0x2c9637e48bf5a4L, 0x9fdec19ccaf112L, 0xe5cde9d5c42023L,
          0x9869620878f0ccL, 0xcf970a21fe6ebaL, 0x1df5ec854e678bL,
          0x4667f0128d00ddL, 0xfa7260db0b3fa8L },
        { 0x6bd2895b34239bL, 0x04c8bc52d2a50dL, 0x14e55ef6cb23e2L,
          0x6440c273a278d5L, 0xf4b12e32193046L, 0x46adf645dd4c08L,
          0x70e29984656e8cL, 0xe7b36eae4acd44L }
    },
},
{
    {
        { 0xea64a5716cf664L, 0x8497ee426fd357L, 0x44d94b4814e851L,
          0xf4aac225a6a2cfL, 0x947b30980c301fL, 0xf390ba17865383L,
          0x16c4fc6d1773d3L, 0x61b98146227220L },
        { 0x07dd03a1dd0270L, 0x290ca820f160dfL, 0x8f2205444ba955L,
          0x4e85e450b6f1b3L, 0xfd73ce9ad78089L, 0x67c12702f2cb0eL,
          0xa7de0d7ee33a61L, 0x6a811cc6553261L }
    },
    {
        { 0x5ef05742d0a427L, 0xe8d2e95220a341L, 0xdd28cbf8044886L,
          0xdad7b4ba1aa58bL, 0xb28f3738ec901bL, 0x1841a935bbe3dbL,
          0x8fd7cd1a075feeL, 0x93b603fc0d3cddL },
        { 0xca54fd55edd859L, 0xa4cb05f64ed687L, 0x3138668ed1a3d7L,
          0x1224fdaee32be5L, 0xf1f532bc80aeb3L, 0xa4f65d0e8d4d69L,
          0xc697a015905fe5L, 0x514da7a6690ce4L }
    },
    {
        { 0xc7b9af83de4a55L, 0xc79bad7b318d93L, 0x1808071f5b1c83L,
          0x92112efb965b16L, 0x655ab387bb740aL, 0x53dbc8b384ff87L,
          0xd153c2872dc6f2L, 0x2ec20e199c7819L },
        { 0x65e46ea3b854b5L, 0x272d5aec711db5L, 0xfd1bb5326e19e8L,
          0x33280b83dc0665L, 0x95b986eb8f1c4aL, 0xa671fc4a685c4aL,
          0xa03cbd583bdbbfL, 0xd329402ab77544L }
    },
    {
        { 0x40fa6518e62b35L, 0x3913b11f9e55a6L, 0x4e8089b5270a41L,
          0x565f52a80d1886L, 0x93b5f05512749bL, 0x35c869c141c547L,
          0x9a44a1af86717fL, 0x2b9984b9c2b2cbL },
        { 0x61fb6074952322L, 0x2d4072f7af1464L, 0x9b2fa8c600eb30L,
          0x6071fb7f10668eL, 0x27cc24d90634caL, 0x3875bc2471d32bL,
          0x678590ba11210cL, 0x352b447fcc5a9aL }
    },
    {
        { 0x795d5415fa3200L, 0xadaa557a92949fL, 0x42fff063cc88c4L,
          0x26d683171b68a5L, 0x3286549e67ad8cL, 0x5bf636386396b2L,
          0x41229b6e12c8eaL, 0x05320c9748952eL },
        { 0xae36b63900b460L, 0x9354ff2f2b6affL, 0x10b810b065ee0cL,
          0x4d6925fcc8bb38L, 0x31c03fd7a22f14L, 0x76b7f4457544e8L,
          0x3a9123cc0eed26L, 0x77acd67e0cd1ccL }
    },
    {
        { 0x2e9053007ec527L, 0x32388ef62937cfL, 0xa445389e229188L,
          0xa44b68e33bcebeL, 0x5a8722e4c4e701L, 0xfd066e8cf07e41L,
          0xa3c1a4f95fab62L, 0xb4d6a1be542f24L },
        { 0xe6a92e4af6c9b5L, 0x9452484c83d61dL, 0x422b55b0062276L,
          0x261973a5279688L, 0xde8be263999fb2L, 0x64e96287b029caL,
          0xd8edfaa06897d4L, 0x408319c6955511L }
    },
    {
        { 0xff6baed50a5632L, 0x922b7d05c5885aL, 0xdf0f3b31b45864L,
          0x27e49c0c04340eL, 0x618c566122c447L, 0x7863a38eafee7eL,
          0x7143affb828cb0L, 0x51fcf4cf9d054eL },
        { 0xc4a4b3127f5e09L, 0x021f47a90be2bdL, 0x1a060197ab956dL,
          0xe77fa1586ea86bL, 0x9ccde87d550ef3L, 0x7dee53a6532654L,
          0x8b4f060e826387L, 0xda38637ad077b5L }
    },
    {
        { 0xbc901b30e9fac8L, 0xfa082046fb2a2aL, 0x92f68ab5e04efcL,
          0x184a30a9ac12d0L, 0x1aa11aab25d479L, 0x8bc5f4c0f03161L,
          0x7e3a083cfc8817L, 0x84d9355597f93fL },
        { 0xc014478239abc6L, 0xb226b098d37b04L, 0xb056942f575789L,
          0x816b95aba745ebL, 0x2a49d39b98ddb6L, 0xc41ca26291af81L,
          0xb3afe99ab26347L, 0x59c31bc604b638L }
    },
},
{
    {
        { 0xa16a8b9c42befdL, 0x731c9c92052f00L, 0x1ad49b41f5dfa0L,
          0x7a289e3bffce36L, 0x868fac00c79cf1L, 0x6d6d28486721abL,
          0x590f928e726c94L, 0x0e802cb51f3841L },
        { 0x6a6a57a0b694bcL, 0xb9bb0cd8120fb8L, 0xad96ac79c05826L,
          0x294da8c7768df0L, 0xfe32311b56c6c6L, 0x291c2c6ae8d050L,
          0x1c765e7e7db4c9L, 0xe058298d65f9f7L }
    },
    {
        { 0x4bfa85b7e8d345L, 0xa04ef95de1dfc8L, 0xb5f7f21324ace3L,
          0x4b350a1574b14aL, 0x11436bff8e5c8dL, 0x1c789f97642369L,
          0xeb5e335fb623ceL, 0x9deacd2442d562L },
        { 0x4ff989f531ee71L, 0x43e2c49aacb52aL, 0xa76319885bfadcL,
          0x08b6d5cd0161a0L, 0x010e3fa541f197L, 0x83a589e3279a16L,
          0xf0991376309f9bL, 0x07c093bf1cea10L }
    },
    {
        { 0x1ce3f0f33d2192L, 0x07b559ac37ce73L, 0xaa2ad38207be27L,
          0x84f053b7ed93deL, 0xbc5c7973b98a4bL, 0xc92346163aa9b9L,
          0x807cc16231a10cL, 0x8ffdf57a061209L },
        { 0xa9ca741497070fL, 0xf608ec9d113b3aL, 0x51327268d0384dL,
          0x96686acf5ec307L, 0x437bbbd71c4665L, 0xdef09d57c379caL,
          0xf8be033621747cL, 0x2775b378ae8047L }
    },
    {
        { 0x4009798b2c4fc2L, 0x148d7d1203772eL, 0x9d9392df8423fbL,
          0xa5bd72eaf8cef4L, 0x579d58d4380b53L, 0x2ff88f18c39d24L,
          0x9ca2fbc5706466L, 0xb42987d1e56af2L },
        { 0xcc2556e5d94ea8L, 0x4e5c2b35369d76L, 0x5de35742a94f9cL,
          0x8d068c95cb4145L, 0x4d553ff51bfcbfL, 0x3ab71648a23fceL,
          0xc9cb3a9d0fa7f3L, 0xf81209bed9ced1L }
    },
    {
        { 0xde7356ee5b66f5L, 0x7b2bf1ae8a25e0L, 0x09a444a2c9b725L,
          0xfd8a2f44906c55L, 0x409cc8082514f3L, 0x47e009928999a9L,
          0x0a582a66a312f4L, 0xf7946f8f6723deL },
        { 0xa55f6ba92d8affL, 0xb62c3c8a544b1cL, 0xa1d14115c16a94L,
          0xc3783192ad5e71L, 0x13d784706b1dd6L, 0x99005f8ee7ff55L,
          0xfb5ea3f8a1e7d8L, 0xdc7f53cb4cac39L }
    },
    {
        { 0x482abaf36e3794L, 0xc23e9e5c74684fL, 0x4544cf6f1629beL,
          0xd8a8ee52f40374L, 0x2eea87ff433bdbL, 0x489a99cae9990eL,
          0xefc131e54b23b6L, 0x25fe6998600270L },
        { 0x03d2d9ec059a7eL, 0xa6445b56979c3cL, 0x491a10c9bfbceaL,
          0x15b5974e937af1L, 0x4be8002797c7fcL, 0xbed8a49fedcfeeL,
          0x35751cea9e0691L, 0xe9a9fa39ef5982L }
    },
    {
        { 0xeffeaca3065de7L, 0x841d544ac4d4e2L, 0x8144679caf199fL,
          0x98cf4f9443967aL, 0x8cd57f4f33183cL, 0x390832ac1b15ebL,
          0xc4b1feaa53b500L, 0xd762a10dff24b5L },
        { 0xccd3eedb0ee2a9L, 0xa6dd4a9362d485L, 0xeb4ff26f1d047aL,
          0xc0771fd23860fcL, 0xdbb4e394b64114L, 0x2ff3f244d29b29L,
          0x9cac005387b365L, 0x05b7aa6de5994aL }
    },
    {
        { 0x5e71752c03dd63L, 0xad10fe9bc74687L, 0x51a5b0c54c76abL,
          0x763fd501f586d4L, 0xc7bd5ce816048bL, 0x8fc83d23f744dcL,
          0x0561802109df9aL, 0x18fb01fccf0e43L },
        { 0xe4606fc038ab23L, 0x5878f1fa664c98L, 0x3aedbbd5da7356L,
          0x3c578f5516746aL, 0x259477f1a17210L, 0xc7a869d028248fL,
          0x6517a6148cbf95L, 0xbc5f91d3d04d47L }
    },
},
{
    {
        { 0x15fd9a9083ca53L, 0x1161da02697ca6L, 0xf516af356b676cL,
          0x8a420d575eec13L, 0x72d67421a9526bL, 0x8d8c29e76b463fL,
          0x38a4f588815627L, 0xf7e528be0650f9L },
        { 0x2cfa78e382edcaL, 0x638d183c4ad83cL, 0x96d3b9de4a0119L,
          0x5769ccba7c1101L, 0xc3b3b792b8d04aL, 0x96212f64951bdeL,
          0xad7905a481161eL, 0x8fd676241c5edfL }
    },
    {
        { 0xf7b063539d6cdeL, 0x69d0549115a84aL, 0x4a976c6cbd9fe4L,
          0xc92953f950ff96L, 0x1d7f0fe654d127L, 0x7293870da0f75dL,
          0x7bb3652cf2277fL, 0x64798c9834484fL },
        { 0xb94d8bfac3a76cL, 0xf5721a97ff776bL, 0x23a6e9f2722e31L,
          0xe9da9969a5c034L, 0xb9bbf83456ebc3L, 0x239f58a96956a4L,
          0x8b75beb18b7f00L, 0x6c2b5b8a51cb97L }
    },
    {
        { 0x78b1c627eb41f3L, 0x0638fcf17c4352L, 0x939edd80c5709cL,
          0x0a8dfc3edc906cL, 0x3942f47efb01edL, 0x4c8275749986feL,
          0x792545c4dffa57L, 0xeee68836c3ff26L },
        { 0x824d08e12b1218L, 0x515a478902457fL, 0xc70cc9cbae55b3L,
          0x1240737bcef9d4L, 0xf22e6162f9db7fL, 0x98c4f0291f8da2L,
          0xa89219cafaaa67L, 0xf35fd87e7d27e2L }
    },
    {
        { 0x19b0cd701b80d0L, 0x3d7e29df9aebd1L, 0xd39c9ca0477cbcL,
          0xac0f6155ff0d3dL, 0x8a51993520fd01L, 0x508ff54b22d6fbL,
          0x8786c47318d3abL, 0x4312c464a683f8L },
        { 0x73b1d3995359f6L, 0x0d94fa5963011eL, 0x5723af29bfe83eL,
          0xafa90016841df3L, 0x791e92ab7c498aL, 0xbc931ad7ea4253L,
          0x438e016b783c06L, 0x1347db22ca662bL }
    },
    {
        { 0x41df37dfbaa861L, 0x98ecb23329e4deL, 0xdaf1560507e018L,
          0xa902269b088e32L, 0xad898a5e4cab2fL, 0xd84e9ed02c1e1bL,
          0xc20a5d58488af3L, 0xc7165af6cc77c6L },
        { 0x8526f3adeb7461L, 0x03577b14a2d332L, 0x28e469de4760b5L,
          0x442c7f9b276266L, 0x90d5c77f9c90faL, 0x7aa87163e211bdL,
          0x56d8ff05decfd6L, 0xa204b56ee23e6eL }
    },
    {
        { 0x2e4374e4aceafcL, 0x978743b6fcd5e5L, 0xa0f6345c4855caL,
          0x9bc7e4fe98074bL, 0x3835d57c33d08aL, 0xeec7c8b6f00566L,
          0x71628a21acf55cL, 0x5da375097fb19eL },
        { 0x6904a8e01a7125L, 0xad33c85e6e3780L, 0x1702928c19f94aL,
          0xb424ff27c04b3dL, 0xb212e3919e2ba3L, 0x4cca8e8c9af4c9L,
          0x98ab7aefd9bf0eL, 0x21d245d9799db5L }
    },
    {
        { 0x6b034dcec08806L, 0xfd763f2b40f2d9L, 0x5e16de029cb906L,
          0x02b70148a0e16aL, 0x463c8eee071e12L, 0x644728125ad509L,
          0x9ee6f2ddc0e07aL, 0x188895c68d4d97L },
        { 0x092fff3b27f971L, 0xb3c159fc9b7722L, 0xe27d8ff3cae42dL,
          0xf8a5ed6e87071dL, 0x318388f607ebd2L, 0x924967b53486f1L,
          0x77304947c46e1fL, 0xf279c60f21d196L }
    },
    {
        { 0xef2bc0384f3201L, 0xf8750c71f94c51L, 0xbaa4f5a986ec65L,
          0x6f8a5de2732a33L, 0x0f13d80299e365L, 0x2709530e85261fL,
          0x097d922f527d56L, 0x4969687be1f3f8L },
        { 0x9f3f5043e1708dL, 0xac67b874aa4be4L, 0x75fb042320a87eL,
          0xa361ad36e2cad6L, 0xcb01470203e9f6L, 0xe3807b7c9b76c6L,
          0xf086833b907c09L, 0xe9bed3c7e85a01L }
    },
},
{
    {
        { 0xa7ea98991780c7L, 0x04e4eccd2476b6L, 0x0af9f58c494b68L,
          0xe0f269fdee64fdL, 0x85a61f6021bd26L, 0xc265c35b5d284bL,
          0x58755ea3775afdL, 0x617f1742ecf2c6L },
        { 0x50109e25ec556aL, 0x235366bfd57e39L, 0x7b3c97644b6b2eL,
          0xf7f9e82b2b7b9cL, 0xb6196ab0ec6409L, 0x88f1d160a20d9eL,
          0xe3be3b4586f761L, 0x9983c26e26395dL }
    },
    {
        { 0x1d7605c6909ee2L, 0xfc4d970995ec8aL, 0x2d82e9dcf2b361L,
          0x07f0ef61225f55L, 0xa240c13aee9c55L, 0xd449d1e5627b54L,
          0x07164a73a44575L, 0x61a15fdbd4bd71L },
        { 0x30696b9d3a9fe4L, 0x68308c77e7e326L, 0x3ac222bce0b8c8L,
          0x83ee319304db8eL, 0xeca503b5e5db0bL, 0x78a8dceb1c6539L,
          0x4a8b05e2d256bcL, 0xa1c3cb8bd9fd57L }
    },
    {
        { 0x5685531d95aa96L, 0xc6f11746bd51ffL, 0xb38308ac9c2343L,
          0x52ee64a2921841L, 0x60809c478f3b01L, 0xe297a99ae403acL,
          0x7edc18fcb09a5bL, 0x4808bcb81ac92aL },
        { 0x3ec1bb234dc89aL, 0x1e8b42e4e39da5L, 0xde67d5ee526486L,
          0x237654876f0684L, 0x0a583bd285a3ddL, 0x3d8b87dfe9b009L,
          0x45bd7360413979L, 0xb5d5f9038a727fL }
    },
    {
        { 0x7b8820f4bde3eeL, 0xea712ef24d5170L, 0x517f88cdf6ec7bL,
          0xb15cecf983ea9aL, 0x9eeee4431a4592L, 0x786c784ebb013eL,
          0x2f06cb31f4e15dL, 0x5603fd84f4fda1L },
        { 0xf6790e99e1321fL, 0x274c66a74a4c09L, 0xa4b70b49a41a4eL,
          0x7700bddada5157L, 0xe54a60d51be8dcL, 0xfaf92761a477e0L,
          0x6661c72b027eacL, 0x50e2340280b917L }
    },
    {
        { 0x635f40f96ec123L, 0x4a331337a766a4L, 0x9ce4416b935587L,
          0xbb6e1f595d97e4L, 0x26147239d4197dL, 0xabd4478490e896L,
          0xf6a1b2a8bba895L, 0x401fa405e27a45L },
        { 0x7354ba50620900L, 0xc443a29385678bL, 0x48aba1053cf5faL,
          0xd67e723bbe152dL, 0x4b858e02a63d68L, 0x174e1ee72be4eeL,
          0xad0fbb39ab8d46L, 0xa0fdffbce17dd7L }
    },
    {
        { 0xa1ea3259c46fd8L, 0xeca122e9fb96efL, 0xf9074a26767acdL,
          0x9b004a22787082L, 0x389f8077f3ba8eL, 0x6463de90d5aabeL,
          0xf30ceaab090585L, 0x71b31e85634ab8L },
        { 0x0dee65caf02aedL, 0x506886e20ac252L, 0x0665f7886b8a59L,
          0xb9b784df2bb328L, 0x46e443adc6b089L, 0x3d5de1966c27fdL,
          0x0419265f0fde70L, 0xed946122b5c034L }
    },
    {
        { 0x5a52ad213b0056L, 0x9fbeb92b909ee3L, 0xb42ba18bdaab08L,
          0xec127c4ffc8a77L, 0xc6d2985fda906aL, 0x5355547994bbe7L,
          0xa7470c09cdfd62L, 0x31a3971d2e675aL },
        { 0x8d8311ccc8b356L, 0xabb0bf801b4372L, 0x33c1cad0294566L,
          0xe2e649ce07b672L, 0x9084d882ae3284L, 0x7a90d4c1835ce2L,
          0xb4d1cd5809d44cL, 0x78227149f0528fL }
    },
    {
        { 0xca884cfbf5844bL, 0x9dd05c48524cf9L, 0xdbffa1936ba889L,
          0xef94fdd29e7666L, 0x358f81b3eaf48fL, 0x96734d51530d56L,
          0x378b2d14adf9e5L, 0x2f850464731f61L },
        { 0xd6ae90599dcb83L, 0xa4f89e06199239L, 0x64052498f0f958L,
          0x2866d99cc27707L, 0x64681a2f551c0fL, 0x2c7b0d04c37080L,
          0x218925b00ac301L, 0x8d57fb354df895L }
    },
},
{
    {
        { 0xdaebde0809c8d7L, 0x58c761c0e95ea1L, 0xbd9965000ae5e2L,
          0x6117a85cd51acdL, 0xc4424d87c55d56L, 0xe9b1ddedfbeeafL,
          0xda98bb50db4791L, 0xff3a5a63fca108L },
        { 0x172fb8e5ccbea1L, 0x9fe12a7a9f6cc9L, 0x1de4b0b8967ce2L,
          0xc1ab60f671dbc6L, 0x338385a5dedcdaL, 0x647a4203a043feL,
          0xe9abc6428ebc89L, 0xc357ff003ba3c8L }
    },
    {
        { 0x37061e7de39ebdL, 0xebb91352be567aL, 0xa9a6f6bd6bb80aL,
          0x039345d99f0ba2L, 0x215494e98bbf47L, 0xf2cb7a4a2a1ccbL,
          0xf51aa1037f67c9L, 0xd29c85c17fff71L },
        { 0x8d4e4f24d30b87L, 0x20fdf5593a8309L, 0x9b9f9cf757075cL,
          0x09142adcd70101L, 0x901d0ee766ca55L, 0x6a5d86a32e418bL,
          0x550ad92d7fcaecL, 0x64e8818d91b26eL }
    },
    {
        { 0x5cea0f747e5ee5L, 0x8ca1d31be99699L, 0x52db8465c136c7L,
          0x8cecb3890e0d74L, 0xb8efe9dede2ad8L, 0x18d6ff8f17ade8L,
          0xd2227352d66c20L, 0xc46593ef2005fdL },
        { 0xe5ebe6ff7141e1L, 0xc968315e0126f2L, 0x95adc731cb91b6L,
          0x753b54c38a6003L, 0xa6141254230a61L, 0x23ac6eb559feceL,
          0x9816b603865c23L, 0x567014e543a570L }
    },
    {
        { 0xd46091ddd2b71fL, 0x3999a5d97d24ffL, 0xce2a4f11ecff3cL,
          0xab2687c581c6f0L, 0xa9fb2ebcba70b4L, 0x6fde35642093e1L,
          0x00253ecaee724aL, 0xa08ce3c2b81bddL },
        { 0xa251238935a2b3L, 0x8cae1d4584f750L, 0x011469e988a219L,
          0x61f7ed35a6a50eL, 0xe13ebaa01fcebdL, 0x794b97631d8867L,
          0xf25755ccda32e7L, 0x368a97b4564cd1L }
    },
    {
        { 0x0d22224aa3397bL, 0x1dbb3e638066dbL, 0xfe0b5ee0ce8e32L,
          0x09c17c87bab4dcL, 0x5cc65ddf188b64L, 0x74c4abf211b5faL,
          0xdcc17b7ab0ba86L, 0xfbdf46fa535501L },
        { 0x4775087aca569eL, 0x6575f9006a1718L, 0xb5c45a9b94de93L,
          0x0fc80068497171L, 0x775d965489f7abL, 0x8775b58f5c0c89L,
          0x05d4e201a06254L, 0x8cab349b6d73a5L }
    },
    {
        { 0xca7816339465b0L, 0x3ef914814498fdL, 0x9ca1f346255c11L,
          0x389fd15b7f38f1L, 0xdac2089354b8f3L, 0x82d07fca840a70L,
          0xf53fd731dd483aL, 0xa6e4eae1590578L },
        { 0x7bf65af3c01b77L, 0x27542f3a75c982L, 0xc5bd947716cfceL,
          0xba5fe76884b9e7L, 0x39bae14d55725dL, 0x982f64efae0eabL,
          0xcfae6627a5293aL, 0x22a25a1d60f464L }
    },
    {
        { 0x74caecc7dd5e16L, 0x23678a2ce7bca3L, 0x467393257f1ba1L,
          0x4eb9948a4c1697L, 0x5d400e8eaba18dL, 0x128d1c89807871L,
          0x78f9627bff38a6L, 0xf80b813a39d4ccL },
        { 0x8aeefa031d3aadL, 0x504219927db664L, 0x244fc694cb6383L,
          0x319047772192a3L, 0xcc86075bbfb57bL, 0xbae3a134451511L,
          0x16cf416f6174f0L, 0xb343cc0d376813L }
    },
    {
        { 0x31ac9b9d1824b7L, 0x6282260ec8f61aL, 0xbbeb9f8c781765L,
          0x06ab5c02d110daL, 0xd583e2247146b8L, 0x79a16084100d05L,
          0x16dbbb4f0a5c95L, 0xfe2af1de331667L },
        { 0x26f0364af8710eL, 0x1cb8c91eec08feL, 0x436bce61d95e9fL,
          0xfe9050c57944a0L, 0x5f45acf07b626bL, 0x48dc93f9cf1276L,
          0x4491371a05bfb7L, 0x51063044bcf785L }
    },
},
{
    {
        { 0xac2e294ed0b3b6L, 0x5c5ade6671637bL, 0x2f289ce1140677L,
          0xaf446e2754eb53L, 0x70911b720421adL, 0x4b73836e0b7556L,
          0xcadf1042a97827L, 0x4824e498005bc6L },
        { 0xb0eeccd937c28aL, 0x1ce061d0c3ee97L, 0xcb076319f33faaL,
          0x9980bf4aea66dcL, 0x2bd0755d111d98L, 0x43feaf67fe4de0L,
          0xe76fb80b077b2fL, 0x227dc9f5793b04L }
    },
    {
        { 0xea24ae514f49baL, 0xbc39ea611436e7L, 0x9d7fed278485d8L,
          0xb6ef00cdf8b131L, 0x0237b4bfdbc7afL, 0x08745b564ccd27L,
          0xaf8595dafc5a76L, 0x43657af29f5500L },
        { 0x300718348470f8L, 0x51f91fd640fd53L, 0x859c807be15512L,
          0x7d1a474ab3e9c5L, 0x5d714d981553e5L, 0x07573436f62310L,
          0xedc5be06b02a62L, 0x5a4b9b7ea47832L }
    },
    {
        { 0x03e0a24e93dbb3L, 0x25841dccadc884L, 0xabc1a818d10ad5L,
          0x207e38a2042dddL, 0x7fffbdbfeba8d8L, 0x74efebba3ec9b5L,
          0x0bc39ca0b40a9fL, 0x69ee9c90267febL },
        { 0xd402facbc62919L, 0xe9f8fc11cf53c6L, 0xe76fa5a7cc7d81L,
          0x4f2d87696bb19dL, 0xd4fb7f9adc67c7L, 0x40621d596702dcL,
          0x5b6a98e438f6c5L, 0xa7c64def1a1036L }
    },
    {
        { 0x84c5e809a092c7L, 0x9e40e0a11c22b7L, 0x820a091d06c99bL,
          0x45fdc77eecca8fL, 0xfe1b8a35794f16L, 0x31f7e5b4ce3d6dL,
          0xfd5e01082c74c8L, 0xfdabf30c1f6f7dL },
        { 0xbfa6017b9248a0L, 0xe898d30546b941L, 0x878c492207ff65L,
          0xbf22e8db874e64L, 0x43fdb1b53a547eL, 0xb66deda5fbd464L,
          0x59127a6c7ae1b5L, 0xa4636466a7515aL }
    },
    {
        { 0x22c4e66de9ab2eL, 0xfaf60c20203c58L, 0xed2d7bf0d5c5edL,
          0xdbc16fe4ca0f19L, 0x54e8ef6465b979L, 0xe2d64b1a310ef9L,
          0xa0f2c953778636L, 0xf3b4aa4281883bL },
        { 0x4ac9af09be6629L, 0xba455e11ca90c5L, 0x0147538856f492L,
          0xc80db7eabd7840L, 0xb3526d96beb9cdL, 0x37657fb9d81503L,
          0x8729a16193cec3L, 0xd9a93fbd69952aL }
    },
    {
        { 0xfce017594f47c6L, 0x228da21e366d05L, 0x27ce0b2dc8baf3L,
          0x8cc660b6b4a951L, 0xf678947384bb01L, 0xc629d7d44d980cL,
          0x47980e4e85e81fL, 0xa2e636a1cd723eL },
        { 0x6b6ebae77fb207L, 0x70179614c92891L, 0x5569541b4d279cL,
          0xbb6b36a41758cbL, 0xecaa22227a8e30L, 0x8b6746ab470ad9L,
          0x4c4601763e2d3dL, 0xe19c4edd3edaecL }
    },
    {
        { 0x0b43fec34718c8L, 0x553c407f33499fL, 0x8272efb970d1dbL,
          0x008c62ca8e8d1cL, 0xe4b79d763eec45L, 0x1fd4230f2d71a3L,
          0x090fdafa368c36L, 0xf62c101fca7baaL },
        { 0x1c9e6c8d2395b3L, 0x671ed6304c5513L, 0x577d933299a465L,
          0x286890e63f9986L, 0xd92a95dbfc979cL, 0xcebd79d2b51019L,
          0xe74d88b3d07251L, 0x8b6db73906f9adL }
    },
    {
        { 0xc0c43db7b3d90cL, 0x85d154e4304a06L, 0xe8aceefaf2f38eL,
          0x5e0429383d9459L, 0x65e5e32431afd1L, 0x9e5f050a900a65L,
          0xcbaa1718a26671L, 0x33d0b249c93de7L },
        { 0x3dcbf92d5b6680L, 0xc47e5ec20006f9L, 0xc9711299a51924L,
          0x665d9b8cd0ed46L, 0xed2d63fa5fcab6L, 0xa817eb6cfbfc5aL,
          0xb38169fb76eb76L, 0x8b93544f11160bL }
    },
},
{
    {
        { 0x02eca52693bdcdL, 0xbbf09232ae01d6L, 0x0b0a2de8b44b3eL,
          0xdb82449b250dffL, 0x0c42b866e1c530L, 0xcd226dca64c2c4L,
          0xcfb2bb1f046b5fL, 0x97e2fae3fccb0dL },
        { 0xdf9290745ed156L, 0x224dcb9f641229L, 0x2126abc5f1f67eL,
          0xa7eed5ae9c8a6bL, 0x40abedc9857d9bL, 0x3f9c7f6de941c6L,
          0x2158d42d725ddfL, 0xbdd10158c69543L }
    },
    {
        { 0xa7dd24e8df2fbcL, 0x3adbcfd13d1aeeL, 0xf6a32d113b2177L,
          0x89a72327a9a14cL, 0xe3aef43dc65df9L, 0xeaec3e3a64d74cL,
          0x4d387d84fec33bL, 0xaba2a0521a2128L },
        { 0x2382c226b85e30L, 0x4352d85cd2aad3L, 0xb0c6001d9772c4L,
          0x7ed82635f3653fL, 0x3626a6f0300f47L, 0x23909de6ca7e4eL,
          0xb43dd81c154141L, 0x9a49fad7e4bc68L }
    },
    {
        { 0xa3661df2428f88L, 0xbe48b0256e0db2L, 0x3cd1871ce79aa9L,
          0x90ab87123dddacL, 0x9c58fb971871a6L, 0xf031f7fa34910eL,
          0xb501eea81060e4L, 0xdb668ba791224eL },
        { 0x240bbcb6a705bcL, 0x7e76fbd2d1865eL, 0x6e2cd022513641L,
          0xe6c522546365c9L, 0xe46a8b8a5a01fbL, 0x696fa7bb67618bL,
          0x418b3b90db6792L, 0x7204acd7108b9cL }
    },
    {
        { 0xb5a143b8456b45L, 0x8a3ab25f53b4d9L, 0xb112a58e13a570L,
          0x613ca3281487d2L, 0x837d8233b1e7c9L, 0x592baded41e9d5L,
          0xdc1893a5cd02f2L, 0x08795028972e23L },
        { 0x7003c08cb76261L, 0x14bde9e332a5e0L, 0x14b2872cbbd78eL,
          0x5594061de238e8L, 0xad12645067466cL, 0xa8d0e64f5e4952L,
          0x5b44b82c7f8d06L, 0xb51bea8fb1b828L }
    },
    {
        { 0xebad6853f0daccL, 0x5c31b8b1cbebbcL, 0x6746975fa5a2dcL,
          0x2d9596531d9faaL, 0x343797d00fc0e4L, 0x38d821c55fe01bL,
          0x0bfdb247323aa0L, 0x42613c4f962a8eL },
        { 0x599a211e134bc0L, 0x75fa4a147a7084L, 0x6e719487f734b5L,
          0xd5ced2d6dfca2bL, 0x9fa0fdc8aeabd2L, 0x5e6b03f12361daL,
          0xad23d315859fcfL, 0x3120ef125a5fc8L }
    },
    {
        { 0x990ef628e9f638L, 0xfdaa240626a60cL, 0x4a3de202abddabL,
          0xd5d10b7d8872b2L, 0xa01b7301ea5880L, 0x481697fa81b9d8L,
          0x29841533471ed8L, 0xefd73f8292d37cL },
        { 0xdda76269994bebL, 0xa0377036a4f865L, 0xda992ece5b47d5L,
          0x912a427e53edbaL, 0x64675989264e45L, 0xd3b68c3af71222L,
          0x9d3436c6dedc5fL, 0x1e027af076b2adL }
    },
    {
        { 0xd56fca14382f4aL, 0x83712a48966b7bL, 0xd6b2cf5a4c9ddbL,
          0xa66be29f602875L, 0x70e4266894f3d0L, 0x007d220b3195caL,
          0xba38d8f82c74d4L, 0xdccc5fcd975cbdL },
        { 0x03e1610c88b38bL, 0xeb9f9a152e0d8dL, 0x6a57ecab646eb7L,
          0x161641fc76b6c1L, 0xf9025adbd2e12bL, 0x87c74db5c0e26dL,
          0xed5cb51bfeca74L, 0x603dfb6e34a08cL }
    },
    {
        { 0xc4be728cb03307L, 0xde34c0ec2741ccL, 0xe01db05a74eb17L,
          0x1bfce0c8905e4bL, 0xb18830ad1b1826L, 0xcacbb41e87bbfbL,
          0x8696842d2f1a79L, 0xa80e5fb08c83eaL },
        { 0xe48f1633f1439cL, 0xc1d4108cd6987bL, 0x05705c4b751814L,
          0xa9bffd0c1c622dL, 0x23de4af46cd053L, 0xf782f5e39457c3L,
          0x815276b5e5d243L, 0x31320416161ae3L }
    },
},
{
    {
        { 0x245966177f2542L, 0x203be7e8372b25L, 0xc7c9426ee2007bL,
          0xc5641380621799L, 0xda56589c28c3ceL, 0x13e8a7c7afc1e3L,
          0xdba81e9e352082L, 0xf43054904435c7L },
        { 0x4d26533691de4aL, 0x364408cfb777abL, 0xccdfb43eae7f88L,
          0xbc40f44a525b11L, 0x8e112a53c60627L, 0x7f7c581e17e696L,
          0x0fd78781ea774aL, 0xd09e6320b1f582L }
    },
    {
        { 0x44390bd70aab15L, 0x41112bc889c3f2L, 0x6b02894d685349L,
          0x71030015584dfeL, 0x373cb1b1ba7887L, 0x53d286c2a017c7L,
          0x2ed03883c81fdcL, 0x3bfc5e3fbcc6fcL },
        { 0xd38ac6ffd6418dL, 0xc667e96bfad89eL, 0x46f4f77eab4d66L,
          0x194c04f0911293L, 0x0fd09cf68c48d5L, 0x6f5b05563cf7f4L,
          0x0c0a8c4acd562fL, 0x94c1d8336d965dL }
    },
    {
        { 0x94fc8f0caa127aL, 0xc762d5dd803690L, 0x8bfdfd11ebf0d3L,
          0xa98cdf248eac50L, 0x3d7365d8b5ff10L, 0x20dc29bc65b4deL,
          0x62ac28e8ec7c68L, 0x7f5a13290372d2L },
        { 0xf3d8a253246658L, 0xa4bebd39ac202aL, 0x078ede75cc1697L,
          0x5525800c8fc022L, 0x302a8025fae77bL, 0x018013957917b6L,
          0x7c8806d864bf55L, 0x4e2d87812f06f1L }
    },
    {
        { 0x8d351183d66e88L, 0xfb861a1a91d02aL, 0x8c27c2a7850e5fL,
          0x9fd6399a5496f6L, 0x52152ae8080049L, 0x600e2fffd1c2dcL,
          0xc75902affe8b2eL, 0x5c4d2cce03b175L },
        { 0x8ad7c424f57e78L, 0x77cf6061736f87L, 0x2876012f85038aL,
          0xff328451b97b95L, 0x3cc6dd5392dfc8L, 0x72f1363a6f5075L,
          0x028ec4471de894L, 0x7030f2f6f45a86L }
    },
    {
        { 0x66400f59695817L, 0xeda0a7df20ea36L, 0x855be51d394992L,
          0x2d082c18336f62L, 0x30944ddf28c868L, 0xfb5f8530dc86d0L,
          0x9562ae5564a0bdL, 0x1f7ea12b6b9b51L },
        { 0x5bd74e0d0a7148L, 0x6c8247fb91e572L, 0x699aba547da498L,
          0xed825811f7c814L, 0x434674b62057b9L, 0x8b4df5e15c15b4L,
          0x2a97da1b110081L, 0x2a96b0c4c417feL }
    },
    {
        { 0x4f75dfc237639dL, 0xe5ad6bc1db7029L, 0xd43e06eb3d28f7L,
          0x89f3bb5e447989L, 0xc426a2c01a1a6eL, 0x33ea71c315878fL,
          0x8a7784ab1b5705L, 0xa59e86e77ca811L },
        { 0xddb133c36ae155L, 0x49f1d4c0d51b42L, 0x55080829d05519L,
          0x20e23be5291816L, 0x35047ec67181ecL, 0x6237dc47aad091L,
          0xa1d3ce1e2e25a2L, 0x1de05220d3db4cL }
    },
    {
        { 0xe9a5e19d9fd423L, 0x0c2c3d09801e43L, 0x043c2dd28df2daL,
          0x4eecab4e1ad12aL, 0x97e17979615aa5L, 0xe57b879ca7bb5eL,
          0xa2a903ccc92619L, 0x5cef370aa56e93L },
        { 0xbef29fa7f3232cL, 0x1cf35ed2b7ad5cL, 0x35c48933b6077aL,
          0xe0651487a1d47dL, 0xedb4673ce14572L, 0xdc9e98c0b17629L,
          0xef98ebe9a02a5cL, 0x1f772e311d03c0L }
    },
    {
        { 0xcbdbdcd4608f72L, 0xb4352235a13c6fL, 0xa6497f64bb3c21L,
          0x3af238312c15c9L, 0xfbbf4b36322d11L, 0x520a5c6c641775L,
          0x18cd967e81e0e1L, 0x980b2c63de3871L },
        { 0xfa9db619ae44a2L, 0x0281dd2176bc56L, 0xfd037118a7f817L,
          0x9c485454129b30L, 0xb439648039626dL, 0x355050ee4ada6bL,
          0xc9c16d67f5d98cL, 0xf53ccc318c4d5eL }
    },
},
{
    {
        { 0x50ae9423ffb20bL, 0xa6c0b426865eb4L, 0x4677f7d09930f1L,
          0x742e0b64a16427L, 0x521d18ef976f9aL, 0x43ac9cfa454749L,
          0xda3a91dc51f50dL, 0xf657029ad6f954L },
        { 0xfe5f0646b4f99aL, 0xd92a5d963ad4ceL, 0xfcb55092e0e081L,
          0xadc85ab8d8a858L, 0x8e9b9660632f0fL, 0xe7a4f168d7216dL,
          0x00a4cc559c3b99L, 0xed6d0bdba09dc1L }
    },
    {
        { 0x7236d141621bebL, 0x1751fd4bc7ca95L, 0xaa619d12f5319cL,
          0xfc2b15b4e9316fL, 0x2d1a9069fd4d33L, 0x28c3bac8ced829L,
          0xf2efab51dd998fL, 0x2c133303b149edL },
        { 0x65237c9f601ac6L, 0xb54dd6507d6a45L, 0xa1ce391fb1a4cfL,
          0x2957533115f67eL, 0x6456da8465279bL, 0x02890aaa993e02L,
          0x6891853b7175e4L, 0x3fda2030f3e59bL }
    },
    {
        { 0xe99fe12d8c6e0bL, 0x7cb07ff5341c56L, 0xc292c7bdf77b24L,
          0xf52dfd0ca29906L, 0x4a6aa26772f02cL, 0x26f7684e1bbd09L,
          0xec56b2bee7c2a8L, 0x67709e6ad4a312L },
        { 0x99c57b2c570263L, 0xeb0100b2faafaeL, 0x980d5d1ff25ecaL,
          0xace35e682cf936L, 0x5a82ce544679edL, 0x5c76a41074b81eL,
          0xf36fa43a00abb1L, 0x064281904ffb2dL }
    },
    {
        { 0x68f6bc804bdd28L, 0xc311d96b5dc7adL, 0xff0d646ed32e45L,
          0xaf3cdc6e0f712dL, 0xd4508e9d483861L, 0xb624be50e1c277L,
          0xc510275c5dd841L, 0x451c5c3298dc02L },
        { 0xf87d479dd34d6bL, 0xda7f293dd06a38L, 0x575e129b699e9fL,
          0x79e5fb2215b2ccL, 0xd280028657e690L, 0x7fecd09e702a71L,
          0x85160abfa13677L, 0x5de3427ce65f64L }
    },
    {
        { 0x84e4bf6e8fff38L, 0x16f3725b358b1cL, 0x360371c3b472a5L,
          0xe64c06152f217aL, 0x8e673790501241L, 0x88e81d6ab2dd96L,
          0xf3e218a1385604L, 0x9736cafe84184dL },
        { 0xb55a043dbb93a3L, 0x335088f9301088L, 0xcea7a2db2a4959L,
          0x48e5d4ab882c33L, 0x114f09bad46179L, 0x4416467b446576L,
          0x01cb23e34c6c2fL, 0xddebf04a02db8aL }
    },
    {
        { 0x36d60cc9bde8a1L, 0x20fd2f2676e4adL, 0xebdcfb78936581L,
          0x245d0d5dbfc2c3L, 0x104c62ca9f82e5L, 0x7387457d654d9bL,
          0xe966777ae7f10eL, 0xefeb16f1d8e582L },
        { 0x4faf4f170364b5L, 0x0e1ab58d612472L, 0x11bbfe7fed6085L,
          0xb360a14a59a09aL, 0x61d96e9722fdb6L, 0x16a12f194068bdL,
          0x225bf07f73c2beL, 0x1e64665c8bd24eL }
    },
    {
        { 0x27a478a3698c75L, 0x778ccd36202aa2L, 0x0149c638d87f1fL,
          0xa660e5f784edaeL, 0xe0d4d2f82adfa8L, 0xf512dd61ba1f9dL,
          0x90cfed96245c58L, 0x6c3a54818b53ddL },
        { 0x833f70cbdc094fL, 0xa5f26f5b1514e7L, 0x93e7cf51c8cf13L,
          0x1436601186ec43L, 0x81924ace78170aL, 0xcc880a08694368L,
          0x2dfa9550b62cbbL, 0x0bc6aa496b4a2cL }
    },
    {
        { 0x5157a7e3561aa2L, 0x525c5008645c1eL, 0x22feb4ece7cbb3L,
          0x36d0d25c89a58bL, 0x43131f7c9bde9cL, 0x74afdda881f731L,
          0x99ab87c7c8e36aL, 0xf07a476c1d4fb2L },
        { 0x1b82056bebc606L, 0x95a1e5afcf089fL, 0xc5bccfa2b55d5cL,
          0x8fbc18e00eb0b1L, 0x93a06fe9efb483L, 0xcafd7252d74c57L,
          0xc7518f03de4350L, 0x9a719bfc6fd762L }
    },
},
{
    {
        { 0x5ee0d832362087L, 0x7f2c0d70b167e8L, 0xb7327895e0e865L,
          0xef5b2e898c4e65L, 0x222797d8fe9cc1L, 0xfe6d73e82d1e15L,
          0xc7c0e9cf62dc4bL, 0x962acfe937cedaL },
        { 0xd763711c1e85c7L, 0x8f2dbbc2836978L, 0xbadc0558c44e98L,
          0xed63eaba3e93f8L, 0x807e85741b55c7L, 0xd51ae5e6d1207bL,
          0xa0ef9a639d541bL, 0x58855f9a0c56a5L }
    },
    {
        { 0x7d88eaa213091dL, 0xcbdfee745b6a0dL, 0x826a0124f5e077L,
          0xb04fc1390f1e4cL, 0x1961ac3aea69aaL, 0x3afb719d5bb63eL,
          0x2a378374ac7e5cL, 0x78efcc1c50ca45L },
        { 0x346e8f0b8abdefL, 0x27e3dbd88095d0L, 0x56d3379ffc6c22L,
          0x67d416cfa4b291L, 0xc3baaf63b1b373L, 0x0184e1fdf73baeL,
          0x38ae8f79167528L, 0x7329d4c35d6297L }
    },
    {
        { 0x45d2ac9f568c52L, 0x51348149808593L, 0x0c92d8331b7ed8L,
          0x921327a0876ecdL, 0xf752d75052736aL, 0x7b56487bc6b837L,
          0x6b1a320a23b4ccL, 0x1983937ec0d665L },
        { 0x2c3017c08554abL, 0x40ad955366e87fL, 0x88c4edf8ed7f02L,
          0x64a7db13cc5e6dL, 0x5ac91fa2dc978bL, 0x016a20d925d2a2L,
          0x3604dfeabb57b4L, 0xc3683ecd7e2e85L }
    },
    {
        { 0xc47150a4c0c6d0L, 0x30af45ee22adcfL, 0x39b5acb022ea4bL,
          0xfbe318577203b5L, 0xe5aaa346fd9b59L, 0x0062c90dd1c8dcL,
          0xcf113f354049acL, 0xd8fba4d63a31b5L },
        { 0x73b54881056a69L, 0x3be6cbcd780bdaL, 0x5776ec230ba2b9L,
          0xbe883cf8e8d6f7L, 0x64efe945c2be6fL, 0x064f704f1ade8dL,
          0x41cfd17743110eL, 0xaac94114c20abeL }
    },
    {
        { 0x91f9192f1c1468L, 0x8176e744563e13L, 0xa48b5f90bda15dL,
          0x2a085aeda42af6L, 0xfd38ab2425c018L, 0x2884ba408abafbL,
          0x356f318cbd091dL, 0x454e450817871bL },
        { 0xe080e818ada531L, 0xa40f1eb3152ba8L, 0x051049f0c38eb1L,
          0x37e4bb3bd45003L, 0x6d0980454a01e5L, 0x6de932feeb824aL,
          0xccdef37dc93481L, 0x8633e0793a05e8L }
    },
    {
        { 0xbe94256034675cL, 0x376c01d08db789L, 0x8707ee79af1b6bL,
          0x633b3ef11bfbacL, 0x694f33fd06db60L, 0x2a68bfcbb13407L,
          0x1c860c9da27c3aL, 0xbca16ded701ac3L },
        { 0x2b76cfac59ffd0L, 0xf9a116554d718dL, 0xf86a1db67f0878L,
          0xe313e05af34e85L, 0xa1888113343159L, 0xdbe4c3f0bb7ed1L,
          0x73b67e80c732bcL, 0xa4e1c87e74110eL }
    },
    {
        { 0xce1106b5c6770cL, 0x422c70b5c0bcb7L, 0x32a39908195e7fL,
          0xa24968d1ccd4aaL, 0x8f08ecf720e557L, 0x5da10a454bcc81L,
          0x9d3c73b6cd846eL, 0xaeb12c7368d065L },
        { 0x2110859cf9fd1bL, 0xd2a4801ee2bd6dL, 0x376e556e9466acL,
          0x767803b3b5aa35L, 0x343f842b8a89baL, 0x3263cc16726bbfL,
          0x26caf1725871b0L, 0xef66ad641b8578L }
    },
    {
        { 0xc9f2249638068cL, 0x96d282c1ccf9afL, 0x71df30c69b435aL,
          0x88c943acb9d5c9L, 0xbf98ef12a8f378L, 0xffc1824114c6ffL,
          0xda3ad2cd52e8c7L, 0xf1222bc1afcb59L },
        { 0x459e94b0ee334aL, 0xd4477b8421933aL, 0x60fb7b0a1e401eL,
          0xfde6e820d1e330L, 0xcecfe9b3233fdeL, 0x09ec4662e93523L,
          0xa5ba64930775b9L, 0xcc397e5adf80f2L }
    },
},
{
    {
        { 0x2fe182d4ddc8a8L, 0x88d6e79ac056bfL, 0xc3ff2d10e41e4eL,
          0x32ec7f92c3679fL, 0x3561f094e61051L, 0x4553f5a6c6250aL,
          0x2b765efdd25c5bL, 0xe3a40a26a1cd7fL },
        { 0xb27309b5d821ddL, 0x950fb8dc2c17caL, 0xfeed0158fb0d4cL,
          0x762c479f550179L, 0x306cf44e095840L, 0x84b413ad379e66L,
          0xd6e5d5abb2e4f1L, 0x8bc12b794b085dL }
    },
    {
        { 0xc0d4cb804b5532L, 0x7a31525b9940a6L, 0x010e7dd68c69d1L,
          0xd81f29d2a18c35L, 0x08ae7703f11e73L, 0x5358f876e55106L,
          0x299e8cac960ef5L, 0x89a6fb4acfc8dcL },
        { 0x5996a406dc7d4aL, 0x21e5112e51b96eL, 0x95b8c3d09a202bL,
          0x306ab0fd441f1fL, 0x2834fed98d4245L, 0xc29c387d0abbdeL,
          0xf6a9bf1b805c15L, 0x602f4f8c4e458dL }
    },
    {
        { 0xf041486e5a893aL, 0x53b891d8934327L, 0x11e000d4000758L,
          0xa4ccde8662bad9L, 0xe34d3edb9a1b64L, 0x72d967584e7a6dL,
          0x773da2f6627be4L, 0xa11c946e835ae3L },
        { 0x02e8203650bc15L, 0x2d35936e58b78dL, 0xe9cfbe8f21a3ccL,
          0x55ad8311049222L, 0xbf99de438fff47L, 0xebbfd803831db5L,
          0xe990636af2af42L, 0xc26ae52b7f5a0eL }
    },
    {
        { 0xb5d85b1fa8f846L, 0x4166489b3b1455L, 0x768260dd36a305L,
          0xc6a82354ff5645L, 0xd241cd8d6e93e5L, 0xeed9aa1a406e74L,
          0x9e96ab05f600d9L, 0xa26b8b56eca2a1L },
        { 0x78321cfd705aefL, 0xc4fb6b3c0161ecL, 0xdc324415199cf1L,
          0x33627d0d0a5067L, 0x13490cb15143eeL, 0x77e0ede85b4f44L,
          0x904f12e394b165L, 0x90f50f5efab32dL }
    },
    {
        { 0x4aa0a16bc2de96L, 0x172596aaa9c12bL, 0xd512e1e60e8a29L,
          0x77d35c1f637e83L, 0xbb0d141d2aae0bL, 0x8a878a58c03738L,
          0x6d24c01ab0e525L, 0xb7d3136f760887L },
        { 0xdbc3f8f3f91b7cL, 0xe7b4bcaa8722c0L, 0x3286a91da0ae65L,
          0x8372274225b084L, 0x5884cd5ae1886cL, 0xb4e63ef3a23cf7L,
          0xfe5f202f2dd0daL, 0x951fac9653916cL }
    },
    {
        { 0x05e2e8f854fa4eL, 0xf411f941edaf10L, 0x26cc562a0a928dL,
          0x78fd34e4abce65L, 0x1d8760998a32e2L, 0x85dc76f4c37518L,
          0xdcaeef500e8021L, 0x7fcb2f84e9b2a5L },
        { 0x9eba91ef382c06L, 0x2052e8524cae53L, 0x617336ef5c1519L,
          0xf1546d5b4e632bL, 0xa9edc81d7b8ffdL, 0xdb2914f29ab68cL,
          0xe805070debbabaL, 0x775e53bc3b719eL }
    },
    {
        { 0xa40e294065256aL, 0x9f113868fb031aL, 0xac03af8059667cL,
          0x432eb3a0475f58L, 0x22332bf01faad0L, 0xc8132e9bc57a11L,
          0x27d5a173bc3f8bL, 0x5471fc6930bf3eL },
        { 0xba28bc0e6bff40L, 0x198d57e555e564L, 0x13ce8319c65b8fL,
          0xb0a5c9d5681b51L, 0x467588bdeb9e11L, 0xf1891a7bb4250bL,
          0x10b938bd12b433L, 0x0b8c80224dcda4L }
    },
    {
        { 0xc428703cf332d3L, 0x9d0053cf2a5b98L, 0x4e4c6207838a15L,
          0x2e92919fbf8a43L, 0x39ad52421cd9a5L, 0x584ed6c1561588L,
          0x20af30517a95c8L, 0xa223077b70e1c8L },
        { 0x679cfea2fa4871L, 0x54f2a46ac633c7L, 0x60306514cdc5f1L,
          0xc4facda75a1dc7L, 0x710a2882d07d19L, 0xd55864e6b44992L,
          0x44d4b6c454c5b2L, 0x2855d2872f9981L }
    },
},
{
    {
        { 0x4071b3ec7b0674L, 0x800eb14f8794d5L, 0x70573afbe6783eL,
          0xafaa4407785901L, 0x112d2a1405f32cL, 0x3761a52169b3e2L,
          0xe168b31842a366L, 0x5bc322f9bf4734L },
        { 0x36ef240976c4a0L, 0x066f3d6fea4e64L, 0x0e954bda989e57L,
          0xe36ef5ef9466e4L, 0x6bb615abeb9226L, 0x5571e5f3d5a2caL,
          0xa86efe24897a86L, 0xed7e9cf28a9f77L }
    },
    {
        { 0xdf10c971f82c68L, 0x796ba1e3b597e6L, 0x1ac77ece718cbfL,
          0xc8175bb410eac8L, 0x0cdf9a1bc555efL, 0x6b889f17524e05L,
          0x6bf1e61ae26d82L, 0xb3f6ad5d2e97d9L },
        { 0x94dcff9f226487L, 0x60e6356be03ddeL, 0xda1f93b6a3dd7dL,
          0xf1be72179ca90cL, 0x05ed3131e6bce5L, 0xcf50908d48af3eL,
          0x3b0e85c61e554fL, 0xfe7e35ba2778d3L }
    },
    {
        { 0x42c503275ac5a9L, 0xa66a66dda062c2L, 0xa4f4f82caa7023L,
          0x489d47664b4f86L, 0x10b108897311adL, 0x55dd637177b2ecL,
          0xa5ccff09a267b1L, 0xf07690bff327b0L },
        { 0x39162ed2250cd2L, 0x1426de08b255f1L, 0xf227afd1bdd731L,
          0x78f8a36fa4c844L, 0x267a211157379cL, 0x3f05f92cc04acbL,
          0x374496cfc69caeL, 0xbf2c5d016ebfecL }
    },
    {
        { 0x605418bd0518d1L, 0x3237f809e1cbc6L, 0x37a7005286c019L,
          0xf1fb0e0b15af0bL, 0xfc3b97caa853c0L, 0x1f48bd0e6beba2L,
          0x8e5d7c5e6a72f1L, 0x575e66d26ebf0cL },
        { 0x099477662eae3dL, 0x53f074f96c9c65L, 0x6cfbfdbb81badeL,
          0x98b4efe3fed7d1L, 0xdaa112338c3382L, 0xdf88b7347b8ec6L,
          0x9b0fe4b9504a4fL, 0x2e7df4cf30c1c3L }
    },
    {
        { 0x25380cb2fc1833L, 0xb8e248c18d62deL, 0x91c8f59d82f9dbL,
          0x5ec2b202444750L, 0x3f3a1f766b6f74L, 0x0180aa9dd7d14dL,
          0xd0a342d2956b9cL, 0x26e910e7139873L },
        { 0x2261dc4139e23dL, 0x7edb181b8343ddL, 0xfcf1073b4038ddL,
          0x88870efa3bfea3L, 0x4e98ba964a263eL, 0x3c6e5dc70811f5L,
          0x17d28f5f86055dL, 0xca9c27666e4199L }
    },
    {
        { 0x0b2d8bd964ef8cL, 0x5a99b8588e2ba6L, 0x9e927b204498ceL,
          0x9ff20c5756eb25L, 0x97cc27b3f27736L, 0xf32dd6d4729583L,
          0xbdc26580381a94L, 0x70fef15ef2c06fL },
        { 0x50a619149252ccL, 0x9eb4a14236b4b9L, 0x9b1b2158e00f78L,
          0x27add366ea9c23L, 0xef61763c3a8e79L, 0xed4542fd82ce56L,
          0xa8737e70caed75L, 0xeca0ac2d452d76L }
    },
    {
        { 0x20c07793d082d0L, 0x6e3ce64c9e9f3bL, 0xb3a4dce75a195fL,
          0x3a3c305bdd9f24L, 0xe2545c88688942L, 0xa463c82080f32bL,
          0x442974842686b8L, 0xf50e20d7213866L },
        { 0x265ac523826e74L, 0x26fba57228e8ecL, 0x8a1e1dbe6b3ed8L,
          0x7c7b278f0fe65aL, 0x9a6df23c395234L, 0x99562060b0f114L,
          0x440c8c4ef90837L, 0x21ad22a3645f65L }
    },
    {
        { 0x1e023a6edd31b2L, 0xf76d1459ff8668L, 0x970705617b45c8L,
          0x06120781e88e37L, 0x85c51c8922faacL, 0x4df392e22756d9L,
          0x8907fd0a03c98eL, 0x626f46a52ea51cL },
        { 0xf8f766a486c8a2L, 0x8c499a288ed18cL, 0x44d2dc63c4f0deL,
          0x47dde686f2a0b6L, 0x9a655f84a973fdL, 0x3e7124e786ac80L,
          0x699e61ce8a0574L, 0xdf0ba9a31cdd0dL }
    },
},
{
    {
        { 0x76270add73e69bL, 0x991120fc67d38aL, 0x7be58309469f0cL,
          0x93aba597db40acL, 0x2b707bc822fc08L, 0x4199fc069551cdL,
          0x38deed4f367324L, 0xca518e12228787L },
        { 0x72f1befd9a9277L, 0x57d4aabe49ae90L, 0x13810d5db23478L,
          0x2a8b7809b4b77fL, 0xb542f4e1b4e004L, 0x4080fd03ec77f0L,
          0xb49e9fecec6596L, 0x20338d33f16037L }
    },
    {
        { 0x4adcdae53554b0L, 0xfea4906e04c4dbL, 0x0808bec7748233L,
          0xde7477c47148d7L, 0xdd9124c03da38cL, 0x6b2503125ee8e9L,
          0xae67399b0d6161L, 0x70c4acd82203b6L },
        { 0x9683916d31dae8L, 0x34775031ac7f69L, 0x9553153988e4adL,
          0xb58f41153a15e1L, 0xb65a2d492ba2ddL, 0x7c3efb1a90169cL,
          0x210f45e6b1747dL, 0x16e8d1bcff488dL }
    },
    {
        { 0x252adf89d703dbL, 0x259ac1dfdfeb39L, 0x7faf6af115e806L,
          0x7aaefd6c1aff21L, 0x80542107c0113dL, 0x481f1a5e19b4b1L,
          0x7c17d43fcc8c61L, 0x8b04452bb0bbbeL },
        { 0xe51e5f54cebae1L, 0x05341ba56a414cL, 0x0083a2c7fb8a30L,
          0xb4663f277f4952L, 0xce72eec4bb0074L, 0x74fdd66a3584d1L,
          0x6b9e58eb02e076L, 0x5be45d53b961f4L }
    },
    {
        { 0xc7474f31ab2e0bL, 0x2838ccbf4bf454L, 0x634392ef3c3eacL,
          0x440e40a137602bL, 0xeea67e9d1ae8e3L, 0xafdf93a77e221eL,
          0x3c9f3da2719a10L, 0x466ecef32c8256L },
        { 0x1061c19f9c432fL, 0xa1332d9b1c7d98L, 0xbc735f2a425c2cL,
          0x1429cdf4b1bccbL, 0x77b42a16bbb5f9L, 0x30078e35955ae4L,
          0x8acd77721cc315L, 0xaa90d5fe86fa99L }
    },
    {
        { 0xfcfd460721115aL, 0x6a7de3e08269b8L, 0xe5964a696dd47eL,
          0x6717cd58dca975L, 0x7ea4ebe98b149eL, 0x6f894d5b7b8057L,
          0xbd6f9607f30e31L, 0x61ca45323df092L },
        { 0x32241f99d782f3L, 0x55173b02abfae2L, 0x0abe0edd15bbbdL,
          0xb6d3c0ab438abbL, 0x62fb4679ffa20bL, 0x30926b5d31560aL,
          0x44bf27c2a0aa6dL, 0xf7473131a4cb97L }
    },
    {
        { 0xa2f6c0db0535deL, 0xcb02ae1c855166L, 0xc699e6bb3422f0L,
          0x774febe281ba8aL, 0x1d9d24fffabcc7L, 0x0b31ba1fe12ba5L,
          0x4c8680313d0af7L, 0x90640d32f47160L },
        { 0xa0c4bf45876603L, 0x717f6fa950ab08L, 0xf12bb53a710de8L,
          0xc500c616a88f50L, 0x0070f992645351L, 0x57aab5d2446893L,
          0xd553fa8b68f657L, 0xe8537c1693c55dL }
    },
    {
        { 0x58e86eb7fc7684L, 0xdf330f7bfc73a9L, 0x41e337dcc11936L,
          0x36d92006e35759L, 0x01327033500d8bL, 0xfa684059483354L,
          0xc8f2980667851bL, 0x538ec8918296b0L },
        { 0xa2a2c4fcff55f9L, 0xb260d4d60d20bdL, 0x3ed576fd9cc59fL,
          0x4ed8c64d514fccL, 0x37ebfb2c22b315L, 0xca67a3694c212cL,
          0x4f8e08c3a1795eL, 0x498f9264e7261fL }
    },
    {
        { 0xfea7382c59b3d4L, 0xb9942ed3f2925fL, 0xe4b00dc8ea77e8L,
          0x74a18ec3cab02eL, 0xbbbb752ef16d0bL, 0x639da4fffab032L,
          0xc371a4a3aa30f0L, 0x8e26b22caa175bL },
        { 0x94e41567e2b62eL, 0x7cceea625a794cL, 0x931d2f4479f015L,
          0x946183d90b25b2L, 0x1504e9768a2807L, 0xa7577d3fa49dddL,
          0x24fc87edd48699L, 0x9edefd63d7d99cL }
    },
},
{
    {
        { 0x0508b340f0b450L, 0xe0069a5c36f7f4L, 0x26556642a5a761L,
          0x0193fd8848e04dL, 0xc108cf573fe2e7L, 0x05eb0ecfd787d4L,
          0x1555ccbff28985L, 0xb5af09f651b995L },
        { 0x167d72ce1134beL, 0xd6d98bf57c669aL, 0x40fb7166dd76faL,
          0xeabbf202a41b31L, 0x300ff0e09b75b0L, 0x32b6fadd9a0c1eL,
          0x805188365a80e0L, 0x8bef69332110feL }
    },
    {
        { 0x637802fbef47d4L, 0xfac114b2d16eaaL, 0x7b3f3ab0415644L,
          0x17ab8d12dd895bL, 0x271b7fe87195f3L, 0xa3f867ea71f65fL,
          0x39ba40cc80583aL, 0x6db067256e1fccL },
        { 0x4feab4e06662a8L, 0xc857415c74bd46L, 0x18032ed732b126L,
          0x87c8aea7a099eaL, 0xb4a753536fe0a8L, 0x33a98da27673f6L,
          0x3e40c022b8e549L, 0x2def1af9a4c587L }
    },
    {
        { 0x9618b68a8c9ad9L, 0xd70b4aa49defdaL, 0xae8b1385f788efL,
          0x87c3542dd523f4L, 0xe42c7055c5b004L, 0x6303360fa7df57L,
          0x33e27a75f6d068L, 0x9b3268e8ff331aL },
        { 0x845cc9623ee0c3L, 0x003af70ac80084L, 0x6a9f931530c41dL,
          0xa1d7051bb127f0L, 0x642ce05ca36245L, 0xc34205b0323ee9L,
          0x7cc8912b7b3513L, 0x6252cc8076cbdbL }
    },
    {
        { 0x10e68a07089522L, 0x36c136158fc658L, 0x490397d74723a4L,
          0x42692c0519d56cL, 0x69d251bf1ff235L, 0xe689d03c2cbf37L,
          0xf04ceba825b7f4L, 0xd6b9bee2281c2eL },
        { 0xc52ef3fe0043abL, 0x351bf28d1d1be8L, 0x277615f0f18a5aL,
          0x31f717f5d6800fL, 0xf5fb82dab922e2L, 0x99aee2f2d6ae43L,
          0x42477fec63b982L, 0x904aeb1a594a01L }
    },
    {
        { 0xaa82174eb39974L, 0xbc38e6195e6aa0L, 0x6a3df8a25c0675L,
          0xf324203ffbe739L, 0xfa5a0b4a3f0649L, 0x79c87327a7a6b8L,
          0xeb65ecd40ad3f5L, 0x718d416e4e45c5L },
        { 0x029dbf4e2326fdL, 0x0c63416e7942f0L, 0x6d0c7286f4e678L,
          0x59f0b10a138601L, 0x8a1d9788d92ea9L, 0x9f8d712c22eca5L,
          0x73970447b6b96bL, 0xa2d49eee6fb955L }
    },
    {
        { 0x249f900bf14a19L, 0xd3522da63a8cd2L, 0x28a32f386964d2L,
          0xacf712bc1fa743L, 0x98a9bfc0bb94d3L, 0x318ece1bc06824L,
          0xfc476754fce7f0L, 0x19caec9e4135b7L },
        { 0x6de68a8c6817bbL, 0x7121960f3b6d89L, 0xa7d4261f5a818eL,
          0x0c0ba519157455L, 0x78b6acf450d5ffL, 0x198b4934e8649aL,
          0x0941a3cfd05da3L, 0x264ea4adb55951L }
    },
    {
        { 0xcfee91c46e5a31L, 0x47b6806fff7366L, 0xdb14be45df849dL,
          0x3c5e22bac66cc7L, 0x7f3f284a5f4769L, 0x4e00815383be36L,
          0x39a9f0b8072b0bL, 0x9887cd5c7eadd6L },
        { 0x7dd8f05b659511L, 0x15c796dd2e1cb9L, 0xe5edb0c0d31345L,
          0x2025df06939c60L, 0x6314c08bf15de1L, 0x03c154804c7fb5L,
          0x413337fbb5d3edL, 0xfc20b40477e983L }
    },
    {
        { 0x7f968805db0ef9L, 0x05562dee9c2a70L, 0x071e5bc7dae133L,
          0xa8cdd12237fc4aL, 0x6d565e74ea492bL, 0xa17cf94381ee52L,
          0x6ab8a4e9f5c546L, 0xbb642f340288efL },
        { 0x64e59215df5c2dL, 0x43696e3bb906f4L, 0x73a841a74ae46cL,
          0xe264883c506b8aL, 0x9542e1aa1be548L, 0x89385395e81b4aL,
          0x5642cfaeaca6ceL, 0xed8077b806e0f9L }
    },
},
{
    {
        { 0x1c776c47e13597L, 0x0ec8b289e584fdL, 0x0bb6043b8b61e8L,
          0xdcc17489cd835bL, 0x493e6ac39fef9aL, 0xb44eb34d133e17L,
          0xfebcd0071cb6f9L, 0xe6cf543d20eff2L },
        { 0xf265cad0a004c7L, 0x9b06c9dd35cc12L, 0x769f985cb4ea53L,
          0x29160a20993434L, 0xdf8dd108d939c4L, 0xefa177c6711e2fL,
          0x1695790cd7a2cdL, 0x38da3d777f6642L }
    },
    {
        { 0x9bfcfd96307b74L, 0xc26a36dbfdabc3L, 0x9341be04abe28eL,
          0xdb20b5273d1387L, 0xf8d229c3d1949cL, 0xf1e0afeb8b3a41L,
          0x29c60dfed565d0L, 0x6930bb58b43b2cL },
        { 0x1d76527fc0718fL, 0xdb981431f67189L, 0x0c62f6451f32ccL,
          0x70a66268bd35e5L, 0x1725641c1cece7L, 0x7f130a8f96f4a4L,
          0x72319e9f06ee98L, 0x215b73867bf9b2L }
    },
    {
        { 0x8d1bec20aaddd7L, 0xfb8b95bb8be4f9L, 0xeac193efde1026L,
          0xa5edea79d5860cL, 0x4adbaea44280d3L, 0xce8b67038f4798L,
          0x914c107ec30deaL, 0xbdc5cf7000776bL },
        { 0xb6fd7d1a206a13L, 0x9941ebadae986eL, 0x76c27a81f1caaaL,
          0x6967c123f108b4L, 0x6f115284aea2d0L, 0x9bb4319144ddacL,
          0x1a4d3eac8ec6fcL, 0xfe4b0b8bf37420L }
    },
    {
        { 0x5d9a4a1ec0ac6fL, 0x84b79f2fc7c80dL, 0x64222f7c14fac3L,
          0xdd9e039c23b3f2L, 0x4a84abdea956bbL, 0x370dcbaebe09dcL,
          0x79a9ea8e0eaf82L, 0x4cfb60aaee375fL },
        { 0x6a10dbf9106827L, 0xa3ba5cf43f305bL, 0x481b885c1bb083L,
          0x2f52380b3117b1L, 0x0066122ddd6791L, 0x4f8923e63bace3L,
          0x5c5f499ecb88d4L, 0xfdc780a3bac146L }
    },
    {
        { 0x34b70ae7ba1f71L, 0x909182945bd184L, 0x3b39778e707313L,
          0xdeefc5e6164e91L, 0xbb55bed4971f39L, 0x7d523398dafc8bL,
          0x82391bfa6adf0fL, 0xfd6f90ae319522L },
        { 0x60fdf77f29bbc9L, 0xeff9ed8aaa4030L, 0x978e045f8c0d3fL,
          0xe0502c3eed65cdL, 0x3104d8f3cfd4c8L, 0xab1be44a639005L,
          0xe83f4319eeab3fL, 0x01970e8451d797L }
    },
    {
        { 0xbc972f83180f4bL, 0xac053c0617779dL, 0x89392c57fa149fL,
          0xdc4699bbcb6263L, 0x0ae8b28ce12882L, 0xdca19a7af1a4dcL,
          0xd3d719f64e1a74L, 0xbb50201affdd5dL },
        { 0x56f73107ac30e9L, 0x65cc9c71878900L, 0x83f586627338a3L,
          0x122adefac5bb13L, 0x97de2001bcd4d5L, 0x6ed3985b8aa3a0L,
          0x8680f1d6821f9bL, 0xcb42028dda9f98L }
    },
    {
        { 0xcdb07080ec2db3L, 0xe28c8333dad1a1L, 0x2093e32de2da07L,
          0x731707383b8987L, 0xad17871f552b8dL, 0x846da9851cf70aL,
          0xf94a16e5c4f5e1L, 0x84299960f8348aL },
        { 0x4bf3f6898db78aL, 0xad77fa83d19b52L, 0x69767728b972dcL,
          0x7dfa35a5321be0L, 0x9881846dd344a6L, 0xe550292ad4e2a8L,
          0x8075217bc68bf1L, 0xdd837c4893be15L }
    },
    {
        { 0x09c931ed4fab5bL, 0xb2dcf08b77a0f1L, 0x7dac5c0e0d38a6L,
          0xa5570b00ae73afL, 0xc7c19d3f5aed28L, 0x575fa6f5251e92L,
          0xb843cd6cdf7275L, 0xd9d3d8e9a01287L },
        { 0xf94e356b3c370bL, 0xc62b99ffe464b0L, 0x7792650a986057L,
          0xeaa67d5c4b1874L, 0xba1ba4d0b07078L, 0xdbf636d7a03699L,
          0x1a16c34edd32a3L, 0x6ce2495a45cb5dL }
    },
},
{
    {
        { 0xd7c4d9aa684441L, 0xce62af630cd42aL, 0xcd2669b43014c4L,
          0xce7e7116f65b24L, 0x1847ce9576fa19L, 0x82585ac9dd8ca6L,
          0x3009096b42e1dbL, 0x2b2c83e384ab8bL },
        { 0xe171ffcb4e9a6eL, 0x9de42187374b40L, 0x5701f9fdb1d616L,
          0x211e122a3e8cbcL, 0x04e8c1a1e400bfL, 0x02974700f37159L,
          0x41775d13df8c28L, 0xcfaad4a61ac2dbL }
    },
    {
        { 0x6341b4d7dc0f49L, 0xaff6c2df471a53L, 0x20ec795fb8e91eL,
          0x4c7a4dfc3b7b62L, 0x9f33ff2d374938L, 0x38f8c653a60f2eL,
          0xc1168ac2efef73L, 0x046146fce408eeL },
        { 0x9b39ac0308b0c3L, 0xe032d6136b8570L, 0xee07d8dfc4aacfL,
          0x0a82acbd5a41ddL, 0xbe0ded27c3d726L, 0xce51d60b926ce9L,
          0xfa2f7f45806c1eL, 0xe367c6d1dec59cL }
    },
    {
        { 0x64511b6da2547bL, 0x76a349c0761405L, 0x37d662601223abL,
          0x0e243c1f4d7c48L, 0xdc9c8b4da756a0L, 0xc7430dfd72e7e9L,
          0x0eb130827b4210L, 0x7a9c044cf11cbdL },
        { 0x2c08ff6e8dd150L, 0x18b738c2932fc6L, 0x07d565104513e8L,
          0x0ca5cffaa40a17L, 0xd48634101baa8fL, 0xfb20fafb72b79eL,
          0x1a051e5654020fL, 0xe3b33174e17f23L }
    },
    {
        { 0x05910484de9428L, 0x620542a5abdf97L, 0xaa0ededa16a4d1L,
          0xa93f71c6d65bb9L, 0x88be135b8dfaf9L, 0x1d9f4e557ca8eeL,
          0x4c896aa26781adL, 0xd3fbe316c6c49fL },
        { 0x088d8522c34c3dL, 0xbb6d645badff1eL, 0xe3080b8385450dL,
          0x5ccc54c50ab1f3L, 0x4e07e6eac0657dL, 0xa7ba596b7ef2c0L,
          0xcceca8a73a81e9L, 0xa0b804c8284c35L }
    },
    {
        { 0x7c55956f17a6a2L, 0xb451d81789cfa8L, 0xdf414e82506eaaL,
          0x6ef40fbae96562L, 0x63ea2830e0297eL, 0xf5df26e73c46faL,
          0xe00641caac8bceL, 0xc89ed8f64371f3L },
        { 0xd22b08e793202eL, 0x39a9033875cb50L, 0xe64eec0f85ddb4L,
          0xdce45a77acf7b5L, 0x39d1e71b9b802dL, 0xafdfe7cbd559acL,
          0x17ec1f8809eeb5L, 0x8c0e38a4889b8cL }
    },
    {
        { 0x47eabfe17089daL, 0x2d18466ec90c50L, 0xa511aa45861531L,
          0xebb3d348c39b39L, 0xa0ac4daf1b5282L, 0xea26be7a9dadbaL,
          0x8992ba8554d86eL, 0x7fcbdb6d5f2ef5L },
        { 0x320e79b56863e7L, 0xeb9d0c0a7dce2dL, 0xb9f4031784cbc6L,
          0x68823ee7ac1f81L, 0xa6b6f4f9d87497L, 0x83c67b657f9b6eL,
          0x37357470fef2a7L, 0xf38028f59596e2L }
    },
    {
        { 0x9ea57ab7e82886L, 0x18221c548c44d5L, 0xbf8e6cf314a24fL,
          0x70ff18efd025e5L, 0x08d03de5334468L, 0x2b206d57404fb7L,
          0xb92327155e36b0L, 0xcc7604ab88ddd9L },
        { 0x3df51524a746f0L, 0x8fdebd8168e3fcL, 0xffc550c7f8c32cL,
          0x1dbbc17148743eL, 0xd48af29b88e18bL, 0x8dca11c750027cL,
          0x717f9db1832be3L, 0x22923e02b06019L }
    },
    {
        { 0xd4e06f5c1cc4d3L, 0x0fa32e32b4f03aL, 0x956b9afc4628d0L,
          0x95c39ce939dad1L, 0x39d41e08a00416L, 0xfd7ff266fb01aaL,
          0xc6033d545af340L, 0x2f655428e36584L },
        { 0x14cfb1f8dff960L, 0x7236ffcda81474L, 0xc6a6788d452d0fL,
          0x2ad4a5277f6094L, 0x369d65a07eea74L, 0x27c6c38d6229aaL,
          0xe590e098863976L, 0x361ca6eb38b142L }
    },
},
{
    {
        { 0x6803413dfeb7efL, 0xb669d71d3f4fadL, 0x5df402ac941606L,
          0xe5d17768e6c5b7L, 0x131bcb392ab236L, 0x7f1fb31ce2e0e0L,
          0xa2c020d9e98c35L, 0x33b23c0f28657bL },
        { 0xed14e739cf7879L, 0x10d4867b4357b3L, 0x127cea331e4e04L,
          0xc60d25faa5f8a7L, 0xfef840a025b987L, 0x78081d666f2a0aL,
          0x0fa0b97ac36198L, 0xe0bb919134dc9fL }
    },
    {
        { 0xc1d2461cc32eaeL, 0x0fdbfdf0f79a37L, 0x70f2bc21c95f02L,
          0x7d68bec372cddfL, 0x44f78178439342L, 0xa3d56784843a6cL,
          0xbadf77a07f8959L, 0xf45819873db4caL },
        { 0xe8eaaf3d54f805L, 0x2f529d1b84c1e7L, 0x404e32e21e535cL,
          0xabac85c159b5f5L, 0x4e8e594b00466fL, 0x40fcaabc941873L,
          0x3b4e370be407c6L, 0xccd57885b2e58dL }
    },
    {
        { 0x3ee615e88b74a8L, 0xd7d6608eab4e69L, 0x27cf9f1e4ace36L,
          0x282359e7aebabbL, 0x96e509bf6d162fL, 0xad906f3f1a290aL,
          0xe7d6c4f1314a58L, 0xeecffe4218431dL },
        { 0xa66e0e9e2cfed9L, 0xb0887ec71f0544L, 0xd34e36ba04c5d7L,
          0x094daa5ed4392dL, 0xcda83adc8aa925L, 0x1adef91b979786L,
          0x3124dcbfddc5d6L, 0x5cc27ed0b70c14L }
    },
    {
        { 0x386dbc00eac2d8L, 0xa716ecbc50ca30L, 0x9e3fc0580d9f04L,
          0x37dde44cfeacebL, 0xd88d74da3522d5L, 0x6bb9e9f2cf239aL,
          0x9e7fb49a7cbfecL, 0xe1a75f00a5c0efL },
        { 0x6e434e7fb9229dL, 0x0ec6df5c8a79b3L, 0x7046380d3fb311L,
          0xe957ef052e20faL, 0x0f4fe9a9ef4614L, 0x1b37d9c54d8f2bL,
          0x23b2dc139d84a2L, 0xf62c4f6724e713L }
    },
    {
        { 0xbd6922c747e219L, 0x34d14383869b7bL, 0x8c875a596f2272L,
          0xd9602c03fe361eL, 0x081348f744839fL, 0x61bd16c61ac1f1L,
          0x993b727d8da4e1L, 0xbb40ba87741271L },
        { 0xe6dcc9881dcfffL, 0x9f513f593ce616L, 0xdc09683618cd8fL,
          0xc3b1d1026639beL, 0xe8f149fc762ee2L, 0x59f26efb244aaeL,
          0x3f2de27693dd96L, 0xd8b68f79c3a7deL }
    },
    {
        { 0x6fa20b9970bd5bL, 0x87242d775f6179L, 0xa95a6c672d9308L,
          0x6eb251837a8a58L, 0xfdea12ac59562cL, 0x4419c1e20f1fc3L,
          0x0c1bd999d66788L, 0x4b7428832c0547L },
        { 0x4f38accdf479abL, 0x01f6271c52a942L, 0xe3298f402ca9a7L,
          0x533dacab718fc8L, 0x133602ab093ca8L, 0xc04da808f98104L,
          0xd0f2e23af08620L, 0x882c817178b164L }
    },
    {
        { 0x28e6678ec30a71L, 0xe646879f78aca1L, 0x868a64b88fa078L,
          0x671030afee3433L, 0xb2a06bb87c0211L, 0x202eca946c406aL,
          0x64d6284e4f0f59L, 0x56ae4a23c9f907L },
        { 0x5abbb561dcc100L, 0x6fef6cf07c7784L, 0xb6e25cddb7302dL,
          0xa26785b42980e8L, 0xe7d4043fb96801L, 0x46df55d8e4282bL,
          0x9c0a5f5c602d6eL, 0xf06560475dfe29L }
    },
    {
        { 0x0e82a1a3dcbc90L, 0xb1ee285656feacL, 0xfa4353b0d3d3b2L,
          0xc2e7a6edd5c5dfL, 0x13707e1416ce53L, 0xc84ce0787ebc07L,
          0xdd273ce8a9a834L, 0x432a6175e8e1e7L },
        { 0xa359670bd0064aL, 0xc899dd56534516L, 0x666560edb27169L,
          0x1537b22a19a068L, 0x3420507eac7527L, 0x479f25e6fc13a7L,
          0xc847acc1bc19b3L, 0xecdecf00b20d45L }
    },
},
{
    {
        { 0x6f241004acea57L, 0xdace1c6da68597L, 0xea7dd4150ce77fL,
          0x1aecb841585884L, 0x92ff208ea4a85cL, 0xde9433c88eebd2L,
          0x53cd3183f4d289L, 0x397085826539afL },
        { 0x4b57599b827d87L, 0xdc82ac03d77638L, 0x694336652f6e61L,
          0xb8fc4b0ad5e8a6L, 0x1b6f7dcf388642L, 0x6f24533a74dd57L,
          0xc66937841750cfL, 0x06757eb28a37afL }
    },
    {
        { 0x0e70d53c133995L, 0x88a5e0c7c8c97dL, 0x4e59dbf85f3be3L,
          0x0f364ac0e92698L, 0x3a1e79bef6940fL, 0xc8a3941d85d23aL,
          0x143bb999a00e58L, 0x61cf7d6c6f2f10L },
        { 0x979c99485150feL, 0xcfd0df259d773fL, 0xce97b9daab7bcdL,
          0xc9fff8e6afd8fcL, 0x246befd89a4628L, 0xf6302821567090L,
          0x15393426749c58L, 0xff47d0ea0f3fd3L }
    },
    {
        { 0x09b0bfd35f6706L, 0x74645812c82e69L, 0xb60729f50d5fe9L,
          0xf13324595c74f1L, 0x33647e3bb76c89L, 0x01264045a9afccL,
          0x46d57ee0f154abL, 0x2efa55525680a4L },
        { 0x12ebfc65329d90L, 0xcb37ae579800afL, 0x5bb53496f8e310L,
          0x9b59c63f1bb936L, 0x5b49baaf4610e9L, 0x2bbeeef4f2d6acL,
          0x87ee21e0badc67L, 0x12e2aadf1ddfa0L }
    },
    {
        { 0x5b4668fa9109eeL, 0xfa951338a6cea2L, 0xe45e6fc4068e16L,
          0x8ae9a0c0205ed8L, 0x2993b96679b79bL, 0xc6b878fed604d3L,
          0x01d020832c77f3L, 0xd45d890495a1abL },
        { 0x99348fa29d2030L, 0x961f9a661f8f7aL, 0xfd53212674f74bL,
          0x45cee23b3e72bcL, 0x3fccb86b77e2d5L, 0xdff03104219cb7L,
          0x233771dc056871L, 0x1214e327d2c521L }
    },
    {
        { 0x9f51e15ff2a8e1L, 0x86571c5138bc70L, 0xbfc4caf0c09d46L,
          0x65e33fec2a0c18L, 0x8214392426867dL, 0x51ce6c080ae4edL,
          0x6cbe8d7b110de6L, 0x7f6e947fd22ea4L },
        { 0x7373a75cadefc4L, 0x6fca1d2b0c682fL, 0xcd2140df3c7c1eL,
          0x8653a37558b7a5L, 0x653e74e55eb321L, 0xbe0c6b3c31af73L,
          0x3376379f4fc365L, 0x3570b3771add4dL }
    },
    {
        { 0x9061ec183c3494L, 0xaf2f28d677bc95L, 0x6fe72793bf8768L,
          0xc5f50e30fa86d8L, 0x6c03060a3293ceL, 0x4d53357e2355a6L,
          0x43a59eae4df931L, 0x6f48f5d13b79c6L },
        { 0xa4d073dddc5192L, 0x6d0e318a65773fL, 0x1008792765de9eL,
          0xa724ed239a0375L, 0x510ff1497d7c9eL, 0x251f6225baa863L,
          0x86464fe648a351L, 0xf85e98fd50fd91L }
    },
    {
        { 0x29c963486ee987L, 0x93e8e5210dcc9fL, 0xa1fc4d1c910b1fL,
          0x015acacfeb603eL, 0xc9f25f80844a5fL, 0x50de93c73f4dacL,
          0x1758783310a4aaL, 0x544d570358f106L },
        { 0x4eeec7b1dc68caL, 0x6238e6fe00fbcbL, 0x34d394cb4e83c9L,
          0x764ffa22292656L, 0x5614cd1f641f2eL, 0x4252eb69e07234L,
          0xcbaef4568d2ba4L, 0x8c9c5508a98b17L }
    },
    {
        { 0xf235d9d4106140L, 0x1bf2fc39eb601eL, 0x6fb6ca9375e0c3L,
          0x4bf5492c0024d2L, 0x3d97093eb54cc6L, 0xc60931f5c90cb5L,
          0xfa88808fbe0f1aL, 0xc22b83dd33e7d4L },
        { 0x9cfec53c0abbf5L, 0x52c3f0a93723dfL, 0x0622b7e39b96b6L,
          0x300de281667270L, 0x50b66c79ef426aL, 0x8849189c6eb295L,
          0xeaec3a98914a7eL, 0x7ed56b0c4c99e0L }
    },
},
{
    {
        { 0x7926403687e557L, 0xa3498165310017L, 0x1b06e91d43a8fdL,
          0xf201db46ac23cbL, 0x6f172ad4f48750L, 0x5ed8c8ce74bd3eL,
          0x492a654daba648L, 0x123010ba9b64ffL },
        { 0xa83125b6e89f93L, 0x3a3b0b0398378aL, 0x9622e0b0aebe7cL,
          0xb9cbfdc49512a4L, 0x13edffd6aaf12aL, 0x555dff59f5eafdL,
          0x3cba6fe1212efaL, 0xd07b744d9bb0f8L }
    },
    {
        { 0x45732b09a48920L, 0xf3080fc13ff36dL, 0x9347395de8f950L,
          0x14d025a382b897L, 0x60c5a7404d72adL, 0x30be7e511a9c71L,
          0x43ffabd31ac33aL, 0x97b06f335cbb14L },
        { 0xe4ff5c57740de9L, 0x5fed090aacf81eL, 0x97196eee8b7c9dL,
          0x316dcd1045910bL, 0x7a2b2f55ad8c63L, 0x674fffdc5b03bbL,
          0xc1cd133e65953cL, 0x3c060520a83556L }
    },
    {
        { 0x797c3f6091c23dL, 0x2ea2de339c9c05L, 0x5d958b4a31f67cL,
          0xf97afe5d5f088cL, 0xbcfbd2a0b37243L, 0xc43ad3eeca630cL,
          0xb92a33742845e0L, 0x970bff7a9a0f16L },
        { 0x86355115970a79L, 0xcee332ef205928L, 0x2c58d70c04c208L,
          0xdbfe19a3f5e5bfL, 0x8f8f2c88e51c56L, 0xb61f58e8e2da75L,
          0x4046a19624d93fL, 0x7de64dbe1f9538L }
    },
    {
        { 0xd018e1cc2d850eL, 0x8cdb64363a723cL, 0x9a65abe90a42afL,
          0xfeece9616f20ccL, 0xc906800d5cff56L, 0x0acf23a3f0deedL,
          0x2143061728dd3aL, 0x66276e2b8ce34cL },
        { 0x23700dc73cc9c7L, 0xdb448515b1778bL, 0x330f41e4aab669L,
          0x2f5aabcf5282a4L, 0xff837a930f9e01L, 0x1a1eb2f901cc98L,
          0xd3f4ed9e69bd7fL, 0xa6b11418a72a7dL }
    },
    {
        { 0x34bde809ea3b43L, 0x5ddcb705ced6aeL, 0x8257f5b95a6cb8L,
          0xaac205dc77dcb8L, 0x77d740d035b397L, 0xca7847fcf7e0a6L,
          0x9404dd6085601bL, 0x0a5046c457e4f9L },
        { 0xcaee868bc11470L, 0xb118796005c5f6L, 0xcc04976ec79173L,
          0x7f51ba721f6827L, 0xa8e3f0c486ff7eL, 0x327163af87838cL,
          0xcf2883e6d039fdL, 0x6fb7ab6db8b0e2L }
    },
    {
        { 0x8ca5bac620d669L, 0xff707c8ed7caa9L, 0xdaefa2b927909bL,
          0x1d2f9557029da3L, 0x52a3ba46d131a0L, 0xe5a94fd3ab1041L,
          0x508917799bc0aeL, 0xf750354fa1bd16L },
        { 0xdd4e83a6cd31fdL, 0xd33505392fac84L, 0xf914cbc1691382L,
          0x669683fda6ade6L, 0x69446438878513L, 0x429d3cc4b1a72dL,
          0x655c46a61eec36L, 0x881eded4bc4970L }
    },
    {
        { 0x5b39d377ca647fL, 0x41533c1e917b34L, 0xea2aeb57daf734L,
          0xf1ef1eb1286560L, 0x582f2e008e0473L, 0x5913d7d5edc74aL,
          0x588c7ec3c1e754L, 0xbd6db057146fe1L },
        { 0x3b0d49e7634907L, 0x4c65ce4e43b9ccL, 0xb87e9582d92d5bL,
          0x05135727ab1519L, 0x03ec0848c3aed0L, 0x4d7aa21561a641L,
          0xe5f821199e92adL, 0x379b55f48a457cL }
    },
    {
        { 0x8317c34d6a8442L, 0xb0ab4a5ae499daL, 0xebcb16e720e8ebL,
          0xfd5c5639a96908L, 0xcab4d67ad23acfL, 0xa600a79bcdf748L,
          0x18a6340a2a6a51L, 0xf2f415c3aabd69L },
        { 0xdb38a4f747258aL, 0xb6ea5602e24415L, 0xfad1ea9f1f7655L,
          0x4e27eb5c957684L, 0xf8283e1b2e1cfcL, 0x8f83bd6aa6291cL,
          0x28d23b55619e84L, 0xb9f34e893770a4L }
    },
},
{
    {
        { 0x1bb84377515fb1L, 0xac73f2a7b860a6L, 0x78afdfa22b390fL,
          0x815502b66048aaL, 0xf513b9785bf620L, 0x2524e653fc5d7cL,
          0xa10adc0178c969L, 0xa1d53965391c8dL },
        { 0x09fccc5a8bcc45L, 0xa1f97d67710e1eL, 0xd694442897d0a1L,
          0x7030beb5f42400L, 0xdebe08c7127908L, 0x96b715c2187637L,
          0xc598250b528129L, 0x0f62f45a1ccb07L }
    },
    {
        { 0x8404941b765479L, 0xfdecff45837dc4L, 0x1796372adbd465L,
          0x5f84c793159806L, 0x6d2e46b6aaad34L, 0xd303b4a384b375L,
          0x440acd5b392002L, 0x4f2a4a7c475e87L },
        { 0x038e1da5606fc2L, 0x2d821c29c2f050L, 0xc074cb3f139db4L,
          0xde2fee74ec59beL, 0x5a819eea84ed59L, 0xd65c62c3e98711L,
          0x72eb440b9723c1L, 0xb92775401be611L }
    },
    {
        { 0x929fe64ab9e9fcL, 0x04379fd0bf1e85L, 0xb322093bc28ee3L,
          0x78ac4e2e4555e1L, 0xdb42b58abc5588L, 0x1c1b5e177c8b12L,
          0xf6d78dd40366c4L, 0xc21ff75bdae22eL },
        { 0x1e3d28ea211df2L, 0xc5a65a13617c0aL, 0x3fa02c058140d5L,
          0x155c346b62d10cL, 0xc9cf142e48268fL, 0xdc140831993bc3L,
          0x07c44d40ee69dcL, 0x61699505e2ac46L }
    },
    {
        { 0x44e4a51d0fb585L, 0x00846bef1f3ce8L, 0xedef39a8e2de1eL,
          0x430afe333b3934L, 0xac78b054337188L, 0x0f39de4c9a3f24L,
          0x039edddc9ae6a4L, 0xf4701578eacd51L },
        { 0x1e396949a2f31aL, 0xc8a40f4b19a8b1L, 0xdddd10c9d239d8L,
          0xf974245887e066L, 0xfdb51113ea28c6L, 0xb5af0fbe1122a9L,
          0xd30c89f36e0267L, 0x7b1c0f774f024cL }
    },
    {
        { 0x1ec995607a39bfL, 0x1c3ecf23a68d15L, 0xd8a5c4e4f59fe9L,
          0xacb2032271abc3L, 0xbc6bdf071ef239L, 0x660d7abb39b391L,
          0x2e73bb2b627a0eL, 0x3464d7e248fc7eL },
        { 0xaa492491666760L, 0xa257b6a8582659L, 0xf572cef5593089L,
          0x2f51bde73ca6bfL, 0x234b63f764cff5L, 0x29f48ead411a35L,
          0xd837840afe1db1L, 0x58ec0b1d9f4c4bL }
    },
    {
        { 0x8e1deba5e6f3dcL, 0xc636cf406a5ff7L, 0xe172b06c80ca0fL,
          0x56dc0985ffb90aL, 0x895c2189a05e83L, 0x6ddfaec7561ac2L,
          0xaa3574996283a0L, 0x6dfb2627e7cd43L },
        { 0x6576de52c8ca27L, 0x6a4a87249018ebL, 0x00c275c5c34342L,
          0xe34805ad2d90c4L, 0x651b161d8743c4L, 0xb3b9d9b7312bf3L,
          0x5d4b8e20bf7e00L, 0x8899bdf78d3d7eL }
    },
    {
        { 0x9644ad8faa9cd1L, 0x34c98bf6e0e58eL, 0x6022aad404c637L,
          0x2a11a737ac013bL, 0x5bdd1035540899L, 0x2e675721e022a4L,
          0xe32045db834c33L, 0x74a260c2f2d01cL },
        { 0x20d59e9c48841cL, 0x05045dde560359L, 0xeba779cac998acL,
          0x5bed10c00a6218L, 0x25d4f8e5327ef4L, 0xa2784744597794L,
          0xefd68ca831d11eL, 0x9ad370d934446aL }
    },
    {
        { 0x3089b3e73c92acL, 0x0ff3f27957a75cL, 0x843d3d9d676f50L,
          0xe547a19d496d43L, 0x68911c98e924a4L, 0xfab38f885b5522L,
          0x104881183e0ac5L, 0xcaccea9dc788c4L },
        { 0xfbe2e95e3c6aadL, 0xa7b3992b3a6cf1L, 0x5302ec587d78b1L,
          0xf589a0e1826100L, 0x2acdb978610632L, 0x1e4ea8f9232b26L,
          0xb21194e9c09a15L, 0xab13645849b909L }
    },
},
{
    {
        { 0x92e5d6df3a71c1L, 0x349ed29297d661L, 0xe58bd521713fc9L,
          0xad999a7b9ddfb5L, 0x271c30f3c28ce0L, 0xf6cd7dc2a9d460L,
          0xaf728e9207dec7L, 0x9c2a532fcb8bf0L },
        { 0xd70218468bf486L, 0x73b45be7ab8ea8L, 0xddfc6581795c93L,
          0x79416606bb8da2L, 0x658f19788e07a2L, 0xa9d5b0826d3d12L,
          0x4d7c95f9535b52L, 0xad55e25268ef8aL }
    },
    {
        { 0x94a9b0ba2bc326L, 0x485ecc5167e5f3L, 0x8340bc7c97fc74L,
          0x06f882b07aaa5cL, 0x4b57455849698aL, 0xd9281ebb36a0baL,
          0x8918c6c8b8108fL, 0xedd1eea5b50d1dL },
        { 0x94d737d2a25f50L, 0x0e5a8232446ad0L, 0x02a54357ced3e2L,
          0xb09a92a4af8cedL, 0x85fc498eeecef2L, 0x06a02b9e71e3d4L,
          0x00ad30784bb49aL, 0xf61585e64a5b4aL }
    },
    {
        { 0x915f6d8b86a4c9L, 0x944bc6ba861e1fL, 0x3091ca754465efL,
          0x11df859eb53a38L, 0xd44dde50144679L, 0x6c8da9a0994eddL,
          0xeebcebf91241efL, 0xc419354c2f6859L },
        { 0x1f4969349581b6L, 0x5712b10bb26cb4L, 0x8fcaa41b09fd59L,
          0xbd39aad72e22e3L, 0xf70e794b1199b0L, 0xdf63c0cc6f863dL,
          0xd58166fee9df4fL, 0xb9224eac45e70bL }
    },
    {
        { 0x80072face525f4L, 0x8597bd666a5502L, 0xf65e203dbc9725L,
          0xeccfbe3f2222a4L, 0x490aa422339834L, 0x134889162489e8L,
          0xaff3f80a735084L, 0x69d53d2f3f1bd6L },
        { 0xb123ffc813341aL, 0x359084c1173848L, 0x751425ed29b08dL,
          0x1edda523890ad4L, 0xb64974c607cf20L, 0xa8c8cb8b42ac7cL,
          0xd5cb305edd42e5L, 0xf3034dc44c090aL }
    },
    {
        { 0x428921dbb18e19L, 0x4cfd680fed2127L, 0x671144d92ac8c3L,
          0x2121901132c894L, 0x25d0e567604cd9L, 0xa372223afbc2a0L,
          0xcf98a5256c16f7L, 0x71f129ab5459e1L },
        { 0xf4afdc5b668b2eL, 0xc5d937a0c2d410L, 0xe2cc4af285d54aL,
          0x1c827778c53e18L, 0x270f2c369a92f6L, 0x799f9ac616327aL,
          0xce658d9d4246f2L, 0x0fb681ffb12e36L }
    },
    {
        { 0xc5ab11ee0690feL, 0x80261e33f74249L, 0x8eb4b4758c1cf2L,
          0x4895a80184ae9bL, 0x4a4bdb6d3e27ebL, 0xa7a1638bfd251cL,
          0x29ec144417a7e3L, 0xd0736093f1b960L },
        { 0xcb1ed8349c73d1L, 0x33fc84a8d1945aL, 0x9f668dbe965118L,
          0x3331743a82811fL, 0xf394dec28ba540L, 0x44ce601654a454L,
          0x240dbb63623645L, 0xf07e7f22e61048L }
    },
    {
        { 0x7c9f1763d45213L, 0x3eefa709c1f77fL, 0xde3c3c51b48350L,
          0x4a2bc649d481a7L, 0xfd4a58a7874f3dL, 0x96655d4037b302L,
          0x945252868bf5abL, 0x1b6d46a75177f6L },
        { 0x7de6763efb8d00L, 0xb2c1ba7a741b7bL, 0xcca6af47bae6edL,
          0xe4378ca5b68b3fL, 0xfb757deaf71948L, 0x7f07b5ebc6ac99L,
          0x752a56827d636dL, 0xc8b7d1d4b8a34fL }
    },
    {
        { 0x76cb78e325331bL, 0x41f41c9add2eedL, 0x03db2385c5f623L,
          0xbbc1d177102fa2L, 0x80f137a60182ecL, 0xfdd856955adf15L,
          0x4f53f5ee3373dcL, 0xec6faf021b669bL },
        { 0x7d4e9830b86081L, 0x10d3cd9f2d979cL, 0x0f48f5824a22c8L,
          0x86c540c02f99eeL, 0xf4c66545e6c5fcL, 0xaf0c588bc404c8L,
          0x2e6edbd423118aL, 0x86e32e90690eabL }
    },
},
{
    {
        { 0x1d12656dfbfa6fL, 0xa4980957646018L, 0x2f1071bc3597d0L,
          0x3df83f91dda80aL, 0x5853e28f3ae449L, 0xb853d319e19aadL,
          0x863f01ba0d8a46L, 0xa84fca62fef108L },
        { 0xbe4c0b7fb84de9L, 0x40a03dcc0727bfL, 0x781f841b18575cL,
          0x6a63045466cddbL, 0x6be758205dc7a2L, 0x420f87f07ae811L,
          0x28082423bf96c8L, 0x723998c51c6821L }
    },
    {
        { 0x38ab64181f5863L, 0xd82ecbd05ff9e1L, 0x339c94ea065856L,
          0x143054aa45156dL, 0xe6d64bf065628cL, 0xe530086a938589L,
          0x22d3a49385d79bL, 0x0b107900ab8245L },
        { 0xb0d80fbca387b5L, 0x698206e35551d7L, 0x199685da10bb73L,
          0xa8e5fa89107378L, 0x36e5724d99dbbfL, 0xd67f476d581b03L,
          0x7a15be788dd1e6L, 0x8dac8e4e5baa31L }
    },
    {
        { 0x4d5d88fe170ef8L, 0xb6ba5de1e9e600L, 0x4a89d41edeabc5L,
          0x737c66b8fac936L, 0x8d05b2365c3125L, 0x85a5cbcb61b68eL,
          0x8fea62620a6af9L, 0x85115ded8b50ecL },
        { 0x5430c8d6a6f30bL, 0x8bef9cf8474295L, 0x0648f5bbe77f38L,
          0xfe2b72f9e47bd7L, 0xad6c5da93106e2L, 0x4fa6f3dfa7a6c3L,
          0xdcd2ed8b396650L, 0x7de1cce1157ef9L }
    },
    {
        { 0x70a5f6c1f241d1L, 0x6c354d8798cd5cL, 0x23c78381a729fbL,
          0xcff8f15523cbdaL, 0x5683ff43493697L, 0xef7dbab7534f53L,
          0xd7bd08e2243d53L, 0x6f644cbf8072a9L },
        { 0xac960f9b22db63L, 0xa97f41723af04dL, 0x692b652d9798afL,
          0x0e35967fedb156L, 0x14b5e50dfe6ee8L, 0x7597edeb411070L,
          0x116f3ce442b3f9L, 0xe9b5ae81b2b6dbL }
    },
    {
        { 0xf4385ee2315930L, 0xc8d029827a8740L, 0x7907a8dd934a43L,
          0x20bc946c582191L, 0xa4acb3e6a405e7L, 0x8c1d6c843df2f5L,
          0x9df1593991f0b5L, 0xbb9df984d9be9dL },
        { 0x63620088e4b190L, 0xee1421eada3a88L, 0xb84f0ccf93b027L,
          0x7a5d6678e95091L, 0x3974462f3e3704L, 0xfa6fb5ec593e98L,
          0x44b6cf7a6477d2L, 0xe885b57b09a562L }
    },
    {
        { 0x6e339e909a0c02L, 0x57afff00e75f29L, 0x797d8d6fb7db03L,
          0xc6e11a3d25a236L, 0x643ce1c0107260L, 0xe644ec462eae1cL,
          0x821d5b83f5a3f5L, 0xa8ad453c0579d6L },
        { 0x6518ed417d43a4L, 0x46e76a53f87ccdL, 0xd6cbaabf9bef95L,
          0x25688324f7cbcfL, 0x367159a08476b4L, 0x1d1b401be6d324L,
          0x348cb98a605026L, 0x144f3fe43b6b1eL }
    },
    {
        { 0xbabbd787b1822cL, 0xd34ba7e2aa51f8L, 0x086f1cc41fbea4L,
          0x96f7eac746f3d9L, 0xad97f26281ecafL, 0x751a905a14ee2cL,
          0xb4e7fe90d7335fL, 0x0d97b8f4892ff0L },
        { 0xdb8a3155a5c40eL, 0x64e5de77ba567bL, 0x4f155f71eefe88L,
          0xe2297e9fb6fbf4L, 0xfe24bf96c16be5L, 0x2251847cdd83e2L,
          0x13ac2c85eda444L, 0x49d1b85283275fL }
    },
    {
        { 0xca08731423e08fL, 0x7046bb087d2f14L, 0x876f10c3bc846cL,
          0x2202b76358fbe3L, 0x0d4fc1c0e26ac6L, 0x1fc748bb986881L,
          0x609e61c8384a18L, 0x28a72d60d88e00L },
        { 0x1332a3178c6e2fL, 0x0367919b3526a4L, 0x53989e4698fe3eL,
          0x14b1145b16a99bL, 0xef9ec80ddbb75fL, 0x76256240e53955L,
          0x54e087a8744ae1L, 0xce50e8a672b875L }
    },
},
{
    {
        { 0x4c88b2ba29629cL, 0x946559c7b2642fL, 0x933d432f7ebe4cL,
          0x97109b663632c9L, 0x799b3fbe53184dL, 0xd4628710f069a6L,
          0x0c182a13a68351L, 0x974a8399a2437aL },
        { 0x29f19972a70278L, 0x01b98b6d9c424bL, 0xd85a60b08f4c37L,
          0xcc3523f2b1da15L, 0xf922115ddffb0fL, 0xee0fe4dde84ae2L,
          0x810440c55365beL, 0xd2f66391a457e8L }
    },
    {
        { 0x5e6879fe2ddd05L, 0x92a7545abdfc61L, 0x7dedd63a5cede8L,
          0x8a03b3f70df4bdL, 0xa5d1f6591f6cbbL, 0x372fde610f3fb2L,
          0x4537f9ea9dee05L, 0x7eb85bbdf7aa50L },
        { 0x963edf8e8c504dL, 0x53c8dcae7bdb6bL, 0xa246e4c6fedf2dL,
          0x75533400c55bdeL, 0x2aa748d0270a54L, 0xadb6cf005860ddL,
          0x8d314509b84763L, 0x626720deb405efL }
    },
    {
        { 0xa3709ae6601328L, 0x68e94fd2ac2478L, 0x38793439d5d247L,
          0xfa467af392c198L, 0x49e7b0d15df607L, 0x8c5812261792a8L,
          0x79f76581d3762fL, 0xaa38895244a39dL },
        { 0xef60af9c5cd0bcL, 0x2b0db53a33b3bbL, 0xe3e0b1f251015dL,
          0xc608afce64489eL, 0xe52b05703651aaL, 0x1dda8b91c6f7b9L,
          0x833f022ff41893L, 0x58eb0a0192818cL }
    },
    {
        { 0x6c1300cfc7b5a7L, 0x6d2ffe1a83ab33L, 0x7b3cd019c02eefL,
          0x6c64559ba60d55L, 0x2e9c16c19e2f73L, 0x11b24aedbe47b1L,
          0xc10a2ee1b8153bL, 0x35c0e081e02e1aL },
        { 0xa9f470c1dd6f16L, 0x4ea93b6f41a290L, 0xac240f825ee03fL,
          0x6cd88adb85aabdL, 0x378a64a1be2f8fL, 0xbf254da417bac1L,
          0x7e4e5a59231142L, 0x057aadc3b8c057L }
    },
    {
        { 0x607c77a80af479L, 0xd3e01ff5ccdf74L, 0x9680aaf101b4c7L,
          0xd2a7be12fc50a6L, 0x92a788db72d782L, 0x35daf2e4640b52L,
          0xc170d6939e601cL, 0x16e05f57b25c2fL },
        { 0x47a42a66fe37f8L, 0xeb74271beca298L, 0x401e11e179da16L,
          0xfb8da82aa53873L, 0xd657d635bb4783L, 0x6847758fcea0b1L,
          0x2f261fb0993154L, 0x868abe3592853aL }
    },
    {
        { 0x1a4c54335766abL, 0xa1c84d66f4e4eaL, 0x5d737a660ba199L,
          0x4a7b1e298b15a2L, 0x207877ffd967d3L, 0xcaec82dc262b4dL,
          0x0b278494f2a37dL, 0x34781416ac1711L },
        { 0x28e3df18fc6856L, 0xbec03f816d003fL, 0x2bd705bff39ebdL,
          0x1dcb53b2d776d3L, 0xabafa7d5c0e7ceL, 0x5b9c8c24a53332L,
          0xe9f90d99d90214L, 0x789747ec129690L }
    },
    {
        { 0x94d3c3954e2dfaL, 0x919f406afb2a8fL, 0x159ef0534e3927L,
          0xcdb4d14a165c37L, 0xa23e5e8288f337L, 0x95867c00f90242L,
          0x2528150e34e781L, 0x104e5016657b95L },
        { 0x695a6c9bcdda24L, 0x609b99523eb5faL, 0xcbce4f516a60f8L,
          0xec63f7df084a29L, 0x3075ada20c811fL, 0x129a1928c716a1L,
          0xd65f4d4cd4cd4aL, 0xe18fa9c62188beL }
    },
    {
        { 0x1672757bac60e3L, 0x525b3b9577144bL, 0x38fc997887055bL,
          0x7a7712631e4408L, 0x884f173cba2fcfL, 0x783cbdc5962ac0L,
          0x4f3ed0a22287dcL, 0x8a73e3450e20e6L },
        { 0xe7a1cd0d764583L, 0x8997d8d0d58ee6L, 0x0ea08e9aa13ed6L,
          0xed478d0cf363cbL, 0x068523d5b37bf4L, 0x8b5a9e8783f13cL,
          0xde47bbd87528a9L, 0xd6499cccaec313L }
    },
},
{
    {
        { 0x54781bbe09859dL, 0x89b6e067f5e648L, 0xb006dfe7075824L,
          0x17316600717f68L, 0x9c865540b4efe2L, 0xdbdb2575e30d8eL,
          0xa6a5db13b4d50fL, 0x3b5662cfa47bebL },
        { 0x9d4091f89d4a59L, 0x790517b550a7dcL, 0x19eae96c52965eL,
          0x1a7b3c5b5ed7a4L, 0x19e9ac6eb16541L, 0x5f6262fef66852L,
          0x1b83091c4cda27L, 0xa4adf6f3bf742bL }
    },
    {
        { 0x8cc2365a5100e7L, 0x3026f508592422L, 0xa4de79a3d714d0L,
          0xefa0d3f90fcb30L, 0x126d559474ada0L, 0xd68fa77c94350aL,
          0xfa80e570c7cb45L, 0xe042bb83985fbfL },
        { 0x51c80f1fe13dbaL, 0xeace234cf055d7L, 0x6b8197b73f95f7L,
          0x9ca5a89dcdbe89L, 0x2124d5fdfd9896L, 0x7c695569e7ca37L,
          0x58e806a8babb37L, 0x91b4cc7baf99ceL }
    },
    {
        { 0x874e253197e968L, 0x36277f53160668L, 0x0b65dda8b95dbeL,
          0x477a792f0872a1L, 0x03a7e3a314268dL, 0xa96c8420c805c7L,
          0xb941968b7bc4a8L, 0x79dce3075db390L },
        { 0x577d4ef6f4cc14L, 0x5b0d205b5d1107L, 0x64ff20f9f93624L,
          0x0b15e315034a2fL, 0x3a0f6bb8b6f35cL, 0x0399a84e0d0ec5L,
          0xd0e58230d5d521L, 0xdeb3da1cb1dd54L }
    },
    {
        { 0x24684ae182401aL, 0x0b79c1c21a706fL, 0xe1d81f8d8998afL,
          0xadf870f4bb069fL, 0xd57f85cf3dd7aaL, 0x62d8e06e4a40f8L,
          0x0c5228c8b55aa1L, 0xc34244aa9c0a1aL },
        { 0xb5c6cf968f544eL, 0xa560533de23ab7L, 0xaa5512047c690cL,
          0x20eda5b12aaaa6L, 0xea0a49a751a6a0L, 0x6d6cfff2baa272L,
          0x95b756ebf4c28aL, 0xd747074e6178a4L }
    },
    {
        { 0xa27b453221a94bL, 0xd56ad13e635f20L, 0x03574b08c95117L,
          0xf0ee953ed30b70L, 0xb48d733957796fL, 0xf5d958358c336bL,
          0x6170cd882db529L, 0xcd3ef00ec9d1eaL },
        { 0xd1bea0de4d105fL, 0xd2d670fad6a559L, 0x652d01252f9690L,
          0x5f51fb2c2529b0L, 0x5e88bf0e89df2aL, 0x9a90684cd686e4L,
          0xf519ccd882c7a1L, 0x933a0dfc2f4d37L }
    },
    {
        { 0x0720a9f3f66938L, 0x99356b6d8149dfL, 0xb89c419a3d7f61L,
          0xe6581344ba6e31L, 0xd130561ab936c8L, 0x0625f6c40dbef1L,
          0x7b2d6a2b6bb847L, 0x3ca8b2984d506bL },
        { 0x6bf729afb011b0L, 0x01c307833448c9L, 0x6ae95080837420L,
          0xf781a8da207fb8L, 0xcc54d5857562a9L, 0xc9b7364858c5abL,
          0xdfb5035359908fL, 0x8bf77fd9631138L }
    },
    {
        { 0xf523365c13fbb1L, 0x88532ea9993ed5L, 0x5318b025a73492L,
          0x94bff5ce5a8f3cL, 0x73f9e61306c2a0L, 0x00abbacf2668a3L,
          0x23ce332076237dL, 0xc867f1734c0f9bL },
        { 0x1e50995cfd2136L, 0x0026a6eb2b70f8L, 0x66cb1845077a7dL,
          0xc31b2b8a3b498eL, 0xc12035b260ec86L, 0x1cbee81e1b3df0L,
          0xfd7b8048d55a42L, 0x912a41cf47a8c8L }
    },
    {
        { 0xab9ffe79e157e3L, 0x9cfe46d44dc158L, 0x435551c8a4a3efL,
          0x638acc03b7e3a8L, 0x08a4ebd49954a7L, 0x295390c13194f7L,
          0x3a2b68b253892aL, 0xc1662c225d5b11L },
        { 0xcfba0723a5d2bbL, 0xffaf6d3cc327c9L, 0x6c6314bc67e254L,
          0x66616312f32208L, 0xf780f97bea72e1L, 0x495af40002122fL,
          0x3562f247578a99L, 0x5f479a377ce51eL }
    },
},
{
    {
        { 0x91a58841a82a12L, 0xa75417580f3a62L, 0x399009ff73417aL,
          0x2db1fb90a8c5cdL, 0x82c8912c046d51L, 0x0a3f5778f18274L,
          0x2ad0ede26ccae2L, 0x7d6bd8b8a4e9c2L },
        { 0xaa0d7974b3de44L, 0xf8658b996ac9bbL, 0x31e7be25f6c334L,
          0x23836ce4df12c9L, 0x029027b59eb5c9L, 0x2f225315b8649dL,
          0xa0fdf03d907162L, 0x101d9df9e80226L }
    },
    {
        { 0xf12037a9a90835L, 0xd2d0882f0222a7L, 0xeaf8d40c3814e2L,
          0xa986dc68b8146bL, 0x147a3318504653L, 0x734e0032feaf67L,
          0x6f27bbf602bec5L, 0xa1e21f16a688f3L },
        { 0x5a8eeab73c4ae5L, 0x4dbaddbe70b412L, 0x871cebacfd2af1L,
          0x18603827d7a286L, 0x024059db5bb401L, 0x2557c093c39b73L,
          0xfc5a7116681697L, 0xf881c0f891b57cL }
    },
    {
        { 0x3c443f18ea191aL, 0x76faa58d700ad0L, 0x6fe6cfabe7fcbfL,
          0xaefc5288990ef7L, 0x44e30fa80004ccL, 0xc744adc6d8ef85L,
          0xafcd931912df70L, 0xf62a9d1572a6d8L },
        { 0x47158a03219f27L, 0x76fb27ead73136L, 0x41bb2adcc2d614L,
          0x8858cb9de1ec21L, 0xab402c45f15866L, 0x6675d5bbc82bbfL,
          0x4ee9dd6f1b28d3L, 0x875884fe373c17L }
    },
    {
        { 0x17806dd2a67d36L, 0xaa23a8632c9ec1L, 0xd914126fc1ee55L,
          0xbf8f7bd653701bL, 0x9b0111aea71367L, 0x61fd4aba98e417L,
          0xeb45298561c5a5L, 0x2187b0ae7af394L },
        { 0x71f12db1616ddeL, 0x061760907da7b4L, 0x414d37602ddb04L,
          0x1100be7286fb58L, 0xd7cf88d6f0d95bL, 0x8539d23746d703L,
          0xdccc9d64e23d73L, 0xaeef1d2ec89680L }
    },
    {
        { 0x82ccf1a336508dL, 0xa128c1f5bad150L, 0x551d8c029a188dL,
          0xef13dd4771404fL, 0xdd67696c37b993L, 0x428c0e20dddad2L,
          0x222278d038c94cL, 0x1a24a51078e3f2L },
        { 0xd297fe6edb0db9L, 0x00988d28251a87L, 0xbb946f8bfaa0d7L,
          0x380f7b9df45ea0L, 0x8526415afccf5eL, 0x909bfbfe9ec7bcL,
          0x2ed7093124755cL, 0x436802889404e2L }
    },
    {
        { 0x21b9fa036d9ef1L, 0xfd64b7ce433526L, 0xd9d7eb76544849L,
          0x201620cd5b54b3L, 0x25fab3dbb61159L, 0x90d4eb0c53e0d3L,
          0xba098319e74772L, 0x8749658ec1681cL },
        { 0xa354349fec316bL, 0x639a9b1a743ea2L, 0x2e514ca37c50e6L,
          0x9f4a4fddbaf6c5L, 0x0df87ef6f511c9L, 0xadd4cef0c00d95L,
          0x401c0ebaa1433fL, 0x3c3a59ebb38af9L }
    },
    {
        { 0x8706245f0e7dcaL, 0xad238cd3fb29caL, 0x03304439b7d8f0L,
          0xfdcd6e6154f495L, 0xc67e24a7d4ad09L, 0x1b209e85438390L,
          0xf893b81b0c211eL, 0x1aa86f07e11e36L },
        { 0x2cca3ffedea8b1L, 0x7eedd073b306cdL, 0x78e37bc12ee222L,
          0x257870bbc42a1dL, 0x5fb2bb91fbd397L, 0x470247009d6c60L,
          0x11748a320bdc36L, 0x3ff24dc04280e8L }
    },
    {
        { 0x0eb1c679839b52L, 0x5bcca27acfbd32L, 0xb506c1674898e3L,
          0x37d662e2489e5eL, 0x8dc0731f694887L, 0x571149ef43f1dcL,
          0x6430a3766d63dcL, 0x0d2640eb50dd70L },
        { 0x2b561493b2675bL, 0x1b4806588c604fL, 0x55c86a8aafbabcL,
          0xa7b9447608aabaL, 0xa42f63504cad8cL, 0x0f72b1dcee7788L,
          0x1d68374755d99aL, 0xd7cdd8f5be2531L }
    },
},
{
    {
        { 0x67873bdbcdfee1L, 0xa5a0c0afcd0a3fL, 0x59389f93cfa3d4L,
          0x14e945ce1c865cL, 0x62d2f8e1d588ccL, 0xfd02f8a8e228b4L,
          0x208f791b42b649L, 0x0e0dff1ab397adL },
        { 0x30ac3d90bc6eb1L, 0xf14f16a5f313bbL, 0x70fa447e2a0ad2L,
          0x6e406855a0db84L, 0xd52282be32e1e7L, 0x315a02a15ca330L,
          0x9a57a70867c2feL, 0x55f07650054923L }
    },
    {
        { 0x2d729f6c0cf08fL, 0x6b80138ebaf57fL, 0x6285bcc0200c25L,
          0xee845192cd2ac7L, 0x28fce4d922778aL, 0x761325ccd1011cL,
          0xd01f2475100e47L, 0xc7a1665c60d8e1L },
        { 0x950966d7ceb064L, 0x0a88e8578420dbL, 0x44f2cfce096f29L,
          0x9d9325f640f1d2L, 0x6a4a81fd2426f1L, 0x3ed6b189c905acL,
          0xba3c0e2008854dL, 0x1df0bd6a0d321bL }
    },
    {
        { 0x0117ad63feb1e7L, 0xa058ba2f1ae02fL, 0x5eee5aa31b3f06L,
          0x540d9d4afacd4dL, 0x38992f41571d91L, 0xef2738ebf2c7deL,
          0x28bfcab92a798dL, 0x37c7c5d2286733L },
        { 0xb99936e6470df0L, 0x3d762d58af6a42L, 0xa8c357ac74eec5L,
          0x9917bebf13afbcL, 0x28f0941f2dc073L, 0x306abf36ce7df7L,
          0xa3c5f6fd6973c8L, 0x640209b3677632L }
    },
    {
        { 0xee872a2e23aef7L, 0xb497b6feb9b08eL, 0xfb94d973f33c63L,
          0x9ea1ff42b32315L, 0x537b49249a4166L, 0x89c7fe6ab4f8beL,
          0xf68007fdad8f0fL, 0xe56ef0b71b8474L },
        { 0x478b2e83f333f9L, 0x144e718b2607f5L, 0x13aa605a4c7ab5L,
          0xfc1fc991d0730dL, 0xe7a04375ab3ea1L, 0xc59986a306d8d3L,
          0x24f6111702a8b1L, 0x7741394e040ad2L }
    },
    {
        { 0x34c6a2560723a7L, 0x8aabd0df4ea691L, 0x9d676a55d7497fL,
          0x12c09577d91fa4L, 0x581c7a86479284L, 0xa54f3daf4fd449L,
          0x2f89f3c4ef44cfL, 0xfc266b5c9ec97cL },
        { 0xfcd3fbe88b142aL, 0x9f3109f4bd69c1L, 0x08839c0b5f5a6aL,
          0x63ca8502e68303L, 0x2f0628dbba0a74L, 0x743cccf5d56b54L,
          0xbd4b06613e09fdL, 0x7a8415bde2ba3eL }
    },
    {
        { 0x2234a3bc076ab2L, 0xd6953e54977a98L, 0xc12215831ebe2eL,
          0x632145fbad78e2L, 0xd7ba78aa5c4b08L, 0x6f4ea71998e32aL,
          0x25900d23485a63L, 0x97ac6286a5176fL },
        { 0x5df91181093f7bL, 0x2bf9829c844563L, 0x525d99d6272449L,
          0x4281cb5b5c8a18L, 0x35df2780544a08L, 0xf4c3d2dbaeb8f4L,
          0xc7ff3175230447L, 0x6b4d7645d2fbffL }
    },
    {
        { 0x4837f802b0c9cbL, 0xb65f8168ce8418L, 0xdf66ea99fc1428L,
          0x9788ee804ea7e8L, 0x9eae9008334e3cL, 0xbc91058d6ba1b6L,
          0x634aba1d7064b6L, 0x12d9bb3397b368L },
        { 0x0645c85c413aa8L, 0xb09dea6ac6b5e3L, 0x29a620d289a50bL,
          0x104db3bbbcceb1L, 0x42e479287b3309L, 0xdfc373eec97f01L,
          0xe953f94b93f84eL, 0x3274b7f052dfbfL }
    },
    {
        { 0x9d5670a1bd6fa9L, 0xec42fc9db6c4d4L, 0xaecd4ed1b42845L,
          0x4eed90e1b03549L, 0xeb3225cbbab1faL, 0x5345e1d28a2816L,
          0x3741cfa0b77d2aL, 0x712b19f7ea8caaL },
        { 0x42e6844661853eL, 0x4cf4126e4a6e5dL, 0x196a9cfc3649f6L,
          0x06621bcf21b6b1L, 0x887021c32e29eaL, 0x5703aeb8c5680fL,
          0x974be24660f6d7L, 0xaf09badc71864eL }
    },
},
{
    {
        { 0x3483535a81b6d3L, 0x19e7301ca037dcL, 0x748cab763ddfebL,
          0xe5d87f66f01a38L, 0xbba4a5c2795cd6L, 0x411c5d4615c36cL,
          0xff48efc706f412L, 0x205bafc4b519dfL },
        { 0xfcaa5be5227110L, 0x7832f463ad0af0L, 0x34ef2c42642b1bL,
          0x7bbef7b072f822L, 0x93cb0a8923a616L, 0x5df02366d91ba7L,
          0x5da94f142f7d21L, 0x3478298a14e891L }
    },
    {
        { 0xad79a0fc831d39L, 0x24d19484803c44L, 0x4f8a86486aeeb2L,
          0x0ca284b926f6b9L, 0x501829c1acd7cdL, 0x9f6038b3d12c52L,
          0x77223abf371ef5L, 0x2e0351613bf4deL },
        { 0x7a5a4f2b4468ccL, 0xdcea921470ae46L, 0xf23b7e811be696L,
          0xe59ad0d720d6fbL, 0x9eacac22983469L, 0x4dd4110c4397eeL,
          0x4ef85bdcbe2675L, 0xe4999f7aa7c74bL }
    },
    {
        { 0x031838c8ea1e98L, 0x539b38304d96a2L, 0x5fbdef0163956eL,
          0x6bd4d35ce3f52aL, 0xe538c2355e897fL, 0x6078d3a472dd3fL,
          0x590241eca9f452L, 0x2bc8495fd7fc07L },
        { 0x23d0c89ead4c8cL, 0x1ea55a9601c66eL, 0x41493c94f5b833L,
          0xc49a300aa5a978L, 0xc98bdc90c69594L, 0x4e44cedccbdc8cL,
          0xb0d4e916adccbfL, 0xd56e36b32c37aeL }
    },
    {
        { 0x052bd405b93152L, 0x688b1d44f1dbfaL, 0xe77ba1abe5cc5fL,
          0x11f8a38a6ac543L, 0x3355fd6e4bb988L, 0xdf29c5af8dffb4L,
          0x751f58981f20eeL, 0x22a0f74da9b7fbL },
        { 0xec8f2bc6397b49L, 0xff59fc93639201L, 0xb7f130aa048264L,
          0xe156a63afdc4ccL, 0x0fd7c34b13acafL, 0x87698d40cb4999L,
          0x6d6ecae7f26f24L, 0xae51fad0f296e2L }
    },
    {
        { 0xd0ad5ebdd0f58dL, 0x6ec6a2c5c67880L, 0xe1ce0349af1e0fL,
          0x08014853996d32L, 0x59af51e5e69d20L, 0x0ef743aaa48ecfL,
          0x8d3d2ea7dafcb0L, 0x4ac4fad89189b6L },
        { 0x92d91c2eae97f1L, 0xef5eca262b4662L, 0x440b213b38b10aL,
          0xec90187fc661daL, 0x85f3f25f64cf8dL, 0xcee53ca457ad1bL,
          0x8deed4bf517672L, 0x7706fb34761828L }
    },
    {
        { 0x1577d9117494feL, 0x52d29be2fd7239L, 0x9a0eef00186d37L,
          0x241d0f527fe108L, 0x42824bae6fb59fL, 0xb8d33df0d48c25L,
          0xfffdb0a47af4b0L, 0x534c601073b0b6L },
        { 0xe6df35951c033bL, 0x3e1002b86c0f94L, 0xa7cb55548fb9b6L,
          0x999818ba7bbff8L, 0xe4ba3d684d8bf2L, 0x53dbb326358f0aL,
          0xeebc1e2f2568e8L, 0xc6917ebb3e0f68L }
    },
    {
        { 0xbe1bbfc19f8d13L, 0xc3951b62d4795cL, 0x9371c49ed535a9L,
          0x77c389f68cebeaL, 0xfc1a947a141d0eL, 0x4b48d7ade44f8bL,
          0x3db1f058580a26L, 0xeed1466258b5fcL },
        { 0x5daa4a19854b21L, 0x5bfa46f1ab1eadL, 0xc152e3559957ebL,
          0xdc84277ea48adaL, 0x68709cffc169b5L, 0xde50ce3720e617L,
          0xe42f262dd9a832L, 0xddffd4d2d6ce29L }
    },
    {
        { 0xd5ba5578fa0a56L, 0x0d7d0f1fafaf4cL, 0x7666e4138b63edL,
          0x04e65135d87f02L, 0xdca8866c958f32L, 0xaa8486d3ce2686L,
          0xe3785caf1cbcd3L, 0x8a9b11403c8335L },
        { 0x5c1dca22e0ef60L, 0x775af5b7d3fb20L, 0xe690ffc2b373a8L,
          0x30fe15d28330e6L, 0x8a1022bdd0f393L, 0x6bd7364966a828L,
          0x8d4b154949208aL, 0xfb38c6bb9d9828L }
    },
},
{
    {
        { 0x6d197640340ac2L, 0x969f473ecab5ffL, 0xead46f7c458e42L,
          0x168646a1d00eedL, 0xf70c878e0ce0cfL, 0xa7291d38d8d15aL,
          0x92cf916fdd10ccL, 0x6d3613424f86d5L },
        { 0xba50d172d5c4b4L, 0xe0af5024626f15L, 0x76f3809d76098aL,
          0x433dc27d6caaa8L, 0x72dc67a70d97a7L, 0x935b360f5c7355L,
          0xdbaac93179bb31L, 0x76738487ed1a33L }
    },
    {
        { 0x8d1ca668f9fa0dL, 0x4ed95d8a02f2bfL, 0xd19fc79f630d7bL,
          0x0448ec4f46fa51L, 0xb371dd8623bf3fL, 0xe94fabcd650e94L,
          0x3af3fcacd90a70L, 0x0f720c403ce3b7L },
        { 0x590814cd636c3bL, 0xcf6928d4469945L, 0x5843aaf484a4c6L,
          0xb5a4c1af9b4722L, 0x25116b36cfb2f9L, 0xf248cf032c2640L,
          0x8cd059e27412a1L, 0x866d536862fc5dL }
    },
    {
        { 0x156e62f6de4a2eL, 0x0365af7aafcc78L, 0x65c861819e925eL,
          0x4db5c01f8b2191L, 0x1fd26d1ad564faL, 0x16bbc5319c8610L,
          0x0718eef815f262L, 0x8684f4727f83d1L },
        { 0xa30fd28b0f48dbL, 0x6fef5066ab8278L, 0xd164e771a652dfL,
          0x5a486f3c6ebc8cL, 0xb68b498dc3132bL, 0x264b6efd73323fL,
          0xc261eb669b2262L, 0xd17015f2a35748L }
    },
    {
        { 0x4241f657c4bb1dL, 0x5671702f5187c4L, 0x8a9449f3973753L,
          0x272f772cc0c0cdL, 0x1b7efee58e280cL, 0x7b323494b5ee9cL,
          0xf23af4731142a5L, 0x80c0e1dd62cc9eL },
        { 0xcbc05bf675ffe3L, 0x66215cf258ce3cL, 0xc5d223928c9110L,
          0x30e12a32a69bc2L, 0x5ef5e8076a9f48L, 0x77964ed2329d5fL,
          0xdf81ba58a72cf2L, 0x38ea70d6e1b365L }
    },
    {
        { 0x1b186802f75c80L, 0x0c153a0698665aL, 0x6f5a7fe522e8ddL,
          0x96738668ddfc27L, 0x7e421d50d3bdceL, 0x2d737cf25001b2L,
          0x568840f0e8490cL, 0xea2610be30c8daL },
        { 0xe7b1bc09561fd4L, 0xeda786c26decb0L, 0x22369906a76160L,
          0x371c71478a3da3L, 0x1db8fce2a2d9bfL, 0x59d7b843292f92L,
          0x8097af95a665f9L, 0x7cb4662542b7a9L }
    },
    {
        { 0xa5c53aec6b0c2fL, 0xc4b87327312d84L, 0xfc374cbc732736L,
          0xa8d78fe9310cc0L, 0xd980e8665d1752L, 0xa62692d6004727L,
          0x5d079280146220L, 0xbd1fedb860fea5L },
        { 0xcbc4f8ab35d111L, 0x5ba8cdf3e32f77L, 0xd5b71adb614b93L,
          0x7b3a2df2f8808dL, 0x09b89c26ef2721L, 0x55a505447c3030L,
          0x21044312986ae6L, 0x427a0112367d4cL }
    },
    {
        { 0xe9fe256c1942d8L, 0x9e7377d96e3546L, 0x43e734cb0c1744L,
          0x5f46821211fbcaL, 0x44f83dc32b6203L, 0x84513086ad1d96L,
          0x54dd5192fbb455L, 0xc2a18222f10089L },
        { 0x01055a21855bfaL, 0x9e6d7b477078b4L, 0x3f8df6d30cea0eL,
          0x81c215032973f7L, 0x17dd761c0b3d40L, 0x040424c50d0abeL,
          0x5599413783deabL, 0xde9271e8f3146fL }
    },
    {
        { 0x5edfd25af4a11dL, 0x3a3c5307846783L, 0xb20086873edd31L,
          0x74e00ecfe0eef8L, 0xba65d2f3dd78c7L, 0xab1364371999f1L,
          0xfa9be5dde9a7e8L, 0xeb146ce87a8609L },
        { 0x76afd6565353e9L, 0xfa7023dd51ba1cL, 0x7a09f2237ede4fL,
          0xca085760ba7a1bL, 0xd973882b99950aL, 0xe894266ea5057aL,
          0xd01c4217f55e49L, 0x69cfb9c5555679L }
    },
},
{
    {
        { 0x67867e7c5d631aL, 0x1de88c55bcf47bL, 0x8366d06afd1352L,
          0xd7dbdef6e20337L, 0xb0f9e2f1253ec7L, 0x1be984510ad240L,
          0x63ec533f4a6118L, 0xd5e4c5b96ce633L },
        { 0x1d0b6c34df4a25L, 0xef9486a5a1b554L, 0x2f0e59e47b6ef3L,
          0x4d8042f2ff84d7L, 0x3e74aa3da359c9L, 0x1baa16fd21c160L,
          0xb4cff210191cbaL, 0x50032d8ebc6472L }
    },
    {
        { 0xb6833e01fc1b13L, 0x8a8b7ba1a5ad8fL, 0xc0cafa2622b820L,
          0xc6663af738ed20L, 0xd8944868b18f97L, 0xcf0c1f9774fbe4L,
          0xeedd4355be814fL, 0xd81c02db57e543L },
        { 0x5e32afc310bad8L, 0x065bc819b813d1L, 0x8efc5fc3142795L,
          0x5006514732d59cL, 0x91e39df2b5a3ceL, 0x2ad4477faf4204L,
          0x1a96b184d9bd4fL, 0xc3fee95a4d9c07L }
    },
    {
        { 0xfac7df06b4ba61L, 0xa6ed551061aaefL, 0x35aa2d6133f609L,
          0x420cfba20ed13dL, 0x861c63eea03d0cL, 0x75f0c56f936d6eL,
          0xa25f68f3d9a3d5L, 0xba0b7fecd9f66eL },
        { 0x292e1354680772L, 0x6f6a2dba73f405L, 0xca6add924ea9e4L,
          0x81cfd61268daaaL, 0x7a4cb6ce6f147aL, 0x8ec3454bded8f5L,
          0xc8a893b11d61cbL, 0x2256ffc7656022L }
    },
    {
        { 0x6b33271575cb78L, 0x560d305adcd23eL, 0xeedbd3ad6d834bL,
          0x614a64a5a31e27L, 0xe40b47647ee0c8L, 0x8ef4ff68bd7c2cL,
          0xa5297fc0b77727L, 0x8759208baf88adL },
        { 0x86cfe64918df68L, 0x9d60a73cdd882eL, 0x546b642b953014L,
          0xbaceae38bbef55L, 0xdf58e43f1c3467L, 0x99a83fee9f9babL,
          0xcd52cbf57a4a8bL, 0xf744e968ae36ecL }
    },
    {
        { 0xb945869a607124L, 0x810dbe9440e6f6L, 0x9911e60738e381L,
          0x51df68c343b80bL, 0xe424336f7a3f39L, 0x2d32acb989015cL,
          0xa69b14931019e8L, 0x8a31a38ec12f93L },
        { 0x0d0d36997c916aL, 0xdc95f3b8885372L, 0xcf1a2613549040L,
          0x60f6f5eabe95a2L, 0xa909e9fe141325L, 0x7d598f2355c865L,
          0x70c6442931a9c9L, 0x2354a85b423850L }
    },
    {
        { 0x4cdd22497f9619L, 0x4776fffc22162eL, 0xee5ec330cd31c2L,
          0x7c04c10f209bb8L, 0x35bbfde579e211L, 0x0e3832515cdfc2L,
          0x657e6d3e26ffa7L, 0xc66a7c3c65c604L },
        { 0x322acd7b45e567L, 0x1589cf0296db9bL, 0x1fd0bd3ba1db73L,
          0xe8826109337a40L, 0xf505a50b3035c7L, 0x4d5af066ed08d7L,
          0xb3c376b5eda400L, 0x9c7b7001944748L }
    },
    {
        { 0xd76832570c3716L, 0xda62af0dd540e0L, 0x76b155d6580feaL,
          0x4f42acc32b5464L, 0x881bb603f5b72bL, 0x09c130ee68b9baL,
          0x37ede3b5c50342L, 0xce61a9cfd15e7dL },
        { 0xfff1d8572605d0L, 0x62ac2d3062abc2L, 0xa85e02efbe43ddL,
          0x859d2baa947020L, 0x2ebc8a9111c20bL, 0x7f590a7a656f66L,
          0x0e1384316b21a6L, 0x29b30c500c7db6L }
    },
    {
        { 0x61e55e2906b8deL, 0x6a97e96949974dL, 0x24b52b526eef67L,
          0x512f5361aa595aL, 0x81cc7b83c48fcbL, 0xa64af2328115adL,
          0x9edf6f93d44b8eL, 0x68d7f7c1fe22e3L },
        { 0x2b2116a520d151L, 0x66a0b7d6aa3efbL, 0x48ae70a9b0f791L,
          0xcf12174037db88L, 0x36868cd317d9f3L, 0xb57305922fc344L,
          0xbaa852646a5d23L, 0xad6569137fc10dL }
    },
},
{
    {
        { 0xcf8e5f512c78d5L, 0xeb94d98805cdbdL, 0xad1dcdf2ab50b5L,
          0xf33c136f33cd31L, 0x0d6226b10aeff5L, 0xf7ff493f2f8fc5L,
          0x7e520d4df57165L, 0x41fbae505271a7L },
        { 0x72c898776480baL, 0x260835925f4523L, 0xed36b8d49f5f01L,
          0x3bc1dcef3d49ebL, 0x30c1c1a4940322L, 0x78c1cda7e0f731L,
          0x51f2dc86d05a31L, 0x57b0aa807f3522L }
    },
    {
        { 0x7ab628e71f88bcL, 0xcf585f38018f21L, 0xdbbe3a413d64f6L,
          0x0f86df1ec493a5L, 0x8355e6c7725de9L, 0x3954ffee00fe1eL,
          0xbb8978f9924e32L, 0x1c192987812714L },
        { 0x7c4ce3eaabca8bL, 0xf861eb59bf7019L, 0x31a84fc682e541L,
          0x2307ca9acd1b92L, 0x6f8b6ce4bf2842L, 0xde252accb9f9a9L,
          0x7f0611d93c46d1L, 0x8e2bd80751dc98L }
    },
    {
        { 0xf2fd8fbe27d54bL, 0x2a1e37ec248071L, 0x2fcc888ab8f49aL,
          0x42c62a3c18a9e5L, 0xe30290870b2446L, 0x90277fac5ac55dL,
          0x8d97d56d6dde41L, 0xf4cf8a95db04feL },
        { 0x3e280f5d30d077L, 0x2c903073cb3293L, 0xe0be2ac24eb0ddL,
          0xa2d1a498bcb4f0L, 0x16db466cd0cd45L, 0x3b28aa79a80232L,
          0xdd7e52f17b008eL, 0x20685f2868e4daL }
    },
    {
        { 0x0a68c147c7a486L, 0xd8ef234c429633L, 0x470667bffe7506L,
          0x55a13c88828d51L, 0x5f327412e44befL, 0x537d92a5929f92L,
          0x0a01d5b31c5cd5L, 0xb77aa7867eb3d7L },
        { 0x36ec45f8b82e4dL, 0x6821da0b37b199L, 0x8af37aad7fa94eL,
          0xf0206421085010L, 0x9b886787e56851L, 0x35f394452948ceL,
          0x125c2baafc1361L, 0x8a57d0e453e332L }
    },
    {
        { 0xefe99488043664L, 0xb8b8509db1aa55L, 0x1a2e5a9332523fL,
          0x5e255dd1045c0fL, 0xe68dd8a7ae7180L, 0x55f1cf345bf532L,
          0xe00722ee63a716L, 0xd1c21386116bacL },
        { 0x626221f1c6d1f4L, 0x240b8303773278L, 0xe393a0d88def16L,
          0x229266eca0495cL, 0x7b5c6c9d3e4608L, 0xdc559cb7927190L,
          0x06afe42c7b3c57L, 0x8a2ad0bb439c9bL }
    },
    {
        { 0xd7360fbffc3e2fL, 0xf721317fbd2e95L, 0x8cacbab5748e69L,
          0x7c89f279054bb9L, 0xcbe50faaa86881L, 0x7aa05d375206e4L,
          0x1ea01bcc752c66L, 0x5968cde1f2c2bcL },
        { 0x487c55f09a853eL, 0x82cbef1e09204bL, 0xad5c492abd8670L,
          0x7175963f12dcb3L, 0x7a85762bf6aa06L, 0x02e5697f8d5237L,
          0xccf7d1937c6157L, 0x3b14ca6c2fd59cL }
    },
    {
        { 0x5e610d81b9f77fL, 0x85876d0051b02fL, 0x5d81c63b8020ddL,
          0xd0b4116d6ce614L, 0x91810e5aa8bf0cL, 0xf27f91fcbf8c66L,
          0x2e5dc5f38480aeL, 0x0a13ffebec7633L },
        { 0x61ff6492bf6af8L, 0xe6aef2d641f827L, 0xad5708a5de5f04L,
          0xe5c3a80cdfee20L, 0x88466e268fcfa2L, 0x8e5bb3ad6e1d7bL,
          0xa514f06ed236b8L, 0x51c9c7ba5f5274L }
    },
    {
        { 0xa19d228f9bc3d8L, 0xf89c3f03381069L, 0xfee890e5c3f379L,
          0x3d3ef3d32fb857L, 0x39988495b418ddL, 0x6786f73c46e89aL,
          0x79691a59e0f12fL, 0x76916bf3bc022bL },
        { 0xea073b62cd8a0aL, 0x1fbedd4102fdbcL, 0x1888b14cb9d015L,
          0x98f2cfd76655f7L, 0xb9b591059f0494L, 0xa3dbbe1e6986a3L,
          0xef016a5eaf2b04L, 0xf671ba7cd2d876L }
    },
},
{
    {
        { 0x1dae3bf1ae05e9L, 0x6a029961f21fefL, 0x95df2b97aec3c6L,
          0x9abbc5ad83189bL, 0xaf994af2d13140L, 0xc3f884686aa406L,
          0xcd77e5075284c5L, 0x1c1e13d2a9a4d7L },
        { 0x7f8815d744b89dL, 0xb1891332ba673eL, 0x55ea93cd594570L,
          0x19c8a18d61b041L, 0x938ebaa8d2c580L, 0x9b4344d05ba078L,
          0x622da438eaf9b7L, 0x809b8079fea368L }
    },
    {
        { 0x3780e51c33b7a2L, 0xd7a205c387b1c8L, 0x79515f84be60e4L,
          0xde02a8b1e18277L, 0x4645c96f0d9150L, 0x45f8acbe0b3fd1L,
          0x5d532ba9b53ac3L, 0x7984dcdb0557c9L },
        { 0x5ae5ca68a92f01L, 0xd2fbb3c9d569caL, 0x668cc570c297c1L,
          0xa4829436295e89L, 0xf646bc1a33ad40L, 0x066aaa4c3f425dL,
          0x23434cdd005de2L, 0x5aca9e9db35af4L }
    },
    {
        { 0x2bca35c6877c56L, 0xab864b4f0ddd7dL, 0x5f6aa74404f46cL,
          0x72be164539c279L, 0x1b1d73ee0283cfL, 0xe550f46ad583d9L,
          0x4ac6518e739ad1L, 0x6b6def78d42100L },
        { 0x4d36b8cfa8468dL, 0x2cb37735a3d7b8L, 0x577f86f5016281L,
          0xdb6fe5f9124733L, 0xacb6d2ae29e039L, 0x2ab8330580b8a1L,
          0x130a4ac643b2d0L, 0xa7996e35e6884eL }
    },
    {
        { 0x6fb627760a0aa8L, 0xe046843cbe04f0L, 0xc01d120e6ad443L,
          0xa42a05cabef2fcL, 0x6b793f112ff09cL, 0x5734ea8a3e5854L,
          0xe482b36775f0adL, 0x2f4f60df864a34L },
        { 0xf521c5884f2449L, 0x58734a99186a71L, 0x157f5d5ac5eaccL,
          0x858d9a4248ee61L, 0x0727e6d48149c3L, 0xd5c3eaaac9ec50L,
          0xa63a64a20ee9b5L, 0x3f0dfc487be9deL }
    },
    {
        { 0x836349db13e3f4L, 0xebdd0263e9316dL, 0x3fd61e8324fd6cL,
          0x85dddfa0964f41L, 0x06e72de52add1bL, 0xb752cff8c4a9e2L,
          0x53b0894fdf09f7L, 0xd5220ab0bc24fdL },
        { 0x8442b35fb1981aL, 0xa733a373edd701L, 0x42b60c3d0ef089L,
          0xa1b16ec46e7bcaL, 0xc0df179a09aaf4L, 0xcd4f187638f3a1L,
          0x9af64f79eab1c2L, 0x86fed79d1d78e3L }
    },
    {
        { 0x42c8d86fe29980L, 0x6657b816575660L, 0x82d52c680f92caL,
          0x8587af102d42beL, 0xb5151316e8bdf0L, 0x706e2d9c333495L,
          0xd53601a9673064L, 0x27b1fbb8219099L },
        { 0x3f0929d705f7c8L, 0xff40b10f3d6e6fL, 0x673c703026af5cL,
          0x2c1dce4e25a422L, 0x5348bd73dad8b6L, 0xc39b6b6be2c329L,
          0x47854ffb921084L, 0xb347b8bb391f20L }
    },
    {
        { 0x79fc841eb9b774L, 0xf32da25b4b6c1dL, 0xcbba76bfe492cbL,
          0x76c51fcd623903L, 0x114cf6fcf0705aL, 0x6b720497815dafL,
          0x630b362473382eL, 0xbf40c3a9704db5L },
        { 0xa8a9ddcc5456ebL, 0x2b4472a72f2dc1L, 0x9874444d6d6ef3L,
          0x27e8d85a0ba5edL, 0x5d225b4194849fL, 0xe852cd6ebaa40dL,
          0xb669c248d4bf3fL, 0xa8601eb2343991L }
    },
    {
        { 0x8a0485459502d3L, 0xcab27eee269a7bL, 0x41793074875adaL,
          0x179e685e2405f9L, 0x0d7b6987b28963L, 0x80c9db8422a43eL,
          0xf5ff318a0f43eeL, 0x7a928054ba7aa7L },
        { 0xa5c79fe0c0834eL, 0x837ca0d1f849ecL, 0xfe0d7fa628ab7bL,
          0x94bcb956edd19aL, 0xa18bc932226fbfL, 0x2795379aad54a3L,
          0xceeacf8371129eL, 0x65ca57fa588be5L }
    },
},
{
    {
        { 0x7a578b52caa330L, 0x7c21944d8ca34aL, 0x6c0fbbb6447282L,
          0xa8a9957f90b2e5L, 0xbbe10666586b71L, 0x716a90249138a2L,
          0x2fa6034e7ed66dL, 0x56f77ed2b9916aL },
        { 0x69f1e26bddefb3L, 0xa4978098c08420L, 0xc3377eb09bc184L,
          0x796ce0cbe6dadeL, 0x3be0625d103bbbL, 0x01be27c992685cL,
          0xc0e25597755f9fL, 0x165c40d1c0dbfaL }
    },
    {
        { 0xc63a397659c761L, 0x10a0e5b630fbadL, 0xf21e8a6655ac56L,
          0xe8580fac1181e2L, 0xbfc2d9c0a84b5cL, 0x2cdbaff7afd5d1L,
          0x95f1182f61e85aL, 0x1173e96719eaf4L },
        { 0xc06d55ec6de8b9L, 0x1b4c8ebafcbcaaL, 0x52af5cbbc2bbcdL,
          0x564fab877bcd10L, 0xfd53a18ae85a6eL, 0x225785994c712fL,
          0x29b11d71352121L, 0xab1cb76c40491aL }
    },
    {
        { 0xb4e8ca8ce32eb4L, 0x7e484acb250b49L, 0x062c6f7a3e31a2L,
          0x497fd83625d1fcL, 0x98f821c362dda7L, 0xcae1f8f6be3111L,
          0x9077e955d4fa42L, 0xa589971a65855aL },
        { 0xda6321d28832a9L, 0xf9ef5dc3936e9eL, 0xa37f117c9797efL,
          0x0eb3c80db581beL, 0x207c5c4baa0002L, 0xc0401b5f38faa0L,
          0xceee523d0f1e6eL, 0x8d27a5fd1f0045L }
    },
    {
        { 0x9411063cf0af29L, 0x304385789a6693L, 0x9a9fb8f640145eL,
          0x7d82fe954832ebL, 0xf2789e1898c520L, 0x448b402f948dc0L,
          0xeca8fdf68996ddL, 0x22227e9a149b2fL },
        { 0x63509ff8e62d6aL, 0xe98d81c8c9c57fL, 0xd3874071fe3bedL,
          0xf1db013539538fL, 0xb04092e48418ceL, 0xbbf8e76d6d9d4dL,
          0x2ea9cda2cec5aeL, 0x8414b3e5078fa9L }
    },
    {
        { 0x5ad1cdbd68a073L, 0xd4cedafc18b591L, 0x78267078e4c1c9L,
          0x9b8d9209ca302aL, 0x3101bd2326115bL, 0x6f154b54c2717aL,
          0x618c31b263e84bL, 0x12c4138bbd6942L },
        { 0xf9ead2580da426L, 0xe748e9947d9680L, 0x9b396a38a4210eL,
          0xfaf03ddf4b8f72L, 0xbd94a5266159e7L, 0x5e730491d4c7cbL,
          0x31d1f9a7910f38L, 0x4fd10ca08d6dd1L }
    },
    {
        { 0x4f510ac9f2331eL, 0xee872dc7e3dcc2L, 0x4a11a32a0a0c73L,
          0x27e5803aa5a630L, 0xe5ae5037af4a8aL, 0x2dcdeba9fffeb0L,
          0x8c27748719d91fL, 0xd3b5b62b9cc61cL },
        { 0x998ac90cca7939L, 0xc22b59864514e5L, 0x950aaa1b35738aL,
          0x4b208bbdab0264L, 0x6677931a557d2eL, 0x2c696d8f7c17d3L,
          0x1672d4a3e15c51L, 0x95fab663db0e82L }
    },
    {
        { 0x3d427346ff205eL, 0x7f187d90ea9fbeL, 0xbd9367f466b2afL,
          0x188e53203daf2fL, 0xefe132927b54d8L, 0x14faf85ef70435L,
          0xa5061281ec95c4L, 0xad01705c22cba7L },
        { 0x7d2dfa66197333L, 0xedd7f078b4f6edL, 0xe0cb68575df105L,
          0x47c9ddb80f76bcL, 0x49ab5319073c54L, 0x845255ae607f44L,
          0x0b4ed9fcc74b7cL, 0xcfb52d50f5c3a6L }
    },
    {
        { 0x545c7c6c278776L, 0x92a39ae98c30f0L, 0x8aa8c01d2f4680L,
          0xa5409ed6b7f840L, 0x0c450acdcb24e7L, 0x5da6fb2c5770d9L,
          0x5b8e8be8658333L, 0xb26bf4a67ea4adL },
        { 0x2e30c81c7d91faL, 0x6e50a490eeb69fL, 0x9458c2bee4bc26L,
          0x419acf233be250L, 0x79d6f8187881abL, 0x694565d403b1beL,
          0x34b3990234fe1dL, 0x60997d72132b38L }
    },
},
{
    {
        { 0x00a974126975dcL, 0x42161c46cf94e7L, 0xcc9fe4bc64ed99L,
          0x020019a4680570L, 0x885595a698da0dL, 0x008444b77dd962L,
          0xbf3c22da4fea0eL, 0xc4630482c81245L },
        { 0xcb248c5793ab18L, 0x4dc7a20eb4320bL, 0x9a0906f1572b7dL,
          0xd5b3019f9ac20fL, 0x79b1bf534520a3L, 0x788dfe869b5322L,
          0x9a05298455b7e2L, 0x2f4aecb016bca9L }
    },
    {
        { 0x414d3798745618L, 0x64ba22eb7c983cL, 0x9a5d19f9f9d532L,
          0x81a00d844a80c8L, 0xb9e24f5cae98d6L, 0x6c3769caca965aL,
          0x50d6081f6e4e6dL, 0x0d9698054422a6L },
        { 0xbd7e7925cdd790L, 0xcff65da6a35219L, 0x40dc3638b60ebeL,
          0x84bee7492a50dcL, 0x57d4be415ad65eL, 0xc54256b1a6d1d3L,
          0x141c64945717ccL, 0x05eb609cd1c736L }
    },
    {
        { 0xfd52eab1e3c7ecL, 0xa4a5eca9f24895L, 0xaaa2a8d79fdb83L,
          0xd105e6072bdfdaL, 0x59e6ae2681d97eL, 0xfedf8e08e8077fL,
          0xb06d0ad629e462L, 0x8c7c2d096fa863L },
        { 0x5eecc4cee8fc91L, 0x5e83ab29e61174L, 0x1fd8925b28c02dL,
          0x93be5382072864L, 0xda0c88624c984eL, 0xdcf9f0ca008286L,
          0x1ecb5a6a58ba75L, 0x1d9b890c2e3c83L }
    },
    {
        { 0x19e866eeeee062L, 0x31c1c7f4f7b387L, 0x9be60181c06652L,
          0xc00a93a2b68bbbL, 0x54c65d69d52b2bL, 0x4591416e8b744aL,
          0x641bcca9a64ab6L, 0xf22bcb1ab08098L },
        { 0x3c0db8ff1f726cL, 0x4f5739e9d2e6a6L, 0x5cb669b45c9530L,
          0x861b04e7b472d0L, 0x3e30515894da77L, 0x3344685c9ac39bL,
          0x9e1730573bdd29L, 0x9cac12c808dc85L }
    },
    {
        { 0xf152b865e27087L, 0x267bd8590a580eL, 0xba79cec8baafc1L,
          0x6140ab19442686L, 0xa67090c5b31693L, 0x50a103a28b4117L,
          0x7722e610ddc08fL, 0x5d19d43e6569b2L },
        { 0x70e0c525962bf6L, 0x808e316fb5fb02L, 0x3fb80da5b667beL,
          0x8aa366efcfacecL, 0xcb0b3e7134280eL, 0x0bf1de4cd7d944L,
          0x0cd23bed092df5L, 0xc9a6a79a153a0cL }
    },
    {
        { 0x1c69ad02d5a4b7L, 0x4bb28d0d9e6f4aL, 0x815308ca984fc6L,
          0x40929c79037ca5L, 0x0ea2b491bd0357L, 0xec17e5b42aad4eL,
          0x1f32ade18e7235L, 0xbc60b05a96a9d3L },
        { 0x3b0229ae20f707L, 0xd63505056bdfadL, 0xac2d922d8b2e1eL,
          0x92b2998235c748L, 0x6002c3ad766f97L, 0x99198001a2a862L,
          0x2af7567b58b684L, 0xd8fe707aaafce5L }
    },
    {
        { 0x54487ab5df7a4bL, 0x51cccdec57ccc2L, 0x23943277510b53L,
          0x3a09f02f555de3L, 0xa696aec1be484dL, 0x56f459f37817a2L,
          0x8d8f61c623dcb4L, 0xc52223c5335656L },
        { 0xf634111b49914aL, 0xbf8e1ab8e4f9bbL, 0x2f59578f4dba02L,
          0x2a94199e004319L, 0x87931f0654d005L, 0x7df57d96fa0814L,
          0xc8da316a154031L, 0x2a44ac041f658bL }
    },
    {
        { 0xfb5f4f89e34ac6L, 0x0a1b10b97790f2L, 0x58fe4e74b8a06cL,
          0x10c1710955f27cL, 0x77b798ad5ebe19L, 0xaf1c35b1f1c2dcL,
          0xc25b8e6a1f8d69L, 0x49cf751f76bf23L },
        { 0x15cb2db436f7b7L, 0x186d7c27e74d1aL, 0x60731dec00a415L,
          0xea1e15615f0772L, 0xf02d591714463fL, 0x26a0c6451adeb1L,
          0x20174cdcc5229eL, 0xb817e50efd512aL }
    },
},
};

static const ge448_precomp base_i[16] = {
    {
        { 0x26a82bc70cc05eL, 0x80e18b00938e26L, 0xf72ab66511433bL,
          0xa3d3a46412ae1aL, 0x0f1767ea6de324L, 0x36da9e14657047L,
          0xed221d15a622bfL, 0x4f1970c66bed0dL },
        { 0x08795bf230fa14L, 0x132c4ed7c8ad98L, 0x1ce67c39c4fdbdL,
          0x05a0c2d73ad3ffL, 0xa3984087789c1eL, 0xc7624bea73736cL,
          0x248876203756c9L, 0x693f46716eb6bcL }
    },
    {
        { 0x28173286ff2f8fL, 0xb769465da85757L, 0xf7f6271fd6e862L,
          0x4a3fcfe8daa9cbL, 0xda82c7e2ba077aL, 0x943332241b8b8cL,
          0x6455bd64316cb6L, 0x0865886b9108afL },
        { 0x22ac13588ed6fcL, 0x9a68fed02dafb8L, 0x1bdb6767f0bffaL,
          0xec4e1d58bb3a33L, 0x56c3b9fce43c82L, 0xa6449a4a8d9523L,
          0xf706cbda7ad43aL, 0xe005a8dbd5125cL }
    },
    {
        { 0xa99d1092030034L, 0x2d8cefc6f950d0L, 0x7a920c3c96f07bL,
          0x958812808bc0d5L, 0x62ada756d761e8L, 0x0def80cbcf7285L,
          0x0e2ba7601eedb5L, 0x7a9f9335a48dcbL },
        { 0xb4731472f435ebL, 0x5512881f225443L, 0xee59d2b33c5840L,
          0xb698017127d7a4L, 0xb18fced86551f7L, 0x0ade260ca1823aL,
          0xd3b9109ce4fd58L, 0xadfd751a2517edL }
    },
    {
        { 0xdf9567ceb5eaf7L, 0x110a6b478ac7d7L, 0x2d335014706e0bL,
          0x0df9c7b0b5a209L, 0xba4223d568e684L, 0xd78af2d8c3719bL,
          0x77467b9a5291b6L, 0x079748e5c89befL },
        { 0xe20d3fadac377fL, 0x34e866972b5c09L, 0xd8687a3c40bbb7L,
          0x7b3946fd2f84c9L, 0xd00e40ca78f50eL, 0xb87594417e7179L,
          0x9c7373bcb23583L, 0x7ddeda3c90fd69L }
    },
    {
        { 0x3d0def76ab686bL, 0x1a467ec49f7c79L, 0x3e53f4fc8989edL,
          0x101e344430a0d9L, 0xa3ae7318ad44eeL, 0xaefa6cdae1d134L,
          0xaa8cd7d824ad4dL, 0xef1650ced584fcL },
        { 0xa74df674f4754fL, 0xf52cea8ef3fb8bL, 0x47c32d42971140L,
          0x391c15da256fbbL, 0xc165faba605671L, 0xf2518c687993b9L,
          0x2daf7acbd5a84dL, 0x1560b6298f12aeL }
    },
    {
        { 0xef4da0254dc10aL, 0x63118655940db8L, 0xe20b14982f2948L,
          0x67b93775581dbaL, 0x422ee7104f5029L, 0x5d440db5122d34L,
          0xb1e56d71a4c640L, 0xbf12abbc2408eeL },
        { 0x0cc9f86016af01L, 0x88366abf3d8cabL, 0x85dda13a2efe12L,
          0x390df605d00674L, 0xf18f5806d187f7L, 0x28c900ff0c5d20L,
          0xad308123e01733L, 0x42d35b554bf2fdL }
    },
    {
        { 0x009135f2ffb1f1L, 0x099fc7e8f9c605L, 0xcc67da626bfa5aL,
          0xc186d12344552bL, 0xb5232501b339e1L, 0x70a544fc9708c5L,
          0x06baaec1e928e7L, 0x0baedd2ef0f50fL },
        { 0x535d6d8bf479e5L, 0x156e536e4ec3e9L, 0x3165741ddb9be2L,
          0x988af7159fd736L, 0x13d8a782e33dddL, 0x54604214e69002L,
          0x34d56e0804a268L, 0xc59b84f0e52a4cL }
    },
    {
        { 0x525d45f24729d9L, 0x5768aba8712327L, 0xa25e43b43035dbL,
          0x15a1ee8927ef21L, 0xa785d216056112L, 0x45e2fbfd508af9L,
          0xb6f721a37ba969L, 0x30d6d8c216d8d3L },
        { 0x3065e0852074c3L, 0xfa40b4a2a0684eL, 0x851325a763f955L,
          0xd4ef19c9f25900L, 0x799c869f665756L, 0x7b052223312990L,
          0xc986c2b28db802L, 0xf48fb8f28ade0aL }
    },
    {
        { 0x1e461731649b68L, 0xa96e5d65beb9dcL, 0x765ddff481935dL,
          0x6cf132c9f3bf2aL, 0x9f6c5c97c35658L, 0x99cd1394696e60L,
          0x99fa9249c0d5e4L, 0x1acd0638845a95L },
        { 0x0b065413636087L, 0xea20e78ea17b7fL, 0x20afc5f6161967L,
          0xfd6c8a2dc81028L, 0x4ef1357e32c8fdL, 0x8aa400400e4a88L,
          0xd6fcaef48cb82fL, 0x7ba7c6db3cd4faL }
    },
    {
        { 0xf843473d19c7abL, 0x968e76dc655c4dL, 0x52c87d9c4b9c2fL,
          0x65f641ae4aa082L, 0x491a39733c3603L, 0xa606ffe5810098L,
          0x09920e68bf8ad4L, 0x691a0c86db7882L },
        { 0x5205883a4d3ef5L, 0xee839b7acf2efeL, 0x4b78e2ac00ca66L,
          0xbe3f071f9fcb91L, 0x61e66c9bf6943aL, 0xe9b4e57061b79dL,
          0x8d1b01b56c06bdL, 0x0dfa315df76ae5L }
    },
    {
        { 0x803df65f1fd093L, 0x1cd6523489b77eL, 0x2cd2e15c20e295L,
          0xcd490be9b912d1L, 0xdd9a2ff2e886d2L, 0xa3c836dfe9d72aL,
          0xfcad5f2298e0c1L, 0xed126e24bcf067L },
        { 0x1e339533dc81bcL, 0xbea4d76ece6a08L, 0x1d15de3991b252L,
          0x74cc5cfe6daf97L, 0x5ad343f0826493L, 0x2d38a471064049L,
          0xf7f47b9ffcfa4dL, 0xef14490418066cL }
    },
    {
        { 0x4e7f86b9bb55abL, 0x310d7853f496a3L, 0xbd682fc0dec42cL,
          0xbde047a411d32aL, 0xea639b4c5a5ea2L, 0x5052078ba08fa1L,
          0xc968b2307729f2L, 0x567b5a623d3e28L },
        { 0x171e825977fbf7L, 0x0319c70be990aaL, 0x8f65023e12cd69L,
          0x1fb9b19f5015e6L, 0x0083f603568a7cL, 0xba3d30b1f3c5acL,
          0xe7b509d3d7a988L, 0x2318b99cd0f6b6L }
    },
    {
        { 0x54d3b8793ab2cfL, 0x366abead2d8306L, 0x66e8eb6d7a4977L,
          0xa61888cae0072eL, 0x9eeeef5dbc3315L, 0x93f09db163e7f5L,
          0xee9095959ade9aL, 0xaf7f578ce59be0L },
        { 0x24bfd8d5ece59eL, 0x8aa698b3689523L, 0xa9a65de2de92cfL,
          0xec11dbca6ad300L, 0x217f3fa09f88caL, 0xf6c33e3b4d6af7L,
          0xcd3bfa21d86d2dL, 0x1497f835f13f25L }
    },
    {
        { 0xa579568cd03d1dL, 0xd717cdae158af6L, 0x59eda97389a19fL,
          0xb32c370099e99cL, 0xa2dba91dabb591L, 0x6d697d577c2c97L,
          0x5423fc2d43fa6dL, 0x56ea8a50b382bfL },
        { 0x4a987bad80c11aL, 0xe4cde217d590a5L, 0x3dd8860f97e559L,
          0xff45e2543b593cL, 0x00eb4535343cb5L, 0x06b9b997bbfbddL,
          0x4da36b716aea24L, 0x247651757a624eL }
    },
    {
        { 0x32207d03474e0dL, 0x3ffbf04b41cc73L, 0x5c4dc45319eb39L,
          0xfee29be758b463L, 0xcc8a381c30c7a7L, 0x147f4e49fe0e53L,
          0x05b2e26e35a2deL, 0x4362f0292f3666L },
        { 0x0476d0c8474b85L, 0x9d8c65fccaf108L, 0xf58d4041d54b6aL,
          0x3ee6862f38e4b0L, 0x7c7c9d53b44f54L, 0x36a3fd80fb0db5L,
          0xfcd94ba18a8ac8L, 0xc1b1d568f35c05L }
    },
    {
        { 0x16539fc1bdd30dL, 0x1356e538df4afbL, 0xc0545d85a1aedbL,
          0xeb2037a489396bL, 0x897fcbd5660894L, 0x02a58a9b7d104aL,
          0x57fa24cc96b980L, 0xf6448e35bd8946L },
        { 0xee727418805c83L, 0x10fa274992cfc6L, 0x95141939e66b21L,
          0xe0ffa44bd08009L, 0x174332220da22bL, 0x4891ff359e6831L,
          0x407ed73a7d687bL, 0x2fb4e0751d99cfL }
    },
};
#else

/* Reduce scalar mod the order of the curve.
 * Scalar Will be 114 bytes.
 *
 * b  [in]  Scalar to reduce.
 */
void sc448_reduce(byte* b)
{
    word32 d[16];
    word64 t[33];
    word64 c;
    word32 o;

    /* Load from bytes */
    t[ 0] =  (((sword32)((b[ 0]        ) >>  0)) <<  0)
          |  (((sword32)((b[ 1]        ) >>  0)) <<  8)
          |  (((sword32)((b[ 2]        ) >>  0)) << 16)
          | ((((sword32)((b[ 3] & 0xf )) >>  0)) << 24);
    t[ 1] =  (((sword32)((b[ 3]        ) >>  4)) <<  0)
          |  (((sword32)((b[ 4]        ) >>  0)) <<  4)
          |  (((sword32)((b[ 5]        ) >>  0)) << 12)
          |  (((sword32)((b[ 6]        ) >>  0)) << 20);
    t[ 2] =  (((sword32)((b[ 7]        ) >>  0)) <<  0)
          |  (((sword32)((b[ 8]        ) >>  0)) <<  8)
          |  (((sword32)((b[ 9]        ) >>  0)) << 16)
          | ((((sword32)((b[10] & 0xf )) >>  0)) << 24);
    t[ 3] =  (((sword32)((b[10]        ) >>  4)) <<  0)
          |  (((sword32)((b[11]        ) >>  0)) <<  4)
          |  (((sword32)((b[12]        ) >>  0)) << 12)
          |  (((sword32)((b[13]        ) >>  0)) << 20);
    t[ 4] =  (((sword32)((b[14]        ) >>  0)) <<  0)
          |  (((sword32)((b[15]        ) >>  0)) <<  8)
          |  (((sword32)((b[16]        ) >>  0)) << 16)
          | ((((sword32)((b[17] & 0xf )) >>  0)) << 24);
    t[ 5] =  (((sword32)((b[17]        ) >>  4)) <<  0)
          |  (((sword32)((b[18]        ) >>  0)) <<  4)
          |  (((sword32)((b[19]        ) >>  0)) << 12)
          |  (((sword32)((b[20]        ) >>  0)) << 20);
    t[ 6] =  (((sword32)((b[21]        ) >>  0)) <<  0)
          |  (((sword32)((b[22]        ) >>  0)) <<  8)
          |  (((sword32)((b[23]        ) >>  0)) << 16)
          | ((((sword32)((b[24] & 0xf )) >>  0)) << 24);
    t[ 7] =  (((sword32)((b[24]        ) >>  4)) <<  0)
          |  (((sword32)((b[25]        ) >>  0)) <<  4)
          |  (((sword32)((b[26]        ) >>  0)) << 12)
          |  (((sword32)((b[27]        ) >>  0)) << 20);
    t[ 8] =  (((sword32)((b[28]        ) >>  0)) <<  0)
          |  (((sword32)((b[29]        ) >>  0)) <<  8)
          |  (((sword32)((b[30]        ) >>  0)) << 16)
          | ((((sword32)((b[31] & 0xf )) >>  0)) << 24);
    t[ 9] =  (((sword32)((b[31]        ) >>  4)) <<  0)
          |  (((sword32)((b[32]        ) >>  0)) <<  4)
          |  (((sword32)((b[33]        ) >>  0)) << 12)
          |  (((sword32)((b[34]        ) >>  0)) << 20);
    t[10] =  (((sword32)((b[35]        ) >>  0)) <<  0)
          |  (((sword32)((b[36]        ) >>  0)) <<  8)
          |  (((sword32)((b[37]        ) >>  0)) << 16)
          | ((((sword32)((b[38] & 0xf )) >>  0)) << 24);
    t[11] =  (((sword32)((b[38]        ) >>  4)) <<  0)
          |  (((sword32)((b[39]        ) >>  0)) <<  4)
          |  (((sword32)((b[40]        ) >>  0)) << 12)
          |  (((sword32)((b[41]        ) >>  0)) << 20);
    t[12] =  (((sword32)((b[42]        ) >>  0)) <<  0)
          |  (((sword32)((b[43]        ) >>  0)) <<  8)
          |  (((sword32)((b[44]        ) >>  0)) << 16)
          | ((((sword32)((b[45] & 0xf )) >>  0)) << 24);
    t[13] =  (((sword32)((b[45]        ) >>  4)) <<  0)
          |  (((sword32)((b[46]        ) >>  0)) <<  4)
          |  (((sword32)((b[47]        ) >>  0)) << 12)
          |  (((sword32)((b[48]        ) >>  0)) << 20);
    t[14] =  (((sword32)((b[49]        ) >>  0)) <<  0)
          |  (((sword32)((b[50]        ) >>  0)) <<  8)
          |  (((sword32)((b[51]        ) >>  0)) << 16)
          | ((((sword32)((b[52] & 0xf )) >>  0)) << 24);
    t[15] =  (((sword32)((b[52]        ) >>  4)) <<  0)
          |  (((sword32)((b[53]        ) >>  0)) <<  4)
          |  (((sword32)((b[54]        ) >>  0)) << 12)
          |  (((sword32)((b[55]        ) >>  0)) << 20);
    t[16] =  (((sword32)((b[56]        ) >>  0)) <<  0)
          |  (((sword32)((b[57]        ) >>  0)) <<  8)
          |  (((sword32)((b[58]        ) >>  0)) << 16)
          | ((((sword32)((b[59] & 0xf )) >>  0)) << 24);
    t[17] =  (((sword32)((b[59]        ) >>  4)) <<  0)
          |  (((sword32)((b[60]        ) >>  0)) <<  4)
          |  (((sword32)((b[61]        ) >>  0)) << 12)
          |  (((sword32)((b[62]        ) >>  0)) << 20);
    t[18] =  (((sword32)((b[63]        ) >>  0)) <<  0)
          |  (((sword32)((b[64]        ) >>  0)) <<  8)
          |  (((sword32)((b[65]        ) >>  0)) << 16)
          | ((((sword32)((b[66] & 0xf )) >>  0)) << 24);
    t[19] =  (((sword32)((b[66]        ) >>  4)) <<  0)
          |  (((sword32)((b[67]        ) >>  0)) <<  4)
          |  (((sword32)((b[68]        ) >>  0)) << 12)
          |  (((sword32)((b[69]        ) >>  0)) << 20);
    t[20] =  (((sword32)((b[70]        ) >>  0)) <<  0)
          |  (((sword32)((b[71]        ) >>  0)) <<  8)
          |  (((sword32)((b[72]        ) >>  0)) << 16)
          | ((((sword32)((b[73] & 0xf )) >>  0)) << 24);
    t[21] =  (((sword32)((b[73]        ) >>  4)) <<  0)
          |  (((sword32)((b[74]        ) >>  0)) <<  4)
          |  (((sword32)((b[75]        ) >>  0)) << 12)
          |  (((sword32)((b[76]        ) >>  0)) << 20);
    t[22] =  (((sword32)((b[77]        ) >>  0)) <<  0)
          |  (((sword32)((b[78]        ) >>  0)) <<  8)
          |  (((sword32)((b[79]        ) >>  0)) << 16)
          | ((((sword32)((b[80] & 0xf )) >>  0)) << 24);
    t[23] =  (((sword32)((b[80]        ) >>  4)) <<  0)
          |  (((sword32)((b[81]        ) >>  0)) <<  4)
          |  (((sword32)((b[82]        ) >>  0)) << 12)
          |  (((sword32)((b[83]        ) >>  0)) << 20);
    t[24] =  (((sword32)((b[84]        ) >>  0)) <<  0)
          |  (((sword32)((b[85]        ) >>  0)) <<  8)
          |  (((sword32)((b[86]        ) >>  0)) << 16)
          | ((((sword32)((b[87] & 0xf )) >>  0)) << 24);
    t[25] =  (((sword32)((b[87]        ) >>  4)) <<  0)
          |  (((sword32)((b[88]        ) >>  0)) <<  4)
          |  (((sword32)((b[89]        ) >>  0)) << 12)
          |  (((sword32)((b[90]        ) >>  0)) << 20);
    t[26] =  (((sword32)((b[91]        ) >>  0)) <<  0)
          |  (((sword32)((b[92]        ) >>  0)) <<  8)
          |  (((sword32)((b[93]        ) >>  0)) << 16)
          | ((((sword32)((b[94] & 0xf )) >>  0)) << 24);
    t[27] =  (((sword32)((b[94]        ) >>  4)) <<  0)
          |  (((sword32)((b[95]        ) >>  0)) <<  4)
          |  (((sword32)((b[96]        ) >>  0)) << 12)
          |  (((sword32)((b[97]        ) >>  0)) << 20);
    t[28] =  (((sword32)((b[98]        ) >>  0)) <<  0)
          |  (((sword32)((b[99]        ) >>  0)) <<  8)
          |  (((sword32)((b[100]        ) >>  0)) << 16)
          | ((((sword32)((b[101] & 0xf )) >>  0)) << 24);
    t[29] =  (((sword32)((b[101]        ) >>  4)) <<  0)
          |  (((sword32)((b[102]        ) >>  0)) <<  4)
          |  (((sword32)((b[103]        ) >>  0)) << 12)
          |  (((sword32)((b[104]        ) >>  0)) << 20);
    t[30] =  (((sword32)((b[105]        ) >>  0)) <<  0)
          |  (((sword32)((b[106]        ) >>  0)) <<  8)
          |  (((sword32)((b[107]        ) >>  0)) << 16)
          | ((((sword32)((b[108] & 0xf )) >>  0)) << 24);
    t[31] =  (((sword32)((b[108]        ) >>  4)) <<  0)
          |  (((sword32)((b[109]        ) >>  0)) <<  4)
          |  (((sword32)((b[110]        ) >>  0)) << 12)
          |  (((sword32)((b[111]        ) >>  0)) << 20);
    t[32] =  (((sword32)((b[112]        ) >>  0)) <<  0)
          |  (((sword32)((b[113]        ) >>  0)) <<  8);

    /* Mod curve order */
    /* 2^446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d */
    /* Mod top half of extra words */
    t[ 8] += (sword64)0x129eec34 * t[24];
    t[ 9] += (sword64)0x21cf5b54 * t[24];
    t[10] += (sword64)0x29c2ab70 * t[24];
    t[11] += (sword64)0x0f635c8c * t[24];
    t[12] += (sword64)0x25bf7a4c * t[24];
    t[13] += (sword64)0x2d944a70 * t[24];
    t[14] += (sword64)0x18eec490 * t[24];
    t[15] += (sword64)0x20cd7704 * t[24];
    t[ 9] += (sword64)0x129eec34 * t[25];
    t[10] += (sword64)0x21cf5b54 * t[25];
    t[11] += (sword64)0x29c2ab70 * t[25];
    t[12] += (sword64)0x0f635c8c * t[25];
    t[13] += (sword64)0x25bf7a4c * t[25];
    t[14] += (sword64)0x2d944a70 * t[25];
    t[15] += (sword64)0x18eec490 * t[25];
    t[16] += (sword64)0x20cd7704 * t[25];
    t[10] += (sword64)0x129eec34 * t[26];
    t[11] += (sword64)0x21cf5b54 * t[26];
    t[12] += (sword64)0x29c2ab70 * t[26];
    t[13] += (sword64)0x0f635c8c * t[26];
    t[14] += (sword64)0x25bf7a4c * t[26];
    t[15] += (sword64)0x2d944a70 * t[26];
    t[16] += (sword64)0x18eec490 * t[26];
    t[17] += (sword64)0x20cd7704 * t[26];
    t[11] += (sword64)0x129eec34 * t[27];
    t[12] += (sword64)0x21cf5b54 * t[27];
    t[13] += (sword64)0x29c2ab70 * t[27];
    t[14] += (sword64)0x0f635c8c * t[27];
    t[15] += (sword64)0x25bf7a4c * t[27];
    t[16] += (sword64)0x2d944a70 * t[27];
    t[17] += (sword64)0x18eec490 * t[27];
    t[18] += (sword64)0x20cd7704 * t[27];
    t[12] += (sword64)0x129eec34 * t[28];
    t[13] += (sword64)0x21cf5b54 * t[28];
    t[14] += (sword64)0x29c2ab70 * t[28];
    t[15] += (sword64)0x0f635c8c * t[28];
    t[16] += (sword64)0x25bf7a4c * t[28];
    t[17] += (sword64)0x2d944a70 * t[28];
    t[18] += (sword64)0x18eec490 * t[28];
    t[19] += (sword64)0x20cd7704 * t[28];
    t[13] += (sword64)0x129eec34 * t[29];
    t[14] += (sword64)0x21cf5b54 * t[29];
    t[15] += (sword64)0x29c2ab70 * t[29];
    t[16] += (sword64)0x0f635c8c * t[29];
    t[17] += (sword64)0x25bf7a4c * t[29];
    t[18] += (sword64)0x2d944a70 * t[29];
    t[19] += (sword64)0x18eec490 * t[29];
    t[20] += (sword64)0x20cd7704 * t[29];
    t[14] += (sword64)0x129eec34 * t[30];
    t[15] += (sword64)0x21cf5b54 * t[30];
    t[16] += (sword64)0x29c2ab70 * t[30];
    t[17] += (sword64)0x0f635c8c * t[30];
    t[18] += (sword64)0x25bf7a4c * t[30];
    t[19] += (sword64)0x2d944a70 * t[30];
    t[20] += (sword64)0x18eec490 * t[30];
    t[21] += (sword64)0x20cd7704 * t[30];
    t[15] += (sword64)0x129eec34 * t[31];
    t[16] += (sword64)0x21cf5b54 * t[31];
    t[17] += (sword64)0x29c2ab70 * t[31];
    t[18] += (sword64)0x0f635c8c * t[31];
    t[19] += (sword64)0x25bf7a4c * t[31];
    t[20] += (sword64)0x2d944a70 * t[31];
    t[21] += (sword64)0x18eec490 * t[31];
    t[22] += (sword64)0x20cd7704 * t[31];
    t[16] += (sword64)0x129eec34 * t[32];
    t[17] += (sword64)0x21cf5b54 * t[32];
    t[18] += (sword64)0x29c2ab70 * t[32];
    t[19] += (sword64)0x0f635c8c * t[32];
    t[20] += (sword64)0x25bf7a4c * t[32];
    t[21] += (sword64)0x2d944a70 * t[32];
    t[22] += (sword64)0x18eec490 * t[32];
    t[23] += (sword64)0x20cd7704 * t[32];
    t[24]  = 0;
    /* Propagate carries */
    c = t[ 8] >> 28; t[ 9] += c; t[ 8] = t[ 8] & 0xfffffff;
    c = t[ 9] >> 28; t[10] += c; t[ 9] = t[ 9] & 0xfffffff;
    c = t[10] >> 28; t[11] += c; t[10] = t[10] & 0xfffffff;
    c = t[11] >> 28; t[12] += c; t[11] = t[11] & 0xfffffff;
    c = t[12] >> 28; t[13] += c; t[12] = t[12] & 0xfffffff;
    c = t[13] >> 28; t[14] += c; t[13] = t[13] & 0xfffffff;
    c = t[14] >> 28; t[15] += c; t[14] = t[14] & 0xfffffff;
    c = t[15] >> 28; t[16] += c; t[15] = t[15] & 0xfffffff;
    c = t[16] >> 28; t[17] += c; t[16] = t[16] & 0xfffffff;
    c = t[17] >> 28; t[18] += c; t[17] = t[17] & 0xfffffff;
    c = t[18] >> 28; t[19] += c; t[18] = t[18] & 0xfffffff;
    c = t[19] >> 28; t[20] += c; t[19] = t[19] & 0xfffffff;
    c = t[20] >> 28; t[21] += c; t[20] = t[20] & 0xfffffff;
    c = t[21] >> 28; t[22] += c; t[21] = t[21] & 0xfffffff;
    c = t[22] >> 28; t[23] += c; t[22] = t[22] & 0xfffffff;
    c = t[23] >> 28; t[24] += c; t[23] = t[23] & 0xfffffff;
    /* Mod bottom half of extra words */
    t[ 0] += (sword64)0x129eec34 * t[16];
    t[ 1] += (sword64)0x21cf5b54 * t[16];
    t[ 2] += (sword64)0x29c2ab70 * t[16];
    t[ 3] += (sword64)0x0f635c8c * t[16];
    t[ 4] += (sword64)0x25bf7a4c * t[16];
    t[ 5] += (sword64)0x2d944a70 * t[16];
    t[ 6] += (sword64)0x18eec490 * t[16];
    t[ 7] += (sword64)0x20cd7704 * t[16];
    t[ 1] += (sword64)0x129eec34 * t[17];
    t[ 2] += (sword64)0x21cf5b54 * t[17];
    t[ 3] += (sword64)0x29c2ab70 * t[17];
    t[ 4] += (sword64)0x0f635c8c * t[17];
    t[ 5] += (sword64)0x25bf7a4c * t[17];
    t[ 6] += (sword64)0x2d944a70 * t[17];
    t[ 7] += (sword64)0x18eec490 * t[17];
    t[ 8] += (sword64)0x20cd7704 * t[17];
    t[ 2] += (sword64)0x129eec34 * t[18];
    t[ 3] += (sword64)0x21cf5b54 * t[18];
    t[ 4] += (sword64)0x29c2ab70 * t[18];
    t[ 5] += (sword64)0x0f635c8c * t[18];
    t[ 6] += (sword64)0x25bf7a4c * t[18];
    t[ 7] += (sword64)0x2d944a70 * t[18];
    t[ 8] += (sword64)0x18eec490 * t[18];
    t[ 9] += (sword64)0x20cd7704 * t[18];
    t[ 3] += (sword64)0x129eec34 * t[19];
    t[ 4] += (sword64)0x21cf5b54 * t[19];
    t[ 5] += (sword64)0x29c2ab70 * t[19];
    t[ 6] += (sword64)0x0f635c8c * t[19];
    t[ 7] += (sword64)0x25bf7a4c * t[19];
    t[ 8] += (sword64)0x2d944a70 * t[19];
    t[ 9] += (sword64)0x18eec490 * t[19];
    t[10] += (sword64)0x20cd7704 * t[19];
    t[ 4] += (sword64)0x129eec34 * t[20];
    t[ 5] += (sword64)0x21cf5b54 * t[20];
    t[ 6] += (sword64)0x29c2ab70 * t[20];
    t[ 7] += (sword64)0x0f635c8c * t[20];
    t[ 8] += (sword64)0x25bf7a4c * t[20];
    t[ 9] += (sword64)0x2d944a70 * t[20];
    t[10] += (sword64)0x18eec490 * t[20];
    t[11] += (sword64)0x20cd7704 * t[20];
    t[ 5] += (sword64)0x129eec34 * t[21];
    t[ 6] += (sword64)0x21cf5b54 * t[21];
    t[ 7] += (sword64)0x29c2ab70 * t[21];
    t[ 8] += (sword64)0x0f635c8c * t[21];
    t[ 9] += (sword64)0x25bf7a4c * t[21];
    t[10] += (sword64)0x2d944a70 * t[21];
    t[11] += (sword64)0x18eec490 * t[21];
    t[12] += (sword64)0x20cd7704 * t[21];
    t[ 6] += (sword64)0x129eec34 * t[22];
    t[ 7] += (sword64)0x21cf5b54 * t[22];
    t[ 8] += (sword64)0x29c2ab70 * t[22];
    t[ 9] += (sword64)0x0f635c8c * t[22];
    t[10] += (sword64)0x25bf7a4c * t[22];
    t[11] += (sword64)0x2d944a70 * t[22];
    t[12] += (sword64)0x18eec490 * t[22];
    t[13] += (sword64)0x20cd7704 * t[22];
    t[ 7] += (sword64)0x129eec34 * t[23];
    t[ 8] += (sword64)0x21cf5b54 * t[23];
    t[ 9] += (sword64)0x29c2ab70 * t[23];
    t[10] += (sword64)0x0f635c8c * t[23];
    t[11] += (sword64)0x25bf7a4c * t[23];
    t[12] += (sword64)0x2d944a70 * t[23];
    t[13] += (sword64)0x18eec490 * t[23];
    t[14] += (sword64)0x20cd7704 * t[23];
    t[ 8] += (sword64)0x129eec34 * t[24];
    t[ 9] += (sword64)0x21cf5b54 * t[24];
    t[10] += (sword64)0x29c2ab70 * t[24];
    t[11] += (sword64)0x0f635c8c * t[24];
    t[12] += (sword64)0x25bf7a4c * t[24];
    t[13] += (sword64)0x2d944a70 * t[24];
    t[14] += (sword64)0x18eec490 * t[24];
    t[15] += (sword64)0x20cd7704 * t[24];
    t[16]  = 0;
    /* Propagate carries */
    c = t[ 0] >> 28; t[ 1] += c; t[ 0] = t[ 0] & 0xfffffff;
    c = t[ 1] >> 28; t[ 2] += c; t[ 1] = t[ 1] & 0xfffffff;
    c = t[ 2] >> 28; t[ 3] += c; t[ 2] = t[ 2] & 0xfffffff;
    c = t[ 3] >> 28; t[ 4] += c; t[ 3] = t[ 3] & 0xfffffff;
    c = t[ 4] >> 28; t[ 5] += c; t[ 4] = t[ 4] & 0xfffffff;
    c = t[ 5] >> 28; t[ 6] += c; t[ 5] = t[ 5] & 0xfffffff;
    c = t[ 6] >> 28; t[ 7] += c; t[ 6] = t[ 6] & 0xfffffff;
    c = t[ 7] >> 28; t[ 8] += c; t[ 7] = t[ 7] & 0xfffffff;
    c = t[ 8] >> 28; t[ 9] += c; t[ 8] = t[ 8] & 0xfffffff;
    c = t[ 9] >> 28; t[10] += c; t[ 9] = t[ 9] & 0xfffffff;
    c = t[10] >> 28; t[11] += c; t[10] = t[10] & 0xfffffff;
    c = t[11] >> 28; t[12] += c; t[11] = t[11] & 0xfffffff;
    c = t[12] >> 28; t[13] += c; t[12] = t[12] & 0xfffffff;
    c = t[13] >> 28; t[14] += c; t[13] = t[13] & 0xfffffff;
    c = t[14] >> 28; t[15] += c; t[14] = t[14] & 0xfffffff;
    c = t[15] >> 28; t[16] += c; t[15] = t[15] & 0xfffffff;
    t[ 0] += (sword64)0x129eec34 * t[16];
    t[ 1] += (sword64)0x21cf5b54 * t[16];
    t[ 2] += (sword64)0x29c2ab70 * t[16];
    t[ 3] += (sword64)0x0f635c8c * t[16];
    t[ 4] += (sword64)0x25bf7a4c * t[16];
    t[ 5] += (sword64)0x2d944a70 * t[16];
    t[ 6] += (sword64)0x18eec490 * t[16];
    t[ 7] += (sword64)0x20cd7704 * t[16];
    /* Propagate carries */
    c = t[ 0] >> 28; t[ 1] += c; d[ 0] = (sword32)(t[ 0] & 0xfffffff);
    c = t[ 1] >> 28; t[ 2] += c; d[ 1] = (sword32)(t[ 1] & 0xfffffff);
    c = t[ 2] >> 28; t[ 3] += c; d[ 2] = (sword32)(t[ 2] & 0xfffffff);
    c = t[ 3] >> 28; t[ 4] += c; d[ 3] = (sword32)(t[ 3] & 0xfffffff);
    c = t[ 4] >> 28; t[ 5] += c; d[ 4] = (sword32)(t[ 4] & 0xfffffff);
    c = t[ 5] >> 28; t[ 6] += c; d[ 5] = (sword32)(t[ 5] & 0xfffffff);
    c = t[ 6] >> 28; t[ 7] += c; d[ 6] = (sword32)(t[ 6] & 0xfffffff);
    c = t[ 7] >> 28; t[ 8] += c; d[ 7] = (sword32)(t[ 7] & 0xfffffff);
    c = t[ 8] >> 28; t[ 9] += c; d[ 8] = (sword32)(t[ 8] & 0xfffffff);
    c = t[ 9] >> 28; t[10] += c; d[ 9] = (sword32)(t[ 9] & 0xfffffff);
    c = t[10] >> 28; t[11] += c; d[10] = (sword32)(t[10] & 0xfffffff);
    c = t[11] >> 28; t[12] += c; d[11] = (sword32)(t[11] & 0xfffffff);
    c = t[12] >> 28; t[13] += c; d[12] = (sword32)(t[12] & 0xfffffff);
    c = t[13] >> 28; t[14] += c; d[13] = (sword32)(t[13] & 0xfffffff);
    c = t[14] >> 28; t[15] += c; d[14] = (sword32)(t[14] & 0xfffffff);
    d[15] = (word32)t[15];
    /* Mod bits over 28 in last word */
    o = d[15] >> 26; d[15] &= 0x3ffffff;
    d[ 0] += 0x4a7bb0d * o;
    d[ 1] += 0x873d6d5 * o;
    d[ 2] += 0xa70aadc * o;
    d[ 3] += 0x3d8d723 * o;
    d[ 4] += 0x96fde93 * o;
    d[ 5] += 0xb65129c * o;
    d[ 6] += 0x63bb124 * o;
    d[ 7] += 0x8335dc1 * o;
    /* Propagate carries */
    o = d[ 0] >> 28; d[ 1] += o; d[ 0] = d[ 0] & 0xfffffff;
    o = d[ 1] >> 28; d[ 2] += o; d[ 1] = d[ 1] & 0xfffffff;
    o = d[ 2] >> 28; d[ 3] += o; d[ 2] = d[ 2] & 0xfffffff;
    o = d[ 3] >> 28; d[ 4] += o; d[ 3] = d[ 3] & 0xfffffff;
    o = d[ 4] >> 28; d[ 5] += o; d[ 4] = d[ 4] & 0xfffffff;
    o = d[ 5] >> 28; d[ 6] += o; d[ 5] = d[ 5] & 0xfffffff;
    o = d[ 6] >> 28; d[ 7] += o; d[ 6] = d[ 6] & 0xfffffff;
    o = d[ 7] >> 28; d[ 8] += o; d[ 7] = d[ 7] & 0xfffffff;
    o = d[ 8] >> 28; d[ 9] += o; d[ 8] = d[ 8] & 0xfffffff;
    o = d[ 9] >> 28; d[10] += o; d[ 9] = d[ 9] & 0xfffffff;
    o = d[10] >> 28; d[11] += o; d[10] = d[10] & 0xfffffff;
    o = d[11] >> 28; d[12] += o; d[11] = d[11] & 0xfffffff;
    o = d[12] >> 28; d[13] += o; d[12] = d[12] & 0xfffffff;
    o = d[13] >> 28; d[14] += o; d[13] = d[13] & 0xfffffff;
    o = d[14] >> 28; d[15] += o; d[14] = d[14] & 0xfffffff;

    /* Convert to bytes */
    b[ 0] = (byte)(d[0 ] >>  0);
    b[ 1] = (byte)(d[0 ] >>  8);
    b[ 2] = (byte)(d[0 ] >> 16);
    b[ 3] = (byte)(d[0 ] >> 24) + ((d[1 ] >>  0) <<  4);
    b[ 4] = (byte)(d[1 ] >>  4);
    b[ 5] = (byte)(d[1 ] >> 12);
    b[ 6] = (byte)(d[1 ] >> 20);
    b[ 7] = (byte)(d[2 ] >>  0);
    b[ 8] = (byte)(d[2 ] >>  8);
    b[ 9] = (byte)(d[2 ] >> 16);
    b[10] = (byte)(d[2 ] >> 24) + ((d[3 ] >>  0) <<  4);
    b[11] = (byte)(d[3 ] >>  4);
    b[12] = (byte)(d[3 ] >> 12);
    b[13] = (byte)(d[3 ] >> 20);
    b[14] = (byte)(d[4 ] >>  0);
    b[15] = (byte)(d[4 ] >>  8);
    b[16] = (byte)(d[4 ] >> 16);
    b[17] = (byte)(d[4 ] >> 24) + ((d[5 ] >>  0) <<  4);
    b[18] = (byte)(d[5 ] >>  4);
    b[19] = (byte)(d[5 ] >> 12);
    b[20] = (byte)(d[5 ] >> 20);
    b[21] = (byte)(d[6 ] >>  0);
    b[22] = (byte)(d[6 ] >>  8);
    b[23] = (byte)(d[6 ] >> 16);
    b[24] = (byte)(d[6 ] >> 24) + ((d[7 ] >>  0) <<  4);
    b[25] = (byte)(d[7 ] >>  4);
    b[26] = (byte)(d[7 ] >> 12);
    b[27] = (byte)(d[7 ] >> 20);
    b[28] = (byte)(d[8 ] >>  0);
    b[29] = (byte)(d[8 ] >>  8);
    b[30] = (byte)(d[8 ] >> 16);
    b[31] = (byte)(d[8 ] >> 24) + ((d[9 ] >>  0) <<  4);
    b[32] = (byte)(d[9 ] >>  4);
    b[33] = (byte)(d[9 ] >> 12);
    b[34] = (byte)(d[9 ] >> 20);
    b[35] = (byte)(d[10] >>  0);
    b[36] = (byte)(d[10] >>  8);
    b[37] = (byte)(d[10] >> 16);
    b[38] = (byte)(d[10] >> 24) + ((d[11] >>  0) <<  4);
    b[39] = (byte)(d[11] >>  4);
    b[40] = (byte)(d[11] >> 12);
    b[41] = (byte)(d[11] >> 20);
    b[42] = (byte)(d[12] >>  0);
    b[43] = (byte)(d[12] >>  8);
    b[44] = (byte)(d[12] >> 16);
    b[45] = (byte)(d[12] >> 24) + ((d[13] >>  0) <<  4);
    b[46] = (byte)(d[13] >>  4);
    b[47] = (byte)(d[13] >> 12);
    b[48] = (byte)(d[13] >> 20);
    b[49] = (byte)(d[14] >>  0);
    b[50] = (byte)(d[14] >>  8);
    b[51] = (byte)(d[14] >> 16);
    b[52] = (byte)(d[14] >> 24) + ((d[15] >>  0) <<  4);
    b[53] = (byte)(d[15] >>  4);
    b[54] = (byte)(d[15] >> 12);
    b[55] = (byte)(d[15] >> 20);
    b[56] = 0;
}

/* Multiply a by b and add d. r = (a * b + d) mod order
 *
 * r  [in]  Scalar to hold result.
 * a  [in]  Scalar to multiply.
 * b  [in]  Scalar to multiply.
 * d  [in]  Scalar to add to multiplicative result.
 */
void sc448_muladd(byte* r, const byte* a, const byte* b, const byte* d)
{
    word32 ad[16], bd[16], dd[16], rd[16];
    word64 t[32];
    word64 c;
    word32 o;

    /* Load from bytes */
    ad[ 0] =  (((sword32)((a[ 0]        ) >>  0)) <<  0)
           |  (((sword32)((a[ 1]        ) >>  0)) <<  8)
           |  (((sword32)((a[ 2]        ) >>  0)) << 16)
           | ((((sword32)((a[ 3] & 0xf )) >>  0)) << 24);
    ad[ 1] =  (((sword32)((a[ 3]        ) >>  4)) <<  0)
           |  (((sword32)((a[ 4]        ) >>  0)) <<  4)
           |  (((sword32)((a[ 5]        ) >>  0)) << 12)
           |  (((sword32)((a[ 6]        ) >>  0)) << 20);
    ad[ 2] =  (((sword32)((a[ 7]        ) >>  0)) <<  0)
           |  (((sword32)((a[ 8]        ) >>  0)) <<  8)
           |  (((sword32)((a[ 9]        ) >>  0)) << 16)
           | ((((sword32)((a[10] & 0xf )) >>  0)) << 24);
    ad[ 3] =  (((sword32)((a[10]        ) >>  4)) <<  0)
           |  (((sword32)((a[11]        ) >>  0)) <<  4)
           |  (((sword32)((a[12]        ) >>  0)) << 12)
           |  (((sword32)((a[13]        ) >>  0)) << 20);
    ad[ 4] =  (((sword32)((a[14]        ) >>  0)) <<  0)
           |  (((sword32)((a[15]        ) >>  0)) <<  8)
           |  (((sword32)((a[16]        ) >>  0)) << 16)
           | ((((sword32)((a[17] & 0xf )) >>  0)) << 24);
    ad[ 5] =  (((sword32)((a[17]        ) >>  4)) <<  0)
           |  (((sword32)((a[18]        ) >>  0)) <<  4)
           |  (((sword32)((a[19]        ) >>  0)) << 12)
           |  (((sword32)((a[20]        ) >>  0)) << 20);
    ad[ 6] =  (((sword32)((a[21]        ) >>  0)) <<  0)
           |  (((sword32)((a[22]        ) >>  0)) <<  8)
           |  (((sword32)((a[23]        ) >>  0)) << 16)
           | ((((sword32)((a[24] & 0xf )) >>  0)) << 24);
    ad[ 7] =  (((sword32)((a[24]        ) >>  4)) <<  0)
           |  (((sword32)((a[25]        ) >>  0)) <<  4)
           |  (((sword32)((a[26]        ) >>  0)) << 12)
           |  (((sword32)((a[27]        ) >>  0)) << 20);
    ad[ 8] =  (((sword32)((a[28]        ) >>  0)) <<  0)
           |  (((sword32)((a[29]        ) >>  0)) <<  8)
           |  (((sword32)((a[30]        ) >>  0)) << 16)
           | ((((sword32)((a[31] & 0xf )) >>  0)) << 24);
    ad[ 9] =  (((sword32)((a[31]        ) >>  4)) <<  0)
           |  (((sword32)((a[32]        ) >>  0)) <<  4)
           |  (((sword32)((a[33]        ) >>  0)) << 12)
           |  (((sword32)((a[34]        ) >>  0)) << 20);
    ad[10] =  (((sword32)((a[35]        ) >>  0)) <<  0)
           |  (((sword32)((a[36]        ) >>  0)) <<  8)
           |  (((sword32)((a[37]        ) >>  0)) << 16)
           | ((((sword32)((a[38] & 0xf )) >>  0)) << 24);
    ad[11] =  (((sword32)((a[38]        ) >>  4)) <<  0)
           |  (((sword32)((a[39]        ) >>  0)) <<  4)
           |  (((sword32)((a[40]        ) >>  0)) << 12)
           |  (((sword32)((a[41]        ) >>  0)) << 20);
    ad[12] =  (((sword32)((a[42]        ) >>  0)) <<  0)
           |  (((sword32)((a[43]        ) >>  0)) <<  8)
           |  (((sword32)((a[44]        ) >>  0)) << 16)
           | ((((sword32)((a[45] & 0xf )) >>  0)) << 24);
    ad[13] =  (((sword32)((a[45]        ) >>  4)) <<  0)
           |  (((sword32)((a[46]        ) >>  0)) <<  4)
           |  (((sword32)((a[47]        ) >>  0)) << 12)
           |  (((sword32)((a[48]        ) >>  0)) << 20);
    ad[14] =  (((sword32)((a[49]        ) >>  0)) <<  0)
           |  (((sword32)((a[50]        ) >>  0)) <<  8)
           |  (((sword32)((a[51]        ) >>  0)) << 16)
           | ((((sword32)((a[52] & 0xf )) >>  0)) << 24);
    ad[15] =  (((sword32)((a[52]        ) >>  4)) <<  0)
           |  (((sword32)((a[53]        ) >>  0)) <<  4)
           |  (((sword32)((a[54]        ) >>  0)) << 12)
           |  (((sword32)((a[55]        ) >>  0)) << 20);
    /* Load from bytes */
    bd[ 0] =  (((sword32)((b[ 0]        ) >>  0)) <<  0)
           |  (((sword32)((b[ 1]        ) >>  0)) <<  8)
           |  (((sword32)((b[ 2]        ) >>  0)) << 16)
           | ((((sword32)((b[ 3] & 0xf )) >>  0)) << 24);
    bd[ 1] =  (((sword32)((b[ 3]        ) >>  4)) <<  0)
           |  (((sword32)((b[ 4]        ) >>  0)) <<  4)
           |  (((sword32)((b[ 5]        ) >>  0)) << 12)
           |  (((sword32)((b[ 6]        ) >>  0)) << 20);
    bd[ 2] =  (((sword32)((b[ 7]        ) >>  0)) <<  0)
           |  (((sword32)((b[ 8]        ) >>  0)) <<  8)
           |  (((sword32)((b[ 9]        ) >>  0)) << 16)
           | ((((sword32)((b[10] & 0xf )) >>  0)) << 24);
    bd[ 3] =  (((sword32)((b[10]        ) >>  4)) <<  0)
           |  (((sword32)((b[11]        ) >>  0)) <<  4)
           |  (((sword32)((b[12]        ) >>  0)) << 12)
           |  (((sword32)((b[13]        ) >>  0)) << 20);
    bd[ 4] =  (((sword32)((b[14]        ) >>  0)) <<  0)
           |  (((sword32)((b[15]        ) >>  0)) <<  8)
           |  (((sword32)((b[16]        ) >>  0)) << 16)
           | ((((sword32)((b[17] & 0xf )) >>  0)) << 24);
    bd[ 5] =  (((sword32)((b[17]        ) >>  4)) <<  0)
           |  (((sword32)((b[18]        ) >>  0)) <<  4)
           |  (((sword32)((b[19]        ) >>  0)) << 12)
           |  (((sword32)((b[20]        ) >>  0)) << 20);
    bd[ 6] =  (((sword32)((b[21]        ) >>  0)) <<  0)
           |  (((sword32)((b[22]        ) >>  0)) <<  8)
           |  (((sword32)((b[23]        ) >>  0)) << 16)
           | ((((sword32)((b[24] & 0xf )) >>  0)) << 24);
    bd[ 7] =  (((sword32)((b[24]        ) >>  4)) <<  0)
           |  (((sword32)((b[25]        ) >>  0)) <<  4)
           |  (((sword32)((b[26]        ) >>  0)) << 12)
           |  (((sword32)((b[27]        ) >>  0)) << 20);
    bd[ 8] =  (((sword32)((b[28]        ) >>  0)) <<  0)
           |  (((sword32)((b[29]        ) >>  0)) <<  8)
           |  (((sword32)((b[30]        ) >>  0)) << 16)
           | ((((sword32)((b[31] & 0xf )) >>  0)) << 24);
    bd[ 9] =  (((sword32)((b[31]        ) >>  4)) <<  0)
           |  (((sword32)((b[32]        ) >>  0)) <<  4)
           |  (((sword32)((b[33]        ) >>  0)) << 12)
           |  (((sword32)((b[34]        ) >>  0)) << 20);
    bd[10] =  (((sword32)((b[35]        ) >>  0)) <<  0)
           |  (((sword32)((b[36]        ) >>  0)) <<  8)
           |  (((sword32)((b[37]        ) >>  0)) << 16)
           | ((((sword32)((b[38] & 0xf )) >>  0)) << 24);
    bd[11] =  (((sword32)((b[38]        ) >>  4)) <<  0)
           |  (((sword32)((b[39]        ) >>  0)) <<  4)
           |  (((sword32)((b[40]        ) >>  0)) << 12)
           |  (((sword32)((b[41]        ) >>  0)) << 20);
    bd[12] =  (((sword32)((b[42]        ) >>  0)) <<  0)
           |  (((sword32)((b[43]        ) >>  0)) <<  8)
           |  (((sword32)((b[44]        ) >>  0)) << 16)
           | ((((sword32)((b[45] & 0xf )) >>  0)) << 24);
    bd[13] =  (((sword32)((b[45]        ) >>  4)) <<  0)
           |  (((sword32)((b[46]        ) >>  0)) <<  4)
           |  (((sword32)((b[47]        ) >>  0)) << 12)
           |  (((sword32)((b[48]        ) >>  0)) << 20);
    bd[14] =  (((sword32)((b[49]        ) >>  0)) <<  0)
           |  (((sword32)((b[50]        ) >>  0)) <<  8)
           |  (((sword32)((b[51]        ) >>  0)) << 16)
           | ((((sword32)((b[52] & 0xf )) >>  0)) << 24);
    bd[15] =  (((sword32)((b[52]        ) >>  4)) <<  0)
           |  (((sword32)((b[53]        ) >>  0)) <<  4)
           |  (((sword32)((b[54]        ) >>  0)) << 12)
           |  (((sword32)((b[55]        ) >>  0)) << 20);
    /* Load from bytes */
    dd[ 0] =  (((sword32)((d[ 0]        ) >>  0)) <<  0)
           |  (((sword32)((d[ 1]        ) >>  0)) <<  8)
           |  (((sword32)((d[ 2]        ) >>  0)) << 16)
           | ((((sword32)((d[ 3] & 0xf )) >>  0)) << 24);
    dd[ 1] =  (((sword32)((d[ 3]        ) >>  4)) <<  0)
           |  (((sword32)((d[ 4]        ) >>  0)) <<  4)
           |  (((sword32)((d[ 5]        ) >>  0)) << 12)
           |  (((sword32)((d[ 6]        ) >>  0)) << 20);
    dd[ 2] =  (((sword32)((d[ 7]        ) >>  0)) <<  0)
           |  (((sword32)((d[ 8]        ) >>  0)) <<  8)
           |  (((sword32)((d[ 9]        ) >>  0)) << 16)
           | ((((sword32)((d[10] & 0xf )) >>  0)) << 24);
    dd[ 3] =  (((sword32)((d[10]        ) >>  4)) <<  0)
           |  (((sword32)((d[11]        ) >>  0)) <<  4)
           |  (((sword32)((d[12]        ) >>  0)) << 12)
           |  (((sword32)((d[13]        ) >>  0)) << 20);
    dd[ 4] =  (((sword32)((d[14]        ) >>  0)) <<  0)
           |  (((sword32)((d[15]        ) >>  0)) <<  8)
           |  (((sword32)((d[16]        ) >>  0)) << 16)
           | ((((sword32)((d[17] & 0xf )) >>  0)) << 24);
    dd[ 5] =  (((sword32)((d[17]        ) >>  4)) <<  0)
           |  (((sword32)((d[18]        ) >>  0)) <<  4)
           |  (((sword32)((d[19]        ) >>  0)) << 12)
           |  (((sword32)((d[20]        ) >>  0)) << 20);
    dd[ 6] =  (((sword32)((d[21]        ) >>  0)) <<  0)
           |  (((sword32)((d[22]        ) >>  0)) <<  8)
           |  (((sword32)((d[23]        ) >>  0)) << 16)
           | ((((sword32)((d[24] & 0xf )) >>  0)) << 24);
    dd[ 7] =  (((sword32)((d[24]        ) >>  4)) <<  0)
           |  (((sword32)((d[25]        ) >>  0)) <<  4)
           |  (((sword32)((d[26]        ) >>  0)) << 12)
           |  (((sword32)((d[27]        ) >>  0)) << 20);
    dd[ 8] =  (((sword32)((d[28]        ) >>  0)) <<  0)
           |  (((sword32)((d[29]        ) >>  0)) <<  8)
           |  (((sword32)((d[30]        ) >>  0)) << 16)
           | ((((sword32)((d[31] & 0xf )) >>  0)) << 24);
    dd[ 9] =  (((sword32)((d[31]        ) >>  4)) <<  0)
           |  (((sword32)((d[32]        ) >>  0)) <<  4)
           |  (((sword32)((d[33]        ) >>  0)) << 12)
           |  (((sword32)((d[34]        ) >>  0)) << 20);
    dd[10] =  (((sword32)((d[35]        ) >>  0)) <<  0)
           |  (((sword32)((d[36]        ) >>  0)) <<  8)
           |  (((sword32)((d[37]        ) >>  0)) << 16)
           | ((((sword32)((d[38] & 0xf )) >>  0)) << 24);
    dd[11] =  (((sword32)((d[38]        ) >>  4)) <<  0)
           |  (((sword32)((d[39]        ) >>  0)) <<  4)
           |  (((sword32)((d[40]        ) >>  0)) << 12)
           |  (((sword32)((d[41]        ) >>  0)) << 20);
    dd[12] =  (((sword32)((d[42]        ) >>  0)) <<  0)
           |  (((sword32)((d[43]        ) >>  0)) <<  8)
           |  (((sword32)((d[44]        ) >>  0)) << 16)
           | ((((sword32)((d[45] & 0xf )) >>  0)) << 24);
    dd[13] =  (((sword32)((d[45]        ) >>  4)) <<  0)
           |  (((sword32)((d[46]        ) >>  0)) <<  4)
           |  (((sword32)((d[47]        ) >>  0)) << 12)
           |  (((sword32)((d[48]        ) >>  0)) << 20);
    dd[14] =  (((sword32)((d[49]        ) >>  0)) <<  0)
           |  (((sword32)((d[50]        ) >>  0)) <<  8)
           |  (((sword32)((d[51]        ) >>  0)) << 16)
           | ((((sword32)((d[52] & 0xf )) >>  0)) << 24);
    dd[15] =  (((sword32)((d[52]        ) >>  4)) <<  0)
           |  (((sword32)((d[53]        ) >>  0)) <<  4)
           |  (((sword32)((d[54]        ) >>  0)) << 12)
           |  (((sword32)((d[55]        ) >>  0)) << 20);

    /* a * b + d */
    t[ 0] = (word64)dd[ 0] + (sword64)ad[ 0] * bd[ 0];
    t[ 1] = (word64)dd[ 1] + (sword64)ad[ 0] * bd[ 1]
                           + (sword64)ad[ 1] * bd[ 0];
    t[ 2] = (word64)dd[ 2] + (sword64)ad[ 0] * bd[ 2]
                           + (sword64)ad[ 1] * bd[ 1]
                           + (sword64)ad[ 2] * bd[ 0];
    t[ 3] = (word64)dd[ 3] + (sword64)ad[ 0] * bd[ 3]
                           + (sword64)ad[ 1] * bd[ 2]
                           + (sword64)ad[ 2] * bd[ 1]
                           + (sword64)ad[ 3] * bd[ 0];
    t[ 4] = (word64)dd[ 4] + (sword64)ad[ 0] * bd[ 4]
                           + (sword64)ad[ 1] * bd[ 3]
                           + (sword64)ad[ 2] * bd[ 2]
                           + (sword64)ad[ 3] * bd[ 1]
                           + (sword64)ad[ 4] * bd[ 0];
    t[ 5] = (word64)dd[ 5] + (sword64)ad[ 0] * bd[ 5]
                           + (sword64)ad[ 1] * bd[ 4]
                           + (sword64)ad[ 2] * bd[ 3]
                           + (sword64)ad[ 3] * bd[ 2]
                           + (sword64)ad[ 4] * bd[ 1]
                           + (sword64)ad[ 5] * bd[ 0];
    t[ 6] = (word64)dd[ 6] + (sword64)ad[ 0] * bd[ 6]
                           + (sword64)ad[ 1] * bd[ 5]
                           + (sword64)ad[ 2] * bd[ 4]
                           + (sword64)ad[ 3] * bd[ 3]
                           + (sword64)ad[ 4] * bd[ 2]
                           + (sword64)ad[ 5] * bd[ 1]
                           + (sword64)ad[ 6] * bd[ 0];
    t[ 7] = (word64)dd[ 7] + (sword64)ad[ 0] * bd[ 7]
                           + (sword64)ad[ 1] * bd[ 6]
                           + (sword64)ad[ 2] * bd[ 5]
                           + (sword64)ad[ 3] * bd[ 4]
                           + (sword64)ad[ 4] * bd[ 3]
                           + (sword64)ad[ 5] * bd[ 2]
                           + (sword64)ad[ 6] * bd[ 1]
                           + (sword64)ad[ 7] * bd[ 0];
    t[ 8] = (word64)dd[ 8] + (sword64)ad[ 0] * bd[ 8]
                           + (sword64)ad[ 1] * bd[ 7]
                           + (sword64)ad[ 2] * bd[ 6]
                           + (sword64)ad[ 3] * bd[ 5]
                           + (sword64)ad[ 4] * bd[ 4]
                           + (sword64)ad[ 5] * bd[ 3]
                           + (sword64)ad[ 6] * bd[ 2]
                           + (sword64)ad[ 7] * bd[ 1]
                           + (sword64)ad[ 8] * bd[ 0];
    t[ 9] = (word64)dd[ 9] + (sword64)ad[ 0] * bd[ 9]
                           + (sword64)ad[ 1] * bd[ 8]
                           + (sword64)ad[ 2] * bd[ 7]
                           + (sword64)ad[ 3] * bd[ 6]
                           + (sword64)ad[ 4] * bd[ 5]
                           + (sword64)ad[ 5] * bd[ 4]
                           + (sword64)ad[ 6] * bd[ 3]
                           + (sword64)ad[ 7] * bd[ 2]
                           + (sword64)ad[ 8] * bd[ 1]
                           + (sword64)ad[ 9] * bd[ 0];
    t[10] = (word64)dd[10] + (sword64)ad[ 0] * bd[10]
                           + (sword64)ad[ 1] * bd[ 9]
                           + (sword64)ad[ 2] * bd[ 8]
                           + (sword64)ad[ 3] * bd[ 7]
                           + (sword64)ad[ 4] * bd[ 6]
                           + (sword64)ad[ 5] * bd[ 5]
                           + (sword64)ad[ 6] * bd[ 4]
                           + (sword64)ad[ 7] * bd[ 3]
                           + (sword64)ad[ 8] * bd[ 2]
                           + (sword64)ad[ 9] * bd[ 1]
                           + (sword64)ad[10] * bd[ 0];
    t[11] = (word64)dd[11] + (sword64)ad[ 0] * bd[11]
                           + (sword64)ad[ 1] * bd[10]
                           + (sword64)ad[ 2] * bd[ 9]
                           + (sword64)ad[ 3] * bd[ 8]
                           + (sword64)ad[ 4] * bd[ 7]
                           + (sword64)ad[ 5] * bd[ 6]
                           + (sword64)ad[ 6] * bd[ 5]
                           + (sword64)ad[ 7] * bd[ 4]
                           + (sword64)ad[ 8] * bd[ 3]
                           + (sword64)ad[ 9] * bd[ 2]
                           + (sword64)ad[10] * bd[ 1]
                           + (sword64)ad[11] * bd[ 0];
    t[12] = (word64)dd[12] + (sword64)ad[ 0] * bd[12]
                           + (sword64)ad[ 1] * bd[11]
                           + (sword64)ad[ 2] * bd[10]
                           + (sword64)ad[ 3] * bd[ 9]
                           + (sword64)ad[ 4] * bd[ 8]
                           + (sword64)ad[ 5] * bd[ 7]
                           + (sword64)ad[ 6] * bd[ 6]
                           + (sword64)ad[ 7] * bd[ 5]
                           + (sword64)ad[ 8] * bd[ 4]
                           + (sword64)ad[ 9] * bd[ 3]
                           + (sword64)ad[10] * bd[ 2]
                           + (sword64)ad[11] * bd[ 1]
                           + (sword64)ad[12] * bd[ 0];
    t[13] = (word64)dd[13] + (sword64)ad[ 0] * bd[13]
                           + (sword64)ad[ 1] * bd[12]
                           + (sword64)ad[ 2] * bd[11]
                           + (sword64)ad[ 3] * bd[10]
                           + (sword64)ad[ 4] * bd[ 9]
                           + (sword64)ad[ 5] * bd[ 8]
                           + (sword64)ad[ 6] * bd[ 7]
                           + (sword64)ad[ 7] * bd[ 6]
                           + (sword64)ad[ 8] * bd[ 5]
                           + (sword64)ad[ 9] * bd[ 4]
                           + (sword64)ad[10] * bd[ 3]
                           + (sword64)ad[11] * bd[ 2]
                           + (sword64)ad[12] * bd[ 1]
                           + (sword64)ad[13] * bd[ 0];
    t[14] = (word64)dd[14] + (sword64)ad[ 0] * bd[14]
                           + (sword64)ad[ 1] * bd[13]
                           + (sword64)ad[ 2] * bd[12]
                           + (sword64)ad[ 3] * bd[11]
                           + (sword64)ad[ 4] * bd[10]
                           + (sword64)ad[ 5] * bd[ 9]
                           + (sword64)ad[ 6] * bd[ 8]
                           + (sword64)ad[ 7] * bd[ 7]
                           + (sword64)ad[ 8] * bd[ 6]
                           + (sword64)ad[ 9] * bd[ 5]
                           + (sword64)ad[10] * bd[ 4]
                           + (sword64)ad[11] * bd[ 3]
                           + (sword64)ad[12] * bd[ 2]
                           + (sword64)ad[13] * bd[ 1]
                           + (sword64)ad[14] * bd[ 0];
    t[15] = (word64)dd[15] + (sword64)ad[ 0] * bd[15]
                           + (sword64)ad[ 1] * bd[14]
                           + (sword64)ad[ 2] * bd[13]
                           + (sword64)ad[ 3] * bd[12]
                           + (sword64)ad[ 4] * bd[11]
                           + (sword64)ad[ 5] * bd[10]
                           + (sword64)ad[ 6] * bd[ 9]
                           + (sword64)ad[ 7] * bd[ 8]
                           + (sword64)ad[ 8] * bd[ 7]
                           + (sword64)ad[ 9] * bd[ 6]
                           + (sword64)ad[10] * bd[ 5]
                           + (sword64)ad[11] * bd[ 4]
                           + (sword64)ad[12] * bd[ 3]
                           + (sword64)ad[13] * bd[ 2]
                           + (sword64)ad[14] * bd[ 1]
                           + (sword64)ad[15] * bd[ 0];
    t[16] = (word64)          (sword64)ad[ 1] * bd[15]
                           + (sword64)ad[ 2] * bd[14]
                           + (sword64)ad[ 3] * bd[13]
                           + (sword64)ad[ 4] * bd[12]
                           + (sword64)ad[ 5] * bd[11]
                           + (sword64)ad[ 6] * bd[10]
                           + (sword64)ad[ 7] * bd[ 9]
                           + (sword64)ad[ 8] * bd[ 8]
                           + (sword64)ad[ 9] * bd[ 7]
                           + (sword64)ad[10] * bd[ 6]
                           + (sword64)ad[11] * bd[ 5]
                           + (sword64)ad[12] * bd[ 4]
                           + (sword64)ad[13] * bd[ 3]
                           + (sword64)ad[14] * bd[ 2]
                           + (sword64)ad[15] * bd[ 1];
    t[17] = (word64)          (sword64)ad[ 2] * bd[15]
                           + (sword64)ad[ 3] * bd[14]
                           + (sword64)ad[ 4] * bd[13]
                           + (sword64)ad[ 5] * bd[12]
                           + (sword64)ad[ 6] * bd[11]
                           + (sword64)ad[ 7] * bd[10]
                           + (sword64)ad[ 8] * bd[ 9]
                           + (sword64)ad[ 9] * bd[ 8]
                           + (sword64)ad[10] * bd[ 7]
                           + (sword64)ad[11] * bd[ 6]
                           + (sword64)ad[12] * bd[ 5]
                           + (sword64)ad[13] * bd[ 4]
                           + (sword64)ad[14] * bd[ 3]
                           + (sword64)ad[15] * bd[ 2];
    t[18] = (word64)          (sword64)ad[ 3] * bd[15]
                           + (sword64)ad[ 4] * bd[14]
                           + (sword64)ad[ 5] * bd[13]
                           + (sword64)ad[ 6] * bd[12]
                           + (sword64)ad[ 7] * bd[11]
                           + (sword64)ad[ 8] * bd[10]
                           + (sword64)ad[ 9] * bd[ 9]
                           + (sword64)ad[10] * bd[ 8]
                           + (sword64)ad[11] * bd[ 7]
                           + (sword64)ad[12] * bd[ 6]
                           + (sword64)ad[13] * bd[ 5]
                           + (sword64)ad[14] * bd[ 4]
                           + (sword64)ad[15] * bd[ 3];
    t[19] = (word64)          (sword64)ad[ 4] * bd[15]
                           + (sword64)ad[ 5] * bd[14]
                           + (sword64)ad[ 6] * bd[13]
                           + (sword64)ad[ 7] * bd[12]
                           + (sword64)ad[ 8] * bd[11]
                           + (sword64)ad[ 9] * bd[10]
                           + (sword64)ad[10] * bd[ 9]
                           + (sword64)ad[11] * bd[ 8]
                           + (sword64)ad[12] * bd[ 7]
                           + (sword64)ad[13] * bd[ 6]
                           + (sword64)ad[14] * bd[ 5]
                           + (sword64)ad[15] * bd[ 4];
    t[20] = (word64)          (sword64)ad[ 5] * bd[15]
                           + (sword64)ad[ 6] * bd[14]
                           + (sword64)ad[ 7] * bd[13]
                           + (sword64)ad[ 8] * bd[12]
                           + (sword64)ad[ 9] * bd[11]
                           + (sword64)ad[10] * bd[10]
                           + (sword64)ad[11] * bd[ 9]
                           + (sword64)ad[12] * bd[ 8]
                           + (sword64)ad[13] * bd[ 7]
                           + (sword64)ad[14] * bd[ 6]
                           + (sword64)ad[15] * bd[ 5];
    t[21] = (word64)          (sword64)ad[ 6] * bd[15]
                           + (sword64)ad[ 7] * bd[14]
                           + (sword64)ad[ 8] * bd[13]
                           + (sword64)ad[ 9] * bd[12]
                           + (sword64)ad[10] * bd[11]
                           + (sword64)ad[11] * bd[10]
                           + (sword64)ad[12] * bd[ 9]
                           + (sword64)ad[13] * bd[ 8]
                           + (sword64)ad[14] * bd[ 7]
                           + (sword64)ad[15] * bd[ 6];
    t[22] = (word64)          (sword64)ad[ 7] * bd[15]
                           + (sword64)ad[ 8] * bd[14]
                           + (sword64)ad[ 9] * bd[13]
                           + (sword64)ad[10] * bd[12]
                           + (sword64)ad[11] * bd[11]
                           + (sword64)ad[12] * bd[10]
                           + (sword64)ad[13] * bd[ 9]
                           + (sword64)ad[14] * bd[ 8]
                           + (sword64)ad[15] * bd[ 7];
    t[23] = (word64)          (sword64)ad[ 8] * bd[15]
                           + (sword64)ad[ 9] * bd[14]
                           + (sword64)ad[10] * bd[13]
                           + (sword64)ad[11] * bd[12]
                           + (sword64)ad[12] * bd[11]
                           + (sword64)ad[13] * bd[10]
                           + (sword64)ad[14] * bd[ 9]
                           + (sword64)ad[15] * bd[ 8];
    t[24] = (word64)          (sword64)ad[ 9] * bd[15]
                           + (sword64)ad[10] * bd[14]
                           + (sword64)ad[11] * bd[13]
                           + (sword64)ad[12] * bd[12]
                           + (sword64)ad[13] * bd[11]
                           + (sword64)ad[14] * bd[10]
                           + (sword64)ad[15] * bd[ 9];
    t[25] = (word64)          (sword64)ad[10] * bd[15]
                           + (sword64)ad[11] * bd[14]
                           + (sword64)ad[12] * bd[13]
                           + (sword64)ad[13] * bd[12]
                           + (sword64)ad[14] * bd[11]
                           + (sword64)ad[15] * bd[10];
    t[26] = (word64)          (sword64)ad[11] * bd[15]
                           + (sword64)ad[12] * bd[14]
                           + (sword64)ad[13] * bd[13]
                           + (sword64)ad[14] * bd[12]
                           + (sword64)ad[15] * bd[11];
    t[27] = (word64)          (sword64)ad[12] * bd[15]
                           + (sword64)ad[13] * bd[14]
                           + (sword64)ad[14] * bd[13]
                           + (sword64)ad[15] * bd[12];
    t[28] = (word64)          (sword64)ad[13] * bd[15]
                           + (sword64)ad[14] * bd[14]
                           + (sword64)ad[15] * bd[13];
    t[29] = (word64)          (sword64)ad[14] * bd[15]
                           + (sword64)ad[15] * bd[14];
    t[30] = (word64)          (sword64)ad[15] * bd[15];
    t[31] = 0;

    /* Mod curve order */
    /* 2^446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d */
    /* Propagate carries */
    c = t[ 0] >> 28; t[ 1] += c; t[ 0] = t[ 0] & 0xfffffff;
    c = t[ 1] >> 28; t[ 2] += c; t[ 1] = t[ 1] & 0xfffffff;
    c = t[ 2] >> 28; t[ 3] += c; t[ 2] = t[ 2] & 0xfffffff;
    c = t[ 3] >> 28; t[ 4] += c; t[ 3] = t[ 3] & 0xfffffff;
    c = t[ 4] >> 28; t[ 5] += c; t[ 4] = t[ 4] & 0xfffffff;
    c = t[ 5] >> 28; t[ 6] += c; t[ 5] = t[ 5] & 0xfffffff;
    c = t[ 6] >> 28; t[ 7] += c; t[ 6] = t[ 6] & 0xfffffff;
    c = t[ 7] >> 28; t[ 8] += c; t[ 7] = t[ 7] & 0xfffffff;
    c = t[ 8] >> 28; t[ 9] += c; t[ 8] = t[ 8] & 0xfffffff;
    c = t[ 9] >> 28; t[10] += c; t[ 9] = t[ 9] & 0xfffffff;
    c = t[10] >> 28; t[11] += c; t[10] = t[10] & 0xfffffff;
    c = t[11] >> 28; t[12] += c; t[11] = t[11] & 0xfffffff;
    c = t[12] >> 28; t[13] += c; t[12] = t[12] & 0xfffffff;
    c = t[13] >> 28; t[14] += c; t[13] = t[13] & 0xfffffff;
    c = t[14] >> 28; t[15] += c; t[14] = t[14] & 0xfffffff;
    c = t[15] >> 28; t[16] += c; t[15] = t[15] & 0xfffffff;
    c = t[16] >> 28; t[17] += c; t[16] = t[16] & 0xfffffff;
    c = t[17] >> 28; t[18] += c; t[17] = t[17] & 0xfffffff;
    c = t[18] >> 28; t[19] += c; t[18] = t[18] & 0xfffffff;
    c = t[19] >> 28; t[20] += c; t[19] = t[19] & 0xfffffff;
    c = t[20] >> 28; t[21] += c; t[20] = t[20] & 0xfffffff;
    c = t[21] >> 28; t[22] += c; t[21] = t[21] & 0xfffffff;
    c = t[22] >> 28; t[23] += c; t[22] = t[22] & 0xfffffff;
    c = t[23] >> 28; t[24] += c; t[23] = t[23] & 0xfffffff;
    c = t[24] >> 28; t[25] += c; t[24] = t[24] & 0xfffffff;
    c = t[25] >> 28; t[26] += c; t[25] = t[25] & 0xfffffff;
    c = t[26] >> 28; t[27] += c; t[26] = t[26] & 0xfffffff;
    c = t[27] >> 28; t[28] += c; t[27] = t[27] & 0xfffffff;
    c = t[28] >> 28; t[29] += c; t[28] = t[28] & 0xfffffff;
    c = t[29] >> 28; t[30] += c; t[29] = t[29] & 0xfffffff;
    c = t[30] >> 28; t[31] += c; t[30] = t[30] & 0xfffffff;
    /* Mod top half of extra words */
    t[ 8] += (sword64)0x129eec34 * t[24];
    t[ 9] += (sword64)0x21cf5b54 * t[24];
    t[10] += (sword64)0x29c2ab70 * t[24];
    t[11] += (sword64)0x0f635c8c * t[24];
    t[12] += (sword64)0x25bf7a4c * t[24];
    t[13] += (sword64)0x2d944a70 * t[24];
    t[14] += (sword64)0x18eec490 * t[24];
    t[15] += (sword64)0x20cd7704 * t[24];
    t[ 9] += (sword64)0x129eec34 * t[25];
    t[10] += (sword64)0x21cf5b54 * t[25];
    t[11] += (sword64)0x29c2ab70 * t[25];
    t[12] += (sword64)0x0f635c8c * t[25];
    t[13] += (sword64)0x25bf7a4c * t[25];
    t[14] += (sword64)0x2d944a70 * t[25];
    t[15] += (sword64)0x18eec490 * t[25];
    t[16] += (sword64)0x20cd7704 * t[25];
    t[10] += (sword64)0x129eec34 * t[26];
    t[11] += (sword64)0x21cf5b54 * t[26];
    t[12] += (sword64)0x29c2ab70 * t[26];
    t[13] += (sword64)0x0f635c8c * t[26];
    t[14] += (sword64)0x25bf7a4c * t[26];
    t[15] += (sword64)0x2d944a70 * t[26];
    t[16] += (sword64)0x18eec490 * t[26];
    t[17] += (sword64)0x20cd7704 * t[26];
    t[11] += (sword64)0x129eec34 * t[27];
    t[12] += (sword64)0x21cf5b54 * t[27];
    t[13] += (sword64)0x29c2ab70 * t[27];
    t[14] += (sword64)0x0f635c8c * t[27];
    t[15] += (sword64)0x25bf7a4c * t[27];
    t[16] += (sword64)0x2d944a70 * t[27];
    t[17] += (sword64)0x18eec490 * t[27];
    t[18] += (sword64)0x20cd7704 * t[27];
    t[12] += (sword64)0x129eec34 * t[28];
    t[13] += (sword64)0x21cf5b54 * t[28];
    t[14] += (sword64)0x29c2ab70 * t[28];
    t[15] += (sword64)0x0f635c8c * t[28];
    t[16] += (sword64)0x25bf7a4c * t[28];
    t[17] += (sword64)0x2d944a70 * t[28];
    t[18] += (sword64)0x18eec490 * t[28];
    t[19] += (sword64)0x20cd7704 * t[28];
    t[13] += (sword64)0x129eec34 * t[29];
    t[14] += (sword64)0x21cf5b54 * t[29];
    t[15] += (sword64)0x29c2ab70 * t[29];
    t[16] += (sword64)0x0f635c8c * t[29];
    t[17] += (sword64)0x25bf7a4c * t[29];
    t[18] += (sword64)0x2d944a70 * t[29];
    t[19] += (sword64)0x18eec490 * t[29];
    t[20] += (sword64)0x20cd7704 * t[29];
    t[14] += (sword64)0x129eec34 * t[30];
    t[15] += (sword64)0x21cf5b54 * t[30];
    t[16] += (sword64)0x29c2ab70 * t[30];
    t[17] += (sword64)0x0f635c8c * t[30];
    t[18] += (sword64)0x25bf7a4c * t[30];
    t[19] += (sword64)0x2d944a70 * t[30];
    t[20] += (sword64)0x18eec490 * t[30];
    t[21] += (sword64)0x20cd7704 * t[30];
    t[15] += (sword64)0x129eec34 * t[31];
    t[16] += (sword64)0x21cf5b54 * t[31];
    t[17] += (sword64)0x29c2ab70 * t[31];
    t[18] += (sword64)0x0f635c8c * t[31];
    t[19] += (sword64)0x25bf7a4c * t[31];
    t[20] += (sword64)0x2d944a70 * t[31];
    t[21] += (sword64)0x18eec490 * t[31];
    t[22] += (sword64)0x20cd7704 * t[31];
    /* Propagate carries */
    c = t[ 8] >> 28; t[ 9] += c; t[ 8] = t[ 8] & 0xfffffff;
    c = t[ 9] >> 28; t[10] += c; t[ 9] = t[ 9] & 0xfffffff;
    c = t[10] >> 28; t[11] += c; t[10] = t[10] & 0xfffffff;
    c = t[11] >> 28; t[12] += c; t[11] = t[11] & 0xfffffff;
    c = t[12] >> 28; t[13] += c; t[12] = t[12] & 0xfffffff;
    c = t[13] >> 28; t[14] += c; t[13] = t[13] & 0xfffffff;
    c = t[14] >> 28; t[15] += c; t[14] = t[14] & 0xfffffff;
    c = t[15] >> 28; t[16] += c; t[15] = t[15] & 0xfffffff;
    c = t[16] >> 28; t[17] += c; t[16] = t[16] & 0xfffffff;
    c = t[17] >> 28; t[18] += c; t[17] = t[17] & 0xfffffff;
    c = t[18] >> 28; t[19] += c; t[18] = t[18] & 0xfffffff;
    c = t[19] >> 28; t[20] += c; t[19] = t[19] & 0xfffffff;
    c = t[20] >> 28; t[21] += c; t[20] = t[20] & 0xfffffff;
    c = t[21] >> 28; t[22] += c; t[21] = t[21] & 0xfffffff;
    c = t[22] >> 28; t[23] += c; t[22] = t[22] & 0xfffffff;
    /* Mod bottom half of extra words */
    t[ 0] += (sword64)0x129eec34 * t[16];
    t[ 1] += (sword64)0x21cf5b54 * t[16];
    t[ 2] += (sword64)0x29c2ab70 * t[16];
    t[ 3] += (sword64)0x0f635c8c * t[16];
    t[ 4] += (sword64)0x25bf7a4c * t[16];
    t[ 5] += (sword64)0x2d944a70 * t[16];
    t[ 6] += (sword64)0x18eec490 * t[16];
    t[ 7] += (sword64)0x20cd7704 * t[16];
    t[ 1] += (sword64)0x129eec34 * t[17];
    t[ 2] += (sword64)0x21cf5b54 * t[17];
    t[ 3] += (sword64)0x29c2ab70 * t[17];
    t[ 4] += (sword64)0x0f635c8c * t[17];
    t[ 5] += (sword64)0x25bf7a4c * t[17];
    t[ 6] += (sword64)0x2d944a70 * t[17];
    t[ 7] += (sword64)0x18eec490 * t[17];
    t[ 8] += (sword64)0x20cd7704 * t[17];
    t[ 2] += (sword64)0x129eec34 * t[18];
    t[ 3] += (sword64)0x21cf5b54 * t[18];
    t[ 4] += (sword64)0x29c2ab70 * t[18];
    t[ 5] += (sword64)0x0f635c8c * t[18];
    t[ 6] += (sword64)0x25bf7a4c * t[18];
    t[ 7] += (sword64)0x2d944a70 * t[18];
    t[ 8] += (sword64)0x18eec490 * t[18];
    t[ 9] += (sword64)0x20cd7704 * t[18];
    t[ 3] += (sword64)0x129eec34 * t[19];
    t[ 4] += (sword64)0x21cf5b54 * t[19];
    t[ 5] += (sword64)0x29c2ab70 * t[19];
    t[ 6] += (sword64)0x0f635c8c * t[19];
    t[ 7] += (sword64)0x25bf7a4c * t[19];
    t[ 8] += (sword64)0x2d944a70 * t[19];
    t[ 9] += (sword64)0x18eec490 * t[19];
    t[10] += (sword64)0x20cd7704 * t[19];
    t[ 4] += (sword64)0x129eec34 * t[20];
    t[ 5] += (sword64)0x21cf5b54 * t[20];
    t[ 6] += (sword64)0x29c2ab70 * t[20];
    t[ 7] += (sword64)0x0f635c8c * t[20];
    t[ 8] += (sword64)0x25bf7a4c * t[20];
    t[ 9] += (sword64)0x2d944a70 * t[20];
    t[10] += (sword64)0x18eec490 * t[20];
    t[11] += (sword64)0x20cd7704 * t[20];
    t[ 5] += (sword64)0x129eec34 * t[21];
    t[ 6] += (sword64)0x21cf5b54 * t[21];
    t[ 7] += (sword64)0x29c2ab70 * t[21];
    t[ 8] += (sword64)0x0f635c8c * t[21];
    t[ 9] += (sword64)0x25bf7a4c * t[21];
    t[10] += (sword64)0x2d944a70 * t[21];
    t[11] += (sword64)0x18eec490 * t[21];
    t[12] += (sword64)0x20cd7704 * t[21];
    t[ 6] += (sword64)0x129eec34 * t[22];
    t[ 7] += (sword64)0x21cf5b54 * t[22];
    t[ 8] += (sword64)0x29c2ab70 * t[22];
    t[ 9] += (sword64)0x0f635c8c * t[22];
    t[10] += (sword64)0x25bf7a4c * t[22];
    t[11] += (sword64)0x2d944a70 * t[22];
    t[12] += (sword64)0x18eec490 * t[22];
    t[13] += (sword64)0x20cd7704 * t[22];
    t[ 7] += (sword64)0x129eec34 * t[23];
    t[ 8] += (sword64)0x21cf5b54 * t[23];
    t[ 9] += (sword64)0x29c2ab70 * t[23];
    t[10] += (sword64)0x0f635c8c * t[23];
    t[11] += (sword64)0x25bf7a4c * t[23];
    t[12] += (sword64)0x2d944a70 * t[23];
    t[13] += (sword64)0x18eec490 * t[23];
    t[14] += (sword64)0x20cd7704 * t[23];
    /* Propagate carries */
    c = t[ 0] >> 28; t[ 1] += c; rd[ 0] = (sword32)(t[ 0] & 0xfffffff);
    c = t[ 1] >> 28; t[ 2] += c; rd[ 1] = (sword32)(t[ 1] & 0xfffffff);
    c = t[ 2] >> 28; t[ 3] += c; rd[ 2] = (sword32)(t[ 2] & 0xfffffff);
    c = t[ 3] >> 28; t[ 4] += c; rd[ 3] = (sword32)(t[ 3] & 0xfffffff);
    c = t[ 4] >> 28; t[ 5] += c; rd[ 4] = (sword32)(t[ 4] & 0xfffffff);
    c = t[ 5] >> 28; t[ 6] += c; rd[ 5] = (sword32)(t[ 5] & 0xfffffff);
    c = t[ 6] >> 28; t[ 7] += c; rd[ 6] = (sword32)(t[ 6] & 0xfffffff);
    c = t[ 7] >> 28; t[ 8] += c; rd[ 7] = (sword32)(t[ 7] & 0xfffffff);
    c = t[ 8] >> 28; t[ 9] += c; rd[ 8] = (sword32)(t[ 8] & 0xfffffff);
    c = t[ 9] >> 28; t[10] += c; rd[ 9] = (sword32)(t[ 9] & 0xfffffff);
    c = t[10] >> 28; t[11] += c; rd[10] = (sword32)(t[10] & 0xfffffff);
    c = t[11] >> 28; t[12] += c; rd[11] = (sword32)(t[11] & 0xfffffff);
    c = t[12] >> 28; t[13] += c; rd[12] = (sword32)(t[12] & 0xfffffff);
    c = t[13] >> 28; t[14] += c; rd[13] = (sword32)(t[13] & 0xfffffff);
    c = t[14] >> 28; t[15] += c; rd[14] = (sword32)(t[14] & 0xfffffff);
    rd[15] = (word32)t[15];
    /* Mod bits over 28 in last word */
    o = rd[15] >> 26; rd[15] &= 0x3ffffff;
    rd[ 0] += 0x4a7bb0d * o;
    rd[ 1] += 0x873d6d5 * o;
    rd[ 2] += 0xa70aadc * o;
    rd[ 3] += 0x3d8d723 * o;
    rd[ 4] += 0x96fde93 * o;
    rd[ 5] += 0xb65129c * o;
    rd[ 6] += 0x63bb124 * o;
    rd[ 7] += 0x8335dc1 * o;
    /* Propagate carries */
    o = rd[ 0] >> 28; rd[ 1] += o; rd[ 0] = rd[ 0] & 0xfffffff;
    o = rd[ 1] >> 28; rd[ 2] += o; rd[ 1] = rd[ 1] & 0xfffffff;
    o = rd[ 2] >> 28; rd[ 3] += o; rd[ 2] = rd[ 2] & 0xfffffff;
    o = rd[ 3] >> 28; rd[ 4] += o; rd[ 3] = rd[ 3] & 0xfffffff;
    o = rd[ 4] >> 28; rd[ 5] += o; rd[ 4] = rd[ 4] & 0xfffffff;
    o = rd[ 5] >> 28; rd[ 6] += o; rd[ 5] = rd[ 5] & 0xfffffff;
    o = rd[ 6] >> 28; rd[ 7] += o; rd[ 6] = rd[ 6] & 0xfffffff;
    o = rd[ 7] >> 28; rd[ 8] += o; rd[ 7] = rd[ 7] & 0xfffffff;
    o = rd[ 8] >> 28; rd[ 9] += o; rd[ 8] = rd[ 8] & 0xfffffff;
    o = rd[ 9] >> 28; rd[10] += o; rd[ 9] = rd[ 9] & 0xfffffff;
    o = rd[10] >> 28; rd[11] += o; rd[10] = rd[10] & 0xfffffff;
    o = rd[11] >> 28; rd[12] += o; rd[11] = rd[11] & 0xfffffff;
    o = rd[12] >> 28; rd[13] += o; rd[12] = rd[12] & 0xfffffff;
    o = rd[13] >> 28; rd[14] += o; rd[13] = rd[13] & 0xfffffff;
    o = rd[14] >> 28; rd[15] += o; rd[14] = rd[14] & 0xfffffff;

    /* Convert to bytes */
    r[ 0] = (byte)(rd[0 ] >>  0);
    r[ 1] = (byte)(rd[0 ] >>  8);
    r[ 2] = (byte)(rd[0 ] >> 16);
    r[ 3] = (byte)(rd[0 ] >> 24) + ((rd[1 ] >>  0) <<  4);
    r[ 4] = (byte)(rd[1 ] >>  4);
    r[ 5] = (byte)(rd[1 ] >> 12);
    r[ 6] = (byte)(rd[1 ] >> 20);
    r[ 7] = (byte)(rd[2 ] >>  0);
    r[ 8] = (byte)(rd[2 ] >>  8);
    r[ 9] = (byte)(rd[2 ] >> 16);
    r[10] = (byte)(rd[2 ] >> 24) + ((rd[3 ] >>  0) <<  4);
    r[11] = (byte)(rd[3 ] >>  4);
    r[12] = (byte)(rd[3 ] >> 12);
    r[13] = (byte)(rd[3 ] >> 20);
    r[14] = (byte)(rd[4 ] >>  0);
    r[15] = (byte)(rd[4 ] >>  8);
    r[16] = (byte)(rd[4 ] >> 16);
    r[17] = (byte)(rd[4 ] >> 24) + ((rd[5 ] >>  0) <<  4);
    r[18] = (byte)(rd[5 ] >>  4);
    r[19] = (byte)(rd[5 ] >> 12);
    r[20] = (byte)(rd[5 ] >> 20);
    r[21] = (byte)(rd[6 ] >>  0);
    r[22] = (byte)(rd[6 ] >>  8);
    r[23] = (byte)(rd[6 ] >> 16);
    r[24] = (byte)(rd[6 ] >> 24) + ((rd[7 ] >>  0) <<  4);
    r[25] = (byte)(rd[7 ] >>  4);
    r[26] = (byte)(rd[7 ] >> 12);
    r[27] = (byte)(rd[7 ] >> 20);
    r[28] = (byte)(rd[8 ] >>  0);
    r[29] = (byte)(rd[8 ] >>  8);
    r[30] = (byte)(rd[8 ] >> 16);
    r[31] = (byte)(rd[8 ] >> 24) + ((rd[9 ] >>  0) <<  4);
    r[32] = (byte)(rd[9 ] >>  4);
    r[33] = (byte)(rd[9 ] >> 12);
    r[34] = (byte)(rd[9 ] >> 20);
    r[35] = (byte)(rd[10] >>  0);
    r[36] = (byte)(rd[10] >>  8);
    r[37] = (byte)(rd[10] >> 16);
    r[38] = (byte)(rd[10] >> 24) + ((rd[11] >>  0) <<  4);
    r[39] = (byte)(rd[11] >>  4);
    r[40] = (byte)(rd[11] >> 12);
    r[41] = (byte)(rd[11] >> 20);
    r[42] = (byte)(rd[12] >>  0);
    r[43] = (byte)(rd[12] >>  8);
    r[44] = (byte)(rd[12] >> 16);
    r[45] = (byte)(rd[12] >> 24) + ((rd[13] >>  0) <<  4);
    r[46] = (byte)(rd[13] >>  4);
    r[47] = (byte)(rd[13] >> 12);
    r[48] = (byte)(rd[13] >> 20);
    r[49] = (byte)(rd[14] >>  0);
    r[50] = (byte)(rd[14] >>  8);
    r[51] = (byte)(rd[14] >> 16);
    r[52] = (byte)(rd[14] >> 24) + ((rd[15] >>  0) <<  4);
    r[53] = (byte)(rd[15] >>  4);
    r[54] = (byte)(rd[15] >> 12);
    r[55] = (byte)(rd[15] >> 20);
    r[56] = 0;
}

/* Precomputed multiples of the base point. */
static const ge448_precomp base[58][8] = {
{
    {
        { 0x70cc05e, 0x26a82bc, 0x0938e26, 0x80e18b0, 0x511433b, 0xf72ab66,
          0x412ae1a, 0xa3d3a46, 0xa6de324, 0x0f1767e, 0x4657047, 0x36da9e1,
          0x5a622bf, 0xed221d1, 0x66bed0d, 0x4f1970c },
        { 0x230fa14, 0x08795bf, 0x7c8ad98, 0x132c4ed, 0x9c4fdbd, 0x1ce67c3,
          0x73ad3ff, 0x05a0c2d, 0x7789c1e, 0xa398408, 0xa73736c, 0xc7624be,
          0x03756c9, 0x2488762, 0x16eb6bc, 0x693f467 }
    },
    {
        { 0x5555555, 0x5555555, 0x5555555, 0x5555555, 0x5555555, 0x5555555,
          0x5555555, 0x5555555, 0xaaaaaa9, 0xaaaaaaa, 0xaaaaaaa, 0xaaaaaaa,
          0xaaaaaaa, 0xaaaaaaa, 0xaaaaaaa, 0xaaaaaaa },
        { 0xa9386ed, 0xeafbcde, 0xda06bda, 0xb2bed1c, 0x098bbbc, 0x833a2a3,
          0x80d6565, 0x8ad8c4b, 0x7e36d72, 0x884dd7b, 0xed7a035, 0xc2b0036,
          0x6205086, 0x8db359d, 0x34ad704, 0xae05e96 }
    },
    {
        { 0x6ff2f8f, 0x2817328, 0xda85757, 0xb769465, 0xfd6e862, 0xf7f6271,
          0x8daa9cb, 0x4a3fcfe, 0x2ba077a, 0xda82c7e, 0x41b8b8c, 0x9433322,
          0x4316cb6, 0x6455bd6, 0xb9108af, 0x0865886 },
        { 0x88ed6fc, 0x22ac135, 0x02dafb8, 0x9a68fed, 0x7f0bffa, 0x1bdb676,
          0x8bb3a33, 0xec4e1d5, 0xce43c82, 0x56c3b9f, 0xa8d9523, 0xa6449a4,
          0xa7ad43a, 0xf706cbd, 0xbd5125c, 0xe005a8d }
    },
    {
        { 0x8ba7f30, 0xce42ac4, 0x9e120e2, 0xe179894, 0x8ba21ae, 0xf1515dd,
          0x301b7bd, 0x70c74cc, 0x3fda4be, 0x0891c69, 0xa09cf4e, 0x29ea255,
          0x17226f9, 0x2c1419a, 0xc6c0cce, 0x49dcbc5 },
        { 0xde51839, 0xe236f86, 0xd4f5b32, 0x44285d0, 0x472b5d4, 0x7ea1ca9,
          0x1c0d8f9, 0x7b8a5bc, 0x90dc322, 0x57d845c, 0x7c02f04, 0x1b979cb,
          0x3a5de02, 0x27164b3, 0x4accde5, 0xd49077e }
    },
    {
        { 0x2030034, 0xa99d109, 0x6f950d0, 0x2d8cefc, 0xc96f07b, 0x7a920c3,
          0x08bc0d5, 0x9588128, 0x6d761e8, 0x62ada75, 0xbcf7285, 0x0def80c,
          0x01eedb5, 0x0e2ba76, 0x5a48dcb, 0x7a9f933 },
        { 0x2f435eb, 0xb473147, 0xf225443, 0x5512881, 0x33c5840, 0xee59d2b,
          0x127d7a4, 0xb698017, 0x86551f7, 0xb18fced, 0xca1823a, 0x0ade260,
          0xce4fd58, 0xd3b9109, 0xa2517ed, 0xadfd751 }
    },
    {
        { 0xabef79c, 0x7fd7652, 0x443a878, 0x6c20a07, 0x12a7109, 0x5c1840d,
          0x876451c, 0x4a06e4a, 0xad95f65, 0x3bed0b4, 0x3fb0260, 0x25d2e67,
          0xaebd971, 0x2e00349, 0x4498b72, 0x54523e0 },
        { 0x07c7bcc, 0xea5d1da, 0x38ea98c, 0xcce7769, 0x61d2b3e, 0x80284e8,
          0x6e1ff1b, 0x48de76b, 0x9c58522, 0x7b12186, 0x2765a1a, 0xbfd053a,
          0x056c667, 0x2d743ec, 0xd8ab61c, 0x3f99b9c }
    },
    {
        { 0xeb5eaf7, 0xdf9567c, 0x78ac7d7, 0x110a6b4, 0x4706e0b, 0x2d33501,
          0x0b5a209, 0x0df9c7b, 0x568e684, 0xba4223d, 0x8c3719b, 0xd78af2d,
          0xa5291b6, 0x77467b9, 0x5c89bef, 0x079748e },
        { 0xdac377f, 0xe20d3fa, 0x72b5c09, 0x34e8669, 0xc40bbb7, 0xd8687a3,
          0xd2f84c9, 0x7b3946f, 0xa78f50e, 0xd00e40c, 0x17e7179, 0xb875944,
          0xcb23583, 0x9c7373b, 0xc90fd69, 0x7ddeda3 }
    },
    {
        { 0x153bde0, 0x2538a67, 0x406b696, 0x223aca9, 0x1ad713e, 0xf9080dc,
          0xd816a64, 0x6c4cb47, 0x5dc8b97, 0xbc28568, 0xc08e2d7, 0xd97b037,
          0x5d0e66b, 0x5b63fb4, 0x520e8a3, 0xd1f1bc5 },
        { 0xe69e09b, 0x4eb873c, 0xbc8ee45, 0x1663164, 0xba8d89f, 0x08f7003,
          0x386ad82, 0x4b98ead, 0xbd94c7b, 0xa4b93b7, 0xc6b38b3, 0x46ba408,
          0xf3574ff, 0xdae87d1, 0xe9bea9b, 0xc7564f4 }
    },
},
{
    {
        { 0x5bfac1c, 0x2e4fdb2, 0xf5f3bca, 0xf0d79aa, 0x20fb7cc, 0xe756b0d,
          0xb39609a, 0xe3696be, 0x5a5ab58, 0xa019fc3, 0x3b281dd, 0xa2b2485,
          0x61ac0a2, 0xe3e2be7, 0xeb56730, 0xf19c34f },
        { 0xa30241e, 0x2d25ce8, 0xb73d7a1, 0xf5661ea, 0xdaac9f4, 0x4611ed0,
          0x4ced72c, 0xd544234, 0xe92e985, 0xce78f52, 0x4da4aad, 0x6fe5dd4,
          0x1d363ce, 0xfcaddc6, 0xc9111bf, 0x3beb69c }
    },
    {
        { 0x940ebc9, 0xd2e7660, 0xb17bbe0, 0xe032018, 0x75c0575, 0xad49391,
          0x21c7f34, 0xdd0b147, 0x3e147e0, 0x52c2ba4, 0x0ee8973, 0x7dd03c6,
          0xecf2754, 0x5472e8d, 0xd6482bb, 0x17a1cd1 },
        { 0x8128b3f, 0xdd43b84, 0xea7dd25, 0xf0cae34, 0xff07df2, 0x81ca99f,
          0x92ebbdc, 0x1c89597, 0x72155e6, 0x45c7a68, 0x39ddd08, 0x907a50e,
          0xbb2d89b, 0xbe398c2, 0x1b3b536, 0x38063f9 }
    },
    {
        { 0xf843b23, 0x149fafb, 0xac7f22a, 0x00ab582, 0xf2f4d4c, 0xa3b981b,
          0x4341a22, 0x2ce1a65, 0x7c03b63, 0x68a4074, 0x12f2cf8, 0x63206a2,
          0x5149741, 0xc9961d3, 0xbc7099e, 0xfb85430 },
        { 0x90a9e59, 0x9c91072, 0x06de367, 0x734e94a, 0xdb99214, 0x5cf3cbe,
          0x45b1fb9, 0xc6bce32, 0xdd7be0d, 0x1a82abe, 0xede7d1c, 0xf74976a,
          0x21503bd, 0x7025b7c, 0x0d096ab, 0xf789491 }
    },
    {
        { 0x555a41b, 0x6bd48bb, 0x67de206, 0xfbdd0d0, 0xdd6dfd1, 0x98bc477,
          0x3e40b8a, 0x1d0693b, 0xda32ae4, 0x6e15563, 0xfcebaa2, 0x0194a20,
          0x0980a93, 0xda11615, 0x0109cec, 0x8e11920 },
        { 0xffb9726, 0x8ea0552, 0x047e44b, 0xeba50a4, 0x60ddf76, 0xc050d24,
          0xac690e0, 0xe009204, 0x9b18edc, 0x47b8639, 0xc77f23f, 0x2f5b76a,
          0x0792905, 0x4296c24, 0x06f6dc7, 0x73f6b4a }
    },
    {
        { 0x3b10cad, 0xb6ef9ea, 0xf7c8fce, 0x312843d, 0x8bedf86, 0x5bdcd52,
          0xf6dd823, 0x2889059, 0x08bfde0, 0x04578e9, 0x123e2e5, 0x3245df3,
          0x7ee9e3a, 0xbf461d5, 0x6f94ceb, 0xddec2d4 },
        { 0x145768f, 0x21b43b9, 0xdae962a, 0xe79a8f9, 0xcbb043f, 0xff1972b,
          0x239649b, 0xe3dcf6d, 0xc533b85, 0xed592bd, 0xdbe22d0, 0x14ff94f,
          0xf1d8e22, 0x6c4eb87, 0xd18cf6d, 0xd8d4c71 }
    },
    {
        { 0x8d96345, 0xcda666c, 0x836cd21, 0x9ecaa25, 0x984606e, 0x6e885bd,
          0x804f054, 0x1dd5fef, 0x6959ae4, 0x9dfff6b, 0xc9b55cc, 0x99b9cf8,
          0x62b9b80, 0xb4716b0, 0x554b128, 0x13ec87c },
        { 0x75aacc2, 0xe696d1f, 0x87fc5ff, 0xf78c993, 0x3809d42, 0x76c0947,
          0xb618fa8, 0x99ce62d, 0x2f53341, 0x35e3e02, 0x0db6c5e, 0x62fc1ac,
          0x00d8b47, 0xa1fb8e6, 0x58f0d1e, 0x0bc1070 }
    },
    {
        { 0x16da513, 0x1f45269, 0xf5cf341, 0x1f2fc04, 0x64d23e0, 0xae92086,
          0xda8a113, 0x4e33082, 0x1cfc085, 0x2688ec6, 0x6e5327f, 0x6f2e8de,
          0xb4e48a8, 0x2070db3, 0x3240ade, 0xd662697 },
        { 0xfbd997b, 0xa6b317f, 0x49e26bd, 0x9fa1b56, 0x8cba0f3, 0xcbf0d25,
          0x17b4745, 0x4a7791b, 0x5c9e190, 0x25f555b, 0x923ec4c, 0x7cd3940,
          0xe98f1b6, 0x16f4c6a, 0xbcd4e0f, 0x7962116 }
    },
    {
        { 0x02491e3, 0x8d58fa3, 0x7ab3898, 0x7cf76c6, 0x647ebc7, 0xbc2f657,
          0xd25f5a3, 0x5f4bfe0, 0xd69505d, 0x503f478, 0x3fb6645, 0x4a889fc,
          0xfa86b18, 0x33e1bc1, 0x5508dd8, 0xabb234f },
        { 0x9a05b48, 0x5348e1b, 0x64dc858, 0x57ac5f1, 0xec8a2d3, 0x21f4d38,
          0xa3a3e9d, 0x5ec6d3c, 0x560a0b8, 0xcd4062e, 0x3433f59, 0x49b74f7,
          0xcab14e3, 0xefd9d87, 0xeb964f5, 0x858ce7f }
    },
},
{
    {
        { 0xeb731b4, 0x7577254, 0x4e2397e, 0x9fff1fb, 0xc821715, 0x749b145,
          0x2e65e67, 0x40619fe, 0x2e618d8, 0x57b8281, 0x707b83e, 0x063186c,
          0x31b24a2, 0xcfc80cb, 0xac75169, 0xcca6185 },
        { 0xb255818, 0x6539f44, 0x0368bce, 0x5895da0, 0x17c7482, 0x841a309,
          0xb1a9c9e, 0x85469e1, 0xe4f7d9d, 0x05664c0, 0x7b35cc0, 0x8a06318,
          0xa0e9b0a, 0x214763a, 0x4b26ac2, 0x1bd872c }
    },
    {
        { 0xa93762b, 0x3578f97, 0x72d52bc, 0x434f69a, 0x22cb565, 0xddcca40,
          0xff20544, 0xa7d1e41, 0x8a66588, 0x823475d, 0x99d7baf, 0x9fc97c7,
          0x660e421, 0x15542f1, 0x843faf6, 0xa7d1f60 },
        { 0x4063ccc, 0xbbfaab5, 0xa49855a, 0x3ad9bad, 0x5bddbfe, 0xffd5f1c,
          0xae87e59, 0x0e419c2, 0xf89956b, 0xdce6ed6, 0xccd8951, 0xf047c21,
          0xa83c991, 0x6ed4a1b, 0x2d28e0a, 0x85af86e }
    },
    {
        { 0x9ed48a8, 0x04433c4, 0x0bc375d, 0xeffa858, 0xfa6e3b5, 0xfb0e1b2,
          0xa1aadda, 0x51483a2, 0xf8b2ea8, 0x733448d, 0xf639f0c, 0xaa0513c,
          0xa23bf84, 0x6bc61a3, 0xdc2430d, 0x3e64f68 },
        { 0xc5876b1, 0x51bf502, 0x1c0dd2a, 0x6b83375, 0x342914f, 0xe597be1,
          0xf8e632c, 0x43d5ab0, 0xd62587b, 0x2696715, 0xed34f24, 0xe87d20a,
          0xe18baf7, 0x25b7e14, 0xe22e084, 0xf5eb753 }
    },
    {
        { 0x24d8295, 0x51da717, 0x18d1340, 0xd478e43, 0x2cf7f66, 0xacf94f4,
          0x3760711, 0x230d7d1, 0x5abc626, 0x078a66a, 0x6b5f6da, 0xd78b0bd,
          0x96d1d0b, 0x23a9713, 0x4bd960f, 0x87623d6 },
        { 0x77db53f, 0x0841a99, 0xf4d03ee, 0x23c1a53, 0x1f95df1, 0x2f62c2e,
          0x116f4e7, 0xd1e2ec1, 0x34811a9, 0x896d2fe, 0xec8096e, 0xad65e2b,
          0xb1744a6, 0x09d36f9, 0xff5ddf7, 0x564bac7 }
    },
    {
        { 0xc3f77cb, 0x48b41e2, 0x0968938, 0x5227673, 0xfd9b452, 0xff1b899,
          0x2e03908, 0x67cf3bf, 0x248a6fb, 0x3731d90, 0x256598f, 0xd800a05,
          0xbdc8530, 0x347d2f2, 0x7ad08a1, 0xc72a300 },
        { 0x1d65f73, 0x5e5be74, 0x4206ead, 0x183d4ae, 0xade4013, 0xcb50c1c,
          0x3102483, 0x39db43d, 0x70d6325, 0x0eb49fa, 0xc1f02b9, 0xa18f6a2,
          0xdbf5e66, 0x3e6fe30, 0x3a82aa5, 0xac4eeb9 }
    },
    {
        { 0x3613d47, 0x295affd, 0xb56f343, 0x7b7e68a, 0x92b173b, 0x9806296,
          0xbad35fb, 0x937061e, 0x5c21eea, 0x2501978, 0x787a746, 0xe92721b,
          0x3651631, 0x463c46c, 0xc6f2d5a, 0x6da4b5d },
        { 0x6e6d18c, 0xcb67cc1, 0x0010588, 0x1b30d52, 0xdb1d1e8, 0x1bb6ea6,
          0xad11474, 0x9c6308a, 0x3d19b1c, 0xc316741, 0xbe4fb79, 0xf2e84d7,
          0xe050f77, 0xeccb873, 0xcc2bf86, 0xf7c8d80 }
    },
    {
        { 0x7ab20e5, 0x16fe2e1, 0xecf3a92, 0x274dead, 0x0972f67, 0x9f43487,
          0x4605751, 0x9a65a45, 0xb8980b2, 0x9351f07, 0x0eb08a5, 0x412962b,
          0x733f440, 0xb8c9bfd, 0x1ca250f, 0xac2cd64 },
        { 0x2ba7d26, 0x68cdd0f, 0x4e0beea, 0xd3d2a4a, 0x9f4a258, 0x50135c1,
          0xf0d02e4, 0xb475e53, 0x589283a, 0x432d8c6, 0xa0a2b6c, 0x29141bf,
          0x13704bc, 0xd7379ec, 0x52459bf, 0x831562c }
    },
    {
        { 0xeeec506, 0x676b366, 0x45da557, 0xdd6cad5, 0x77057d2, 0x9de39cb,
          0xdf05bf1, 0x388c5fe, 0xdfb1f03, 0x6e55650, 0x52126c9, 0xdbceffa,
          0x3a4a220, 0xe4d187b, 0xeb27020, 0xac914f9 },
        { 0xd2e5f30, 0x3f4ab98, 0xdd94451, 0x6ae97da, 0x0d80981, 0x64af695,
          0xf2aa2ce, 0x36b4b90, 0x18fcf59, 0x6adcd7a, 0xc116c81, 0x3ddfe6d,
          0x549b9e3, 0x661072b, 0xec4584d, 0xd9e3134 }
    },
},
{
    {
        { 0xa1e400c, 0x6e46707, 0x551e806, 0xcdc990b, 0x3a07724, 0xfa51251,
          0x1b3e4f5, 0x500553f, 0xef4dac3, 0x67e8b58, 0x2cb4cc7, 0x958349f,
          0x7f9143c, 0x948b4ed, 0x2b7822b, 0xe646d09 },
        { 0x2bc3c26, 0xd185dd5, 0xc837fc9, 0x34ba16e, 0x5a788b7, 0x516d4ba,
          0x56142b0, 0x72f2de7, 0xf445b3d, 0x5846f61, 0xf4631a1, 0xdaec5c9,
          0x169ea9b, 0xa10b18d, 0xaf6751b, 0x85d2998 }
    },
    {
        { 0x43ddf31, 0xda0cac4, 0x1860911, 0x0966e17, 0x3cba600, 0x9c3a717,
          0x571f895, 0x5781880, 0x737ac21, 0x5e2a927, 0x6c253fb, 0x8a46148,
          0x95ee626, 0xe801cf5, 0x5f84fc0, 0x271166a },
        { 0xba856bd, 0x306937f, 0xbe80a43, 0x80cb179, 0xffb5980, 0x70393b2,
          0x660fc64, 0xa8e4a1c, 0xc0d5c98, 0x5078abf, 0xfbd31ff, 0x62ba530,
          0x9e51b88, 0xda60844, 0x355ae15, 0xdb6ecb0 }
    },
    {
        { 0x23c5d49, 0xbcbb6ea, 0x87959bc, 0x08906ba, 0x0991665, 0x61cc088,
          0xd90d13c, 0x21d6b41, 0xd03afe9, 0x0c27ac1, 0x5cfea52, 0x159995f,
          0xbdfe220, 0x4057e20, 0xcbdf058, 0xdd1b349 },
        { 0x2e37159, 0x0cd6626, 0x3eb0d17, 0x8cea8e4, 0x5bce7f0, 0x553af08,
          0x5b6511d, 0xb94cb5f, 0x50e0330, 0x7b8d3a5, 0x57ab7e7, 0x4159110,
          0x6aa886f, 0x320820e, 0xc5b6b81, 0x130d4d6 }
    },
    {
        { 0xc7bb2ed, 0x2f98059, 0xa49bdfb, 0x33ebf4c, 0xb0a675b, 0x04c72a1,
          0xadb6c14, 0x94f9ea4, 0xcf728c0, 0x03376d8, 0x4c6eb6a, 0x5c059d3,
          0xeb8da48, 0x0178408, 0x2956817, 0x8bf607b },
        { 0xceb3d28, 0x7ad2822, 0x37ae653, 0xd07a403, 0xc1e46b2, 0xbc68739,
          0x9154ba9, 0x15d7cca, 0xa26617d, 0x6b97103, 0xb2e0d28, 0xa610314,
          0xfd4d363, 0x52a08ba, 0xc7dc2af, 0x80c2638 }
    },
    {
        { 0x3187140, 0x0cde7ef, 0x4b70acd, 0x93b92ca, 0x7a79cdc, 0x5696e50,
          0x8eaab66, 0x73cc972, 0x8f1b0c7, 0x6b8c5b6, 0x4f7e0b1, 0xb39a318,
          0x376108a, 0x72cfb0d, 0x98536a7, 0x0c53efc },
        { 0x24c2f1e, 0x03b52a8, 0x6399b78, 0x717132e, 0x349a85d, 0x31ebd25,
          0x1a200d4, 0x265ee81, 0x407d7ad, 0x0b1aad2, 0x94d2962, 0x9a9ebc8,
          0x41171d9, 0x994e6cd, 0x6c8fa83, 0x09178d8 }
    },
    {
        { 0xa2593a1, 0x7d1d238, 0xb38fb19, 0x863e93a, 0xe7712a9, 0xd23a4cc,
          0x27efcd5, 0x7477b13, 0x1392f6c, 0x3ba69ff, 0xf7bb5a5, 0x63e0c32,
          0x026effd, 0x20412c0, 0xef424ab, 0xd3ee8e4 },
        { 0x64e5174, 0x14c0b2d, 0xe58c47b, 0x2a611f2, 0xc1e8635, 0xaa58a06,
          0xcf17034, 0x1870c3e, 0x83f1bf3, 0xb0d5e34, 0x16c7eb3, 0xb19905c,
          0x6efa4ca, 0xbf85d62, 0x180f92b, 0xfd16b2f }
    },
    {
        { 0x3adcb48, 0xc0431af, 0xba90496, 0xc9a7a8d, 0x3895294, 0xd765a16,
          0x551de70, 0xb02a41a, 0x749b8a1, 0xb71b261, 0xc6f3e47, 0x0dfa89e,
          0x0f5d9ce, 0x392c0d8, 0x31aee3c, 0x43c59d8 },
        { 0x4d76f49, 0x94bfb6d, 0x27d68a5, 0xe8f5b82, 0x630fd08, 0x78ae1d9,
          0xce1bdae, 0x1379029, 0x66715dc, 0x9689da0, 0xd3278c7, 0x5d4cb24,
          0x9e84fbc, 0x77c9833, 0xea1048c, 0xc8478dc }
    },
    {
        { 0x770d2ba, 0xe4b8f31, 0x42ea095, 0x744f652, 0x036f138, 0xd06e090,
          0x3b078ca, 0xd3a3d5b, 0x78b8417, 0xc7ae541, 0xc738fd7, 0xad6c5d4,
          0x4676454, 0x6178984, 0x5d9a392, 0xfbf3423 },
        { 0xfff772f, 0x8e451a7, 0x5ffbead, 0x8605bb7, 0x930d59f, 0x6f75cc1,
          0x8f3f460, 0xd4f4755, 0x6700c8a, 0xefd2d79, 0x2406421, 0xceb462a,
          0x9dfe8f1, 0x8ed0f97, 0xd1d7600, 0x0280bf1 }
    },
},
{
    {
        { 0xdd9a54d, 0x761c219, 0x86a39c0, 0x1127fcb, 0x4c9bedd, 0x7d0e4f0,
          0x4d976b6, 0x27c017a, 0xda042cf, 0x800c973, 0x2593f11, 0xe7419af,
          0xae67960, 0xbd49448, 0x744fd85, 0xd3b60b7 },
        { 0x61676fe, 0x5e74ed9, 0x39af627, 0x7383ef3, 0x5e62df7, 0x34407e0,
          0x8bf3196, 0xb053461, 0x583b407, 0xd6b7184, 0x55011be, 0xe3d0685,
          0x2124b52, 0x94083d0, 0xf780aaf, 0xa908324 }
    },
    {
        { 0x73ec9c3, 0xb27af1a, 0x70fa725, 0xb66ad9f, 0x8cf73e4, 0x07724f5,
          0x9949358, 0xc3fcd57, 0xda0cc01, 0x06efb79, 0x10597c9, 0x1e977d2,
          0x703e8d6, 0xcd732be, 0x6d0b69e, 0x6fd29bf },
        { 0x667128e, 0xca658ac, 0xc7872b3, 0xca0036a, 0x5355837, 0xc969858,
          0x075cf1c, 0x59f3be8, 0x3809a11, 0x9f1b9b0, 0x9733871, 0x6881ced,
          0xe902a5f, 0x8cda0fb, 0x4e3871e, 0x4d8c69b }
    },
    {
        { 0xddee82f, 0x5c3bd07, 0x2f9723b, 0xe52dd31, 0x74f1be8, 0xcf87611,
          0x35f8657, 0xd9ecbd8, 0xfbfea17, 0x4f77393, 0xd78fe2c, 0xec9579f,
          0x0fb0450, 0x320de92, 0x95d9c47, 0xbfc9b8d },
        { 0x5e1b4c3, 0x818bd42, 0x40e2c78, 0x0e0c41c, 0xbccb0d0, 0x0f7ce9a,
          0x5ef81fb, 0xc7e9fa4, 0x73574ad, 0x2561d6f, 0xd2efb0b, 0xa2d8d99,
          0xe96cd0a, 0xcf8f316, 0x4964807, 0x088f0f1 }
    },
    {
        { 0x45d5a19, 0x0a84989, 0x6c2131f, 0x47ab39c, 0xf3fc35d, 0x5c02824,
          0x9ee8127, 0x3be77c8, 0xc90b80a, 0xa8491b7, 0xa28aa93, 0x5397631,
          0x6c0b344, 0x54d6e81, 0x876d0e4, 0x22878be },
        { 0x6db3bf6, 0xeecb8a4, 0x54577a3, 0x340f295, 0x9a00f85, 0xa779868,
          0x4bb9147, 0x98465d7, 0xda3c736, 0x9532d7d, 0x7504b20, 0x6d574f1,
          0xd86e435, 0x6e356f4, 0x4533887, 0x70c2e8d }
    },
    {
        { 0xd293980, 0xdce5a0a, 0x069010e, 0x32d7210, 0x06deaaa, 0x64af59f,
          0x59239e4, 0xd6b43c4, 0x9199c29, 0x74bf255, 0x11e1e2b, 0x3efff41,
          0xcb0f8d8, 0x1aa7b5e, 0x989e395, 0x9baa22b },
        { 0x7b33ac1, 0xf78db80, 0x54ce80a, 0x05a3b43, 0x7bc8e12, 0x371defc,
          0x1224610, 0x63305a0, 0x6d697ef, 0x028b1ae, 0x1cd8051, 0x7aba39c,
          0x28ee4b4, 0x76ed7a9, 0x7f99901, 0x31bd02a }
    },
    {
        { 0xf075566, 0xf9dab7a, 0xf56f18b, 0x84e29a5, 0xf64e56d, 0x3a4c45a,
          0x6a7302d, 0xcf3644a, 0x156b658, 0xfb40808, 0xf96be52, 0xf33ef9c,
          0xcaa2f08, 0xfe92038, 0xb261894, 0xcfaf2e3 },
        { 0x224ce3f, 0xf2a0dbc, 0x592eb27, 0xed05009, 0x95889d0, 0x501743f,
          0x77c95c2, 0xa88a478, 0xdd63da9, 0x86755fb, 0xc7ee828, 0x9024acf,
          0xf38113b, 0x634b020, 0x6056e64, 0x3c5aacc }
    },
    {
        { 0xa2ef760, 0xe03ff3a, 0xb1c3bac, 0x3b95767, 0x940d754, 0x51ce6aa,
          0x47a9a3d, 0x7cbac3f, 0x34f8d1a, 0xa864ac4, 0x80dbd47, 0x1eff3f2,
          0x7ebd5ca, 0xd8ab660, 0x05b07ed, 0xc4df5c4 },
        { 0xa4f095b, 0x3dc92df, 0x7cdbd9a, 0x5ae36a5, 0x7891e04, 0x7ff2973,
          0x0a5fe7b, 0x37c0313, 0xaa6e35e, 0x210d7b0, 0xbf200d8, 0x6edfb53,
          0x84afb85, 0x787b68d, 0x72c6de3, 0x9b5c49b }
    },
    {
        { 0x4010f4e, 0x5185716, 0x0536ebe, 0xe0b144b, 0x887d663, 0xacabb14,
          0xedf584f, 0xac1caed, 0xaf175a3, 0xb43fb8f, 0xf992a3c, 0x310b6d5,
          0x85178a4, 0xf2c4aa2, 0x8bd56bf, 0x69c9969 },
        { 0xa4d972e, 0x73d6372, 0x9583803, 0x3d5bb2e, 0xd891581, 0x7bf7d18,
          0x568a34a, 0xa5ce5d7, 0x1f45c81, 0x670b433, 0x1f96910, 0x97265a7,
          0xb07c1ea, 0xdb14eb3, 0xfed447c, 0xdf008ea }
    },
},
{
    {
        { 0x00c2f10, 0x0379f5a, 0xd350285, 0xb320b4f, 0x8efdd7d, 0x74e560e,
          0xf46a140, 0xf2f017e, 0x0f34624, 0x2ced1a6, 0xca08ec9, 0x7c4b4e3,
          0x5d8bc6b, 0xdffc2a1, 0x527b007, 0xcc8f3f3 },
        { 0x861fe83, 0x59f8ac4, 0xd03144c, 0x8d48d2c, 0xbfa6dce, 0xa8457d2,
          0x677c136, 0xd7ed333, 0xc228e18, 0xcb8e219, 0x16ab1e4, 0x5f70bc9,
          0x3780370, 0x2ae3a3d, 0x88f17ad, 0x9f33654 }
    },
    {
        { 0x960e4bb, 0xeab0710, 0xab9cfd3, 0xc668a78, 0xb0ef946, 0x2e85553,
          0x8df5df3, 0xa43c4b9, 0x3cb3646, 0x0ecd559, 0x18dbe71, 0x6f543c4,
          0xf59818b, 0xee7edaa, 0x90911c1, 0xc44e8d2 },
        { 0x269b509, 0xafb38b1, 0x52afe2c, 0x9e2737c, 0xccfa664, 0x5b2ef02,
          0xe1cc58b, 0x1e0aeac, 0x5ea134e, 0x37a57e9, 0x83b9fc2, 0xc9c465a,
          0x6e3ecca, 0x4b9e8c7, 0x9bdbab5, 0xca07dbe }
    },
    {
        { 0xb0d7807, 0xd297f3c, 0xf59ce61, 0xee441a5, 0xb2db844, 0x728553b,
          0x640e9e0, 0x90f87e5, 0xcb76dff, 0xaa72cbf, 0x4012d57, 0x065c686,
          0x9678b44, 0xd5ee88f, 0x2177603, 0x3d74b85 },
        { 0x748b68e, 0x3f9c947, 0x8f44d44, 0x03856d9, 0x462426c, 0xde34b84,
          0x845ab29, 0xc16d1bb, 0xd2e18de, 0x9df6217, 0xb154643, 0xec6d219,
          0x2ee0f8f, 0x22a8ec3, 0x91c5175, 0x632ad38 }
    },
    {
        { 0x6869267, 0x19d9d23, 0xfe5532a, 0x628df94, 0x6dc9a01, 0x458d76c,
          0x2cc39c8, 0x405fe6c, 0xf3a04ba, 0x7dddc67, 0x12500c7, 0xfee6303,
          0xa50e9de, 0x580b6f0, 0x6090604, 0xfb5918a },
        { 0x3af6b2d, 0xd715925, 0x1c7d1ec, 0x83d62d6, 0x85858c4, 0x94398c1,
          0x14bfb64, 0x94643dc, 0xaf7db80, 0x758fa38, 0xa8a1557, 0xe2d7d93,
          0x3562af1, 0xa569e85, 0x84346aa, 0xd226bdd }
    },
    {
        { 0xd0ccd20, 0xc2d0a5e, 0x5dbc0cf, 0xeb9adb8, 0x26d7e88, 0xe0a29ee,
          0x84a8e98, 0x8bb39f8, 0x37396ea, 0x511f1c1, 0xc8b2fb3, 0xbc9ec5a,
          0x090e5bc, 0x299d81c, 0x4cdd587, 0xe1dfe34 },
        { 0x5e465b7, 0x80f61f4, 0x1bad59e, 0x5699c53, 0xb79ff92, 0x85e92e4,
          0x9db244c, 0x1e64fce, 0xa22097d, 0x3748574, 0xefff24e, 0xe2aa6b9,
          0x0a10bc6, 0xb951be7, 0x9067a1c, 0x6685326 }
    },
    {
        { 0xa6114d3, 0xf716ddf, 0x037ec1f, 0x9e515f5, 0x44944a6, 0x7734541,
          0xaba97cc, 0x1540c4c, 0x8b54bb7, 0xe41e548, 0xcae37bc, 0x4363156,
          0xf3d2ce8, 0xc384eaf, 0x4c58ba4, 0x72a4f45 },
        { 0xdcaf3fc, 0x0ceb530, 0x78dcdbb, 0x72d5365, 0xc6320fa, 0x9b44084,
          0xeb74c70, 0x6262d34, 0x608e6dc, 0x8abac85, 0x10dd38d, 0x82a5264,
          0xa819b8d, 0xbc39911, 0x03ad0d9, 0xbda15fe }
    },
    {
        { 0xf9dc60b, 0xadbf587, 0x7d846d2, 0xf9d814f, 0xb77bde0, 0xccdd241,
          0x2242f50, 0x89cb6d7, 0xe6360a8, 0x95c0e3e, 0xdf49713, 0x7c7dd5a,
          0x57d5814, 0x68e0e49, 0x0c16571, 0x3aa097d },
        { 0x267d03a, 0xb56b672, 0x8c44af4, 0x4f55708, 0xf3252a5, 0x67c49e7,
          0xc94a469, 0x871d6cf, 0x01fbfaa, 0x57ae998, 0x48a5d8e, 0x5c0e48f,
          0x5e240b9, 0xe9bf9c8, 0x99d41ca, 0xa410189 }
    },
    {
        { 0xb2889b4, 0x6beb0c7, 0x9455370, 0x78b7f89, 0x47ca364, 0xd434214,
          0x9f21e5b, 0xdd9d2da, 0x0a7e4aa, 0xa0c7c18, 0xda1660c, 0x022c0d4,
          0x5a57002, 0xe1f5c16, 0x518f68f, 0x51c7c9e },
        { 0x2586502, 0x6d521b6, 0x183ec1b, 0xa0f2cb3, 0xcaa5e16, 0x578b4e0,
          0x764997f, 0x7bd4fbd, 0x64b1804, 0x7ec56c3, 0x0ee08e4, 0xb75a254,
          0xdc19080, 0x6bf74a6, 0x97d6e59, 0x6ec793d }
    },
},
{
    {
        { 0x0a4beb9, 0x16789d6, 0x9b9c801, 0x512b2cd, 0x8c7bb9c, 0xf8b6d10,
          0x9ebdc8c, 0xd85651e, 0x9ba971a, 0xc945082, 0x7e1cf78, 0x852d9ea,
          0x0af01e2, 0x6a45e35, 0x6151dcf, 0xe6cdadf },
        { 0x2b8c01b, 0xc454bb4, 0x3d54cd2, 0x59e0c49, 0x454d608, 0x8e1e686,
          0xd8c6103, 0x0dbae4b, 0x6c18b18, 0xa5603a1, 0x3369093, 0x227a6b2,
          0x5f3de1c, 0xf1e8929, 0x8ab63c5, 0x42f0b58 }
    },
    {
        { 0x5b596d8, 0xf1974cc, 0x44719f0, 0xee8093f, 0xf6f5b54, 0x40ba933,
          0x2f3d654, 0xd6e5365, 0x26d73b8, 0x9aeb835, 0x0776382, 0x50ed535,
          0xad43875, 0x3be47d6, 0xc786e48, 0x21d56df },
        { 0xb73bb39, 0x8a75e18, 0xf265a78, 0x9eba84c, 0x2e772e7, 0x7c02a4d,
          0x4c1ecd2, 0xf7df6d4, 0x6cef71b, 0xa8d9ea0, 0xcae3b68, 0x86e8f91,
          0x99efefa, 0x2fd1411, 0x214e6f6, 0x0b36ab2 }
    },
    {
        { 0xbdce61c, 0xd79065c, 0xdecb229, 0xcb562ff, 0x4600849, 0xef5d3d1,
          0x1d23ac8, 0x348b31b, 0x15c36b8, 0xb2ea699, 0x4822836, 0x268683d,
          0xc6f0b7d, 0x083edbe, 0x1a7821c, 0xaf4f39d },
        { 0x4e64841, 0x23be6e8, 0x65bf791, 0xe9e2463, 0x02bfd7c, 0xa3208ac,
          0xd01357d, 0x231989c, 0x6422ab4, 0x79b8aad, 0x91b8564, 0x57d2b7e,
          0x8c04421, 0x28ebbcc, 0x7d09c05, 0xdc787d8 }
    },
    {
        { 0x6c7bed5, 0xeb99f62, 0x39cd0e8, 0x326b15f, 0xd860615, 0xd9d53dc,
          0x1bf4205, 0xdf636e7, 0x0752209, 0x1eaa0bf, 0x4744abb, 0x17ce69a,
          0xf3ea2fb, 0x474572d, 0x224a7f3, 0xc4f6f73 },
        { 0x63081b4, 0x7ed86ad, 0x4a20afb, 0xcd4cdc7, 0xb301b2e, 0x7563831,
          0xe038699, 0x5b4d2b1, 0x802a15f, 0xa15d1fa, 0x13e9172, 0x6687aaf,
          0xba6da90, 0x3eccd36, 0x7474e83, 0x34e829d }
    },
    {
        { 0x19c9b27, 0x4cea19b, 0x5f52523, 0xa14c37a, 0x726625c, 0x248b16d,
          0x6cabc21, 0x8c40f9f, 0x32a5c65, 0x918470c, 0x2a98d5b, 0x314056b,
          0x34a0714, 0x6c974cf, 0x4f6314a, 0x0c8f8a9 },
        { 0x70bccfd, 0x4844557, 0x740c9fd, 0xf5835db, 0xa21407c, 0x12e59b5,
          0xdb1689d, 0xbe338e0, 0xdd5e915, 0x5a50ce9, 0xef99f39, 0xb1780e9,
          0xee4d833, 0x1262b55, 0x89c5340, 0x4be3f22 }
    },
    {
        { 0x6c4b858, 0xbb99b90, 0x550ca53, 0xa7724d1, 0x826962e, 0x7d31f5a,
          0xa5804da, 0xf239322, 0x0275048, 0x3e11320, 0x3ee4cb6, 0xcbb1bb8,
          0x1331191, 0xdb86525, 0x7d1d903, 0xb7caf9e },
        { 0x77d7a9d, 0x06e3b05, 0xb3bbbf5, 0x7a132b0, 0x7c50575, 0xd61fbc5,
          0xaf4b646, 0x393f712, 0xcb7efe9, 0xef77972, 0x5ea4995, 0x20e6d5d,
          0xfbbe4c6, 0x0ac23d4, 0xc807f2a, 0x8456617 }
    },
    {
        { 0x5396143, 0x4995fb3, 0xb99dc46, 0xa8b4bd1, 0x4150064, 0x2293e8e,
          0x22a3545, 0x2f77d49, 0xb2192c4, 0xe866b03, 0x5e0aa38, 0x58b01f0,
          0x2ed246b, 0xe406b23, 0xed60974, 0x447edb3 },
        { 0x8869703, 0xf541b33, 0x383420a, 0x6959fe0, 0x4be4e48, 0xd6b39db,
          0xb5714ef, 0x048f3b4, 0x5d9e4b8, 0x68b4968, 0x2177963, 0xbda8e6c,
          0xc4211fe, 0x5094e35, 0x2d46d1a, 0xea591c3 }
    },
    {
        { 0x2fef780, 0x3a768ff, 0x32970c6, 0x4218d28, 0xec6da17, 0xce598e4,
          0xfbb126a, 0xf675645, 0x0427617, 0xb04c23f, 0xe4fce74, 0xc9f93fb,
          0x3c91b00, 0x44a414b, 0x1d3b3cc, 0x4d982f3 },
        { 0xb24cce0, 0xb1d40e8, 0x133e73d, 0x5a21c07, 0x0bb589d, 0x6e9358e,
          0x2399844, 0x39cfb17, 0x166080e, 0x83f7647, 0x450b468, 0xcfe7bf8,
          0x1e8434f, 0x2a288f7, 0x21a81e3, 0xd39f1e5 }
    },
},
{
    {
        { 0x528af6f, 0x78c6f13, 0x94b74d9, 0x0001fe2, 0x01aab44, 0xae77425,
          0xef0039c, 0x7cbe937, 0x0fa2a67, 0xaf3e4f0, 0xda1378e, 0xe28175f,
          0x8ccd90e, 0x72adeed, 0x00af22f, 0x16a8ce1 },
        { 0xcbf63dd, 0x69fae17, 0x9e39e26, 0x6786172, 0xf827a18, 0xe92b3d5,
          0x8403682, 0x4d75e41, 0x9056a79, 0x01a4fd9, 0x20008f5, 0x89efb2d,
          0xb78ff15, 0xa2f6918, 0xa3437f5, 0xf41c870 }
    },
    {
        { 0x7be353c, 0xc840ae5, 0x3fb2691, 0x465a5eb, 0x7eba833, 0x34a89f0,
          0x013346e, 0xf620896, 0xe875df2, 0x563b5f0, 0xfbc44ce, 0x5f7fc8b,
          0xcfedf9d, 0x22fcb5a, 0x7dc691b, 0x7cf68d4 },
        { 0x76a103f, 0x37f7c2d, 0xfd87b7d, 0x728a128, 0xccf2132, 0x7db2ad8,
          0xb100e63, 0xa4c13fe, 0x7b511d5, 0xcd28a51, 0x721ca5c, 0xb910280,
          0xd84bd52, 0xec1305f, 0x2729791, 0xb964642 }
    },
    {
        { 0x5bc7462, 0x83fccdf, 0xd6f012f, 0x01f3dda, 0x3a6a87c, 0x57f1171,
          0xff403ac, 0xedb47ce, 0xbaab073, 0x6c184e5, 0x6f0d6a1, 0x5b17c7d,
          0x3ef2c91, 0x45a4c4f, 0x86a8f41, 0x26c3f7e },
        { 0xb646514, 0x81a6db0, 0xca8b9ae, 0xf84059f, 0x9f02305, 0xd73dab6,
          0xc4b7c6c, 0x0de3fae, 0x696df2f, 0x18abb88, 0x75d7740, 0x45dd1b9,
          0x9ee35bc, 0x3aeccc6, 0xb029f88, 0x478252e }
    },
    {
        { 0x8b2ce15, 0x66bf85b, 0x335709d, 0x1175425, 0x8123874, 0x00169ef,
          0x9b89868, 0xfd3c18c, 0x775204e, 0xb3612f9, 0xc2cd510, 0x4b8d09d,
          0x14559ad, 0xafa12e6, 0x9657493, 0x1ddaa88 },
        { 0x1e77a08, 0x87d700b, 0x14d2e71, 0xaf4cf2f, 0xbf90c94, 0xe00835d,
          0x6dc8429, 0xb16a6ec, 0xf8a4d92, 0x02a7210, 0x3d0c48d, 0x5a5ab40,
          0xb5b9bea, 0x0052b3a, 0xe138f89, 0x6242739 }
    },
    {
        { 0x16b2819, 0x7c215d3, 0xfeb9d7a, 0xdacb65e, 0xd833423, 0xc3c569e,
          0x886a058, 0xbc08435, 0x7e5cb61, 0x132c4db, 0x9422aff, 0x6373a27,
          0xfca9fc4, 0x43b9d7e, 0xdbe465f, 0xe3319a5 },
        { 0x0b39da7, 0x51d3687, 0x4b75492, 0xcb6d798, 0xeadd87a, 0x77eb272,
          0xe0d3f6c, 0xf2fb47d, 0xf9f791c, 0x807fd86, 0x975e885, 0xf01086b,
          0xb6a3604, 0xf9314b5, 0x67be852, 0x8cd4538 }
    },
    {
        { 0x858f79b, 0x7c1e6b3, 0x938caf9, 0xf0477c4, 0x3e88c44, 0xb311bbf,
          0x1e3a3c1, 0x9234c09, 0x95a1d4d, 0x531af2b, 0xb8d1c64, 0xf3cc969,
          0xb51e78d, 0x6f3c328, 0x34e8881, 0x5a1bd6c },
        { 0x3a9336f, 0x2e31239, 0x5ced897, 0x020f0cc, 0x5fab121, 0x4b45d7b,
          0x1841210, 0x8068b1c, 0x8349170, 0x1bd85fc, 0x0f97fe5, 0xfe816d8,
          0x14b84fc, 0x1089818, 0xb93cd48, 0x1d4fabb }
    },
    {
        { 0xaef599e, 0x1f11d45, 0xb09c58a, 0x8d91243, 0xd08c3c3, 0xd2eec7b,
          0x3b02793, 0x5a6039b, 0x8fb2c00, 0xb27fed5, 0xe8acf5e, 0xb5de44d,
          0x6e6c698, 0x2c3e0cd, 0x777180d, 0x2f96ed4 },
        { 0x96d0e36, 0x67de8bf, 0xc9b6d65, 0xd36a2b6, 0x637d59c, 0x8df5d37,
          0xc8d9878, 0x951899f, 0xb13fcf8, 0x0fa090d, 0x1f5c7b4, 0xa527081,
          0x513a37a, 0x56a6560, 0x14dc1fe, 0xc6f5530 }
    },
    {
        { 0x94945d6, 0x7f6def7, 0x8cc8832, 0x2f52fe3, 0xa812ff5, 0x0228ad9,
          0xbb8478a, 0xcd282e5, 0xbe91b07, 0xa0bc9af, 0x11165e2, 0x0360cdc,
          0x7b857e4, 0xb5240fd, 0xfa36b08, 0x67f1665 },
        { 0xad2c93f, 0x84ce588, 0xe8ff4c0, 0x94db722, 0x489c8a3, 0xad2edbb,
          0x7e5f278, 0x6b2d5b8, 0xd1d0798, 0x0265e58, 0x4c5589e, 0xd2c9f26,
          0x4e4074d, 0xde81f09, 0x303089f, 0xc539595 }
    },
},
{
    {
        { 0x83e882c, 0x183492f, 0xb5e6c12, 0x4d58203, 0xefec20b, 0x1ac96c3,
          0xe1cd15e, 0xabd5a5b, 0xcbbb14b, 0x7e1e242, 0xd0543b3, 0x9f03f45,
          0xd678158, 0xc94bc47, 0xa446cad, 0x7917be0 },
        { 0x9b37394, 0x53f2be2, 0x064cc76, 0x0cb0a6c, 0xfba3da3, 0x3a857bc,
          0x80fcb49, 0xac86bc5, 0x30ab146, 0x9d5336e, 0x5bc1270, 0xafb093d,
          0xe5c3b6e, 0x996689d, 0xea076ba, 0x55189fa }
    },
    {
        { 0x646ce03, 0x99ef986, 0x30e6100, 0xa155f81, 0x29b6b07, 0x75bef17,
          0x1de077b, 0xc46f08e, 0x7ed0526, 0xf52fdc5, 0x61a299a, 0xe09d989,
          0x7b8e93a, 0x9527329, 0x0acd185, 0x11255b5 },
        { 0x4a6acdd, 0x57919db, 0x4451d74, 0x708a578, 0x283f7b3, 0x5b0bd01,
          0xc3d9260, 0xe82f40c, 0x82bbdc2, 0x2ab96ec, 0xc164d87, 0x921f680,
          0xc17a6a9, 0xf0f7883, 0x382a001, 0xc366478 }
    },
    {
        { 0x2e40791, 0x5c9aa07, 0xa0776bf, 0xf0b72d6, 0xeaa50dc, 0x445f9b2,
          0x6bda47f, 0xa929fa9, 0x3bbfc49, 0x539dc71, 0x006a78b, 0x4f16dd0,
          0xeef39c7, 0x331ba3d, 0xc34157c, 0xbfa0a24 },
        { 0x6a3b482, 0x0220beb, 0x6c43885, 0x3164d4d, 0xacdea23, 0xa03bb5d,
          0x9d8f450, 0xd6b8b5a, 0xbd208fe, 0xd218e65, 0x35c476f, 0x43948ed,
          0x0a2ed2b, 0x29a0dd8, 0x25295b7, 0xa6ccf33 }
    },
    {
        { 0xac38939, 0xf68f15f, 0xf8010c1, 0xb3dd5a2, 0xa35f141, 0xf7ac290,
          0x7388574, 0xdc8f3b2, 0xe95fed2, 0x7ec3de1, 0x257ac7d, 0xc625451,
          0x664e55a, 0x66fc33e, 0x4832ba5, 0xd3968d3 },
        { 0xc026448, 0x980291b, 0x24da4a5, 0xfcb2125, 0x827a360, 0xbca7df4,
          0x85ca63b, 0xfcc395c, 0x8e9f733, 0xcf566ec, 0xd465f70, 0x835ee9b,
          0x372f916, 0xe66d111, 0x04d9211, 0xc066cf9 }
    },
    {
        { 0x8b48818, 0xb9763a3, 0x4288f96, 0xa6d23cc, 0xed3a229, 0xe27fcf5,
          0xabaff00, 0x6aebf9c, 0x8131cd1, 0xf337503, 0xffabd58, 0x13ad41d,
          0x861c83b, 0x1bee6af, 0x9c142e7, 0x274fe96 },
        { 0x9b84b5b, 0x70ebcc9, 0x8191cfc, 0xe1a57d7, 0xcbf00b8, 0x46ccd06,
          0xefe402d, 0xc233e8e, 0xbeebeb3, 0xb4ab215, 0xbd14e7b, 0xb7424ea,
          0xa679578, 0x351259a, 0x471d684, 0x6d6d01e }
    },
    {
        { 0x815ae38, 0x755c465, 0x611db56, 0xadc3e85, 0x188dd50, 0x633999b,
          0xc12d907, 0xfdf7509, 0x238b6af, 0x25bcfde, 0x397f5e7, 0x50d705d,
          0x944c974, 0xb65f60b, 0x27ac325, 0x8867fc3 },
        { 0x3763eff, 0x2edc441, 0x341fb63, 0x892c0b3, 0xb3a7f28, 0xb34b83a,
          0x15c2f18, 0x9aa106d, 0x1bb2277, 0x720bbc6, 0x5cfaefd, 0x637f72a,
          0xf43e565, 0xf57db6e, 0xb58e772, 0xceb7c67 }
    },
    {
        { 0x6ecc1de, 0x2793da5, 0x38f31b2, 0x4e10974, 0x8781267, 0x4229b4f,
          0xdec04a1, 0xe5d2272, 0xec17cff, 0x6abb463, 0x0cbb048, 0x28aaa7e,
          0xd22ef85, 0x41dc081, 0x5e63d0f, 0xcbc361e },
        { 0xad5dbaa, 0xb78aafc, 0xfc1edc3, 0x0111505, 0x92c7bfa, 0x63ed66d,
          0xe468919, 0x2982284, 0xb8c0d8c, 0x30f1f21, 0x2685093, 0xf056747,
          0xf03dd0f, 0x0e085b6, 0x5581e66, 0xa8c8db8 }
    },
    {
        { 0x264ad0c, 0x42009a6, 0x593bef4, 0x13bf2b8, 0x5d4e8b1, 0x1d11190,
          0xef7bddc, 0xfe3e940, 0x624e62c, 0xa012275, 0x1d6d3cc, 0xcb65924,
          0xedb7ab6, 0xc7bcc70, 0xb750b1c, 0xff9fafb },
        { 0x7fea84b, 0xf65df29, 0x90b0e02, 0x17c84a8, 0x301e821, 0xa92a859,
          0xfb480d1, 0xbee8cb2, 0x59c604e, 0x7010b8c, 0xe803c43, 0x47bf3f4,
          0x47b3fff, 0xd645142, 0x9f0da13, 0xc4c5dcb }
    },
},
{
    {
        { 0xb5253b3, 0x8af700c, 0x206957a, 0x31ca605, 0x3eafdcd, 0x2574439,
          0xd3ae15e, 0x2ba5ae1, 0x5b82579, 0x710b738, 0x112b95a, 0x145ab57,
          0x38c55c5, 0x4b133a0, 0x2a16fef, 0xf7559c9 },
        { 0xd9ba896, 0x70c3e68, 0xc33d07a, 0x475dd32, 0x3a41e40, 0xe084e47,
          0xfd2e706, 0xddc9382, 0x79510bd, 0x34b7275, 0xa5f901e, 0x5e78a69,
          0xdcfb823, 0x429dfd7, 0x014f0a3, 0x1d9dc18 }
    },
    {
        { 0xaf403d7, 0x364fcdf, 0xb7d7b34, 0xd9ea4ff, 0xcbb1dac, 0x21a3426,
          0x143b4f5, 0xfa51052, 0x6df2409, 0x2bca073, 0x8ad7285, 0x7e6985a,
          0x4aaa27f, 0x3a1a9d0, 0x9fc0c6c, 0x1a815e1 },
        { 0xbb65bb3, 0xfab6147, 0x33ced0b, 0xa36dc0d, 0x2062d78, 0x26a8859,
          0x28a5fb7, 0x3438617, 0x4ebb1ad, 0xe82da25, 0xd05aa11, 0x70f5071,
          0xadaac48, 0x0b7f847, 0x93cb269, 0xeb812bc }
    },
    {
        { 0xf7caccc, 0xcb317cc, 0xcf85098, 0xd3410d9, 0x7f078d7, 0xca68c8d,
          0xb782efc, 0xfe9e812, 0x5f544b5, 0x32e7c0f, 0x3a7b7f2, 0x44fe95a,
          0xe91327b, 0xf4f1543, 0x76645ed, 0x27d118d },
        { 0xd7abc2c, 0x690547c, 0xb53c8af, 0xf64680f, 0x79ea989, 0xbe0cbe0,
          0xa91af28, 0x6cf0cce, 0x9daa2f9, 0xa3b85a2, 0x91faed0, 0xd4b663c,
          0xa8b20ba, 0x782c7b7, 0xb8d98ce, 0xf494faf }
    },
    {
        { 0x002f55a, 0x080c0d7, 0x2d6d9dd, 0xf4f8f14, 0x382f025, 0xb326229,
          0xad28c20, 0x58fd0b5, 0x8d06a15, 0x704b992, 0x7fbd8e4, 0xf4545d9,
          0xed55581, 0xc32fa63, 0x01ac0fd, 0x3ab7936 },
        { 0x6099fd1, 0x13ece52, 0x9c79178, 0x776dba8, 0xce26c45, 0x8d28212,
          0x60d739c, 0x09fddaf, 0xa84826e, 0xf9931ed, 0xb29439e, 0x6e73d90,
          0x9095e61, 0x94cfefc, 0x802f474, 0x3050d16 }
    },
    {
        { 0x9f6394b, 0x0898f8f, 0x88b0e91, 0x48b8cea, 0x4c1b362, 0x4bc9925,
          0x827d9ec, 0xe3fccb4, 0xd950d6a, 0x5d4cf9a, 0x39b5b38, 0xa16f1ef,
          0x620f288, 0x3c76d1d, 0xe119390, 0x9fdd059 },
        { 0xfb5edf8, 0x7b5de9e, 0x769d14e, 0x3e290b9, 0x6bd10b5, 0x4df3a91,
          0x82f8f7b, 0xae99bca, 0xc9524af, 0x5481d5d, 0x69504f1, 0xf112e4f,
          0x51931ec, 0xb048f09, 0x18f51b1, 0xbff876a }
    },
    {
        { 0x46c1c37, 0x932e2a7, 0x9aea4c1, 0x903ad52, 0x8f161f2, 0x717ac91,
          0xf425e2a, 0xa57d197, 0x7f39e0e, 0xae89dac, 0xbaa2a58, 0x91655c0,
          0x54836dd, 0xe3dc286, 0xa9ec9e6, 0xb5f0baa },
        { 0xbdbda04, 0xf7c4662, 0x51059c0, 0xbe5393b, 0xdd95b0f, 0xb16d552,
          0x1b3bd96, 0xde495b3, 0xc0206c5, 0xb2a6e02, 0x014d3a9, 0x045cc09,
          0x2a2f490, 0xf66a315, 0xc5dea05, 0x208c108 }
    },
    {
        { 0x65237ea, 0x6e38b68, 0x9f27fc6, 0x93a1303, 0xa95068a, 0x9a6d510,
          0xe7c9e54, 0x6fbf216, 0x571ac1d, 0x7824290, 0x91c2a0c, 0x8cb23ba,
          0xc7e434d, 0x611202e, 0x76058b4, 0x8f901bf },
        { 0x0849588, 0xef0ac05, 0xdd31804, 0xe0d2dde, 0xeb2ca81, 0xaf5417c,
          0x5d1a509, 0x420ac06, 0x9683bb6, 0x46e345e, 0xf613f7f, 0x6daf635,
          0x48a9576, 0xc9e8291, 0x176d147, 0x5f9f1d1 }
    },
    {
        { 0x77e9709, 0xd24ae1d, 0x0047b8a, 0x77751dc, 0xc6a1593, 0xe325334,
          0x671f86a, 0x9baf962, 0xc29a15e, 0x425af6a, 0x2796e33, 0x3108600,
          0xfc253a5, 0xb6ea78c, 0xafae0ea, 0x4c733e0 },
        { 0x97c99b9, 0x4b7443a, 0x50203a6, 0xc14e9e4, 0x52680ba, 0xd1bb515,
          0xd55533a, 0xa56a3ef, 0x169e1a0, 0xa66e38c, 0xeed7da0, 0xb3e4df9,
          0xddce3d9, 0x022c937, 0xf6e36b4, 0x8552089 }
    },
},
{
    {
        { 0xf5cc82e, 0x8e4bf95, 0xc3ed6c9, 0x2ad80c3, 0xc9045e1, 0xf2e5b2c,
          0x59b06d4, 0x42c9065, 0x7b43b84, 0xc1f7379, 0x72d7992, 0x1710dbf,
          0x767b41c, 0xe98cf47, 0x7bfb9e9, 0xe713fce },
        { 0x9fa5134, 0x9f54ae9, 0xde40d0e, 0x3002fd8, 0x9311334, 0xdc282b7,
          0xbfeb360, 0x5519810, 0x0f96ffe, 0x31539c7, 0xd27777b, 0x04eacc0,
          0x8ff5053, 0x5982410, 0x32b67ad, 0x5982366 }
    },
    {
        { 0x6bea5c2, 0x6eb4554, 0xd509a33, 0x82cfae0, 0x394bb59, 0x6a69bd8,
          0x5770ee1, 0x1880d8d, 0x7dacf9e, 0x6351844, 0xf02b891, 0x5b1ecc5,
          0xb6c9a5a, 0xeb7d900, 0x8897da8, 0xdab8a76 },
        { 0x98851a6, 0x28c7be5, 0x4d73c3b, 0x0101d4f, 0x5084996, 0x3c2569c,
          0x280bde0, 0xb9bc911, 0xcd0d4f9, 0x513a22a, 0x2a15f3b, 0xdf2986d,
          0x2aa4943, 0x231c28f, 0x0333870, 0x29623ad }
    },
    {
        { 0x4084416, 0x2ceb178, 0x49516cd, 0x924cf1c, 0x4be856f, 0x76536c0,
          0x47a265b, 0x11b59cd, 0x4999494, 0x720dc84, 0x007b795, 0x910f794,
          0x2d3df83, 0x8434e14, 0xbd478d3, 0x8f53878 },
        { 0xaeb9c2f, 0xd9b072e, 0xfd8a29f, 0x16f87ea, 0x2fd0de1, 0x8c42f9b,
          0x0e816ef, 0x916721e, 0x18bde37, 0x2ecb470, 0x2375da2, 0xcde3b7a,
          0xef94281, 0x30d0657, 0x5cd7af8, 0x5105456 }
    },
    {
        { 0x4bdced3, 0x7230b33, 0x0838569, 0x0c6a3e1, 0xe3493b8, 0xf19c9ec,
          0x0d97c57, 0xf275927, 0x0c862eb, 0xf14181e, 0x32c72bc, 0xfd3bac1,
          0xf3be362, 0x620563f, 0x47283b7, 0x672ccaf },
        { 0x2b7bf16, 0x191e3fa, 0x520dad7, 0xf838633, 0x3629d87, 0xd3dde55,
          0xaf86ebe, 0x14d8836, 0x221b2ce, 0x3db7dfb, 0x0aed72a, 0x3872abb,
          0x8c665b7, 0xb60de52, 0x44982cb, 0x89c2596 }
    },
    {
        { 0x4dbba25, 0x799a2de, 0xa42715e, 0xd818aae, 0xf55c362, 0xbc88f4d,
          0x713c9ae, 0x142a163, 0xfbfb33f, 0x411e8ee, 0x6bb684a, 0x34b4629,
          0xdc81817, 0x4344bec, 0x17f9d46, 0xcc9573d },
        { 0xff38a7d, 0xf85f8bc, 0x0caf117, 0xa14bf73, 0x4ba6429, 0x126874f,
          0xaa5db97, 0xcc9bf22, 0x6aba827, 0x62b56df, 0x9c9772a, 0xfee1cb8,
          0x177e541, 0xe36838f, 0xadd438f, 0x698815d }
    },
    {
        { 0x38ed1ad, 0xc9fd894, 0x7b6a601, 0x73cd79d, 0x05e8d20, 0x2210e62,
          0x3592af5, 0x72384ac, 0x763d07e, 0x5ccc079, 0xa5f79eb, 0x2f31a4a,
          0x2945a95, 0x693f4ed, 0x8056fdc, 0xc712017 },
        { 0xdf4b09a, 0x361ecd2, 0xb7d929a, 0xa5644ea, 0x3fabe9a, 0x34abc0b,
          0xe942a8c, 0x1a2473c, 0x6454bc3, 0xe00c924, 0xdff7366, 0xab324bc,
          0x21b8f99, 0xe1412f1, 0xe33551e, 0x970b572 }
    },
    {
        { 0xbd0a6b5, 0x6ca4cac, 0x921d654, 0x5584787, 0xc809bda, 0x18e5253,
          0xf0cbe5e, 0x01b32c3, 0x0f987dd, 0xb9aa754, 0x6dfa4db, 0x628f4bb,
          0x891890b, 0x0255f0b, 0x874e590, 0x25b7df4 },
        { 0x8ed5f95, 0xbded318, 0xca93023, 0x9dc428d, 0xbccf520, 0xc68f25a,
          0xe616e6c, 0xc4f3764, 0xa1d9993, 0xd9a57f1, 0x533431b, 0xd1964a5,
          0x02ab6d0, 0x06cd77f, 0x03e52e0, 0xa660791 }
    },
    {
        { 0x5f72700, 0xab08864, 0x0a1a44e, 0xf77b2ff, 0xc2a24b5, 0x43ebdd8,
          0x4f564d7, 0xa6d6711, 0xf414160, 0x495df63, 0x76f6de6, 0xf5bacd7,
          0x7c2b43d, 0x3011aff, 0x3241928, 0xbb1e64c },
        { 0x5034073, 0xf70c572, 0x68f1e97, 0x891c62a, 0xb22e374, 0xed8eb2e,
          0x7dbcc2f, 0xd3a53e9, 0xdc8f220, 0x1d06281, 0xace4393, 0x9eef48f,
          0xd2abecd, 0x96014f5, 0x2653ceb, 0x1da7e09 }
    },
},
{
    {
        { 0xd00bc94, 0x7593318, 0xc7262a2, 0x586f3c6, 0x958ad31, 0xea68f52,
          0xd4e8bed, 0x6707fcc, 0xcb3f9ce, 0xb7e35d6, 0xf4b1be8, 0x2cbb6f7,
          0x7b41aee, 0xa535268, 0xf7b39b8, 0x1d77845 },
        { 0xeaf9554, 0xb1f3995, 0xfe9e7d4, 0x3250f70, 0xa00c23c, 0x62e5d1b,
          0xc10e3bf, 0x5e422f5, 0xc25cec4, 0x7a18039, 0x7cc4d5b, 0xb4e66a1,
          0x36d0e0c, 0xad7c5f6, 0xa4cf347, 0x9f40b12 }
    },
    {
        { 0x51e3696, 0x697f882, 0xab0a648, 0xc89bc40, 0x9785804, 0x8f261a5,
          0xb51a2bd, 0x4c7f900, 0x8a2dfcf, 0xd00e7af, 0xb642aeb, 0xf9c534d,
          0xb63df0e, 0xea2a79f, 0xf2f64a4, 0x392a69a },
        { 0xc331b6c, 0x0c0f01c, 0x6a5edb5, 0x414bf2e, 0x5068391, 0xfe5ed81,
          0x62fbc34, 0x0a8078d, 0x54bca98, 0x78a4382, 0x3d727c7, 0xf7a49ae,
          0xab4dffe, 0x96c1de1, 0x3b9440a, 0x45901f7 }
    },
    {
        { 0xacfe46e, 0x3f1189f, 0x4467443, 0xdca6f46, 0x2eb5bcf, 0xac38542,
          0x906bf72, 0xb02dce9, 0xfe1d454, 0xdd8cdac, 0x65f7218, 0xc26f04c,
          0x6ea145d, 0xb474859, 0x5bdb315, 0xc53dc6b },
        { 0x9ad7197, 0xbe5be74, 0x18b5ecc, 0x627e919, 0x9ea405d, 0x57c889c,
          0x1a5360b, 0x2e5650c, 0x1b30b27, 0x42290df, 0x5242687, 0x4a07157,
          0xd379133, 0x553ed1f, 0x01db019, 0xb9d7a07 }
    },
    {
        { 0x56597dc, 0xcfe551c, 0x925ebd6, 0x81af92a, 0xf4e8d57, 0x83efe16,
          0x1f640d3, 0x61bb431, 0x78b414a, 0xf80440f, 0x6c9e3b4, 0x72f3c63,
          0x6a03c66, 0xb55f43a, 0xe417037, 0x47a9ded },
        { 0xdbb612b, 0x1a7e287, 0xdbb9220, 0x895c3c7, 0x6c04764, 0xd50c86e,
          0x53cf7ca, 0xed52698, 0xf74af55, 0xc78d799, 0xb969ff2, 0xb2ba0f2,
          0x1c6530b, 0x06d4815, 0x165a575, 0x764a1fe }
    },
    {
        { 0xc1b5ece, 0x4383a3b, 0x54ff148, 0x0563c88, 0x5af796e, 0x9a45279,
          0x88e9953, 0xffba7c0, 0xb6a3001, 0xfe9fb5e, 0x25b6b19, 0x7950988,
          0xd81be5e, 0x67c899a, 0x2f9d29b, 0xc89ac8d },
        { 0x29ab8f7, 0x7c76ba3, 0x6e40f74, 0xb2a18c9, 0x3864d9b, 0x1b5056e,
          0x9b582b8, 0xdfa503d, 0x7c9c68e, 0xfb03519, 0x6b3c22b, 0xdc50131,
          0xa6c96ff, 0x38ab231, 0x8cb1c10, 0x4ea527c }
    },
    {
        { 0xc05b4ed, 0xd632f20, 0xb2a032d, 0xe0199fa, 0x26812d7, 0x3732956,
          0x013df13, 0x2aed855, 0x39f96ac, 0x92ca24b, 0xbb9751a, 0x620273d,
          0xf7437a1, 0x5d0d21e, 0x077de56, 0x9de2a43 },
        { 0x11a4674, 0x0569b12, 0x89c3989, 0xfc3923e, 0x2c5c770, 0x3d12704,
          0x84e8c37, 0x0072b90, 0xac39f9a, 0x7178d4d, 0x778d345, 0x5f8292f,
          0x77c7307, 0x9e5bf0f, 0xc3a20f5, 0x7691610 }
    },
    {
        { 0x705fe96, 0x7c4ead5, 0xc8e464c, 0x377ec35, 0x7689954, 0x3e5b990,
          0xa2d31ea, 0xc0f6949, 0xc580671, 0x839d395, 0xb215b09, 0x2f347a6,
          0x683df83, 0xfdcfa33, 0x6af39a8, 0x6e12cc2 },
        { 0x13a3bd2, 0xae46ec8, 0x59366f8, 0x03a7d3b, 0xb87aed4, 0xe2029d5,
          0xfe1b83d, 0xbdc4e43, 0xdb8a1a8, 0x768437c, 0xea0dd7f, 0xe47acc3,
          0x62a0af4, 0x550e0cc, 0x1a20962, 0xcaf2cbc }
    },
    {
        { 0xf28a78f, 0x5a784f7, 0x07e9724, 0x952a9b5, 0x1bab7a3, 0x8ac5e41,
          0xb7bc1e1, 0x1251e3f, 0xdc15e22, 0xe360f82, 0x95213f5, 0x3ac72da,
          0x4dcd47b, 0x65ee9ba, 0x3af5952, 0xdfeab7b },
        { 0x26fd3c6, 0x34c5c80, 0xf3ac7ee, 0xd977b08, 0x7dba2f6, 0x003bd01,
          0xac98c8d, 0xcfc5cf8, 0x0e46922, 0x05eb604, 0xfaa9352, 0xc248b17,
          0x395c7a7, 0xfa41c0f, 0xb71ee44, 0x29931d4 }
    },
},
{
    {
        { 0x07861c5, 0xac087bb, 0x5ae8240, 0x3bd37db, 0xf94518f, 0x94c68ec,
          0xff88a5b, 0xd32a378, 0x9b441d1, 0x42c8aaf, 0xfc07f12, 0x089db70,
          0xd3d4455, 0x211c386, 0x546b158, 0x1db9af7 },
        { 0x51bc927, 0xdfd1b65, 0x0733df4, 0x69c0493, 0x2aeb586, 0xdc72cd4,
          0x823aa13, 0xeebdace, 0x56ad643, 0x51b3b3c, 0xd4e0426, 0xb983a99,
          0x69c4ecc, 0xa1e5b6c, 0x45e6668, 0x37cd382 }
    },
    {
        { 0x9f73aea, 0x158ce6d, 0x14ff475, 0x36a7749, 0xdc0b018, 0x0d4e424,
          0x3946f09, 0xc2c4448, 0xfacda62, 0x7a7de3f, 0xb486709, 0x49a19e6,
          0xdb61da7, 0x65094d8, 0x8f5ee87, 0x09edfd9 },
        { 0xb37226d, 0xe460fcf, 0x69bf470, 0x3b9d039, 0x247ca22, 0x3d4d511,
          0xc782cb1, 0xc7248d6, 0x00ad293, 0x91189a0, 0xe8abe75, 0x1244942,
          0xbf52cdb, 0x9f88d12, 0xbbbcadf, 0x368463e }
    },
    {
        { 0x8074f45, 0x419e4b3, 0x0771c83, 0xd3f8e2e, 0x2e68d34, 0xd2743b4,
          0xb116a00, 0xc68b7db, 0xd84cc37, 0xfad2cf7, 0xb7a0f4d, 0xcfd27c0,
          0x190e587, 0x3b9e23f, 0x751ca9e, 0x7bab499 },
        { 0xa8f12ee, 0x3270861, 0x31b36d5, 0xee1f38d, 0xe4c0eed, 0x748bb31,
          0x110ebad, 0x9be5c9b, 0xc8b6cb6, 0x728660b, 0x93d914a, 0x7bc9df7,
          0xc88c859, 0x73a4f2c, 0xb4e7f0e, 0xbe4a2fd }
    },
    {
        { 0xa450e77, 0xe566ff8, 0x6a13aba, 0xb0b4006, 0xcd7dc90, 0x483a510,
          0x5fa9ccc, 0xb1a2013, 0xa80e67c, 0xeb0b631, 0x020801a, 0x7c34e1f,
          0xf4e447c, 0x0257dc8, 0x74c6f0f, 0x7abe7d1 },
        { 0xb19a576, 0xf115a3a, 0x064ca0e, 0x8f0474a, 0x351f99b, 0x999bb6b,
          0x773edc3, 0x855254b, 0x427d717, 0x49f6c2f, 0x2e0cef2, 0x9f68253,
          0x2ee34f5, 0x1fe126c, 0x80150f7, 0x1ec2cae }
    },
    {
        { 0xc005b7a, 0x862c5af, 0xec4ef17, 0x61adea7, 0x007b446, 0xf885fd3,
          0x9b0e30e, 0x25c129d, 0xfeec7e0, 0xbc10f25, 0xdf79ee1, 0x3901ac4,
          0xfe9e19f, 0xad49db7, 0x360d050, 0xc8624d9 },
        { 0xbf3260b, 0xc74a576, 0x8c010c2, 0xbde8024, 0x09b6977, 0xf155329,
          0xd52dcf8, 0x6a5a82e, 0x29b9dfc, 0x4fbf59d, 0xc7b730c, 0x337d049,
          0x3a89cd4, 0xb3deac6, 0xad2f2eb, 0x1e07595 }
    },
    {
        { 0x3b7c84e, 0xa0b0a4d, 0x8cf2b00, 0xf132c37, 0xeaaa8ec, 0x192814b,
          0x7b4b5df, 0xe7929f9, 0x42d0ab7, 0xf08a68e, 0x7b60cdd, 0x814afb1,
          0x7d9c160, 0x78c348c, 0x44db217, 0xf8a9488 },
        { 0xeaa2578, 0xcdefd88, 0xbd0e260, 0xf717f56, 0x1694d02, 0x7754e13,
          0x181dbd8, 0x1254c14, 0x6e5f312, 0x0dacdd2, 0xcef87bf, 0xb8abdfb,
          0xe74e2ea, 0xb985972, 0x002b424, 0x1717621 }
    },
    {
        { 0x162df70, 0x92cc75e, 0x18ee849, 0x1e20c06, 0x26aa590, 0xc036b46,
          0x4da5155, 0x31be67e, 0xf7213b0, 0x04911b5, 0xbb2e72e, 0x39261d7,
          0x5c015a3, 0x9e84466, 0x298ae67, 0x2f59fc0 },
        { 0x1701fcc, 0xa3ea7ba, 0x0ebd651, 0x87a5fa9, 0x301d7b1, 0xa607ed4,
          0x3b2e271, 0xbd4ec5f, 0xdc4180f, 0x732a1a2, 0xfeaa8c1, 0xbe15d82,
          0x66f2f3f, 0x1036702, 0x9e79ce8, 0xccfd397 }
    },
    {
        { 0x70a54ad, 0x82ab835, 0xe3bec75, 0x5c1dee8, 0x54b556b, 0xf583ff4,
          0xf461e60, 0x9220199, 0x87fc4e7, 0xdf61ca8, 0x0776dad, 0x6641fd2,
          0x8edd061, 0x00c6edd, 0x55f7e87, 0xaf9b142 },
        { 0x9bbe3ec, 0x73f15e4, 0xf8bc1fa, 0xdd3b788, 0x1b8ff86, 0xb24cc07,
          0x41be58b, 0x6c260d2, 0x6b10ada, 0xec1c4e3, 0x7fdb985, 0xf6b4209,
          0xd47c212, 0x0d0ac85, 0x07d78d1, 0x967191c }
    },
},
{
    {
        { 0x843d0f3, 0x3b11638, 0xf27f10e, 0x4b89297, 0x863ba2a, 0x477236e,
          0xadd280c, 0x1949622, 0x04da757, 0x7cd5235, 0x79e4ff7, 0xe0e99d2,
          0x537da41, 0xb4ef894, 0x5a24ff1, 0xc55dde4 },
        { 0xb587521, 0x18d8e21, 0x3777833, 0x8010b5d, 0xd3a54c8, 0x4af522d,
          0x4c0ac13, 0x7cd476b, 0x4099f67, 0x4587e61, 0x605ee64, 0x494d0ed,
          0xcc80903, 0x3218ba2, 0x0b2e169, 0x5ff56aa }
    },
    {
        { 0x3a06c69, 0x51ec94e, 0x5e65c52, 0xa26d7be, 0xd44ee96, 0x156f113,
          0xbf5b9b4, 0x70f0968, 0x5f5332d, 0x9b7e469, 0x6703829, 0x36c295f,
          0xd04f492, 0x1522690, 0x728043b, 0xcf35ca4 },
        { 0x190a7c3, 0xf9ca3e1, 0xf971b07, 0x53d2413, 0x9c48b49, 0xae59652,
          0xfefff5c, 0x74672b8, 0xa7643b0, 0x0a3018b, 0x3e9b0a8, 0x51919e8,
          0xc932fb5, 0x89ad33d, 0x643e687, 0x52a4419 }
    },
    {
        { 0xd2d0acd, 0x7778990, 0x487fdf1, 0x3bdbcce, 0x2b03dd2, 0xdc413ca,
          0x9a2b7d0, 0x278755b, 0x35ddd7f, 0x4ebb8b5, 0xbcbdb92, 0x0465152,
          0x671d051, 0x34f22d6, 0x87192b9, 0x1ba04c7 },
        { 0x83560c1, 0xb1693f4, 0x7d174e9, 0xe08a593, 0x64dc9af, 0x47ffdc4,
          0xce8126c, 0x1123596, 0x1124628, 0x632d95f, 0xfee7c76, 0x66287ab,
          0xc552332, 0xb40fe60, 0xe304e1e, 0x3f11729 }
    },
    {
        { 0x5030a8c, 0x97a6ea0, 0x09c27b2, 0x6924198, 0xac9dd5d, 0x3308501,
          0xbe73fdc, 0x9fed7fa, 0x0535286, 0xea55544, 0x6c9b832, 0xc7c07ab,
          0xc51b967, 0x178c882, 0x86ee075, 0x6fa0c69 },
        { 0xb8b5c4a, 0xbaa4a15, 0x3130c0a, 0xf83c0ea, 0x2800331, 0xcf8624b,
          0x7ccbcb8, 0xade85cd, 0xf08445d, 0x971d7f6, 0x6a546dc, 0xfd480b7,
          0xc93761c, 0xdc15a38, 0x9d04631, 0xc4c495c }
    },
    {
        { 0x9470efe, 0x5f4cee8, 0x88d93ad, 0x9fe8961, 0xf4e49ce, 0x24783b3,
          0x52ffb3e, 0x1bc7ed7, 0x6d81e17, 0xa3abe6a, 0x7a333c3, 0xd6bb8b4,
          0x10a3527, 0x3485c0b, 0x31a9d10, 0x7cddc9c },
        { 0xc38ca37, 0x0c78112, 0xdd2f8d8, 0x10e249d, 0xc511911, 0x72c88cc,
          0x29a6c84, 0x4d75b5a, 0xa227b1e, 0xc74b267, 0xf8e35ad, 0x698390c,
          0xe98d230, 0x8f27edf, 0x6bdc7f4, 0xec922f2 }
    },
    {
        { 0xfc32e11, 0xac34023, 0x47200d1, 0xe0ae2f5, 0xbd98c82, 0xa7c7492,
          0x7b02154, 0x3910b68, 0xe28ab6d, 0x6fdd06c, 0xd98b012, 0xd3a7e49,
          0x9f54207, 0x4c1c82b, 0x45c176f, 0xef5bbe6 },
        { 0xd3e71eb, 0x3d17960, 0x080e70c, 0x90d7e84, 0xbff5d9e, 0x83e6438,
          0x535d85c, 0x1877e1f, 0xfbb69cc, 0x931ed6e, 0x1247848, 0xcf96265,
          0x750da4e, 0x76d618b, 0x717fbf6, 0xc076708 }
    },
    {
        { 0xeec5126, 0x80a5ac5, 0x3379c80, 0x6d05dd1, 0x2336d32, 0x514b089,
          0x6725137, 0x586c006, 0x574f954, 0xab2365a, 0xac7d356, 0x3c89ea0,
          0x27460ba, 0xf1f2edd, 0xab9870f, 0xf200ddb },
        { 0xa35e885, 0xc8f1b2c, 0xe6e7550, 0x5d22f86, 0x9554615, 0x24b9a40,
          0x616314f, 0xcb41107, 0xc976a11, 0xca752f0, 0xa08291a, 0x3e2f839,
          0xf2c420e, 0x0cff22f, 0x82b9747, 0xafd603e }
    },
    {
        { 0x810a3da, 0xaddeddc, 0xd3a87bf, 0x78b6c2d, 0xde3a04c, 0xbc7020b,
          0x9b6d045, 0x47ab973, 0x0959358, 0x3b046d6, 0x509ee3e, 0x0f953e7,
          0x69fc61b, 0x803dc86, 0x893c8d4, 0xcceaec0 },
        { 0xb048a45, 0x21f8c40, 0xfcaea8a, 0xb535073, 0x90e360b, 0xe712c35,
          0x8403338, 0x5d0f3f4, 0x7207f2d, 0xe0ea26c, 0xffd9e05, 0x20f6b57,
          0x4788b00, 0xb97d68e, 0x1889cce, 0xb121554 }
    },
},
{
    {
        { 0x464238e, 0x0079817, 0x0d381ca, 0x2110302, 0xd9f01b5, 0x1cc4c6e,
          0x5a131b1, 0x5e35dc5, 0x06944eb, 0xb61848d, 0x29631a3, 0x83792a0,
          0xafca0dd, 0xbe1017f, 0x782fcbb, 0x70aaa01 },
        { 0x99945e7, 0xc63b7a0, 0xc4486c1, 0xe9164ec, 0x885f2c1, 0xb133e35,
          0xc99ae02, 0x186f0d3, 0x2bf53e6, 0x2fca492, 0x48a02bc, 0xf922aa2,
          0x0dd3dca, 0x4fe6490, 0xf6a8207, 0xe8c313f }
    },
    {
        { 0x97caf1e, 0xc5b3583, 0x922a4b6, 0xa001922, 0xdf07c95, 0x67e36be,
          0xb2f4f34, 0xabaa0ae, 0xdedc333, 0x66dc926, 0x38ec5b3, 0x82021c4,
          0x00ab176, 0x82b4f26, 0x69c45af, 0x1b7c22e },
        { 0x0924ad9, 0x07b0dbe, 0xa407dde, 0xe030936, 0x26ccd06, 0x66e1ce9,
          0xe3505a9, 0xb50c108, 0xda98f51, 0x8b921e1, 0x20cf7c7, 0x449ca1a,
          0xe67d079, 0xadb80c7, 0x834372d, 0x205aa54 }
    },
    {
        { 0x19bf847, 0x1482b48, 0x5906f0f, 0xd6c16ab, 0x23ad060, 0x323fb17,
          0xc832be7, 0x0346389, 0x2ee45bf, 0xe71b2d8, 0xfb22276, 0x761c37d,
          0x5d70be2, 0xa9b3334, 0x5a0627a, 0x81a0656 },
        { 0x99a6282, 0x3377503, 0xd0436f0, 0xafc8d2e, 0xc53342f, 0x22f71d3,
          0x8939ad3, 0x66ca56d, 0x30e09ba, 0x15a9192, 0xa6de890, 0x261091e,
          0xe78f2d5, 0x609d700, 0x8eaaf78, 0x8aa52ee }
    },
    {
        { 0xce76258, 0xa398788, 0x494b975, 0x3031d07, 0x043dfe2, 0x4a6d652,
          0xb4401ec, 0xdb1a849, 0xce8bbcc, 0xf81ebbb, 0x16efe9e, 0x937dd47,
          0xef85ecc, 0x9c19350, 0x214273b, 0x260d932 },
        { 0x77bf1a3, 0x1d7e21e, 0xa544eb7, 0x199d689, 0x94ced50, 0x9da5941,
          0x8a0aeaa, 0x71a60be, 0x26d3b51, 0x183a0ae, 0x8df9728, 0x49f176a,
          0x3230674, 0x744376e, 0xe25541c, 0xb2cb21a }
    },
    {
        { 0x9a0071f, 0x7a72158, 0xe7d2a6b, 0xe19dd29, 0x55113f0, 0x3deb34e,
          0xede573b, 0xef1f8eb, 0x5665e37, 0xa8f7ff9, 0xf2d7777, 0xa2c21ea,
          0x91e2e39, 0x1387afa, 0x7db68f6, 0x04057b9 },
        { 0x1c241f7, 0x8b9d5ae, 0x8e75993, 0x689588a, 0x5c0e2d4, 0x79585b4,
          0x7b64974, 0xba1ef16, 0x1c08a75, 0x72685bc, 0xd572edd, 0xf0a5814,
          0x5ab0e70, 0x71464a3, 0x339aea7, 0xc93c92b }
    },
    {
        { 0x5b8a87d, 0x1917e2a, 0x3a82756, 0xea5db76, 0x6420e2b, 0x5bba2fb,
          0x019372a, 0x5cc0501, 0xccc5efd, 0xb1ef8be, 0xf49c57d, 0xaf06393,
          0x87a0bc4, 0x3ab1adf, 0x34fe6b6, 0x2ee4cca },
        { 0x6b8ba9b, 0xd160668, 0x7efec13, 0xef137d9, 0x50abb76, 0x7b60465,
          0xf753a00, 0xb40ec2b, 0xeaf8f1d, 0x696ed22, 0xd8ba3d8, 0x398c91f,
          0x37db313, 0x11f2034, 0xfe5079e, 0xe1ec33b }
    },
    {
        { 0xbdc81f0, 0x8a10c00, 0x6fe8e05, 0x5f39256, 0x14a368e, 0xa595dab,
          0x38cec6b, 0x32b3181, 0x1b00d00, 0xd77afde, 0x4d9923d, 0x3c97928,
          0x76e13dd, 0x78f0e7a, 0xbf75675, 0x5ee8e59 },
        { 0x91b130c, 0x49ec893, 0xa47a441, 0x9416182, 0x76e2ce8, 0x54555b5,
          0x349c40b, 0xcbdd2fd, 0x9392bbe, 0x10ae737, 0x2e2dab0, 0x270b111,
          0xaf293f4, 0x5cb7712, 0xd6095c6, 0xfc22a33 }
    },
    {
        { 0x0f15878, 0xdcb5bbd, 0xb6bba48, 0xbcf27ad, 0x7b70eba, 0x979913e,
          0x158578a, 0x4c0f34b, 0x6ed6088, 0x53f59a7, 0x75b0fc2, 0x19b3b2c,
          0x0153f3c, 0xad628dc, 0xcec1607, 0x5195a2b },
        { 0xdfe0f7a, 0x95f8b84, 0x152920b, 0x935c6b0, 0x4da1056, 0x25f9e31,
          0xb28c229, 0x4910a94, 0x8ee4d6e, 0x54b03b4, 0x694e3ed, 0xc991fc3,
          0xdbe5709, 0x68c4c26, 0x63d7657, 0xc9cfce4 }
    },
},
{
    {
        { 0xf52a44e, 0x21c9227, 0xe85bfbd, 0x7f105a2, 0x6268fc2, 0x887781f,
          0xa2d7e35, 0x56ee808, 0x2d3930f, 0x14f9de5, 0xdcb561a, 0x4a4e356,
          0x7f95598, 0x8736226, 0x5f34151, 0x211c342 },
        { 0x0eaf9cb, 0x8fcb75b, 0x3d60ce2, 0xcc9edf9, 0xa5fe627, 0x54412c9,
          0x842dd09, 0x6036a72, 0xa6c6099, 0x71ce668, 0x5386764, 0x02b30d7,
          0x6f18e23, 0xb69bed3, 0xd1de9f4, 0x124c9b1 }
    },
    {
        { 0xe69b531, 0xe8f8d95, 0xaff1049, 0xe1e115e, 0xeddea0c, 0x9087cd1,
          0x7449916, 0x8ed55a5, 0x7808404, 0x8009f54, 0x17fea55, 0x990f216,
          0xfe8ecf9, 0x68ba624, 0x56d1f47, 0x8ac2950 },
        { 0x529dfb0, 0x3257887, 0x244c080, 0xc4a613f, 0x28672fa, 0xabb1ac0,
          0x31eb291, 0xb2915c5, 0x8fababa, 0x6e368ca, 0x1fde498, 0x6b8c259,
          0xf2a548c, 0x67724a1, 0xf90409b, 0x6b3b7e8 }
    },
    {
        { 0xfae20aa, 0x5415003, 0x85df5ce, 0x95858a9, 0x0ac6bee, 0x42bc987,
          0x39ea1a9, 0x8d843c5, 0xb571043, 0x5de200c, 0x1741a33, 0x084fcd5,
          0x0009d1c, 0xe1ca20c, 0xe957e6d, 0x0271d28 },
        { 0x9e3be55, 0x84cbf80, 0x1c578c6, 0xc804dda, 0x409a93a, 0xea85489,
          0x972021d, 0x64a450a, 0xe681312, 0xc6a2161, 0x65bc111, 0x280bff9,
          0x0f8526f, 0xd358a4b, 0x953a3ab, 0xd967be8 }
    },
    {
        { 0x7dd066c, 0x4c5e615, 0x634c8d4, 0x37afd33, 0x42d8b87, 0xa3ac88a,
          0x938b607, 0x9681e9b, 0x37fe4c8, 0x7a286ab, 0x2494245, 0xdeee574,
          0x6af75a8, 0x184b9d3, 0x3670c04, 0x20f696a },
        { 0xa39e8b9, 0x1340adf, 0x0850b2e, 0x03c1929, 0x2c0e1ef, 0x435ebd4,
          0x142ee9b, 0x49de18b, 0x3f116f2, 0xb440b27, 0x2214463, 0xd94e9fa,
          0x6311543, 0x1b0ddd3, 0x991ba3c, 0x1ae042a }
    },
    {
        { 0x5bb47aa, 0xbc322f8, 0x54a5845, 0x9e25625, 0x21115f3, 0x96b65ae,
          0xbb5757b, 0x46fbed4, 0x4c42dce, 0x18aec4f, 0x8d801f0, 0xc59caf6,
          0x1205521, 0x9189463, 0x89feb7a, 0x66bd8e0 },
        { 0xc529ee7, 0x39ebe95, 0x8eadb99, 0x28d8992, 0x6927544, 0x6058c78,
          0xd3808ec, 0x877e7a5, 0x1c52eaf, 0x8f65111, 0xae221cd, 0xfb59812,
          0xf890391, 0x22289c6, 0x4966e92, 0xa97695b }
    },
    {
        { 0x6ff10f0, 0xf0a9122, 0xa2a65c8, 0x49a931b, 0xb1d3cb0, 0x3fcebbc,
          0xca9685f, 0x70eb79b, 0xab38cb6, 0x82520b5, 0x76304c3, 0xccf991b,
          0xaf8b07c, 0x575aab1, 0x5ed5efb, 0xec8166a },
        { 0xc8689b1, 0xddc5698, 0xb2e78d7, 0x227c949, 0x8e07d91, 0x6132321,
          0x22cfd62, 0x658a11d, 0x004dd5f, 0x908fb44, 0x90d21b1, 0xe3d14f0,
          0xa6a1639, 0x6f3db9d, 0x333a525, 0x09d86c0 }
    },
    {
        { 0x6f043f7, 0xd83eaf0, 0xb52d5f6, 0x88ab648, 0x57144d7, 0x67c664d,
          0xeafc8b5, 0x55d7644, 0xcceb291, 0x1c89f20, 0x831ac47, 0x51aec7b,
          0x6148854, 0x51172fa, 0xf6d7bfe, 0x8fabf7e },
        { 0x477ee27, 0x5910316, 0x20fe61e, 0x5f299dd, 0x42826ab, 0x48079a8,
          0x22591fa, 0xf4a83ba, 0x55482ec, 0x8fac660, 0x6b65b3b, 0x48fd5f1,
          0x9fd9e19, 0x4288a7c, 0x9377894, 0x27db819 }
    },
    {
        { 0x7fd9dd6, 0x2936ee4, 0x9ec87c6, 0xcce5f0e, 0xdb6e3b4, 0x15a50e3,
          0xad701c8, 0x61df105, 0x1dff1f7, 0x3601add, 0xe8a16e1, 0xb761e06,
          0x1af3f91, 0x4341e02, 0x933fa3f, 0x9156a4a },
        { 0x54bc01d, 0x9dc46ae, 0x64eb910, 0x605577a, 0x5a59a99, 0x22b99f8,
          0x0a229d8, 0xab2dbaf, 0x6599364, 0xa8bfb65, 0xe94ebf0, 0x39ed4a5,
          0x0dbb23e, 0x7b46a1e, 0x8751422, 0x117b195 }
    },
},
{
    {
        { 0x423bddf, 0xd19e8fd, 0x387ef59, 0x9d77042, 0x849590a, 0x315cbdd,
          0x7866c1e, 0xfdc637c, 0x03515a6, 0x72be83d, 0x0376780, 0xd44a4a0,
          0x19e0c2b, 0x3b96131, 0x7b1a689, 0x023aca3 },
        { 0x82282ea, 0xf5f3687, 0x8a8b5c7, 0x4471089, 0x17a3066, 0xcd2f00a,
          0x81ed681, 0x754e112, 0x0bfcefd, 0x9c6c70c, 0x3b6f29b, 0xd6aced0,
          0x2817a2a, 0xe443d56, 0xe7c0012, 0xe590ef4 }
    },
    {
        { 0x3e62e2a, 0xc2f9676, 0xb2daa26, 0x661816e, 0xdd5f512, 0x3515fd2,
          0x56b6e75, 0xdc36e27, 0x74cc658, 0x0bdde46, 0x00e7644, 0x1029086,
          0x1694a09, 0xfdf0045, 0xceac169, 0x454bcb6 },
        { 0x6481eb6, 0xf4c92ab, 0x09750e7, 0x8b77afa, 0x6362d6d, 0xe6f4231,
          0xf53a3ae, 0x0d45dee, 0xd7dcf98, 0xdac7aac, 0x125ec4a, 0x628cb7f,
          0xaec0320, 0x41e8a20, 0xea2e35b, 0x7418c7e }
    },
    {
        { 0xdf40519, 0x4d649ab, 0x3525833, 0x8cb22d4, 0x7a5333f, 0x15f6d13,
          0x72c23ee, 0x8c3991b, 0x0cd44a3, 0x248b9a5, 0xccc1a75, 0x6b4c4e0,
          0x15c99a9, 0x3221efb, 0x0a9c504, 0x236d504 },
        { 0xd559100, 0x401c7fb, 0x07c524d, 0xcf0e075, 0x34a9275, 0x39647c0,
          0xf7e8683, 0x2355422, 0xb3ae670, 0x3e0a16e, 0xad61b7f, 0x1c83bcb,
          0x9ca6cbe, 0x491bcb1, 0x5e29458, 0xe668dc4 }
    },
    {
        { 0x219379e, 0xe44c65b, 0xbb607ee, 0x211381b, 0xb7bc6db, 0xd4c7428,
          0xb76a2e8, 0xba62a03, 0x8bb0b31, 0xe1729c9, 0xc6bbc10, 0x3caeb50,
          0xb0187aa, 0x6c66727, 0xfb90dcf, 0xbf9d2f0 },
        { 0x1184dc6, 0xec69350, 0x2698eb5, 0xd58d2a3, 0xa316b07, 0xb366d8d,
          0x251c017, 0xe1e39bb, 0xadb157f, 0xbe44ba9, 0x8a8b06c, 0xbaa9a9a,
          0x6e473e1, 0xd0f4635, 0x1d681c6, 0xd25a8f6 }
    },
    {
        { 0xcb102c7, 0xba39d5f, 0xd8aa1eb, 0x66eba21, 0x697fbf4, 0xcc2591a,
          0x2317f54, 0x5adb579, 0xf76c6f9, 0xa01ae71, 0x5042705, 0x2c525de,
          0x4f4479f, 0xc8f4272, 0xe6d7a5b, 0x26ab54a },
        { 0xdc28106, 0xda217b5, 0xeb2ae6a, 0xc7cadea, 0x53ea3b2, 0x0b16094,
          0xcc6111b, 0xcddcc1c, 0xa7a7beb, 0x5c47aff, 0x0e52dab, 0xf9931bd,
          0xc6dcf96, 0x5231835, 0xf27ea4e, 0x7095bde }
    },
    {
        { 0xc33b4e2, 0xee8adae, 0x63ceb44, 0x3006651, 0x880b086, 0xf1476fb,
          0x9569ce8, 0x0703328, 0x238b595, 0x2cabf9a, 0x26c8158, 0x85017bc,
          0x68d5144, 0x420b5b5, 0xf9c696f, 0xa9f5f1e },
        { 0xc8fec5a, 0x1409c3a, 0x28e9579, 0x541516f, 0x0e1f446, 0x06573f7,
          0x2311b96, 0x3e3c706, 0x3c2ffd8, 0x0033f1a, 0xca6711c, 0x8e808fc,
          0x07aef98, 0x716752d, 0x92525b3, 0x5e53e9a }
    },
    {
        { 0x5a1c29f, 0xce98a42, 0x3ca6dc9, 0xaa70348, 0xedfa48b, 0xe77d822,
          0x068abca, 0xd2e3455, 0x482cfca, 0xb456e81, 0x7fbfb08, 0xc5aa981,
          0x8243194, 0x8979f25, 0x2cd043d, 0x727f217 },
        { 0xaa53923, 0x7cca616, 0xe9bcb72, 0x387c5ae, 0x37580bb, 0x0173fd4,
          0x75fc0d9, 0xdd7795b, 0x345deae, 0x47d1c37, 0xb0d1c03, 0x2eb5d7f,
          0x958f002, 0xf7a1b92, 0x8f61b67, 0x7365cf4 }
    },
    {
        { 0x562a5ed, 0x4b22c3b, 0x5c7cd07, 0x711216f, 0x9ba0648, 0x51f72c4,
          0x0de9e6f, 0xc10d093, 0xfda63ba, 0xaca479b, 0xaf532b0, 0x4722a55,
          0x7236f39, 0x8d59eb7, 0x4465c34, 0x5cad874 },
        { 0x722b0c1, 0xa2119e5, 0xf343ea4, 0xb670264, 0xc19f387, 0x6910f02,
          0x0381fba, 0xcfec5bc, 0x52c0a1d, 0x5f5de0d, 0x6378cb6, 0x4e474d5,
          0x27e2ba3, 0x2fc8027, 0x159b541, 0xa215da3 }
    },
},
{
    {
        { 0x8499895, 0xed53585, 0x65c998d, 0xa0aefd5, 0x2d5a561, 0x210d850,
          0xa2cd9d6, 0xc2cc23c, 0xc4d297e, 0x2371d46, 0xd18d441, 0x88b2143,
          0x043993d, 0xbebdad9, 0xad5f28d, 0x6ba91e7 },
        { 0x3a731f4, 0xc2bb3f1, 0x5d0d5c3, 0xd35cfac, 0x35ac427, 0x9950998,
          0x5458adb, 0x8938bb5, 0xab26f3b, 0x0bd738c, 0xa28cd8d, 0x56db3d5,
          0xa1d8b4b, 0x87eb95f, 0xe7f3b4b, 0xd6700ef }
    },
    {
        { 0xea1e57b, 0x962c920, 0x6dded6d, 0xd3be37e, 0x2c96a73, 0xf499b62,
          0x6c99752, 0x3eaf7b4, 0x025590b, 0xa310c89, 0x721db23, 0x535aa4a,
          0x19714a0, 0x56ab578, 0xd4048c1, 0xeecb4fa },
        { 0x470c466, 0x7b79ec4, 0x1383cee, 0xc4e8f2e, 0x5750c45, 0x0f5d776,
          0x725527d, 0xa3b3bc3, 0x6d00cce, 0x2f5deb6, 0x95a8d81, 0x5d5a0f4,
          0xe02b824, 0x50a442e, 0x2a11628, 0xafb0446 }
    },
    {
        { 0x0c613de, 0x72b67bc, 0xe6f0b24, 0x0150d4b, 0x8ed289d, 0x847854e,
          0xa320f88, 0xe08292f, 0x29c6160, 0xd5b6da3, 0x4fb9d06, 0x2a48e2d,
          0x2de087c, 0x55d9e41, 0x4f02100, 0x65683b5 },
        { 0xa8886c6, 0x4dc8c2e, 0x20d6114, 0xe966dd2, 0xa57af97, 0x99745eb,
          0xb854725, 0x23a9a71, 0x621a047, 0x8effe05, 0x049a4be, 0xf16d284,
          0x5b0660f, 0x95828c2, 0x56e96b0, 0xd5b69ba }
    },
    {
        { 0x4ffa0b8, 0x0b5b424, 0x096cc5e, 0x0585b45, 0xf505d37, 0x413e1ae,
          0x0c7ab8d, 0xe5652a3, 0x2990120, 0xab32fb7, 0x3f09368, 0x6b8b16e,
          0xefe128e, 0xbf9fadb, 0x14b7671, 0x85f366b },
        { 0x090608d, 0xcb2f294, 0xac3045f, 0x25e2769, 0x6131904, 0x069c4f0,
          0x329a779, 0x1c57cf1, 0xb7cace7, 0x72fe0d5, 0x0897a45, 0x04d9f43,
          0x359a645, 0xbaf32f6, 0xfa7485a, 0x0fa854f }
    },
    {
        { 0x5f56f60, 0xae3533c, 0x0ad9360, 0x9773bbb, 0x38fbe6b, 0x769b34a,
          0xffb0c00, 0xb5ba8e9, 0x75472e4, 0xa939318, 0xce5f30f, 0x12cac92,
          0xa9e7dbc, 0x514fc06, 0x58b4734, 0xd7ca865 },
        { 0x65a730b, 0xd101ff3, 0xabe70e9, 0x92da451, 0xef7bf4b, 0xfb5f94a,
          0x1d56c7b, 0x8c3ef4c, 0x8435c10, 0xb085766, 0xe7ed4cc, 0x7fbbbda,
          0x24f372f, 0x1da6eaf, 0x59b8ae3, 0x0ab2c1f }
    },
    {
        { 0xf10a4b9, 0x63a1a78, 0x0c7e510, 0xbb5278d, 0xf874142, 0x97b224e,
          0xb2517b1, 0x0a9ff52, 0xc5cd920, 0x1b5a485, 0xa1823b9, 0x1a8e2eb,
          0x0e914a8, 0x2b088c0, 0xcf13432, 0xe5ec3ad },
        { 0x6e7e253, 0x0d6ab3e, 0x6f18458, 0x9f0f5cd, 0xf459a6d, 0x839a744,
          0x1eb15f7, 0xb4b4f94, 0xc72cb14, 0xe0313ac, 0xb20472d, 0x58ee933,
          0x872543e, 0x5f73d7a, 0x501f067, 0xb1700c5 }
    },
    {
        { 0x085f67f, 0xb70428e, 0x43cabe5, 0x5441d51, 0xe0a6055, 0x4d0e8c2,
          0x0882e4f, 0x8d39a08, 0xc1cb39d, 0x615bb32, 0xf7a1642, 0x113f18d,
          0x250681f, 0xbab8cf5, 0x677b72a, 0x3017ba2 },
        { 0x5a3a876, 0xcd2b6e9, 0x2035a69, 0x0476501, 0xefa2ea0, 0x31d6440,
          0x56874d5, 0xde8f8d1, 0x0199d4a, 0xcbc71cd, 0xe7f2170, 0xc546b61,
          0x112c4c3, 0x4e57e4e, 0xd1622ba, 0x58955a8 }
    },
    {
        { 0x04e2f6f, 0x0064cd7, 0xe0edd38, 0xe9d458d, 0x7e0a5c8, 0xeb1a597,
          0x01fc0a8, 0xe322ece, 0x1032a19, 0x8b9d166, 0xa89de94, 0x3e7b539,
          0x001c754, 0xfa30262, 0xdb588f6, 0xe33de4d },
        { 0x954eb94, 0x4dafbdb, 0x0584c1b, 0xbb43648, 0x5dbe29b, 0x622c93e,
          0xf57b931, 0x968f9e3, 0x0f6453b, 0x98f03be, 0x08f696c, 0xb0ecc7f,
          0xa505335, 0x5af55f4, 0xfb3fa9b, 0x028533e }
    },
},
{
    {
        { 0x27e8d86, 0x3bc8e68, 0x63f105a, 0x4e43b30, 0x4981250, 0x5301b7d,
          0x9f72fa8, 0x8b0a75e, 0x357348c, 0x88f59db, 0xec4208e, 0x5f0ebb1,
          0xc043d3b, 0x4712561, 0xc806b97, 0x9e5ded0 },
        { 0x2121d09, 0xf9bd0a6, 0xe337cd1, 0x1759ecb, 0xe945542, 0xd1acc0e,
          0xbd2f63a, 0x3683feb, 0xda5dfe9, 0x44f1bcc, 0x707f22f, 0xa3606c9,
          0x2d96ca5, 0x45ef064, 0x9022df9, 0xfc3107d }
    },
    {
        { 0x44be755, 0xe81320b, 0x5c7c761, 0xdf213d5, 0xb4e5db9, 0xf43d2d5,
          0x8dedcd2, 0x3bcfd82, 0xd37a9ec, 0xdf368a6, 0xf475a77, 0xfef20ae,
          0x162c064, 0x22f5894, 0x0142a7d, 0x956bc66 },
        { 0x7daec78, 0xaaa10e2, 0xb6e9a78, 0x3cb9b72, 0xe383f72, 0xa740bad,
          0x7759007, 0xc31b401, 0xa7afc50, 0xdada964, 0xfd3d11f, 0x6bf062c,
          0x5db3679, 0x9470d53, 0x03abf13, 0x3394473 }
    },
    {
        { 0x46e5d7f, 0x533f440, 0x49048c8, 0xd1793e3, 0x1929b94, 0x59e1150,
          0x8364134, 0xcddbbcb, 0x582774f, 0x795c794, 0xe03081a, 0x114dfc4,
          0xef54042, 0x541ef68, 0x23f18cd, 0x159295b },
        { 0x48a2c8c, 0xfb7e2ba, 0xbb6d116, 0xe2d4572, 0xd750b53, 0x7bb0b22,
          0xd142ee8, 0xc58888c, 0x90c9e2d, 0xd11537a, 0xd02eb9e, 0x77d5858,
          0xd444a79, 0x1fa4c75, 0xd58a68d, 0xf19b2d3 }
    },
    {
        { 0xeb8b90f, 0x37e5b73, 0x3f2a963, 0x3737f7a, 0x9de35e0, 0x87913fa,
          0x8731edd, 0xec7f992, 0x219491e, 0x6e6259e, 0x4de236c, 0xb2148a0,
          0xfdd309b, 0x89700e8, 0x9f0bf80, 0x9ce51e4 },
        { 0x301f17b, 0xe7ec421, 0x3bc5f4f, 0xa4b570a, 0x1285ee2, 0xc2b1b2a,
          0xc53db73, 0x5e86bc8, 0xf24fa90, 0xb65fcea, 0x08ab024, 0x9e74c56,
          0xf9ed877, 0x5c8003d, 0x4a2cbbc, 0xa632e9e }
    },
    {
        { 0xc91c8b5, 0x32a4546, 0xc969363, 0xc122b5a, 0x3648b3a, 0xbbbec5e,
          0x25143b0, 0xd5a365e, 0x54157ce, 0xcf3e464, 0xf9bab64, 0x9712f04,
          0x04b4008, 0xc12d43a, 0x2edf1c7, 0x51932d7 },
        { 0xb2f8470, 0xaef1655, 0x6c24ace, 0xaa8e3f3, 0x6b4e761, 0x7da75da,
          0xb90bca2, 0xd371827, 0x0afb45c, 0x84db450, 0xef46b5d, 0xae12045,
          0xd962f98, 0x91639a5, 0x72f2ac0, 0x669cbe6 }
    },
    {
        { 0x83a4356, 0x851bb31, 0x9a1bf15, 0x7d436bf, 0x120b378, 0x46a3f0e,
          0x3f5b357, 0x9302abc, 0x93fef53, 0x1e06726, 0x5fd2ee9, 0xb12f4a9,
          0x7de9433, 0x94a884c, 0xa6f2874, 0x2645234 },
        { 0xcdb8dfa, 0x6fb56f5, 0x9e0ee4e, 0x4a17dfc, 0x83ab01e, 0xe269d83,
          0xb77c10f, 0xda932da, 0x0321243, 0x463af0c, 0x16fc8a3, 0xbe1d682,
          0x48b39e3, 0x2eae3ea, 0x3b03e7b, 0x9423021 }
    },
    {
        { 0xb22f28a, 0xaeb507c, 0x49a6b44, 0xa77458b, 0xc03dc17, 0x232ed5a,
          0x9c61ac6, 0x79dfc16, 0xcd71b93, 0x7c48be9, 0xc429cd9, 0x983d68a,
          0x98ae2c8, 0x7709c47, 0xa5df075, 0xe4765c0 },
        { 0x3367f33, 0x23c4deb, 0x37d72a7, 0xbdf2b7e, 0x0af2d26, 0xbaab5c7,
          0xfd026ab, 0xd609f7f, 0x541b039, 0x23b72b2, 0x83be852, 0x8d06bac,
          0xcb23d1c, 0x911d4a9, 0xfb0dbd7, 0xeae815c }
    },
    {
        { 0x2c33481, 0x487c35c, 0xb6136db, 0xffab636, 0xa3d3aa4, 0xccd4dae,
          0xc3704e0, 0x87149bb, 0xc0e8396, 0x9de8119, 0x58e7ca6, 0xd49357a,
          0x1562d75, 0x6878918, 0x5ab1fad, 0xc745381 },
        { 0x02c9b91, 0x0f15798, 0xb1ddde5, 0x7ffc3f0, 0x6aae50d, 0xa01d5e0,
          0xe279873, 0x6a97e65, 0xb5b1b41, 0x4bcf42f, 0x32f5982, 0x1c6410f,
          0x50701c8, 0xd4f7600, 0x873b90d, 0xff02663 }
    },
},
{
    {
        { 0xe5b2de2, 0xdc53ea2, 0x38acecb, 0x94b352d, 0x0d9d5e5, 0x37d960b,
          0x90bd997, 0xabd868f, 0x35a7376, 0x781668f, 0x10118bf, 0x043d597,
          0xf57928a, 0xd4da719, 0x983e46c, 0x01942f6 },
        { 0x728bd76, 0xab97fc8, 0x4b5c1c5, 0x825956b, 0xc82a104, 0x202809f,
          0xc8e3132, 0xdb63e9c, 0xc2181af, 0xa41c701, 0x43e066a, 0xd280180,
          0x24044ce, 0xc734e41, 0x505193c, 0x4d9ab23 }
    },
    {
        { 0xf9f0c3f, 0x0bcd42a, 0xb94a218, 0xda21a46, 0x0ffc788, 0xe55243c,
          0x47a5551, 0x318aae6, 0x79af9cb, 0x8c2938b, 0xec1dce5, 0x5d15232,
          0x8ad2e5c, 0x3d310ba, 0x94f792a, 0xd3d9724 },
        { 0x12a9553, 0xdeb4ca1, 0xeb54d9d, 0x2f1ed04, 0x69fb7a1, 0xaa9c9cf,
          0x54dcd3a, 0xeb73c3a, 0xf5f201f, 0xee3eddc, 0xba7d234, 0x35f9e1c,
          0xd2e242f, 0x1d1d04c, 0x0df7515, 0x48df9d8 }
    },
    {
        { 0xa81dd9a, 0x4ecc77d, 0x03aa015, 0xa6ac4bb, 0xbbc4fed, 0x7645842,
          0x9d6cf52, 0x9ae34cd, 0x5917e0b, 0xf8ff033, 0xc2cc175, 0x7c9da37,
          0xaaacfbe, 0x1e74dcc, 0x7999af8, 0xa8f2df0 },
        { 0x102a466, 0xd06c4ea, 0xae190dd, 0x2156e87, 0xec4a863, 0xc95db8a,
          0x244a6fe, 0x49edffd, 0x904f81e, 0x110fae6, 0xa1cd104, 0xbaa3e50,
          0x0478b65, 0x5bd38a2, 0xdaefbcc, 0x2b57d05 }
    },
    {
        { 0x86f4534, 0x1ce92ba, 0x414f5e3, 0xb2a8592, 0x9979436, 0xdd7a4c6,
          0x3f0add7, 0x7599aff, 0xe2d4f64, 0xe0ce4d3, 0x401a29f, 0x74475cc,
          0xa2377d9, 0xaef6541, 0x3f917b6, 0x54048f5 },
        { 0x05312ec, 0x1b86b22, 0x31493cb, 0x779ba22, 0xaac9320, 0xc718369,
          0x617fce4, 0xeab01a8, 0xf7187fa, 0x17b1f10, 0xa1aca46, 0xe68eda0,
          0x2586342, 0x61033fe, 0x0b6ca43, 0xfc14e79 }
    },
    {
        { 0x13d2491, 0x9f22319, 0x7997202, 0x66bdb53, 0x4617f34, 0x0bafb0c,
          0xf3bb7b3, 0x5917831, 0xb45bddb, 0x6feb2a6, 0x0202c19, 0x08662b3,
          0x05852f6, 0x0bc2b57, 0x91818c2, 0x2c00fd4 },
        { 0xda37dac, 0xca7672c, 0x5a30865, 0xfe4c04c, 0x322e92a, 0x5f1399f,
          0x25b1beb, 0xe7d67ea, 0xdce7f68, 0xe08b014, 0xf2f2b3c, 0x24df52a,
          0x750ecd1, 0x2028b23, 0xc810a45, 0x9b25d4b }
    },
    {
        { 0x7a9d799, 0xa35b715, 0x01f9c99, 0x6da1eb3, 0xe363ba8, 0x33ef91c,
          0xce140da, 0x21c0e2e, 0x158cd84, 0xb0b11bf, 0x93da438, 0x6a87442,
          0x3db585b, 0x924f10d, 0x10c6159, 0xf5ddd73 },
        { 0x6a74c21, 0xb72dcb8, 0xcc8f79f, 0x6d14198, 0x9c5a8d6, 0x99f4b6c,
          0x90e135c, 0x0639688, 0x83f6385, 0x330edb8, 0x9079675, 0xe1a5a6b,
          0xb8f5fe0, 0x6e37fa8, 0x61dca1e, 0x60e2fd9 }
    },
    {
        { 0x66c395e, 0xc6cb403, 0xb51d0f1, 0x03b21a7, 0xe693181, 0xbc478a5,
          0xc6cff33, 0x0017c2f, 0x39d8d1e, 0x740a5b8, 0x4d9ec6d, 0x3968d66,
          0xb0ef1b0, 0xfd53738, 0x1ed0a04, 0x73ca8fd },
        { 0x75ab371, 0x4ace938, 0xddad7e9, 0xd602936, 0x750bcc2, 0x1f5424a,
          0x68c7a17, 0xfe09b36, 0x58341ec, 0x165f7de, 0x6ce61e5, 0x95b825a,
          0x66c83c4, 0x9d31e19, 0xcc5887b, 0x65b3e08 }
    },
    {
        { 0x21482d1, 0xd37e932, 0x08b6380, 0x9af6597, 0x7d61e4b, 0x279426a,
          0x80997ad, 0x80dd0ec, 0xd5b76d4, 0x7239b0d, 0xe76c098, 0x92e6c73,
          0xeab3e1d, 0xeeb2321, 0xeb1a910, 0xa69c4a7 },
        { 0x833d9ae, 0x46d6aa7, 0x572b0fe, 0x3ee6957, 0xcdb3d97, 0x44ccbed,
          0xcbea01b, 0x342f29d, 0x8926876, 0x0d518c5, 0x5585d2c, 0xaaabae7,
          0xe008f58, 0xc548c77, 0x21fab2c, 0x819e2fa }
    },
},
{
    {
        { 0xc16e981, 0x468e149, 0x9ddbb7c, 0x286c790, 0xdb7a38a, 0x2a92d47,
          0x8a27cb2, 0xde614e6, 0xe5b0ab6, 0x8dc8822, 0xcf48565, 0x38441ae,
          0x089435b, 0x11ed5c9, 0x82d0d31, 0x2389286 },
        { 0x72f2f31, 0xc6698d4, 0x56d76af, 0x295242c, 0xeba563b, 0x4099205,
          0x3ab7384, 0xae7de5a, 0xd0ed86c, 0xccdf127, 0x965c3c3, 0xb9b6d5b,
          0x2c31ad7, 0xe351a8f, 0xac12f13, 0xa761dd8 }
    },
    {
        { 0xf171ab7, 0xda115dd, 0x401f93d, 0x2de17b1, 0x40964b4, 0x95019ca,
          0x65ba3c3, 0x169d1f4, 0x0090d08, 0x534a007, 0x82bf410, 0x805c5e2,
          0x65f8d90, 0x15dfe11, 0xca72456, 0x827a416 },
        { 0x33a36c4, 0x5af8884, 0xd8ee604, 0x8bfa54c, 0x9ce290f, 0x08fd141,
          0x287b3a6, 0x2db5e8c, 0x03cdad2, 0xe5be981, 0xbf810b9, 0x155b874,
          0x670f473, 0x2ae42de, 0x7f74657, 0x2218584 }
    },
    {
        { 0x23ffa43, 0x54b2a50, 0xa24d919, 0xcf87b16, 0x63524e8, 0x1ff5402,
          0x56d1e54, 0x73c94e0, 0x3899fb5, 0x7651552, 0x18723bf, 0x13a7214,
          0x3561517, 0x39afbdd, 0x9f2862e, 0x49b790a },
        { 0x527d2ce, 0xc8c1f4f, 0x7609bb7, 0x1997aec, 0x02a3400, 0x583ad80,
          0x4f79706, 0xac2374e, 0x21b7183, 0xbf1f9a8, 0x6600fe0, 0x06158ab,
          0xbd56751, 0xfcc9b2e, 0xddaaec7, 0xe1de5ac }
    },
    {
        { 0x788fdab, 0x230baa1, 0x7d04597, 0xf30860a, 0x99f4caa, 0xa2c7ece,
          0x6ad065e, 0xbd39f10, 0x3bef7bd, 0xfd92f5d, 0x96d2203, 0x6069fad,
          0xc4d9e0d, 0xbff38ca, 0x1fda313, 0x419a017 },
        { 0x572f035, 0x5d77fd8, 0xb282b40, 0x5af99f2, 0x23facff, 0x7257d3b,
          0x58c90af, 0xf2ee223, 0x9b6a52a, 0xcc2687d, 0x302430e, 0x140892c,
          0x3ec4f38, 0xa934d5e, 0x3bd18be, 0xc087d7c }
    },
    {
        { 0xa2c5ed7, 0x7e94138, 0x53610bf, 0xbc8ceef, 0xd86f803, 0xe89356b,
          0x5a55330, 0x9a3a380, 0x11ad648, 0xe894aba, 0xba95918, 0x2e68fba,
          0xfcad344, 0x643e2ba, 0x61640aa, 0x0dd0256 },
        { 0xe25cbdd, 0xc02e479, 0x13a1b3f, 0xd78c4d8, 0xcca9692, 0xa6dae8f,
          0xe5de8a0, 0x3dd91e9, 0x764ea36, 0x78ae0ce, 0x85dbc5e, 0xb4ad999,
          0xe82a169, 0x967ff23, 0xbaee1fc, 0xaeb26ec }
    },
    {
        { 0x9a6f90c, 0x8c50255, 0x0ea374a, 0x56e7abe, 0x56413b2, 0x675c722,
          0x946753f, 0xd3fc17e, 0xe235f7c, 0x28c4e1f, 0xb028eb0, 0xe209bcd,
          0x489fe88, 0x7d0f93a, 0x063706a, 0xb966a2e },
        { 0x4a30319, 0xb6c228c, 0xca6d674, 0x6868efe, 0x057311a, 0x0610a70,
          0xbad7f89, 0x0808112, 0x1dd6181, 0x2a2462c, 0xb58e88a, 0x52ed9fe,
          0x33821a2, 0xbbff16f, 0x17f882a, 0xda53e96 }
    },
    {
        { 0x8c30e5d, 0xb6ffca3, 0x5c905f5, 0xa90f991, 0xd753e88, 0x72fb200,
          0x7256c6a, 0xe509d4c, 0xd866500, 0x369e552, 0x33cf8ae, 0xee4b7e0,
          0xefcf6eb, 0x280d954, 0xd557f0e, 0x5b275d3 },
        { 0xb5cecf8, 0xeb17211, 0xbdb2f8d, 0xd6ad50f, 0x35e04b7, 0x2478c7b,
          0xac73bd3, 0x97e7143, 0x4817e24, 0x09d6ede, 0x2c405e1, 0x68fea71,
          0x05f67a1, 0x34adbc9, 0x73edf99, 0xd20ab70 }
    },
    {
        { 0x569f191, 0xe116a96, 0x4d6e29a, 0xb3f0bce, 0xf51dbab, 0x30b9e1a,
          0x346d276, 0x1dd36f3, 0x0749a27, 0x8315103, 0xab47f70, 0x242f148,
          0x5585681, 0xe8a5bcf, 0x5ed79ba, 0x8b80184 },
        { 0x3894ad1, 0xa4042fd, 0x2b88bc6, 0x82f781d, 0xbe4c397, 0x2d34cac,
          0xdd99c9f, 0x8731aea, 0xef1d382, 0x0f95498, 0xdd0bbc9, 0xcaba2e1,
          0x54064e8, 0x78889e9, 0x61a8ab9, 0x8cd9c97 }
    },
},
{
    {
        { 0xfa0459e, 0xf31f53f, 0x315cd6b, 0xf8742a1, 0xae64e97, 0xabe2f50,
          0x9b9da48, 0xbd78741, 0x51e526e, 0x4521a33, 0xe10ba45, 0xfa05935,
          0xe8f903c, 0x5c947e1, 0x5a754ee, 0x0aa47d1 },
        { 0xd814825, 0xb2849ef, 0x5c9968d, 0x9c2a5d2, 0x04e634c, 0x24dbb26,
          0xdb38194, 0x33f3a4c, 0xc8a2b6b, 0xe04f609, 0xabbbfdb, 0xcaefd8e,
          0x404498b, 0x683119a, 0x8b21cbd, 0x24ab7a9 }
    },
    {
        { 0x21fa2dd, 0x6f13269, 0xc10a4bc, 0xd79e61c, 0x4bd6d46, 0xac4b3ce,
          0xbd3f37b, 0x52459b6, 0xa396966, 0xce0f0a3, 0xa1ed488, 0x050d1d5,
          0xe0b17fa, 0x1b9c403, 0x04a2e66, 0xee1abd0 },
        { 0x5cf3e3b, 0x97065c3, 0xbe33441, 0x6513d5f, 0x79047ae, 0xcd34634,
          0xfd22df1, 0x45cbb1c, 0x967b17c, 0x7a173ae, 0x2223cda, 0x75f5ba7,
          0xefe0a73, 0xe3d12db, 0xfd7adcf, 0x3b7f94d }
    },
    {
        { 0xf1e9b7d, 0xd596a13, 0x6734e0c, 0x04f5bdd, 0x8be163a, 0x18b694f,
          0xd959fa3, 0x15620c7, 0x53d2a3b, 0x65fc2c5, 0xc4d36f2, 0xd44a364,
          0x268ceab, 0xc8b421f, 0xbfe2bd4, 0x564139a },
        { 0x19d4633, 0xb524610, 0x6346934, 0x5ab3f88, 0x9819422, 0x96691fe,
          0x8b39b82, 0xdfdec89, 0x97cfb27, 0x84b1c79, 0x4d6d004, 0xe59a98d,
          0x12c350f, 0x5e5d0c6, 0xd415774, 0xb431220 }
    },
    {
        { 0x6aae0a2, 0x3d0ca73, 0x48c2d8c, 0x7b1991f, 0x5cdae72, 0x00ae856,
          0xbd55128, 0xdbb6ca0, 0x45c82bf, 0x3c2ab2a, 0x79545ca, 0xea5a559,
          0xd5927d0, 0xeba9a26, 0x83257fc, 0xb52e401 },
        { 0xca9650a, 0x55ed517, 0xe3ebff2, 0xbdaa081, 0x9f8831b, 0x8cf7ce4,
          0x6e3b8d3, 0x1d0b5bd, 0xd8fc869, 0xa314a9f, 0xb892bab, 0x07f2079,
          0xa0cc9d9, 0xb700dbf, 0x6dc0a39, 0x7105a08 }
    },
    {
        { 0x8c7d901, 0x0c7e05d, 0xaf3182b, 0xa7ff681, 0xf9a0d06, 0xb88e3ca,
          0xc343b7f, 0xfe20a12, 0x03251f9, 0x9f02577, 0xc40c5eb, 0xf225ded,
          0xb208ea7, 0x50e0cec, 0xe6eeb65, 0x5b250f0 },
        { 0x4806b6e, 0x807a153, 0xfa94139, 0xded120a, 0x49366fb, 0x237ddc7,
          0x5a34bcb, 0xdd3674e, 0x9c4a61d, 0xef6cdff, 0xb2fb896, 0x036194b,
          0x9528cd9, 0x3865953, 0x6936a52, 0x0723c59 }
    },
    {
        { 0xe17719d, 0x1f84cd5, 0xc73b394, 0x545939b, 0x83e84e7, 0xefbf3c5,
          0xf77fd66, 0x6cc46f1, 0x1383ab8, 0xa629f59, 0xcd35cd2, 0x9177ffa,
          0x9dd411b, 0x039187f, 0x7b7eea8, 0xa9cf1cf },
        { 0xac47e5d, 0xa3b105a, 0xd0a9da4, 0xa755bea, 0x73da15e, 0x50cfbae,
          0x60b628c, 0x9456cbc, 0x9b7a910, 0x7ffc362, 0xcd6d6a4, 0x30b5924,
          0x0b04ab6, 0x198629f, 0x624dea9, 0xc74609c }
    },
    {
        { 0xaf12fa6, 0x27d4d77, 0x690aeb2, 0xdd8a216, 0xfe24417, 0xe48fc02,
          0x720e17e, 0x1970403, 0xce37b42, 0x95013fd, 0xde4bd9b, 0x06817d2,
          0x63d0ba2, 0xc5863e7, 0xa556f5d, 0xa1bafc0 },
        { 0x410a78a, 0xf28ec7b, 0x0a01a63, 0x0dcac42, 0xb5bce11, 0xfcd3fa4,
          0xd278b89, 0x054d7e5, 0x5ce49e3, 0x5195db8, 0x2c73d96, 0x4c0b167,
          0x20a1bdb, 0xd943077, 0x59c77a7, 0x66fa8b3 }
    },
    {
        { 0xd7462fe, 0xb9e93ae, 0x18dde4f, 0xbfe54b2, 0x3dbb08e, 0xaabb528,
          0x0e5fc45, 0x8c36702, 0x8e69be3, 0x3502888, 0xc12a11d, 0x6d2efc1,
          0xf265e30, 0xfce5ceb, 0x5742c7e, 0x58c8bb3 },
        { 0xccf7fa0, 0x32e89dc, 0xdd020a4, 0xa811f33, 0x5129fe5, 0xa10d620,
          0xe4ed29b, 0x3841c88, 0xd8b1ea6, 0xf3303a9, 0x1781f58, 0xa9a0cad,
          0x8f3ef0b, 0x4502b38, 0x74c6d35, 0x2b7587e }
    },
},
{
    {
        { 0x23ae7cd, 0xc6eaea1, 0x73c0caa, 0xa1884d4, 0xef1ea88, 0x901e76f,
          0xa14269d, 0xdb9935c, 0x947f1de, 0xe8b2486, 0xa657588, 0x4ad56f4,
          0x2913fb1, 0xe768054, 0x37600da, 0x2abff5d },
        { 0xa81a797, 0xa814813, 0x46acb69, 0x63e76a4, 0x4ab8277, 0xb103839,
          0x9d8e759, 0x587de34, 0xddf62df, 0xdfaeb8d, 0x9239d49, 0x24fe1cf,
          0xe130d1c, 0x7de7409, 0x581d070, 0x3ecfef9 }
    },
    {
        { 0xf87c72d, 0x8d177a0, 0x8c6d1de, 0xae7e581, 0x8cece85, 0x0077b5f,
          0x32d2187, 0x3824838, 0x6db2bd2, 0x49d8b15, 0xc8d85b9, 0xe9e5513,
          0xe05c53f, 0x63c410c, 0xd86f752, 0xceaf2fb },
        { 0x93806c5, 0x0b432fe, 0x3d06c75, 0x18eb15d, 0x12cfc02, 0xcaad826,
          0x1e2d045, 0x581e040, 0x95edcfd, 0xd573cb5, 0xdbc66e3, 0xce71948,
          0xacc14ea, 0xcf68721, 0x6cac4dc, 0xf68bea2 }
    },
    {
        { 0xcb74da2, 0xd8576af, 0xc433f46, 0x8771c29, 0xe2f5b8e, 0x7315af6,
          0xba33928, 0xc195481, 0x2fb1f94, 0xb77dcc2, 0xa610f75, 0xcb3e57c,
          0x53907df, 0xeb2a927, 0x23eff95, 0x916f149 },
        { 0xb6cd291, 0xbb378e4, 0x2f13ce1, 0xa2a5e2b, 0xbcd00b0, 0xa8a0e60,
          0x682b75a, 0x5902741, 0x3f65a77, 0xa0882c9, 0xc93cfff, 0x2069f75,
          0x70c0cb9, 0x1ede405, 0x0d526c4, 0x13840c9 }
    },
    {
        { 0x03ced48, 0xdc2caaa, 0xa0315be, 0x2079219, 0x3b1f642, 0xca49356,
          0xb0665f2, 0x0202dc7, 0xb7a5238, 0xe5d6bbd, 0x26eab32, 0x36fbd5e,
          0xf5819b4, 0xb3988f1, 0x4aa4d69, 0x5b15dc8 },
        { 0x54e5c24, 0xa52feed, 0xe91a797, 0x927471b, 0xd57f677, 0xd119bfd,
          0x78e4c4f, 0xde38f7b, 0xb150bc3, 0xa7af516, 0x26b76c2, 0x403b21e,
          0x92300dc, 0x589067d, 0x066802a, 0x04e406a }
    },
    {
        { 0xa9ca9bb, 0x28e7d09, 0xfccf4a0, 0xaa84fd5, 0x635b7ed, 0xdbe9fb8,
          0xd56fc7c, 0x9ede3f5, 0xb01cb29, 0xa4b5031, 0x7f93703, 0x584299d,
          0xb6fe825, 0xbd28868, 0x8b9c2d9, 0x1d385d4 },
        { 0x822be80, 0x6606f4a, 0x626d0fd, 0xb5a0165, 0x14568ad, 0x9920a20,
          0x1c6d174, 0x7d430f4, 0xe02e9e9, 0xc243e16, 0xa6bd649, 0x367f1d2,
          0x71b8c36, 0x6939100, 0x4de2984, 0x2ede131 }
    },
    {
        { 0x5beec32, 0xdc78187, 0xa525ff4, 0x1fff0cc, 0x676df34, 0x6e86425,
          0x3f638e1, 0x2b4e8a6, 0x9b1e59f, 0xc4991d2, 0x1589717, 0x399d001,
          0xbe041cd, 0x406464e, 0x9e65bb0, 0x901cb3d },
        { 0xfb42307, 0xf5f4572, 0xf1b7307, 0xf81b3b0, 0xf2094d1, 0x8fb695c,
          0xdb56f7b, 0x7db4792, 0x5a794e0, 0x36836d5, 0x09bc879, 0x2da477b,
          0x1887c40, 0x1cdfadb, 0xf2699b6, 0x65dc6c2 }
    },
    {
        { 0x4737972, 0x36f9f21, 0x7a387b0, 0x48f0c8b, 0x39a1d24, 0xa156ed3,
          0x0fed268, 0x375293a, 0x7ff75cb, 0xf679f48, 0x1cc9e62, 0xd15a00f,
          0x22c3877, 0x92a7dc7, 0x6fb0ed4, 0xe987063 },
        { 0x16f5f3c, 0xfd8e59c, 0xaeeb48e, 0x375732e, 0xca1ab42, 0x2dd9213,
          0x9ffccea, 0xcb06209, 0xb23edfd, 0xfc611f6, 0x99b060e, 0x2716349,
          0x820de8a, 0xb938b5d, 0xeb49a32, 0x138f6e7 }
    },
    {
        { 0xe485f70, 0x7feda63, 0xeb27b2c, 0x646380a, 0xc4511c7, 0xcf8fe32,
          0xff9406a, 0x2c68e1e, 0x20b6020, 0xa9f2fd9, 0x3b3e465, 0x1c98fc6,
          0x93e53aa, 0xb8dac35, 0xa750e96, 0x2fb47b6 },
        { 0x1950bb3, 0xea373ef, 0x4ac7aec, 0x8156694, 0xb55b931, 0x8d6b3c2,
          0xb62ef7d, 0x5d13f2d, 0xab9182b, 0x4647f2a, 0x33bf07c, 0x8f56c5a,
          0xb35a221, 0xc5ab284, 0x5a46a6b, 0x0747ab7 }
    },
},
{
    {
        { 0x86b85c5, 0x5b9236c, 0xc482448, 0x5967a0d, 0x7df6ae0, 0x397c955,
          0x5378f2b, 0xf83ee1c, 0x6e05dd1, 0xf82df65, 0x19d7c8b, 0x4c424f6,
          0xa6d5f2a, 0xa612550, 0x63c3ebf, 0xfe8482a },
        { 0x0142c82, 0xcb8d403, 0x3679e6c, 0x08b0662, 0x3eca5ee, 0x3ea5146,
          0x1370500, 0x089eb3b, 0x5a0d306, 0xcbfb19c, 0x42a65bb, 0x2f68588,
          0xe51e119, 0xe3e1db5, 0x110895e, 0x2c150e7 }
    },
    {
        { 0xf6d4c4c, 0xf323488, 0x63b87e2, 0x5fc931f, 0x35c759f, 0x8867da0,
          0x9746d4c, 0xb6f1eff, 0x990be0a, 0x8a8172d, 0x5c407b4, 0x1113eee,
          0x378ed8a, 0xd80dacf, 0x3fa7fd1, 0x99b57cf },
        { 0x5176405, 0xf5bb6d9, 0x92e83b5, 0x6b8963a, 0x8a7ef8d, 0xac55b6b,
          0x6c1fbf0, 0xe73fa12, 0x60148df, 0xdb37560, 0xf3f1fba, 0x72f1a98,
          0xea550f2, 0x1f71d0a, 0x9544a87, 0xc3ea4f0 }
    },
    {
        { 0x4322bf3, 0x5b09da2, 0x61264e1, 0x2a573d5, 0x803acc4, 0x93cb2e1,
          0xe502fc6, 0x397b4fb, 0x39e0ebc, 0xddfb212, 0xbbcbc57, 0xeccd8f5,
          0x4663788, 0x49d3bed, 0x1218df9, 0x37192aa },
        { 0x2ffa3c6, 0x8a05bc9, 0x23ebf4d, 0xc38c281, 0xfe343a8, 0xc80d547,
          0x6c63516, 0xa8d5a5b, 0x8d8fa6b, 0xc5d8ce1, 0x24a87c0, 0xeb5e872,
          0x75bfa23, 0x9806e9e, 0x689469a, 0x11f0889 }
    },
    {
        { 0x8e75666, 0x81005f6, 0xd349505, 0xb84d861, 0x9f321ea, 0xe083282,
          0xcfa33a1, 0xb751d7a, 0x067c550, 0x793cf6f, 0x1027e56, 0x073a6b2,
          0x66a6012, 0x53f40ee, 0xc210fa9, 0x70bfaa8 },
        { 0xe4b5998, 0x1518e39, 0x24b8d9c, 0x8f0b530, 0xafdf923, 0xd91c281,
          0x24e3f69, 0xc5cfb28, 0x870871f, 0x63a529a, 0x2128dad, 0x3d3e887,
          0xcb30cce, 0xed658dc, 0xafb7bae, 0xf9373b9 }
    },
    {
        { 0xde58ed2, 0x22d4dbe, 0x03f8789, 0x4fefc1d, 0x344817f, 0x6b0a1fe,
          0xa56b0b2, 0x96bef40, 0xda249fa, 0x32684ee, 0x524a91b, 0x8298864,
          0x0c736a1, 0xa958baf, 0xef2f3e5, 0xd033a7d },
        { 0x43f4d6a, 0x5be3edc, 0x9c89abb, 0x326a39d, 0x55d997a, 0x90c44f7,
          0x6e966c2, 0x2058106, 0x6548038, 0xdbae490, 0xd473fc1, 0xac7bc97,
          0x4b2603a, 0xb34488b, 0x5e9bb98, 0x27aea27 }
    },
    {
        { 0x1b88773, 0xa59e728, 0x0c241f6, 0xe2f05d4, 0x4e75749, 0xa56229e,
          0x1b10705, 0x8f00c0b, 0x19394d3, 0x8559946, 0xaaf5e32, 0x0d7e352,
          0x787b8ea, 0x526c462, 0xa179d48, 0x89297d9 },
        { 0xef43892, 0xeff17e6, 0x221f841, 0x17091eb, 0x4a4b848, 0x82f5eb3,
          0x8eb7b76, 0x6bea477, 0x76c536c, 0x21f2271, 0x96c81bb, 0xd9ef2c8,
          0x54bf4d3, 0x7c27546, 0xd7c28c8, 0x9dd4662 }
    },
    {
        { 0x20e1a6b, 0xe7fff00, 0xa08d467, 0x26a35c6, 0x3248c91, 0xb3c773d,
          0xba7d935, 0xa646615, 0xb0d26fa, 0xa91f453, 0x60c6d32, 0xdcf9c34,
          0x9e3e3dc, 0x6366861, 0xf30f3e2, 0x3012813 },
        { 0xc2fc61a, 0xac6623d, 0x2bfd2ff, 0x108dc25, 0x231d6ea, 0xd7f5c0d,
          0xad1107e, 0xa904f9a, 0x0d1e9c8, 0x46941c2, 0xc810cf2, 0xe5b6451,
          0x4f511d1, 0xaba8e67, 0x08373fe, 0x5b4b94f }
    },
    {
        { 0x849c230, 0x002d4e2, 0xd8ba391, 0x9bed0ef, 0x828e319, 0x745e0c0,
          0xca58de2, 0xcd40907, 0x1abaa4a, 0x2c87ab1, 0xdb64391, 0x3c17a97,
          0x86c72d2, 0x36b184e, 0x485f7aa, 0xb03d202 },
        { 0xde24aba, 0x2b6b79b, 0x2325fb2, 0xdcb7854, 0x66ebae2, 0xf5d1db9,
          0x903840a, 0x35a4d5b, 0x190e9da, 0x7afeb09, 0x35c1792, 0x1818f6a,
          0x3faa269, 0x90091fa, 0x2570235, 0xc4ccff6 }
    },
},
{
    {
        { 0xec85940, 0xa177619, 0x7ef7eee, 0xfca24db, 0x7a90c11, 0xb2450f3,
          0xdbf4f85, 0x29d256d, 0x51316c3, 0x920c8d0, 0x04474da, 0x2f7f7ba,
          0x2ec9a0b, 0x308117f, 0xd0d2085, 0xd0a231a },
        { 0x7ab641d, 0xf3288fc, 0x9f4fa32, 0xc68bade, 0xbbf8253, 0x768f014,
          0xc0a33f0, 0x5eff260, 0x6bb93ce, 0xc71b453, 0x680697f, 0xa71d045,
          0xce72bc3, 0xb62444c, 0xd1379f3, 0x11f03e8 }
    },
    {
        { 0xc16df92, 0x1f54789, 0xe3ed142, 0x874c642, 0xfa2a9f1, 0x6699f60,
          0x3fecfc1, 0xbd1b8d3, 0x8a3d953, 0x59682d5, 0x4a36b81, 0xf17c021,
          0x181a666, 0xeb9621d, 0x3cf1ad8, 0x7c2c3ab },
        { 0xe529f7c, 0xe6888c3, 0xb355315, 0x197b66a, 0x83e31ac, 0x63b558a,
          0x891c68e, 0x4aa7bc5, 0x592e360, 0xc17d989, 0x1363666, 0xc750a29,
          0x4909ac0, 0x0d53470, 0x4594a10, 0xd6d0272 }
    },
    {
        { 0x3fbb635, 0x35c541b, 0x5982afa, 0x50016d0, 0x96b0ca0, 0x58ebce4,
          0x577ea56, 0xb940027, 0xe38480f, 0xf29d305, 0xebd6a2c, 0x43705b0,
          0xe90c639, 0x0e4acda, 0xf56e05e, 0xbe94a29 },
        { 0x30659ad, 0xc61f4a0, 0xc402211, 0x39074ad, 0x51b621d, 0xfe0d8d5,
          0xd1d5222, 0x2d02e8d, 0x46c2683, 0x05ece3c, 0xc689d41, 0xf70705a,
          0x4d837bf, 0xe3caf44, 0x75ba6d0, 0xfda0584 }
    },
    {
        { 0xcb7d458, 0x1098163, 0xf5ba834, 0x12b645f, 0x28af72c, 0x70a3181,
          0xf32e5dd, 0x5f4727e, 0x10a21b4, 0x7cbae15, 0x6785389, 0xa80bf80,
          0xb8f93b7, 0x9827402, 0x08349da, 0xe385f82 },
        { 0x9589f6e, 0x2d05461, 0xe7c0191, 0x6aa5b26, 0xbd5574d, 0xe79ae12,
          0x4148e61, 0x5d13f91, 0x13716ff, 0x7b2be0f, 0x80bb81f, 0x82b0fe6,
          0x3e2569c, 0x697633c, 0x873f8b3, 0x6c1f083 }
    },
    {
        { 0x0be1674, 0x6e26d85, 0xab8044f, 0xe4e47f6, 0x82fc434, 0xfdf46e8,
          0xc89cadc, 0x639ae2c, 0x4b85bdc, 0x2244a52, 0xb7cf4ea, 0xb1e4790,
          0x7e0bb8f, 0x51dce03, 0x2716cee, 0xdd14335 },
        { 0x8e8841d, 0x1c049b4, 0xb97c621, 0x6bf26dc, 0xba01178, 0x21d6255,
          0x8e4f0e4, 0x477258a, 0x68f8ef1, 0xf5e437e, 0x8b03e1e, 0xd118fbc,
          0xe1c91b3, 0x3d6bc51, 0xd5b6907, 0xa259486 }
    },
    {
        { 0x7b6f5dc, 0x4159cfc, 0x493694a, 0x05a52b3, 0x83b8883, 0xeeb511c,
          0x2b06400, 0x19d79e4, 0x738f37e, 0x8e503a2, 0x5a94ad9, 0xa30e579,
          0x262618d, 0x3981c75, 0x2dcba19, 0x06b6c69 },
        { 0x4d1b051, 0xd7242ee, 0x3b350c4, 0x6274ccb, 0xf540019, 0x66df0bb,
          0x5ae12d5, 0x4d66be6, 0x1049cba, 0xcea2960, 0x8df84b3, 0x4047339,
          0x75a31c8, 0x7d6c96b, 0x874174c, 0xbb80159 }
    },
    {
        { 0x59f1aa4, 0xf0f7be0, 0xdcff451, 0x798f39a, 0x8014e1e, 0x96763ff,
          0x09cc5ec, 0x03987a8, 0x893650a, 0x4919656, 0x75e24df, 0x92e8eef,
          0xe89d639, 0x54e97cd, 0x7682cc0, 0x8081d06 },
        { 0xa8ceb71, 0xb9ef41a, 0xa4d7aaa, 0xb8173a4, 0xc54ee10, 0x93d81b1,
          0x70a445a, 0xabe1805, 0x64d569d, 0xac0ff97, 0x3e570be, 0x86946b2,
          0x4180641, 0x8e11dd2, 0x99f67dc, 0x3d0b33c }
    },
    {
        { 0x48bf5a4, 0x2c9637e, 0xccaf112, 0x9fdec19, 0x5c42023, 0xe5cde9d,
          0x878f0cc, 0x9869620, 0x1fe6eba, 0xcf970a2, 0x54e678b, 0x1df5ec8,
          0x28d00dd, 0x4667f01, 0xb0b3fa8, 0xfa7260d },
        { 0xb34239b, 0x6bd2895, 0x2d2a50d, 0x04c8bc5, 0x6cb23e2, 0x14e55ef,
          0x3a278d5, 0x6440c27, 0x2193046, 0xf4b12e3, 0x5dd4c08, 0x46adf64,
          0x4656e8c, 0x70e2998, 0xe4acd44, 0xe7b36ea }
    },
},
{
    {
        { 0x16cf664, 0xea64a57, 0x26fd357, 0x8497ee4, 0x814e851, 0x44d94b4,
          0x5a6a2cf, 0xf4aac22, 0x80c301f, 0x947b309, 0x7865383, 0xf390ba1,
          0xd1773d3, 0x16c4fc6, 0x6227220, 0x61b9814 },
        { 0x1dd0270, 0x07dd03a, 0x0f160df, 0x290ca82, 0x44ba955, 0x8f22054,
          0x0b6f1b3, 0x4e85e45, 0xad78089, 0xfd73ce9, 0x2f2cb0e, 0x67c1270,
          0xee33a61, 0xa7de0d7, 0x6553261, 0x6a811cc }
    },
    {
        { 0x2d0a427, 0x5ef0574, 0x220a341, 0xe8d2e95, 0x8044886, 0xdd28cbf,
          0xa1aa58b, 0xdad7b4b, 0x8ec901b, 0xb28f373, 0x5bbe3db, 0x1841a93,
          0xa075fee, 0x8fd7cd1, 0xc0d3cdd, 0x93b603f },
        { 0x5edd859, 0xca54fd5, 0x64ed687, 0xa4cb05f, 0xed1a3d7, 0x3138668,
          0xee32be5, 0x1224fda, 0xc80aeb3, 0xf1f532b, 0xe8d4d69, 0xa4f65d0,
          0x5905fe5, 0xc697a01, 0x6690ce4, 0x514da7a }
    },
    {
        { 0x3de4a55, 0xc7b9af8, 0xb318d93, 0xc79bad7, 0xf5b1c83, 0x1808071,
          0xb965b16, 0x92112ef, 0x7bb740a, 0x655ab38, 0x384ff87, 0x53dbc8b,
          0x72dc6f2, 0xd153c28, 0x99c7819, 0x2ec20e1 },
        { 0x3b854b5, 0x65e46ea, 0xc711db5, 0x272d5ae, 0x26e19e8, 0xfd1bb53,
          0x3dc0665, 0x33280b8, 0xb8f1c4a, 0x95b986e, 0xa685c4a, 0xa671fc4,
          0x83bdbbf, 0xa03cbd5, 0xab77544, 0xd329402 }
    },
    {
        { 0x8e62b35, 0x40fa651, 0xf9e55a6, 0x3913b11, 0x5270a41, 0x4e8089b,
          0x80d1886, 0x565f52a, 0x512749b, 0x93b5f05, 0x141c547, 0x35c869c,
          0xf86717f, 0x9a44a1a, 0x9c2b2cb, 0x2b9984b },
        { 0x4952322, 0x61fb607, 0x7af1464, 0x2d4072f, 0x600eb30, 0x9b2fa8c,
          0xf10668e, 0x6071fb7, 0x90634ca, 0x27cc24d, 0x471d32b, 0x3875bc2,
          0xa11210c, 0x678590b, 0xfcc5a9a, 0x352b447 }
    },
    {
        { 0x5fa3200, 0x795d541, 0xa92949f, 0xadaa557, 0x3cc88c4, 0x42fff06,
          0x71b68a5, 0x26d6831, 0xe67ad8c, 0x3286549, 0x86396b2, 0x5bf6363,
          0xe12c8ea, 0x41229b6, 0x748952e, 0x05320c9 },
        { 0x900b460, 0xae36b63, 0xf2b6aff, 0x9354ff2, 0x065ee0c, 0x10b810b,
          0xcc8bb38, 0x4d6925f, 0x7a22f14, 0x31c03fd, 0x57544e8, 0x76b7f44,
          0xc0eed26, 0x3a9123c, 0xe0cd1cc, 0x77acd67 }
    },
    {
        { 0x07ec527, 0x2e90530, 0x62937cf, 0x32388ef, 0xe229188, 0xa445389,
          0x33bcebe, 0xa44b68e, 0x4c4e701, 0x5a8722e, 0xcf07e41, 0xfd066e8,
          0x95fab62, 0xa3c1a4f, 0xe542f24, 0xb4d6a1b },
        { 0xaf6c9b5, 0xe6a92e4, 0xc83d61d, 0x9452484, 0x0062276, 0x422b55b,
          0x5279688, 0x261973a, 0x3999fb2, 0xde8be26, 0x7b029ca, 0x64e9628,
          0x06897d4, 0xd8edfaa, 0x6955511, 0x408319c }
    },
    {
        { 0x50a5632, 0xff6baed, 0x5c5885a, 0x922b7d0, 0x1b45864, 0xdf0f3b3,
          0xc04340e, 0x27e49c0, 0x122c447, 0x618c566, 0xeafee7e, 0x7863a38,
          0xb828cb0, 0x7143aff, 0xf9d054e, 0x51fcf4c },
        { 0x27f5e09, 0xc4a4b31, 0x90be2bd, 0x021f47a, 0x7ab956d, 0x1a06019,
          0x86ea86b, 0xe77fa15, 0xd550ef3, 0x9ccde87, 0x6532654, 0x7dee53a,
          0xe826387, 0x8b4f060, 0xad077b5, 0xda38637 }
    },
    {
        { 0x0e9fac8, 0xbc901b3, 0x6fb2a2a, 0xfa08204, 0x5e04efc, 0x92f68ab,
          0x9ac12d0, 0x184a30a, 0xb25d479, 0x1aa11aa, 0x0f03161, 0x8bc5f4c,
          0xcfc8817, 0x7e3a083, 0x597f93f, 0x84d9355 },
        { 0x239abc6, 0xc014478, 0x8d37b04, 0xb226b09, 0xf575789, 0xb056942,
          0xba745eb, 0x816b95a, 0xb98ddb6, 0x2a49d39, 0x291af81, 0xc41ca26,
          0xab26347, 0xb3afe99, 0x604b638, 0x59c31bc }
    },
},
{
    {
        { 0xc42befd, 0xa16a8b9, 0x2052f00, 0x731c9c9, 0x1f5dfa0, 0x1ad49b4,
          0xbffce36, 0x7a289e3, 0x0c79cf1, 0x868fac0, 0x86721ab, 0x6d6d284,
          0xe726c94, 0x590f928, 0x51f3841, 0x0e802cb },
        { 0x0b694bc, 0x6a6a57a, 0x8120fb8, 0xb9bb0cd, 0x9c05826, 0xad96ac7,
          0x7768df0, 0x294da8c, 0xb56c6c6, 0xfe32311, 0xae8d050, 0x291c2c6,
          0xe7db4c9, 0x1c765e7, 0xd65f9f7, 0xe058298 }
    },
    {
        { 0x7e8d345, 0x4bfa85b, 0xde1dfc8, 0xa04ef95, 0x324ace3, 0xb5f7f21,
          0x574b14a, 0x4b350a1, 0xf8e5c8d, 0x11436bf, 0x7642369, 0x1c789f9,
          0xfb623ce, 0xeb5e335, 0x442d562, 0x9deacd2 },
        { 0x531ee71, 0x4ff989f, 0xaacb52a, 0x43e2c49, 0x85bfadc, 0xa763198,
          0xd0161a0, 0x08b6d5c, 0x541f197, 0x010e3fa, 0x3279a16, 0x83a589e,
          0x6309f9b, 0xf099137, 0xf1cea10, 0x07c093b }
    },
    {
        { 0x33d2192, 0x1ce3f0f, 0xc37ce73, 0x07b559a, 0x207be27, 0xaa2ad38,
          0x7ed93de, 0x84f053b, 0x3b98a4b, 0xbc5c797, 0x63aa9b9, 0xc923461,
          0x231a10c, 0x807cc16, 0xa061209, 0x8ffdf57 },
        { 0x497070f, 0xa9ca741, 0xd113b3a, 0xf608ec9, 0x8d0384d, 0x5132726,
          0xf5ec307, 0x96686ac, 0x71c4665, 0x437bbbd, 0x7c379ca, 0xdef09d5,
          0x621747c, 0xf8be033, 0x8ae8047, 0x2775b37 }
    },
    {
        { 0xb2c4fc2, 0x4009798, 0x203772e, 0x148d7d1, 0xf8423fb, 0x9d9392d,
          0xaf8cef4, 0xa5bd72e, 0x4380b53, 0x579d58d, 0x8c39d24, 0x2ff88f1,
          0x5706466, 0x9ca2fbc, 0x1e56af2, 0xb42987d },
        { 0x5d94ea8, 0xcc2556e, 0x5369d76, 0x4e5c2b3, 0x2a94f9c, 0x5de3574,
          0x5cb4145, 0x8d068c9, 0x51bfcbf, 0x4d553ff, 0x8a23fce, 0x3ab7164,
          0xd0fa7f3, 0xc9cb3a9, 0xed9ced1, 0xf81209b }
    },
    {
        { 0xe5b66f5, 0xde7356e, 0xe8a25e0, 0x7b2bf1a, 0x2c9b725, 0x09a444a,
          0x4906c55, 0xfd8a2f4, 0x82514f3, 0x409cc80, 0x28999a9, 0x47e0099,
          0x6a312f4, 0x0a582a6, 0xf6723de, 0xf7946f8 },
        { 0x92d8aff, 0xa55f6ba, 0xa544b1c, 0xb62c3c8, 0x5c16a94, 0xa1d1411,
          0x2ad5e71, 0xc378319, 0x06b1dd6, 0x13d7847, 0xee7ff55, 0x99005f8,
          0x8a1e7d8, 0xfb5ea3f, 0xb4cac39, 0xdc7f53c }
    },
    {
        { 0x36e3794, 0x482abaf, 0xc74684f, 0xc23e9e5, 0xf1629be, 0x4544cf6,
          0x2f40374, 0xd8a8ee5, 0xf433bdb, 0x2eea87f, 0xae9990e, 0x489a99c,
          0x54b23b6, 0xefc131e, 0x8600270, 0x25fe699 },
        { 0xc059a7e, 0x03d2d9e, 0x6979c3c, 0xa6445b5, 0x9bfbcea, 0x491a10c,
          0xe937af1, 0x15b5974, 0x797c7fc, 0x4be8002, 0xfedcfee, 0xbed8a49,
          0xa9e0691, 0x35751ce, 0x9ef5982, 0xe9a9fa3 }
    },
    {
        { 0x3065de7, 0xeffeaca, 0xac4d4e2, 0x841d544, 0xcaf199f, 0x8144679,
          0x443967a, 0x98cf4f9, 0xf33183c, 0x8cd57f4, 0xc1b15eb, 0x390832a,
          0xa53b500, 0xc4b1fea, 0xdff24b5, 0xd762a10 },
        { 0xb0ee2a9, 0xccd3eed, 0x362d485, 0xa6dd4a9, 0xf1d047a, 0xeb4ff26,
          0x23860fc, 0xc0771fd, 0x4b64114, 0xdbb4e39, 0x4d29b29, 0x2ff3f24,
          0x387b365, 0x9cac005, 0xde5994a, 0x05b7aa6 }
    },
    {
        { 0xc03dd63, 0x5e71752, 0xbc74687, 0xad10fe9, 0x54c76ab, 0x51a5b0c,
          0x1f586d4, 0x763fd50, 0x816048b, 0xc7bd5ce, 0x3f744dc, 0x8fc83d2,
          0x109df9a, 0x0561802, 0xccf0e43, 0x18fb01f },
        { 0x038ab23, 0xe4606fc, 0xa664c98, 0x5878f1f, 0x5da7356, 0x3aedbbd,
          0x516746a, 0x3c578f5, 0x1a17210, 0x259477f, 0x028248f, 0xc7a869d,
          0x48cbf95, 0x6517a61, 0x3d04d47, 0xbc5f91d }
    },
},
{
    {
        { 0x083ca53, 0x15fd9a9, 0x2697ca6, 0x1161da0, 0x56b676c, 0xf516af3,
          0x75eec13, 0x8a420d5, 0x1a9526b, 0x72d6742, 0x76b463f, 0x8d8c29e,
          0x8815627, 0x38a4f58, 0xe0650f9, 0xf7e528b },
        { 0x382edca, 0x2cfa78e, 0xc4ad83c, 0x638d183, 0xe4a0119, 0x96d3b9d,
          0xa7c1101, 0x5769ccb, 0x2b8d04a, 0xc3b3b79, 0x4951bde, 0x96212f6,
          0x481161e, 0xad7905a, 0x41c5edf, 0x8fd6762 }
    },
    {
        { 0x39d6cde, 0xf7b0635, 0x115a84a, 0x69d0549, 0xcbd9fe4, 0x4a976c6,
          0x950ff96, 0xc92953f, 0x654d127, 0x1d7f0fe, 0xda0f75d, 0x7293870,
          0xcf2277f, 0x7bb3652, 0x834484f, 0x64798c9 },
        { 0xac3a76c, 0xb94d8bf, 0x7ff776b, 0xf5721a9, 0x2722e31, 0x23a6e9f,
          0x9a5c034, 0xe9da996, 0x456ebc3, 0xb9bbf83, 0x96956a4, 0x239f58a,
          0x18b7f00, 0x8b75beb, 0xa51cb97, 0x6c2b5b8 }
    },
    {
        { 0x7eb41f3, 0x78b1c62, 0x17c4352, 0x0638fcf, 0x0c5709c, 0x939edd8,
          0xedc906c, 0x0a8dfc3, 0xefb01ed, 0x3942f47, 0x49986fe, 0x4c82757,
          0x4dffa57, 0x792545c, 0x6c3ff26, 0xeee6883 },
        { 0x12b1218, 0x824d08e, 0x902457f, 0x515a478, 0xbae55b3, 0xc70cc9c,
          0xbcef9d4, 0x1240737, 0x2f9db7f, 0xf22e616, 0x91f8da2, 0x98c4f02,
          0xafaaa67, 0xa89219c, 0xe7d27e2, 0xf35fd87 }
    },
    {
        { 0x01b80d0, 0x19b0cd7, 0xf9aebd1, 0x3d7e29d, 0x0477cbc, 0xd39c9ca,
          0x5ff0d3d, 0xac0f615, 0x520fd01, 0x8a51993, 0xb22d6fb, 0x508ff54,
          0x318d3ab, 0x8786c47, 0x4a683f8, 0x4312c46 },
        { 0x95359f6, 0x73b1d39, 0x963011e, 0x0d94fa5, 0x9bfe83e, 0x5723af2,
          0x6841df3, 0xafa9001, 0xb7c498a, 0x791e92a, 0x7ea4253, 0xbc931ad,
          0xb783c06, 0x438e016, 0x2ca662b, 0x1347db2 }
    },
    {
        { 0xfbaa861, 0x41df37d, 0x329e4de, 0x98ecb23, 0x507e018, 0xdaf1560,
          0xb088e32, 0xa902269, 0xe4cab2f, 0xad898a5, 0x02c1e1b, 0xd84e9ed,
          0x8488af3, 0xc20a5d5, 0x6cc77c6, 0xc7165af },
        { 0xdeb7461, 0x8526f3a, 0x4a2d332, 0x03577b1, 0xe4760b5, 0x28e469d,
          0xb276266, 0x442c7f9, 0xf9c90fa, 0x90d5c77, 0x3e211bd, 0x7aa8716,
          0x5decfd6, 0x56d8ff0, 0xee23e6e, 0xa204b56 }
    },
    {
        { 0x4aceafc, 0x2e4374e, 0x6fcd5e5, 0x978743b, 0xc4855ca, 0xa0f6345,
          0xe98074b, 0x9bc7e4f, 0xc33d08a, 0x3835d57, 0x6f00566, 0xeec7c8b,
          0x1acf55c, 0x71628a2, 0x97fb19e, 0x5da3750 },
        { 0x01a7125, 0x6904a8e, 0xe6e3780, 0xad33c85, 0xc19f94a, 0x1702928,
          0x7c04b3d, 0xb424ff2, 0x19e2ba3, 0xb212e39, 0xc9af4c9, 0x4cca8e8,
          0xfd9bf0e, 0x98ab7ae, 0x9799db5, 0x21d245d }
    },
    {
        { 0xec08806, 0x6b034dc, 0xb40f2d9, 0xfd763f2, 0x29cb906, 0x5e16de0,
          0x8a0e16a, 0x02b7014, 0xe071e12, 0x463c8ee, 0x25ad509, 0x6447281,
          0xdc0e07a, 0x9ee6f2d, 0x68d4d97, 0x188895c },
        { 0xb27f971, 0x092fff3, 0xc9b7722, 0xb3c159f, 0x3cae42d, 0xe27d8ff,
          0xe87071d, 0xf8a5ed6, 0x607ebd2, 0x318388f, 0x53486f1, 0x924967b,
          0x7c46e1f, 0x7730494, 0xf21d196, 0xf279c60 }
    },
    {
        { 0x84f3201, 0xef2bc03, 0x1f94c51, 0xf8750c7, 0x986ec65, 0xbaa4f5a,
          0x2732a33, 0x6f8a5de, 0x299e365, 0x0f13d80, 0xe85261f, 0x2709530,
          0xf527d56, 0x097d922, 0xbe1f3f8, 0x4969687 },
        { 0x3e1708d, 0x9f3f504, 0x4aa4be4, 0xac67b87, 0x320a87e, 0x75fb042,
          0x6e2cad6, 0xa361ad3, 0x203e9f6, 0xcb01470, 0xc9b76c6, 0xe3807b7,
          0xb907c09, 0xf086833, 0x7e85a01, 0xe9bed3c }
    },
},
{
    {
        { 0x91780c7, 0xa7ea989, 0xd2476b6, 0x04e4ecc, 0xc494b68, 0x0af9f58,
          0xdee64fd, 0xe0f269f, 0x021bd26, 0x85a61f6, 0xb5d284b, 0xc265c35,
          0x3775afd, 0x58755ea, 0x2ecf2c6, 0x617f174 },
        { 0x5ec556a, 0x50109e2, 0xfd57e39, 0x235366b, 0x44b6b2e, 0x7b3c976,
          0xb2b7b9c, 0xf7f9e82, 0x0ec6409, 0xb6196ab, 0x0a20d9e, 0x88f1d16,
          0x586f761, 0xe3be3b4, 0xe26395d, 0x9983c26 }
    },
    {
        { 0x6909ee2, 0x1d7605c, 0x995ec8a, 0xfc4d970, 0xcf2b361, 0x2d82e9d,
          0x1225f55, 0x07f0ef6, 0xaee9c55, 0xa240c13, 0x5627b54, 0xd449d1e,
          0x3a44575, 0x07164a7, 0xbd4bd71, 0x61a15fd },
        { 0xd3a9fe4, 0x30696b9, 0x7e7e326, 0x68308c7, 0xce0b8c8, 0x3ac222b,
          0x304db8e, 0x83ee319, 0x5e5db0b, 0xeca503b, 0xb1c6539, 0x78a8dce,
          0x2d256bc, 0x4a8b05e, 0xbd9fd57, 0xa1c3cb8 }
    },
    {
        { 0xd95aa96, 0x5685531, 0x6bd51ff, 0xc6f1174, 0xc9c2343, 0xb38308a,
          0x2921841, 0x52ee64a, 0x78f3b01, 0x60809c4, 0xae403ac, 0xe297a99,
          0xcb09a5b, 0x7edc18f, 0x81ac92a, 0x4808bcb },
        { 0x34dc89a, 0x3ec1bb2, 0x4e39da5, 0x1e8b42e, 0xe526486, 0xde67d5e,
          0x76f0684, 0x2376548, 0x285a3dd, 0x0a583bd, 0xfe9b009, 0x3d8b87d,
          0x0413979, 0x45bd736, 0x38a727f, 0xb5d5f90 }
    },
    {
        { 0x4bde3ee, 0x7b8820f, 0x24d5170, 0xea712ef, 0xdf6ec7b, 0x517f88c,
          0x983ea9a, 0xb15cecf, 0x31a4592, 0x9eeee44, 0xebb013e, 0x786c784,
          0x1f4e15d, 0x2f06cb3, 0x4f4fda1, 0x5603fd8 },
        { 0x9e1321f, 0xf6790e9, 0x74a4c09, 0x274c66a, 0x9a41a4e, 0xa4b70b4,
          0xada5157, 0x7700bdd, 0x51be8dc, 0xe54a60d, 0x1a477e0, 0xfaf9276,
          0xb027eac, 0x6661c72, 0x280b917, 0x50e2340 }
    },
    {
        { 0x96ec123, 0x635f40f, 0x7a766a4, 0x4a33133, 0xb935587, 0x9ce4416,
          0x95d97e4, 0xbb6e1f5, 0x9d4197d, 0x2614723, 0x490e896, 0xabd4478,
          0x8bba895, 0xf6a1b2a, 0x5e27a45, 0x401fa40 },
        { 0x0620900, 0x7354ba5, 0x385678b, 0xc443a29, 0x53cf5fa, 0x48aba10,
          0xbbe152d, 0xd67e723, 0x2a63d68, 0x4b858e0, 0x72be4ee, 0x174e1ee,
          0x9ab8d46, 0xad0fbb3, 0xce17dd7, 0xa0fdffb }
    },
    {
        { 0x9c46fd8, 0xa1ea325, 0x9fb96ef, 0xeca122e, 0x6767acd, 0xf9074a2,
          0x2787082, 0x9b004a2, 0x7f3ba8e, 0x389f807, 0x0d5aabe, 0x6463de9,
          0xb090585, 0xf30ceaa, 0x5634ab8, 0x71b31e8 },
        { 0xaf02aed, 0x0dee65c, 0x20ac252, 0x506886e, 0x86b8a59, 0x0665f78,
          0xf2bb328, 0xb9b784d, 0xdc6b089, 0x46e443a, 0x66c27fd, 0x3d5de19,
          0xf0fde70, 0x0419265, 0x2b5c034, 0xed94612 }
    },
    {
        { 0x13b0056, 0x5a52ad2, 0xb909ee3, 0x9fbeb92, 0xbdaab08, 0xb42ba18,
          0xffc8a77, 0xec127c4, 0xfda906a, 0xc6d2985, 0x994bbe7, 0x5355547,
          0x9cdfd62, 0xa7470c0, 0xd2e675a, 0x31a3971 },
        { 0xcc8b356, 0x8d8311c, 0x01b4372, 0xabb0bf8, 0x0294566, 0x33c1cad,
          0xe07b672, 0xe2e649c, 0x2ae3284, 0x9084d88, 0x1835ce2, 0x7a90d4c,
          0x809d44c, 0xb4d1cd5, 0x9f0528f, 0x7822714 }
    },
    {
        { 0xbf5844b, 0xca884cf, 0x8524cf9, 0x9dd05c4, 0x36ba889, 0xdbffa19,
          0x29e7666, 0xef94fdd, 0x3eaf48f, 0x358f81b, 0x1530d56, 0x96734d5,
          0x4adf9e5, 0x378b2d1, 0x4731f61, 0x2f85046 },
        { 0x99dcb83, 0xd6ae905, 0x6199239, 0xa4f89e0, 0x8f0f958, 0x6405249,
          0xcc27707, 0x2866d99, 0xf551c0f, 0x64681a2, 0x4c37080, 0x2c7b0d0,
          0x00ac301, 0x218925b, 0x54df895, 0x8d57fb3 }
    },
},
{
    {
        { 0x809c8d7, 0xdaebde0, 0x0e95ea1, 0x58c761c, 0x00ae5e2, 0xbd99650,
          0xcd51acd, 0x6117a85, 0x7c55d56, 0xc4424d8, 0xdfbeeaf, 0xe9b1dde,
          0x0db4791, 0xda98bb5, 0x3fca108, 0xff3a5a6 },
        { 0x5ccbea1, 0x172fb8e, 0xa9f6cc9, 0x9fe12a7, 0x8967ce2, 0x1de4b0b,
          0x671dbc6, 0xc1ab60f, 0x5dedcda, 0x338385a, 0x3a043fe, 0x647a420,
          0x28ebc89, 0xe9abc64, 0x03ba3c8, 0xc357ff0 }
    },
    {
        { 0xde39ebd, 0x37061e7, 0x2be567a, 0xebb9135, 0xd6bb80a, 0xa9a6f6b,
          0x99f0ba2, 0x039345d, 0x98bbf47, 0x215494e, 0xa2a1ccb, 0xf2cb7a4,
          0x37f67c9, 0xf51aa10, 0x17fff71, 0xd29c85c },
        { 0x4d30b87, 0x8d4e4f2, 0x93a8309, 0x20fdf55, 0x757075c, 0x9b9f9cf,
          0xcd70101, 0x09142ad, 0x766ca55, 0x901d0ee, 0x32e418b, 0x6a5d86a,
          0xd7fcaec, 0x550ad92, 0xd91b26e, 0x64e8818 }
    },
    {
        { 0x47e5ee5, 0x5cea0f7, 0xbe99699, 0x8ca1d31, 0x5c136c7, 0x52db846,
          0x90e0d74, 0x8cecb38, 0xede2ad8, 0xb8efe9d, 0xf17ade8, 0x18d6ff8,
          0x2d66c20, 0xd222735, 0xf2005fd, 0xc46593e },
        { 0xf7141e1, 0xe5ebe6f, 0xe0126f2, 0xc968315, 0x1cb91b6, 0x95adc73,
          0x38a6003, 0x753b54c, 0x4230a61, 0xa614125, 0x559fece, 0x23ac6eb,
          0x3865c23, 0x9816b60, 0x543a570, 0x567014e }
    },
    {
        { 0xdd2b71f, 0xd46091d, 0x97d24ff, 0x3999a5d, 0x1ecff3c, 0xce2a4f1,
          0x581c6f0, 0xab2687c, 0xcba70b4, 0xa9fb2eb, 0x42093e1, 0x6fde356,
          0xaee724a, 0x00253ec, 0x2b81bdd, 0xa08ce3c },
        { 0x935a2b3, 0xa251238, 0x584f750, 0x8cae1d4, 0x988a219, 0x011469e,
          0x5a6a50e, 0x61f7ed3, 0x01fcebd, 0xe13ebaa, 0x31d8867, 0x794b976,
          0xcda32e7, 0xf25755c, 0x4564cd1, 0x368a97b }
    },
    {
        { 0xaa3397b, 0x0d22224, 0x38066db, 0x1dbb3e6, 0x0ce8e32, 0xfe0b5ee,
          0x7bab4dc, 0x09c17c8, 0xf188b64, 0x5cc65dd, 0x211b5fa, 0x74c4abf,
          0xab0ba86, 0xdcc17b7, 0xa535501, 0xfbdf46f },
        { 0xaca569e, 0x4775087, 0x06a1718, 0x6575f90, 0xb94de93, 0xb5c45a9,
          0x8497171, 0x0fc8006, 0x489f7ab, 0x775d965, 0xf5c0c89, 0x8775b58,
          0x1a06254, 0x05d4e20, 0xb6d73a5, 0x8cab349 }
    },
    {
        { 0x39465b0, 0xca78163, 0x14498fd, 0x3ef9148, 0x6255c11, 0x9ca1f34,
          0xb7f38f1, 0x389fd15, 0x354b8f3, 0xdac2089, 0xa840a70, 0x82d07fc,
          0x1dd483a, 0xf53fd73, 0x1590578, 0xa6e4eae },
        { 0x3c01b77, 0x7bf65af, 0xa75c982, 0x27542f3, 0x716cfce, 0xc5bd947,
          0x884b9e7, 0xba5fe76, 0xd55725d, 0x39bae14, 0xfae0eab, 0x982f64e,
          0x7a5293a, 0xcfae662, 0xd60f464, 0x22a25a1 }
    },
    {
        { 0x7dd5e16, 0x74caecc, 0xce7bca3, 0x23678a2, 0x57f1ba1, 0x4673932,
          0xa4c1697, 0x4eb9948, 0xeaba18d, 0x5d400e8, 0x9807871, 0x128d1c8,
          0xbff38a6, 0x78f9627, 0xa39d4cc, 0xf80b813 },
        { 0x31d3aad, 0x8aeefa0, 0x27db664, 0x5042199, 0x4cb6383, 0x244fc69,
          0x72192a3, 0x3190477, 0xbbfb57b, 0xcc86075, 0x4451511, 0xbae3a13,
          0xf6174f0, 0x16cf416, 0xd376813, 0xb343cc0 }
    },
    {
        { 0xd1824b7, 0x31ac9b9, 0xec8f61a, 0x6282260, 0xc781765, 0xbbeb9f8,
          0x2d110da, 0x06ab5c0, 0x47146b8, 0xd583e22, 0x4100d05, 0x79a1608,
          0xf0a5c95, 0x16dbbb4, 0xe331667, 0xfe2af1d },
        { 0xaf8710e, 0x26f0364, 0xeec08fe, 0x1cb8c91, 0x1d95e9f, 0x436bce6,
          0x57944a0, 0xfe9050c, 0x07b626b, 0x5f45acf, 0x9cf1276, 0x48dc93f,
          0xa05bfb7, 0x4491371, 0x4bcf785, 0x5106304 }
    },
},
{
    {
        { 0xed0b3b6, 0xac2e294, 0x671637b, 0x5c5ade6, 0x1140677, 0x2f289ce,
          0x754eb53, 0xaf446e2, 0x20421ad, 0x70911b7, 0xe0b7556, 0x4b73836,
          0x2a97827, 0xcadf104, 0x8005bc6, 0x4824e49 },
        { 0x937c28a, 0xb0eeccd, 0x0c3ee97, 0x1ce061d, 0x9f33faa, 0xcb07631,
          0xaea66dc, 0x9980bf4, 0xd111d98, 0x2bd0755, 0x7fe4de0, 0x43feaf6,
          0xb077b2f, 0xe76fb80, 0x5793b04, 0x227dc9f }
    },
    {
        { 0x14f49ba, 0xea24ae5, 0x11436e7, 0xbc39ea6, 0x78485d8, 0x9d7fed2,
          0xdf8b131, 0xb6ef00c, 0xfdbc7af, 0x0237b4b, 0x64ccd27, 0x08745b5,
          0xafc5a76, 0xaf8595d, 0x29f5500, 0x43657af },
        { 0x48470f8, 0x3007183, 0x640fd53, 0x51f91fd, 0xbe15512, 0x859c807,
          0xab3e9c5, 0x7d1a474, 0x81553e5, 0x5d714d9, 0x6f62310, 0x0757343,
          0x6b02a62, 0xedc5be0, 0xea47832, 0x5a4b9b7 }
    },
    {
        { 0xe93dbb3, 0x03e0a24, 0xcadc884, 0x25841dc, 0x8d10ad5, 0xabc1a81,
          0x2042ddd, 0x207e38a, 0xfeba8d8, 0x7fffbdb, 0xa3ec9b5, 0x74efebb,
          0x0b40a9f, 0x0bc39ca, 0x0267feb, 0x69ee9c9 },
        { 0xbc62919, 0xd402fac, 0x1cf53c6, 0xe9f8fc1, 0x7cc7d81, 0xe76fa5a,
          0x96bb19d, 0x4f2d876, 0xadc67c7, 0xd4fb7f9, 0x96702dc, 0x40621d5,
          0x438f6c5, 0x5b6a98e, 0xf1a1036, 0xa7c64de }
    },
    {
        { 0x9a092c7, 0x84c5e80, 0x11c22b7, 0x9e40e0a, 0xd06c99b, 0x820a091,
          0xeecca8f, 0x45fdc77, 0x5794f16, 0xfe1b8a3, 0x4ce3d6d, 0x31f7e5b,
          0x82c74c8, 0xfd5e010, 0xc1f6f7d, 0xfdabf30 },
        { 0xb9248a0, 0xbfa6017, 0x546b941, 0xe898d30, 0x207ff65, 0x878c492,
          0xb874e64, 0xbf22e8d, 0x53a547e, 0x43fdb1b, 0x5fbd464, 0xb66deda,
          0xc7ae1b5, 0x59127a6, 0x6a7515a, 0xa463646 }
    },
    {
        { 0xde9ab2e, 0x22c4e66, 0x0203c58, 0xfaf60c2, 0x0d5c5ed, 0xed2d7bf,
          0x4ca0f19, 0xdbc16fe, 0x465b979, 0x54e8ef6, 0xa310ef9, 0xe2d64b1,
          0x3778636, 0xa0f2c95, 0x281883b, 0xf3b4aa4 },
        { 0x9be6629, 0x4ac9af0, 0x1ca90c5, 0xba455e1, 0x856f492, 0x0147538,
          0xabd7840, 0xc80db7e, 0x6beb9cd, 0xb3526d9, 0x9d81503, 0x37657fb,
          0x193cec3, 0x8729a16, 0xd69952a, 0xd9a93fb }
    },
    {
        { 0x94f47c6, 0xfce0175, 0xe366d05, 0x228da21, 0xdc8baf3, 0x27ce0b2,
          0x6b4a951, 0x8cc660b, 0x384bb01, 0xf678947, 0x44d980c, 0xc629d7d,
          0xe85e81f, 0x47980e4, 0x1cd723e, 0xa2e636a },
        { 0x77fb207, 0x6b6ebae, 0x4c92891, 0x7017961, 0xb4d279c, 0x5569541,
          0x41758cb, 0xbb6b36a, 0x27a8e30, 0xecaa222, 0xb470ad9, 0x8b6746a,
          0x63e2d3d, 0x4c46017, 0xd3edaec, 0xe19c4ed }
    },
    {
        { 0x34718c8, 0x0b43fec, 0xf33499f, 0x553c407, 0x970d1db, 0x8272efb,
          0xa8e8d1c, 0x008c62c, 0x63eec45, 0xe4b79d7, 0xf2d71a3, 0x1fd4230,
          0xa368c36, 0x090fdaf, 0xfca7baa, 0xf62c101 },
        { 0xd2395b3, 0x1c9e6c8, 0x04c5513, 0x671ed63, 0x299a465, 0x577d933,
          0x63f9986, 0x286890e, 0xbfc979c, 0xd92a95d, 0x2b51019, 0xcebd79d,
          0x3d07251, 0xe74d88b, 0x906f9ad, 0x8b6db73 }
    },
    {
        { 0x7b3d90c, 0xc0c43db, 0x4304a06, 0x85d154e, 0xaf2f38e, 0xe8aceef,
          0x83d9459, 0x5e04293, 0x431afd1, 0x65e5e32, 0xa900a65, 0x9e5f050,
          0x8a26671, 0xcbaa171, 0x9c93de7, 0x33d0b24 },
        { 0xd5b6680, 0x3dcbf92, 0x20006f9, 0xc47e5ec, 0x9a51924, 0xc971129,
          0xcd0ed46, 0x665d9b8, 0xa5fcab6, 0xed2d63f, 0xcfbfc5a, 0xa817eb6,
          0xb76eb76, 0xb38169f, 0xf11160b, 0x8b93544 }
    },
},
{
    {
        { 0x693bdcd, 0x02eca52, 0x2ae01d6, 0xbbf0923, 0x8b44b3e, 0x0b0a2de,
          0xb250dff, 0xdb82449, 0x6e1c530, 0x0c42b86, 0xa64c2c4, 0xcd226dc,
          0xf046b5f, 0xcfb2bb1, 0x3fccb0d, 0x97e2fae },
        { 0x45ed156, 0xdf92907, 0xf641229, 0x224dcb9, 0x5f1f67e, 0x2126abc,
          0xe9c8a6b, 0xa7eed5a, 0x9857d9b, 0x40abedc, 0xde941c6, 0x3f9c7f6,
          0xd725ddf, 0x2158d42, 0x8c69543, 0xbdd1015 }
    },
    {
        { 0x8df2fbc, 0xa7dd24e, 0x13d1aee, 0x3adbcfd, 0x13b2177, 0xf6a32d1,
          0x7a9a14c, 0x89a7232, 0xdc65df9, 0xe3aef43, 0xa64d74c, 0xeaec3e3,
          0x4fec33b, 0x4d387d8, 0x21a2128, 0xaba2a05 },
        { 0x6b85e30, 0x2382c22, 0xcd2aad3, 0x4352d85, 0xd9772c4, 0xb0c6001,
          0x5f3653f, 0x7ed8263, 0x0300f47, 0x3626a6f, 0x6ca7e4e, 0x23909de,
          0xc154141, 0xb43dd81, 0x7e4bc68, 0x9a49fad }
    },
    {
        { 0x2428f88, 0xa3661df, 0x56e0db2, 0xbe48b02, 0xce79aa9, 0x3cd1871,
          0x23dddac, 0x90ab871, 0x71871a6, 0x9c58fb9, 0xa34910e, 0xf031f7f,
          0x81060e4, 0xb501eea, 0x791224e, 0xdb668ba },
        { 0x6a705bc, 0x240bbcb, 0x2d1865e, 0x7e76fbd, 0x2513641, 0x6e2cd02,
          0x46365c9, 0xe6c5225, 0xa5a01fb, 0xe46a8b8, 0xb67618b, 0x696fa7b,
          0x0db6792, 0x418b3b9, 0x7108b9c, 0x7204acd }
    },
    {
        { 0x8456b45, 0xb5a143b, 0xf53b4d9, 0x8a3ab25, 0xe13a570, 0xb112a58,
          0x81487d2, 0x613ca32, 0x3b1e7c9, 0x837d823, 0xd41e9d5, 0x592bade,
          0x5cd02f2, 0xdc1893a, 0x8972e23, 0x0879502 },
        { 0xcb76261, 0x7003c08, 0x332a5e0, 0x14bde9e, 0xcbbd78e, 0x14b2872,
          0xde238e8, 0x5594061, 0x067466c, 0xad12645, 0xf5e4952, 0xa8d0e64,
          0xc7f8d06, 0x5b44b82, 0xfb1b828, 0xb51bea8 }
    },
    {
        { 0x3f0dacc, 0xebad685, 0x1cbebbc, 0x5c31b8b, 0xfa5a2dc, 0x6746975,
          0x31d9faa, 0x2d95965, 0x00fc0e4, 0x343797d, 0x55fe01b, 0x38d821c,
          0x7323aa0, 0x0bfdb24, 0xf962a8e, 0x42613c4 },
        { 0xe134bc0, 0x599a211, 0x47a7084, 0x75fa4a1, 0x7f734b5, 0x6e71948,
          0x6dfca2b, 0xd5ced2d, 0x8aeabd2, 0x9fa0fdc, 0x12361da, 0x5e6b03f,
          0x5859fcf, 0xad23d31, 0x25a5fc8, 0x3120ef1 }
    },
    {
        { 0x8e9f638, 0x990ef62, 0x626a60c, 0xfdaa240, 0x2abddab, 0x4a3de20,
          0xd8872b2, 0xd5d10b7, 0x1ea5880, 0xa01b730, 0xa81b9d8, 0x481697f,
          0x3471ed8, 0x2984153, 0x292d37c, 0xefd73f8 },
        { 0x9994beb, 0xdda7626, 0x6a4f865, 0xa037703, 0xe5b47d5, 0xda992ec,
          0xe53edba, 0x912a427, 0x9264e45, 0x6467598, 0xaf71222, 0xd3b68c3,
          0x6dedc5f, 0x9d3436c, 0x076b2ad, 0x1e027af }
    },
    {
        { 0x4382f4a, 0xd56fca1, 0x8966b7b, 0x83712a4, 0xa4c9ddb, 0xd6b2cf5,
          0xf602875, 0xa66be29, 0x894f3d0, 0x70e4266, 0xb3195ca, 0x007d220,
          0x82c74d4, 0xba38d8f, 0xd975cbd, 0xdccc5fc },
        { 0xc88b38b, 0x03e1610, 0x52e0d8d, 0xeb9f9a1, 0xb646eb7, 0x6a57eca,
          0xc76b6c1, 0x161641f, 0xbd2e12b, 0xf9025ad, 0x5c0e26d, 0x87c74db,
          0xbfeca74, 0xed5cb51, 0xe34a08c, 0x603dfb6 }
    },
    {
        { 0xcb03307, 0xc4be728, 0xc2741cc, 0xde34c0e, 0xa74eb17, 0xe01db05,
          0x8905e4b, 0x1bfce0c, 0xd1b1826, 0xb18830a, 0xe87bbfb, 0xcacbb41,
          0xd2f1a79, 0x8696842, 0x08c83ea, 0xa80e5fb },
        { 0x3f1439c, 0xe48f163, 0xcd6987b, 0xc1d4108, 0xb751814, 0x05705c4,
          0xc1c622d, 0xa9bffd0, 0x46cd053, 0x23de4af, 0x39457c3, 0xf782f5e,
          0x5e5d243, 0x815276b, 0x6161ae3, 0x3132041 }
    },
},
{
    {
        { 0x77f2542, 0x2459661, 0x8372b25, 0x203be7e, 0xee2007b, 0xc7c9426,
          0x0621799, 0xc564138, 0xc28c3ce, 0xda56589, 0x7afc1e3, 0x13e8a7c,
          0xe352082, 0xdba81e9, 0x04435c7, 0xf430549 },
        { 0x691de4a, 0x4d26533, 0xfb777ab, 0x364408c, 0xeae7f88, 0xccdfb43,
          0xa525b11, 0xbc40f44, 0x3c60627, 0x8e112a5, 0xe17e696, 0x7f7c581,
          0x1ea774a, 0x0fd7878, 0x0b1f582, 0xd09e632 }
    },
    {
        { 0x70aab15, 0x44390bd, 0x889c3f2, 0x41112bc, 0xd685349, 0x6b02894,
          0x5584dfe, 0x7103001, 0x1ba7887, 0x373cb1b, 0x2a017c7, 0x53d286c,
          0x3c81fdc, 0x2ed0388, 0xfbcc6fc, 0x3bfc5e3 },
        { 0xfd6418d, 0xd38ac6f, 0xbfad89e, 0xc667e96, 0xeab4d66, 0x46f4f77,
          0x0911293, 0x194c04f, 0x68c48d5, 0x0fd09cf, 0x63cf7f4, 0x6f5b055,
          0xacd562f, 0x0c0a8c4, 0x36d965d, 0x94c1d83 }
    },
    {
        { 0xcaa127a, 0x94fc8f0, 0xd803690, 0xc762d5d, 0x1ebf0d3, 0x8bfdfd1,
          0x48eac50, 0xa98cdf2, 0x8b5ff10, 0x3d7365d, 0xc65b4de, 0x20dc29b,
          0x8ec7c68, 0x62ac28e, 0x90372d2, 0x7f5a132 },
        { 0x3246658, 0xf3d8a25, 0x9ac202a, 0xa4bebd3, 0x5cc1697, 0x078ede7,
          0xc8fc022, 0x5525800, 0x5fae77b, 0x302a802, 0x57917b6, 0x0180139,
          0x864bf55, 0x7c8806d, 0x12f06f1, 0x4e2d878 }
    },
    {
        { 0x3d66e88, 0x8d35118, 0xa91d02a, 0xfb861a1, 0x7850e5f, 0x8c27c2a,
          0xa5496f6, 0x9fd6399, 0x8080049, 0x52152ae, 0xfd1c2dc, 0x600e2ff,
          0xffe8b2e, 0xc75902a, 0xe03b175, 0x5c4d2cc },
        { 0x4f57e78, 0x8ad7c42, 0x1736f87, 0x77cf606, 0xf85038a, 0x2876012,
          0x1b97b95, 0xff32845, 0x392dfc8, 0x3cc6dd5, 0xa6f5075, 0x72f1363,
          0x71de894, 0x028ec44, 0x6f45a86, 0x7030f2f }
    },
    {
        { 0x9695817, 0x66400f5, 0xf20ea36, 0xeda0a7d, 0xd394992, 0x855be51,
          0x8336f62, 0x2d082c1, 0xf28c868, 0x30944dd, 0x0dc86d0, 0xfb5f853,
          0x564a0bd, 0x9562ae5, 0xb6b9b51, 0x1f7ea12 },
        { 0xd0a7148, 0x5bd74e0, 0xb91e572, 0x6c8247f, 0x47da498, 0x699aba5,
          0x1f7c814, 0xed82581, 0x62057b9, 0x434674b, 0x15c15b4, 0x8b4df5e,
          0xb110081, 0x2a97da1, 0x4c417fe, 0x2a96b0c }
    },
    {
        { 0x237639d, 0x4f75dfc, 0x1db7029, 0xe5ad6bc, 0xb3d28f7, 0xd43e06e,
          0xe447989, 0x89f3bb5, 0x01a1a6e, 0xc426a2c, 0x315878f, 0x33ea71c,
          0xb1b5705, 0x8a7784a, 0x77ca811, 0xa59e86e },
        { 0x36ae155, 0xddb133c, 0x0d51b42, 0x49f1d4c, 0x9d05519, 0x5508082,
          0x5291816, 0x20e23be, 0x67181ec, 0x35047ec, 0x7aad091, 0x6237dc4,
          0xe2e25a2, 0xa1d3ce1, 0x0d3db4c, 0x1de0522 }
    },
    {
        { 0xd9fd423, 0xe9a5e19, 0x9801e43, 0x0c2c3d0, 0x28df2da, 0x043c2dd,
          0xe1ad12a, 0x4eecab4, 0x9615aa5, 0x97e1797, 0xca7bb5e, 0xe57b879,
          0xcc92619, 0xa2a903c, 0xaa56e93, 0x5cef370 },
        { 0x7f3232c, 0xbef29fa, 0x2b7ad5c, 0x1cf35ed, 0x3b6077a, 0x35c4893,
          0x7a1d47d, 0xe065148, 0xce14572, 0xedb4673, 0x0b17629, 0xdc9e98c,
          0x9a02a5c, 0xef98ebe, 0x11d03c0, 0x1f772e3 }
    },
    {
        { 0x4608f72, 0xcbdbdcd, 0x5a13c6f, 0xb435223, 0x4bb3c21, 0xa6497f6,
          0x12c15c9, 0x3af2383, 0x6322d11, 0xfbbf4b3, 0xc641775, 0x520a5c6,
          0xe81e0e1, 0x18cd967, 0x3de3871, 0x980b2c6 },
        { 0x9ae44a2, 0xfa9db61, 0x176bc56, 0x0281dd2, 0x8a7f817, 0xfd03711,
          0x4129b30, 0x9c48545, 0x039626d, 0xb439648, 0xe4ada6b, 0x355050e,
          0x7f5d98c, 0xc9c16d6, 0x18c4d5e, 0xf53ccc3 }
    },
},
{
    {
        { 0x3ffb20b, 0x50ae942, 0x6865eb4, 0xa6c0b42, 0x09930f1, 0x4677f7d,
          0x4a16427, 0x742e0b6, 0xf976f9a, 0x521d18e, 0xa454749, 0x43ac9cf,
          0xc51f50d, 0xda3a91d, 0xad6f954, 0xf657029 },
        { 0x6b4f99a, 0xfe5f064, 0x63ad4ce, 0xd92a5d9, 0x2e0e081, 0xfcb5509,
          0x8d8a858, 0xadc85ab, 0x0632f0f, 0x8e9b966, 0x8d7216d, 0xe7a4f16,
          0x59c3b99, 0x00a4cc5, 0xba09dc1, 0xed6d0bd }
    },
    {
        { 0x1621beb, 0x7236d14, 0xbc7ca95, 0x1751fd4, 0x2f5319c, 0xaa619d1,
          0x4e9316f, 0xfc2b15b, 0x9fd4d33, 0x2d1a906, 0x8ced829, 0x28c3bac,
          0x1dd998f, 0xf2efab5, 0x3b149ed, 0x2c13330 },
        { 0xf601ac6, 0x65237c9, 0x07d6a45, 0xb54dd65, 0xfb1a4cf, 0xa1ce391,
          0x115f67e, 0x2957533, 0x465279b, 0x6456da8, 0xa993e02, 0x02890aa,
          0xb7175e4, 0x6891853, 0x0f3e59b, 0x3fda203 }
    },
    {
        { 0xd8c6e0b, 0xe99fe12, 0x5341c56, 0x7cb07ff, 0xdf77b24, 0xc292c7b,
          0xca29906, 0xf52dfd0, 0x772f02c, 0x4a6aa26, 0xe1bbd09, 0x26f7684,
          0xee7c2a8, 0xec56b2b, 0xad4a312, 0x67709e6 },
        { 0xc570263, 0x99c57b2, 0x2faafae, 0xeb0100b, 0xff25eca, 0x980d5d1,
          0x82cf936, 0xace35e6, 0x44679ed, 0x5a82ce5, 0x074b81e, 0x5c76a41,
          0xa00abb1, 0xf36fa43, 0x04ffb2d, 0x0642819 }
    },
    {
        { 0x04bdd28, 0x68f6bc8, 0xb5dc7ad, 0xc311d96, 0xed32e45, 0xff0d646,
          0xe0f712d, 0xaf3cdc6, 0xd483861, 0xd4508e9, 0x0e1c277, 0xb624be5,
          0xc5dd841, 0xc510275, 0x298dc02, 0x451c5c3 },
        { 0xdd34d6b, 0xf87d479, 0xdd06a38, 0xda7f293, 0xb699e9f, 0x575e129,
          0x215b2cc, 0x79e5fb2, 0x657e690, 0xd280028, 0xe702a71, 0x7fecd09,
          0xfa13677, 0x85160ab, 0xce65f64, 0x5de3427 }
    },
    {
        { 0xe8fff38, 0x84e4bf6, 0xb358b1c, 0x16f3725, 0x3b472a5, 0x360371c,
          0x52f217a, 0xe64c061, 0x0501241, 0x8e67379, 0xab2dd96, 0x88e81d6,
          0x1385604, 0xf3e218a, 0xe84184d, 0x9736caf },
        { 0xdbb93a3, 0xb55a043, 0x9301088, 0x335088f, 0xb2a4959, 0xcea7a2d,
          0xb882c33, 0x48e5d4a, 0xad46179, 0x114f09b, 0xb446576, 0x4416467,
          0x34c6c2f, 0x01cb23e, 0xa02db8a, 0xddebf04 }
    },
    {
        { 0x9bde8a1, 0x36d60cc, 0x676e4ad, 0x20fd2f2, 0x8936581, 0xebdcfb7,
          0xdbfc2c3, 0x245d0d5, 0xa9f82e5, 0x104c62c, 0xd654d9b, 0x7387457,
          0xae7f10e, 0xe966777, 0x1d8e582, 0xefeb16f },
        { 0x70364b5, 0x4faf4f1, 0xd612472, 0x0e1ab58, 0xfed6085, 0x11bbfe7,
          0xa59a09a, 0xb360a14, 0x722fdb6, 0x61d96e9, 0x94068bd, 0x16a12f1,
          0xf73c2be, 0x225bf07, 0xc8bd24e, 0x1e64665 }
    },
    {
        { 0x3698c75, 0x27a478a, 0x6202aa2, 0x778ccd3, 0x8d87f1f, 0x0149c63,
          0x784edae, 0xa660e5f, 0x82adfa8, 0xe0d4d2f, 0x1ba1f9d, 0xf512dd6,
          0x6245c58, 0x90cfed9, 0x18b53dd, 0x6c3a548 },
        { 0xbdc094f, 0x833f70c, 0xb1514e7, 0xa5f26f5, 0x1c8cf13, 0x93e7cf5,
          0x186ec43, 0x1436601, 0xe78170a, 0x81924ac, 0x8694368, 0xcc880a0,
          0x0b62cbb, 0x2dfa955, 0x96b4a2c, 0x0bc6aa4 }
    },
    {
        { 0x3561aa2, 0x5157a7e, 0x8645c1e, 0x525c500, 0xce7cbb3, 0x22feb4e,
          0xc89a58b, 0x36d0d25, 0xc9bde9c, 0x43131f7, 0x881f731, 0x74afdda,
          0x7c8e36a, 0x99ab87c, 0xc1d4fb2, 0xf07a476 },
        { 0xbebc606, 0x1b82056, 0xfcf089f, 0x95a1e5a, 0x2b55d5c, 0xc5bccfa,
          0x00eb0b1, 0x8fbc18e, 0x9efb483, 0x93a06fe, 0x2d74c57, 0xcafd725,
          0x3de4350, 0xc7518f0, 0xc6fd762, 0x9a719bf }
    },
},
{
    {
        { 0x2362087, 0x5ee0d83, 0x0b167e8, 0x7f2c0d7, 0x5e0e865, 0xb732789,
          0x98c4e65, 0xef5b2e8, 0x8fe9cc1, 0x222797d, 0x82d1e15, 0xfe6d73e,
          0xf62dc4b, 0xc7c0e9c, 0x937ceda, 0x962acfe },
        { 0xc1e85c7, 0xd763711, 0x2836978, 0x8f2dbbc, 0x8c44e98, 0xbadc055,
          0xa3e93f8, 0xed63eab, 0x41b55c7, 0x807e857, 0x6d1207b, 0xd51ae5e,
          0x39d541b, 0xa0ef9a6, 0xa0c56a5, 0x58855f9 }
    },
    {
        { 0x213091d, 0x7d88eaa, 0x45b6a0d, 0xcbdfee7, 0x4f5e077, 0x826a012,
          0x90f1e4c, 0xb04fc13, 0xaea69aa, 0x1961ac3, 0xd5bb63e, 0x3afb719,
          0x4ac7e5c, 0x2a37837, 0xc50ca45, 0x78efcc1 },
        { 0xb8abdef, 0x346e8f0, 0x88095d0, 0x27e3dbd, 0xffc6c22, 0x56d3379,
          0xfa4b291, 0x67d416c, 0x3b1b373, 0xc3baaf6, 0xdf73bae, 0x0184e1f,
          0x9167528, 0x38ae8f7, 0x35d6297, 0x7329d4c }
    },
    {
        { 0xf568c52, 0x45d2ac9, 0x9808593, 0x5134814, 0x31b7ed8, 0x0c92d83,
          0x0876ecd, 0x921327a, 0x052736a, 0xf752d75, 0xbc6b837, 0x7b56487,
          0xa23b4cc, 0x6b1a320, 0xec0d665, 0x1983937 },
        { 0x08554ab, 0x2c3017c, 0x366e87f, 0x40ad955, 0x8ed7f02, 0x88c4edf,
          0x3cc5e6d, 0x64a7db1, 0x2dc978b, 0x5ac91fa, 0x925d2a2, 0x016a20d,
          0xabb57b4, 0x3604dfe, 0xd7e2e85, 0xc3683ec }
    },
    {
        { 0x4c0c6d0, 0xc47150a, 0xe22adcf, 0x30af45e, 0x022ea4b, 0x39b5acb,
          0x77203b5, 0xfbe3185, 0x6fd9b59, 0xe5aaa34, 0xdd1c8dc, 0x0062c90,
          0x54049ac, 0xcf113f3, 0x63a31b5, 0xd8fba4d },
        { 0x1056a69, 0x73b5488, 0xd780bda, 0x3be6cbc, 0x30ba2b9, 0x5776ec2,
          0x8e8d6f7, 0xbe883cf, 0x5c2be6f, 0x64efe94, 0xf1ade8d, 0x064f704,
          0x743110e, 0x41cfd17, 0x4c20abe, 0xaac9411 }
    },
    {
        { 0xf1c1468, 0x91f9192, 0x4563e13, 0x8176e74, 0x0bda15d, 0xa48b5f9,
          0xda42af6, 0x2a085ae, 0x425c018, 0xfd38ab2, 0x08abafb, 0x2884ba4,
          0xcbd091d, 0x356f318, 0x817871b, 0x454e450 },
        { 0x8ada531, 0xe080e81, 0x3152ba8, 0xa40f1eb, 0x0c38eb1, 0x051049f,
          0xbd45003, 0x37e4bb3, 0x54a01e5, 0x6d09804, 0xeeb824a, 0x6de932f,
          0xdc93481, 0xccdef37, 0x93a05e8, 0x8633e07 }
    },
    {
        { 0x034675c, 0xbe94256, 0x08db789, 0x376c01d, 0x9af1b6b, 0x8707ee7,
          0x11bfbac, 0x633b3ef, 0xd06db60, 0x694f33f, 0xbb13407, 0x2a68bfc,
          0xda27c3a, 0x1c860c9, 0xd701ac3, 0xbca16de },
        { 0xc59ffd0, 0x2b76cfa, 0x54d718d, 0xf9a1165, 0x67f0878, 0xf86a1db,
          0xaf34e85, 0xe313e05, 0x3343159, 0xa188811, 0x0bb7ed1, 0xdbe4c3f,
          0x0c732bc, 0x73b67e8, 0xe74110e, 0xa4e1c87 }
    },
    {
        { 0x5c6770c, 0xce1106b, 0x5c0bcb7, 0x422c70b, 0x8195e7f, 0x32a3990,
          0x1ccd4aa, 0xa24968d, 0x720e557, 0x8f08ecf, 0x54bcc81, 0x5da10a4,
          0x6cd846e, 0x9d3c73b, 0x368d065, 0xaeb12c7 },
        { 0xcf9fd1b, 0x2110859, 0xee2bd6d, 0xd2a4801, 0xe9466ac, 0x376e556,
          0x3b5aa35, 0x767803b, 0xb8a89ba, 0x343f842, 0x6726bbf, 0x3263cc1,
          0x25871b0, 0x26caf17, 0x41b8578, 0xef66ad6 }
    },
    {
        { 0x638068c, 0xc9f2249, 0x1ccf9af, 0x96d282c, 0x69b435a, 0x71df30c,
          0xcb9d5c9, 0x88c943a, 0x2a8f378, 0xbf98ef1, 0x114c6ff, 0xffc1824,
          0xd52e8c7, 0xda3ad2c, 0x1afcb59, 0xf1222bc },
        { 0x0ee334a, 0x459e94b, 0x421933a, 0xd4477b8, 0xa1e401e, 0x60fb7b0,
          0x0d1e330, 0xfde6e82, 0x3233fde, 0xcecfe9b, 0x2e93523, 0x09ec466,
          0x30775b9, 0xa5ba649, 0xadf80f2, 0xcc397e5 }
    },
},
{
    {
        { 0x4ddc8a8, 0x2fe182d, 0xac056bf, 0x88d6e79, 0x0e41e4e, 0xc3ff2d1,
          0x2c3679f, 0x32ec7f9, 0x4e61051, 0x3561f09, 0x6c6250a, 0x4553f5a,
          0xdd25c5b, 0x2b765ef, 0x6a1cd7f, 0xe3a40a2 },
        { 0x5d821dd, 0xb27309b, 0xc2c17ca, 0x950fb8d, 0x8fb0d4c, 0xfeed015,
          0xf550179, 0x762c479, 0xe095840, 0x306cf44, 0xd379e66, 0x84b413a,
          0xbb2e4f1, 0xd6e5d5a, 0x94b085d, 0x8bc12b7 }
    },
    {
        { 0x04b5532, 0xc0d4cb8, 0xb9940a6, 0x7a31525, 0x68c69d1, 0x010e7dd,
          0x2a18c35, 0xd81f29d, 0x3f11e73, 0x08ae770, 0x6e55106, 0x5358f87,
          0xc960ef5, 0x299e8ca, 0xacfc8dc, 0x89a6fb4 },
        { 0x6dc7d4a, 0x5996a40, 0xe51b96e, 0x21e5112, 0x09a202b, 0x95b8c3d,
          0xd441f1f, 0x306ab0f, 0x98d4245, 0x2834fed, 0xd0abbde, 0xc29c387,
          0xb805c15, 0xf6a9bf1, 0xc4e458d, 0x602f4f8 }
    },
    {
        { 0xe5a893a, 0xf041486, 0x8934327, 0x53b891d, 0x4000758, 0x11e000d,
          0x662bad9, 0xa4ccde8, 0xb9a1b64, 0xe34d3ed, 0x84e7a6d, 0x72d9675,
          0x6627be4, 0x773da2f, 0xe835ae3, 0xa11c946 },
        { 0x650bc15, 0x02e8203, 0xe58b78d, 0x2d35936, 0xf21a3cc, 0xe9cfbe8,
          0x1049222, 0x55ad831, 0x38fff47, 0xbf99de4, 0x3831db5, 0xebbfd80,
          0xaf2af42, 0xe990636, 0xb7f5a0e, 0xc26ae52 }
    },
    {
        { 0xfa8f846, 0xb5d85b1, 0xb3b1455, 0x4166489, 0xd36a305, 0x768260d,
          0x4ff5645, 0xc6a8235, 0xd6e93e5, 0xd241cd8, 0xa406e74, 0xeed9aa1,
          0x5f600d9, 0x9e96ab0, 0x6eca2a1, 0xa26b8b5 },
        { 0xd705aef, 0x78321cf, 0xc0161ec, 0xc4fb6b3, 0x5199cf1, 0xdc32441,
          0xd0a5067, 0x33627d0, 0x15143ee, 0x13490cb, 0x85b4f44, 0x77e0ede,
          0x394b165, 0x904f12e, 0xefab32d, 0x90f50f5 }
    },
    {
        { 0xbc2de96, 0x4aa0a16, 0xaa9c12b, 0x172596a, 0x60e8a29, 0xd512e1e,
          0xf637e83, 0x77d35c1, 0xd2aae0b, 0xbb0d141, 0x8c03738, 0x8a878a5,
          0xab0e525, 0x6d24c01, 0xf760887, 0xb7d3136 },
        { 0x3f91b7c, 0xdbc3f8f, 0xa8722c0, 0xe7b4bca, 0xda0ae65, 0x3286a91,
          0x225b084, 0x8372274, 0xae1886c, 0x5884cd5, 0x3a23cf7, 0xb4e63ef,
          0xf2dd0da, 0xfe5f202, 0x653916c, 0x951fac9 }
    },
    {
        { 0x854fa4e, 0x05e2e8f, 0x1edaf10, 0xf411f94, 0xa0a928d, 0x26cc562,
          0x4abce65, 0x78fd34e, 0x98a32e2, 0x1d87609, 0x4c37518, 0x85dc76f,
          0x00e8021, 0xdcaeef5, 0x4e9b2a5, 0x7fcb2f8 },
        { 0xf382c06, 0x9eba91e, 0x24cae53, 0x2052e85, 0xf5c1519, 0x617336e,
          0xb4e632b, 0xf1546d5, 0xd7b8ffd, 0xa9edc81, 0x29ab68c, 0xdb2914f,
          0xdebbaba, 0xe805070, 0xc3b719e, 0x775e53b }
    },
    {
        { 0x065256a, 0xa40e294, 0x8fb031a, 0x9f11386, 0x059667c, 0xac03af8,
          0x0475f58, 0x432eb3a, 0x01faad0, 0x22332bf, 0xbc57a11, 0xc8132e9,
          0x3bc3f8b, 0x27d5a17, 0x930bf3e, 0x5471fc6 },
        { 0xe6bff40, 0xba28bc0, 0x555e564, 0x198d57e, 0x9c65b8f, 0x13ce831,
          0x5681b51, 0xb0a5c9d, 0xdeb9e11, 0x467588b, 0xbb4250b, 0xf1891a7,
          0xd12b433, 0x10b938b, 0x24dcda4, 0x0b8c802 }
    },
    {
        { 0xcf332d3, 0xc428703, 0xf2a5b98, 0x9d0053c, 0x7838a15, 0x4e4c620,
          0xfbf8a43, 0x2e92919, 0x21cd9a5, 0x39ad524, 0x1561588, 0x584ed6c,
          0x17a95c8, 0x20af305, 0xb70e1c8, 0xa223077 },
        { 0x2fa4871, 0x679cfea, 0xac633c7, 0x54f2a46, 0x4cdc5f1, 0x6030651,
          0x75a1dc7, 0xc4facda, 0x2d07d19, 0x710a288, 0x6b44992, 0xd55864e,
          0x454c5b2, 0x44d4b6c, 0x72f9981, 0x2855d28 }
    },
},
{
    {
        { 0xc7b0674, 0x4071b3e, 0xf8794d5, 0x800eb14, 0xbe6783e, 0x70573af,
          0x7785901, 0xafaa440, 0x405f32c, 0x112d2a1, 0x169b3e2, 0x3761a52,
          0x842a366, 0xe168b31, 0x9bf4734, 0x5bc322f },
        { 0x976c4a0, 0x36ef240, 0xfea4e64, 0x066f3d6, 0xa989e57, 0x0e954bd,
          0xf9466e4, 0xe36ef5e, 0xbeb9226, 0x6bb615a, 0x3d5a2ca, 0x5571e5f,
          0x4897a86, 0xa86efe2, 0x28a9f77, 0xed7e9cf }
    },
    {
        { 0x1f82c68, 0xdf10c97, 0x3b597e6, 0x796ba1e, 0xe718cbf, 0x1ac77ec,
          0x410eac8, 0xc8175bb, 0xbc555ef, 0x0cdf9a1, 0x7524e05, 0x6b889f1,
          0xae26d82, 0x6bf1e61, 0xd2e97d9, 0xb3f6ad5 },
        { 0xf226487, 0x94dcff9, 0xbe03dde, 0x60e6356, 0x6a3dd7d, 0xda1f93b,
          0x79ca90c, 0xf1be721, 0x1e6bce5, 0x05ed313, 0xd48af3e, 0xcf50908,
          0x61e554f, 0x3b0e85c, 0xa2778d3, 0xfe7e35b }
    },
    {
        { 0x75ac5a9, 0x42c5032, 0xda062c2, 0xa66a66d, 0xcaa7023, 0xa4f4f82,
          0x64b4f86, 0x489d476, 0x97311ad, 0x10b1088, 0x177b2ec, 0x55dd637,
          0x9a267b1, 0xa5ccff0, 0xff327b0, 0xf07690b },
        { 0x2250cd2, 0x39162ed, 0x8b255f1, 0x1426de0, 0x1bdd731, 0xf227afd,
          0xfa4c844, 0x78f8a36, 0x157379c, 0x267a211, 0xcc04acb, 0x3f05f92,
          0xfc69cae, 0x374496c, 0x16ebfec, 0xbf2c5d0 }
    },
    {
        { 0xd0518d1, 0x605418b, 0x9e1cbc6, 0x3237f80, 0x286c019, 0x37a7005,
          0xb15af0b, 0xf1fb0e0, 0xaa853c0, 0xfc3b97c, 0xe6beba2, 0x1f48bd0,
          0xe6a72f1, 0x8e5d7c5, 0x26ebf0c, 0x575e66d },
        { 0x62eae3d, 0x0994776, 0x96c9c65, 0x53f074f, 0xb81bade, 0x6cfbfdb,
          0x3fed7d1, 0x98b4efe, 0x38c3382, 0xdaa1123, 0x47b8ec6, 0xdf88b73,
          0x9504a4f, 0x9b0fe4b, 0xf30c1c3, 0x2e7df4c }
    },
    {
        { 0x2fc1833, 0x25380cb, 0x18d62de, 0xb8e248c, 0xd82f9db, 0x91c8f59,
          0x2444750, 0x5ec2b20, 0x66b6f74, 0x3f3a1f7, 0xdd7d14d, 0x0180aa9,
          0x2956b9c, 0xd0a342d, 0x7139873, 0x26e910e },
        { 0x139e23d, 0x2261dc4, 0xb8343dd, 0x7edb181, 0xb4038dd, 0xfcf1073,
          0xa3bfea3, 0x88870ef, 0x64a263e, 0x4e98ba9, 0x70811f5, 0x3c6e5dc,
          0xf86055d, 0x17d28f5, 0x66e4199, 0xca9c276 }
    },
    {
        { 0x964ef8c, 0x0b2d8bd, 0x88e2ba6, 0x5a99b85, 0x04498ce, 0x9e927b2,
          0x756eb25, 0x9ff20c5, 0x3f27736, 0x97cc27b, 0x4729583, 0xf32dd6d,
          0x0381a94, 0xbdc2658, 0xef2c06f, 0x70fef15 },
        { 0x49252cc, 0x50a6191, 0x236b4b9, 0x9eb4a14, 0x8e00f78, 0x9b1b215,
          0x6ea9c23, 0x27add36, 0xc3a8e79, 0xef61763, 0xd82ce56, 0xed4542f,
          0x0caed75, 0xa8737e7, 0xd452d76, 0xeca0ac2 }
    },
    {
        { 0x3d082d0, 0x20c0779, 0xc9e9f3b, 0x6e3ce64, 0x75a195f, 0xb3a4dce,
          0xbdd9f24, 0x3a3c305, 0x8688942, 0xe2545c8, 0x080f32b, 0xa463c82,
          0x42686b8, 0x4429748, 0x7213866, 0xf50e20d },
        { 0x3826e74, 0x265ac52, 0x228e8ec, 0x26fba57, 0xe6b3ed8, 0x8a1e1db,
          0xf0fe65a, 0x7c7b278, 0xc395234, 0x9a6df23, 0x0b0f114, 0x9956206,
          0xef90837, 0x440c8c4, 0x3645f65, 0x21ad22a }
    },
    {
        { 0xedd31b2, 0x1e023a6, 0x9ff8668, 0xf76d145, 0x17b45c8, 0x9707056,
          0x1e88e37, 0x0612078, 0x922faac, 0x85c51c8, 0x22756d9, 0x4df392e,
          0xa03c98e, 0x8907fd0, 0x52ea51c, 0x626f46a },
        { 0x486c8a2, 0xf8f766a, 0x88ed18c, 0x8c499a2, 0x3c4f0de, 0x44d2dc6,
          0x6f2a0b6, 0x47dde68, 0x4a973fd, 0x9a655f8, 0x786ac80, 0x3e7124e,
          0xe8a0574, 0x699e61c, 0x31cdd0d, 0xdf0ba9a }
    },
},
{
    {
        { 0xd73e69b, 0x76270ad, 0xc67d38a, 0x991120f, 0x9469f0c, 0x7be5830,
          0x7db40ac, 0x93aba59, 0x822fc08, 0x2b707bc, 0x69551cd, 0x4199fc0,
          0xf367324, 0x38deed4, 0x2228787, 0xca518e1 },
        { 0xd9a9277, 0x72f1bef, 0xe49ae90, 0x57d4aab, 0xdb23478, 0x13810d5,
          0x9b4b77f, 0x2a8b780, 0x1b4e004, 0xb542f4e, 0x3ec77f0, 0x4080fd0,
          0xcec6596, 0xb49e9fe, 0x3f16037, 0x20338d3 }
    },
    {
        { 0x53554b0, 0x4adcdae, 0xe04c4db, 0xfea4906, 0x7748233, 0x0808bec,
          0x47148d7, 0xde7477c, 0x03da38c, 0xdd9124c, 0x25ee8e9, 0x6b25031,
          0xb0d6161, 0xae67399, 0x82203b6, 0x70c4acd },
        { 0xd31dae8, 0x9683916, 0x1ac7f69, 0x3477503, 0x988e4ad, 0x9553153,
          0x53a15e1, 0xb58f411, 0x92ba2dd, 0xb65a2d4, 0xa90169c, 0x7c3efb1,
          0x6b1747d, 0x210f45e, 0xcff488d, 0x16e8d1b }
    },
    {
        { 0x9d703db, 0x252adf8, 0xfdfeb39, 0x259ac1d, 0x115e806, 0x7faf6af,
          0xc1aff21, 0x7aaefd6, 0x7c0113d, 0x8054210, 0xe19b4b1, 0x481f1a5,
          0xfcc8c61, 0x7c17d43, 0xbb0bbbe, 0x8b04452 },
        { 0x4cebae1, 0xe51e5f5, 0x56a414c, 0x05341ba, 0x7fb8a30, 0x0083a2c,
          0x77f4952, 0xb4663f2, 0x4bb0074, 0xce72eec, 0xa3584d1, 0x74fdd66,
          0xb02e076, 0x6b9e58e, 0x3b961f4, 0x5be45d5 }
    },
    {
        { 0x1ab2e0b, 0xc7474f3, 0xf4bf454, 0x2838ccb, 0xf3c3eac, 0x634392e,
          0x137602b, 0x440e40a, 0xd1ae8e3, 0xeea67e9, 0x77e221e, 0xafdf93a,
          0x2719a10, 0x3c9f3da, 0x32c8256, 0x466ecef },
        { 0xf9c432f, 0x1061c19, 0xb1c7d98, 0xa1332d9, 0xa425c2c, 0xbc735f2,
          0x4b1bccb, 0x1429cdf, 0x6bbb5f9, 0x77b42a1, 0x5955ae4, 0x30078e3,
          0x21cc315, 0x8acd777, 0xe86fa99, 0xaa90d5f }
    },
    {
        { 0x721115a, 0xfcfd460, 0x08269b8, 0x6a7de3e, 0x96dd47e, 0xe5964a6,
          0x8dca975, 0x6717cd5, 0x98b149e, 0x7ea4ebe, 0xb7b8057, 0x6f894d5,
          0x7f30e31, 0xbd6f960, 0x23df092, 0x61ca453 },
        { 0x9d782f3, 0x32241f9, 0x2abfae2, 0x55173b0, 0xd15bbbd, 0x0abe0ed,
          0xb438abb, 0xb6d3c0a, 0x9ffa20b, 0x62fb467, 0xd31560a, 0x30926b5,
          0x2a0aa6d, 0x44bf27c, 0x1a4cb97, 0xf747313 }
    },
    {
        { 0xb0535de, 0xa2f6c0d, 0xc855166, 0xcb02ae1, 0xb3422f0, 0xc699e6b,
          0x281ba8a, 0x774febe, 0xffabcc7, 0x1d9d24f, 0xfe12ba5, 0x0b31ba1,
          0x13d0af7, 0x4c86803, 0x2f47160, 0x90640d3 },
        { 0x5876603, 0xa0c4bf4, 0x950ab08, 0x717f6fa, 0xa710de8, 0xf12bb53,
          0x6a88f50, 0xc500c61, 0x2645351, 0x0070f99, 0x2446893, 0x57aab5d,
          0xb68f657, 0xd553fa8, 0x693c55d, 0xe8537c1 }
    },
    {
        { 0x7fc7684, 0x58e86eb, 0xbfc73a9, 0xdf330f7, 0xcc11936, 0x41e337d,
          0x6e35759, 0x36d9200, 0x3500d8b, 0x0132703, 0x9483354, 0xfa68405,
          0x667851b, 0xc8f2980, 0x18296b0, 0x538ec89 },
        { 0xcff55f9, 0xa2a2c4f, 0x60d20bd, 0xb260d4d, 0xd9cc59f, 0x3ed576f,
          0xd514fcc, 0x4ed8c64, 0xc22b315, 0x37ebfb2, 0x94c212c, 0xca67a36,
          0x3a1795e, 0x4f8e08c, 0x4e7261f, 0x498f926 }
    },
    {
        { 0xc59b3d4, 0xfea7382, 0x3f2925f, 0xb9942ed, 0x8ea77e8, 0xe4b00dc,
          0x3cab02e, 0x74a18ec, 0xef16d0b, 0xbbbb752, 0xffab032, 0x639da4f,
          0x3aa30f0, 0xc371a4a, 0xcaa175b, 0x8e26b22 },
        { 0x7e2b62e, 0x94e4156, 0x25a794c, 0x7cceea6, 0x479f015, 0x931d2f4,
          0x90b25b2, 0x946183d, 0x68a2807, 0x1504e97, 0xfa49ddd, 0xa7577d3,
          0xdd48699, 0x24fc87e, 0x3d7d99c, 0x9edefd6 }
    },
},
{
    {
        { 0x0f0b450, 0x0508b34, 0xc36f7f4, 0xe0069a5, 0x2a5a761, 0x2655664,
          0x848e04d, 0x0193fd8, 0x73fe2e7, 0xc108cf5, 0xfd787d4, 0x05eb0ec,
          0xff28985, 0x1555ccb, 0x651b995, 0xb5af09f },
        { 0xe1134be, 0x167d72c, 0x57c669a, 0xd6d98bf, 0x6dd76fa, 0x40fb716,
          0x2a41b31, 0xeabbf20, 0x09b75b0, 0x300ff0e, 0xd9a0c1e, 0x32b6fad,
          0x65a80e0, 0x8051883, 0x32110fe, 0x8bef693 }
    },
    {
        { 0xbef47d4, 0x637802f, 0x2d16eaa, 0xfac114b, 0x0415644, 0x7b3f3ab,
          0x2dd895b, 0x17ab8d1, 0x87195f3, 0x271b7fe, 0xa71f65f, 0xa3f867e,
          0xc80583a, 0x39ba40c, 0x56e1fcc, 0x6db0672 },
        { 0x06662a8, 0x4feab4e, 0xc74bd46, 0xc857415, 0x732b126, 0x18032ed,
          0x7a099ea, 0x87c8aea, 0x36fe0a8, 0xb4a7535, 0x27673f6, 0x33a98da,
          0x2b8e549, 0x3e40c02, 0x9a4c587, 0x2def1af }
    },
    {
        { 0xa8c9ad9, 0x9618b68, 0x49defda, 0xd70b4aa, 0x5f788ef, 0xae8b138,
          0xdd523f4, 0x87c3542, 0x5c5b004, 0xe42c705, 0xfa7df57, 0x6303360,
          0x5f6d068, 0x33e27a7, 0x8ff331a, 0x9b3268e },
        { 0x23ee0c3, 0x845cc96, 0xac80084, 0x003af70, 0x530c41d, 0x6a9f931,
          0xbb127f0, 0xa1d7051, 0xca36245, 0x642ce05, 0x0323ee9, 0xc34205b,
          0xb7b3513, 0x7cc8912, 0x076cbdb, 0x6252cc8 }
    },
    {
        { 0x7089522, 0x10e68a0, 0x58fc658, 0x36c1361, 0x74723a4, 0x490397d,
          0x519d56c, 0x42692c0, 0xf1ff235, 0x69d251b, 0xc2cbf37, 0xe689d03,
          0x825b7f4, 0xf04ceba, 0x2281c2e, 0xd6b9bee },
        { 0xe0043ab, 0xc52ef3f, 0xd1d1be8, 0x351bf28, 0x0f18a5a, 0x277615f,
          0x5d6800f, 0x31f717f, 0xab922e2, 0xf5fb82d, 0x2d6ae43, 0x99aee2f,
          0xc63b982, 0x42477fe, 0xa594a01, 0x904aeb1 }
    },
    {
        { 0xeb39974, 0xaa82174, 0x95e6aa0, 0xbc38e61, 0x25c0675, 0x6a3df8a,
          0xffbe739, 0xf324203, 0xa3f0649, 0xfa5a0b4, 0x7a7a6b8, 0x79c8732,
          0x40ad3f5, 0xeb65ecd, 0xe4e45c5, 0x718d416 },
        { 0xe2326fd, 0x029dbf4, 0xe7942f0, 0x0c63416, 0x6f4e678, 0x6d0c728,
          0xa138601, 0x59f0b10, 0x8d92ea9, 0x8a1d978, 0xc22eca5, 0x9f8d712,
          0x7b6b96b, 0x7397044, 0xe6fb955, 0xa2d49ee }
    },
    {
        { 0xbf14a19, 0x249f900, 0x63a8cd2, 0xd3522da, 0x86964d2, 0x28a32f3,
          0xc1fa743, 0xacf712b, 0x0bb94d3, 0x98a9bfc, 0xbc06824, 0x318ece1,
          0x4fce7f0, 0xfc47675, 0xe4135b7, 0x19caec9 },
        { 0xc6817bb, 0x6de68a8, 0xf3b6d89, 0x7121960, 0xf5a818e, 0xa7d4261,
          0x9157455, 0x0c0ba51, 0x450d5ff, 0x78b6acf, 0x4e8649a, 0x198b493,
          0xfd05da3, 0x0941a3c, 0xdb55951, 0x264ea4a }
    },
    {
        { 0x46e5a31, 0xcfee91c, 0xfff7366, 0x47b6806, 0x5df849d, 0xdb14be4,
          0xac66cc7, 0x3c5e22b, 0xa5f4769, 0x7f3f284, 0x383be36, 0x4e00815,
          0x8072b0b, 0x39a9f0b, 0xc7eadd6, 0x9887cd5 },
        { 0xb659511, 0x7dd8f05, 0xd2e1cb9, 0x15c796d, 0x0d31345, 0xe5edb0c,
          0x6939c60, 0x2025df0, 0xbf15de1, 0x6314c08, 0x04c7fb5, 0x03c1548,
          0xbb5d3ed, 0x413337f, 0x477e983, 0xfc20b40 }
    },
    {
        { 0x5db0ef9, 0x7f96880, 0xe9c2a70, 0x05562de, 0x7dae133, 0x071e5bc,
          0x237fc4a, 0xa8cdd12, 0x4ea492b, 0x6d565e7, 0x381ee52, 0xa17cf94,
          0x9f5c546, 0x6ab8a4e, 0x40288ef, 0xbb642f3 },
        { 0x5df5c2d, 0x64e5921, 0xbb906f4, 0x43696e3, 0x74ae46c, 0x73a841a,
          0xc506b8a, 0xe264883, 0xa1be548, 0x9542e1a, 0x5e81b4a, 0x8938539,
          0xeaca6ce, 0x5642cfa, 0x806e0f9, 0xed8077b }
    },
},
{
    {
        { 0x7e13597, 0x1c776c4, 0x9e584fd, 0x0ec8b28, 0xb8b61e8, 0x0bb6043,
          0x9cd835b, 0xdcc1748, 0x39fef9a, 0x493e6ac, 0xd133e17, 0xb44eb34,
          0x71cb6f9, 0xfebcd00, 0xd20eff2, 0xe6cf543 },
        { 0x0a004c7, 0xf265cad, 0xd35cc12, 0x9b06c9d, 0xcb4ea53, 0x769f985,
          0x0993434, 0x29160a2, 0x8d939c4, 0xdf8dd10, 0x6711e2f, 0xefa177c,
          0xcd7a2cd, 0x1695790, 0x77f6642, 0x38da3d7 }
    },
    {
        { 0x6307b74, 0x9bfcfd9, 0xbfdabc3, 0xc26a36d, 0x4abe28e, 0x9341be0,
          0x73d1387, 0xdb20b52, 0x3d1949c, 0xf8d229c, 0xb8b3a41, 0xf1e0afe,
          0xed565d0, 0x29c60df, 0x8b43b2c, 0x6930bb5 },
        { 0xfc0718f, 0x1d76527, 0x1f67189, 0xdb98143, 0x51f32cc, 0x0c62f64,
          0x8bd35e5, 0x70a6626, 0xc1cece7, 0x1725641, 0xf96f4a4, 0x7f130a8,
          0xf06ee98, 0x72319e9, 0x67bf9b2, 0x215b738 }
    },
    {
        { 0x0aaddd7, 0x8d1bec2, 0xb8be4f9, 0xfb8b95b, 0xfde1026, 0xeac193e,
          0x9d5860c, 0xa5edea7, 0x44280d3, 0x4adbaea, 0x38f4798, 0xce8b670,
          0xec30dea, 0x914c107, 0x000776b, 0xbdc5cf7 },
        { 0xa206a13, 0xb6fd7d1, 0xdae986e, 0x9941eba, 0x1f1caaa, 0x76c27a8,
          0x3f108b4, 0x6967c12, 0x4aea2d0, 0x6f11528, 0x144ddac, 0x9bb4319,
          0xc8ec6fc, 0x1a4d3ea, 0xbf37420, 0xfe4b0b8 }
    },
    {
        { 0xec0ac6f, 0x5d9a4a1, 0xfc7c80d, 0x84b79f2, 0xc14fac3, 0x64222f7,
          0xc23b3f2, 0xdd9e039, 0xea956bb, 0x4a84abd, 0xebe09dc, 0x370dcba,
          0xe0eaf82, 0x79a9ea8, 0xaee375f, 0x4cfb60a },
        { 0x9106827, 0x6a10dbf, 0x43f305b, 0xa3ba5cf, 0xc1bb083, 0x481b885,
          0xb3117b1, 0x2f52380, 0xddd6791, 0x0066122, 0x63bace3, 0x4f8923e,
          0xecb88d4, 0x5c5f499, 0x3bac146, 0xfdc780a }
    },
    {
        { 0x7ba1f71, 0x34b70ae, 0x45bd184, 0x9091829, 0xe707313, 0x3b39778,
          0x6164e91, 0xdeefc5e, 0x4971f39, 0xbb55bed, 0x8dafc8b, 0x7d52339,
          0xa6adf0f, 0x82391bf, 0xe319522, 0xfd6f90a },
        { 0xf29bbc9, 0x60fdf77, 0xaaa4030, 0xeff9ed8, 0xf8c0d3f, 0x978e045,
          0xeed65cd, 0xe0502c3, 0x3cfd4c8, 0x3104d8f, 0xa639005, 0xab1be44,
          0x9eeab3f, 0xe83f431, 0x451d797, 0x01970e8 }
    },
    {
        { 0x3180f4b, 0xbc972f8, 0x617779d, 0xac053c0, 0x7fa149f, 0x89392c5,
          0xbcb6263, 0xdc4699b, 0xce12882, 0x0ae8b28, 0xaf1a4dc, 0xdca19a7,
          0x64e1a74, 0xd3d719f, 0xaffdd5d, 0xbb50201 },
        { 0x7ac30e9, 0x56f7310, 0x1878900, 0x65cc9c7, 0x27338a3, 0x83f5866,
          0xac5bb13, 0x122adef, 0x1bcd4d5, 0x97de200, 0xb8aa3a0, 0x6ed3985,
          0x6821f9b, 0x8680f1d, 0xdda9f98, 0xcb42028 }
    },
    {
        { 0x0ec2db3, 0xcdb0708, 0x3dad1a1, 0xe28c833, 0xde2da07, 0x2093e32,
          0x83b8987, 0x7317073, 0xf552b8d, 0xad17871, 0x51cf70a, 0x846da98,
          0x5c4f5e1, 0xf94a16e, 0x0f8348a, 0x8429996 },
        { 0x98db78a, 0x4bf3f68, 0x3d19b52, 0xad77fa8, 0x8b972dc, 0x6976772,
          0x5321be0, 0x7dfa35a, 0xdd344a6, 0x9881846, 0xad4e2a8, 0xe550292,
          0xbc68bf1, 0x8075217, 0x893be15, 0xdd837c4 }
    },
    {
        { 0xd4fab5b, 0x09c931e, 0xb77a0f1, 0xb2dcf08, 0xe0d38a6, 0x7dac5c0,
          0x0ae73af, 0xa5570b0, 0xf5aed28, 0xc7c19d3, 0x5251e92, 0x575fa6f,
          0xcdf7275, 0xb843cd6, 0x9a01287, 0xd9d3d8e },
        { 0xb3c370b, 0xf94e356, 0xfe464b0, 0xc62b99f, 0xa986057, 0x7792650,
          0xc4b1874, 0xeaa67d5, 0x0b07078, 0xba1ba4d, 0x7a03699, 0xdbf636d,
          0xedd32a3, 0x1a16c34, 0xa45cb5d, 0x6ce2495 }
    },
},
{
    {
        { 0xa684441, 0xd7c4d9a, 0x30cd42a, 0xce62af6, 0x43014c4, 0xcd2669b,
          0x6f65b24, 0xce7e711, 0x576fa19, 0x1847ce9, 0x9dd8ca6, 0x82585ac,
          0xb42e1db, 0x3009096, 0x384ab8b, 0x2b2c83e },
        { 0xb4e9a6e, 0xe171ffc, 0x7374b40, 0x9de4218, 0xdb1d616, 0x5701f9f,
          0xa3e8cbc, 0x211e122, 0x1e400bf, 0x04e8c1a, 0x0f37159, 0x0297470,
          0x3df8c28, 0x41775d1, 0x61ac2db, 0xcfaad4a }
    },
    {
        { 0x7dc0f49, 0x6341b4d, 0xf471a53, 0xaff6c2d, 0xfb8e91e, 0x20ec795,
          0xc3b7b62, 0x4c7a4df, 0xd374938, 0x9f33ff2, 0x3a60f2e, 0x38f8c65,
          0x2efef73, 0xc1168ac, 0xce408ee, 0x046146f },
        { 0x308b0c3, 0x9b39ac0, 0x36b8570, 0xe032d61, 0xfc4aacf, 0xee07d8d,
          0xd5a41dd, 0x0a82acb, 0x7c3d726, 0xbe0ded2, 0xb926ce9, 0xce51d60,
          0x5806c1e, 0xfa2f7f4, 0x1dec59c, 0xe367c6d }
    },
    {
        { 0xda2547b, 0x64511b6, 0x0761405, 0x76a349c, 0x01223ab, 0x37d6626,
          0xf4d7c48, 0x0e243c1, 0xda756a0, 0xdc9c8b4, 0xd72e7e9, 0xc7430df,
          0x27b4210, 0x0eb1308, 0xcf11cbd, 0x7a9c044 },
        { 0xe8dd150, 0x2c08ff6, 0x2932fc6, 0x18b738c, 0x04513e8, 0x07d5651,
          0xaa40a17, 0x0ca5cff, 0x01baa8f, 0xd486341, 0xb72b79e, 0xfb20faf,
          0x654020f, 0x1a051e5, 0x4e17f23, 0xe3b3317 }
    },
    {
        { 0x4de9428, 0x0591048, 0x5abdf97, 0x620542a, 0xa16a4d1, 0xaa0eded,
          0x6d65bb9, 0xa93f71c, 0xb8dfaf9, 0x88be135, 0x57ca8ee, 0x1d9f4e5,
          0x26781ad, 0x4c896aa, 0x6c6c49f, 0xd3fbe31 },
        { 0x2c34c3d, 0x088d852, 0xbadff1e, 0xbb6d645, 0x385450d, 0xe3080b8,
          0x50ab1f3, 0x5ccc54c, 0xac0657d, 0x4e07e6e, 0xb7ef2c0, 0xa7ba596,
          0x73a81e9, 0xcceca8a, 0x8284c35, 0xa0b804c }
    },
    {
        { 0xf17a6a2, 0x7c55956, 0x789cfa8, 0xb451d81, 0x2506eaa, 0xdf414e8,
          0xae96562, 0x6ef40fb, 0x0e0297e, 0x63ea283, 0x73c46fa, 0xf5df26e,
          0xaac8bce, 0xe00641c, 0x64371f3, 0xc89ed8f },
        { 0x793202e, 0xd22b08e, 0x875cb50, 0x39a9033, 0xf85ddb4, 0xe64eec0,
          0x7acf7b5, 0xdce45a7, 0xb9b802d, 0x39d1e71, 0xbd559ac, 0xafdfe7c,
          0x809eeb5, 0x17ec1f8, 0x4889b8c, 0x8c0e38a }
    },
    {
        { 0x17089da, 0x47eabfe, 0xec90c50, 0x2d18466, 0x5861531, 0xa511aa4,
          0x8c39b39, 0xebb3d34, 0xf1b5282, 0xa0ac4da, 0xa9dadba, 0xea26be7,
          0x554d86e, 0x8992ba8, 0xd5f2ef5, 0x7fcbdb6 },
        { 0x56863e7, 0x320e79b, 0xa7dce2d, 0xeb9d0c0, 0x784cbc6, 0xb9f4031,
          0x7ac1f81, 0x68823ee, 0x9d87497, 0xa6b6f4f, 0x57f9b6e, 0x83c67b6,
          0x0fef2a7, 0x3735747, 0x59596e2, 0xf38028f }
    },
    {
        { 0x7e82886, 0x9ea57ab, 0x48c44d5, 0x18221c5, 0x314a24f, 0xbf8e6cf,
          0xfd025e5, 0x70ff18e, 0x5334468, 0x08d03de, 0x7404fb7, 0x2b206d5,
          0x55e36b0, 0xb923271, 0xb88ddd9, 0xcc7604a },
        { 0x4a746f0, 0x3df5152, 0x168e3fc, 0x8fdebd8, 0x7f8c32c, 0xffc550c,
          0x148743e, 0x1dbbc17, 0xb88e18b, 0xd48af29, 0x750027c, 0x8dca11c,
          0x1832be3, 0x717f9db, 0x2b06019, 0x22923e0 }
    },
    {
        { 0xc1cc4d3, 0xd4e06f5, 0x2b4f03a, 0x0fa32e3, 0xc4628d0, 0x956b9af,
          0x939dad1, 0x95c39ce, 0x8a00416, 0x39d41e0, 0x6fb01aa, 0xfd7ff26,
          0x45af340, 0xc6033d5, 0x8e36584, 0x2f65542 },
        { 0x8dff960, 0x14cfb1f, 0xda81474, 0x7236ffc, 0xd452d0f, 0xc6a6788,
          0x77f6094, 0x2ad4a52, 0x07eea74, 0x369d65a, 0xd6229aa, 0x27c6c38,
          0x8863976, 0xe590e09, 0xb38b142, 0x361ca6e }
    },
},
{
    {
        { 0xdfeb7ef, 0x6803413, 0xd3f4fad, 0xb669d71, 0xc941606, 0x5df402a,
          0x8e6c5b7, 0xe5d1776, 0x92ab236, 0x131bcb3, 0xce2e0e0, 0x7f1fb31,
          0x9e98c35, 0xa2c020d, 0xf28657b, 0x33b23c0 },
        { 0x9cf7879, 0xed14e73, 0xb4357b3, 0x10d4867, 0x31e4e04, 0x127cea3,
          0xaa5f8a7, 0xc60d25f, 0x025b987, 0xfef840a, 0x66f2a0a, 0x78081d6,
          0xac36198, 0x0fa0b97, 0x134dc9f, 0xe0bb919 }
    },
    {
        { 0xcc32eae, 0xc1d2461, 0x0f79a37, 0x0fdbfdf, 0x1c95f02, 0x70f2bc2,
          0x372cddf, 0x7d68bec, 0x8439342, 0x44f7817, 0x4843a6c, 0xa3d5678,
          0x07f8959, 0xbadf77a, 0x73db4ca, 0xf458198 },
        { 0xd54f805, 0xe8eaaf3, 0xb84c1e7, 0x2f529d1, 0x21e535c, 0x404e32e,
          0x159b5f5, 0xabac85c, 0xb00466f, 0x4e8e594, 0xc941873, 0x40fcaab,
          0xbe407c6, 0x3b4e370, 0x5b2e58d, 0xccd5788 }
    },
    {
        { 0x88b74a8, 0x3ee615e, 0xeab4e69, 0xd7d6608, 0xe4ace36, 0x27cf9f1,
          0x7aebabb, 0x282359e, 0xf6d162f, 0x96e509b, 0xf1a290a, 0xad906f3,
          0x1314a58, 0xe7d6c4f, 0x218431d, 0xeecffe4 },
        { 0xe2cfed9, 0xa66e0e9, 0x71f0544, 0xb0887ec, 0xa04c5d7, 0xd34e36b,
          0xed4392d, 0x094daa5, 0xc8aa925, 0xcda83ad, 0xb979786, 0x1adef91,
          0xfddc5d6, 0x3124dcb, 0x0b70c14, 0x5cc27ed }
    },
    {
        { 0x0eac2d8, 0x386dbc0, 0xc50ca30, 0xa716ecb, 0x80d9f04, 0x9e3fc05,
          0xcfeaceb, 0x37dde44, 0xa3522d5, 0xd88d74d, 0x2cf239a, 0x6bb9e9f,
          0xa7cbfec, 0x9e7fb49, 0x0a5c0ef, 0xe1a75f0 },
        { 0xfb9229d, 0x6e434e7, 0xc8a79b3, 0x0ec6df5, 0xd3fb311, 0x7046380,
          0x52e20fa, 0xe957ef0, 0x9ef4614, 0x0f4fe9a, 0x54d8f2b, 0x1b37d9c,
          0x39d84a2, 0x23b2dc1, 0x724e713, 0xf62c4f6 }
    },
    {
        { 0x747e219, 0xbd6922c, 0x3869b7b, 0x34d1438, 0x96f2272, 0x8c875a5,
          0x3fe361e, 0xd9602c0, 0x744839f, 0x081348f, 0x61ac1f1, 0x61bd16c,
          0xd8da4e1, 0x993b727, 0x7741271, 0xbb40ba8 },
        { 0x81dcfff, 0xe6dcc98, 0x93ce616, 0x9f513f5, 0x618cd8f, 0xdc09683,
          0x26639be, 0xc3b1d10, 0xc762ee2, 0xe8f149f, 0xb244aae, 0x59f26ef,
          0x693dd96, 0x3f2de27, 0x9c3a7de, 0xd8b68f7 }
    },
    {
        { 0x970bd5b, 0x6fa20b9, 0x75f6179, 0x87242d7, 0x72d9308, 0xa95a6c6,
          0x37a8a58, 0x6eb2518, 0xc59562c, 0xfdea12a, 0x20f1fc3, 0x4419c1e,
          0x9d66788, 0x0c1bd99, 0x32c0547, 0x4b74288 },
        { 0xdf479ab, 0x4f38acc, 0xc52a942, 0x01f6271, 0x02ca9a7, 0xe3298f4,
          0xb718fc8, 0x533daca, 0xb093ca8, 0x133602a, 0x8f98104, 0xc04da80,
          0xaf08620, 0xd0f2e23, 0x178b164, 0x882c817 }
    },
    {
        { 0xec30a71, 0x28e6678, 0xf78aca1, 0xe646879, 0x88fa078, 0x868a64b,
          0xfee3433, 0x671030a, 0x87c0211, 0xb2a06bb, 0x46c406a, 0x202eca9,
          0xe4f0f59, 0x64d6284, 0x3c9f907, 0x56ae4a2 },
        { 0x1dcc100, 0x5abbb56, 0x07c7784, 0x6fef6cf, 0xdb7302d, 0xb6e25cd,
          0x42980e8, 0xa26785b, 0xfb96801, 0xe7d4043, 0x8e4282b, 0x46df55d,
          0xc602d6e, 0x9c0a5f5, 0x75dfe29, 0xf065604 }
    },
    {
        { 0x3dcbc90, 0x0e82a1a, 0x656feac, 0xb1ee285, 0x0d3d3b2, 0xfa4353b,
          0xdd5c5df, 0xc2e7a6e, 0x416ce53, 0x13707e1, 0x87ebc07, 0xc84ce07,
          0x8a9a834, 0xdd273ce, 0x5e8e1e7, 0x432a617 },
        { 0xbd0064a, 0xa359670, 0x6534516, 0xc899dd5, 0xdb27169, 0x666560e,
          0xa19a068, 0x1537b22, 0xeac7527, 0x3420507, 0x6fc13a7, 0x479f25e,
          0x1bc19b3, 0xc847acc, 0x0b20d45, 0xecdecf0 }
    },
},
{
    {
        { 0x4acea57, 0x6f24100, 0xda68597, 0xdace1c6, 0x50ce77f, 0xea7dd41,
          0x1585884, 0x1aecb84, 0xea4a85c, 0x92ff208, 0x88eebd2, 0xde9433c,
          0x3f4d289, 0x53cd318, 0x26539af, 0x3970858 },
        { 0xb827d87, 0x4b57599, 0x3d77638, 0xdc82ac0, 0x52f6e61, 0x6943366,
          0xad5e8a6, 0xb8fc4b0, 0xf388642, 0x1b6f7dc, 0xa74dd57, 0x6f24533,
          0x41750cf, 0xc669378, 0x28a37af, 0x06757eb }
    },
    {
        { 0xc133995, 0x0e70d53, 0x7c8c97d, 0x88a5e0c, 0x85f3be3, 0x4e59dbf,
          0x0e92698, 0x0f364ac, 0xef6940f, 0x3a1e79b, 0xd85d23a, 0xc8a3941,
          0x9a00e58, 0x143bb99, 0xc6f2f10, 0x61cf7d6 },
        { 0x85150fe, 0x979c994, 0x59d773f, 0xcfd0df2, 0xaab7bcd, 0xce97b9d,
          0x6afd8fc, 0xc9fff8e, 0x89a4628, 0x246befd, 0x1567090, 0xf630282,
          0x6749c58, 0x1539342, 0xa0f3fd3, 0xff47d0e }
    },
    {
        { 0x35f6706, 0x09b0bfd, 0x2c82e69, 0x7464581, 0x50d5fe9, 0xb60729f,
          0x95c74f1, 0xf133245, 0xbb76c89, 0x33647e3, 0x5a9afcc, 0x0126404,
          0x0f154ab, 0x46d57ee, 0x25680a4, 0x2efa555 },
        { 0x5329d90, 0x12ebfc6, 0x79800af, 0xcb37ae5, 0x6f8e310, 0x5bb5349,
          0xf1bb936, 0x9b59c63, 0xf4610e9, 0x5b49baa, 0x4f2d6ac, 0x2bbeeef,
          0x0badc67, 0x87ee21e, 0xf1ddfa0, 0x12e2aad }
    },
    {
        { 0xa9109ee, 0x5b4668f, 0x8a6cea2, 0xfa95133, 0x4068e16, 0xe45e6fc,
          0x0205ed8, 0x8ae9a0c, 0x679b79b, 0x2993b96, 0xed604d3, 0xc6b878f,
          0x32c77f3, 0x01d0208, 0x495a1ab, 0xd45d890 },
        { 0x29d2030, 0x99348fa, 0x61f8f7a, 0x961f9a6, 0x674f74b, 0xfd53212,
          0xb3e72bc, 0x45cee23, 0xb77e2d5, 0x3fccb86, 0x4219cb7, 0xdff0310,
          0xc056871, 0x233771d, 0x7d2c521, 0x1214e32 }
    },
    {
        { 0xff2a8e1, 0x9f51e15, 0x138bc70, 0x86571c5, 0x0c09d46, 0xbfc4caf,
          0xc2a0c18, 0x65e33fe, 0x426867d, 0x8214392, 0x80ae4ed, 0x51ce6c0,
          0xb110de6, 0x6cbe8d7, 0xfd22ea4, 0x7f6e947 },
        { 0xcadefc4, 0x7373a75, 0xb0c682f, 0x6fca1d2, 0xf3c7c1e, 0xcd2140d,
          0x558b7a5, 0x8653a37, 0x55eb321, 0x653e74e, 0xc31af73, 0xbe0c6b3,
          0xf4fc365, 0x3376379, 0x71add4d, 0x3570b37 }
    },
    {
        { 0x83c3494, 0x9061ec1, 0x677bc95, 0xaf2f28d, 0x3bf8768, 0x6fe7279,
          0x0fa86d8, 0xc5f50e3, 0xa3293ce, 0x6c03060, 0xe2355a6, 0x4d53357,
          0xe4df931, 0x43a59ea, 0x13b79c6, 0x6f48f5d },
        { 0xddc5192, 0xa4d073d, 0xa65773f, 0x6d0e318, 0x765de9e, 0x1008792,
          0x39a0375, 0xa724ed2, 0x97d7c9e, 0x510ff14, 0x5baa863, 0x251f622,
          0x648a351, 0x86464fe, 0xd50fd91, 0xf85e98f }
    },
    {
        { 0x86ee987, 0x29c9634, 0x10dcc9f, 0x93e8e52, 0xc910b1f, 0xa1fc4d1,
          0xfeb603e, 0x015acac, 0x0844a5f, 0xc9f25f8, 0x73f4dac, 0x50de93c,
          0x310a4aa, 0x1758783, 0x358f106, 0x544d570 },
        { 0x1dc68ca, 0x4eeec7b, 0xe00fbcb, 0x6238e6f, 0xb4e83c9, 0x34d394c,
          0x2292656, 0x764ffa2, 0xf641f2e, 0x5614cd1, 0x9e07234, 0x4252eb6,
          0x68d2ba4, 0xcbaef45, 0x8a98b17, 0x8c9c550 }
    },
    {
        { 0x4106140, 0xf235d9d, 0x9eb601e, 0x1bf2fc3, 0x375e0c3, 0x6fb6ca9,
          0xc0024d2, 0x4bf5492, 0xeb54cc6, 0x3d97093, 0x5c90cb5, 0xc60931f,
          0xfbe0f1a, 0xfa88808, 0xd33e7d4, 0xc22b83d },
        { 0xc0abbf5, 0x9cfec53, 0x93723df, 0x52c3f0a, 0x39b96b6, 0x0622b7e,
          0x1667270, 0x300de28, 0x9ef426a, 0x50b66c7, 0xc6eb295, 0x8849189,
          0x8914a7e, 0xeaec3a9, 0xc4c99e0, 0x7ed56b0 }
    },
},
{
    {
        { 0x687e557, 0x7926403, 0x5310017, 0xa349816, 0xd43a8fd, 0x1b06e91,
          0x6ac23cb, 0xf201db4, 0x4f48750, 0x6f172ad, 0xe74bd3e, 0x5ed8c8c,
          0xdaba648, 0x492a654, 0xa9b64ff, 0x123010b },
        { 0x6e89f93, 0xa83125b, 0x398378a, 0x3a3b0b0, 0x0aebe7c, 0x9622e0b,
          0x49512a4, 0xb9cbfdc, 0x6aaf12a, 0x13edffd, 0x9f5eafd, 0x555dff5,
          0x1212efa, 0x3cba6fe, 0xd9bb0f8, 0xd07b744 }
    },
    {
        { 0x9a48920, 0x45732b0, 0x13ff36d, 0xf3080fc, 0xde8f950, 0x9347395,
          0x382b897, 0x14d025a, 0x04d72ad, 0x60c5a74, 0x11a9c71, 0x30be7e5,
          0x31ac33a, 0x43ffabd, 0x35cbb14, 0x97b06f3 },
        { 0x7740de9, 0xe4ff5c5, 0xaacf81e, 0x5fed090, 0xe8b7c9d, 0x97196ee,
          0x045910b, 0x316dcd1, 0x5ad8c63, 0x7a2b2f5, 0xc5b03bb, 0x674fffd,
          0xe65953c, 0xc1cd133, 0x0a83556, 0x3c06052 }
    },
    {
        { 0x091c23d, 0x797c3f6, 0x39c9c05, 0x2ea2de3, 0xa31f67c, 0x5d958b4,
          0xd5f088c, 0xf97afe5, 0x0b37243, 0xbcfbd2a, 0xeca630c, 0xc43ad3e,
          0x42845e0, 0xb92a337, 0xa9a0f16, 0x970bff7 },
        { 0x5970a79, 0x8635511, 0xf205928, 0xcee332e, 0xc04c208, 0x2c58d70,
          0x3f5e5bf, 0xdbfe19a, 0x8e51c56, 0x8f8f2c8, 0x8e2da75, 0xb61f58e,
          0x624d93f, 0x4046a19, 0xe1f9538, 0x7de64db }
    },
    {
        { 0xc2d850e, 0xd018e1c, 0x63a723c, 0x8cdb643, 0x90a42af, 0x9a65abe,
          0x16f20cc, 0xfeece96, 0xd5cff56, 0xc906800, 0x3f0deed, 0x0acf23a,
          0x728dd3a, 0x2143061, 0xb8ce34c, 0x66276e2 },
        { 0x73cc9c7, 0x23700dc, 0x5b1778b, 0xdb44851, 0x4aab669, 0x330f41e,
          0xf5282a4, 0x2f5aabc, 0x30f9e01, 0xff837a9, 0x901cc98, 0x1a1eb2f,
          0xe69bd7f, 0xd3f4ed9, 0x8a72a7d, 0xa6b1141 }
    },
    {
        { 0x9ea3b43, 0x34bde80, 0x5ced6ae, 0x5ddcb70, 0x95a6cb8, 0x8257f5b,
          0xc77dcb8, 0xaac205d, 0x035b397, 0x77d740d, 0xcf7e0a6, 0xca7847f,
          0x085601b, 0x9404dd6, 0x457e4f9, 0x0a5046c },
        { 0xbc11470, 0xcaee868, 0x005c5f6, 0xb118796, 0xec79173, 0xcc04976,
          0x21f6827, 0x7f51ba7, 0x486ff7e, 0xa8e3f0c, 0xf87838c, 0x327163a,
          0x6d039fd, 0xcf2883e, 0xdb8b0e2, 0x6fb7ab6 }
    },
    {
        { 0x620d669, 0x8ca5bac, 0xed7caa9, 0xff707c8, 0x927909b, 0xdaefa2b,
          0x7029da3, 0x1d2f955, 0x6d131a0, 0x52a3ba4, 0x3ab1041, 0xe5a94fd,
          0x99bc0ae, 0x5089177, 0xfa1bd16, 0xf750354 },
        { 0x6cd31fd, 0xdd4e83a, 0x92fac84, 0xd335053, 0x1691382, 0xf914cbc,
          0xda6ade6, 0x669683f, 0x8878513, 0x6944643, 0x4b1a72d, 0x429d3cc,
          0x61eec36, 0x655c46a, 0x4bc4970, 0x881eded }
    },
    {
        { 0x7ca647f, 0x5b39d37, 0xe917b34, 0x41533c1, 0x7daf734, 0xea2aeb5,
          0x1286560, 0xf1ef1eb, 0x08e0473, 0x582f2e0, 0x5edc74a, 0x5913d7d,
          0x3c1e754, 0x588c7ec, 0x7146fe1, 0xbd6db05 },
        { 0x7634907, 0x3b0d49e, 0xe43b9cc, 0x4c65ce4, 0x2d92d5b, 0xb87e958,
          0x7ab1519, 0x0513572, 0x8c3aed0, 0x03ec084, 0x561a641, 0x4d7aa21,
          0x99e92ad, 0xe5f8211, 0x48a457c, 0x379b55f }
    },
    {
        { 0xd6a8442, 0x8317c34, 0xae499da, 0xb0ab4a5, 0x720e8eb, 0xebcb16e,
          0x9a96908, 0xfd5c563, 0xad23acf, 0xcab4d67, 0xbcdf748, 0xa600a79,
          0xa2a6a51, 0x18a6340, 0x3aabd69, 0xf2f415c },
        { 0x747258a, 0xdb38a4f, 0x2e24415, 0xb6ea560, 0xf1f7655, 0xfad1ea9,
          0xc957684, 0x4e27eb5, 0xb2e1cfc, 0xf8283e1, 0xaa6291c, 0x8f83bd6,
          0x5619e84, 0x28d23b5, 0x93770a4, 0xb9f34e8 }
    },
},
{
    {
        { 0x7515fb1, 0x1bb8437, 0x7b860a6, 0xac73f2a, 0x22b390f, 0x78afdfa,
          0x66048aa, 0x815502b, 0x85bf620, 0xf513b97, 0x3fc5d7c, 0x2524e65,
          0x178c969, 0xa10adc0, 0x5391c8d, 0xa1d5396 },
        { 0xa8bcc45, 0x09fccc5, 0x7710e1e, 0xa1f97d6, 0x897d0a1, 0xd694442,
          0x5f42400, 0x7030beb, 0x7127908, 0xdebe08c, 0x2187637, 0x96b715c,
          0xb528129, 0xc598250, 0xa1ccb07, 0x0f62f45 }
    },
    {
        { 0xb765479, 0x8404941, 0x5837dc4, 0xfdecff4, 0xadbd465, 0x1796372,
          0x3159806, 0x5f84c79, 0x6aaad34, 0x6d2e46b, 0x384b375, 0xd303b4a,
          0xb392002, 0x440acd5, 0xc475e87, 0x4f2a4a7 },
        { 0x5606fc2, 0x038e1da, 0x9c2f050, 0x2d821c2, 0xf139db4, 0xc074cb3,
          0x4ec59be, 0xde2fee7, 0xa84ed59, 0x5a819ee, 0x3e98711, 0xd65c62c,
          0xb9723c1, 0x72eb440, 0x01be611, 0xb927754 }
    },
    {
        { 0xab9e9fc, 0x929fe64, 0x0bf1e85, 0x04379fd, 0xbc28ee3, 0xb322093,
          0xe4555e1, 0x78ac4e2, 0xabc5588, 0xdb42b58, 0x77c8b12, 0x1c1b5e1,
          0x40366c4, 0xf6d78dd, 0xbdae22e, 0xc21ff75 },
        { 0xa211df2, 0x1e3d28e, 0x3617c0a, 0xc5a65a1, 0x58140d5, 0x3fa02c0,
          0xb62d10c, 0x155c346, 0xe48268f, 0xc9cf142, 0x1993bc3, 0xdc14083,
          0x0ee69dc, 0x07c44d4, 0x5e2ac46, 0x6169950 }
    },
    {
        { 0xd0fb585, 0x44e4a51, 0xf1f3ce8, 0x00846be, 0x8e2de1e, 0xedef39a,
          0x33b3934, 0x430afe3, 0x4337188, 0xac78b05, 0xc9a3f24, 0x0f39de4,
          0xc9ae6a4, 0x039eddd, 0x8eacd51, 0xf470157 },
        { 0x9a2f31a, 0x1e39694, 0xb19a8b1, 0xc8a40f4, 0x9d239d8, 0xdddd10c,
          0x887e066, 0xf974245, 0x3ea28c6, 0xfdb5111, 0xe1122a9, 0xb5af0fb,
          0x36e0267, 0xd30c89f, 0x74f024c, 0x7b1c0f7 }
    },
    {
        { 0x07a39bf, 0x1ec9956, 0x3a68d15, 0x1c3ecf2, 0x4f59fe9, 0xd8a5c4e,
          0x271abc3, 0xacb2032, 0x71ef239, 0xbc6bdf0, 0xb39b391, 0x660d7ab,
          0xb627a0e, 0x2e73bb2, 0x248fc7e, 0x3464d7e },
        { 0x1666760, 0xaa49249, 0x8582659, 0xa257b6a, 0x5593089, 0xf572cef,
          0x73ca6bf, 0x2f51bde, 0x764cff5, 0x234b63f, 0xd411a35, 0x29f48ea,
          0xafe1db1, 0xd837840, 0xd9f4c4b, 0x58ec0b1 }
    },
    {
        { 0x5e6f3dc, 0x8e1deba, 0x06a5ff7, 0xc636cf4, 0xc80ca0f, 0xe172b06,
          0x5ffb90a, 0x56dc098, 0x9a05e83, 0x895c218, 0x7561ac2, 0x6ddfaec,
          0x96283a0, 0xaa35749, 0x7e7cd43, 0x6dfb262 },
        { 0x2c8ca27, 0x6576de5, 0x49018eb, 0x6a4a872, 0x5c34342, 0x00c275c,
          0xd2d90c4, 0xe34805a, 0xd8743c4, 0x651b161, 0x7312bf3, 0xb3b9d9b,
          0x0bf7e00, 0x5d4b8e2, 0x78d3d7e, 0x8899bdf }
    },
    {
        { 0xfaa9cd1, 0x9644ad8, 0x6e0e58e, 0x34c98bf, 0x404c637, 0x6022aad,
          0x7ac013b, 0x2a11a73, 0x5540899, 0x5bdd103, 0x1e022a4, 0x2e67572,
          0xb834c33, 0xe32045d, 0x2f2d01c, 0x74a260c },
        { 0xc48841c, 0x20d59e9, 0xe560359, 0x05045dd, 0xac998ac, 0xeba779c,
          0x00a6218, 0x5bed10c, 0x5327ef4, 0x25d4f8e, 0x4597794, 0xa278474,
          0x831d11e, 0xefd68ca, 0x934446a, 0x9ad370d }
    },
    {
        { 0x73c92ac, 0x3089b3e, 0x957a75c, 0x0ff3f27, 0xd676f50, 0x843d3d9,
          0xd496d43, 0xe547a19, 0x8e924a4, 0x68911c9, 0x85b5522, 0xfab38f8,
          0x83e0ac5, 0x1048811, 0xdc788c4, 0xcaccea9 },
        { 0xe3c6aad, 0xfbe2e95, 0xb3a6cf1, 0xa7b3992, 0x87d78b1, 0x5302ec5,
          0x1826100, 0xf589a0e, 0x8610632, 0x2acdb97, 0x9232b26, 0x1e4ea8f,
          0x9c09a15, 0xb21194e, 0x849b909, 0xab13645 }
    },
},
{
    {
        { 0xf3a71c1, 0x92e5d6d, 0x297d661, 0x349ed29, 0x1713fc9, 0xe58bd52,
          0xb9ddfb5, 0xad999a7, 0x3c28ce0, 0x271c30f, 0x2a9d460, 0xf6cd7dc,
          0x207dec7, 0xaf728e9, 0xfcb8bf0, 0x9c2a532 },
        { 0x68bf486, 0xd702184, 0x7ab8ea8, 0x73b45be, 0x1795c93, 0xddfc658,
          0x6bb8da2, 0x7941660, 0x88e07a2, 0x658f197, 0x26d3d12, 0xa9d5b08,
          0x9535b52, 0x4d7c95f, 0x268ef8a, 0xad55e25 }
    },
    {
        { 0xa2bc326, 0x94a9b0b, 0x167e5f3, 0x485ecc5, 0xc97fc74, 0x8340bc7,
          0x07aaa5c, 0x06f882b, 0x849698a, 0x4b57455, 0xb36a0ba, 0xd9281eb,
          0x8b8108f, 0x8918c6c, 0x5b50d1d, 0xedd1eea },
        { 0x2a25f50, 0x94d737d, 0x2446ad0, 0x0e5a823, 0x7ced3e2, 0x02a5435,
          0x4af8ced, 0xb09a92a, 0xeeecef2, 0x85fc498, 0xe71e3d4, 0x06a02b9,
          0x84bb49a, 0x00ad307, 0x64a5b4a, 0xf61585e }
    },
    {
        { 0xb86a4c9, 0x915f6d8, 0xa861e1f, 0x944bc6b, 0x54465ef, 0x3091ca7,
          0xeb53a38, 0x11df859, 0x0144679, 0xd44dde5, 0x0994edd, 0x6c8da9a,
          0x91241ef, 0xeebcebf, 0xc2f6859, 0xc419354 },
        { 0x49581b6, 0x1f49693, 0xbb26cb4, 0x5712b10, 0xb09fd59, 0x8fcaa41,
          0x72e22e3, 0xbd39aad, 0xb1199b0, 0xf70e794, 0xc6f863d, 0xdf63c0c,
          0xee9df4f, 0xd58166f, 0xc45e70b, 0xb9224ea }
    },
    {
        { 0xce525f4, 0x80072fa, 0x66a5502, 0x8597bd6, 0xdbc9725, 0xf65e203,
          0xf2222a4, 0xeccfbe3, 0x2339834, 0x490aa42, 0x62489e8, 0x1348891,
          0xa735084, 0xaff3f80, 0xf3f1bd6, 0x69d53d2 },
        { 0x813341a, 0xb123ffc, 0x1173848, 0x359084c, 0xd29b08d, 0x751425e,
          0x3890ad4, 0x1edda52, 0x607cf20, 0xb64974c, 0xb42ac7c, 0xa8c8cb8,
          0xedd42e5, 0xd5cb305, 0x44c090a, 0xf3034dc }
    },
    {
        { 0xbb18e19, 0x428921d, 0xfed2127, 0x4cfd680, 0x92ac8c3, 0x671144d,
          0x132c894, 0x2121901, 0x7604cd9, 0x25d0e56, 0xafbc2a0, 0xa372223,
          0x56c16f7, 0xcf98a52, 0xb5459e1, 0x71f129a },
        { 0xb668b2e, 0xf4afdc5, 0x0c2d410, 0xc5d937a, 0x285d54a, 0xe2cc4af,
          0x8c53e18, 0x1c82777, 0x69a92f6, 0x270f2c3, 0x616327a, 0x799f9ac,
          0xd4246f2, 0xce658d9, 0xfb12e36, 0x0fb681f }
    },
    {
        { 0xe0690fe, 0xc5ab11e, 0x3f74249, 0x80261e3, 0x58c1cf2, 0x8eb4b47,
          0x184ae9b, 0x4895a80, 0xd3e27eb, 0x4a4bdb6, 0xbfd251c, 0xa7a1638,
          0x417a7e3, 0x29ec144, 0x3f1b960, 0xd073609 },
        { 0x49c73d1, 0xcb1ed83, 0x8d1945a, 0x33fc84a, 0xe965118, 0x9f668db,
          0xa82811f, 0x3331743, 0x28ba540, 0xf394dec, 0x654a454, 0x44ce601,
          0x3623645, 0x240dbb6, 0x2e61048, 0xf07e7f2 }
    },
    {
        { 0x3d45213, 0x7c9f176, 0x9c1f77f, 0x3eefa70, 0x1b48350, 0xde3c3c5,
          0x9d481a7, 0x4a2bc64, 0x7874f3d, 0xfd4a58a, 0x037b302, 0x96655d4,
          0x68bf5ab, 0x9452528, 0x75177f6, 0x1b6d46a },
        { 0xefb8d00, 0x7de6763, 0xa741b7b, 0xb2c1ba7, 0x7bae6ed, 0xcca6af4,
          0x5b68b3f, 0xe4378ca, 0xaf71948, 0xfb757de, 0xbc6ac99, 0x7f07b5e,
          0x27d636d, 0x752a568, 0x4b8a34f, 0xc8b7d1d }
    },
    {
        { 0x325331b, 0x76cb78e, 0xadd2eed, 0x41f41c9, 0x5c5f623, 0x03db238,
          0x7102fa2, 0xbbc1d17, 0x60182ec, 0x80f137a, 0x55adf15, 0xfdd8569,
          0xe3373dc, 0x4f53f5e, 0x21b669b, 0xec6faf0 },
        { 0x0b86081, 0x7d4e983, 0xf2d979c, 0x10d3cd9, 0x24a22c8, 0x0f48f58,
          0x02f99ee, 0x86c540c, 0x5e6c5fc, 0xf4c6654, 0xbc404c8, 0xaf0c588,
          0x423118a, 0x2e6edbd, 0x0690eab, 0x86e32e9 }
    },
},
{
    {
        { 0xdfbfa6f, 0x1d12656, 0x7646018, 0xa498095, 0xc3597d0, 0x2f1071b,
          0x1dda80a, 0x3df83f9, 0xf3ae449, 0x5853e28, 0x9e19aad, 0xb853d31,
          0xa0d8a46, 0x863f01b, 0x2fef108, 0xa84fca6 },
        { 0xfb84de9, 0xbe4c0b7, 0xc0727bf, 0x40a03dc, 0xb18575c, 0x781f841,
          0x466cddb, 0x6a63045, 0x05dc7a2, 0x6be7582, 0x07ae811, 0x420f87f,
          0x3bf96c8, 0x2808242, 0x51c6821, 0x723998c }
    },
    {
        { 0x81f5863, 0x38ab641, 0x05ff9e1, 0xd82ecbd, 0xa065856, 0x339c94e,
          0xa45156d, 0x143054a, 0x065628c, 0xe6d64bf, 0xa938589, 0xe530086,
          0x385d79b, 0x22d3a49, 0x0ab8245, 0x0b10790 },
        { 0xca387b5, 0xb0d80fb, 0x35551d7, 0x698206e, 0xa10bb73, 0x199685d,
          0x9107378, 0xa8e5fa8, 0xd99dbbf, 0x36e5724, 0xd581b03, 0xd67f476,
          0x88dd1e6, 0x7a15be7, 0xe5baa31, 0x8dac8e4 }
    },
    {
        { 0xe170ef8, 0x4d5d88f, 0x1e9e600, 0xb6ba5de, 0xedeabc5, 0x4a89d41,
          0x8fac936, 0x737c66b, 0x65c3125, 0x8d05b23, 0xb61b68e, 0x85a5cbc,
          0x20a6af9, 0x8fea626, 0xd8b50ec, 0x85115de },
        { 0x6a6f30b, 0x5430c8d, 0x8474295, 0x8bef9cf, 0xbe77f38, 0x0648f5b,
          0x9e47bd7, 0xfe2b72f, 0x93106e2, 0xad6c5da, 0xfa7a6c3, 0x4fa6f3d,
          0xb396650, 0xdcd2ed8, 0x1157ef9, 0x7de1cce }
    },
    {
        { 0x1f241d1, 0x70a5f6c, 0x798cd5c, 0x6c354d8, 0x1a729fb, 0x23c7838,
          0x523cbda, 0xcff8f15, 0x3493697, 0x5683ff4, 0x7534f53, 0xef7dbab,
          0x2243d53, 0xd7bd08e, 0xf8072a9, 0x6f644cb },
        { 0xb22db63, 0xac960f9, 0x23af04d, 0xa97f417, 0xd9798af, 0x692b652,
          0xfedb156, 0x0e35967, 0xdfe6ee8, 0x14b5e50, 0xb411070, 0x7597ede,
          0x442b3f9, 0x116f3ce, 0x1b2b6db, 0xe9b5ae8 }
    },
    {
        { 0x2315930, 0xf4385ee, 0x27a8740, 0xc8d0298, 0xd934a43, 0x7907a8d,
          0xc582191, 0x20bc946, 0x6a405e7, 0xa4acb3e, 0x43df2f5, 0x8c1d6c8,
          0x991f0b5, 0x9df1593, 0x4d9be9d, 0xbb9df98 },
        { 0x8e4b190, 0x6362008, 0xada3a88, 0xee1421e, 0xf93b027, 0xb84f0cc,
          0x8e95091, 0x7a5d667, 0xf3e3704, 0x3974462, 0xc593e98, 0xfa6fb5e,
          0xa6477d2, 0x44b6cf7, 0xb09a562, 0xe885b57 }
    },
    {
        { 0x09a0c02, 0x6e339e9, 0x0e75f29, 0x57afff0, 0xfb7db03, 0x797d8d6,
          0xd25a236, 0xc6e11a3, 0x0107260, 0x643ce1c, 0x62eae1c, 0xe644ec4,
          0x3f5a3f5, 0x821d5b8, 0xc0579d6, 0xa8ad453 },
        { 0x17d43a4, 0x6518ed4, 0x3f87ccd, 0x46e76a5, 0xf9bef95, 0xd6cbaab,
          0x4f7cbcf, 0x2568832, 0x08476b4, 0x367159a, 0xbe6d324, 0x1d1b401,
          0xa605026, 0x348cb98, 0x43b6b1e, 0x144f3fe }
    },
    {
        { 0x7b1822c, 0xbabbd78, 0x2aa51f8, 0xd34ba7e, 0x41fbea4, 0x086f1cc,
          0x746f3d9, 0x96f7eac, 0x281ecaf, 0xad97f26, 0xa14ee2c, 0x751a905,
          0x0d7335f, 0xb4e7fe9, 0x4892ff0, 0x0d97b8f },
        { 0x5a5c40e, 0xdb8a315, 0x7ba567b, 0x64e5de7, 0x1eefe88, 0x4f155f7,
          0xfb6fbf4, 0xe2297e9, 0x6c16be5, 0xfe24bf9, 0xcdd83e2, 0x2251847,
          0x5eda444, 0x13ac2c8, 0x283275f, 0x49d1b85 }
    },
    {
        { 0x423e08f, 0xca08731, 0x87d2f14, 0x7046bb0, 0x3bc846c, 0x876f10c,
          0x358fbe3, 0x2202b76, 0x0e26ac6, 0x0d4fc1c, 0xb986881, 0x1fc748b,
          0x8384a18, 0x609e61c, 0x0d88e00, 0x28a72d6 },
        { 0x78c6e2f, 0x1332a31, 0xb3526a4, 0x0367919, 0x698fe3e, 0x53989e4,
          0xb16a99b, 0x14b1145, 0xddbb75f, 0xef9ec80, 0x0e53955, 0x7625624,
          0x8744ae1, 0x54e087a, 0x672b875, 0xce50e8a }
    },
},
{
    {
        { 0xa29629c, 0x4c88b2b, 0x7b2642f, 0x946559c, 0xf7ebe4c, 0x933d432,
          0x63632c9, 0x97109b6, 0xe53184d, 0x799b3fb, 0x0f069a6, 0xd462871,
          0x3a68351, 0x0c182a1, 0x9a2437a, 0x974a839 },
        { 0x2a70278, 0x29f1997, 0xd9c424b, 0x01b98b6, 0x08f4c37, 0xd85a60b,
          0x2b1da15, 0xcc3523f, 0xddffb0f, 0xf922115, 0xde84ae2, 0xee0fe4d,
          0x55365be, 0x810440c, 0x1a457e8, 0xd2f6639 }
    },
    {
        { 0xe2ddd05, 0x5e6879f, 0xabdfc61, 0x92a7545, 0xa5cede8, 0x7dedd63,
          0x70df4bd, 0x8a03b3f, 0x91f6cbb, 0xa5d1f65, 0x10f3fb2, 0x372fde6,
          0xa9dee05, 0x4537f9e, 0xdf7aa50, 0x7eb85bb },
        { 0xe8c504d, 0x963edf8, 0xe7bdb6b, 0x53c8dca, 0x6fedf2d, 0xa246e4c,
          0x0c55bde, 0x7553340, 0x0270a54, 0x2aa748d, 0x05860dd, 0xadb6cf0,
          0x9b84763, 0x8d31450, 0xeb405ef, 0x626720d }
    },
    {
        { 0x6601328, 0xa3709ae, 0x2ac2478, 0x68e94fd, 0x9d5d247, 0x3879343,
          0x392c198, 0xfa467af, 0x15df607, 0x49e7b0d, 0x61792a8, 0x8c58122,
          0x1d3762f, 0x79f7658, 0x244a39d, 0xaa38895 },
        { 0xc5cd0bc, 0xef60af9, 0xa33b3bb, 0x2b0db53, 0x251015d, 0xe3e0b1f,
          0xe64489e, 0xc608afc, 0x03651aa, 0xe52b057, 0x1c6f7b9, 0x1dda8b9,
          0xff41893, 0x833f022, 0x192818c, 0x58eb0a0 }
    },
    {
        { 0xfc7b5a7, 0x6c1300c, 0xa83ab33, 0x6d2ffe1, 0x9c02eef, 0x7b3cd01,
          0xba60d55, 0x6c64559, 0x19e2f73, 0x2e9c16c, 0xdbe47b1, 0x11b24ae,
          0x1b8153b, 0xc10a2ee, 0x1e02e1a, 0x35c0e08 },
        { 0x1dd6f16, 0xa9f470c, 0xf41a290, 0x4ea93b6, 0x25ee03f, 0xac240f8,
          0xb85aabd, 0x6cd88ad, 0x1be2f8f, 0x378a64a, 0x417bac1, 0xbf254da,
          0x9231142, 0x7e4e5a5, 0x3b8c057, 0x057aadc }
    },
    {
        { 0x80af479, 0x607c77a, 0x5ccdf74, 0xd3e01ff, 0x101b4c7, 0x9680aaf,
          0x2fc50a6, 0xd2a7be1, 0xb72d782, 0x92a788d, 0x4640b52, 0x35daf2e,
          0x39e601c, 0xc170d69, 0x7b25c2f, 0x16e05f5 },
        { 0x6fe37f8, 0x47a42a6, 0xbeca298, 0xeb74271, 0x179da16, 0x401e11e,
          0xaa53873, 0xfb8da82, 0x5bb4783, 0xd657d63, 0xfcea0b1, 0x6847758,
          0x0993154, 0x2f261fb, 0x592853a, 0x868abe3 }
    },
    {
        { 0x35766ab, 0x1a4c543, 0x6f4e4ea, 0xa1c84d6, 0x60ba199, 0x5d737a6,
          0x98b15a2, 0x4a7b1e2, 0xfd967d3, 0x207877f, 0xc262b4d, 0xcaec82d,
          0x4f2a37d, 0x0b27849, 0x6ac1711, 0x3478141 },
        { 0x8fc6856, 0x28e3df1, 0x16d003f, 0xbec03f8, 0xff39ebd, 0x2bd705b,
          0x2d776d3, 0x1dcb53b, 0x5c0e7ce, 0xabafa7d, 0x4a53332, 0x5b9c8c2,
          0x9d90214, 0xe9f90d9, 0xc129690, 0x789747e }
    },
    {
        { 0x54e2dfa, 0x94d3c39, 0xafb2a8f, 0x919f406, 0x34e3927, 0x159ef05,
          0xa165c37, 0xcdb4d14, 0x288f337, 0xa23e5e8, 0x0f90242, 0x95867c0,
          0xe34e781, 0x2528150, 0x6657b95, 0x104e501 },
        { 0xbcdda24, 0x695a6c9, 0x23eb5fa, 0x609b995, 0x16a60f8, 0xcbce4f5,
          0xf084a29, 0xec63f7d, 0x20c811f, 0x3075ada, 0x8c716a1, 0x129a192,
          0xcd4cd4a, 0xd65f4d4, 0x62188be, 0xe18fa9c }
    },
    {
        { 0xbac60e3, 0x1672757, 0x577144b, 0x525b3b9, 0x887055b, 0x38fc997,
          0x31e4408, 0x7a77126, 0xcba2fcf, 0x884f173, 0x5962ac0, 0x783cbdc,
          0x22287dc, 0x4f3ed0a, 0x50e20e6, 0x8a73e34 },
        { 0xd764583, 0xe7a1cd0, 0x0d58ee6, 0x8997d8d, 0xaa13ed6, 0x0ea08e9,
          0xcf363cb, 0xed478d0, 0x5b37bf4, 0x068523d, 0x783f13c, 0x8b5a9e8,
          0x87528a9, 0xde47bbd, 0xcaec313, 0xd6499cc }
    },
},
{
    {
        { 0xe09859d, 0x54781bb, 0x7f5e648, 0x89b6e06, 0x7075824, 0xb006dfe,
          0x0717f68, 0x1731660, 0x0b4efe2, 0x9c86554, 0x5e30d8e, 0xdbdb257,
          0x3b4d50f, 0xa6a5db1, 0xfa47beb, 0x3b5662c },
        { 0x89d4a59, 0x9d4091f, 0x550a7dc, 0x790517b, 0xc52965e, 0x19eae96,
          0xb5ed7a4, 0x1a7b3c5, 0xeb16541, 0x19e9ac6, 0xef66852, 0x5f6262f,
          0xc4cda27, 0x1b83091, 0x3bf742b, 0xa4adf6f }
    },
    {
        { 0xa5100e7, 0x8cc2365, 0x8592422, 0x3026f50, 0x3d714d0, 0xa4de79a,
          0x90fcb30, 0xefa0d3f, 0x474ada0, 0x126d559, 0xc94350a, 0xd68fa77,
          0x0c7cb45, 0xfa80e57, 0x3985fbf, 0xe042bb8 },
        { 0xfe13dba, 0x51c80f1, 0xcf055d7, 0xeace234, 0x73f95f7, 0x6b8197b,
          0xdcdbe89, 0x9ca5a89, 0xdfd9896, 0x2124d5f, 0x9e7ca37, 0x7c69556,
          0x8babb37, 0x58e806a, 0xbaf99ce, 0x91b4cc7 }
    },
    {
        { 0x197e968, 0x874e253, 0x3160668, 0x36277f5, 0x8b95dbe, 0x0b65dda,
          0xf0872a1, 0x477a792, 0x314268d, 0x03a7e3a, 0x0c805c7, 0xa96c842,
          0xb7bc4a8, 0xb941968, 0x75db390, 0x79dce30 },
        { 0x6f4cc14, 0x577d4ef, 0xb5d1107, 0x5b0d205, 0x9f93624, 0x64ff20f,
          0x5034a2f, 0x0b15e31, 0x8b6f35c, 0x3a0f6bb, 0xe0d0ec5, 0x0399a84,
          0x0d5d521, 0xd0e5823, 0xcb1dd54, 0xdeb3da1 }
    },
    {
        { 0x182401a, 0x24684ae, 0x21a706f, 0x0b79c1c, 0xd8998af, 0xe1d81f8,
          0x4bb069f, 0xadf870f, 0xf3dd7aa, 0xd57f85c, 0xe4a40f8, 0x62d8e06,
          0x8b55aa1, 0x0c5228c, 0xa9c0a1a, 0xc34244a },
        { 0x68f544e, 0xb5c6cf9, 0xde23ab7, 0xa560533, 0x47c690c, 0xaa55120,
          0x12aaaa6, 0x20eda5b, 0x751a6a0, 0xea0a49a, 0x2baa272, 0x6d6cfff,
          0xbf4c28a, 0x95b756e, 0xe6178a4, 0xd747074 }
    },
    {
        { 0x221a94b, 0xa27b453, 0xe635f20, 0xd56ad13, 0x8c95117, 0x03574b0,
          0xed30b70, 0xf0ee953, 0x957796f, 0xb48d733, 0x58c336b, 0xf5d9583,
          0x82db529, 0x6170cd8, 0xec9d1ea, 0xcd3ef00 },
        { 0xe4d105f, 0xd1bea0d, 0xad6a559, 0xd2d670f, 0x52f9690, 0x652d012,
          0xc2529b0, 0x5f51fb2, 0xe89df2a, 0x5e88bf0, 0xcd686e4, 0x9a90684,
          0x882c7a1, 0xf519ccd, 0xc2f4d37, 0x933a0df }
    },
    {
        { 0x3f66938, 0x0720a9f, 0xd8149df, 0x99356b6, 0xa3d7f61, 0xb89c419,
          0x4ba6e31, 0xe658134, 0xab936c8, 0xd130561, 0x40dbef1, 0x0625f6c,
          0xb6bb847, 0x7b2d6a2, 0x84d506b, 0x3ca8b29 },
        { 0xfb011b0, 0x6bf729a, 0x33448c9, 0x01c3078, 0x0837420, 0x6ae9508,
          0xa207fb8, 0xf781a8d, 0x57562a9, 0xcc54d58, 0x858c5ab, 0xc9b7364,
          0x359908f, 0xdfb5035, 0x9631138, 0x8bf77fd }
    },
    {
        { 0xc13fbb1, 0xf523365, 0x9993ed5, 0x88532ea, 0x5a73492, 0x5318b02,
          0xe5a8f3c, 0x94bff5c, 0x306c2a0, 0x73f9e61, 0xf2668a3, 0x00abbac,
          0x076237d, 0x23ce332, 0x34c0f9b, 0xc867f17 },
        { 0xcfd2136, 0x1e50995, 0xb2b70f8, 0x0026a6e, 0x5077a7d, 0x66cb184,
          0xa3b498e, 0xc31b2b8, 0x260ec86, 0xc12035b, 0xe1b3df0, 0x1cbee81,
          0x8d55a42, 0xfd7b804, 0xf47a8c8, 0x912a41c }
    },
    {
        { 0x9e157e3, 0xab9ffe7, 0x44dc158, 0x9cfe46d, 0x8a4a3ef, 0x435551c,
          0x3b7e3a8, 0x638acc0, 0x49954a7, 0x08a4ebd, 0x13194f7, 0x295390c,
          0x253892a, 0x3a2b68b, 0x25d5b11, 0xc1662c2 },
        { 0x3a5d2bb, 0xcfba072, 0xcc327c9, 0xffaf6d3, 0xc67e254, 0x6c6314b,
          0x2f32208, 0x6661631, 0xbea72e1, 0xf780f97, 0x002122f, 0x495af40,
          0x7578a99, 0x3562f24, 0x77ce51e, 0x5f479a3 }
    },
},
{
    {
        { 0x1a82a12, 0x91a5884, 0x80f3a62, 0xa754175, 0xf73417a, 0x399009f,
          0x0a8c5cd, 0x2db1fb9, 0xc046d51, 0x82c8912, 0x8f18274, 0x0a3f577,
          0x26ccae2, 0x2ad0ede, 0x8a4e9c2, 0x7d6bd8b },
        { 0x4b3de44, 0xaa0d797, 0x96ac9bb, 0xf8658b9, 0x5f6c334, 0x31e7be2,
          0x4df12c9, 0x23836ce, 0x59eb5c9, 0x029027b, 0x5b8649d, 0x2f22531,
          0xd907162, 0xa0fdf03, 0x9e80226, 0x101d9df }
    },
    {
        { 0x9a90835, 0xf12037a, 0xf0222a7, 0xd2d0882, 0xc3814e2, 0xeaf8d40,
          0x8b8146b, 0xa986dc6, 0x8504653, 0x147a331, 0x2feaf67, 0x734e003,
          0x602bec5, 0x6f27bbf, 0x6a688f3, 0xa1e21f1 },
        { 0x73c4ae5, 0x5a8eeab, 0xe70b412, 0x4dbaddb, 0xcfd2af1, 0x871ceba,
          0x7d7a286, 0x1860382, 0xb5bb401, 0x024059d, 0x3c39b73, 0x2557c09,
          0x6681697, 0xfc5a711, 0x891b57c, 0xf881c0f }
    },
    {
        { 0x8ea191a, 0x3c443f1, 0xd700ad0, 0x76faa58, 0xbe7fcbf, 0x6fe6cfa,
          0x8990ef7, 0xaefc528, 0x80004cc, 0x44e30fa, 0x6d8ef85, 0xc744adc,
          0x912df70, 0xafcd931, 0x572a6d8, 0xf62a9d1 },
        { 0x3219f27, 0x47158a0, 0xad73136, 0x76fb27e, 0xcc2d614, 0x41bb2ad,
          0xde1ec21, 0x8858cb9, 0x5f15866, 0xab402c4, 0xbc82bbf, 0x6675d5b,
          0xf1b28d3, 0x4ee9dd6, 0xe373c17, 0x875884f }
    },
    {
        { 0x2a67d36, 0x17806dd, 0x32c9ec1, 0xaa23a86, 0xfc1ee55, 0xd914126,
          0x653701b, 0xbf8f7bd, 0xea71367, 0x9b0111a, 0xa98e417, 0x61fd4ab,
          0x561c5a5, 0xeb45298, 0xe7af394, 0x2187b0a },
        { 0x1616dde, 0x71f12db, 0x07da7b4, 0x0617609, 0x02ddb04, 0x414d376,
          0x286fb58, 0x1100be7, 0x6f0d95b, 0xd7cf88d, 0x746d703, 0x8539d23,
          0x4e23d73, 0xdccc9d6, 0xec89680, 0xaeef1d2 }
    },
    {
        { 0x336508d, 0x82ccf1a, 0x5bad150, 0xa128c1f, 0x29a188d, 0x551d8c0,
          0x771404f, 0xef13dd4, 0xc37b993, 0xdd67696, 0x0dddad2, 0x428c0e2,
          0x038c94c, 0x222278d, 0x078e3f2, 0x1a24a51 },
        { 0xedb0db9, 0xd297fe6, 0x8251a87, 0x00988d2, 0xbfaa0d7, 0xbb946f8,
          0xdf45ea0, 0x380f7b9, 0xafccf5e, 0x8526415, 0xe9ec7bc, 0x909bfbf,
          0x124755c, 0x2ed7093, 0x89404e2, 0x4368028 }
    },
    {
        { 0x36d9ef1, 0x21b9fa0, 0xe433526, 0xfd64b7c, 0x6544849, 0xd9d7eb7,
          0xd5b54b3, 0x201620c, 0xbb61159, 0x25fab3d, 0xc53e0d3, 0x90d4eb0,
          0x9e74772, 0xba09831, 0xec1681c, 0x8749658 },
        { 0xfec316b, 0xa354349, 0xa743ea2, 0x639a9b1, 0x37c50e6, 0x2e514ca,
          0xdbaf6c5, 0x9f4a4fd, 0x6f511c9, 0x0df87ef, 0x0c00d95, 0xadd4cef,
          0xaa1433f, 0x401c0eb, 0xbb38af9, 0x3c3a59e }
    },
    {
        { 0xf0e7dca, 0x8706245, 0x3fb29ca, 0xad238cd, 0x9b7d8f0, 0x0330443,
          0x154f495, 0xfdcd6e6, 0x7d4ad09, 0xc67e24a, 0x5438390, 0x1b209e8,
          0xb0c211e, 0xf893b81, 0x7e11e36, 0x1aa86f0 },
        { 0xedea8b1, 0x2cca3ff, 0x3b306cd, 0x7eedd07, 0x12ee222, 0x78e37bc,
          0xbc42a1d, 0x257870b, 0x1fbd397, 0x5fb2bb9, 0x09d6c60, 0x4702470,
          0x20bdc36, 0x11748a3, 0x04280e8, 0x3ff24dc }
    },
    {
        { 0x9839b52, 0x0eb1c67, 0xacfbd32, 0x5bcca27, 0x74898e3, 0xb506c16,
          0x2489e5e, 0x37d662e, 0xf694887, 0x8dc0731, 0xf43f1dc, 0x571149e,
          0x66d63dc, 0x6430a37, 0xb50dd70, 0x0d2640e },
        { 0x3b2675b, 0x2b56149, 0x88c604f, 0x1b48065, 0xaafbabc, 0x55c86a8,
          0x608aaba, 0xa7b9447, 0x04cad8c, 0xa42f635, 0xcee7788, 0x0f72b1d,
          0x755d99a, 0x1d68374, 0x5be2531, 0xd7cdd8f }
    },
},
{
    {
        { 0xbcdfee1, 0x67873bd, 0xfcd0a3f, 0xa5a0c0a, 0x3cfa3d4, 0x59389f9,
          0xe1c865c, 0x14e945c, 0x1d588cc, 0x62d2f8e, 0x8e228b4, 0xfd02f8a,
          0xb42b649, 0x208f791, 0xab397ad, 0x0e0dff1 },
        { 0x0bc6eb1, 0x30ac3d9, 0x5f313bb, 0xf14f16a, 0xe2a0ad2, 0x70fa447,
          0x5a0db84, 0x6e40685, 0xe32e1e7, 0xd52282b, 0x15ca330, 0x315a02a,
          0x867c2fe, 0x9a57a70, 0x0054923, 0x55f0765 }
    },
    {
        { 0xc0cf08f, 0x2d729f6, 0xebaf57f, 0x6b80138, 0x0200c25, 0x6285bcc,
          0x2cd2ac7, 0xee84519, 0x922778a, 0x28fce4d, 0xcd1011c, 0x761325c,
          0x5100e47, 0xd01f247, 0xc60d8e1, 0xc7a1665 },
        { 0x7ceb064, 0x950966d, 0x78420db, 0x0a88e85, 0xe096f29, 0x44f2cfc,
          0x640f1d2, 0x9d9325f, 0xd2426f1, 0x6a4a81f, 0x9c905ac, 0x3ed6b18,
          0x008854d, 0xba3c0e2, 0xa0d321b, 0x1df0bd6 }
    },
    {
        { 0x3feb1e7, 0x0117ad6, 0xf1ae02f, 0xa058ba2, 0x31b3f06, 0x5eee5aa,
          0xafacd4d, 0x540d9d4, 0x1571d91, 0x38992f4, 0xbf2c7de, 0xef2738e,
          0x92a798d, 0x28bfcab, 0x2286733, 0x37c7c5d },
        { 0x6470df0, 0xb99936e, 0x8af6a42, 0x3d762d5, 0xc74eec5, 0xa8c357a,
          0xf13afbc, 0x9917beb, 0xf2dc073, 0x28f0941, 0x6ce7df7, 0x306abf3,
          0xd6973c8, 0xa3c5f6f, 0x3677632, 0x640209b }
    },
    {
        { 0xe23aef7, 0xee872a2, 0xeb9b08e, 0xb497b6f, 0x3f33c63, 0xfb94d97,
          0x2b32315, 0x9ea1ff4, 0x49a4166, 0x537b492, 0xab4f8be, 0x89c7fe6,
          0xdad8f0f, 0xf68007f, 0x71b8474, 0xe56ef0b },
        { 0x3f333f9, 0x478b2e8, 0xb2607f5, 0x144e718, 0xa4c7ab5, 0x13aa605,
          0x1d0730d, 0xfc1fc99, 0x5ab3ea1, 0xe7a0437, 0x306d8d3, 0xc59986a,
          0x702a8b1, 0x24f6111, 0xe040ad2, 0x7741394 }
    },
    {
        { 0x60723a7, 0x34c6a25, 0xf4ea691, 0x8aabd0d, 0x5d7497f, 0x9d676a5,
          0x7d91fa4, 0x12c0957, 0x6479284, 0x581c7a8, 0xf4fd449, 0xa54f3da,
          0x4ef44cf, 0x2f89f3c, 0xc9ec97c, 0xfc266b5 },
        { 0x88b142a, 0xfcd3fbe, 0x4bd69c1, 0x9f3109f, 0xb5f5a6a, 0x08839c0,
          0x2e68303, 0x63ca850, 0xbba0a74, 0x2f0628d, 0x5d56b54, 0x743cccf,
          0x13e09fd, 0xbd4b066, 0xde2ba3e, 0x7a8415b }
    },
    {
        { 0xc076ab2, 0x2234a3b, 0x4977a98, 0xd6953e5, 0x31ebe2e, 0xc122158,
          0xbad78e2, 0x632145f, 0xa5c4b08, 0xd7ba78a, 0x998e32a, 0x6f4ea71,
          0x3485a63, 0x25900d2, 0x6a5176f, 0x97ac628 },
        { 0x1093f7b, 0x5df9118, 0xc844563, 0x2bf9829, 0x6272449, 0x525d99d,
          0xb5c8a18, 0x4281cb5, 0x0544a08, 0x35df278, 0xbaeb8f4, 0xf4c3d2d,
          0x5230447, 0xc7ff317, 0x5d2fbff, 0x6b4d764 }
    },
    {
        { 0x2b0c9cb, 0x4837f80, 0x8ce8418, 0xb65f816, 0x9fc1428, 0xdf66ea9,
          0x04ea7e8, 0x9788ee8, 0x8334e3c, 0x9eae900, 0xd6ba1b6, 0xbc91058,
          0xd7064b6, 0x634aba1, 0x397b368, 0x12d9bb3 },
        { 0xc413aa8, 0x0645c85, 0xac6b5e3, 0xb09dea6, 0x289a50b, 0x29a620d,
          0xbbcceb1, 0x104db3b, 0x87b3309, 0x42e4792, 0xec97f01, 0xdfc373e,
          0xb93f84e, 0xe953f94, 0x052dfbf, 0x3274b7f }
    },
    {
        { 0x1bd6fa9, 0x9d5670a, 0xdb6c4d4, 0xec42fc9, 0x1b42845, 0xaecd4ed,
          0x1b03549, 0x4eed90e, 0xbbab1fa, 0xeb3225c, 0x28a2816, 0x5345e1d,
          0x0b77d2a, 0x3741cfa, 0x7ea8caa, 0x712b19f },
        { 0x661853e, 0x42e6844, 0xe4a6e5d, 0x4cf4126, 0xc3649f6, 0x196a9cf,
          0xf21b6b1, 0x06621bc, 0x32e29ea, 0x887021c, 0x8c5680f, 0x5703aeb,
          0x660f6d7, 0x974be24, 0xc71864e, 0xaf09bad }
    },
},
{
    {
        { 0xa81b6d3, 0x3483535, 0xca037dc, 0x19e7301, 0x63ddfeb, 0x748cab7,
          0x6f01a38, 0xe5d87f6, 0x2795cd6, 0xbba4a5c, 0x615c36c, 0x411c5d4,
          0x706f412, 0xff48efc, 0x4b519df, 0x205bafc },
        { 0x5227110, 0xfcaa5be, 0x3ad0af0, 0x7832f46, 0x2642b1b, 0x34ef2c4,
          0x072f822, 0x7bbef7b, 0x923a616, 0x93cb0a8, 0x6d91ba7, 0x5df0236,
          0x42f7d21, 0x5da94f1, 0xa14e891, 0x3478298 }
    },
    {
        { 0xc831d39, 0xad79a0f, 0x4803c44, 0x24d1948, 0x86aeeb2, 0x4f8a864,
          0x926f6b9, 0x0ca284b, 0x1acd7cd, 0x501829c, 0x3d12c52, 0x9f6038b,
          0xf371ef5, 0x77223ab, 0x13bf4de, 0x2e03516 },
        { 0xb4468cc, 0x7a5a4f2, 0x470ae46, 0xdcea921, 0x11be696, 0xf23b7e8,
          0x720d6fb, 0xe59ad0d, 0x2983469, 0x9eacac2, 0xc4397ee, 0x4dd4110,
          0xcbe2675, 0x4ef85bd, 0xaa7c74b, 0xe4999f7 }
    },
    {
        { 0x8ea1e98, 0x031838c, 0x04d96a2, 0x539b383, 0x163956e, 0x5fbdef0,
          0xce3f52a, 0x6bd4d35, 0x55e897f, 0xe538c23, 0x472dd3f, 0x6078d3a,
          0xca9f452, 0x590241e, 0xfd7fc07, 0x2bc8495 },
        { 0xead4c8c, 0x23d0c89, 0x601c66e, 0x1ea55a9, 0x4f5b833, 0x41493c9,
          0xaa5a978, 0xc49a300, 0x0c69594, 0xc98bdc9, 0xccbdc8c, 0x4e44ced,
          0x6adccbf, 0xb0d4e91, 0x32c37ae, 0xd56e36b }
    },
    {
        { 0x5b93152, 0x052bd40, 0x4f1dbfa, 0x688b1d4, 0xbe5cc5f, 0xe77ba1a,
          0xa6ac543, 0x11f8a38, 0xe4bb988, 0x3355fd6, 0xf8dffb4, 0xdf29c5a,
          0x81f20ee, 0x751f589, 0xda9b7fb, 0x22a0f74 },
        { 0x6397b49, 0xec8f2bc, 0x3639201, 0xff59fc9, 0xa048264, 0xb7f130a,
          0xafdc4cc, 0xe156a63, 0xb13acaf, 0x0fd7c34, 0x0cb4999, 0x87698d4,
          0x7f26f24, 0x6d6ecae, 0x0f296e2, 0xae51fad }
    },
    {
        { 0xdd0f58d, 0xd0ad5eb, 0x5c67880, 0x6ec6a2c, 0x9af1e0f, 0xe1ce034,
          0x3996d32, 0x0801485, 0x5e69d20, 0x59af51e, 0xaa48ecf, 0x0ef743a,
          0x7dafcb0, 0x8d3d2ea, 0x89189b6, 0x4ac4fad },
        { 0xeae97f1, 0x92d91c2, 0x62b4662, 0xef5eca2, 0xb38b10a, 0x440b213,
          0xfc661da, 0xec90187, 0xf64cf8d, 0x85f3f25, 0x457ad1b, 0xcee53ca,
          0xf517672, 0x8deed4b, 0x4761828, 0x7706fb3 }
    },
    {
        { 0x17494fe, 0x1577d91, 0x2fd7239, 0x52d29be, 0x0186d37, 0x9a0eef0,
          0x27fe108, 0x241d0f5, 0xe6fb59f, 0x42824ba, 0x0d48c25, 0xb8d33df,
          0x47af4b0, 0xfffdb0a, 0x073b0b6, 0x534c601 },
        { 0x51c033b, 0xe6df359, 0x86c0f94, 0x3e1002b, 0x48fb9b6, 0xa7cb555,
          0xa7bbff8, 0x999818b, 0x84d8bf2, 0xe4ba3d6, 0x6358f0a, 0x53dbb32,
          0xf2568e8, 0xeebc1e2, 0xb3e0f68, 0xc6917eb }
    },
    {
        { 0x19f8d13, 0xbe1bbfc, 0x2d4795c, 0xc3951b6, 0xed535a9, 0x9371c49,
          0x68cebea, 0x77c389f, 0xa141d0e, 0xfc1a947, 0xde44f8b, 0x4b48d7a,
          0x8580a26, 0x3db1f05, 0x258b5fc, 0xeed1466 },
        { 0x9854b21, 0x5daa4a1, 0x1ab1ead, 0x5bfa46f, 0x59957eb, 0xc152e35,
          0xea48ada, 0xdc84277, 0xfc169b5, 0x68709cf, 0x720e617, 0xde50ce3,
          0xdd9a832, 0xe42f262, 0x2d6ce29, 0xddffd4d }
    },
    {
        { 0x8fa0a56, 0xd5ba557, 0xfafaf4c, 0x0d7d0f1, 0x38b63ed, 0x7666e41,
          0x5d87f02, 0x04e6513, 0xc958f32, 0xdca8866, 0x3ce2686, 0xaa8486d,
          0xf1cbcd3, 0xe3785ca, 0x03c8335, 0x8a9b114 },
        { 0x2e0ef60, 0x5c1dca2, 0x7d3fb20, 0x775af5b, 0x2b373a8, 0xe690ffc,
          0x28330e6, 0x30fe15d, 0xdd0f393, 0x8a1022b, 0x966a828, 0x6bd7364,
          0x949208a, 0x8d4b154, 0xb9d9828, 0xfb38c6b }
    },
},
{
    {
        { 0x0340ac2, 0x6d19764, 0xecab5ff, 0x969f473, 0xc458e42, 0xead46f7,
          0x1d00eed, 0x168646a, 0xe0ce0cf, 0xf70c878, 0x8d8d15a, 0xa7291d3,
          0xfdd10cc, 0x92cf916, 0x24f86d5, 0x6d36134 },
        { 0x2d5c4b4, 0xba50d17, 0x4626f15, 0xe0af502, 0xd76098a, 0x76f3809,
          0xd6caaa8, 0x433dc27, 0x70d97a7, 0x72dc67a, 0xf5c7355, 0x935b360,
          0x179bb31, 0xdbaac93, 0x7ed1a33, 0x7673848 }
    },
    {
        { 0x8f9fa0d, 0x8d1ca66, 0xa02f2bf, 0x4ed95d8, 0xf630d7b, 0xd19fc79,
          0xf46fa51, 0x0448ec4, 0x623bf3f, 0xb371dd8, 0xd650e94, 0xe94fabc,
          0xcd90a70, 0x3af3fca, 0x03ce3b7, 0x0f720c4 },
        { 0xd636c3b, 0x590814c, 0x4469945, 0xcf6928d, 0x484a4c6, 0x5843aaf,
          0xf9b4722, 0xb5a4c1a, 0x6cfb2f9, 0x25116b3, 0x32c2640, 0xf248cf0,
          0x27412a1, 0x8cd059e, 0x862fc5d, 0x866d536 }
    },
    {
        { 0x6de4a2e, 0x156e62f, 0xaafcc78, 0x0365af7, 0x19e925e, 0x65c8618,
          0xf8b2191, 0x4db5c01, 0xad564fa, 0x1fd26d1, 0x19c8610, 0x16bbc53,
          0x815f262, 0x0718eef, 0x27f83d1, 0x8684f47 },
        { 0xb0f48db, 0xa30fd28, 0x6ab8278, 0x6fef506, 0x1a652df, 0xd164e77,
          0xc6ebc8c, 0x5a486f3, 0xdc3132b, 0xb68b498, 0xd73323f, 0x264b6ef,
          0x69b2262, 0xc261eb6, 0x2a35748, 0xd17015f }
    },
    {
        { 0x7c4bb1d, 0x4241f65, 0xf5187c4, 0x5671702, 0x3973753, 0x8a9449f,
          0xcc0c0cd, 0x272f772, 0x58e280c, 0x1b7efee, 0x4b5ee9c, 0x7b32349,
          0x31142a5, 0xf23af47, 0xd62cc9e, 0x80c0e1d },
        { 0x675ffe3, 0xcbc05bf, 0x258ce3c, 0x66215cf, 0x28c9110, 0xc5d2239,
          0x2a69bc2, 0x30e12a3, 0x76a9f48, 0x5ef5e80, 0x2329d5f, 0x77964ed,
          0x8a72cf2, 0xdf81ba5, 0x6e1b365, 0x38ea70d }
    },
    {
        { 0x2f75c80, 0x1b18680, 0x698665a, 0x0c153a0, 0x522e8dd, 0x6f5a7fe,
          0x8ddfc27, 0x9673866, 0x0d3bdce, 0x7e421d5, 0x25001b2, 0x2d737cf,
          0x0e8490c, 0x568840f, 0xe30c8da, 0xea2610b },
        { 0x9561fd4, 0xe7b1bc0, 0x26decb0, 0xeda786c, 0x6a76160, 0x2236990,
          0x78a3da3, 0x371c714, 0x2a2d9bf, 0x1db8fce, 0x3292f92, 0x59d7b84,
          0x5a665f9, 0x8097af9, 0x542b7a9, 0x7cb4662 }
    },
    {
        { 0xc6b0c2f, 0xa5c53ae, 0x7312d84, 0xc4b8732, 0xc732736, 0xfc374cb,
          0x9310cc0, 0xa8d78fe, 0x65d1752, 0xd980e86, 0x6004727, 0xa62692d,
          0x0146220, 0x5d07928, 0x860fea5, 0xbd1fedb },
        { 0xb35d111, 0xcbc4f8a, 0x3e32f77, 0x5ba8cdf, 0xb614b93, 0xd5b71ad,
          0x2f8808d, 0x7b3a2df, 0x6ef2721, 0x09b89c2, 0x47c3030, 0x55a5054,
          0x2986ae6, 0x2104431, 0x2367d4c, 0x427a011 }
    },
    {
        { 0xc1942d8, 0xe9fe256, 0x96e3546, 0x9e7377d, 0xb0c1744, 0x43e734c,
          0x211fbca, 0x5f46821, 0x32b6203, 0x44f83dc, 0x6ad1d96, 0x8451308,
          0x2fbb455, 0x54dd519, 0x2f10089, 0xc2a1822 },
        { 0x1855bfa, 0x01055a2, 0x77078b4, 0x9e6d7b4, 0x30cea0e, 0x3f8df6d,
          0x32973f7, 0x81c2150, 0xc0b3d40, 0x17dd761, 0x50d0abe, 0x040424c,
          0x783deab, 0x5599413, 0x8f3146f, 0xde9271e }
    },
    {
        { 0xaf4a11d, 0x5edfd25, 0x7846783, 0x3a3c530, 0x73edd31, 0xb200868,
          0xfe0eef8, 0x74e00ec, 0x3dd78c7, 0xba65d2f, 0x71999f1, 0xab13643,
          0xde9a7e8, 0xfa9be5d, 0x87a8609, 0xeb146ce },
        { 0x65353e9, 0x76afd65, 0xd51ba1c, 0xfa7023d, 0x37ede4f, 0x7a09f22,
          0x0ba7a1b, 0xca08576, 0xb99950a, 0xd973882, 0xea5057a, 0xe894266,
          0x7f55e49, 0xd01c421, 0x5555679, 0x69cfb9c }
    },
},
{
    {
        { 0xc5d631a, 0x67867e7, 0x5bcf47b, 0x1de88c5, 0xafd1352, 0x8366d06,
          0x6e20337, 0xd7dbdef, 0x1253ec7, 0xb0f9e2f, 0x10ad240, 0x1be9845,
          0xf4a6118, 0x63ec533, 0x96ce633, 0xd5e4c5b },
        { 0x4df4a25, 0x1d0b6c3, 0x5a1b554, 0xef9486a, 0x47b6ef3, 0x2f0e59e,
          0x2ff84d7, 0x4d8042f, 0xda359c9, 0x3e74aa3, 0xd21c160, 0x1baa16f,
          0x0191cba, 0xb4cff21, 0xebc6472, 0x50032d8 }
    },
    {
        { 0x1fc1b13, 0xb6833e0, 0x1a5ad8f, 0x8a8b7ba, 0x622b820, 0xc0cafa2,
          0x738ed20, 0xc6663af, 0x8b18f97, 0xd894486, 0x774fbe4, 0xcf0c1f9,
          0x5be814f, 0xeedd435, 0xb57e543, 0xd81c02d },
        { 0x310bad8, 0x5e32afc, 0x9b813d1, 0x065bc81, 0x3142795, 0x8efc5fc,
          0x732d59c, 0x5006514, 0x2b5a3ce, 0x91e39df, 0xfaf4204, 0x2ad4477,
          0x4d9bd4f, 0x1a96b18, 0xa4d9c07, 0xc3fee95 }
    },
    {
        { 0x6b4ba61, 0xfac7df0, 0x061aaef, 0xa6ed551, 0x133f609, 0x35aa2d6,
          0x20ed13d, 0x420cfba, 0xea03d0c, 0x861c63e, 0xf936d6e, 0x75f0c56,
          0x3d9a3d5, 0xa25f68f, 0xcd9f66e, 0xba0b7fe },
        { 0x4680772, 0x292e135, 0xa73f405, 0x6f6a2db, 0x24ea9e4, 0xca6add9,
          0x268daaa, 0x81cfd61, 0xe6f147a, 0x7a4cb6c, 0xbded8f5, 0x8ec3454,
          0x11d61cb, 0xc8a893b, 0x7656022, 0x2256ffc }
    },
    {
        { 0x575cb78, 0x6b33271, 0xadcd23e, 0x560d305, 0xd6d834b, 0xeedbd3a,
          0x5a31e27, 0x614a64a, 0x47ee0c8, 0xe40b476, 0x8bd7c2c, 0x8ef4ff6,
          0x0b77727, 0xa5297fc, 0xbaf88ad, 0x8759208 },
        { 0x918df68, 0x86cfe64, 0xcdd882e, 0x9d60a73, 0xb953014, 0x546b642,
          0x8bbef55, 0xbaceae3, 0xf1c3467, 0xdf58e43, 0xe9f9bab, 0x99a83fe,
          0x57a4a8b, 0xcd52cbf, 0x8ae36ec, 0xf744e96 }
    },
    {
        { 0xa607124, 0xb945869, 0x440e6f6, 0x810dbe9, 0x738e381, 0x9911e60,
          0x343b80b, 0x51df68c, 0xf7a3f39, 0xe424336, 0x989015c, 0x2d32acb,
          0x31019e8, 0xa69b149, 0xec12f93, 0x8a31a38 },
        { 0x97c916a, 0x0d0d369, 0x8885372, 0xdc95f3b, 0x3549040, 0xcf1a261,
          0xabe95a2, 0x60f6f5e, 0xe141325, 0xa909e9f, 0x355c865, 0x7d598f2,
          0x931a9c9, 0x70c6442, 0xb423850, 0x2354a85 }
    },
    {
        { 0x97f9619, 0x4cdd224, 0xc22162e, 0x4776fff, 0x0cd31c2, 0xee5ec33,
          0xf209bb8, 0x7c04c10, 0x579e211, 0x35bbfde, 0x15cdfc2, 0x0e38325,
          0xe26ffa7, 0x657e6d3, 0xc65c604, 0xc66a7c3 },
        { 0xb45e567, 0x322acd7, 0x296db9b, 0x1589cf0, 0xba1db73, 0x1fd0bd3,
          0x9337a40, 0xe882610, 0xb3035c7, 0xf505a50, 0x6ed08d7, 0x4d5af06,
          0x5eda400, 0xb3c376b, 0x1944748, 0x9c7b700 }
    },
    {
        { 0x70c3716, 0xd768325, 0xdd540e0, 0xda62af0, 0x6580fea, 0x76b155d,
          0x32b5464, 0x4f42acc, 0x3f5b72b, 0x881bb60, 0xe68b9ba, 0x09c130e,
          0x5c50342, 0x37ede3b, 0xfd15e7d, 0xce61a9c },
        { 0x72605d0, 0xfff1d85, 0x062abc2, 0x62ac2d3, 0xfbe43dd, 0xa85e02e,
          0xa947020, 0x859d2ba, 0x111c20b, 0x2ebc8a9, 0xa656f66, 0x7f590a7,
          0x16b21a6, 0x0e13843, 0x00c7db6, 0x29b30c5 }
    },
    {
        { 0x906b8de, 0x61e55e2, 0x949974d, 0x6a97e96, 0x26eef67, 0x24b52b5,
          0x1aa595a, 0x512f536, 0x3c48fcb, 0x81cc7b8, 0x28115ad, 0xa64af23,
          0x3d44b8e, 0x9edf6f9, 0x1fe22e3, 0x68d7f7c },
        { 0x520d151, 0x2b2116a, 0x6aa3efb, 0x66a0b7d, 0x9b0f791, 0x48ae70a,
          0x037db88, 0xcf12174, 0x317d9f3, 0x36868cd, 0x22fc344, 0xb573059,
          0x46a5d23, 0xbaa8526, 0x37fc10d, 0xad65691 }
    },
},
{
    {
        { 0x12c78d5, 0xcf8e5f5, 0x805cdbd, 0xeb94d98, 0x2ab50b5, 0xad1dcdf,
          0xf33cd31, 0xf33c136, 0x10aeff5, 0x0d6226b, 0xf2f8fc5, 0xf7ff493,
          0xdf57165, 0x7e520d4, 0x05271a7, 0x41fbae5 },
        { 0x76480ba, 0x72c8987, 0x25f4523, 0x2608359, 0x49f5f01, 0xed36b8d,
          0xf3d49eb, 0x3bc1dce, 0x4940322, 0x30c1c1a, 0x7e0f731, 0x78c1cda,
          0x6d05a31, 0x51f2dc8, 0x07f3522, 0x57b0aa8 }
    },
    {
        { 0x71f88bc, 0x7ab628e, 0x8018f21, 0xcf585f3, 0x13d64f6, 0xdbbe3a4,
          0xec493a5, 0x0f86df1, 0x7725de9, 0x8355e6c, 0xe00fe1e, 0x3954ffe,
          0x9924e32, 0xbb8978f, 0x7812714, 0x1c19298 },
        { 0xaabca8b, 0x7c4ce3e, 0x9bf7019, 0xf861eb5, 0x682e541, 0x31a84fc,
          0xacd1b92, 0x2307ca9, 0x4bf2842, 0x6f8b6ce, 0xcb9f9a9, 0xde252ac,
          0x93c46d1, 0x7f0611d, 0x751dc98, 0x8e2bd80 }
    },
    {
        { 0xe27d54b, 0xf2fd8fb, 0xc248071, 0x2a1e37e, 0xab8f49a, 0x2fcc888,
          0xc18a9e5, 0x42c62a3, 0x70b2446, 0xe302908, 0xc5ac55d, 0x90277fa,
          0xd6dde41, 0x8d97d56, 0x5db04fe, 0xf4cf8a9 },
        { 0xd30d077, 0x3e280f5, 0x3cb3293, 0x2c90307, 0x24eb0dd, 0xe0be2ac,
          0x8bcb4f0, 0xa2d1a49, 0xcd0cd45, 0x16db466, 0x9a80232, 0x3b28aa7,
          0x17b008e, 0xdd7e52f, 0x868e4da, 0x20685f2 }
    },
    {
        { 0x7c7a486, 0x0a68c14, 0xc429633, 0xd8ef234, 0xffe7506, 0x470667b,
          0x8828d51, 0x55a13c8, 0x2e44bef, 0x5f32741, 0x5929f92, 0x537d92a,
          0x31c5cd5, 0x0a01d5b, 0x67eb3d7, 0xb77aa78 },
        { 0x8b82e4d, 0x36ec45f, 0xb37b199, 0x6821da0, 0xd7fa94e, 0x8af37aa,
          0x1085010, 0xf020642, 0x7e56851, 0x9b88678, 0x52948ce, 0x35f3944,
          0xafc1361, 0x125c2ba, 0x453e332, 0x8a57d0e }
    },
    {
        { 0x8043664, 0xefe9948, 0xdb1aa55, 0xb8b8509, 0x332523f, 0x1a2e5a9,
          0x1045c0f, 0x5e255dd, 0x7ae7180, 0xe68dd8a, 0x45bf532, 0x55f1cf3,
          0xe63a716, 0xe00722e, 0x6116bac, 0xd1c2138 },
        { 0x1c6d1f4, 0x626221f, 0x3773278, 0x240b830, 0x88def16, 0xe393a0d,
          0xca0495c, 0x229266e, 0xd3e4608, 0x7b5c6c9, 0x7927190, 0xdc559cb,
          0xc7b3c57, 0x06afe42, 0xb439c9b, 0x8a2ad0b }
    },
    {
        { 0xffc3e2f, 0xd7360fb, 0xfbd2e95, 0xf721317, 0x5748e69, 0x8cacbab,
          0x9054bb9, 0x7c89f27, 0xaa86881, 0xcbe50fa, 0x75206e4, 0x7aa05d3,
          0xc752c66, 0x1ea01bc, 0x1f2c2bc, 0x5968cde },
        { 0x09a853e, 0x487c55f, 0xe09204b, 0x82cbef1, 0xabd8670, 0xad5c492,
          0xf12dcb3, 0x7175963, 0xbf6aa06, 0x7a85762, 0xf8d5237, 0x02e5697,
          0x37c6157, 0xccf7d19, 0xc2fd59c, 0x3b14ca6 }
    },
    {
        { 0x1b9f77f, 0x5e610d8, 0x051b02f, 0x85876d0, 0xb8020dd, 0x5d81c63,
          0xd6ce614, 0xd0b4116, 0xaa8bf0c, 0x91810e5, 0xcbf8c66, 0xf27f91f,
          0x38480ae, 0x2e5dc5f, 0xbec7633, 0x0a13ffe },
        { 0x2bf6af8, 0x61ff649, 0x641f827, 0xe6aef2d, 0x5de5f04, 0xad5708a,
          0xcdfee20, 0xe5c3a80, 0x68fcfa2, 0x88466e2, 0xd6e1d7b, 0x8e5bb3a,
          0xed236b8, 0xa514f06, 0xa5f5274, 0x51c9c7b }
    },
    {
        { 0xf9bc3d8, 0xa19d228, 0x3381069, 0xf89c3f0, 0x5c3f379, 0xfee890e,
          0x32fb857, 0x3d3ef3d, 0x5b418dd, 0x3998849, 0xc46e89a, 0x6786f73,
          0x9e0f12f, 0x79691a5, 0x3bc022b, 0x76916bf },
        { 0x2cd8a0a, 0xea073b6, 0x102fdbc, 0x1fbedd4, 0xcb9d015, 0x1888b14,
          0x76655f7, 0x98f2cfd, 0x59f0494, 0xb9b5910, 0xe6986a3, 0xa3dbbe1,
          0xeaf2b04, 0xef016a5, 0xcd2d876, 0xf671ba7 }
    },
},
{
    {
        { 0x1ae05e9, 0x1dae3bf, 0x1f21fef, 0x6a02996, 0x7aec3c6, 0x95df2b9,
          0xd83189b, 0x9abbc5a, 0x2d13140, 0xaf994af, 0x86aa406, 0xc3f8846,
          0x75284c5, 0xcd77e50, 0x2a9a4d7, 0x1c1e13d },
        { 0x744b89d, 0x7f8815d, 0x2ba673e, 0xb189133, 0xd594570, 0x55ea93c,
          0xd61b041, 0x19c8a18, 0x8d2c580, 0x938ebaa, 0x05ba078, 0x9b4344d,
          0x8eaf9b7, 0x622da43, 0x9fea368, 0x809b807 }
    },
    {
        { 0xc33b7a2, 0x3780e51, 0x387b1c8, 0xd7a205c, 0x4be60e4, 0x79515f8,
          0x1e18277, 0xde02a8b, 0xf0d9150, 0x4645c96, 0xe0b3fd1, 0x45f8acb,
          0x9b53ac3, 0x5d532ba, 0xb0557c9, 0x7984dcd },
        { 0x8a92f01, 0x5ae5ca6, 0x9d569ca, 0xd2fbb3c, 0x0c297c1, 0x668cc57,
          0x6295e89, 0xa482943, 0xa33ad40, 0xf646bc1, 0xc3f425d, 0x066aaa4,
          0xd005de2, 0x23434cd, 0xdb35af4, 0x5aca9e9 }
    },
    {
        { 0x6877c56, 0x2bca35c, 0xf0ddd7d, 0xab864b4, 0x404f46c, 0x5f6aa74,
          0x539c279, 0x72be164, 0xe0283cf, 0x1b1d73e, 0xad583d9, 0xe550f46,
          0xe739ad1, 0x4ac6518, 0x8d42100, 0x6b6def7 },
        { 0xfa8468d, 0x4d36b8c, 0x5a3d7b8, 0x2cb3773, 0x5016281, 0x577f86f,
          0x9124733, 0xdb6fe5f, 0xe29e039, 0xacb6d2a, 0x580b8a1, 0x2ab8330,
          0x643b2d0, 0x130a4ac, 0x5e6884e, 0xa7996e3 }
    },
    {
        { 0x60a0aa8, 0x6fb6277, 0xcbe04f0, 0xe046843, 0xe6ad443, 0xc01d120,
          0xabef2fc, 0xa42a05c, 0x12ff09c, 0x6b793f1, 0xa3e5854, 0x5734ea8,
          0x775f0ad, 0xe482b36, 0xf864a34, 0x2f4f60d },
        { 0x84f2449, 0xf521c58, 0x9186a71, 0x58734a9, 0xac5eacc, 0x157f5d5,
          0x248ee61, 0x858d9a4, 0x48149c3, 0x0727e6d, 0xac9ec50, 0xd5c3eaa,
          0x20ee9b5, 0xa63a64a, 0x87be9de, 0x3f0dfc4 }
    },
    {
        { 0xb13e3f4, 0x836349d, 0x3e9316d, 0xebdd026, 0x324fd6c, 0x3fd61e8,
          0x0964f41, 0x85dddfa, 0x52add1b, 0x06e72de, 0x8c4a9e2, 0xb752cff,
          0xfdf09f7, 0x53b0894, 0x0bc24fd, 0xd5220ab },
        { 0xfb1981a, 0x8442b35, 0x3edd701, 0xa733a37, 0xd0ef089, 0x42b60c3,
          0x46e7bca, 0xa1b16ec, 0xa09aaf4, 0xc0df179, 0x638f3a1, 0xcd4f187,
          0x9eab1c2, 0x9af64f7, 0xd1d78e3, 0x86fed79 }
    },
    {
        { 0xfe29980, 0x42c8d86, 0x6575660, 0x6657b81, 0x80f92ca, 0x82d52c6,
          0x02d42be, 0x8587af1, 0x6e8bdf0, 0xb515131, 0xc333495, 0x706e2d9,
          0x9673064, 0xd53601a, 0x8219099, 0x27b1fbb },
        { 0x705f7c8, 0x3f0929d, 0xf3d6e6f, 0xff40b10, 0x026af5c, 0x673c703,
          0xe25a422, 0x2c1dce4, 0x3dad8b6, 0x5348bd7, 0xbe2c329, 0xc39b6b6,
          0xb921084, 0x47854ff, 0xb391f20, 0xb347b8b }
    },
    {
        { 0xeb9b774, 0x79fc841, 0xb4b6c1d, 0xf32da25, 0xfe492cb, 0xcbba76b,
          0xd623903, 0x76c51fc, 0xcf0705a, 0x114cf6f, 0x7815daf, 0x6b72049,
          0x473382e, 0x630b362, 0x9704db5, 0xbf40c3a },
        { 0xc5456eb, 0xa8a9ddc, 0x72f2dc1, 0x2b4472a, 0xd6d6ef3, 0x9874444,
          0xa0ba5ed, 0x27e8d85, 0x194849f, 0x5d225b4, 0xebaa40d, 0xe852cd6,
          0x8d4bf3f, 0xb669c24, 0x2343991, 0xa8601eb }
    },
    {
        { 0x59502d3, 0x8a04854, 0xe269a7b, 0xcab27ee, 0x4875ada, 0x4179307,
          0xe2405f9, 0x179e685, 0x7b28963, 0x0d7b698, 0x422a43e, 0x80c9db8,
          0xa0f43ee, 0xf5ff318, 0x4ba7aa7, 0x7a92805 },
        { 0x0c0834e, 0xa5c79fe, 0x1f849ec, 0x837ca0d, 0x628ab7b, 0xfe0d7fa,
          0x6edd19a, 0x94bcb95, 0x2226fbf, 0xa18bc93, 0xaad54a3, 0x2795379,
          0x371129e, 0xceeacf8, 0xa588be5, 0x65ca57f }
    },
},
{
    {
        { 0x2caa330, 0x7a578b5, 0xd8ca34a, 0x7c21944, 0x6447282, 0x6c0fbbb,
          0xf90b2e5, 0xa8a9957, 0x6586b71, 0xbbe1066, 0x49138a2, 0x716a902,
          0xe7ed66d, 0x2fa6034, 0x2b9916a, 0x56f77ed },
        { 0xbddefb3, 0x69f1e26, 0x8c08420, 0xa497809, 0x09bc184, 0xc3377eb,
          0xbe6dade, 0x796ce0c, 0xd103bbb, 0x3be0625, 0x992685c, 0x01be27c,
          0x7755f9f, 0xc0e2559, 0x1c0dbfa, 0x165c40d }
    },
    {
        { 0x659c761, 0xc63a397, 0x630fbad, 0x10a0e5b, 0x655ac56, 0xf21e8a6,
          0xc1181e2, 0xe8580fa, 0x0a84b5c, 0xbfc2d9c, 0x7afd5d1, 0x2cdbaff,
          0xf61e85a, 0x95f1182, 0x719eaf4, 0x1173e96 },
        { 0xc6de8b9, 0xc06d55e, 0xafcbcaa, 0x1b4c8eb, 0xbc2bbcd, 0x52af5cb,
          0x77bcd10, 0x564fab8, 0xae85a6e, 0xfd53a18, 0x94c712f, 0x2257859,
          0x1352121, 0x29b11d7, 0xc40491a, 0xab1cb76 }
    },
    {
        { 0xce32eb4, 0xb4e8ca8, 0xb250b49, 0x7e484ac, 0xa3e31a2, 0x062c6f7,
          0x625d1fc, 0x497fd83, 0x362dda7, 0x98f821c, 0x6be3111, 0xcae1f8f,
          0x5d4fa42, 0x9077e95, 0xa65855a, 0xa589971 },
        { 0x28832a9, 0xda6321d, 0x3936e9e, 0xf9ef5dc, 0xc9797ef, 0xa37f117,
          0xdb581be, 0x0eb3c80, 0xbaa0002, 0x207c5c4, 0xf38faa0, 0xc0401b5,
          0xd0f1e6e, 0xceee523, 0xd1f0045, 0x8d27a5f }
    },
    {
        { 0xcf0af29, 0x9411063, 0x89a6693, 0x3043857, 0x640145e, 0x9a9fb8f,
          0x54832eb, 0x7d82fe9, 0x898c520, 0xf2789e1, 0xf948dc0, 0x448b402,
          0x68996dd, 0xeca8fdf, 0xa149b2f, 0x22227e9 },
        { 0x8e62d6a, 0x63509ff, 0x8c9c57f, 0xe98d81c, 0x1fe3bed, 0xd387407,
          0x539538f, 0xf1db013, 0x48418ce, 0xb04092e, 0xd6d9d4d, 0xbbf8e76,
          0x2cec5ae, 0x2ea9cda, 0x5078fa9, 0x8414b3e }
    },
    {
        { 0xd68a073, 0x5ad1cdb, 0xc18b591, 0xd4cedaf, 0x8e4c1c9, 0x7826707,
          0x9ca302a, 0x9b8d920, 0x326115b, 0x3101bd2, 0x4c2717a, 0x6f154b5,
          0x263e84b, 0x618c31b, 0xbbd6942, 0x12c4138 },
        { 0x80da426, 0xf9ead25, 0x47d9680, 0xe748e99, 0x8a4210e, 0x9b396a3,
          0xf4b8f72, 0xfaf03dd, 0x66159e7, 0xbd94a52, 0x1d4c7cb, 0x5e73049,
          0x7910f38, 0x31d1f9a, 0x08d6dd1, 0x4fd10ca }
    },
    {
        { 0x9f2331e, 0x4f510ac, 0x7e3dcc2, 0xee872dc, 0xa0a0c73, 0x4a11a32,
          0xaa5a630, 0x27e5803, 0x7af4a8a, 0xe5ae503, 0x9fffeb0, 0x2dcdeba,
          0x719d91f, 0x8c27748, 0xb9cc61c, 0xd3b5b62 },
        { 0xcca7939, 0x998ac90, 0x64514e5, 0xc22b598, 0xb35738a, 0x950aaa1,
          0xdab0264, 0x4b208bb, 0xa557d2e, 0x6677931, 0xf7c17d3, 0x2c696d8,
          0x3e15c51, 0x1672d4a, 0x3db0e82, 0x95fab66 }
    },
    {
        { 0x6ff205e, 0x3d42734, 0x0ea9fbe, 0x7f187d9, 0x466b2af, 0xbd9367f,
          0x03daf2f, 0x188e532, 0x27b54d8, 0xefe1329, 0xef70435, 0x14faf85,
          0x1ec95c4, 0xa506128, 0xc22cba7, 0xad01705 },
        { 0x6197333, 0x7d2dfa6, 0x8b4f6ed, 0xedd7f07, 0x75df105, 0xe0cb685,
          0x80f76bc, 0x47c9ddb, 0x9073c54, 0x49ab531, 0xe607f44, 0x845255a,
          0xcc74b7c, 0x0b4ed9f, 0x0f5c3a6, 0xcfb52d5 }
    },
    {
        { 0xc278776, 0x545c7c6, 0x98c30f0, 0x92a39ae, 0xd2f4680, 0x8aa8c01,
          0x6b7f840, 0xa5409ed, 0xdcb24e7, 0x0c450ac, 0xc5770d9, 0x5da6fb2,
          0x8658333, 0x5b8e8be, 0x67ea4ad, 0xb26bf4a },
        { 0xc7d91fa, 0x2e30c81, 0x0eeb69f, 0x6e50a49, 0xee4bc26, 0x9458c2b,
          0x33be250, 0x419acf2, 0x87881ab, 0x79d6f81, 0x403b1be, 0x694565d,
          0x234fe1d, 0x34b3990, 0x2132b38, 0x60997d7 }
    },
},
{
    {
        { 0x26975dc, 0x00a9741, 0x6cf94e7, 0x42161c4, 0xc64ed99, 0xcc9fe4b,
          0x4680570, 0x020019a, 0x698da0d, 0x885595a, 0x77dd962, 0x008444b,
          0xa4fea0e, 0xbf3c22d, 0x2c81245, 0xc463048 },
        { 0x793ab18, 0xcb248c5, 0xeb4320b, 0x4dc7a20, 0x1572b7d, 0x9a0906f,
          0xf9ac20f, 0xd5b3019, 0x34520a3, 0x79b1bf5, 0x69b5322, 0x788dfe8,
          0x455b7e2, 0x9a05298, 0x016bca9, 0x2f4aecb }
    },
    {
        { 0x8745618, 0x414d379, 0xb7c983c, 0x64ba22e, 0x9f9d532, 0x9a5d19f,
          0x44a80c8, 0x81a00d8, 0xcae98d6, 0xb9e24f5, 0xaca965a, 0x6c3769c,
          0xf6e4e6d, 0x50d6081, 0x54422a6, 0x0d96980 },
        { 0x5cdd790, 0xbd7e792, 0x6a35219, 0xcff65da, 0x8b60ebe, 0x40dc363,
          0x92a50dc, 0x84bee74, 0x15ad65e, 0x57d4be4, 0x1a6d1d3, 0xc54256b,
          0x45717cc, 0x141c649, 0xcd1c736, 0x05eb609 }
    },
    {
        { 0x1e3c7ec, 0xfd52eab, 0x9f24895, 0xa4a5eca, 0x79fdb83, 0xaaa2a8d,
          0x72bdfda, 0xd105e60, 0x681d97e, 0x59e6ae2, 0x8e8077f, 0xfedf8e0,
          0x629e462, 0xb06d0ad, 0x96fa863, 0x8c7c2d0 },
        { 0xee8fc91, 0x5eecc4c, 0x9e61174, 0x5e83ab2, 0xb28c02d, 0x1fd8925,
          0x2072864, 0x93be538, 0x24c984e, 0xda0c886, 0xa008286, 0xdcf9f0c,
          0xa58ba75, 0x1ecb5a6, 0xc2e3c83, 0x1d9b890 }
    },
    {
        { 0xeeee062, 0x19e866e, 0x4f7b387, 0x31c1c7f, 0x1c06652, 0x9be6018,
          0x2b68bbb, 0xc00a93a, 0x9d52b2b, 0x54c65d6, 0xe8b744a, 0x4591416,
          0x9a64ab6, 0x641bcca, 0xab08098, 0xf22bcb1 },
        { 0xf1f726c, 0x3c0db8f, 0x9d2e6a6, 0x4f5739e, 0x45c9530, 0x5cb669b,
          0x7b472d0, 0x861b04e, 0x894da77, 0x3e30515, 0xc9ac39b, 0x3344685,
          0x73bdd29, 0x9e17305, 0x808dc85, 0x9cac12c }
    },
    {
        { 0x5e27087, 0xf152b86, 0x90a580e, 0x267bd85, 0x8baafc1, 0xba79cec,
          0x9442686, 0x6140ab1, 0x5b31693, 0xa67090c, 0x28b4117, 0x50a103a,
          0x0ddc08f, 0x7722e61, 0xe6569b2, 0x5d19d43 },
        { 0x5962bf6, 0x70e0c52, 0xfb5fb02, 0x808e316, 0x5b667be, 0x3fb80da,
          0xfcfacec, 0x8aa366e, 0x134280e, 0xcb0b3e7, 0xcd7d944, 0x0bf1de4,
          0xd092df5, 0x0cd23be, 0xa153a0c, 0xc9a6a79 }
    },
    {
        { 0x2d5a4b7, 0x1c69ad0, 0xd9e6f4a, 0x4bb28d0, 0xa984fc6, 0x815308c,
          0x9037ca5, 0x40929c7, 0x1bd0357, 0x0ea2b49, 0x42aad4e, 0xec17e5b,
          0x18e7235, 0x1f32ade, 0xa96a9d3, 0xbc60b05 },
        { 0xe20f707, 0x3b0229a, 0x56bdfad, 0xd635050, 0xd8b2e1e, 0xac2d922,
          0x235c748, 0x92b2998, 0xd766f97, 0x6002c3a, 0x1a2a862, 0x9919800,
          0xb58b684, 0x2af7567, 0xaaafce5, 0xd8fe707 }
    },
    {
        { 0x5df7a4b, 0x54487ab, 0xc57ccc2, 0x51cccde, 0x7510b53, 0x2394327,
          0xf555de3, 0x3a09f02, 0x1be484d, 0xa696aec, 0x37817a2, 0x56f459f,
          0x623dcb4, 0x8d8f61c, 0x5335656, 0xc52223c },
        { 0xb49914a, 0xf634111, 0x8e4f9bb, 0xbf8e1ab, 0xf4dba02, 0x2f59578,
          0xe004319, 0x2a94199, 0x654d005, 0x87931f0, 0x6fa0814, 0x7df57d9,
          0xa154031, 0xc8da316, 0x41f658b, 0x2a44ac0 }
    },
    {
        { 0x9e34ac6, 0xfb5f4f8, 0x97790f2, 0x0a1b10b, 0x4b8a06c, 0x58fe4e7,
          0x955f27c, 0x10c1710, 0xd5ebe19, 0x77b798a, 0x1f1c2dc, 0xaf1c35b,
          0xa1f8d69, 0xc25b8e6, 0xf76bf23, 0x49cf751 },
        { 0x436f7b7, 0x15cb2db, 0x7e74d1a, 0x186d7c2, 0xc00a415, 0x60731de,
          0x15f0772, 0xea1e156, 0x714463f, 0xf02d591, 0x51adeb1, 0x26a0c64,
          0xcc5229e, 0x20174cd, 0xefd512a, 0xb817e50 }
    },
},
};

static const ge448_precomp base_i[16] = {
    {
        { 0x70cc05e, 0x26a82bc, 0x0938e26, 0x80e18b0, 0x511433b, 0xf72ab66,
          0x412ae1a, 0xa3d3a46, 0xa6de324, 0x0f1767e, 0x4657047, 0x36da9e1,
          0x5a622bf, 0xed221d1, 0x66bed0d, 0x4f1970c },
        { 0x230fa14, 0x08795bf, 0x7c8ad98, 0x132c4ed, 0x9c4fdbd, 0x1ce67c3,
          0x73ad3ff, 0x05a0c2d, 0x7789c1e, 0xa398408, 0xa73736c, 0xc7624be,
          0x03756c9, 0x2488762, 0x16eb6bc, 0x693f467 }
    },
    {
        { 0x6ff2f8f, 0x2817328, 0xda85757, 0xb769465, 0xfd6e862, 0xf7f6271,
          0x8daa9cb, 0x4a3fcfe, 0x2ba077a, 0xda82c7e, 0x41b8b8c, 0x9433322,
          0x4316cb6, 0x6455bd6, 0xb9108af, 0x0865886 },
        { 0x88ed6fc, 0x22ac135, 0x02dafb8, 0x9a68fed, 0x7f0bffa, 0x1bdb676,
          0x8bb3a33, 0xec4e1d5, 0xce43c82, 0x56c3b9f, 0xa8d9523, 0xa6449a4,
          0xa7ad43a, 0xf706cbd, 0xbd5125c, 0xe005a8d }
    },
    {
        { 0x2030034, 0xa99d109, 0x6f950d0, 0x2d8cefc, 0xc96f07b, 0x7a920c3,
          0x08bc0d5, 0x9588128, 0x6d761e8, 0x62ada75, 0xbcf7285, 0x0def80c,
          0x01eedb5, 0x0e2ba76, 0x5a48dcb, 0x7a9f933 },
        { 0x2f435eb, 0xb473147, 0xf225443, 0x5512881, 0x33c5840, 0xee59d2b,
          0x127d7a4, 0xb698017, 0x86551f7, 0xb18fced, 0xca1823a, 0x0ade260,
          0xce4fd58, 0xd3b9109, 0xa2517ed, 0xadfd751 }
    },
    {
        { 0xeb5eaf7, 0xdf9567c, 0x78ac7d7, 0x110a6b4, 0x4706e0b, 0x2d33501,
          0x0b5a209, 0x0df9c7b, 0x568e684, 0xba4223d, 0x8c3719b, 0xd78af2d,
          0xa5291b6, 0x77467b9, 0x5c89bef, 0x079748e },
        { 0xdac377f, 0xe20d3fa, 0x72b5c09, 0x34e8669, 0xc40bbb7, 0xd8687a3,
          0xd2f84c9, 0x7b3946f, 0xa78f50e, 0xd00e40c, 0x17e7179, 0xb875944,
          0xcb23583, 0x9c7373b, 0xc90fd69, 0x7ddeda3 }
    },
    {
        { 0x6ab686b, 0x3d0def7, 0x49f7c79, 0x1a467ec, 0xc8989ed, 0x3e53f4f,
          0x430a0d9, 0x101e344, 0x8ad44ee, 0xa3ae731, 0xae1d134, 0xaefa6cd,
          0x824ad4d, 0xaa8cd7d, 0xed584fc, 0xef1650c },
        { 0x4f4754f, 0xa74df67, 0xef3fb8b, 0xf52cea8, 0x2971140, 0x47c32d4,
          0xa256fbb, 0x391c15d, 0xa605671, 0xc165fab, 0x87993b9, 0xf2518c6,
          0xbd5a84d, 0x2daf7ac, 0x98f12ae, 0x1560b62 }
    },
    {
        { 0x54dc10a, 0xef4da02, 0x5940db8, 0x6311865, 0x82f2948, 0xe20b149,
          0x5581dba, 0x67b9377, 0x04f5029, 0x422ee71, 0x5122d34, 0x5d440db,
          0x1a4c640, 0xb1e56d7, 0xc2408ee, 0xbf12abb },
        { 0x016af01, 0x0cc9f86, 0xf3d8cab, 0x88366ab, 0xa2efe12, 0x85dda13,
          0x5d00674, 0x390df60, 0x6d187f7, 0xf18f580, 0xf0c5d20, 0x28c900f,
          0x3e01733, 0xad30812, 0x54bf2fd, 0x42d35b5 }
    },
    {
        { 0x2ffb1f1, 0x009135f, 0x8f9c605, 0x099fc7e, 0x26bfa5a, 0xcc67da6,
          0x344552b, 0xc186d12, 0x1b339e1, 0xb523250, 0xc9708c5, 0x70a544f,
          0x1e928e7, 0x06baaec, 0xef0f50f, 0x0baedd2 },
        { 0xbf479e5, 0x535d6d8, 0xe4ec3e9, 0x156e536, 0xddb9be2, 0x3165741,
          0x59fd736, 0x988af71, 0x2e33ddd, 0x13d8a78, 0x4e69002, 0x5460421,
          0x804a268, 0x34d56e0, 0x0e52a4c, 0xc59b84f }
    },
    {
        { 0x24729d9, 0x525d45f, 0x8712327, 0x5768aba, 0x43035db, 0xa25e43b,
          0x927ef21, 0x15a1ee8, 0x6056112, 0xa785d21, 0xd508af9, 0x45e2fbf,
          0x37ba969, 0xb6f721a, 0x216d8d3, 0x30d6d8c },
        { 0x52074c3, 0x3065e08, 0x2a0684e, 0xfa40b4a, 0x763f955, 0x851325a,
          0x9f25900, 0xd4ef19c, 0xf665756, 0x799c869, 0x3312990, 0x7b05222,
          0x28db802, 0xc986c2b, 0x28ade0a, 0xf48fb8f }
    },
    {
        { 0x1649b68, 0x1e46173, 0x5beb9dc, 0xa96e5d6, 0x481935d, 0x765ddff,
          0x9f3bf2a, 0x6cf132c, 0x7c35658, 0x9f6c5c9, 0x4696e60, 0x99cd139,
          0x9c0d5e4, 0x99fa924, 0x8845a95, 0x1acd063 },
        { 0x3636087, 0x0b06541, 0xea17b7f, 0xea20e78, 0x6161967, 0x20afc5f,
          0xdc81028, 0xfd6c8a2, 0xe32c8fd, 0x4ef1357, 0x00e4a88, 0x8aa4004,
          0x48cb82f, 0xd6fcaef, 0xb3cd4fa, 0x7ba7c6d }
    },
    {
        { 0xd19c7ab, 0xf843473, 0xc655c4d, 0x968e76d, 0xc4b9c2f, 0x52c87d9,
          0xe4aa082, 0x65f641a, 0x33c3603, 0x491a397, 0x5810098, 0xa606ffe,
          0x8bf8ad4, 0x09920e6, 0x6db7882, 0x691a0c8 },
        { 0xa4d3ef5, 0x5205883, 0xacf2efe, 0xee839b7, 0xc00ca66, 0x4b78e2a,
          0xf9fcb91, 0xbe3f071, 0xbf6943a, 0x61e66c9, 0x061b79d, 0xe9b4e57,
          0x56c06bd, 0x8d1b01b, 0xdf76ae5, 0x0dfa315 }
    },
    {
        { 0xf1fd093, 0x803df65, 0x489b77e, 0x1cd6523, 0xc20e295, 0x2cd2e15,
          0x9b912d1, 0xcd490be, 0x2e886d2, 0xdd9a2ff, 0xfe9d72a, 0xa3c836d,
          0x298e0c1, 0xfcad5f2, 0x4bcf067, 0xed126e2 },
        { 0x3dc81bc, 0x1e33953, 0xece6a08, 0xbea4d76, 0x991b252, 0x1d15de3,
          0xe6daf97, 0x74cc5cf, 0x0826493, 0x5ad343f, 0x1064049, 0x2d38a47,
          0xffcfa4d, 0xf7f47b9, 0x418066c, 0xef14490 }
    },
    {
        { 0x9bb55ab, 0x4e7f86b, 0x3f496a3, 0x310d785, 0x0dec42c, 0xbd682fc,
          0x411d32a, 0xbde047a, 0xc5a5ea2, 0xea639b4, 0xba08fa1, 0x5052078,
          0x07729f2, 0xc968b23, 0x23d3e28, 0x567b5a6 },
        { 0x977fbf7, 0x171e825, 0xbe990aa, 0x0319c70, 0xe12cd69, 0x8f65023,
          0xf5015e6, 0x1fb9b19, 0x3568a7c, 0x0083f60, 0x1f3c5ac, 0xba3d30b,
          0x3d7a988, 0xe7b509d, 0xcd0f6b6, 0x2318b99 }
    },
    {
        { 0x93ab2cf, 0x54d3b87, 0xd2d8306, 0x366abea, 0xd7a4977, 0x66e8eb6,
          0xae0072e, 0xa61888c, 0xdbc3315, 0x9eeeef5, 0x163e7f5, 0x93f09db,
          0x59ade9a, 0xee90959, 0xce59be0, 0xaf7f578 },
        { 0x5ece59e, 0x24bfd8d, 0x3689523, 0x8aa698b, 0x2de92cf, 0xa9a65de,
          0xa6ad300, 0xec11dbc, 0x09f88ca, 0x217f3fa, 0xb4d6af7, 0xf6c33e3,
          0x1d86d2d, 0xcd3bfa2, 0x5f13f25, 0x1497f83 }
    },
    {
        { 0xcd03d1d, 0xa579568, 0xe158af6, 0xd717cda, 0x389a19f, 0x59eda97,
          0x099e99c, 0xb32c370, 0xdabb591, 0xa2dba91, 0x77c2c97, 0x6d697d5,
          0xd43fa6d, 0x5423fc2, 0x0b382bf, 0x56ea8a5 },
        { 0xd80c11a, 0x4a987ba, 0x7d590a5, 0xe4cde21, 0xf97e559, 0x3dd8860,
          0x43b593c, 0xff45e25, 0x5343cb5, 0x00eb453, 0x7bbfbdd, 0x06b9b99,
          0x16aea24, 0x4da36b7, 0x57a624e, 0x2476517 }
    },
    {
        { 0x3474e0d, 0x32207d0, 0xb41cc73, 0x3ffbf04, 0x319eb39, 0x5c4dc45,
          0x758b463, 0xfee29be, 0xc30c7a7, 0xcc8a381, 0x9fe0e53, 0x147f4e4,
          0xe35a2de, 0x05b2e26, 0x92f3666, 0x4362f02 },
        { 0x8474b85, 0x0476d0c, 0xccaf108, 0x9d8c65f, 0x1d54b6a, 0xf58d404,
          0xf38e4b0, 0x3ee6862, 0x3b44f54, 0x7c7c9d5, 0x0fb0db5, 0x36a3fd8,
          0x18a8ac8, 0xfcd94ba, 0x8f35c05, 0xc1b1d56 }
    },
    {
        { 0x1bdd30d, 0x16539fc, 0x8df4afb, 0x1356e53, 0x5a1aedb, 0xc0545d8,
          0x489396b, 0xeb2037a, 0x5660894, 0x897fcbd, 0xb7d104a, 0x02a58a9,
          0xc96b980, 0x57fa24c, 0x5bd8946, 0xf6448e3 },
        { 0x8805c83, 0xee72741, 0x992cfc6, 0x10fa274, 0x9e66b21, 0x9514193,
          0xbd08009, 0xe0ffa44, 0x20da22b, 0x1743322, 0x59e6831, 0x4891ff3,
          0xa7d687b, 0x407ed73, 0x51d99cf, 0x2fb4e07 }
    },
};
#endif

/* Set the 0 point.
 *
 * p  [in]  Point to set to 0.
 */
static WC_INLINE void ge448_0(ge448_p2 *p)
{
    fe448_0(p->X);
    fe448_1(p->Y);
    fe448_1(p->Z);
}

/* Set the precompute point to 0.
 *
 * p  [in]  Precompute point to set.
 */
static void ge448_precomp_0(ge448_precomp *p)
{
    fe448_0(p->x);
    fe448_1(p->y);
}

/* Double the point on the Twisted Edwards curve. r = 2.p
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to double.
 */
static WC_INLINE void ge448_dbl(ge448_p2 *r,const ge448_p2 *p)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];

    fe448_add(t0, p->X, p->Y);    /* t0 = B1 = X1+Y1     */
    fe448_reduce(t0);
    fe448_sqr(t0, t0);            /* t0 = B = (X1+Y1)^2  */
    fe448_sqr(r->X, p->X);        /* r->X = C = X1^2     */
    fe448_sqr(r->Y, p->Y);        /* r->Y = D = Y1^2     */
    fe448_add(t1, r->X, r->Y);    /* t1 = E = C+D        */
    fe448_reduce(t1);
    fe448_sub(r->Y, r->X, r->Y);  /* r->Y = Y31 = C-D    */
    fe448_sqr(r->Z, p->Z);        /* r->Z = H = Z1^2     */
    fe448_add(r->Z, r->Z, r->Z);  /* r->Z = J1 = 2*H     */
    fe448_sub(r->Z, t1, r->Z);    /* r->Z = J = E-2*H    */
    fe448_reduce(r->Z);
    fe448_sub(r->X, t0, t1);      /* r->X = X31 = B-E    */
    fe448_mul(r->X, r->X, r->Z);  /* r->X = X3 = (B-E)*J */
    fe448_mul(r->Y, r->Y, t1);    /* r->Y = Y3 = E*(C-D) */
    fe448_mul(r->Z, t1, r->Z);    /* r->Z = Z3 = E*J     */
}

/* Add two point on the Twisted Edwards curve. r = p + q
 * Second point has z-ordinate of 1.
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to add.
 * q  [in]  Point to add.
 */
static WC_INLINE void ge448_madd(ge448_p2 *r, const ge448_p2 *p,
                                 const ge448_precomp *q)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];
    ge448 t2[GE448_WORDS];
    ge448 t3[GE448_WORDS];
    ge448 t4[GE448_WORDS];

                                    /* p->Z = A                   */
    fe448_mul(t1, p->X, q->x);      /* t1 = C = X1*X2             */
    fe448_mul(t2, p->Y, q->y);      /* t2 = D = Y1*Y2             */
    fe448_mul(t3, t1, t2);          /* t3 = E1 = C*D              */
    fe448_mul39081(t3, t3);         /* t3 = E = d*C*D             */
    fe448_sqr(t0, p->Z);            /* t0 = B = A^2               */
    fe448_add(t4, t0, t3);          /* t4 = F = B-(-E)            */
    fe448_sub(t0, t0, t3);          /* t0 = G = B+(-E)            */
    fe448_reduce(t0);
    fe448_add(r->X, p->X, p->Y);    /* r->X = H1 = X1+Y1          */
    fe448_reduce(r->X);
    fe448_add(r->Y, q->x, q->y);    /* r->Y = H2 = X2+Y2          */
    fe448_reduce(r->Y);
    fe448_mul(r->X, r->X, r->Y);    /* r->X = H = (X1+Y1)*(X2+Y2) */
    fe448_sub(r->X, r->X, t1);      /* r->X = X31 = H-C           */
    fe448_sub(r->X, r->X, t2);      /* r->X = X32 = H-C-D         */
    fe448_reduce(r->X);
    fe448_mul(r->X, r->X, t4);      /* r->X = X33 = F*(H-C-D)     */
    fe448_mul(r->X, r->X, p->Z);    /* r->X = X3 = A*F*(H-C-D)    */
    fe448_sub(r->Y, t2, t1);        /* r->Y = Y31 = D-C           */
    fe448_reduce(r->Y);
    fe448_mul(r->Y, r->Y, t0);      /* r->Y = Y32 = G*(D-C)       */
    fe448_mul(r->Y, r->Y, p->Z);    /* r->Y = Y3 = A*F*(D-C)      */
    fe448_mul(r->Z, t4, t0);        /* r->Z = Z3 = F*G            */
}

/* Subtract one point from another on the Twisted Edwards curve. r = p - q
 * Second point has z-ordinate of 1.
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to subtract from.
 * q  [in]  Point to subtract.
 */
static WC_INLINE void ge448_msub(ge448_p2 *r, const ge448_p2 *p,
                                 const ge448_precomp *q)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];
    ge448 t2[GE448_WORDS];
    ge448 t3[GE448_WORDS];
    ge448 t4[GE448_WORDS];

                                    /* p->Z = A                   */
    fe448_sqr(t0, p->Z);            /* t0 = B = A^2               */
    fe448_mul(t1, p->X, q->x);      /* t1 = C = X1*X2             */
    fe448_mul(t2, p->Y, q->y);      /* t2 = D = Y1*Y2             */
    fe448_mul(t3, t1, t2);          /* t3 = E1 = C*D              */
    fe448_mul39081(t3, t3);         /* t3 = E = d*C*D             */
    fe448_sub(t4, t0, t3);          /* t4 = F = B-(--E)           */
    fe448_add(t0, t0, t3);          /* t0 = G = B+(--E)           */
    fe448_reduce(t0);
    fe448_add(r->X, p->X, p->Y);    /* r->X = H1 = X1+Y1          */
    fe448_reduce(r->X);
    fe448_sub(r->Y, q->y, q->x);    /* r->Y = H2 = Y2+(-X2)       */
    fe448_reduce(r->Y);
    fe448_mul(r->X, r->X, r->Y);    /* r->X = H = (X1+Y1)*(X2+Y2) */
    fe448_add(r->X, r->X, t1);      /* r->X = X31 = H-(-C)        */
    fe448_sub(r->X, r->X, t2);      /* r->X = X32 = H-(-C)-D      */
    fe448_reduce(r->X);
    fe448_mul(r->X, r->X, t4);      /* r->X = X33 = F*(H-C-D)     */
    fe448_mul(r->X, r->X, p->Z);    /* r->X = X3 = A*F*(H-C-D)    */
    fe448_add(r->Y, t2, t1);        /* r->Y = Y31 = D-C           */
    fe448_reduce(r->Y);
    fe448_mul(r->Y, r->Y, t0);      /* r->Y = Y32 = G*(D-C)       */
    fe448_mul(r->Y, r->Y, p->Z);    /* r->Y = Y3 = A*F*(D-C)      */
    fe448_mul(r->Z, t4, t0);        /* r->Z = Z3 = F*G            */
}

/* Add two point on the Twisted Edwards curve. r = p + q
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to add.
 * q  [in]  Point to add.
 */
static WC_INLINE void ge448_add(ge448_p2* r, const ge448_p2* p,
                                const ge448_p2* q)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];
    ge448 t2[GE448_WORDS];
    ge448 t3[GE448_WORDS];
    ge448 t4[GE448_WORDS];

    fe448_mul(t1, p->X, q->X);      /* t1 = C = X1*X2             */
    fe448_mul(t2, p->Y, q->Y);      /* t2 = D = Y1*Y2             */
    fe448_mul(t3, t1, t2);          /* t3 = E1 = C*D              */
    fe448_mul39081(t3, t3);         /* t3 = E = d*C*D             */
    fe448_mul(r->Z, p->Z, q->Z);    /* r->Z = A = Z1*Z2           */
    fe448_sqr(t0, r->Z);            /* t0 = B = A^2               */
    fe448_add(t4, t0, t3);          /* t4 = F = B-(-E)            */
    fe448_sub(t0, t0, t3);          /* t0 = G = B+(-E)            */
    fe448_reduce(t0);
    fe448_add(r->X, p->X, p->Y);    /* r->X = H1 = X1+Y1          */
    fe448_reduce(r->X);
    fe448_add(r->Y, q->X, q->Y);    /* r->Y = H2 = X2+Y2          */
    fe448_reduce(r->Y);
    fe448_mul(r->X, r->X, r->Y);    /* r->X = H = (X1+Y1)*(X2+Y2) */
    fe448_sub(r->X, r->X, t1);      /* r->X = X31 = H-C           */
    fe448_sub(r->X, r->X, t2);      /* r->X = X32 = H-C-D         */
    fe448_reduce(r->X);
    fe448_mul(r->X, r->X, t4);      /* r->X = X33 = F*(H-C-D)     */
    fe448_mul(r->X, r->X, r->Z);    /* r->X = X3 = A*F*(H-C-D)    */
    fe448_sub(r->Y, t2, t1);        /* r->Y = Y31 = D-C           */
    fe448_reduce(r->Y);
    fe448_mul(r->Y, r->Y, t0);      /* r->Y = Y32 = G*(D-C)       */
    fe448_mul(r->Y, r->Y, r->Z);    /* r->Y = Y3 = A*F*(D-C)      */
    fe448_mul(r->Z, t4, t0);        /* r->Z = Z3 = F*G            */
}

/* Subtract one point from another on the Twisted Edwards curve. r = p - q
 *
 * r  [in]  Point to hold result.
 * p  [in]  Point to subtract from.
 * q  [in]  Point to subtract.
 */
static WC_INLINE void ge448_sub(ge448_p2 *r, const ge448_p2 *p,
                                const ge448_p2 *q)
{
    ge448 t0[GE448_WORDS];
    ge448 t1[GE448_WORDS];
    ge448 t2[GE448_WORDS];
    ge448 t3[GE448_WORDS];
    ge448 t4[GE448_WORDS];

    fe448_mul(t1, p->X, q->X);      /* t1 = C = X1*X2             */
    fe448_mul(t2, p->Y, q->Y);      /* t2 = D = Y1*Y2             */
    fe448_mul(t3, t1, t2);          /* t3 = E1 = C*D              */
    fe448_mul39081(t3, t3);         /* t3 = E = d*C*D             */
    fe448_mul(r->Z, p->Z, q->Z);    /* r->Z = A = Z1*Z2           */
    fe448_sqr(t0, p->Z);            /* t0 = B = A^2               */
    fe448_sub(t4, t0, t3);          /* t4 = F = B-(--E)           */
    fe448_add(t0, t0, t3);          /* t0 = G = B+(--E)           */
    fe448_reduce(t0);
    fe448_add(r->X, p->X, p->Y);    /* r->X = H1 = X1+Y1          */
    fe448_reduce(r->X);
    fe448_sub(r->Y, q->Y, q->X);    /* r->Y = H2 = Y2+(-X2)       */
    fe448_reduce(r->Y);
    fe448_mul(r->X, r->X, r->Y);    /* r->X = H = (X1+Y1)*(X2+Y2) */
    fe448_add(r->X, r->X, t1);      /* r->X = X31 = H-(-C)        */
    fe448_sub(r->X, r->X, t2);      /* r->X = X32 = H-(-C)-D      */
    fe448_reduce(r->X);
    fe448_mul(r->X, r->X, t4);      /* r->X = X33 = F*(H-C-D)     */
    fe448_mul(r->X, r->X, r->Z);    /* r->X = X3 = A*F*(H-C-D)    */
    fe448_add(r->Y, t2, t1);        /* r->Y = Y31 = D-C           */
    fe448_reduce(r->Y);
    fe448_mul(r->Y, r->Y, t0);      /* r->Y = Y32 = G*(D-C)       */
    fe448_mul(r->Y, r->Y, r->Z);    /* r->Y = Y3 = A*F*(D-C)      */
    fe448_mul(r->Z, t4, t0);        /* r->Z = Z3 = F*G            */
}

/* Convert point to byte array assuming projective ordinates.
 *
 * b  [in]  Array of bytes to hold compressed point.
 * p  [in]  Point to convert.
 */
void ge448_to_bytes(byte *b, const ge448_p2 *p)
{
    ge448 recip[GE448_WORDS];
    ge448 x[GE448_WORDS];
    ge448 y[GE448_WORDS];

    fe448_invert(recip, p->Z);
    fe448_mul(x, p->X, recip);
    fe448_mul(y, p->Y, recip);
    fe448_to_bytes(b, y);
    b[56] = (byte)fe448_isnegative(x) << 7;
}

/* Convert point to byte array assuming z is 1.
 *
 * b  [in]  Array of bytes to hold compressed point.
 * p  [in]  Point to convert.
 */
static void ge448_p2z1_to_bytes(byte *b, const ge448_p2 *p)
{
    fe448_to_bytes(b, p->Y);
    b[56] = (byte)fe448_isnegative(p->X) << 7;
}

/* Compress the point to y-ordinate and negative bit.
 *
 * out    [in]  Array of bytes to hold compressed key.
 * xIn    [in]  The x-ordinate.
 * yIn    [in]  The y-ordinate.
 */
int ge448_compress_key(byte* out, const byte* xIn, const byte* yIn)
{
    ge448_p2  g;
    byte      bArray[ED448_KEY_SIZE];
    word32    i;

    fe448_from_bytes(g.X, xIn);
    fe448_from_bytes(g.Y, yIn);
    fe448_1(g.Z);

    ge448_p2z1_to_bytes(bArray, &g);

    for (i = 0; i < 57; i++) {
        out[57 - 1 - i] = bArray[i];
    }

    return 0;
}

/* Determine whether the value is negative.
 *
 * b  [in]  An 8-bit signed value.
 * returns 1 when negative and 0 otherwise.
 */
static byte negative(byte b)
{
    return ((byte)b) >> 7;
}

/* Determine whether two values are equal. a == b
 * Constant time implementation.
 *
 * a  [in]  An 8-bit unsigned value.
 * b  [in]  An 8-bit unsigned value.
 * returns 1 when equal and 0 otherwise.
 */
static byte equal(byte a, byte b)
{
    return (byte)(((word32)(a ^ b) - 1) >> 31);
}

/* Conditional move the point into result point if two values are equal.
 * Constant time implementation.
 *
 * f  [in]  Point to conditionally overwrite.
 * p  [in]  Point to conditionally copy.
 * b  [in]  An 8-bit unsigned value.
 * n  [in]  An 8-bit unsigned value.
 */
static WC_INLINE void cmov(ge448_precomp* r, const ge448_precomp* p, byte b,
                           byte n)
{
    b = equal(b, n);
    fe448_cmov(r->x, p->x, b);
    fe448_cmov(r->y, p->y, b);
}

/* Select one of the entries from the precomputed table and negate if required.
 * Constant time implementation.
 *
 * r    [in]  Point to hold chosen point.
 * pos  [in]  Position of array of entries to choose from.
 * b    [in]  Index of point to select. -ve value means negate the point.
 */
static void ge448_select(ge448_precomp* r, int pos, byte b)
{
    ge448 minusx[16];
    byte bnegative = negative(b);
    byte babs = (byte)(b - (((-bnegative) & b) << 1));

    ge448_precomp_0(r);
    cmov(r, &base[pos][0], babs, 1);
    cmov(r, &base[pos][1], babs, 2);
    cmov(r, &base[pos][2], babs, 3);
    cmov(r, &base[pos][3], babs, 4);
    cmov(r, &base[pos][4], babs, 5);
    cmov(r, &base[pos][5], babs, 6);
    cmov(r, &base[pos][6], babs, 7);
    cmov(r, &base[pos][7], babs, 8);
    fe448_neg(minusx, r->x);
    fe448_cmov(r->x, minusx, bnegative);
}

/* Perform a scalar multiplication of the base point. r = a * base
 *
 * r  [in]  Point to hold result.
 * a  [in]  Scalar to multiply by.
 */
void ge448_scalarmult_base(ge448_p2* r, const byte* a)
{
    byte          carry;
    ge448_precomp t;
    int           i;
    byte          e[113];

    carry = 0;
    for (i = 0; i < 56; ++i) {
        e[2 * i + 0] = ((a[i] >> 0) & 0xf) + carry;
        carry = e[2 * i + 0] + 8;
        carry >>= 4;
        e[2 * i + 0] -= (byte)(carry << 4);

        e[2 * i + 1] = ((a[i] >> 4) & 0xf) + carry;
        carry = e[2 * i + 1] + 8;
        carry >>= 4;
        e[2 * i + 1] -= (byte)(carry << 4);
    }
    e[112] = carry;
    /* each e[i] is between -8 and 8 */

    /* Odd indeces first - sum based on even index so multiply by 16 */
    ge448_select(&t, 0, e[1]);
    fe448_copy(r->X, t.x);
    fe448_copy(r->Y, t.y);
    fe448_1(r->Z);
    for (i = 3; i < 112; i += 2) {
        ge448_select(&t, i / 2, e[i]);
        ge448_madd(r, r, &t);
    }

    ge448_dbl(r, r);
    ge448_dbl(r, r);
    ge448_dbl(r, r);
    ge448_dbl(r, r);

    /* Add even indeces */
    for (i = 0; i <= 112; i += 2) {
        ge448_select(&t, i / 2, e[i]);
        ge448_madd(r, r, &t);
    }
}

/* Create to a sliding window for the scalar multiplicaton.
 *
 * r  [in]  Array of indeces.
 * a  [in]  Scalar to break up.
 */
static void slide(sword8 *r, const byte *a)
{
    int i;
    int b;
    int k;

    for (i = 0; i < 448; ++i) {
        r[i] = (a[i >> 3] >> (i & 7)) & 1;
    }

    for (i = 0; i < 448; ++i) {
        if (r[i] == 0) {
            continue;
        }

        for (b = 1; b <= 7 && i + b < 448; ++b) {
            if (r[i + b] == 0) {
                continue;
            }

            if (r[i] + (r[i + b] << b) <= 31) {
                r[i] += (sword8)(r[i + b] << b);
                r[i + b] = 0;
            }
            else if (r[i] - (r[i + b] << b) >= -31) {
                r[i] -= (sword8)(r[i + b] << b);
                for (k = i + b; k < 448; ++k) {
                    if (!r[k]) {
                        r[k] = 1;
                        break;
                    }
                    r[k] = 0;
                }
            }
            else {
                break;
            }
        }
    }
}

/* Perform a scalar multiplication of the base point and public point.
 *   r = a * p + b * base
 * Uses a sliding window of 5 bits.
 * Not constant time.
 *
 * r  [in]  Point to hold result.
 * a  [in]  Scalar to multiply by.
 */
int ge448_double_scalarmult_vartime(ge448_p2 *r, const byte *a,
                                    const ge448_p2 *p, const byte *b)
{
#define GE448_WINDOW_BUF_SIZE 448

#if defined(WOLFSSL_SMALL_STACK) && (!defined(WOLFSSL_NO_MALLOC) ||                                                           defined(XMALLOC_USER))
    sword8       *aslide = NULL;
    sword8       *bslide = NULL;
    ge448_p2     *pi = NULL; /* p,3p,..,31p */
    ge448_p2     *p2 = NULL;
#else
    sword8       aslide[448];
    sword8       bslide[448];
    ge448_p2     pi[16]; /* p,3p,..,31p */
    ge448_p2     p2[1];
#endif
    int          i;
    int          ret = 0;

#if defined(WOLFSSL_SMALL_STACK) && (!defined(WOLFSSL_NO_MALLOC) ||                                                           defined(XMALLOC_USER))
    aslide = (sword8 *)XMALLOC(GE448_WINDOW_BUF_SIZE, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (aslide == NULL) {
        ret = MEMORY_E;
    }
    if (ret == 0) {
        bslide = (sword8 *)XMALLOC(GE448_WINDOW_BUF_SIZE, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (bslide == NULL) {
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        pi = (ge448_p2 *)XMALLOC(16 * sizeof *pi, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (pi == NULL) {
            ret = MEMORY_E;
        }
    }
    if (ret == 0) {
        p2 = (ge448_p2 *)XMALLOC(sizeof *p2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (p2 == NULL) {
            ret = MEMORY_E;
        }
    }
#endif

    if (ret == 0) {
        slide(aslide, a);
        slide(bslide, b);

        fe448_copy(pi[0].X, p->X);
        fe448_copy(pi[0].Y, p->Y);
        fe448_copy(pi[0].Z, p->Z);
        ge448_dbl(p2, p);
        ge448_add(&pi[1], p2, &pi[0]);
        ge448_add(&pi[2], p2, &pi[1]);
        ge448_add(&pi[3], p2, &pi[2]);
        ge448_add(&pi[4], p2, &pi[3]);
        ge448_add(&pi[5], p2, &pi[4]);
        ge448_add(&pi[6], p2, &pi[5]);
        ge448_add(&pi[7], p2, &pi[6]);
        ge448_add(&pi[8], p2, &pi[7]);
        ge448_add(&pi[9], p2, &pi[8]);
        ge448_add(&pi[10], p2, &pi[9]);
        ge448_add(&pi[11], p2, &pi[10]);
        ge448_add(&pi[12], p2, &pi[11]);
        ge448_add(&pi[13], p2, &pi[12]);
        ge448_add(&pi[14], p2, &pi[13]);
        ge448_add(&pi[15], p2, &pi[14]);

        ge448_0(r);

        /* Find first index that is not 0. */
        for (i = 447; i >= 0; --i) {
            if (aslide[i] || bslide[i]) {
                break;
            }
        }

        for (; i >= 0; --i) {
            ge448_dbl(r, r);

            if (aslide[i] > 0)
                ge448_add(r, r, &pi[aslide[i]/2]);
            else if (aslide[i] < 0)
                ge448_sub(r, r ,&pi[(-aslide[i])/2]);

            if (bslide[i] > 0)
                ge448_madd(r, r, &base_i[bslide[i]/2]);
            else if (bslide[i] < 0)
                ge448_msub(r, r, &base_i[(-bslide[i])/2]);
        }
    }

#if defined(WOLFSSL_SMALL_STACK) && (!defined(WOLFSSL_NO_MALLOC) ||                                                           defined(XMALLOC_USER))
    if (p2 != NULL) {
        XFREE(p2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (pi != NULL) {
        XFREE(pi, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (bslide != NULL) {
        XFREE(bslide, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (aslide != NULL) {
        XFREE(aslide, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif

    return ret;
}

/* Convert compressed point to negative of affine point.
 * Calculates x from the y and the negative bit.
 * Not constant time.
 *
 * r  [in]  Uncompressed point.
 * b  [in]  Array of bytes representing point.
 * returns 0 on success and -1 on failure.
 */
int ge448_from_bytes_negate_vartime(ge448_p2 *r, const byte *b)
{
    int   ret = 0;
    ge448 u[GE448_WORDS];
    ge448 v[GE448_WORDS];
    ge448 u3[GE448_WORDS];
    ge448 vxx[GE448_WORDS];
    ge448 check[GE448_WORDS];

    fe448_from_bytes(r->Y, b);
    fe448_1(r->Z);
    fe448_sqr(u, r->Y);                /* u = y^2                      */
    fe448_mul39081(v, u);              /* v = 39081.y^2                */
    fe448_sub(u, u, r->Z);             /* u = y^2-1                    */
    fe448_reduce(u);
    fe448_add(v, v, r->Z);             /* v = 39081.y^2-1              */
    fe448_reduce(v);
    fe448_neg(v, v);                   /* v = -39081.y^2-1 = d.y^2-1   */

    fe448_sqr(r->X, v);                /* x = v^2                      */
    fe448_mul(r->X, r->X, v);          /* x = v^3                      */
    fe448_sqr(u3, u);                  /* x = u^2.v^3                  */
    fe448_mul(r->X, r->X, u3);         /* x = u^2.v^3                  */
    fe448_mul(u3, u3, u);              /* u3 = u^3                     */
    fe448_mul(r->X, r->X, u3);         /* x = u^5.v^3                  */

    fe448_pow_2_446_222_1(r->X, r->X); /* x = (u^5.v^3)^((q-3)/4)      */
    fe448_mul(r->X, r->X, u3);         /* x = u^3(u^5.v^3)^((q-3)/4)   */
    fe448_mul(r->X, r->X, v);          /* x = u^3.v(u^5.v^3)^((q-3)/4) */

    fe448_sqr(vxx, r->X);
    fe448_mul(vxx, vxx, v);
    fe448_sub(check, vxx, u);          /* check = v.x^2-u              */
    fe448_reduce(check);
    /* Note; vx^2+u is NOT correct. */
    if (fe448_isnonzero(check)) {
        ret = -1;
    }

    /* Calculating negative of point in bytes - negate only if X is correct. */
    if (fe448_isnegative(r->X) == (b[56] >> 7)) {
        fe448_neg(r->X, r->X);
    }

    return ret;
}

#endif /* ED448_SMALL */
#endif /* HAVE_CURVE448 || HAVE_ED448 */
