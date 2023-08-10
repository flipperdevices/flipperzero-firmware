/* fe_x25519_128.i
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

/* Generated using (from wolfssl):
 *   cd ../scripts
 *   ruby ./x25519/fe_x25519_128_gen.rb > ../wolfssl/wolfcrypt/src/fe_x25519_128.i
 */

void fe_init(void)
{
}

/* Convert a number represented as an array of bytes to an array of words with
 * 51-bits of data in each word.
 *
 * in   An array of bytes.
 * out  An array of words.
 */
void fe_frombytes(fe out, const unsigned char *in)
{
    out[0] = (((sword64)((in[ 0]      )       ))      )
           | (((sword64)((in[ 1]      )       )) <<  8)
           | (((sword64)((in[ 2]      )       )) << 16)
           | (((sword64)((in[ 3]      )       )) << 24)
           | (((sword64)((in[ 4]      )       )) << 32)
           | (((sword64)((in[ 5]      )       )) << 40)
           | (((sword64)((in[ 6]      ) & 0x07)) << 48);
    out[1] = (((sword64)((in[ 6] >>  3) & 0x1f))      )
           | (((sword64)((in[ 7]      )       )) <<  5)
           | (((sword64)((in[ 8]      )       )) << 13)
           | (((sword64)((in[ 9]      )       )) << 21)
           | (((sword64)((in[10]      )       )) << 29)
           | (((sword64)((in[11]      )       )) << 37)
           | (((sword64)((in[12]      ) & 0x3f)) << 45);
    out[2] = (((sword64)((in[12] >>  6) & 0x03))      )
           | (((sword64)((in[13]      )       )) <<  2)
           | (((sword64)((in[14]      )       )) << 10)
           | (((sword64)((in[15]      )       )) << 18)
           | (((sword64)((in[16]      )       )) << 26)
           | (((sword64)((in[17]      )       )) << 34)
           | (((sword64)((in[18]      )       )) << 42)
           | (((sword64)((in[19]      ) & 0x01)) << 50);
    out[3] = (((sword64)((in[19] >>  1) & 0x7f))      )
           | (((sword64)((in[20]      )       )) <<  7)
           | (((sword64)((in[21]      )       )) << 15)
           | (((sword64)((in[22]      )       )) << 23)
           | (((sword64)((in[23]      )       )) << 31)
           | (((sword64)((in[24]      )       )) << 39)
           | (((sword64)((in[25]      ) & 0x0f)) << 47);
    out[4] = (((sword64)((in[25] >>  4) & 0x0f))      )
           | (((sword64)((in[26]      )       )) <<  4)
           | (((sword64)((in[27]      )       )) << 12)
           | (((sword64)((in[28]      )       )) << 20)
           | (((sword64)((in[29]      )       )) << 28)
           | (((sword64)((in[30]      )       )) << 36)
           | (((sword64)((in[31]      ) & 0x7f)) << 44);
}

/* Convert a number represented as an array of words to an array of bytes.
 * The array of words is normalized to an array of 51-bit data words and if
 * greater than the mod, modulo reduced by the prime 2^255 - 1.
 *
 * n    An array of words.
 * out  An array of bytes.
 */
