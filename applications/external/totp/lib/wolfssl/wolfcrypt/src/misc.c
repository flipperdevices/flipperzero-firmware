/* misc.c
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
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLF_CRYPT_MISC_C
#define WOLF_CRYPT_MISC_C

#include <wolfssl/wolfcrypt/misc.h>

/* inlining these functions is a huge speed increase and a small size decrease,
   because the functions are smaller than function call setup/cleanup, e.g.,
   md5 benchmark is twice as fast with inline.  If you don't want it, then
   define NO_INLINE and compile this file into wolfssl, otherwise it's used as
   a source header
 */

/* Check for if compiling misc.c when not needed. */
#if !defined(WOLFSSL_MISC_INCLUDED) && !defined(NO_INLINE)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning misc.c does not need to be compiled when using inline (NO_INLINE not defined)
    #endif

#else


#if defined(__ICCARM__)
    #include <intrinsics.h>
#endif


#ifdef INTEL_INTRINSICS

    #include <stdlib.h>      /* get intrinsic definitions */

    /* for non visual studio probably need no long version, 32 bit only
     * i.e., _rotl and _rotr */
    #pragma intrinsic(_lrotl, _lrotr)

    WC_MISC_STATIC WC_INLINE word32 rotlFixed(word32 x, word32 y)
    {
        return y ? _lrotl(x, y) : x;
    }

    WC_MISC_STATIC WC_INLINE word32 rotrFixed(word32 x, word32 y)
    {
        return y ? _lrotr(x, y) : x;
    }

#elif defined(__CCRX__)

    #include <builtin.h>      /* get intrinsic definitions */

    #if !defined(NO_INLINE)

    #define rotlFixed(x, y) _builtin_rotl(x, y)

    #define rotrFixed(x, y) _builtin_rotr(x, y)

    #else /* create real function */

    WC_MISC_STATIC WC_INLINE word32 rotlFixed(word32 x, word32 y)
    {
        return _builtin_rotl(x, y);
    }

    WC_MISC_STATIC WC_INLINE word32 rotrFixed(word32 x, word32 y)
    {
        return _builtin_rotr(x, y);
    }

    #endif

#else /* generic */
/* This routine performs a left circular arithmetic shift of <x> by <y> value. */

    WC_MISC_STATIC WC_INLINE word32 rotlFixed(word32 x, word32 y)
    {
        return (x << y) | (x >> (sizeof(x) * 8 - y));
    }

/* This routine performs a right circular arithmetic shift of <x> by <y> value. */
    WC_MISC_STATIC WC_INLINE word32 rotrFixed(word32 x, word32 y)
    {
        return (x >> y) | (x << (sizeof(x) * 8 - y));
    }

#endif

#ifdef WC_RC2

/* This routine performs a left circular arithmetic shift of <x> by <y> value */
WC_MISC_STATIC WC_INLINE word16 rotlFixed16(word16 x, word16 y)
{
    return (x << y) | (x >> (sizeof(x) * 8 - y));
}


/* This routine performs a right circular arithmetic shift of <x> by <y> value */
WC_MISC_STATIC WC_INLINE word16 rotrFixed16(word16 x, word16 y)
{
    return (x >> y) | (x << (sizeof(x) * 8 - y));
}

#endif /* WC_RC2 */

/* This routine performs a byte swap of 32-bit word value. */
#if defined(__CCRX__) && !defined(NO_INLINE) /* shortest version for CC-RX */
    #define ByteReverseWord32(value) _builtin_revl(value)
