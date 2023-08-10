/* ripemd.c
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

#ifdef WOLFSSL_RIPEMD

#include <wolfssl/wolfcrypt/ripemd.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/error-crypt.h>

int wc_InitRipeMd(RipeMd* ripemd)
{
    if (ripemd == NULL) {
        return BAD_FUNC_ARG;
    }

    ripemd->digest[0] = 0x67452301L;
    ripemd->digest[1] = 0xEFCDAB89L;
    ripemd->digest[2] = 0x98BADCFEL;
    ripemd->digest[3] = 0x10325476L;
    ripemd->digest[4] = 0xC3D2E1F0L;

    ripemd->buffLen = 0;
    ripemd->loLen   = 0;
    ripemd->hiLen   = 0;

    return 0;
}


/* for all */
#define F(x, y, z)    ((x) ^ (y) ^ (z))
#define G(x, y, z)    ((z) ^ ((x) & ((y)^(z))))
#define H(x, y, z)    ((z) ^ ((x) | ~(y)))
#define I(x, y, z)    ((y) ^ ((z) & ((x)^(y))))
#define J(x, y, z)    ((x) ^ ((y) | ~(z)))

#define k0 0
#define k1 0x5a827999
#define k2 0x6ed9eba1
#define k3 0x8f1bbcdc
#define k4 0xa953fd4e
#define k5 0x50a28be6
#define k6 0x5c4dd124
#define k7 0x6d703ef3
#define k8 0x7a6d76e9
#define k9 0

/* for 160 and 320 */
#define Subround(f, a, b, c, d, e, x, s, k) \
    a += f(b, c, d) + (x) + (k);            \
    (a) = rotlFixed((word32)(a), s) + (e);  \
    (c) = rotlFixed((word32)(c), 10U)