void fe_tobytes(unsigned char *out, const fe n)
{
    fe      in;
    sword64 c;

    in[0] = n[0];
    in[1] = n[1];
    in[2] = n[2];
    in[3] = n[3];
    in[4] = n[4];

    /* Normalize to 51-bits of data per word. */
    in[0] += (in[4] >> 51) * 19; in[4] &= 0x7ffffffffffff;

    in[1] += in[0] >> 51; in[0] &= 0x7ffffffffffff;
    in[2] += in[1] >> 51; in[1] &= 0x7ffffffffffff;
    in[3] += in[2] >> 51; in[2] &= 0x7ffffffffffff;
    in[4] += in[3] >> 51; in[3] &= 0x7ffffffffffff;
    in[0] += (in[4] >> 51) * 19;
    in[4] &= 0x7ffffffffffff;

    c = (in[0] + 19) >> 51;
    c = (in[1] + c) >> 51;
    c = (in[2] + c) >> 51;
    c = (in[3] + c) >> 51;
    c = (in[4] + c) >> 51;
    in[0] += c * 19;
    in[1] += in[0] >> 51; in[0] &= 0x7ffffffffffff;
    in[2] += in[1] >> 51; in[1] &= 0x7ffffffffffff;
    in[3] += in[2] >> 51; in[2] &= 0x7ffffffffffff;
    in[4] += in[3] >> 51; in[3] &= 0x7ffffffffffff;
    in[4] &= 0x7ffffffffffff;

    out[ 0] = (((byte)((in[0]      )       ))      );
    out[ 1] = (((byte)((in[0] >>  8)       ))      );
    out[ 2] = (((byte)((in[0] >> 16)       ))      );
    out[ 3] = (((byte)((in[0] >> 24)       ))      );
    out[ 4] = (((byte)((in[0] >> 32)       ))      );
    out[ 5] = (((byte)((in[0] >> 40)       ))      );
    out[ 6] = (((byte)((in[0] >> 48) & 0x07))      )
            | (((byte)((in[1]      ) & 0x1f)) <<  3);
    out[ 7] = (((byte)((in[1] >>  5)       ))      );
    out[ 8] = (((byte)((in[1] >> 13)       ))      );
    out[ 9] = (((byte)((in[1] >> 21)       ))      );
    out[10] = (((byte)((in[1] >> 29)       ))      );
    out[11] = (((byte)((in[1] >> 37)       ))      );
    out[12] = (((byte)((in[1] >> 45) & 0x3f))      )
            | (((byte)((in[2]      ) & 0x03)) <<  6);
    out[13] = (((byte)((in[2] >>  2)       ))      );
    out[14] = (((byte)((in[2] >> 10)       ))      );
    out[15] = (((byte)((in[2] >> 18)       ))      );
    out[16] = (((byte)((in[2] >> 26)       ))      );
    out[17] = (((byte)((in[2] >> 34)       ))      );
    out[18] = (((byte)((in[2] >> 42)       ))      );
    out[19] = (((byte)((in[2] >> 50) & 0x01))      )
            | (((byte)((in[3]      ) & 0x7f)) <<  1);
    out[20] = (((byte)((in[3] >>  7)       ))      );
    out[21] = (((byte)((in[3] >> 15)       ))      );
    out[22] = (((byte)((in[3] >> 23)       ))      );
    out[23] = (((byte)((in[3] >> 31)       ))      );
    out[24] = (((byte)((in[3] >> 39)       ))      );
    out[25] = (((byte)((in[3] >> 47) & 0x0f))      )
            | (((byte)((in[4]      ) & 0x0f)) <<  4);
    out[26] = (((byte)((in[4] >>  4)       ))      );
    out[27] = (((byte)((in[4] >> 12)       ))      );
    out[28] = (((byte)((in[4] >> 20)       ))      );
    out[29] = (((byte)((in[4] >> 28)       ))      );
    out[30] = (((byte)((in[4] >> 36)       ))      );
    out[31] = (((byte)((in[4] >> 44) & 0x7f))      );
}

/* Set the field element to 1.
 *
 * n  The field element number.
 */
void fe_1(fe n)
{
    n[0] = 0x0000000000001;
    n[1] = 0x0000000000000;
    n[2] = 0x0000000000000;
    n[3] = 0x0000000000000;
    n[4] = 0x0000000000000;
}

/* Set the field element to 0.
 *
 * n  The field element number.
 */
void fe_0(fe n)
{
    n[0] = 0x0000000000000;
    n[1] = 0x0000000000000;
    n[2] = 0x0000000000000;
    n[3] = 0x0000000000000;
    n[4] = 0x0000000000000;
}

/* Copy field element a into field element r.
 *
 * r  Field element to copy into.
 * a  Field element to copy.
 */
void fe_copy(fe r, const fe a)
{
    r[0] = a[0];
    r[1] = a[1];
    r[2] = a[2];
    r[3] = a[3];
    r[4] = a[4];
}

/* Constant time, conditional swap of field elements a and b.
 *
 * f  A field element.
 * g  A field element.
 * b  If 1 then swap and if 0 then don't swap.
 */