#else
WC_MISC_STATIC WC_INLINE word32 ByteReverseWord32(word32 value)
{
#ifdef PPC_INTRINSICS
    /* PPC: load reverse indexed instruction */
    return (word32)__lwbrx(&value,0);
#elif defined(__ICCARM__)
    return (word32)__REV(value);
#elif defined(KEIL_INTRINSICS)
    return (word32)__rev(value);
#elif defined(__CCRX__)
    return (word32)_builtin_revl(value);
#elif defined(WOLF_ALLOW_BUILTIN) && \
        defined(__GNUC_PREREQ) && __GNUC_PREREQ(4, 3)
    return (word32)__builtin_bswap32(value);
#elif defined(WOLFSSL_BYTESWAP32_ASM) && defined(__GNUC__) && \
      defined(__aarch64__)
    __asm__ volatile (
        "REV32 %0, %0  \n"
        : "+r" (value)
        :
    );
    return value;
#elif defined(WOLFSSL_BYTESWAP32_ASM) && defined(__GNUC__) && \
      (defined(__thumb__) || defined(__arm__))
    __asm__ volatile (
        "REV %0, %0  \n"
        : "+r" (value)
        :
    );
    return value;
#elif defined(FAST_ROTATE)
    /* 5 instructions with rotate instruction, 9 without */
    return (rotrFixed(value, 8U) & 0xff00ff00) |
           (rotlFixed(value, 8U) & 0x00ff00ff);
#else
    /* 6 instructions with rotate instruction, 8 without */
    value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
    return rotlFixed(value, 16U);
#endif
}
#endif /* __CCRX__ */
/* This routine performs a byte swap of words array of a given count. */
WC_MISC_STATIC WC_INLINE void ByteReverseWords(word32* out, const word32* in,
                                    word32 byteCount)
{
    word32 i;

#ifdef WOLFSSL_USE_ALIGN
    if ((((size_t)in & 0x3) == 0) &&
        (((size_t)out & 0x3) == 0))
#endif
    {
        word32 count = byteCount/(word32)sizeof(word32);
        for (i = 0; i < count; i++)
            out[i] = ByteReverseWord32(in[i]);
    }
#ifdef WOLFSSL_USE_ALIGN
    else {
        byte *in_bytes = (byte *)in;
        byte *out_bytes = (byte *)out;
        word32 scratch;

        byteCount &= ~0x3U;

        for (i = 0; i < byteCount; i += sizeof(word32)) {
            XMEMCPY(&scratch, in_bytes + i, sizeof(scratch));
            scratch = ByteReverseWord32(scratch);
            XMEMCPY(out_bytes + i, &scratch, sizeof(scratch));
        }
    }
#endif
}

#if defined(WORD64_AVAILABLE) && !defined(WOLFSSL_NO_WORD64_OPS)


WC_MISC_STATIC WC_INLINE word64 rotlFixed64(word64 x, word64 y)
{
    return (x << y) | (x >> (sizeof(y) * 8 - y));
}


WC_MISC_STATIC WC_INLINE word64 rotrFixed64(word64 x, word64 y)
{
    return (x >> y) | (x << (sizeof(y) * 8 - y));
}


WC_MISC_STATIC WC_INLINE word64 ByteReverseWord64(word64 value)
{
#if defined(WOLF_ALLOW_BUILTIN) && defined(__GNUC_PREREQ) && __GNUC_PREREQ(4, 3)
    return (word64)__builtin_bswap64(value);
#elif defined(WOLFCRYPT_SLOW_WORD64)
    return (word64)((word64)ByteReverseWord32((word32) value)) << 32 |
        (word64)ByteReverseWord32((word32)(value   >> 32));
#else
    value = ((value & W64LIT(0xFF00FF00FF00FF00)) >> 8) |
        ((value & W64LIT(0x00FF00FF00FF00FF)) << 8);
    value = ((value & W64LIT(0xFFFF0000FFFF0000)) >> 16) |
        ((value & W64LIT(0x0000FFFF0000FFFF)) << 16);
    return rotlFixed64(value, 32U);
#endif
}


WC_MISC_STATIC WC_INLINE void ByteReverseWords64(word64* out, const word64* in,
                                      word32 byteCount)
{
    word32 count = byteCount/(word32)sizeof(word64), i;

    for (i = 0; i < count; i++)
        out[i] = ByteReverseWord64(in[i]);

}

