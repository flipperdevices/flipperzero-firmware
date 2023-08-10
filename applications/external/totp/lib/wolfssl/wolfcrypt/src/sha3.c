/* sha3.c
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

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_XILINX_CRYPT) && \
   !defined(WOLFSSL_AFALG_XILINX_SHA3)

#if defined(HAVE_FIPS) && defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)
    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$l")
        #pragma const_seg(".fipsB$l")
    #endif
#endif

#include <wolfssl/wolfcrypt/sha3.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/hash.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#if !defined(WOLFSSL_ARMASM) || !defined(WOLFSSL_ARMASM_CRYPTO_SHA3)

#ifdef USE_INTEL_SPEEDUP
    #include <wolfssl/wolfcrypt/cpuid.h>

    word32 cpuid_flags;
    int cpuid_flags_set = 0;
    void (*sha3_block)(word64 *s) = NULL;
    void (*sha3_block_n)(word64 *s, const byte* data, word32 n,
        word64 c) = NULL;
#endif

#ifdef WOLFSSL_SHA3_SMALL
/* Rotate a 64-bit value left.
 *
 * a  Number to rotate left.
 * r  Number od bits to rotate left.
 * returns the rotated number.
 */
#define ROTL64(a, n)    (((a)<<(n))|((a)>>(64-(n))))

/* An array of values to XOR for block operation. */
static const word64 hash_keccak_r[24] =
{
    0x0000000000000001UL, 0x0000000000008082UL,
    0x800000000000808aUL, 0x8000000080008000UL,
    0x000000000000808bUL, 0x0000000080000001UL,
    0x8000000080008081UL, 0x8000000000008009UL,
    0x000000000000008aUL, 0x0000000000000088UL,
    0x0000000080008009UL, 0x000000008000000aUL,
    0x000000008000808bUL, 0x800000000000008bUL,
    0x8000000000008089UL, 0x8000000000008003UL,
    0x8000000000008002UL, 0x8000000000000080UL,
    0x000000000000800aUL, 0x800000008000000aUL,
    0x8000000080008081UL, 0x8000000000008080UL,
    0x0000000080000001UL, 0x8000000080008008UL
};

/* Indices used in swap and rotate operation. */
#define K_I_0   10
#define K_I_1    7
#define K_I_2   11
#define K_I_3   17
#define K_I_4   18
#define K_I_5    3
#define K_I_6    5
#define K_I_7   16
#define K_I_8    8
#define K_I_9   21
#define K_I_10  24
#define K_I_11   4
#define K_I_12  15
#define K_I_13  23
#define K_I_14  19
#define K_I_15  13
#define K_I_16  12
#define K_I_17   2
#define K_I_18  20
#define K_I_19  14
#define K_I_20  22
#define K_I_21   9
#define K_I_22   6
#define K_I_23   1

/* Number of bits to rotate in swap and rotate operation. */
#define K_R_0    1
#define K_R_1    3
#define K_R_2    6
#define K_R_3   10
#define K_R_4   15
#define K_R_5   21
#define K_R_6   28
#define K_R_7   36
#define K_R_8   45
#define K_R_9   55
#define K_R_10   2
#define K_R_11  14
#define K_R_12  27
#define K_R_13  41
#define K_R_14  56
#define K_R_15   8
#define K_R_16  25
#define K_R_17  43
#define K_R_18  62
#define K_R_19  18
#define K_R_20  39
#define K_R_21  61
#define K_R_22  20
#define K_R_23  44

/* Swap and rotate left operation.
 *
 * s   The state.
 * t1  Temporary value.
 * t2  Second temporary value.
 * i   The index of the loop.
 */
