/* misc.h
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
This module implements the arithmetic-shift right, left, byte swapping, XOR,
masking and clearing memory logic.

*/
#ifndef WOLF_CRYPT_MISC_H
#define WOLF_CRYPT_MISC_H


#include <wolfssl/wolfcrypt/types.h>


#ifdef __cplusplus
    extern "C" {
#endif


#ifdef NO_INLINE

#define WC_MISC_STATIC

WOLFSSL_LOCAL
word32 rotlFixed(word32 x, word32 y);
WOLFSSL_LOCAL
word32 rotrFixed(word32 x, word32 y);

#ifdef WC_RC2
WOLFSSL_LOCAL
word16 rotlFixed16(word16 x, word16 y);
WOLFSSL_LOCAL
word16 rotrFixed16(word16 x, word16 y);
#endif

WOLFSSL_LOCAL
word32 ByteReverseWord32(word32 value);
WOLFSSL_LOCAL
void   ByteReverseWords(word32* out, const word32* in, word32 byteCount);

WOLFSSL_LOCAL
void XorWordsOut(wolfssl_word** r, const wolfssl_word** a,
        const wolfssl_word** b, word32 n);
WOLFSSL_LOCAL
void xorbufout(void* out, const void* buf, const void* mask, word32 count);
WOLFSSL_LOCAL
void XorWords(wolfssl_word** r, const wolfssl_word** a, word32 n);
WOLFSSL_LOCAL
void xorbuf(void* buf, const void* mask, word32 count);

WOLFSSL_LOCAL
void ForceZero(void* mem, word32 len);

WOLFSSL_LOCAL
int ConstantCompare(const byte* a, const byte* b, int length);

#ifdef WORD64_AVAILABLE
WOLFSSL_LOCAL
word64 rotlFixed64(word64 x, word64 y);
WOLFSSL_LOCAL
word64 rotrFixed64(word64 x, word64 y);

WOLFSSL_LOCAL
word64 ByteReverseWord64(word64 value);
WOLFSSL_LOCAL
void   ByteReverseWords64(word64* out, const word64* in, word32 byteCount);
#endif /* WORD64_AVAILABLE */

#ifndef WOLFSSL_HAVE_MIN
    #if defined(HAVE_FIPS) && !defined(min) /* so ifdef check passes */
        #define min min
    #endif
    WOLFSSL_LOCAL word32 min(word32 a, word32 b);
#endif

#ifndef WOLFSSL_HAVE_MAX
    #if defined(HAVE_FIPS) && !defined(max) /* so ifdef check passes */
        #define max max
    #endif
    WOLFSSL_LOCAL word32 max(word32 a, word32 b);
#endif /* WOLFSSL_HAVE_MAX */


void c32to24(word32 in, word24 out);
void c16toa(word16 wc_u16, byte* c);
void c32toa(word32 wc_u32, byte* c);
void c24to32(const word24 wc_u24, word32* wc_u32);
void ato16(const byte* c, word16* wc_u16);
void ato24(const byte* c, word32* wc_u24);
void ato32(const byte* c, word32* wc_u32);
void ato32le(const byte* c, word32* wc_u32);
word32 btoi(byte b);

WOLFSSL_LOCAL signed char HexCharToByte(char ch);
WOLFSSL_LOCAL char ByteToHex(byte in);
WOLFSSL_LOCAL int  ByteToHexStr(byte in, char* out);

WOLFSSL_LOCAL byte ctMaskGT(int a, int b);
WOLFSSL_LOCAL byte ctMaskGTE(int a, int b);
WOLFSSL_LOCAL int  ctMaskIntGTE(int a, int b);
WOLFSSL_LOCAL byte ctMaskLT(int a, int b);
WOLFSSL_LOCAL byte ctMaskLTE(int a, int b);
WOLFSSL_LOCAL byte ctMaskEq(int a, int b);
WOLFSSL_LOCAL word16 ctMask16GT(int a, int b);
WOLFSSL_LOCAL word16 ctMask16GTE(int a, int b);
WOLFSSL_LOCAL word16 ctMask16LT(int a, int b);
WOLFSSL_LOCAL word16 ctMask16LTE(int a, int b);
WOLFSSL_LOCAL word16 ctMask16Eq(int a, int b);
WOLFSSL_LOCAL byte ctMaskNotEq(int a, int b);
WOLFSSL_LOCAL byte ctMaskSel(byte m, byte a, byte b);
WOLFSSL_LOCAL int  ctMaskSelInt(byte m, int a, int b);
WOLFSSL_LOCAL word32 ctMaskSelWord32(byte m, word32 a, word32 b);
WOLFSSL_LOCAL byte ctSetLTE(int a, int b);
WOLFSSL_LOCAL void ctMaskCopy(byte mask, byte* dst, byte* src, word16 size);
WOLFSSL_LOCAL word32 MakeWordFromHash(const byte* hashID);
WOLFSSL_LOCAL word32 HashObject(const byte* o, word32 len, int* error);

WOLFSSL_LOCAL void w64Increment(w64wrapper *n);
WOLFSSL_LOCAL void w64Decrement(w64wrapper *n);
WOLFSSL_LOCAL byte w64Equal(w64wrapper a, w64wrapper b);
WOLFSSL_LOCAL word32 w64GetLow32(w64wrapper n);
WOLFSSL_LOCAL word32 w64GetHigh32(w64wrapper n);
WOLFSSL_LOCAL void w64SetLow32(w64wrapper *n, word32 low);
WOLFSSL_LOCAL w64wrapper w64Add32(w64wrapper a, word32 b, byte *wrap);
WOLFSSL_LOCAL w64wrapper w64Sub32(w64wrapper a, word32 b, byte *wrap);
WOLFSSL_LOCAL byte w64GT(w64wrapper a, w64wrapper b);
WOLFSSL_LOCAL byte w64IsZero(w64wrapper a);
WOLFSSL_LOCAL void c64toa(const w64wrapper *a, byte *out);
WOLFSSL_LOCAL void ato64(const byte *in, w64wrapper *w64);
WOLFSSL_LOCAL w64wrapper w64From32(word32 hi, word32 lo);
WOLFSSL_LOCAL byte w64GTE(w64wrapper a, w64wrapper b);
WOLFSSL_LOCAL byte w64LT(w64wrapper a, w64wrapper b);
WOLFSSL_LOCAL w64wrapper w64Sub(w64wrapper a, w64wrapper b);
WOLFSSL_LOCAL void w64Zero(w64wrapper *a);

#else /* !NO_INLINE */

#define WC_MISC_STATIC static

#endif /* NO_INLINE */


#ifdef __cplusplus
    }   /* extern "C" */
#endif


#endif /* WOLF_CRYPT_MISC_H */