static void Transform(RipeMd* ripemd)
{
    word32 a1, b1, c1, d1, e1, a2, b2, c2, d2, e2;
    a1 = a2 = ripemd->digest[0];
    b1 = b2 = ripemd->digest[1];
    c1 = c2 = ripemd->digest[2];
    d1 = d2 = ripemd->digest[3];
    e1 = e2 = ripemd->digest[4];

    Subround(F, a1, b1, c1, d1, e1, ripemd->buffer[ 0], 11, k0);
    Subround(F, e1, a1, b1, c1, d1, ripemd->buffer[ 1], 14, k0);
    Subround(F, d1, e1, a1, b1, c1, ripemd->buffer[ 2], 15, k0);
    Subround(F, c1, d1, e1, a1, b1, ripemd->buffer[ 3], 12, k0);
    Subround(F, b1, c1, d1, e1, a1, ripemd->buffer[ 4],  5, k0);
    Subround(F, a1, b1, c1, d1, e1, ripemd->buffer[ 5],  8, k0);
    Subround(F, e1, a1, b1, c1, d1, ripemd->buffer[ 6],  7, k0);
    Subround(F, d1, e1, a1, b1, c1, ripemd->buffer[ 7],  9, k0);
    Subround(F, c1, d1, e1, a1, b1, ripemd->buffer[ 8], 11, k0);
    Subround(F, b1, c1, d1, e1, a1, ripemd->buffer[ 9], 13, k0);
    Subround(F, a1, b1, c1, d1, e1, ripemd->buffer[10], 14, k0);
    Subround(F, e1, a1, b1, c1, d1, ripemd->buffer[11], 15, k0);
    Subround(F, d1, e1, a1, b1, c1, ripemd->buffer[12],  6, k0);
    Subround(F, c1, d1, e1, a1, b1, ripemd->buffer[13],  7, k0);
    Subround(F, b1, c1, d1, e1, a1, ripemd->buffer[14],  9, k0);
    Subround(F, a1, b1, c1, d1, e1, ripemd->buffer[15],  8, k0);

    Subround(G, e1, a1, b1, c1, d1, ripemd->buffer[ 7],  7, k1);
    Subround(G, d1, e1, a1, b1, c1, ripemd->buffer[ 4],  6, k1);
    Subround(G, c1, d1, e1, a1, b1, ripemd->buffer[13],  8, k1);
    Subround(G, b1, c1, d1, e1, a1, ripemd->buffer[ 1], 13, k1);
    Subround(G, a1, b1, c1, d1, e1, ripemd->buffer[10], 11, k1);
    Subround(G, e1, a1, b1, c1, d1, ripemd->buffer[ 6],  9, k1);
    Subround(G, d1, e1, a1, b1, c1, ripemd->buffer[15],  7, k1);
    Subround(G, c1, d1, e1, a1, b1, ripemd->buffer[ 3], 15, k1);
    Subround(G, b1, c1, d1, e1, a1, ripemd->buffer[12],  7, k1);
    Subround(G, a1, b1, c1, d1, e1, ripemd->buffer[ 0], 12, k1);
    Subround(G, e1, a1, b1, c1, d1, ripemd->buffer[ 9], 15, k1);
    Subround(G, d1, e1, a1, b1, c1, ripemd->buffer[ 5],  9, k1);
    Subround(G, c1, d1, e1, a1, b1, ripemd->buffer[ 2], 11, k1);
    Subround(G, b1, c1, d1, e1, a1, ripemd->buffer[14],  7, k1);
    Subround(G, a1, b1, c1, d1, e1, ripemd->buffer[11], 13, k1);
    Subround(G, e1, a1, b1, c1, d1, ripemd->buffer[ 8], 12, k1);

    Subround(H, d1, e1, a1, b1, c1, ripemd->buffer[ 3], 11, k2);
    Subround(H, c1, d1, e1, a1, b1, ripemd->buffer[10], 13, k2);
    Subround(H, b1, c1, d1, e1, a1, ripemd->buffer[14],  6, k2);
    Subround(H, a1, b1, c1, d1, e1, ripemd->buffer[ 4],  7, k2);
    Subround(H, e1, a1, b1, c1, d1, ripemd->buffer[ 9], 14, k2);
    Subround(H, d1, e1, a1, b1, c1, ripemd->buffer[15],  9, k2);
    Subround(H, c1, d1, e1, a1, b1, ripemd->buffer[ 8], 13, k2);
    Subround(H, b1, c1, d1, e1, a1, ripemd->buffer[ 1], 15, k2);
    Subround(H, a1, b1, c1, d1, e1, ripemd->buffer[ 2], 14, k2);
    Subround(H, e1, a1, b1, c1, d1, ripemd->buffer[ 7],  8, k2);
    Subround(H, d1, e1, a1, b1, c1, ripemd->buffer[ 0], 13, k2);
    Subround(H, c1, d1, e1, a1, b1, ripemd->buffer[ 6],  6, k2);
    Subround(H, b1, c1, d1, e1, a1, ripemd->buffer[13],  5, k2);
    Subround(H, a1, b1, c1, d1, e1, ripemd->buffer[11], 12, k2);
    Subround(H, e1, a1, b1, c1, d1, ripemd->buffer[ 5],  7, k2);
    Subround(H, d1, e1, a1, b1, c1, ripemd->buffer[12],  5, k2);

    Subround(I, c1, d1, e1, a1, b1, ripemd->buffer[ 1], 11, k3);
    Subround(I, b1, c1, d1, e1, a1, ripemd->buffer[ 9], 12, k3);
    Subround(I, a1, b1, c1, d1, e1, ripemd->buffer[11], 14, k3);
    Subround(I, e1, a1, b1, c1, d1, ripemd->buffer[10], 15, k3);
    Subround(I, d1, e1, a1, b1, c1, ripemd->buffer[ 0], 14, k3);
    Subround(I, c1, d1, e1, a1, b1, ripemd->buffer[ 8], 15, k3);
    Subround(I, b1, c1, d1, e1, a1, ripemd->buffer[12],  9, k3);
    Subround(I, a1, b1, c1, d1, e1, ripemd->buffer[ 4],  8, k3);
    Subround(I, e1, a1, b1, c1, d1, ripemd->buffer[13],  9, k3);
    Subround(I, d1, e1, a1, b1, c1, ripemd->buffer[ 3], 14, k3);
    Subround(I, c1, d1, e1, a1, b1, ripemd->buffer[ 7],  5, k3);
    Subround(I, b1, c1, d1, e1, a1, ripemd->buffer[15],  6, k3);
    Subround(I, a1, b1, c1, d1, e1, ripemd->buffer[14],  8, k3);
    Subround(I, e1, a1, b1, c1, d1, ripemd->buffer[ 5],  6, k3);
    Subround(I, d1, e1, a1, b1, c1, ripemd->buffer[ 6],  5, k3);
    Subround(I, c1, d1, e1, a1, b1, ripemd->buffer[ 2], 12, k3);

    Subround(J, b1, c1, d1, e1, a1, ripemd->buffer[ 4],  9, k4);
    Subround(J, a1, b1, c1, d1, e1, ripemd->buffer[ 0], 15, k4);
    Subround(J, e1, a1, b1, c1, d1, ripemd->buffer[ 5],  5, k4);
    Subround(J, d1, e1, a1, b1, c1, ripemd->buffer[ 9], 11, k4);
    Subround(J, c1, d1, e1, a1, b1, ripemd->buffer[ 7],  6, k4);
    Subround(J, b1, c1, d1, e1, a1, ripemd->buffer[12],  8, k4);
    Subround(J, a1, b1, c1, d1, e1, ripemd->buffer[ 2], 13, k4);
    Subround(J, e1, a1, b1, c1, d1, ripemd->buffer[10], 12, k4);
    Subround(J, d1, e1, a1, b1, c1, ripemd->buffer[14],  5, k4);
    Subround(J, c1, d1, e1, a1, b1, ripemd->buffer[ 1], 12, k4);
    Subround(J, b1, c1, d1, e1, a1, ripemd->buffer[ 3], 13, k4);
    Subround(J, a1, b1, c1, d1, e1, ripemd->buffer[ 8], 14, k4);
    Subround(J, e1, a1, b1, c1, d1, ripemd->buffer[11], 11, k4);
    Subround(J, d1, e1, a1, b1, c1, ripemd->buffer[ 6],  8, k4);
    Subround(J, c1, d1, e1, a1, b1, ripemd->buffer[15],  5, k4);
    Subround(J, b1, c1, d1, e1, a1, ripemd->buffer[13],  6, k4);

    Subround(J, a2, b2, c2, d2, e2, ripemd->buffer[ 5],  8, k5);
    Subround(J, e2, a2, b2, c2, d2, ripemd->buffer[14],  9, k5);
    Subround(J, d2, e2, a2, b2, c2, ripemd->buffer[ 7],  9, k5);
    Subround(J, c2, d2, e2, a2, b2, ripemd->buffer[ 0], 11, k5);
    Subround(J, b2, c2, d2, e2, a2, ripemd->buffer[ 9], 13, k5);
    Subround(J, a2, b2, c2, d2, e2, ripemd->buffer[ 2], 15, k5);
    Subround(J, e2, a2, b2, c2, d2, ripemd->buffer[11], 15, k5);
    Subround(J, d2, e2, a2, b2, c2, ripemd->buffer[ 4],  5, k5);
    Subround(J, c2, d2, e2, a2, b2, ripemd->buffer[13],  7, k5);
    Subround(J, b2, c2, d2, e2, a2, ripemd->buffer[ 6],  7, k5);
    Subround(J, a2, b2, c2, d2, e2, ripemd->buffer[15],  8, k5);
    Subround(J, e2, a2, b2, c2, d2, ripemd->buffer[ 8], 11, k5);
    Subround(J, d2, e2, a2, b2, c2, ripemd->buffer[ 1], 14, k5);
    Subround(J, c2, d2, e2, a2, b2, ripemd->buffer[10], 14, k5);
    Subround(J, b2, c2, d2, e2, a2, ripemd->buffer[ 3], 12, k5);
    Subround(J, a2, b2, c2, d2, e2, ripemd->buffer[12],  6, k5);

    Subround(I, e2, a2, b2, c2, d2, ripemd->buffer[ 6],  9, k6);
    Subround(I, d2, e2, a2, b2, c2, ripemd->buffer[11], 13, k6);
    Subround(I, c2, d2, e2, a2, b2, ripemd->buffer[ 3], 15, k6);
    Subround(I, b2, c2, d2, e2, a2, ripemd->buffer[ 7],  7, k6);
    Subround(I, a2, b2, c2, d2, e2, ripemd->buffer[ 0], 12, k6);
    Subround(I, e2, a2, b2, c2, d2, ripemd->buffer[13],  8, k6);
    Subround(I, d2, e2, a2, b2, c2, ripemd->buffer[ 5],  9, k6);
    Subround(I, c2, d2, e2, a2, b2, ripemd->buffer[10], 11, k6);
    Subround(I, b2, c2, d2, e2, a2, ripemd->buffer[14],  7, k6);
    Subround(I, a2, b2, c2, d2, e2, ripemd->buffer[15],  7, k6);
    Subround(I, e2, a2, b2, c2, d2, ripemd->buffer[ 8], 12, k6);
    Subround(I, d2, e2, a2, b2, c2, ripemd->buffer[12],  7, k6);
    Subround(I, c2, d2, e2, a2, b2, ripemd->buffer[ 4],  6, k6);
    Subround(I, b2, c2, d2, e2, a2, ripemd->buffer[ 9], 15, k6);
    Subround(I, a2, b2, c2, d2, e2, ripemd->buffer[ 1], 13, k6);
    Subround(I, e2, a2, b2, c2, d2, ripemd->buffer[ 2], 11, k6);

    Subround(H, d2, e2, a2, b2, c2, ripemd->buffer[15],  9, k7);
    Subround(H, c2, d2, e2, a2, b2, ripemd->buffer[ 5],  7, k7);
    Subround(H, b2, c2, d2, e2, a2, ripemd->buffer[ 1], 15, k7);
    Subround(H, a2, b2, c2, d2, e2, ripemd->buffer[ 3], 11, k7);
    Subround(H, e2, a2, b2, c2, d2, ripemd->buffer[ 7],  8, k7);
    Subround(H, d2, e2, a2, b2, c2, ripemd->buffer[14],  6, k7);
    Subround(H, c2, d2, e2, a2, b2, ripemd->buffer[ 6],  6, k7);
    Subround(H, b2, c2, d2, e2, a2, ripemd->buffer[ 9], 14, k7);
    Subround(H, a2, b2, c2, d2, e2, ripemd->buffer[11], 12, k7);
    Subround(H, e2, a2, b2, c2, d2, ripemd->buffer[ 8], 13, k7);
    Subround(H, d2, e2, a2, b2, c2, ripemd->buffer[12],  5, k7);
    Subround(H, c2, d2, e2, a2, b2, ripemd->buffer[ 2], 14, k7);
    Subround(H, b2, c2, d2, e2, a2, ripemd->buffer[10], 13, k7);
    Subround(H, a2, b2, c2, d2, e2, ripemd->buffer[ 0], 13, k7);
    Subround(H, e2, a2, b2, c2, d2, ripemd->buffer[ 4],  7, k7);
    Subround(H, d2, e2, a2, b2, c2, ripemd->buffer[13],  5, k7);

    Subround(G, c2, d2, e2, a2, b2, ripemd->buffer[ 8], 15, k8);
    Subround(G, b2, c2, d2, e2, a2, ripemd->buffer[ 6],  5, k8);
    Subround(G, a2, b2, c2, d2, e2, ripemd->buffer[ 4],  8, k8);
    Subround(G, e2, a2, b2, c2, d2, ripemd->buffer[ 1], 11, k8);
    Subround(G, d2, e2, a2, b2, c2, ripemd->buffer[ 3], 14, k8);
    Subround(G, c2, d2, e2, a2, b2, ripemd->buffer[11], 14, k8);
    Subround(G, b2, c2, d2, e2, a2, ripemd->buffer[15],  6, k8);
    Subround(G, a2, b2, c2, d2, e2, ripemd->buffer[ 0], 14, k8);
    Subround(G, e2, a2, b2, c2, d2, ripemd->buffer[ 5],  6, k8);
    Subround(G, d2, e2, a2, b2, c2, ripemd->buffer[12],  9, k8);
    Subround(G, c2, d2, e2, a2, b2, ripemd->buffer[ 2], 12, k8);
    Subround(G, b2, c2, d2, e2, a2, ripemd->buffer[13],  9, k8);
    Subround(G, a2, b2, c2, d2, e2, ripemd->buffer[ 9], 12, k8);
    Subround(G, e2, a2, b2, c2, d2, ripemd->buffer[ 7],  5, k8);
    Subround(G, d2, e2, a2, b2, c2, ripemd->buffer[10], 15, k8);
    Subround(G, c2, d2, e2, a2, b2, ripemd->buffer[14],  8, k8);

    Subround(F, b2, c2, d2, e2, a2, ripemd->buffer[12],  8, k9);
    Subround(F, a2, b2, c2, d2, e2, ripemd->buffer[15],  5, k9);
    Subround(F, e2, a2, b2, c2, d2, ripemd->buffer[10], 12, k9);
    Subround(F, d2, e2, a2, b2, c2, ripemd->buffer[ 4],  9, k9);
    Subround(F, c2, d2, e2, a2, b2, ripemd->buffer[ 1], 12, k9);
    Subround(F, b2, c2, d2, e2, a2, ripemd->buffer[ 5],  5, k9);
    Subround(F, a2, b2, c2, d2, e2, ripemd->buffer[ 8], 14, k9);
    Subround(F, e2, a2, b2, c2, d2, ripemd->buffer[ 7],  6, k9);
    Subround(F, d2, e2, a2, b2, c2, ripemd->buffer[ 6],  8, k9);
    Subround(F, c2, d2, e2, a2, b2, ripemd->buffer[ 2], 13, k9);
    Subround(F, b2, c2, d2, e2, a2, ripemd->buffer[13],  6, k9);
    Subround(F, a2, b2, c2, d2, e2, ripemd->buffer[14],  5, k9);
    Subround(F, e2, a2, b2, c2, d2, ripemd->buffer[ 0], 15, k9);
    Subround(F, d2, e2, a2, b2, c2, ripemd->buffer[ 3], 13, k9);
    Subround(F, c2, d2, e2, a2, b2, ripemd->buffer[ 9], 11, k9);
    Subround(F, b2, c2, d2, e2, a2, ripemd->buffer[11], 11, k9);

    c1                = ripemd->digest[1] + c1 + d2;
    ripemd->digest[1] = ripemd->digest[2] + d1 + e2;
    ripemd->digest[2] = ripemd->digest[3] + e1 + a2;
    ripemd->digest[3] = ripemd->digest[4] + a1 + b2;
    ripemd->digest[4] = ripemd->digest[0] + b1 + c2;
    ripemd->digest[0] = c1;
}


