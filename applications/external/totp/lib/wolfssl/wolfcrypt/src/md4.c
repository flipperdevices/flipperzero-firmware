/* md4.c
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

#ifndef NO_MD4

#include <wolfssl/wolfcrypt/md4.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


void wc_InitMd4(Md4* md4)
{
    md4->digest[0] = 0x67452301L;
    md4->digest[1] = 0xefcdab89L;
    md4->digest[2] = 0x98badcfeL;
    md4->digest[3] = 0x10325476L;

    md4->buffLen = 0;
    md4->loLen   = 0;
    md4->hiLen   = 0;
}


static void Transform(Md4* md4)
{
#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

    /* Copy context->state[] to working vars  */
    word32 A = md4->digest[0];
    word32 B = md4->digest[1];
    word32 C = md4->digest[2];
    word32 D = md4->digest[3];

#define function(a,b,c,d,k,s) (a)=rotlFixed((a)+F(b,c,d)+md4->buffer[k],s);
    function(A,B,C,D, 0, 3);
    function(D,A,B,C, 1, 7);
    function(C,D,A,B, 2,11);
    function(B,C,D,A, 3,19);
    function(A,B,C,D, 4, 3);
    function(D,A,B,C, 5, 7);
    function(C,D,A,B, 6,11);
    function(B,C,D,A, 7,19);
    function(A,B,C,D, 8, 3);
    function(D,A,B,C, 9, 7);
    function(C,D,A,B,10,11);
    function(B,C,D,A,11,19);
    function(A,B,C,D,12, 3);
    function(D,A,B,C,13, 7);
    function(C,D,A,B,14,11);
    function(B,C,D,A,15,19);

#undef function
#define function(a,b,c,d,k,s) \
    (a)=rotlFixed((a)+G(b,c,d)+md4->buffer[k]+0x5a827999,s);

    function(A,B,C,D, 0, 3);
    function(D,A,B,C, 4, 5);
    function(C,D,A,B, 8, 9);
    function(B,C,D,A,12,13);
    function(A,B,C,D, 1, 3);
    function(D,A,B,C, 5, 5);
    function(C,D,A,B, 9, 9);
    function(B,C,D,A,13,13);
    function(A,B,C,D, 2, 3);
    function(D,A,B,C, 6, 5);
    function(C,D,A,B,10, 9);
    function(B,C,D,A,14,13);
    function(A,B,C,D, 3, 3);
    function(D,A,B,C, 7, 5);
    function(C,D,A,B,11, 9);
    function(B,C,D,A,15,13);

#undef function
#define function(a,b,c,d,k,s) \
    (a)=rotlFixed((a)+H(b,c,d)+md4->buffer[k]+0x6ed9eba1,s);

    function(A,B,C,D, 0, 3);
    function(D,A,B,C, 8, 9);
    function(C,D,A,B, 4,11);
    function(B,C,D,A,12,15);
    function(A,B,C,D, 2, 3);
    function(D,A,B,C,10, 9);
    function(C,D,A,B, 6,11);
    function(B,C,D,A,14,15);
    function(A,B,C,D, 1, 3);
    function(D,A,B,C, 9, 9);
    function(C,D,A,B, 5,11);
    function(B,C,D,A,13,15);
    function(A,B,C,D, 3, 3);
    function(D,A,B,C,11, 9);
    function(C,D,A,B, 7,11);
    function(B,C,D,A,15,15);

    /* Add the working vars back into digest state[]  */
    md4->digest[0] += A;
    md4->digest[1] += B;
    md4->digest[2] += C;
    md4->digest[3] += D;
}


static WC_INLINE void AddLength(Md4* md4, word32 len)
{
    word32 tmp = md4->loLen;
    if ( (md4->loLen += len) < tmp)
        md4->hiLen++;                       /* carry low to high */
}


void wc_Md4Update(Md4* md4, const byte* data, word32 len)
{
    /* do block size increments */
    byte* local = (byte*)md4->buffer;

    while (len) {
        word32 add = min(len, MD4_BLOCK_SIZE - md4->buffLen);
        XMEMCPY(&local[md4->buffLen], data, add);

        md4->buffLen += add;
        data         += add;
        len          -= add;

        if (md4->buffLen == MD4_BLOCK_SIZE) {
            #ifdef BIG_ENDIAN_ORDER
                ByteReverseWords(md4->buffer, md4->buffer, MD4_BLOCK_SIZE);
            #endif
            Transform(md4);
            AddLength(md4, MD4_BLOCK_SIZE);
            md4->buffLen = 0;
        }
    }
}


void wc_Md4Final(Md4* md4, byte* hash)
{
    byte* local = (byte*)md4->buffer;

    AddLength(md4, md4->buffLen);               /* before adding pads */

    local[md4->buffLen++] = 0x80;  /* add 1 */

    /* pad with zeros */
    if (md4->buffLen > MD4_PAD_SIZE) {
        XMEMSET(&local[md4->buffLen], 0, MD4_BLOCK_SIZE - md4->buffLen);
        md4->buffLen += MD4_BLOCK_SIZE - md4->buffLen;

        #ifdef BIG_ENDIAN_ORDER
            ByteReverseWords(md4->buffer, md4->buffer, MD4_BLOCK_SIZE);
        #endif
        Transform(md4);
        md4->buffLen = 0;
    }
    XMEMSET(&local[md4->buffLen], 0, MD4_PAD_SIZE - md4->buffLen);

    /* put lengths in bits */
    md4->hiLen = (md4->loLen >> (8*sizeof(md4->loLen) - 3)) +
                 (md4->hiLen << 3);
    md4->loLen = md4->loLen << 3;

    /* store lengths */
    #ifdef BIG_ENDIAN_ORDER
        ByteReverseWords(md4->buffer, md4->buffer, MD4_BLOCK_SIZE);
    #endif
    /* ! length ordering dependent on digest endian type ! */
    XMEMCPY(&local[MD4_PAD_SIZE], &md4->loLen, sizeof(word32));
    XMEMCPY(&local[MD4_PAD_SIZE + sizeof(word32)], &md4->hiLen, sizeof(word32));

    Transform(md4);
    #ifdef BIG_ENDIAN_ORDER
        ByteReverseWords(md4->digest, md4->digest, MD4_DIGEST_SIZE);
    #endif
    XMEMCPY(hash, md4->digest, MD4_DIGEST_SIZE);

    wc_InitMd4(md4);  /* reset state */
}


#endif /* NO_MD4 */

