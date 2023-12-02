/* rc2.c
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

DESCRIPTION
This library provides the interface to the RC2 encryption algorithm (RFC 2268)

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WC_RC2

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#include <wolfssl/wolfcrypt/rc2.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

/* Table based on value of PI, defined in RFC 2268 */
static const byte pitable[256] = {
    0xd9, 0x78, 0xf9, 0xc4, 0x19, 0xdd, 0xb5, 0xed,
    0x28, 0xe9, 0xfd, 0x79, 0x4a, 0xa0, 0xd8, 0x9d,
    0xc6, 0x7e, 0x37, 0x83, 0x2b, 0x76, 0x53, 0x8e,
    0x62, 0x4c, 0x64, 0x88, 0x44, 0x8b, 0xfb, 0xa2,
    0x17, 0x9a, 0x59, 0xf5, 0x87, 0xb3, 0x4f, 0x13,
    0x61, 0x45, 0x6d, 0x8d, 0x09, 0x81, 0x7d, 0x32,
    0xbd, 0x8f, 0x40, 0xeb, 0x86, 0xb7, 0x7b, 0x0b,
    0xf0, 0x95, 0x21, 0x22, 0x5c, 0x6b, 0x4e, 0x82,
    0x54, 0xd6, 0x65, 0x93, 0xce, 0x60, 0xb2, 0x1c,
    0x73, 0x56, 0xc0, 0x14, 0xa7, 0x8c, 0xf1, 0xdc,
    0x12, 0x75, 0xca, 0x1f, 0x3b, 0xbe, 0xe4, 0xd1,
    0x42, 0x3d, 0xd4, 0x30, 0xa3, 0x3c, 0xb6, 0x26,
    0x6f, 0xbf, 0x0e, 0xda, 0x46, 0x69, 0x07, 0x57,
    0x27, 0xf2, 0x1d, 0x9b, 0xbc, 0x94, 0x43, 0x03,
    0xf8, 0x11, 0xc7, 0xf6, 0x90, 0xef, 0x3e, 0xe7,
    0x06, 0xc3, 0xd5, 0x2f, 0xc8, 0x66, 0x1e, 0xd7,
    0x08, 0xe8, 0xea, 0xde, 0x80, 0x52, 0xee, 0xf7,
    0x84, 0xaa, 0x72, 0xac, 0x35, 0x4d, 0x6a, 0x2a,
    0x96, 0x1a, 0xd2, 0x71, 0x5a, 0x15, 0x49, 0x74,
    0x4b, 0x9f, 0xd0, 0x5e, 0x04, 0x18, 0xa4, 0xec,
    0xc2, 0xe0, 0x41, 0x6e, 0x0f, 0x51, 0xcb, 0xcc,
    0x24, 0x91, 0xaf, 0x50, 0xa1, 0xf4, 0x70, 0x39,
    0x99, 0x7c, 0x3a, 0x85, 0x23, 0xb8, 0xb4, 0x7a,
    0xfc, 0x02, 0x36, 0x5b, 0x25, 0x55, 0x97, 0x31,
    0x2d, 0x5d, 0xfa, 0x98, 0xe3, 0x8a, 0x92, 0xae,
    0x05, 0xdf, 0x29, 0x10, 0x67, 0x6c, 0xba, 0xc9,
    0xd3, 0x00, 0xe6, 0xcf, 0xe1, 0x9e, 0xa8, 0x2c,
    0x63, 0x16, 0x01, 0x3f, 0x58, 0xe2, 0x89, 0xa9,
    0x0d, 0x38, 0x34, 0x1b, 0xab, 0x33, 0xff, 0xb0,
    0xbb, 0x48, 0x0c, 0x5f, 0xb9, 0xb1, 0xcd, 0x2e,
    0xc5, 0xf3, 0xdb, 0x47, 0xe5, 0xa5, 0x9c, 0x77,
    0x0a, 0xa6, 0x20, 0x68, 0xfe, 0x7f, 0xc1, 0xad
};