static WC_INLINE void AddLength(RipeMd* ripemd, word32 len)
{
    word32 tmp = ripemd->loLen;
    if ( (ripemd->loLen += len) < tmp)
        ripemd->hiLen++;                       /* carry low to high */
}


int wc_RipeMdUpdate(RipeMd* ripemd, const byte* data, word32 len)
{
    /* do block size increments */
    byte* local;

    if (ripemd == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

    local = (byte*)ripemd->buffer;

    while (len) {
        word32 add = min(len, RIPEMD_BLOCK_SIZE - ripemd->buffLen);
        XMEMCPY(&local[ripemd->buffLen], data, add);

        ripemd->buffLen += add;
        data         += add;
        len          -= add;

        if (ripemd->buffLen == RIPEMD_BLOCK_SIZE) {
            #ifdef BIG_ENDIAN_ORDER
                ByteReverseWords(ripemd->buffer, ripemd->buffer,
                                 RIPEMD_BLOCK_SIZE);
            #endif
            Transform(ripemd);
            AddLength(ripemd, RIPEMD_BLOCK_SIZE);
            ripemd->buffLen = 0;
        }
    }
    return 0;
}


int wc_RipeMdFinal(RipeMd* ripemd, byte* hash)
{
    byte* local;

    if (ripemd == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    local = (byte*)ripemd->buffer;

    AddLength(ripemd, ripemd->buffLen);               /* before adding pads */

    /* ensure we have a valid buffer length; */
    if (ripemd->buffLen > RIPEMD_BLOCK_SIZE) {
        /* exit with error code if there's a bad buffer size in buffLen */
        return BAD_STATE_E;
    } /* buffLen check */

    local[ripemd->buffLen++] = 0x80;  /* add 1 */

    /* pad with zeros */
    if (ripemd->buffLen > RIPEMD_PAD_SIZE) {
        XMEMSET(&local[ripemd->buffLen], 0, RIPEMD_BLOCK_SIZE - ripemd->buffLen);
        ripemd->buffLen += RIPEMD_BLOCK_SIZE - ripemd->buffLen;

        #ifdef BIG_ENDIAN_ORDER
            ByteReverseWords(ripemd->buffer, ripemd->buffer, RIPEMD_BLOCK_SIZE);
        #endif
        Transform(ripemd);
        ripemd->buffLen = 0;
    }
    XMEMSET(&local[ripemd->buffLen], 0, RIPEMD_PAD_SIZE - ripemd->buffLen);

    /* put lengths in bits */
    ripemd->loLen = ripemd->loLen << 3;
    ripemd->hiLen = (ripemd->loLen >> (8*sizeof(ripemd->loLen) - 3)) +
                 (ripemd->hiLen << 3);

    /* store lengths */
    #ifdef BIG_ENDIAN_ORDER
        ByteReverseWords(ripemd->buffer, ripemd->buffer, RIPEMD_BLOCK_SIZE);
    #endif
    /* ! length ordering dependent on digest endian type ! */
    XMEMCPY(&local[RIPEMD_PAD_SIZE], &ripemd->loLen, sizeof(word32));
    XMEMCPY(&local[RIPEMD_PAD_SIZE + sizeof(word32)], &ripemd->hiLen,
           sizeof(word32));

    Transform(ripemd);
    #ifdef BIG_ENDIAN_ORDER
        ByteReverseWords(ripemd->digest, ripemd->digest, RIPEMD_DIGEST_SIZE);
    #endif
    XMEMCPY(hash, ripemd->digest, RIPEMD_DIGEST_SIZE);

    return wc_InitRipeMd(ripemd);  /* reset state */
}


#endif /* WOLFSSL_RIPEMD */