#endif /* WORD64_AVAILABLE && !WOLFSSL_NO_WORD64_OPS */

#ifndef WOLFSSL_NO_XOR_OPS
/* This routine performs a bitwise XOR operation of <*r> and <*a> for <n> number
of wolfssl_words, placing the result in <*r>. */
WC_MISC_STATIC WC_INLINE void XorWordsOut(wolfssl_word** r,
                       const wolfssl_word** a, const wolfssl_word** b, word32 n)
{
    word32 i;

    for (i = 0; i < n; i++)
        *((*r)++) = *((*a)++) ^ *((*b)++);
}

/* This routine performs a bitwise XOR operation of <*buf> and <*mask> of n
counts, placing the result in <*buf>. */

WC_MISC_STATIC WC_INLINE void xorbufout(void* out, const void* buf,
                                        const void* mask, word32 count)
{
    word32      i;
    byte*       o;
    byte*       b;
    const byte* m;

    o = (byte*)out;
    b = (byte*)buf;
    m = (const byte*)mask;


    if (((wc_ptr_t)o) % WOLFSSL_WORD_SIZE ==
            ((wc_ptr_t)b) % WOLFSSL_WORD_SIZE &&
            ((wc_ptr_t)b) % WOLFSSL_WORD_SIZE ==
                        ((wc_ptr_t)m) % WOLFSSL_WORD_SIZE) {
        /* Alignment checks out. Possible to XOR words. */
        /* Move alignment so that it lines up with a
         * WOLFSSL_WORD_SIZE boundary */
        while (((wc_ptr_t)b) % WOLFSSL_WORD_SIZE != 0 && count > 0) {
            *(o++) = (byte)(*(b++) ^ *(m++));
            count--;
        }
        XorWordsOut( (wolfssl_word**)&o, (const wolfssl_word**)&b,
                     (const wolfssl_word**)&m, count / WOLFSSL_WORD_SIZE);
        count %= WOLFSSL_WORD_SIZE;
    }

    for (i = 0; i < count; i++)
        o[i] = (byte)(b[i] ^ m[i]);
}

/* This routine performs a bitwise XOR operation of <*r> and <*a> for <n> number
of wolfssl_words, placing the result in <*r>. */
WC_MISC_STATIC WC_INLINE void XorWords(wolfssl_word** r, const wolfssl_word** a,
                                       word32 n)
{
    word32 i;

    for (i = 0; i < n; i++)
        *((*r)++) ^= *((*a)++);
}

/* This routine performs a bitwise XOR operation of <*buf> and <*mask> of n
counts, placing the result in <*buf>. */

WC_MISC_STATIC WC_INLINE void xorbuf(void* buf, const void* mask, word32 count)
{
    word32      i;
    byte*       b;
    const byte* m;

    b = (byte*)buf;
    m = (const byte*)mask;

    if (((wc_ptr_t)b) % WOLFSSL_WORD_SIZE ==
            ((wc_ptr_t)m) % WOLFSSL_WORD_SIZE) {
        /* Alignment checks out. Possible to XOR words. */
        /* Move alignment so that it lines up with a
         * WOLFSSL_WORD_SIZE boundary */
        while (((wc_ptr_t)buf) % WOLFSSL_WORD_SIZE != 0 && count > 0) {
            *(b++) ^= *(m++);
            count--;
        }
        XorWords( (wolfssl_word**)&b,
                  (const wolfssl_word**)&m, count / WOLFSSL_WORD_SIZE);
        count %= WOLFSSL_WORD_SIZE;
    }

    for (i = 0; i < count; i++)
        b[i] ^= m[i];
}
#endif

#ifndef WOLFSSL_NO_FORCE_ZERO
/* This routine fills the first len bytes of the memory area pointed by mem
   with zeros. It ensures compiler optimizations doesn't skip it  */