void fe_cswap(fe f, fe g, int b)
{
    sword64 m = b;
    sword64 t0, t1, t2, t3, t4;

    /* Convert conditional into mask. */
    m = -m;
    t0 = m & (f[0] ^ g[0]);
    t1 = m & (f[1] ^ g[1]);
    t2 = m & (f[2] ^ g[2]);
    t3 = m & (f[3] ^ g[3]);
    t4 = m & (f[4] ^ g[4]);

    f[0] ^= t0;
    f[1] ^= t1;
    f[2] ^= t2;
    f[3] ^= t3;
    f[4] ^= t4;

    g[0] ^= t0;
    g[1] ^= t1;
    g[2] ^= t2;
    g[3] ^= t3;
    g[4] ^= t4;
}

/* Subtract b from a into r. (r = a - b)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_sub(fe r, const fe a, const fe b)
{
    r[0] = a[0] - b[0];
    r[1] = a[1] - b[1];
    r[2] = a[2] - b[2];
    r[3] = a[3] - b[3];
    r[4] = a[4] - b[4];
}

/* Add b to a into r. (r = a + b)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_add(fe r, const fe a, const fe b)
{
    r[0] = a[0] + b[0];
    r[1] = a[1] + b[1];
    r[2] = a[2] + b[2];
    r[3] = a[3] + b[3];
    r[4] = a[4] + b[4];
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_mul(fe r, const fe a, const fe b)
{
    const __int128_t k19 = 19;
    __int128_t t0 = ((__int128_t)a[0]) * b[0];
    __int128_t t1 = ((__int128_t)a[0]) * b[1]
                  + ((__int128_t)a[1]) * b[0];
    __int128_t t2 = ((__int128_t)a[0]) * b[2]
                  + ((__int128_t)a[1]) * b[1]
                  + ((__int128_t)a[2]) * b[0];
    __int128_t t3 = ((__int128_t)a[0]) * b[3]
                  + ((__int128_t)a[1]) * b[2]
                  + ((__int128_t)a[2]) * b[1]
                  + ((__int128_t)a[3]) * b[0];
    __int128_t t4 = ((__int128_t)a[0]) * b[4]
                  + ((__int128_t)a[1]) * b[3]
                  + ((__int128_t)a[2]) * b[2]
                  + ((__int128_t)a[3]) * b[1]
                  + ((__int128_t)a[4]) * b[0];
    __int128_t t5 = ((__int128_t)a[1]) * b[4]
                  + ((__int128_t)a[2]) * b[3]
                  + ((__int128_t)a[3]) * b[2]
                  + ((__int128_t)a[4]) * b[1];
    __int128_t t6 = ((__int128_t)a[2]) * b[4]
                  + ((__int128_t)a[3]) * b[3]
                  + ((__int128_t)a[4]) * b[2];
    __int128_t t7 = ((__int128_t)a[3]) * b[4]
                  + ((__int128_t)a[4]) * b[3];
    __int128_t t8 = ((__int128_t)a[4]) * b[4];

    /* Modulo reduce double long word. */
    t0 += t5 * k19;
    t1 += t6 * k19;
    t2 += t7 * k19;
    t3 += t8 * k19;

    /* Normalize to 51-bits of data per word. */
    t0 += (t4 >> 51) * k19; t4 &= 0x7ffffffffffff;

    t1 += t0 >> 51; r[0] = t0 & 0x7ffffffffffff;
    t2 += t1 >> 51; r[1] = t1 & 0x7ffffffffffff;
    t3 += t2 >> 51; r[2] = t2 & 0x7ffffffffffff;
    t4 += t3 >> 51; r[3] = t3 & 0x7ffffffffffff;
    r[0] += (sword64)((t4 >> 51) * k19);
    r[4] = t4 & 0x7ffffffffffff;
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_sq(fe r, const fe a)
{
    const __int128_t k19 = 19;
    const __int128_t k2 = 2;
    __int128_t t0 = ((__int128_t)a[0]) * a[0];
    __int128_t t1 = ((__int128_t)a[0]) * a[1] * k2;
    __int128_t t2 = ((__int128_t)a[0]) * a[2] * k2
                  + ((__int128_t)a[1]) * a[1];
    __int128_t t3 = ((__int128_t)a[0]) * a[3] * k2
                  + ((__int128_t)a[1]) * a[2] * k2;
    __int128_t t4 = ((__int128_t)a[0]) * a[4] * k2
                  + ((__int128_t)a[1]) * a[3] * k2
                  + ((__int128_t)a[2]) * a[2];
    __int128_t t5 = ((__int128_t)a[1]) * a[4] * k2
                  + ((__int128_t)a[2]) * a[3] * k2;
    __int128_t t6 = ((__int128_t)a[2]) * a[4] * k2
                  + ((__int128_t)a[3]) * a[3];
    __int128_t t7 = ((__int128_t)a[3]) * a[4] * k2;
    __int128_t t8 = ((__int128_t)a[4]) * a[4];

    /* Modulo reduce double long word. */
    t0 += t5 * k19;
    t1 += t6 * k19;
    t2 += t7 * k19;
    t3 += t8 * k19;

    /* Normalize to 51-bits of data per word. */
    t0 += (t4 >> 51) * k19; t4 &= 0x7ffffffffffff;

    t1 += t0 >> 51; r[0] = t0 & 0x7ffffffffffff;
    t2 += t1 >> 51; r[1] = t1 & 0x7ffffffffffff;
    t3 += t2 >> 51; r[2] = t2 & 0x7ffffffffffff;
    t4 += t3 >> 51; r[3] = t3 & 0x7ffffffffffff;
    r[0] += (sword64)((t4 >> 51) * k19);
    r[4] = t4 & 0x7ffffffffffff;
}