#define SWAP_ROTL(s, t1, t2, i)                                         \
do {                                                                    \
    t2 = s[K_I_##i]; s[K_I_##i] = ROTL64(t1, K_R_##i);                  \
}                                                                       \
while (0)

/* Mix the XOR of the column's values into each number by column.
 *
 * s  The state.
 * b  Temporary array of XORed column values.
 * x  The index of the column.
 * t  Temporary variable.
 */
#define COL_MIX(s, b, x, t)                                             \
do {                                                                    \
    for (x = 0; x < 5; x++)                                             \
        b[x] = s[x + 0] ^ s[x + 5] ^ s[x + 10] ^ s[x + 15] ^ s[x + 20]; \
    for (x = 0; x < 5; x++) {                                           \
        t = b[(x + 4) % 5] ^ ROTL64(b[(x + 1) % 5], 1);                 \
        s[x +  0] ^= t;                                                 \
        s[x +  5] ^= t;                                                 \
        s[x + 10] ^= t;                                                 \
        s[x + 15] ^= t;                                                 \
        s[x + 20] ^= t;                                                 \
    }                                                                   \
}                                                                       \
while (0)

#ifdef SHA3_BY_SPEC
/* Mix the row values.
 * BMI1 has ANDN instruction ((~a) & b) - Haswell and above.
 *
 * s   The state.
 * b   Temporary array of XORed row values.
 * y   The index of the row to work on.
 * x   The index of the column.
 * t0  Temporary variable.
 * t1  Temporary variable.
 */
#define ROW_MIX(s, b, y, x, t0, t1)                                     \
do {                                                                    \
    for (y = 0; y < 5; y++) {                                           \
        for (x = 0; x < 5; x++)                                         \
            b[x] = s[y * 5 + x];                                        \
        for (x = 0; x < 5; x++)                                         \
            s[y * 5 + x] = b[x] ^ (~b[(x + 1) % 5] & b[(x + 2) % 5]);   \
    }                                                                   \
}                                                                       \
while (0)
#else
/* Mix the row values.
 * a ^ (~b & c) == a ^ (c & (b ^ c)) == (a ^ b) ^ (b | c)
 *
 * s   The state.
 * b   Temporary array of XORed row values.
 * y   The index of the row to work on.
 * x   The index of the column.
 * t0  Temporary variable.
 * t1  Temporary variable.
 */
#define ROW_MIX(s, b, y, x, t12, t34)                                   \
do {                                                                    \
    for (y = 0; y < 5; y++) {                                           \
        for (x = 0; x < 5; x++)                                         \
            b[x] = s[y * 5 + x];                                        \
        t12 = (b[1] ^ b[2]); t34 = (b[3] ^ b[4]);                       \
        s[y * 5 + 0] = b[0] ^ (b[2] &  t12);                            \
        s[y * 5 + 1] =  t12 ^ (b[2] | b[3]);                            \
        s[y * 5 + 2] = b[2] ^ (b[4] &  t34);                            \
        s[y * 5 + 3] =  t34 ^ (b[4] | b[0]);                            \
        s[y * 5 + 4] = b[4] ^ (b[1] & (b[0] ^ b[1]));                   \
    }                                                                   \
}                                                                       \
while (0)
#endif /* SHA3_BY_SPEC */

/* The block operation performed on the state.
 *
 * s  The state.
 */
#ifndef USE_INTEL_SPEEDUP
static
#endif
void BlockSha3(word64 *s)
{
    byte i, x, y;
    word64 t0, t1;
    word64 b[5];

    for (i = 0; i < 24; i++)
    {
        COL_MIX(s, b, x, t0);

        t0 = s[1];
        SWAP_ROTL(s, t0, t1,  0);
        SWAP_ROTL(s, t1, t0,  1);
        SWAP_ROTL(s, t0, t1,  2);
        SWAP_ROTL(s, t1, t0,  3);
        SWAP_ROTL(s, t0, t1,  4);
        SWAP_ROTL(s, t1, t0,  5);
        SWAP_ROTL(s, t0, t1,  6);
        SWAP_ROTL(s, t1, t0,  7);
        SWAP_ROTL(s, t0, t1,  8);
        SWAP_ROTL(s, t1, t0,  9);
        SWAP_ROTL(s, t0, t1, 10);
        SWAP_ROTL(s, t1, t0, 11);
        SWAP_ROTL(s, t0, t1, 12);
        SWAP_ROTL(s, t1, t0, 13);
        SWAP_ROTL(s, t0, t1, 14);
        SWAP_ROTL(s, t1, t0, 15);
        SWAP_ROTL(s, t0, t1, 16);
        SWAP_ROTL(s, t1, t0, 17);
        SWAP_ROTL(s, t0, t1, 18);
        SWAP_ROTL(s, t1, t0, 19);
        SWAP_ROTL(s, t0, t1, 20);
        SWAP_ROTL(s, t1, t0, 21);
        SWAP_ROTL(s, t0, t1, 22);
        SWAP_ROTL(s, t1, t0, 23);

        ROW_MIX(s, b, y, x, t0, t1);

        s[0] ^= hash_keccak_r[i];
    }
}
#else
/* Rotate a 64-bit value left.
 *
 * a  Number to rotate left.
 * r  Number od bits to rotate left.
 * returns the rotated number.
 */
#define ROTL64(a, n)    (((a)<<(n))|((a)>>(64-(n))))


/* An array of values to XOR for block operation. */
static const word64 hash_keccak_r[24] =
{
    W64LIT(0x0000000000000001), W64LIT(0x0000000000008082),
    W64LIT(0x800000000000808a), W64LIT(0x8000000080008000),
    W64LIT(0x000000000000808b), W64LIT(0x0000000080000001),
    W64LIT(0x8000000080008081), W64LIT(0x8000000000008009),
    W64LIT(0x000000000000008a), W64LIT(0x0000000000000088),
    W64LIT(0x0000000080008009), W64LIT(0x000000008000000a),
    W64LIT(0x000000008000808b), W64LIT(0x800000000000008b),
    W64LIT(0x8000000000008089), W64LIT(0x8000000000008003),
    W64LIT(0x8000000000008002), W64LIT(0x8000000000000080),
    W64LIT(0x000000000000800a), W64LIT(0x800000008000000a),
    W64LIT(0x8000000080008081), W64LIT(0x8000000000008080),
    W64LIT(0x0000000080000001), W64LIT(0x8000000080008008)
};

/* Indices used in swap and rotate operation. */
#define KI_0     6
#define KI_1    12
#define KI_2    18
#define KI_3    24
#define KI_4     3
#define KI_5     9
#define KI_6    10
#define KI_7    16
#define KI_8    22
#define KI_9     1
#define KI_10    7
#define KI_11   13
#define KI_12   19
#define KI_13   20
#define KI_14    4
#define KI_15    5
#define KI_16   11
#define KI_17   17
#define KI_18   23
#define KI_19    2
#define KI_20    8
#define KI_21   14
#define KI_22   15
#define KI_23   21

/* Number of bits to rotate in swap and rotate operation. */
#define KR_0    44
#define KR_1    43
#define KR_2    21
#define KR_3    14
#define KR_4    28
#define KR_5    20
#define KR_6     3
#define KR_7    45
#define KR_8    61
#define KR_9     1
#define KR_10    6
#define KR_11   25
#define KR_12    8
#define KR_13   18
#define KR_14   27
#define KR_15   36
#define KR_16   10
#define KR_17   15
#define KR_18   56
#define KR_19   62
#define KR_20   55
#define KR_21   39
#define KR_22   41
#define KR_23    2

/* Mix the XOR of the column's values into each number by column.
 *
 * s  The state.
 * b  Temporary array of XORed column values.
 * x  The index of the column.
 * t  Temporary variable.
 */
#define COL_MIX(s, b, x, t)                                                         \
do {                                                                                \
    (b)[0] = (s)[0] ^ (s)[5] ^ (s)[10] ^ (s)[15] ^ (s)[20];                         \
    (b)[1] = (s)[1] ^ (s)[6] ^ (s)[11] ^ (s)[16] ^ (s)[21];                         \
    (b)[2] = (s)[2] ^ (s)[7] ^ (s)[12] ^ (s)[17] ^ (s)[22];                         \
    (b)[3] = (s)[3] ^ (s)[8] ^ (s)[13] ^ (s)[18] ^ (s)[23];                         \
    (b)[4] = (s)[4] ^ (s)[9] ^ (s)[14] ^ (s)[19] ^ (s)[24];                         \
    (t) = (b)[(0 + 4) % 5] ^ ROTL64((b)[(0 + 1) % 5], 1);                           \
    (s)[ 0] ^= (t); (s)[ 5] ^= (t); (s)[10] ^= (t); (s)[15] ^= (t); (s)[20] ^= (t); \
    (t) = (b)[(1 + 4) % 5] ^ ROTL64((b)[(1 + 1) % 5], 1);                           \
    (s)[ 1] ^= (t); (s)[ 6] ^= (t); (s)[11] ^= (t); (s)[16] ^= (t); (s)[21] ^= (t); \
    (t) = (b)[(2 + 4) % 5] ^ ROTL64((b)[(2 + 1) % 5], 1);                           \
    (s)[ 2] ^= (t); (s)[ 7] ^= (t); (s)[12] ^= (t); (s)[17] ^= (t); (s)[22] ^= (t); \
    (t) = (b)[(3 + 4) % 5] ^ ROTL64((b)[(3 + 1) % 5], 1);                           \
    (s)[ 3] ^= (t); (s)[ 8] ^= (t); (s)[13] ^= (t); (s)[18] ^= (t); (s)[23] ^= (t); \
    (t) = (b)[(4 + 4) % 5] ^ ROTL64((b)[(4 + 1) % 5], 1);                           \
    (s)[ 4] ^= (t); (s)[ 9] ^= (t); (s)[14] ^= (t); (s)[19] ^= (t); (s)[24] ^= (t); \
}                                                                                   \
while (0)

#define S(s1, i) ROTL64((s1)[KI_##i], KR_##i)

#ifdef SHA3_BY_SPEC
/* Mix the row values.
 * BMI1 has ANDN instruction ((~a) & b) - Haswell and above.
 *
 * s2  The new state.
 * s1  The current state.
 * b   Temporary array of XORed row values.
 * t0  Temporary variable. (Unused)
 * t1  Temporary variable. (Unused)
 */
#define ROW_MIX(s2, s1, b, t0, t1)                    \
do {                                                  \
    (b)[0] = (s1)[0];                                 \
    (b)[1] = S((s1), 0);                              \
    (b)[2] = S((s1), 1);                              \
    (b)[3] = S((s1), 2);                              \
    (b)[4] = S((s1), 3);                              \
    (s2)[0] = (b)[0] ^ (~(b)[1] & (b)[2]);            \
    (s2)[1] = (b)[1] ^ (~(b)[2] & (b)[3]);            \
    (s2)[2] = (b)[2] ^ (~(b)[3] & (b)[4]);            \
    (s2)[3] = (b)[3] ^ (~(b)[4] & (b)[0]);            \
    (s2)[4] = (b)[4] ^ (~(b)[0] & (b)[1]);            \
    (b)[0] = S((s1), 4);                              \
    (b)[1] = S((s1), 5);                              \
    (b)[2] = S((s1), 6);                              \
    (b)[3] = S((s1), 7);                              \
    (b)[4] = S((s1), 8);                              \
    (s2)[5] = (b)[0] ^ (~(b)[1] & (b)[2]);            \
    (s2)[6] = (b)[1] ^ (~(b)[2] & (b)[3]);            \
    (s2)[7] = (b)[2] ^ (~(b)[3] & (b)[4]);            \
    (s2)[8] = (b)[3] ^ (~(b)[4] & (b)[0]);            \
    (s2)[9] = (b)[4] ^ (~(b)[0] & (b)[1]);            \
    (b)[0] = S((s1), 9);                              \
    (b)[1] = S((s1), 10);                             \
    (b)[2] = S((s1), 11);                             \
    (b)[3] = S((s1), 12);                             \
    (b)[4] = S((s1), 13);                             \
    (s2)[10] = (b)[0] ^ (~(b)[1] & (b)[2]);           \
    (s2)[11] = (b)[1] ^ (~(b)[2] & (b)[3]);           \
    (s2)[12] = (b)[2] ^ (~(b)[3] & (b)[4]);           \
    (s2)[13] = (b)[3] ^ (~(b)[4] & (b)[0]);           \
    (s2)[14] = (b)[4] ^ (~(b)[0] & (b)[1]);           \
    (b)[0] = S((s1), 14);                             \
    (b)[1] = S((s1), 15);                             \
    (b)[2] = S((s1), 16);                             \
    (b)[3] = S((s1), 17);                             \
    (b)[4] = S((s1), 18);                             \
    (s2)[15] = (b)[0] ^ (~(b)[1] & (b)[2]);           \
    (s2)[16] = (b)[1] ^ (~(b)[2] & (b)[3]);           \
    (s2)[17] = (b)[2] ^ (~(b)[3] & (b)[4]);           \
    (s2)[18] = (b)[3] ^ (~(b)[4] & (b)[0]);           \
    (s2)[19] = (b)[4] ^ (~(b)[0] & (b)[1]);           \
    (b)[0] = S((s1), 19);                             \
    (b)[1] = S((s1), 20);                             \
    (b)[2] = S((s1), 21);                             \
    (b)[3] = S((s1), 22);                             \
    (b)[4] = S((s1), 23);                             \
    (s2)[20] = (b)[0] ^ (~(b)[1] & (b)[2]);           \
    (s2)[21] = (b)[1] ^ (~(b)[2] & (b)[3]);           \
    (s2)[22] = (b)[2] ^ (~(b)[3] & (b)[4]);           \
    (s2)[23] = (b)[3] ^ (~(b)[4] & (b)[0]);           \
    (s2)[24] = (b)[4] ^ (~(b)[0] & (b)[1]);           \
}                                                     \
while (0)
#else
/* Mix the row values.
 * a ^ (~b & c) == a ^ (c & (b ^ c)) == (a ^ b) ^ (b | c)
 *
 * s2  The new state.
 * s1  The current state.
 * b   Temporary array of XORed row values.
 * t12 Temporary variable.
 * t34 Temporary variable.
 */
#define ROW_MIX(s2, s1, b, t12, t34)                      \
do {                                                      \
    (b)[0] = (s1)[0];                                     \
    (b)[1] = S((s1), 0);                                  \
    (b)[2] = S((s1), 1);                                  \
    (b)[3] = S((s1), 2);                                  \
    (b)[4] = S((s1), 3);                                  \
    (t12) = ((b)[1] ^ (b)[2]); (t34) = ((b)[3] ^ (b)[4]); \
    (s2)[0] = (b)[0] ^ ((b)[2] &  (t12));                 \
    (s2)[1] =  (t12) ^ ((b)[2] | (b)[3]);                 \
    (s2)[2] = (b)[2] ^ ((b)[4] &  (t34));                 \
    (s2)[3] =  (t34) ^ ((b)[4] | (b)[0]);                 \
    (s2)[4] = (b)[4] ^ ((b)[1] & ((b)[0] ^ (b)[1]));      \
    (b)[0] = S((s1), 4);                                  \
    (b)[1] = S((s1), 5);                                  \
    (b)[2] = S((s1), 6);                                  \
    (b)[3] = S((s1), 7);                                  \
    (b)[4] = S((s1), 8);                                  \
    (t12) = ((b)[1] ^ (b)[2]); (t34) = ((b)[3] ^ (b)[4]); \
    (s2)[5] = (b)[0] ^ ((b)[2] &  (t12));                 \
    (s2)[6] =  (t12) ^ ((b)[2] | (b)[3]);                 \
    (s2)[7] = (b)[2] ^ ((b)[4] &  (t34));                 \
    (s2)[8] =  (t34) ^ ((b)[4] | (b)[0]);                 \
    (s2)[9] = (b)[4] ^ ((b)[1] & ((b)[0] ^ (b)[1]));      \
    (b)[0] = S((s1), 9);                                  \
    (b)[1] = S((s1), 10);                                 \
    (b)[2] = S((s1), 11);                                 \
    (b)[3] = S((s1), 12);                                 \
    (b)[4] = S((s1), 13);                                 \
    (t12) = ((b)[1] ^ (b)[2]); (t34) = ((b)[3] ^ (b)[4]); \
    (s2)[10] = (b)[0] ^ ((b)[2] &  (t12));                \
    (s2)[11] =  (t12) ^ ((b)[2] | (b)[3]);                \
    (s2)[12] = (b)[2] ^ ((b)[4] &  (t34));                \
    (s2)[13] =  (t34) ^ ((b)[4] | (b)[0]);                \
    (s2)[14] = (b)[4] ^ ((b)[1] & ((b)[0] ^ (b)[1]));     \
    (b)[0] = S((s1), 14);                                 \
    (b)[1] = S((s1), 15);                                 \
    (b)[2] = S((s1), 16);                                 \
    (b)[3] = S((s1), 17);                                 \
    (b)[4] = S((s1), 18);                                 \
    (t12) = ((b)[1] ^ (b)[2]); (t34) = ((b)[3] ^ (b)[4]); \
    (s2)[15] = (b)[0] ^ ((b)[2] &  (t12));                \
    (s2)[16] =  (t12) ^ ((b)[2] | (b)[3]);                \
    (s2)[17] = (b)[2] ^ ((b)[4] &  (t34));                \
    (s2)[18] =  (t34) ^ ((b)[4] | (b)[0]);                \
    (s2)[19] = (b)[4] ^ ((b)[1] & ((b)[0] ^ (b)[1]));     \
    (b)[0] = S((s1), 19);                                 \
    (b)[1] = S((s1), 20);                                 \
    (b)[2] = S((s1), 21);                                 \
    (b)[3] = S((s1), 22);                                 \
    (b)[4] = S((s1), 23);                                 \
    (t12) = ((b)[1] ^ (b)[2]); (t34) = ((b)[3] ^ (b)[4]); \
    (s2)[20] = (b)[0] ^ ((b)[2] &  (t12));                \
    (s2)[21] =  (t12) ^ ((b)[2] | (b)[3]);                \
    (s2)[22] = (b)[2] ^ ((b)[4] &  (t34));                \
    (s2)[23] =  (t34) ^ ((b)[4] | (b)[0]);                \
    (s2)[24] = (b)[4] ^ ((b)[1] & ((b)[0] ^ (b)[1]));     \
}                                                         \
while (0)
#endif /* SHA3_BY_SPEC */

/* The block operation performed on the state.
 *
 * s  The state.
 */
#ifndef USE_INTEL_SPEEDUP
static
#endif
void BlockSha3(word64 *s)
{
    word64 n[25];
    word64 b[5];
    word64 t0;
#ifndef SHA3_BY_SPEC
    word64 t1;
#endif
    byte i;

    for (i = 0; i < 24; i += 2)
    {
        COL_MIX(s, b, x, t0);
        ROW_MIX(n, s, b, t0, t1);
        n[0] ^= hash_keccak_r[i];

        COL_MIX(n, b, x, t0);
        ROW_MIX(s, n, b, t0, t1);
        s[0] ^= hash_keccak_r[i+1];
    }
}
#endif /* WOLFSSL_SHA3_SMALL */
#endif /* !WOLFSSL_ARMASM */

static WC_INLINE word64 Load64Unaligned(const unsigned char *a)
{
    return ((word64)a[0] <<  0) |
           ((word64)a[1] <<  8) |
           ((word64)a[2] << 16) |
           ((word64)a[3] << 24) |
           ((word64)a[4] << 32) |
           ((word64)a[5] << 40) |
           ((word64)a[6] << 48) |
           ((word64)a[7] << 56);
}

/* Convert the array of bytes, in little-endian order, to a 64-bit integer.
 *
 * a  Array of bytes.
 * returns a 64-bit integer.
 */
static word64 Load64BitBigEndian(const byte* a)
{
#if defined(BIG_ENDIAN_ORDER) || (WOLFSSL_GENERAL_ALIGNMENT == 1)
    word64 n = 0;
    int i;

    for (i = 0; i < 8; i++)
        n |= (word64)a[i] << (8 * i);

    return n;
#elif ((WOLFSSL_GENERAL_ALIGNMENT > 0) && (WOLFSSL_GENERAL_ALIGNMENT == 4))
    word64 n;

    n  =          *(word32*) a;
    n |= ((word64)*(word32*)(a + 4)) << 32;

    return n;
#elif ((WOLFSSL_GENERAL_ALIGNMENT > 0) && (WOLFSSL_GENERAL_ALIGNMENT == 2))
    word64 n;

    n  =          *(word16*) a;
    n |= ((word64)*(word16*)(a + 2)) << 16;
    n |= ((word64)*(word16*)(a + 4)) << 32;
    n |= ((word64)*(word16*)(a + 6)) << 48;

    return n;
#else
    return *(const word64*)a;
#endif
}

/* Initialize the state for a SHA3-224 hash operation.
 *
 * sha3   wc_Sha3 object holding state.
 * returns 0 on success.
 */
static int InitSha3(wc_Sha3* sha3)
{
    int i;

    for (i = 0; i < 25; i++)
        sha3->s[i] = 0;
    sha3->i = 0;
#ifdef WOLFSSL_HASH_FLAGS
    sha3->flags = 0;
#endif

#ifdef USE_INTEL_SPEEDUP
    if (!cpuid_flags_set) {
        cpuid_flags = cpuid_get_flags();
        cpuid_flags_set = 1;
        if (IS_INTEL_BMI1(cpuid_flags) && IS_INTEL_BMI2(cpuid_flags)) {
            sha3_block = sha3_block_bmi2;
            sha3_block_n = sha3_block_n_bmi2;
        }
        else if (IS_INTEL_AVX2(cpuid_flags)) {
            sha3_block = sha3_block_avx2;
        }
        else {
            sha3_block = BlockSha3;
        }
    }
#endif

    return 0;
}

/* Update the SHA-3 hash state with message data.
 *
 * sha3  wc_Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * p     Number of 64-bit numbers in a block of data to process.
 * returns 0 on success.
 */
static int Sha3Update(wc_Sha3* sha3, const byte* data, word32 len, byte p)
{
    word32 i;
    word32 blocks;

    if (sha3->i > 0) {
        byte *t;
        byte l = (byte)(p * 8 - sha3->i);
        if (l > len) {
            l = (byte)len;
        }

        t = &sha3->t[sha3->i];
        for (i = 0; i < l; i++) {
            t[i] = data[i];
        }
        data += i;
        len -= i;
        sha3->i += (byte) i;

        if (sha3->i == p * 8) {
            for (i = 0; i < p; i++) {
                sha3->s[i] ^= Load64BitBigEndian(sha3->t + 8 * i);
            }
        #ifdef USE_INTEL_SPEEDUP
            (*sha3_block)(sha3->s);
        #else
            BlockSha3(sha3->s);
        #endif
            sha3->i = 0;
        }
    }
    blocks = len / (p * 8);
    #ifdef USE_INTEL_SPEEDUP
    if ((sha3_block_n != NULL) && (blocks > 0)) {
        (*sha3_block_n)(sha3->s, data, blocks, p * 8);
        len -= blocks * (p * 8);
        data += blocks * (p * 8);
        blocks = 0;
    }
    #endif
    for (; blocks > 0; blocks--) {
        for (i = 0; i < p; i++) {
            sha3->s[i] ^= Load64Unaligned(data + 8 * i);
        }
    #ifdef USE_INTEL_SPEEDUP
        (*sha3_block)(sha3->s);
    #else
        BlockSha3(sha3->s);
    #endif
        len -= p * 8;
        data += p * 8;
    }
    XMEMCPY(sha3->t, data, len);
    sha3->i += (byte)len;

    return 0;
}

/* Calculate the SHA-3 hash based on all the message data seen.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result.
 * p     Number of 64-bit numbers in a block of data to process.
 * len   Number of bytes in output.
 * returns 0 on success.
 */
static int Sha3Final(wc_Sha3* sha3, byte padChar, byte* hash, byte p, word32 l)
{
    word32 rate = p * 8;
    word32 j;
    word32 i;

    sha3->t[rate - 1]  = 0x00;
#ifdef WOLFSSL_HASH_FLAGS
    if ((p == WC_SHA3_256_COUNT) && (sha3->flags & WC_HASH_SHA3_KECCAK256))
        padChar = 0x01;
#endif
    sha3->t[sha3->i ]  = padChar;
    sha3->t[rate - 1] |= 0x80;
    if (rate - 1 > (word32)sha3->i + 1) {
        XMEMSET(sha3->t + sha3->i + 1, 0, rate - 1 - (sha3->i + 1));
    }
    for (i = 0; i < p; i++) {
        sha3->s[i] ^= Load64BitBigEndian(sha3->t + 8 * i);
    }
    for (j = 0; l - j >= rate; j += rate) {
    #ifdef USE_INTEL_SPEEDUP
        (*sha3_block)(sha3->s);
    #else
        BlockSha3(sha3->s);
    #endif
    #if defined(BIG_ENDIAN_ORDER)
        ByteReverseWords64((word64*)(hash + j), sha3->s, rate);
    #else
        XMEMCPY(hash + j, sha3->s, rate);
    #endif
    }
    if (j != l) {
    #ifdef USE_INTEL_SPEEDUP
        (*sha3_block)(sha3->s);
    #else
        BlockSha3(sha3->s);
    #endif
    #if defined(BIG_ENDIAN_ORDER)
        ByteReverseWords64(sha3->s, sha3->s, rate);
    #endif
        XMEMCPY(hash + j, sha3->s, l - j);
    }
    return 0;
}

/* Initialize the state for a SHA-3 hash operation.
 *
 * sha3   wc_Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
static int wc_InitSha3(wc_Sha3* sha3, void* heap, int devId)
{
    int ret = 0;

    if (sha3 == NULL)
        return BAD_FUNC_ARG;

    sha3->heap = heap;
    ret = InitSha3(sha3);
    if (ret != 0)
        return ret;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    ret = wolfAsync_DevCtxInit(&sha3->asyncDev,
                        WOLFSSL_ASYNC_MARKER_SHA3, sha3->heap, devId);
#else
    (void)devId;
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}

/* Update the SHA-3 hash state with message data.
 *
 * sha3  wc_Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * p     Number of 64-bit numbers in a block of data to process.
 * returns 0 on success.
 */
static int wc_Sha3Update(wc_Sha3* sha3, const byte* data, word32 len, byte p)
{
    int ret;

    if (sha3 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    if (data == NULL && len == 0) {
        /* valid, but do nothing */
        return 0;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    if (sha3->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA3) {
    #if defined(HAVE_INTEL_QA) && defined(QAT_V2)
        /* QAT only supports SHA3_256 */
        if (p == WC_SHA3_256_COUNT) {
            ret = IntelQaSymSha3(&sha3->asyncDev, NULL, data, len);
            if (ret != NOT_COMPILED_IN)
                return ret;
            /* fall-through when unavailable */
        }
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = Sha3Update(sha3, data, len, p);

    return ret;
}

/* Calculate the SHA-3 hash based on all the message data seen.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result.
 * p     Number of 64-bit numbers in a block of data to process.
 * len   Number of bytes in output.
 * returns 0 on success.
 */
static int wc_Sha3Final(wc_Sha3* sha3, byte* hash, byte p, byte len)
{
    int ret;

    if (sha3 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    if (sha3->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA3) {
    #if defined(HAVE_INTEL_QA) && defined(QAT_V2)
        /* QAT only supports SHA3_256 */
        /* QAT SHA-3 only supported on v2 (8970 or later cards) */
        if (len == WC_SHA3_256_DIGEST_SIZE) {
            ret = IntelQaSymSha3(&sha3->asyncDev, hash, NULL, len);
            if (ret != NOT_COMPILED_IN)
                return ret;
            /* fall-through when unavailable */
        }
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = Sha3Final(sha3, 0x06, hash, p, (word32)len);
    if (ret != 0)
        return ret;

    return InitSha3(sha3);  /* reset state */
}

/* Dispose of any dynamically allocated data from the SHA3-384 operation.
 * (Required for async ops.)
 *
 * sha3  wc_Sha3 object holding state.
 * returns 0 on success.
 */
static void wc_Sha3Free(wc_Sha3* sha3)
{
    (void)sha3;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    if (sha3 == NULL)
        return;

    wolfAsync_DevCtxFree(&sha3->asyncDev, WOLFSSL_ASYNC_MARKER_SHA3);
#endif /* WOLFSSL_ASYNC_CRYPT */
}


/* Copy the state of the SHA3 operation.
 *
 * src  wc_Sha3 object holding state top copy.
 * dst  wc_Sha3 object to copy into.
 * returns 0 on success.
 */
static int wc_Sha3Copy(wc_Sha3* src, wc_Sha3* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(wc_Sha3));

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
#endif
#ifdef WOLFSSL_HASH_FLAGS
     dst->flags |= WC_HASH_FLAG_ISCOPY;
#endif

    return ret;
}

/* Calculate the SHA3-224 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 28 bytes.
 * p     Number of 64-bit numbers in a block of data to process.
 * len   Number of bytes in output.
 * returns 0 on success.
 */
static int wc_Sha3GetHash(wc_Sha3* sha3, byte* hash, byte p, byte len)
{
    int ret;
    wc_Sha3 tmpSha3;

    if (sha3 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha3Copy(sha3, &tmpSha3);
    if (ret == 0) {
        ret = wc_Sha3Final(&tmpSha3, hash, p, len);
    }
    return ret;
}


/* Initialize the state for a SHA3-224 hash operation.
 *
 * sha3   wc_Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
int wc_InitSha3_224(wc_Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-224 hash state with message data.
 *
 * sha3  wc_Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
int wc_Sha3_224_Update(wc_Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, WC_SHA3_224_COUNT);
}

/* Calculate the SHA3-224 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 28 bytes.
 * returns 0 on success.
 */
int wc_Sha3_224_Final(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, WC_SHA3_224_COUNT, WC_SHA3_224_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-224 operation.
 * (Required for async ops.)
 *
 * sha3  wc_Sha3 object holding state.
 * returns 0 on success.
 */
void wc_Sha3_224_Free(wc_Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-224 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 28 bytes.
 * returns 0 on success.
 */
int wc_Sha3_224_GetHash(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, WC_SHA3_224_COUNT, WC_SHA3_224_DIGEST_SIZE);
}

/* Copy the state of the SHA3-224 operation.
 *
 * src  wc_Sha3 object holding state top copy.
 * dst  wc_Sha3 object to copy into.
 * returns 0 on success.
 */
int wc_Sha3_224_Copy(wc_Sha3* src, wc_Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}


/* Initialize the state for a SHA3-256 hash operation.
 *
 * sha3   wc_Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
int wc_InitSha3_256(wc_Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-256 hash state with message data.
 *
 * sha3  wc_Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
int wc_Sha3_256_Update(wc_Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, WC_SHA3_256_COUNT);
}

/* Calculate the SHA3-256 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 32 bytes.
 * returns 0 on success.
 */
int wc_Sha3_256_Final(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, WC_SHA3_256_COUNT, WC_SHA3_256_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-256 operation.
 * (Required for async ops.)
 *
 * sha3  wc_Sha3 object holding state.
 * returns 0 on success.
 */
void wc_Sha3_256_Free(wc_Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-256 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 32 bytes.
 * returns 0 on success.
 */
int wc_Sha3_256_GetHash(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, WC_SHA3_256_COUNT, WC_SHA3_256_DIGEST_SIZE);
}

/* Copy the state of the SHA3-256 operation.
 *
 * src  wc_Sha3 object holding state top copy.
 * dst  wc_Sha3 object to copy into.
 * returns 0 on success.
 */
int wc_Sha3_256_Copy(wc_Sha3* src, wc_Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}


/* Initialize the state for a SHA3-384 hash operation.
 *
 * sha3   wc_Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
int wc_InitSha3_384(wc_Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-384 hash state with message data.
 *
 * sha3  wc_Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
int wc_Sha3_384_Update(wc_Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, WC_SHA3_384_COUNT);
}

/* Calculate the SHA3-384 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 48 bytes.
 * returns 0 on success.
 */
int wc_Sha3_384_Final(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, WC_SHA3_384_COUNT, WC_SHA3_384_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-384 operation.
 * (Required for async ops.)
 *
 * sha3  wc_Sha3 object holding state.
 * returns 0 on success.
 */
void wc_Sha3_384_Free(wc_Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-384 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 48 bytes.
 * returns 0 on success.
 */
int wc_Sha3_384_GetHash(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, WC_SHA3_384_COUNT, WC_SHA3_384_DIGEST_SIZE);
}

/* Copy the state of the SHA3-384 operation.
 *
 * src  wc_Sha3 object holding state top copy.
 * dst  wc_Sha3 object to copy into.
 * returns 0 on success.
 */
int wc_Sha3_384_Copy(wc_Sha3* src, wc_Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}


/* Initialize the state for a SHA3-512 hash operation.
 *
 * sha3   wc_Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
int wc_InitSha3_512(wc_Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-512 hash state with message data.
 *
 * sha3  wc_Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
int wc_Sha3_512_Update(wc_Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, WC_SHA3_512_COUNT);
}

/* Calculate the SHA3-512 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 64 bytes.
 * returns 0 on success.
 */
int wc_Sha3_512_Final(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, WC_SHA3_512_COUNT, WC_SHA3_512_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-512 operation.
 * (Required for async ops.)
 *
 * sha3  wc_Sha3 object holding state.
 * returns 0 on success.
 */
void wc_Sha3_512_Free(wc_Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-512 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  wc_Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 64 bytes.
 * returns 0 on success.
 */
int wc_Sha3_512_GetHash(wc_Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, WC_SHA3_512_COUNT, WC_SHA3_512_DIGEST_SIZE);
}

/* Copy the state of the SHA3-512 operation.
 *
 * src  wc_Sha3 object holding state top copy.
 * dst  wc_Sha3 object to copy into.
 * returns 0 on success.
 */
int wc_Sha3_512_Copy(wc_Sha3* src, wc_Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}

#ifdef WOLFSSL_HASH_FLAGS
int wc_Sha3_SetFlags(wc_Sha3* sha3, word32 flags)
{
    if (sha3) {
        sha3->flags = flags;
    }
    return 0;
}
int wc_Sha3_GetFlags(wc_Sha3* sha3, word32* flags)
{
    if (sha3 && flags) {
        *flags = sha3->flags;
    }
    return 0;
}
#endif

#ifdef WOLFSSL_SHAKE128
/* Initialize the state for a Shake128 hash operation.
 *
 * shake  wc_Shake object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
int wc_InitShake128(wc_Shake* shake, void* heap, int devId)
{
    return wc_InitSha3(shake, heap, devId);
}

/* Update the SHAKE128 hash state with message data.
 *
 * shake  wc_Shake object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
int wc_Shake128_Update(wc_Shake* shake, const byte* data, word32 len)
{
    if (shake == NULL || (data == NULL && len > 0)) {
         return BAD_FUNC_ARG;
    }

    if (data == NULL && len == 0) {
        /* valid, but do nothing */
        return 0;
    }

    return Sha3Update(shake, data, len, WC_SHA3_128_COUNT);
}

/* Calculate the SHAKE128 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * shake  wc_Shake object holding state.
 * hash  Buffer to hold the hash result. Must be at least 64 bytes.
 * returns 0 on success.
 */
int wc_Shake128_Final(wc_Shake* shake, byte* hash, word32 hashLen)
{
    int ret;

    if (shake == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = Sha3Final(shake, 0x1f, hash, WC_SHA3_128_COUNT, hashLen);
    if (ret != 0)
        return ret;

    return InitSha3(shake);  /* reset state */
}

/* Absorb the data for squeezing.
 *
 * Update and final with data but no output and no reset
 *
 * shake  wc_Shake object holding state.
 * data  Data to absorb.
 * len  Length of d to absorb in bytes.
 * returns 0 on success.
 */
int wc_Shake128_Absorb(wc_Shake* shake, const byte* data, word32 len)
{
    int ret;

    ret = Sha3Update(shake, data, len, WC_SHA3_128_COUNT);
    if (ret == 0) {
        byte hash[1];
        ret = Sha3Final(shake, 0x1f, hash, WC_SHA3_128_COUNT, 0);
    }
    /* No partial data. */
    shake->i = 0;

    return ret;
}

/* Squeeze the state to produce pseudo-random output.
 *
 * shake  wc_Shake object holding state.
 * out  Output buffer.
 * blockCnt  Number of blocks to write.
 * returns 0 on success.
 */
int wc_Shake128_SqueezeBlocks(wc_Shake* shake, byte* out, word32 blockCnt)
{
    for (; (blockCnt > 0); blockCnt--) {
    #ifdef USE_INTEL_SPEEDUP
        (*sha3_block)(shake->s);
    #else
        BlockSha3(shake->s);
    #endif
    #if defined(BIG_ENDIAN_ORDER)
        ByteReverseWords64((word64*)out, shake->s, WC_SHA3_128_COUNT * 8);
    #else
        XMEMCPY(out, shake->s, WC_SHA3_128_COUNT * 8);
    #endif
        out += WC_SHA3_128_COUNT * 8;
    }

    return 0;
}

/* Dispose of any dynamically allocated data from the SHAKE128 operation.
 * (Required for async ops.)
 *
 * shake  wc_Shake object holding state.
 * returns 0 on success.
 */
void wc_Shake128_Free(wc_Shake* shake)
{
    wc_Sha3Free(shake);
}

/* Copy the state of the SHA3-512 operation.
 *
 * src  wc_Shake object holding state top copy.
 * dst  wc_Shake object to copy into.
 * returns 0 on success.
 */
int wc_Shake128_Copy(wc_Shake* src, wc_Shake* dst)
{
    return wc_Sha3Copy(src, dst);
}
#endif

#ifdef WOLFSSL_SHAKE256
/* Initialize the state for a Shake256 hash operation.
 *
 * shake  wc_Shake object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
int wc_InitShake256(wc_Shake* shake, void* heap, int devId)
{
    return wc_InitSha3(shake, heap, devId);
}

/* Update the SHAKE256 hash state with message data.
 *
 * shake  wc_Shake object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
int wc_Shake256_Update(wc_Shake* shake, const byte* data, word32 len)
{
    if (shake == NULL || (data == NULL && len > 0)) {
         return BAD_FUNC_ARG;
    }

    if (data == NULL && len == 0) {
        /* valid, but do nothing */
        return 0;
    }

    return Sha3Update(shake, data, len, WC_SHA3_256_COUNT);
}

/* Calculate the SHAKE256 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * shake  wc_Shake object holding state.
 * hash  Buffer to hold the hash result. Must be at least 64 bytes.
 * returns 0 on success.
 */
int wc_Shake256_Final(wc_Shake* shake, byte* hash, word32 hashLen)
{
    int ret;

    if (shake == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = Sha3Final(shake, 0x1f, hash, WC_SHA3_256_COUNT, hashLen);
    if (ret != 0)
        return ret;

    return InitSha3(shake);  /* reset state */
}

/* Absorb the data for squeezing.
 *
 * Update and final with data but no output and no reset
 *
 * shake  wc_Shake object holding state.
 * data  Data to absorb.
 * len  Length of d to absorb in bytes.
 * returns 0 on success.
 */
int wc_Shake256_Absorb(wc_Shake* shake, const byte* data, word32 len)
{
    int ret;

    ret = Sha3Update(shake, data, len, WC_SHA3_256_COUNT);
    if (ret == 0) {
        byte hash[1];
        ret = Sha3Final(shake, 0x1f, hash, WC_SHA3_256_COUNT, 0);
    }
    /* No partial data. */
    shake->i = 0;

    return ret;
}

/* Squeeze the state to produce pseudo-random output.
 *
 * shake  wc_Shake object holding state.
 * out  Output buffer.
 * blockCnt  Number of blocks to write.
 * returns 0 on success.
 */
int wc_Shake256_SqueezeBlocks(wc_Shake* shake, byte* out, word32 blockCnt)
{
    for (; (blockCnt > 0); blockCnt--) {
    #ifdef USE_INTEL_SPEEDUP
        (*sha3_block)(shake->s);
    #else
        BlockSha3(shake->s);
    #endif
    #if defined(BIG_ENDIAN_ORDER)
        ByteReverseWords64((word64*)out, shake->s, WC_SHA3_256_COUNT * 8);
    #else
        XMEMCPY(out, shake->s, WC_SHA3_256_COUNT * 8);
    #endif
        out += WC_SHA3_256_COUNT * 8;
    }

    return 0;
}

/* Dispose of any dynamically allocated data from the SHAKE256 operation.
 * (Required for async ops.)
 *
 * shake  wc_Shake object holding state.
 * returns 0 on success.
 */
void wc_Shake256_Free(wc_Shake* shake)
{
    wc_Sha3Free(shake);
}

/* Copy the state of the SHA3-512 operation.
 *
 * src  wc_Shake object holding state top copy.
 * dst  wc_Shake object to copy into.
 * returns 0 on success.
 */
int wc_Shake256_Copy(wc_Shake* src, wc_Shake* dst)
{
    return wc_Sha3Copy(src, dst);
}
#endif

#endif /* WOLFSSL_SHA3 */