WC_MISC_STATIC WC_INLINE void ForceZero(void* mem, word32 len)
{
    volatile byte* z = (volatile byte*)mem;

#if (defined(WOLFSSL_X86_64_BUILD) || defined(WOLFSSL_AARCH64_BUILD)) \
            && defined(WORD64_AVAILABLE)
    volatile word64* w;
    #ifndef WOLFSSL_UNALIGNED_64BIT_ACCESS
        word32 l = (sizeof(word64) - ((size_t)z & (sizeof(word64)-1))) &
                                                             (sizeof(word64)-1);

        if (len < l) l = len;
        len -= l;
        while (l--) *z++ = 0;
    #endif
        for (w = (volatile word64*)z;
             len >= sizeof(*w);
             len -= (word32)sizeof(*w))
        {
            *w++ = 0;
        }
    z = (volatile byte*)w;
#endif

    while (len--) *z++ = 0;
}
#endif


#ifndef WOLFSSL_NO_CONST_CMP
/* check all length bytes for equality, return 0 on success */
WC_MISC_STATIC WC_INLINE int ConstantCompare(const byte* a, const byte* b,
                                             int length)
{
    int i;
    int compareSum = 0;

    for (i = 0; i < length; i++) {
        compareSum |= a[i] ^ b[i];
    }

    return compareSum;
}
#endif


#ifndef WOLFSSL_HAVE_MIN
    #define WOLFSSL_HAVE_MIN
    #if defined(HAVE_FIPS) && !defined(min) /* so ifdef check passes */
        #define min min
    #endif
    /* returns the smaller of a and b */
    WC_MISC_STATIC WC_INLINE word32 min(word32 a, word32 b)
    {
        return a > b ? b : a;
    }
#endif /* !WOLFSSL_HAVE_MIN */

#ifndef WOLFSSL_HAVE_MAX
    #define WOLFSSL_HAVE_MAX
    #if defined(HAVE_FIPS) && !defined(max) /* so ifdef check passes */
        #define max max
    #endif
    WC_MISC_STATIC WC_INLINE word32 max(word32 a, word32 b)
    {
        return a > b ? a : b;
    }
#endif /* !WOLFSSL_HAVE_MAX */

#ifndef WOLFSSL_NO_INT_ENCODE
/* converts a 32 bit integer to 24 bit */
WC_MISC_STATIC WC_INLINE void c32to24(word32 in, word24 out)
{
    out[0] = (byte)((in >> 16) & 0xff);
    out[1] = (byte)((in >>  8) & 0xff);
    out[2] =  (byte)(in        & 0xff);
}

/* convert 16 bit integer to opaque */
WC_MISC_STATIC WC_INLINE void c16toa(word16 wc_u16, byte* c)
{
    c[0] = (byte)((wc_u16 >> 8) & 0xff);
    c[1] =  (byte)(wc_u16       & 0xff);
}

/* convert 32 bit integer to opaque */
WC_MISC_STATIC WC_INLINE void c32toa(word32 wc_u32, byte* c)
{
    c[0] = (byte)((wc_u32 >> 24) & 0xff);
    c[1] = (byte)((wc_u32 >> 16) & 0xff);
    c[2] = (byte)((wc_u32 >>  8) & 0xff);
    c[3] =  (byte)(wc_u32        & 0xff);
}
#endif

#ifndef WOLFSSL_NO_INT_DECODE
/* convert a 24 bit integer into a 32 bit one */
WC_MISC_STATIC WC_INLINE void c24to32(const word24 wc_u24, word32* wc_u32)
{
    *wc_u32 = ((word32)wc_u24[0] << 16) |
              ((word32)wc_u24[1] << 8) |
               (word32)wc_u24[2];
}


/* convert opaque to 24 bit integer */
WC_MISC_STATIC WC_INLINE void ato24(const byte* c, word32* wc_u24)
{
    *wc_u24 = ((word32)c[0] << 16) | ((word32)c[1] << 8) | c[2];
}

/* convert opaque to 16 bit integer */
WC_MISC_STATIC WC_INLINE void ato16(const byte* c, word16* wc_u16)
{
    *wc_u16 = (word16) ((c[0] << 8) | (c[1]));
}