/* Multiply a by 121666 and put result in r. (r = 121666 * a)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_mul121666(fe r, fe a)
{
    const __int128_t k19 = 19;
    const __int128_t k121666 = 121666;
    __int128_t t0 = ((__int128_t)a[0]) * k121666;
    __int128_t t1 = ((__int128_t)a[1]) * k121666;
    __int128_t t2 = ((__int128_t)a[2]) * k121666;
    __int128_t t3 = ((__int128_t)a[3]) * k121666;
    __int128_t t4 = ((__int128_t)a[4]) * k121666;

    /* Normalize to 51-bits of data per word. */
    t0 += (t4 >> 51) * k19; t4 &= 0x7ffffffffffff;

    t1 += t0 >> 51; r[0] = t0 & 0x7ffffffffffff;
    t2 += t1 >> 51; r[1] = t1 & 0x7ffffffffffff;
    t3 += t2 >> 51; r[2] = t2 & 0x7ffffffffffff;
    t4 += t3 >> 51; r[3] = t3 & 0x7ffffffffffff;
    r[0] += (sword64)((t4 >> 51) * k19);
    r[4] = t4 & 0x7ffffffffffff;
}

/* Find the inverse of a modulo 2^255 - 1 and put result in r.
 * (r * a) mod (2^255 - 1) = 1
 * Implementation is constant time.
 *
 * r  A field element.
 * a  A field element.
 */
void fe_invert(fe r, const fe a)
{
    fe  t0, t1, t2, t3;
    int i;

    /* a ^ (2^255 - 21) */
    fe_sq(t0,  a); for (i = 1; i <   1; ++i) fe_sq(t0, t0);
    fe_sq(t1, t0); for (i = 1; i <   2; ++i) fe_sq(t1, t1); fe_mul(t1,  a, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t2, t0); for (i = 1; i <   1; ++i) fe_sq(t2, t2); fe_mul(t1, t1, t2);
    fe_sq(t2, t1); for (i = 1; i <   5; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t2, t1); for (i = 1; i <  10; ++i) fe_sq(t2, t2); fe_mul(t2, t2, t1);
    fe_sq(t3, t2); for (i = 1; i <  20; ++i) fe_sq(t3, t3); fe_mul(t2, t3, t2);
    fe_sq(t2, t2); for (i = 1; i <  10; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t2, t1); for (i = 1; i <  50; ++i) fe_sq(t2, t2); fe_mul(t2, t2, t1);
    fe_sq(t3, t2); for (i = 1; i < 100; ++i) fe_sq(t3, t3); fe_mul(t2, t3, t2);
    fe_sq(t2, t2); for (i = 1; i <  50; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t1, t1); for (i = 1; i <   5; ++i) fe_sq(t1, t1); fe_mul( r, t1, t0);
}