/**
  Sets RC2 IV, for use with CBC mode.
  rc2       RC2 structure to load IV into
  iv        IV, of size RC2_BLOCK_SIZE octets
  return    0 on success, negative on error
*/
int wc_Rc2SetIV(Rc2* rc2, const byte* iv)
{
    if (rc2 == NULL)
        return BAD_FUNC_ARG;

    if (iv)
        XMEMCPY(rc2->reg, iv, RC2_BLOCK_SIZE);
    else
        XMEMSET(rc2->reg,  0, RC2_BLOCK_SIZE);

    return 0;
}

/**
   Set RC2 key, performing key expansion operation
   rc2      RC2 structure to load expanded key into
   key      User key, up to 64 bytes
   length   Length of key, octets
   bits     Effective RC2 key length in bits (max 1024 bits)
   return   0 on success, negative on error
 */
int wc_Rc2SetKey(Rc2* rc2, const byte* key, word32 length,
                 const byte* iv, word32 bits)
{
    int i;
    unsigned int T8, TM;
    byte* L = NULL;

    if (rc2 == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (length == 0 || length > 128 || bits == 0 || bits > 1024) {
        return WC_KEY_SIZE_E;
    }

    rc2->keylen = length;
    rc2->bits   = bits;

    L = (byte*)rc2->key;
    XMEMCPY(L, key, length);

    /* compute effective key length in bytes (T8) */
    T8 = (bits + 7) >> 3;

    /* TM mask has 8 - (8*T8 - T1) least significant bits set */
    TM = 0xff >> (8*T8 - bits);

    /* key expansion */
    for (i = length; i < RC2_MAX_KEY_SIZE; i++) {
        L[i] = pitable[(L[i-1] + L[i-length]) & 255];
    }

    L[RC2_MAX_KEY_SIZE - T8] = pitable[L[RC2_MAX_KEY_SIZE - T8] & TM];

    for (i = RC2_MAX_KEY_SIZE-T8-1; i >= 0; i--) {
        L[i] = pitable[L[i+1] ^ L[i+T8]];
    }

    /* store key into 16-bit word format */
    for (i = 0; i < RC2_MAX_KEY_SIZE/2; i++) {
       rc2->key[i] = (word16)L[2*i] + ((word16)L[2*i+1] << 8);
    }

    return wc_Rc2SetIV(rc2, iv);
}

/**
   RC2 ECB encrypt operation on one single RC2_BLOCK_SIZE block.
   rc2      Initialized RC2 structure
   out      [out] Destination for the encrypted ciphertext
   in       Input plaintext to be encrypted
   sz       Size of the output buffer, out
   return   0 on success, negative on error
*/
int wc_Rc2EcbEncrypt(Rc2* rc2, byte* out, const byte* in, word32 sz)
{
    int i, j = 0;
    word16 r10, r32, r54, r76;
    word16* key;

    if (rc2 == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }
    key = rc2->key;

    if (sz != RC2_BLOCK_SIZE) {
        return BUFFER_E;
    }

    r10 = (in[1] << 8) | in[0]; /* R[0] */
    r32 = (in[3] << 8) | in[2]; /* R[1] */
    r54 = (in[5] << 8) | in[4]; /* R[2] */
    r76 = (in[7] << 8) | in[6]; /* R[3] */

    for (i = 0; i < 16; i++) {
        j = i * 4;

        /* mixing round */
        r10 = r10 + key[j] + (r76 & r54) + (~r76 & r32);
        r10 = rotlFixed16(r10, 1);

        r32 = r32 + key[j+1] + (r10 & r76) + (~r10 & r54);
        r32 = rotlFixed16(r32, 2);

        r54 = r54 + key[j+2] + (r32 & r10) + (~r32 & r76);
        r54 = rotlFixed16(r54, 3);

        r76 = r76 + key[j+3] + (r54 & r32) + (~r54 & r10);
        r76 = rotlFixed16(r76, 5);

        /* mashing round on loop 5, 11 */
        if (i == 4 || i == 10) {
            r10 = r10 + key[r76 & 63];
            r32 = r32 + key[r10 & 63];
            r54 = r54 + key[r32 & 63];
            r76 = r76 + key[r54 & 63];
        }
    }

    out[0] = (byte)r10;
    out[1] = (byte)(r10 >> 8);
    out[2] = (byte)r32;
    out[3] = (byte)(r32 >> 8);
    out[4] = (byte)r54;
    out[5] = (byte)(r54 >> 8);
    out[6] = (byte)r76;
    out[7] = (byte)(r76 >> 8);

    return 0;
}

/**
   RC2 ECB decrypt operation on one single RC2_BLOCK_SIZE block.
   rc2      Initialized RC2 structure
   out      [out] Destination for decrypted plaintext
   in       Input ciphertext to be decrypted
   sz       Size of the output buffer, out
   return   0 on success, negative on error
*/
int wc_Rc2EcbDecrypt(Rc2* rc2, byte* out, const byte* in, word32 sz)
{
    int i, j = 63;
    word16 r0, r1, r2, r3;
    word16* key;

    if (rc2 == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }
    key = rc2->key;

    if (sz != RC2_BLOCK_SIZE) {
        return BUFFER_E;
    }

    r0 = (in[1] << 8) | in[0];
    r1 = (in[3] << 8) | in[2];
    r2 = (in[5] << 8) | in[4];
    r3 = (in[7] << 8) | in[6];

    for (i = 16; i > 0; i--) {
        j = 4*i - 1;

        r3 = rotrFixed16(r3, 5);
        r3 = r3 - key[j] - (r2 & r1) - (~r2 & r0);

        r2 = rotrFixed16(r2, 3);
        r2 = r2 - key[j-1] - (r1 & r0) - (~r1 & r3);

        r1 = rotrFixed16(r1, 2);
        r1 = r1 - key[j-2] - (r0 & r3) - (~r0 & r2);

        r0 = rotrFixed16(r0, 1);
        r0 = r0 - key[j-3] - (r3 & r2) - (~r3 & r1);

        if (i == 12 || i == 6) {
            r3 = r3 - key[r2 & 63];
            r2 = r2 - key[r1 & 63];
            r1 = r1 - key[r0 & 63];
            r0 = r0 - key[r3 & 63];
        }
    }

    out[0] = (byte)r0;
    out[1] = (byte)(r0 >> 8);
    out[2] = (byte)r1;
    out[3] = (byte)(r1 >> 8);
    out[4] = (byte)r2;
    out[5] = (byte)(r2 >> 8);
    out[6] = (byte)r3;
    out[7] = (byte)(r3 >> 8);

    return 0;
}

int wc_Rc2CbcEncrypt(Rc2* rc2, byte* out, const byte* in, word32 sz)
{
    int ret;
    word32 blocks = (sz / RC2_BLOCK_SIZE);

    if (rc2 == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (sz == 0) {
        return 0;
    }

    while (blocks--) {
        xorbuf((byte*)rc2->reg, in, RC2_BLOCK_SIZE);
        ret = wc_Rc2EcbEncrypt(rc2, (byte*)rc2->reg, (byte*)rc2->reg,
                               RC2_BLOCK_SIZE);
        if (ret != 0) {
            return ret;
        }
        XMEMCPY(out, rc2->reg, RC2_BLOCK_SIZE);

        out += RC2_BLOCK_SIZE;
        in  += RC2_BLOCK_SIZE;
    }

    return 0;
}

int wc_Rc2CbcDecrypt(Rc2* rc2, byte* out, const byte* in, word32 sz)
{
    int ret;
    word32 blocks = (sz / RC2_BLOCK_SIZE);

    if (rc2 == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (sz == 0) {
        return 0;
    }

    while (blocks--) {
        XMEMCPY(rc2->tmp, in, RC2_BLOCK_SIZE);
        ret = wc_Rc2EcbDecrypt(rc2, out, (byte*)rc2->tmp, RC2_BLOCK_SIZE);
        if (ret != 0) {
            return ret;
        }
        xorbuf(out, (byte*)rc2->reg, RC2_BLOCK_SIZE);
        /* store iv for next call */
        XMEMCPY(rc2->reg, rc2->tmp, RC2_BLOCK_SIZE);

        out += RC2_BLOCK_SIZE;
        in  += RC2_BLOCK_SIZE;
    }

    return 0;
}


#endif /* WC_RC2 */