/* convert opaque to 32 bit integer */
WC_MISC_STATIC WC_INLINE void ato32(const byte* c, word32* wc_u32)
{
    *wc_u32 = ((word32)c[0] << 24) |
              ((word32)c[1] << 16) |
              ((word32)c[2] << 8) |
               (word32)c[3];
}

/* convert opaque to 32 bit integer. Interpret as little endian. */
WC_MISC_STATIC WC_INLINE void ato32le(const byte* c, word32* wc_u32)
{
    *wc_u32 =  (word32)c[0] |
              ((word32)c[1] << 8) |
              ((word32)c[2] << 16) |
              ((word32)c[3] << 24);
}


WC_MISC_STATIC WC_INLINE word32 btoi(byte b)
{
    return (word32)(b - 0x30);
}
#endif

WC_MISC_STATIC WC_INLINE signed char HexCharToByte(char ch)
{
    signed char ret = (signed char)ch;
    if (ret >= '0' && ret <= '9')
        ret -= '0';
    else if (ret >= 'A' && ret <= 'F')
        ret -= 'A' - 10;
    else if (ret >= 'a' && ret <= 'f')
        ret -= 'a' - 10;
    else
        ret = -1; /* error case - return code must be signed */
    return ret;
}

WC_MISC_STATIC WC_INLINE char ByteToHex(byte in)
{
    static const char kHexChar[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                     '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    return (char)(kHexChar[in & 0xF]);
}

WC_MISC_STATIC WC_INLINE int ByteToHexStr(byte in, char* out)
{
    if (out == NULL)
        return -1;

    out[0] = ByteToHex((byte)(in >> 4));
    out[1] = ByteToHex((byte)(in & 0xf));
    return 0;
}

#ifndef WOLFSSL_NO_CT_OPS
/* Constant time - mask set when a > b. */
WC_MISC_STATIC WC_INLINE byte ctMaskGT(int a, int b)
{
    return (byte)((((word32)a - (word32)b - 1) >> 31) - 1);
}

/* Constant time - mask set when a >= b. */
WC_MISC_STATIC WC_INLINE byte ctMaskGTE(int a, int b)
{
    return (byte)((((word32)a - (word32)b) >> 31) - 1);
}

/* Constant time - mask set when a >= b. */
WC_MISC_STATIC WC_INLINE int ctMaskIntGTE(int a, int b)
{
    return (int)((((word32)a - (word32)b) >> 31) - 1);
}

/* Constant time - mask set when a < b. */
WC_MISC_STATIC WC_INLINE byte ctMaskLT(int a, int b)
{
    return (byte)((((word32)b - (word32)a - 1) >> 31) - 1);
}

/* Constant time - mask set when a <= b. */
WC_MISC_STATIC WC_INLINE byte ctMaskLTE(int a, int b)
{
    return (byte)((((word32)b - (word32)a) >> 31) - 1);
}

/* Constant time - mask set when a == b. */
WC_MISC_STATIC WC_INLINE byte ctMaskEq(int a, int b)
{
    return (byte)((byte)(~ctMaskGT(a, b)) & (byte)(~ctMaskLT(a, b)));
}

/* Constant time - sets 16 bit integer mask when a > b */
WC_MISC_STATIC WC_INLINE word16 ctMask16GT(int a, int b)
{
    return (word16)((((word32)a - (word32)b - 1) >> 31) - 1);
}

/* Constant time - sets 16 bit integer mask when a >= b */
WC_MISC_STATIC WC_INLINE word16 ctMask16GTE(int a, int b)
{
    return (word16)((((word32)a - (word32)b) >> 31) - 1);
}

/* Constant time - sets 16 bit integer mask when a < b. */
WC_MISC_STATIC WC_INLINE word16 ctMask16LT(int a, int b)
{
    return (word16)((((word32)b - (word32)a - 1) >> 31) - 1);
}

/* Constant time - sets 16 bit integer mask when a <= b. */
WC_MISC_STATIC WC_INLINE word16 ctMask16LTE(int a, int b)
{
    return (word16)((((word32)b - (word32)a) >> 31) - 1);
}

/* Constant time - sets 16 bit integer mask when a == b. */
WC_MISC_STATIC WC_INLINE word16 ctMask16Eq(int a, int b)
{
    return (word16)((word16)(~ctMask16GT(a, b)) & (word16)(~ctMask16LT(a, b)));
}

/* Constant time - mask set when a != b. */
WC_MISC_STATIC WC_INLINE byte ctMaskNotEq(int a, int b)
{
    return (byte)((byte)ctMaskGT(a, b) | (byte)ctMaskLT(a, b));
}

/* Constant time - select a when mask is set and b otherwise. */
WC_MISC_STATIC WC_INLINE byte ctMaskSel(byte m, byte a, byte b)
{
    return (byte)((b & ((byte)~(word32)m)) | (a & m));
}

/* Constant time - select integer a when mask is set and integer b otherwise. */
WC_MISC_STATIC WC_INLINE int ctMaskSelInt(byte m, int a, int b)
{
    return (b & (~(signed int)(signed char)m)) |
           (a & ( (signed int)(signed char)m));
}

/* Constant time - select word32 a when mask is set and word32 b otherwise. */
WC_MISC_STATIC WC_INLINE word32 ctMaskSelWord32(byte m, word32 a, word32 b)
{
    return (((word32)b & (word32)(~(signed int)(signed char)m)) |
            ((word32)a & (word32)( (signed int)(signed char)m)));
}

/* Constant time - bit set when a <= b. */
WC_MISC_STATIC WC_INLINE byte ctSetLTE(int a, int b)
{
    return (byte)(((word32)a - (word32)b - 1) >> 31);
}

/* Constant time - conditionally copy size bytes from src to dst if mask is set
 */
WC_MISC_STATIC WC_INLINE void ctMaskCopy(byte mask, byte* dst, byte* src,
    word16 size)
{
    int i;
    for (i = 0; i < size; ++i) {
        dst[i] ^= (dst[i] ^ src[i]) & mask;
    }
}

#endif

#if defined(WOLFSSL_W64_WRAPPER)
#if defined(WORD64_AVAILABLE) && !defined(WOLFSSL_W64_WRAPPER_TEST)
WC_MISC_STATIC WC_INLINE void w64Increment(w64wrapper *n) {
    n->n++;
}

WC_MISC_STATIC WC_INLINE void w64Decrement(w64wrapper *n) {
    n->n--;
}

WC_MISC_STATIC WC_INLINE byte w64Equal(w64wrapper a, w64wrapper b) {
    return (a.n == b.n);
}

WC_MISC_STATIC WC_INLINE word32 w64GetLow32(w64wrapper n) {
    return (word32)n.n;
}

WC_MISC_STATIC WC_INLINE word32 w64GetHigh32(w64wrapper n) {
    return (word32)(n.n >> 32);
}

WC_MISC_STATIC WC_INLINE void w64SetLow32(w64wrapper *n, word32 low) {
    n->n = (n->n & (~(word64)(0xffffffff))) | low;
}

WC_MISC_STATIC WC_INLINE w64wrapper w64Add32(w64wrapper a, word32 b, byte *wrap)
{
    a.n = a.n + b;
    if (a.n < b && wrap != NULL)
        *wrap = 1;

    return a;
}

WC_MISC_STATIC WC_INLINE w64wrapper w64Sub32(w64wrapper a, word32 b, byte *wrap)
{
    if (a.n < b && wrap != NULL)
        *wrap = 1;
    a.n = a.n - b;
    return a;
}

WC_MISC_STATIC WC_INLINE byte w64GT(w64wrapper a, w64wrapper b)
{
    return a.n > b.n;
}

WC_MISC_STATIC WC_INLINE byte w64IsZero(w64wrapper a)
{
    return a.n == 0;
}

WC_MISC_STATIC WC_INLINE void c64toa(const w64wrapper *a, byte *out)
{
#ifdef BIG_ENDIAN_ORDER
    XMEMCPY(out, &a->n, sizeof(a->n));
#else
    word64 _out;
    _out = ByteReverseWord64(a->n);
    XMEMCPY(out, &_out, sizeof(_out));
#endif /* BIG_ENDIAN_ORDER */
}

WC_MISC_STATIC WC_INLINE void ato64(const byte *in, w64wrapper *w64)
{
#ifdef BIG_ENDIAN_ORDER
    XMEMCPY(&w64->n, in, sizeof(w64->n));
#else
    word64 _in;
    XMEMCPY(&_in, in, sizeof(_in));
    w64->n = ByteReverseWord64(_in);
#endif /* BIG_ENDIAN_ORDER */
}

WC_MISC_STATIC WC_INLINE w64wrapper w64From32(word32 hi, word32 lo)
{
    w64wrapper ret;
    ret.n = ((word64)hi << 32) | lo;
    return ret;
}

WC_MISC_STATIC WC_INLINE byte w64GTE(w64wrapper a, w64wrapper b)
{
    return a.n >= b.n;
}

WC_MISC_STATIC WC_INLINE byte w64LT(w64wrapper a, w64wrapper b)
{
    return a.n < b.n;
}

WC_MISC_STATIC WC_INLINE w64wrapper w64Sub(w64wrapper a, w64wrapper b)
{
    a.n -= b.n;
    return a;
}

WC_MISC_STATIC WC_INLINE void w64Zero(w64wrapper *a)
{
    a->n = 0;
}

#else
WC_MISC_STATIC WC_INLINE void w64Increment(w64wrapper *n)
{
    n->n[1]++;
    if (n->n[1] == 0)
        n->n[0]++;
}

WC_MISC_STATIC WC_INLINE void w64Decrement(w64wrapper *n) {
    if (n->n[1] == 0)
        n->n[0]--;
    n->n[1]--;
}

WC_MISC_STATIC WC_INLINE byte w64Equal(w64wrapper a, w64wrapper b)
{
    return (a.n[0] == b.n[0] && a.n[1] == b.n[1]);
}

WC_MISC_STATIC WC_INLINE word32 w64GetLow32(w64wrapper n) {
    return n.n[1];
}

WC_MISC_STATIC WC_INLINE word32 w64GetHigh32(w64wrapper n) {
    return n.n[0];
}

WC_MISC_STATIC WC_INLINE void w64SetLow32(w64wrapper *n, word32 low)
{
    n->n[1] = low;
}

WC_MISC_STATIC WC_INLINE w64wrapper w64Add32(w64wrapper a, word32 b, byte *wrap)
{
    a.n[1] = a.n[1] + b;
    if (a.n[1] < b) {
        a.n[0]++;
        if (wrap != NULL && a.n[0] == 0)
                *wrap = 1;
    }

    return a;
}

WC_MISC_STATIC WC_INLINE w64wrapper w64Sub32(w64wrapper a, word32 b, byte *wrap)
{
    byte _underflow = 0;
    if (a.n[1] < b)
        _underflow = 1;

    a.n[1] -= b;
    if (_underflow) {
        if (a.n[0] == 0 && wrap != NULL)
            *wrap = 1;
        a.n[0]--;
    }

    return a;
}

WC_MISC_STATIC WC_INLINE w64wrapper w64Sub(w64wrapper a, w64wrapper b)
{
    if (a.n[1] < b.n[1])
        a.n[0]--;
    a.n[1] -= b.n[1];
    a.n[0] -= b.n[0];
    return a;
}

WC_MISC_STATIC WC_INLINE void w64Zero(w64wrapper *a)
{
    a->n[0] = a->n[1] = 0;
}

WC_MISC_STATIC WC_INLINE byte w64GT(w64wrapper a, w64wrapper b)
{
    if (a.n[0] > b.n[0])
        return 1;
    if (a.n[0] == b.n[0])
        return a.n[1] > b.n[1];
    return 0;
}

WC_MISC_STATIC WC_INLINE byte w64GTE(w64wrapper a, w64wrapper b)
{
    if (a.n[0] > b.n[0])
        return 1;
    if (a.n[0] == b.n[0])
        return a.n[1] >= b.n[1];
    return 0;
}

WC_MISC_STATIC WC_INLINE byte w64IsZero(w64wrapper a)
{
    return a.n[0] == 0 && a.n[1] == 0;
}

WC_MISC_STATIC WC_INLINE void c64toa(w64wrapper *a, byte *out)
{
#ifdef BIG_ENDIAN_ORDER
    word32 *_out = (word32*)(out);
    _out[0] = a->n[0];
    _out[1] = a->n[1];
#else
    c32toa(a->n[0], out);
    c32toa(a->n[1], out + 4);
#endif /* BIG_ENDIAN_ORDER */
}

WC_MISC_STATIC WC_INLINE void ato64(const byte *in, w64wrapper *w64)
{
#ifdef BIG_ENDIAN_ORDER
    const word32 *_in = (const word32*)(in);
    w64->n[0] = *_in;
    w64->n[1] = *(_in + 1);
#else
    ato32(in, &w64->n[0]);
    ato32(in + 4, &w64->n[1]);
#endif /* BIG_ENDIAN_ORDER */
}

WC_MISC_STATIC WC_INLINE w64wrapper w64From32(word32 hi, word32 lo)
{
    w64wrapper w64;
    w64.n[0] = hi;
    w64.n[1] = lo;
    return w64;
}

WC_MISC_STATIC WC_INLINE byte w64LT(w64wrapper a, w64wrapper b)
{
    if (a.n[0] < b.n[0])
        return 1;
    if (a.n[0] == b.n[0])
        return a.n[1] < b.n[1];

    return 0;
}

#endif /* WORD64_AVAILABLE && !WOLFSSL_W64_WRAPPER_TEST */
#endif /* WOLFSSL_W64_WRAPPER */

#if defined(HAVE_SESSION_TICKET) || !defined(NO_CERTS) || \
    !defined(NO_SESSION_CACHE)
/* Make a word from the front of random hash */
WC_MISC_STATIC WC_INLINE word32 MakeWordFromHash(const byte* hashID)
{
    return ((word32)hashID[0] << 24) | ((word32)hashID[1] << 16) |
           ((word32)hashID[2] <<  8) |  (word32)hashID[3];
}
#endif /* HAVE_SESSION_TICKET || !NO_CERTS || !NO_SESSION_CACHE */


#if !defined(WOLFCRYPT_ONLY) && !defined(NO_HASH_WRAPPER) && \
    (!defined(NO_SESSION_CACHE) || defined(HAVE_SESSION_TICKET))

#include <wolfssl/wolfcrypt/hash.h>

/* some session IDs aren't random after all, let's make them random */
WC_MISC_STATIC WC_INLINE word32 HashObject(const byte* o, word32 len,
                                           int* error)
{
    byte digest[WC_MAX_DIGEST_SIZE];

#ifndef NO_MD5
    *error =  wc_Md5Hash(o, len, digest);
#elif !defined(NO_SHA)
    *error =  wc_ShaHash(o, len, digest);
#elif !defined(NO_SHA256)
    *error =  wc_Sha256Hash(o, len, digest);
#else
    #error "We need a digest to hash the session IDs"
#endif

    return *error == 0 ? MakeWordFromHash(digest) : 0; /* 0 on failure */
}
#endif /* WOLFCRYPT_ONLY && !NO_HASH_WRAPPER &&
        * (!NO_SESSION_CACHE || HAVE_SESSION_TICKET) */

#endif /* !WOLFSSL_MISC_INCLUDED && !NO_INLINE */

#endif /* WOLF_CRYPT_MISC_C */