#ifndef CURVE25519_SMALL
/* Scalar multiply the field element a by n using Montgomery Ladder and places
 * result in r.
 *
 * r  A field element as an array of bytes.
 * n  The scalar as an array of bytes.
 * a  A field element as an array of bytes.
 */
int curve25519(byte* r, const byte* n, const byte* a)
{
    fe           x1, x2, z2, x3, z3;
    fe           t0, t1;
    int          pos;
    unsigned int swap;
    unsigned int b;

    fe_frombytes(x1, a);
    fe_1(x2);
    fe_0(z2);
    fe_copy(x3, x1);
    fe_1(z3);

    swap = 0;
    for (pos = 254;pos >= 0;--pos) {
        b = n[pos / 8] >> (pos & 7);
        b &= 1;
        swap ^= b;
        fe_cswap(x2, x3, (int)swap);
        fe_cswap(z2, z3, (int)swap);
        swap = b;

        fe_sub(t0, x3, z3);
        fe_sub(t1, x2, z2);
        fe_add(x2, x2, z2);
        fe_add(z2, x3, z3);
        fe_mul(z3, t0, x2);
        fe_mul(z2, z2, t1);
        fe_sq(t0, t1);
        fe_sq(t1, x2);
        fe_add(x3, z3, z2);
        fe_sub(z2, z3, z2);
        fe_mul(x2, t1, t0);
        fe_sub(t1, t1, t0);
        fe_sq(z2, z2);
        fe_mul121666(z3, t1);
        fe_sq(x3, x3);
        fe_add(t0, t0, z3);
        fe_mul(z3, x1, z2);
        fe_mul(z2, t1, t0);
    }
    fe_cswap(x2, x3, (int)swap);
    fe_cswap(z2, z3, (int)swap);

    fe_invert(z2, z2);
    fe_mul(x2, x2, z2);
    fe_tobytes(r, x2);

    return 0;
}
#endif /* !CURVE25519_SMALL */

/* The field element value 0 as an array of bytes. */
static const unsigned char zero[32] = {0};

/* Constant time check as to whether a is not 0.
 *
 * a  A field element.
 */
int fe_isnonzero(const fe a)
{
    unsigned char s[32];
    fe_tobytes(s, a);
    return ConstantCompare(s, zero, 32);
}

/* Checks whether a is negative.
 *
 * a  A field element.
 */
int fe_isnegative(const fe a)
{
    unsigned char s[32];
    fe_tobytes(s, a);
    return s[0] & 1;
}

/* Negates field element a and stores the result in r.
 *
 * r  A field element.
 * a  A field element.
 */
void fe_neg(fe r, const fe a)
{
    r[0] = -a[0];
    r[1] = -a[1];
    r[2] = -a[2];
    r[3] = -a[3];
    r[4] = -a[4];
}

/* Constant time, conditional move of b into a.
 * a is not changed if the condition is 0.
 *
 * f  A field element.
 * g  A field element.
 * b  If 1 then copy and if 0 then don't copy.
 */
void fe_cmov(fe f, const fe g, int b)
{
    sword64 m = b;
    sword64 t0, t1, t2, t3, t4;

    /* Convert conditional into mask. */
    m = -m;
    t0 = m & (f[0] ^ g[0]);
    t1 = m & (f[1] ^ g[1]);
    t2 = m & (f[2] ^ g[2]);
    t3 = m & (f[3] ^ g[3]);
    t4 = m & (f[4] ^ g[4]);

    f[0] ^= t0;
    f[1] ^= t1;
    f[2] ^= t2;
    f[3] ^= t3;
    f[4] ^= t4;
}

void fe_pow22523(fe r, const fe a)
{
    fe t0, t1, t2;
    int i;

    /* a ^ (2^255 - 23) */
    fe_sq(t0,  a); for (i = 1; i <   1; ++i) fe_sq(t0, t0);
    fe_sq(t1, t0); for (i = 1; i <   2; ++i) fe_sq(t1, t1); fe_mul(t1,  a, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t0, t0); for (i = 1; i <   1; ++i) fe_sq(t0, t0); fe_mul(t0, t1, t0);
    fe_sq(t1, t0); for (i = 1; i <   5; ++i) fe_sq(t1, t1); fe_mul(t0, t1, t0);
    fe_sq(t1, t0); for (i = 1; i <  10; ++i) fe_sq(t1, t1); fe_mul(t1, t1, t0);
    fe_sq(t2, t1); for (i = 1; i <  20; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t1, t1); for (i = 1; i <  10; ++i) fe_sq(t1, t1); fe_mul(t0, t1, t0);
    fe_sq(t1, t0); for (i = 1; i <  50; ++i) fe_sq(t1, t1); fe_mul(t1, t1, t0);
    fe_sq(t2, t1); for (i = 1; i < 100; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t1, t1); for (i = 1; i <  50; ++i) fe_sq(t1, t1); fe_mul(t0, t1, t0);
    fe_sq(t0, t0); for (i = 1; i <   2; ++i) fe_sq(t0, t0); fe_mul( r, t0, a);

    return;
}

/* Double the square of a and put result in r. (r = 2 * a * a)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_sq2(fe r, const fe a)
{
    const __int128_t k2 = 2;
    const __int128_t k19 = 19;
    __int128_t t0 = k2 * (((__int128_t)a[0]) * a[0]);
    __int128_t t1 = k2 * (((__int128_t)a[0]) * a[1] * k2);
    __int128_t t2 = k2 * (((__int128_t)a[0]) * a[2] * k2
                  + ((__int128_t)a[1]) * a[1]);
    __int128_t t3 = k2 * (((__int128_t)a[0]) * a[3] * k2
                  + ((__int128_t)a[1]) * a[2] * k2);
    __int128_t t4 = k2 * (((__int128_t)a[0]) * a[4] * k2
                  + ((__int128_t)a[1]) * a[3] * k2
                  + ((__int128_t)a[2]) * a[2]);
    __int128_t t5 = k2 * (((__int128_t)a[1]) * a[4] * k2
                  + ((__int128_t)a[2]) * a[3] * k2);
    __int128_t t6 = k2 * (((__int128_t)a[2]) * a[4] * k2
                  + ((__int128_t)a[3]) * a[3]);
    __int128_t t7 = k2 * (((__int128_t)a[3]) * a[4] * k2);
    __int128_t t8 = k2 * (((__int128_t)a[4]) * a[4]);

    /* Modulo reduce double long word. */
    t0 += t5 * k19;
    t1 += t6 * k19;
    t2 += t7 * k19;
    t3 += t8 * k19;

    /* Normalize to 51-bits of data per word. */
    t0 += (t4 >> 51) * k19; t4 &= 0x7ffffffffffff;

    t1 += t0 >> 51; r[0] = t0 & 0x7ffffffffffff;
    t2 += t1 >> 51; r[1] = t1 & 0x7ffffffffffff;
    t3 += t2 >> 51; r[2] = t2 & 0x7ffffffffffff;
    t4 += t3 >> 51; r[3] = t3 & 0x7ffffffffffff;
    r[0] += (sword64)((t4 >> 51) * k19);
    r[4] = t4 & 0x7ffffffffffff;
}

/* Load 3 little endian bytes into a 64-bit word.
 *
 * in  An array of bytes.
 * returns a 64-bit word.
 */
word64 load_3(const unsigned char *in)
{
    word64 result;

    result = ((((word64)in[0])      ) |
              (((word64)in[1]) <<  8) |
              (((word64)in[2]) << 16));

    return result;
}

/* Load 4 little endian bytes into a 64-bit word.
 *
 * in  An array of bytes.
 * returns a 64-bit word.
 */
word64 load_4(const unsigned char *in)
{
    word64 result;

    result = ((((word64)in[0])      ) |
              (((word64)in[1]) <<  8) |
              (((word64)in[2]) << 16) |
              (((word64)in[3]) << 24));

    return result;
}

