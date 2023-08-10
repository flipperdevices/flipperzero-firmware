/* armv8-curve25519
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
#endif /* HAVE_CONFIG_H */
#include <wolfssl/wolfcrypt/settings.h>

/* Generated using (from wolfssl):
 *   cd ../scripts
 *   ruby ./x25519/x25519.rb arm64 ../wolfssl/wolfcrypt/src/port/arm/armv8-curve25519.c
 */
#ifdef WOLFSSL_ARMASM
#ifdef __aarch64__
#ifdef HAVE_CURVE25519
#include <wolfssl/wolfcrypt/fe_operations.h>

void fe_init()
{
    __asm__ __volatile__ (
        "\n\t"
        : 
        :
        : "memory"
    );
}

void fe_frombytes(fe out, const unsigned char* in)
{
    __asm__ __volatile__ (
        "ldp	x2, x3, [%x[in]]\n\t"
        "ldp	x4, x5, [%x[in], #16]\n\t"
        "and	x5, x5, #0x7fffffffffffffff\n\t"
        "stp	x2, x3, [%x[out]]\n\t"
        "stp	x4, x5, [%x[out], #16]\n\t"
        : [out] "+r" (out), [in] "+r" (in)
        :
        : "memory", "x2", "x3", "x4", "x5", "x6"
    );
}

void fe_tobytes(unsigned char* out, const fe n)
{
    __asm__ __volatile__ (
        "mov	x7, #19\n\t"
        "ldp	x2, x3, [%x[n]]\n\t"
        "ldp	x4, x5, [%x[n], #16]\n\t"
        "adds	x6, x2, x7\n\t"
        "adcs	x6, x3, xzr\n\t"
        "adcs	x6, x4, xzr\n\t"
        "adc	x6, x5, xzr\n\t"
        "and	x6, x7, x6, asr 63\n\t"
        "adds	x2, x2, x6\n\t"
        "adcs	x3, x3, xzr\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adc	x5, x5, xzr\n\t"
        "and	x5, x5, #0x7fffffffffffffff\n\t"
        "stp	x2, x3, [%x[out]]\n\t"
        "stp	x4, x5, [%x[out], #16]\n\t"
        : [out] "+r" (out), [n] "+r" (n)
        :
        : "memory", "x2", "x3", "x4", "x5", "x6", "x7"
    );
}

void fe_1(fe n)
{
    __asm__ __volatile__ (
        /* Set one */
        "mov	x1, #1\n\t"
        "stp	x1, xzr, [%x[n]]\n\t"
        "stp	xzr, xzr, [%x[n], #16]\n\t"
        : [n] "+r" (n)
        :
        : "memory", "x1"
    );
}

void fe_0(fe n)
{
    __asm__ __volatile__ (
        /* Set zero */
        "stp	xzr, xzr, [%x[n]]\n\t"
        "stp	xzr, xzr, [%x[n], #16]\n\t"
        : [n] "+r" (n)
        :
        : "memory"
    );
}

void fe_copy(fe r, const fe a)
{
    __asm__ __volatile__ (
        /* Copy */
        "ldp	x2, x3, [%x[a]]\n\t"
        "ldp	x4, x5, [%x[a], #16]\n\t"
        "stp	x2, x3, [%x[r]]\n\t"
        "stp	x4, x5, [%x[r], #16]\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "x2", "x3", "x4", "x5"
    );
}

void fe_sub(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        /* Sub */
        "ldp	x3, x4, [%x[a]]\n\t"
        "ldp	x5, x6, [%x[a], #16]\n\t"
        "ldp	x7, x8, [%x[b]]\n\t"
        "ldp	x9, x10, [%x[b], #16]\n\t"
        "subs	x3, x3, x7\n\t"
        "sbcs	x4, x4, x8\n\t"
        "sbcs	x5, x5, x9\n\t"
        "sbcs	x6, x6, x10\n\t"
        "mov	x12, #-19\n\t"
        "csetm	x11, cc\n\t"
        /*   Mask the modulus */
        "and	x12, x11, x12\n\t"
        "and	x13, x11, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x3, x3, x12\n\t"
        "adcs	x4, x4, x11\n\t"
        "adcs	x5, x5, x11\n\t"
        "adc	x6, x6, x13\n\t"
        "stp	x3, x4, [%x[r]]\n\t"
        "stp	x5, x6, [%x[r], #16]\n\t"
        : [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13"
    );
}

void fe_add(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        /* Add */
        "ldp	x3, x4, [%x[a]]\n\t"
        "ldp	x5, x6, [%x[a], #16]\n\t"
        "ldp	x7, x8, [%x[b]]\n\t"
        "ldp	x9, x10, [%x[b], #16]\n\t"
        "adds	x3, x3, x7\n\t"
        "adcs	x4, x4, x8\n\t"
        "adcs	x5, x5, x9\n\t"
        "adc	x6, x6, x10\n\t"
        "mov	x12, #-19\n\t"
        "asr	x11, x6, #63\n\t"
        /*   Mask the modulus */
        "and	x12, x11, x12\n\t"
        "and	x13, x11, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x3, x3, x12\n\t"
        "sbcs	x4, x4, x11\n\t"
        "sbcs	x5, x5, x11\n\t"
        "sbc	x6, x6, x13\n\t"
        "stp	x3, x4, [%x[r]]\n\t"
        "stp	x5, x6, [%x[r], #16]\n\t"
        : [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13"
    );
}

void fe_neg(fe r, const fe a)
{
    __asm__ __volatile__ (
        "ldp	x2, x3, [%x[a]]\n\t"
        "ldp	x4, x5, [%x[a], #16]\n\t"
        "mov	x6, #-19\n\t"
        "mov	x7, #-1\n\t"
        "mov	x8, #-1\n\t"
        "mov	x9, #0x7fffffffffffffff\n\t"
        "subs	x6, x6, x2\n\t"
        "sbcs	x7, x7, x3\n\t"
        "sbcs	x8, x8, x4\n\t"
        "sbc	x9, x9, x5\n\t"
        "stp	x6, x7, [%x[r]]\n\t"
        "stp	x8, x9, [%x[r], #16]\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9"
    );
}

int fe_isnonzero(const fe a)
{
    __asm__ __volatile__ (
        "mov	x6, #19\n\t"
        "ldp	x1, x2, [%x[a]]\n\t"
        "ldp	x3, x4, [%x[a], #16]\n\t"
        "adds	x5, x1, x6\n\t"
        "adcs	x5, x2, xzr\n\t"
        "adcs	x5, x3, xzr\n\t"
        "adc	x5, x4, xzr\n\t"
        "and	x5, x6, x5, asr 63\n\t"
        "adds	x1, x1, x5\n\t"
        "adcs	x2, x2, xzr\n\t"
        "adcs	x3, x3, xzr\n\t"
        "adc	x4, x4, xzr\n\t"
        "and	x4, x4, #0x7fffffffffffffff\n\t"
        "orr	%x[a], x1, x2\n\t"
        "orr	x3, x3, x4\n\t"
        "orr	%x[a], %x[a], x3\n\t"
        : [a] "+r" (a)
        :
        : "memory", "x1", "x2", "x3", "x4", "x5", "x6"
    );
    return (uint32_t)(size_t)a;
}

int fe_isnegative(const fe a)
{
    __asm__ __volatile__ (
        "mov	x6, #19\n\t"
        "ldp	x1, x2, [%x[a]]\n\t"
        "ldp	x3, x4, [%x[a], #16]\n\t"
        "adds	x5, x1, x6\n\t"
        "adcs	x5, x2, xzr\n\t"
        "adcs	x5, x3, xzr\n\t"
        "adc	x5, x4, xzr\n\t"
        "and	%x[a], x1, #1\n\t"
        "eor	%x[a], %x[a], x5, lsr 63\n\t"
        : [a] "+r" (a)
        :
        : "memory", "x1", "x2", "x3", "x4", "x5", "x6"
    );
    return (uint32_t)(size_t)a;
}

void fe_cmov_table(fe* r, fe* base, signed char b)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-32]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[r], [x29, #16]\n\t"
        "sxtb	%x[b], %w[b]\n\t"
        "sbfx	x3, %x[b], #7, #1\n\t"
        "eor	%x[r], %x[b], x3\n\t"
        "sub	%x[r], %x[r], x3\n\t"
        "mov	x4, #1\n\t"
        "mov	x5, xzr\n\t"
        "mov	x6, xzr\n\t"
        "mov	x7, xzr\n\t"
        "mov	x8, #1\n\t"
        "mov	x9, xzr\n\t"
        "mov	x10, xzr\n\t"
        "mov	x11, xzr\n\t"
        "mov	x12, xzr\n\t"
        "mov	x13, xzr\n\t"
        "mov	x14, xzr\n\t"
        "mov	x15, xzr\n\t"
        "cmp	%x[r], #1\n\t"
        "ldp	x16, x17, [%x[base]]\n\t"
        "ldp	x19, x20, [%x[base], #16]\n\t"
        "ldp	x21, x22, [%x[base], #32]\n\t"
        "ldp	x23, x24, [%x[base], #48]\n\t"
        "ldp	x25, x26, [%x[base], #64]\n\t"
        "ldp	x27, x28, [%x[base], #80]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "cmp	%x[r], #2\n\t"
        "ldp	x16, x17, [%x[base], #96]\n\t"
        "ldp	x19, x20, [%x[base], #112]\n\t"
        "ldp	x21, x22, [%x[base], #128]\n\t"
        "ldp	x23, x24, [%x[base], #144]\n\t"
        "ldp	x25, x26, [%x[base], #160]\n\t"
        "ldp	x27, x28, [%x[base], #176]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "cmp	%x[r], #3\n\t"
        "ldp	x16, x17, [%x[base], #192]\n\t"
        "ldp	x19, x20, [%x[base], #208]\n\t"
        "ldp	x21, x22, [%x[base], #224]\n\t"
        "ldp	x23, x24, [%x[base], #240]\n\t"
        "ldp	x25, x26, [%x[base], #256]\n\t"
        "ldp	x27, x28, [%x[base], #272]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "cmp	%x[r], #4\n\t"
        "ldp	x16, x17, [%x[base], #288]\n\t"
        "ldp	x19, x20, [%x[base], #304]\n\t"
        "ldp	x21, x22, [%x[base], #320]\n\t"
        "ldp	x23, x24, [%x[base], #336]\n\t"
        "ldp	x25, x26, [%x[base], #352]\n\t"
        "ldp	x27, x28, [%x[base], #368]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "add	%x[base], %x[base], #0x180\n\t"
        "cmp	%x[r], #5\n\t"
        "ldp	x16, x17, [%x[base]]\n\t"
        "ldp	x19, x20, [%x[base], #16]\n\t"
        "ldp	x21, x22, [%x[base], #32]\n\t"
        "ldp	x23, x24, [%x[base], #48]\n\t"
        "ldp	x25, x26, [%x[base], #64]\n\t"
        "ldp	x27, x28, [%x[base], #80]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "cmp	%x[r], #6\n\t"
        "ldp	x16, x17, [%x[base], #96]\n\t"
        "ldp	x19, x20, [%x[base], #112]\n\t"
        "ldp	x21, x22, [%x[base], #128]\n\t"
        "ldp	x23, x24, [%x[base], #144]\n\t"
        "ldp	x25, x26, [%x[base], #160]\n\t"
        "ldp	x27, x28, [%x[base], #176]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "cmp	%x[r], #7\n\t"
        "ldp	x16, x17, [%x[base], #192]\n\t"
        "ldp	x19, x20, [%x[base], #208]\n\t"
        "ldp	x21, x22, [%x[base], #224]\n\t"
        "ldp	x23, x24, [%x[base], #240]\n\t"
        "ldp	x25, x26, [%x[base], #256]\n\t"
        "ldp	x27, x28, [%x[base], #272]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "cmp	%x[r], #8\n\t"
        "ldp	x16, x17, [%x[base], #288]\n\t"
        "ldp	x19, x20, [%x[base], #304]\n\t"
        "ldp	x21, x22, [%x[base], #320]\n\t"
        "ldp	x23, x24, [%x[base], #336]\n\t"
        "ldp	x25, x26, [%x[base], #352]\n\t"
        "ldp	x27, x28, [%x[base], #368]\n\t"
        "csel	x4, x16, x4, eq\n\t"
        "csel	x5, x17, x5, eq\n\t"
        "csel	x6, x19, x6, eq\n\t"
        "csel	x7, x20, x7, eq\n\t"
        "csel	x8, x21, x8, eq\n\t"
        "csel	x9, x22, x9, eq\n\t"
        "csel	x10, x23, x10, eq\n\t"
        "csel	x11, x24, x11, eq\n\t"
        "csel	x12, x25, x12, eq\n\t"
        "csel	x13, x26, x13, eq\n\t"
        "csel	x14, x27, x14, eq\n\t"
        "csel	x15, x28, x15, eq\n\t"
        "mov	x16, #-19\n\t"
        "mov	x17, #-1\n\t"
        "mov	x19, #-1\n\t"
        "mov	x20, #0x7fffffffffffffff\n\t"
        "subs	x16, x16, x12\n\t"
        "sbcs	x17, x17, x13\n\t"
        "sbcs	x19, x19, x14\n\t"
        "sbc	x20, x20, x15\n\t"
        "cmp	%x[b], #0\n\t"
        "mov	x3, x4\n\t"
        "csel	x4, x8, x4, lt\n\t"
        "csel	x8, x3, x8, lt\n\t"
        "mov	x3, x5\n\t"
        "csel	x5, x9, x5, lt\n\t"
        "csel	x9, x3, x9, lt\n\t"
        "mov	x3, x6\n\t"
        "csel	x6, x10, x6, lt\n\t"
        "csel	x10, x3, x10, lt\n\t"
        "mov	x3, x7\n\t"
        "csel	x7, x11, x7, lt\n\t"
        "csel	x11, x3, x11, lt\n\t"
        "csel	x12, x16, x12, lt\n\t"
        "csel	x13, x17, x13, lt\n\t"
        "csel	x14, x19, x14, lt\n\t"
        "csel	x15, x20, x15, lt\n\t"
        "ldr	%x[r], [x29, #16]\n\t"
        "stp	x4, x5, [%x[r]]\n\t"
        "stp	x6, x7, [%x[r], #16]\n\t"
        "stp	x8, x9, [%x[r], #32]\n\t"
        "stp	x10, x11, [%x[r], #48]\n\t"
        "stp	x12, x13, [%x[r], #64]\n\t"
        "stp	x14, x15, [%x[r], #80]\n\t"
        "ldp	x29, x30, [sp], #32\n\t"
        : [r] "+r" (r), [base] "+r" (base), [b] "+r" (b)
        :
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

void fe_mul(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        /* Multiply */
        "ldp	x14, x15, [%x[a]]\n\t"
        "ldp	x16, x17, [%x[a], #16]\n\t"
        "ldp	x19, x20, [%x[b]]\n\t"
        "ldp	x21, x22, [%x[b], #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x6, x14, x19\n\t"
        "umulh	x7, x14, x19\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x14, x20\n\t"
        "umulh	x8, x14, x20\n\t"
        "adds	x7, x7, x3\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x15, x19\n\t"
        "umulh	x4, x15, x19\n\t"
        "adds	x7, x7, x3\n\t"
        "adcs	x8, x8, x4\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x14, x21\n\t"
        "umulh	x4, x14, x21\n\t"
        "adds	x8, x8, x3\n\t"
        "adc	x9, x9, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x15, x20\n\t"
        "umulh	x4, x15, x20\n\t"
        "adds	x8, x8, x3\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x16, x19\n\t"
        "umulh	x4, x16, x19\n\t"
        "adds	x8, x8, x3\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x14, x22\n\t"
        "umulh	x4, x14, x22\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	x10, x10, x4\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x15, x21\n\t"
        "umulh	x4, x15, x21\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	x10, x10, x4\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x16, x20\n\t"
        "umulh	x4, x16, x20\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	x10, x10, x4\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x17, x19\n\t"
        "umulh	x4, x17, x19\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	x10, x10, x4\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x15, x22\n\t"
        "umulh	x4, x15, x22\n\t"
        "adds	x10, x10, x3\n\t"
        "adcs	x11, x11, x4\n\t"
        "adc	x12, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x16, x21\n\t"
        "umulh	x4, x16, x21\n\t"
        "adds	x10, x10, x3\n\t"
        "adcs	x11, x11, x4\n\t"
        "adc	x12, x12, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x17, x20\n\t"
        "umulh	x4, x17, x20\n\t"
        "adds	x10, x10, x3\n\t"
        "adcs	x11, x11, x4\n\t"
        "adc	x12, x12, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x16, x22\n\t"
        "umulh	x4, x16, x22\n\t"
        "adds	x11, x11, x3\n\t"
        "adcs	x12, x12, x4\n\t"
        "adc	x13, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x17, x21\n\t"
        "umulh	x4, x17, x21\n\t"
        "adds	x11, x11, x3\n\t"
        "adcs	x12, x12, x4\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x17, x22\n\t"
        "umulh	x4, x17, x22\n\t"
        "adds	x12, x12, x3\n\t"
        "adc	x13, x13, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x13, x13, x12, #63\n\t"
        "extr	x12, x12, x11, #63\n\t"
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, x10\n\t"
        "umulh	x10, x3, x10\n\t"
        "adds	x6, x6, x4\n\t"
        "mul	x4, x3, x11\n\t"
        "umulh	x11, x3, x11\n\t"
        "adcs	x7, x7, x4\n\t"
        "mul	x4, x3, x12\n\t"
        "umulh	x12, x3, x12\n\t"
        "adcs	x8, x8, x4\n\t"
        "mul	x4, x3, x13\n\t"
        "umulh	x5, x3, x13\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x7, x7, x10\n\t"
        "adcs	x8, x8, x11\n\t"
        "adcs	x9, x9, x12\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x9, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x9, asr 63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Store */
        "stp	x6, x7, [%x[r]]\n\t"
        "stp	x8, x9, [%x[r], #16]\n\t"
        : [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22"
    );
}

void fe_sq(fe r, const fe a)
{
    __asm__ __volatile__ (
        /* Square */
        "ldp	x13, x14, [%x[a]]\n\t"
        "ldp	x15, x16, [%x[a], #16]\n\t"
        /*  A[0] * A[1] */
        "mul	x6, x13, x14\n\t"
        "umulh	x7, x13, x14\n\t"
        /*  A[0] * A[2] */
        "mul	x2, x13, x15\n\t"
        "umulh	x8, x13, x15\n\t"
        "adds	x7, x7, x2\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x2, x13, x16\n\t"
        "umulh	x9, x13, x16\n\t"
        "adds	x8, x8, x2\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x2, x14, x15\n\t"
        "umulh	x3, x14, x15\n\t"
        "adds	x8, x8, x2\n\t"
        "adcs	x9, x9, x3\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x2, x14, x16\n\t"
        "umulh	x3, x14, x16\n\t"
        "adds	x9, x9, x2\n\t"
        "adc	x10, x10, x3\n\t"
        /*  A[2] * A[3] */
        "mul	x2, x15, x16\n\t"
        "umulh	x11, x15, x16\n\t"
        "adds	x10, x10, x2\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Double */
        "adds	x6, x6, x6\n\t"
        "adcs	x7, x7, x7\n\t"
        "adcs	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	x10, x10, x10\n\t"
        "adcs	x11, x11, x11\n\t"
        "adc	x12, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x5, x13, x13\n\t"
        "umulh	x4, x13, x13\n\t"
        /*  A[1] * A[1] */
        "mul	x2, x14, x14\n\t"
        "umulh	x3, x14, x14\n\t"
        "adds	x6, x6, x4\n\t"
        "adcs	x7, x7, x2\n\t"
        "adc	x4, x3, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x2, x15, x15\n\t"
        "umulh	x3, x15, x15\n\t"
        "adds	x8, x8, x4\n\t"
        "adcs	x9, x9, x2\n\t"
        "adc	x4, x3, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x2, x16, x16\n\t"
        "umulh	x3, x16, x16\n\t"
        "adds	x10, x10, x4\n\t"
        "adcs	x11, x11, x2\n\t"
        "adc	x12, x12, x3\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x12, x12, x11, #63\n\t"
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "and	x8, x8, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x2, #19\n\t"
        "mul	x3, x2, x9\n\t"
        "umulh	x9, x2, x9\n\t"
        "adds	x5, x5, x3\n\t"
        "mul	x3, x2, x10\n\t"
        "umulh	x10, x2, x10\n\t"
        "adcs	x6, x6, x3\n\t"
        "mul	x3, x2, x11\n\t"
        "umulh	x11, x2, x11\n\t"
        "adcs	x7, x7, x3\n\t"
        "mul	x3, x2, x12\n\t"
        "umulh	x4, x2, x12\n\t"
        "adcs	x8, x8, x3\n\t"
        "adc	x4, x4, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adcs	x8, x8, x11\n\t"
        "adc	x4, x4, xzr\n\t"
        /*  Overflow */
        "extr	x4, x4, x8, #63\n\t"
        "mul	x4, x4, x2\n\t"
        "and	x8, x8, #0x7fffffffffffffff\n\t"
        "adds	x5, x5, x4\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adc	x8, x8, xzr\n\t"
        /* Reduce if top bit set */
        "and	x4, x2, x8, asr 63\n\t"
        "and	x8, x8, #0x7fffffffffffffff\n\t"
        "adds	x5, x5, x4\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adc	x8, x8, xzr\n\t"
        /* Store */
        "stp	x5, x6, [%x[r]]\n\t"
        "stp	x7, x8, [%x[r], #16]\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16"
    );
}

void fe_invert(fe r, const fe a)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-160]!\n\t"
        "add	x29, sp, #0\n\t"
        /* Invert */
        "str	%x[r], [x29, #144]\n\t"
        "str	%x[a], [x29, #152]\n\t"
        "add	x0, x29, #16\n\t"
#ifndef NDEBUG
        "ldr	x1, [x29, #152]\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "ldr	x1, [x29, #152]\n\t"
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #16\n\t"
        "add	x1, x29, #16\n\t"
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #16\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #48\n\t"
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x20, #3\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_invert1_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert1_%=\n\t"
        "add	x0, x29, #48\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
        "add	x1, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x20, #8\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_invert2_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert2_%=\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x70\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x20, #18\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_fe_invert3_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert3_%=\n\t"
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x20, #9\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_invert4_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert4_%=\n\t"
        "add	x0, x29, #48\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
        "add	x1, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x20, #48\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_invert5_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert5_%=\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x70\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x20, #0x62\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_fe_invert6_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert6_%=\n\t"
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x20, #49\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_invert7_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert7_%=\n\t"
        "add	x0, x29, #48\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x20, #4\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "\n"
    "L_fe_invert8_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x20, x20, #1\n\t"
        "bcs	L_fe_invert8_%=\n\t"
        "ldr	x0, [x29, #144]\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "ldp	x29, x30, [sp], #0xa0\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "x2", "x20"
    );
}

int curve25519(byte* r, const byte* n, const byte* a)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-192]!\n\t"
        "add	x29, sp, #0\n\t"
        "mov	x23, xzr\n\t"
        "str	%x[r], [x29, #176]\n\t"
        "str	%x[a], [x29, #184]\n\t"
        /* Copy */
        "ldp	x6, x7, [%x[a]]\n\t"
        "ldp	x8, x9, [%x[a], #16]\n\t"
        "stp	x6, x7, [x29, #80]\n\t"
        "stp	x8, x9, [x29, #96]\n\t"
        /* Set one */
        "mov	%x[a], #1\n\t"
        "stp	%x[a], xzr, [%x[r]]\n\t"
        "stp	xzr, xzr, [%x[r], #16]\n\t"
        /* Set zero */
        "stp	xzr, xzr, [x29, #16]\n\t"
        "stp	xzr, xzr, [x29, #32]\n\t"
        /* Set one */
        "mov	%x[a], #1\n\t"
        "stp	%x[a], xzr, [x29, #48]\n\t"
        "stp	xzr, xzr, [x29, #64]\n\t"
        "mov	x25, #62\n\t"
        "mov	x24, #24\n\t"
        "\n"
    "L_curve25519_words_%=: \n\t"
        "\n"
    "L_curve25519_bits_%=: \n\t"
        "ldr	%x[a], [%x[n], x24]\n\t"
        "lsr	%x[a], %x[a], x25\n\t"
        "and	%x[a], %x[a], #1\n\t"
        "eor	x23, x23, %x[a]\n\t"
        /* Conditional Swap */
        "cmp	x23, #1\n\t"
        "ldp	x10, x11, [%x[r]]\n\t"
        "ldp	x12, x13, [%x[r], #16]\n\t"
        "ldp	x6, x7, [x29, #80]\n\t"
        "ldp	x8, x9, [x29, #96]\n\t"
        "csel	x14, x10, x6, eq\n\t"
        "csel	x10, x6, x10, eq\n\t"
        "csel	x15, x11, x7, eq\n\t"
        "csel	x11, x7, x11, eq\n\t"
        "csel	x16, x12, x8, eq\n\t"
        "csel	x12, x8, x12, eq\n\t"
        "csel	x17, x13, x9, eq\n\t"
        "csel	x13, x9, x13, eq\n\t"
        /* Conditional Swap */
        "cmp	x23, #1\n\t"
        "ldp	x19, x20, [x29, #16]\n\t"
        "ldp	x21, x22, [x29, #32]\n\t"
        "ldp	x6, x7, [x29, #48]\n\t"
        "ldp	x8, x9, [x29, #64]\n\t"
        "csel	x5, x19, x6, eq\n\t"
        "csel	x19, x6, x19, eq\n\t"
        "csel	x26, x20, x7, eq\n\t"
        "csel	x20, x7, x20, eq\n\t"
        "csel	x27, x21, x8, eq\n\t"
        "csel	x21, x8, x21, eq\n\t"
        "csel	x28, x22, x9, eq\n\t"
        "csel	x22, x9, x22, eq\n\t"
        "mov	x23, %x[a]\n\t"
        /* Add */
        "adds	x6, x10, x19\n\t"
        "adcs	x7, x11, x20\n\t"
        "adcs	x8, x12, x21\n\t"
        "adc	x9, x13, x22\n\t"
        "mov	x3, #-19\n\t"
        "asr	%x[a], x9, #63\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x6, x6, x3\n\t"
        "sbcs	x7, x7, %x[a]\n\t"
        "sbcs	x8, x8, %x[a]\n\t"
        "sbc	x9, x9, x4\n\t"
        /* Sub */
        "subs	x19, x10, x19\n\t"
        "sbcs	x20, x11, x20\n\t"
        "sbcs	x21, x12, x21\n\t"
        "sbcs	x22, x13, x22\n\t"
        "mov	x3, #-19\n\t"
        "csetm	%x[a], cc\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x19, x19, x3\n\t"
        "adcs	x20, x20, %x[a]\n\t"
        "adcs	x21, x21, %x[a]\n\t"
        "adc	x22, x22, x4\n\t"
        "stp	x19, x20, [x29, #144]\n\t"
        "stp	x21, x22, [x29, #160]\n\t"
        /* Add */
        "adds	x10, x14, x5\n\t"
        "adcs	x11, x15, x26\n\t"
        "adcs	x12, x16, x27\n\t"
        "adc	x13, x17, x28\n\t"
        "mov	x3, #-19\n\t"
        "asr	%x[a], x13, #63\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x10, x10, x3\n\t"
        "sbcs	x11, x11, %x[a]\n\t"
        "sbcs	x12, x12, %x[a]\n\t"
        "sbc	x13, x13, x4\n\t"
        /* Sub */
        "subs	x14, x14, x5\n\t"
        "sbcs	x15, x15, x26\n\t"
        "sbcs	x16, x16, x27\n\t"
        "sbcs	x17, x17, x28\n\t"
        "mov	x3, #-19\n\t"
        "csetm	%x[a], cc\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x14, x14, x3\n\t"
        "adcs	x15, x15, %x[a]\n\t"
        "adcs	x16, x16, %x[a]\n\t"
        "adc	x17, x17, x4\n\t"
        /* Multiply */
        /*  A[0] * B[0] */
        "mul	x19, x14, x6\n\t"
        "umulh	x20, x14, x6\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x14, x7\n\t"
        "umulh	x21, x14, x7\n\t"
        "adds	x20, x20, x3\n\t"
        "adc	x21, x21, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x15, x6\n\t"
        "umulh	x4, x15, x6\n\t"
        "adds	x20, x20, x3\n\t"
        "adcs	x21, x21, x4\n\t"
        "adc	x22, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x14, x8\n\t"
        "umulh	x4, x14, x8\n\t"
        "adds	x21, x21, x3\n\t"
        "adc	x22, x22, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x15, x7\n\t"
        "umulh	x4, x15, x7\n\t"
        "adds	x21, x21, x3\n\t"
        "adcs	x22, x22, x4\n\t"
        "adc	%x[a], xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x16, x6\n\t"
        "umulh	x4, x16, x6\n\t"
        "adds	x21, x21, x3\n\t"
        "adcs	x22, x22, x4\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x14, x9\n\t"
        "umulh	x4, x14, x9\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x15, x8\n\t"
        "umulh	x4, x15, x8\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x16, x7\n\t"
        "umulh	x4, x16, x7\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x17, x6\n\t"
        "umulh	x4, x17, x6\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x15, x9\n\t"
        "umulh	x4, x15, x9\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x16, x8\n\t"
        "umulh	x4, x16, x8\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x17, x7\n\t"
        "umulh	x4, x17, x7\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x16, x9\n\t"
        "umulh	x4, x16, x9\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x17, x8\n\t"
        "umulh	x4, x17, x8\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, x28, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x17, x9\n\t"
        "umulh	x4, x17, x9\n\t"
        "adds	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x22, #63\n\t"
        "and	x22, x22, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x19, x19, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x20, x20, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x21, x21, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x22, x22, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x20, x20, %x[a]\n\t"
        "adcs	x21, x21, x26\n\t"
        "adcs	x22, x22, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x22, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x22, x22, #0x7fffffffffffffff\n\t"
        "adds	x19, x19, x5\n\t"
        "adcs	x20, x20, xzr\n\t"
        "adcs	x21, x21, xzr\n\t"
        "adc	x22, x22, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x22, asr 63\n\t"
        "and	x22, x22, #0x7fffffffffffffff\n\t"
        "adds	x19, x19, x5\n\t"
        "adcs	x20, x20, xzr\n\t"
        "adcs	x21, x21, xzr\n\t"
        "adc	x22, x22, xzr\n\t"
        /* Store */
        "stp	x19, x20, [x29, #112]\n\t"
        "stp	x21, x22, [x29, #128]\n\t"
        /* Multiply */
        "ldp	%x[a], x26, [x29, #144]\n\t"
        "ldp	x27, x28, [x29, #160]\n\t"
        /*  A[0] * B[0] */
        "mul	x19, x10, %x[a]\n\t"
        "umulh	x20, x10, %x[a]\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x10, x26\n\t"
        "umulh	x21, x10, x26\n\t"
        "adds	x20, x20, x3\n\t"
        "adc	x21, x21, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x11, %x[a]\n\t"
        "umulh	x4, x11, %x[a]\n\t"
        "adds	x20, x20, x3\n\t"
        "adcs	x21, x21, x4\n\t"
        "adc	x22, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x10, x27\n\t"
        "umulh	x4, x10, x27\n\t"
        "adds	x21, x21, x3\n\t"
        "adc	x22, x22, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x11, x26\n\t"
        "umulh	x4, x11, x26\n\t"
        "adds	x21, x21, x3\n\t"
        "adcs	x22, x22, x4\n\t"
        "adc	x14, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x12, %x[a]\n\t"
        "umulh	x4, x12, %x[a]\n\t"
        "adds	x21, x21, x3\n\t"
        "adcs	x22, x22, x4\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x10, x28\n\t"
        "umulh	x4, x10, x28\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	x14, x14, x4\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x11, x27\n\t"
        "umulh	x4, x11, x27\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	x14, x14, x4\n\t"
        "adc	x15, x15, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x12, x26\n\t"
        "umulh	x4, x12, x26\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	x14, x14, x4\n\t"
        "adc	x15, x15, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x13, %x[a]\n\t"
        "umulh	x4, x13, %x[a]\n\t"
        "adds	x22, x22, x3\n\t"
        "adcs	x14, x14, x4\n\t"
        "adc	x15, x15, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x11, x28\n\t"
        "umulh	x4, x11, x28\n\t"
        "adds	x14, x14, x3\n\t"
        "adcs	x15, x15, x4\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x12, x27\n\t"
        "umulh	x4, x12, x27\n\t"
        "adds	x14, x14, x3\n\t"
        "adcs	x15, x15, x4\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x13, x26\n\t"
        "umulh	x4, x13, x26\n\t"
        "adds	x14, x14, x3\n\t"
        "adcs	x15, x15, x4\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x12, x28\n\t"
        "umulh	x4, x12, x28\n\t"
        "adds	x15, x15, x3\n\t"
        "adcs	x16, x16, x4\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x13, x27\n\t"
        "umulh	x4, x13, x27\n\t"
        "adds	x15, x15, x3\n\t"
        "adcs	x16, x16, x4\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x13, x28\n\t"
        "umulh	x4, x13, x28\n\t"
        "adds	x16, x16, x3\n\t"
        "adc	x17, x17, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x15, #63\n\t"
        "extr	x15, x15, x14, #63\n\t"
        "extr	x14, x14, x22, #63\n\t"
        "and	x22, x22, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, x14\n\t"
        "umulh	x14, x3, x14\n\t"
        "adds	x19, x19, x4\n\t"
        "mul	x4, x3, x15\n\t"
        "umulh	x15, x3, x15\n\t"
        "adcs	x20, x20, x4\n\t"
        "mul	x4, x3, x16\n\t"
        "umulh	x16, x3, x16\n\t"
        "adcs	x21, x21, x4\n\t"
        "mul	x4, x3, x17\n\t"
        "umulh	x5, x3, x17\n\t"
        "adcs	x22, x22, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x20, x20, x14\n\t"
        "adcs	x21, x21, x15\n\t"
        "adcs	x22, x22, x16\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x22, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x22, x22, #0x7fffffffffffffff\n\t"
        "adds	x19, x19, x5\n\t"
        "adcs	x20, x20, xzr\n\t"
        "adcs	x21, x21, xzr\n\t"
        "adc	x22, x22, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x22, asr 63\n\t"
        "and	x22, x22, #0x7fffffffffffffff\n\t"
        "adds	x19, x19, x5\n\t"
        "adcs	x20, x20, xzr\n\t"
        "adcs	x21, x21, xzr\n\t"
        "adc	x22, x22, xzr\n\t"
        /* Store */
        /* Square */
        /*  A[0] * A[1] */
        "mul	x11, %x[a], x26\n\t"
        "umulh	x12, %x[a], x26\n\t"
        /*  A[0] * A[2] */
        "mul	x3, %x[a], x27\n\t"
        "umulh	x13, %x[a], x27\n\t"
        "adds	x12, x12, x3\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x3, %x[a], x28\n\t"
        "umulh	x14, %x[a], x28\n\t"
        "adds	x13, x13, x3\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x3, x26, x27\n\t"
        "umulh	x4, x26, x27\n\t"
        "adds	x13, x13, x3\n\t"
        "adcs	x14, x14, x4\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x3, x26, x28\n\t"
        "umulh	x4, x26, x28\n\t"
        "adds	x14, x14, x3\n\t"
        "adc	x15, x15, x4\n\t"
        /*  A[2] * A[3] */
        "mul	x3, x27, x28\n\t"
        "umulh	x16, x27, x28\n\t"
        "adds	x15, x15, x3\n\t"
        "adc	x16, x16, xzr\n\t"
        /* Double */
        "adds	x11, x11, x11\n\t"
        "adcs	x12, x12, x12\n\t"
        "adcs	x13, x13, x13\n\t"
        "adcs	x14, x14, x14\n\t"
        "adcs	x15, x15, x15\n\t"
        "adcs	x16, x16, x16\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x10, %x[a], %x[a]\n\t"
        "umulh	x5, %x[a], %x[a]\n\t"
        /*  A[1] * A[1] */
        "mul	x3, x26, x26\n\t"
        "umulh	x4, x26, x26\n\t"
        "adds	x11, x11, x5\n\t"
        "adcs	x12, x12, x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x3, x27, x27\n\t"
        "umulh	x4, x27, x27\n\t"
        "adds	x13, x13, x5\n\t"
        "adcs	x14, x14, x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x3, x28, x28\n\t"
        "umulh	x4, x28, x28\n\t"
        "adds	x15, x15, x5\n\t"
        "adcs	x16, x16, x3\n\t"
        "adc	x17, x17, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x15, #63\n\t"
        "extr	x15, x15, x14, #63\n\t"
        "extr	x14, x14, x13, #63\n\t"
        "and	x13, x13, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, x14\n\t"
        "umulh	x14, x3, x14\n\t"
        "adds	x10, x10, x4\n\t"
        "mul	x4, x3, x15\n\t"
        "umulh	x15, x3, x15\n\t"
        "adcs	x11, x11, x4\n\t"
        "mul	x4, x3, x16\n\t"
        "umulh	x16, x3, x16\n\t"
        "adcs	x12, x12, x4\n\t"
        "mul	x4, x3, x17\n\t"
        "umulh	x5, x3, x17\n\t"
        "adcs	x13, x13, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x11, x11, x14\n\t"
        "adcs	x12, x12, x15\n\t"
        "adcs	x13, x13, x16\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x13, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x13, x13, #0x7fffffffffffffff\n\t"
        "adds	x10, x10, x5\n\t"
        "adcs	x11, x11, xzr\n\t"
        "adcs	x12, x12, xzr\n\t"
        "adc	x13, x13, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x13, asr 63\n\t"
        "and	x13, x13, #0x7fffffffffffffff\n\t"
        "adds	x10, x10, x5\n\t"
        "adcs	x11, x11, xzr\n\t"
        "adcs	x12, x12, xzr\n\t"
        "adc	x13, x13, xzr\n\t"
        /* Store */
        /* Square */
        /*  A[0] * A[1] */
        "mul	x15, x6, x7\n\t"
        "umulh	x16, x6, x7\n\t"
        /*  A[0] * A[2] */
        "mul	x3, x6, x8\n\t"
        "umulh	x17, x6, x8\n\t"
        "adds	x16, x16, x3\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x3, x6, x9\n\t"
        "umulh	%x[a], x6, x9\n\t"
        "adds	x17, x17, x3\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x3, x7, x8\n\t"
        "umulh	x4, x7, x8\n\t"
        "adds	x17, x17, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x3, x7, x9\n\t"
        "umulh	x4, x7, x9\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adc	x26, x26, x4\n\t"
        /*  A[2] * A[3] */
        "mul	x3, x8, x9\n\t"
        "umulh	x27, x8, x9\n\t"
        "adds	x26, x26, x3\n\t"
        "adc	x27, x27, xzr\n\t"
        /* Double */
        "adds	x15, x15, x15\n\t"
        "adcs	x16, x16, x16\n\t"
        "adcs	x17, x17, x17\n\t"
        "adcs	%x[a], %x[a], %x[a]\n\t"
        "adcs	x26, x26, x26\n\t"
        "adcs	x27, x27, x27\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x14, x6, x6\n\t"
        "umulh	x5, x6, x6\n\t"
        /*  A[1] * A[1] */
        "mul	x3, x7, x7\n\t"
        "umulh	x4, x7, x7\n\t"
        "adds	x15, x15, x5\n\t"
        "adcs	x16, x16, x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x3, x8, x8\n\t"
        "umulh	x4, x8, x8\n\t"
        "adds	x17, x17, x5\n\t"
        "adcs	%x[a], %x[a], x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x3, x9, x9\n\t"
        "umulh	x4, x9, x9\n\t"
        "adds	x26, x26, x5\n\t"
        "adcs	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x17, #63\n\t"
        "and	x17, x17, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x14, x14, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x15, x15, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x16, x16, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x17, x17, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x15, x15, %x[a]\n\t"
        "adcs	x16, x16, x26\n\t"
        "adcs	x17, x17, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x17, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x17, x17, #0x7fffffffffffffff\n\t"
        "adds	x14, x14, x5\n\t"
        "adcs	x15, x15, xzr\n\t"
        "adcs	x16, x16, xzr\n\t"
        "adc	x17, x17, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x17, asr 63\n\t"
        "and	x17, x17, #0x7fffffffffffffff\n\t"
        "adds	x14, x14, x5\n\t"
        "adcs	x15, x15, xzr\n\t"
        "adcs	x16, x16, xzr\n\t"
        "adc	x17, x17, xzr\n\t"
        /* Store */
        /* Multiply */
        /*  A[0] * B[0] */
        "mul	x6, x14, x10\n\t"
        "umulh	x7, x14, x10\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x14, x11\n\t"
        "umulh	x8, x14, x11\n\t"
        "adds	x7, x7, x3\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x15, x10\n\t"
        "umulh	x4, x15, x10\n\t"
        "adds	x7, x7, x3\n\t"
        "adcs	x8, x8, x4\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x14, x12\n\t"
        "umulh	x4, x14, x12\n\t"
        "adds	x8, x8, x3\n\t"
        "adc	x9, x9, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x15, x11\n\t"
        "umulh	x4, x15, x11\n\t"
        "adds	x8, x8, x3\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	%x[a], xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x16, x10\n\t"
        "umulh	x4, x16, x10\n\t"
        "adds	x8, x8, x3\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x14, x13\n\t"
        "umulh	x4, x14, x13\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x15, x12\n\t"
        "umulh	x4, x15, x12\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x16, x11\n\t"
        "umulh	x4, x16, x11\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x17, x10\n\t"
        "umulh	x4, x17, x10\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x15, x13\n\t"
        "umulh	x4, x15, x13\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x16, x12\n\t"
        "umulh	x4, x16, x12\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x17, x11\n\t"
        "umulh	x4, x17, x11\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x16, x13\n\t"
        "umulh	x4, x16, x13\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x17, x12\n\t"
        "umulh	x4, x17, x12\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, x28, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x17, x13\n\t"
        "umulh	x4, x17, x13\n\t"
        "adds	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x9, #63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x6, x6, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x7, x7, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x8, x8, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x7, x7, %x[a]\n\t"
        "adcs	x8, x8, x26\n\t"
        "adcs	x9, x9, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x9, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x9, asr 63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Store */
        "stp	x6, x7, [%x[r]]\n\t"
        "stp	x8, x9, [%x[r], #16]\n\t"
        /* Sub */
        "subs	x14, x14, x10\n\t"
        "sbcs	x15, x15, x11\n\t"
        "sbcs	x16, x16, x12\n\t"
        "sbcs	x17, x17, x13\n\t"
        "mov	x3, #-19\n\t"
        "csetm	%x[a], cc\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x14, x14, x3\n\t"
        "adcs	x15, x15, %x[a]\n\t"
        "adcs	x16, x16, %x[a]\n\t"
        "adc	x17, x17, x4\n\t"
        /* Multiply by 121666 */
        "mov	x5, #0xdb42\n\t"
        "movk	x5, #1, lsl 16\n\t"
        "mul	x6, x14, x5\n\t"
        "umulh	x7, x14, x5\n\t"
        "mul	x3, x15, x5\n\t"
        "umulh	x4, x15, x5\n\t"
        "adds	x7, x7, x3\n\t"
        "adc	x8, xzr, x4\n\t"
        "mul	x3, x16, x5\n\t"
        "umulh	x4, x16, x5\n\t"
        "adds	x8, x8, x3\n\t"
        "adc	x9, xzr, x4\n\t"
        "mul	x3, x17, x5\n\t"
        "umulh	x4, x17, x5\n\t"
        "adds	x9, x9, x3\n\t"
        "adc	x4, xzr, x4\n\t"
        "mov	x5, #19\n\t"
        "extr	x4, x4, x9, #63\n\t"
        "mul	x4, x4, x5\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x4\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Add */
        "adds	x10, x10, x6\n\t"
        "adcs	x11, x11, x7\n\t"
        "adcs	x12, x12, x8\n\t"
        "adc	x13, x13, x9\n\t"
        "mov	x3, #-19\n\t"
        "asr	%x[a], x13, #63\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x10, x10, x3\n\t"
        "sbcs	x11, x11, %x[a]\n\t"
        "sbcs	x12, x12, %x[a]\n\t"
        "sbc	x13, x13, x4\n\t"
        /* Multiply */
        /*  A[0] * B[0] */
        "mul	x6, x14, x10\n\t"
        "umulh	x7, x14, x10\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x14, x11\n\t"
        "umulh	x8, x14, x11\n\t"
        "adds	x7, x7, x3\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x15, x10\n\t"
        "umulh	x4, x15, x10\n\t"
        "adds	x7, x7, x3\n\t"
        "adcs	x8, x8, x4\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x14, x12\n\t"
        "umulh	x4, x14, x12\n\t"
        "adds	x8, x8, x3\n\t"
        "adc	x9, x9, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x15, x11\n\t"
        "umulh	x4, x15, x11\n\t"
        "adds	x8, x8, x3\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	%x[a], xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x16, x10\n\t"
        "umulh	x4, x16, x10\n\t"
        "adds	x8, x8, x3\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x14, x13\n\t"
        "umulh	x4, x14, x13\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x15, x12\n\t"
        "umulh	x4, x15, x12\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x16, x11\n\t"
        "umulh	x4, x16, x11\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x17, x10\n\t"
        "umulh	x4, x17, x10\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x15, x13\n\t"
        "umulh	x4, x15, x13\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x16, x12\n\t"
        "umulh	x4, x16, x12\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x17, x11\n\t"
        "umulh	x4, x17, x11\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x16, x13\n\t"
        "umulh	x4, x16, x13\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x17, x12\n\t"
        "umulh	x4, x17, x12\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, x28, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x17, x13\n\t"
        "umulh	x4, x17, x13\n\t"
        "adds	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x9, #63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x6, x6, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x7, x7, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x8, x8, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x7, x7, %x[a]\n\t"
        "adcs	x8, x8, x26\n\t"
        "adcs	x9, x9, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x9, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x9, asr 63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Store */
        "stp	x6, x7, [x29, #16]\n\t"
        "stp	x8, x9, [x29, #32]\n\t"
        /* Add */
        "ldp	x6, x7, [x29, #112]\n\t"
        "ldp	x8, x9, [x29, #128]\n\t"
        "adds	x10, x6, x19\n\t"
        "adcs	x11, x7, x20\n\t"
        "adcs	x12, x8, x21\n\t"
        "adc	x13, x9, x22\n\t"
        "mov	x3, #-19\n\t"
        "asr	%x[a], x13, #63\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x10, x10, x3\n\t"
        "sbcs	x11, x11, %x[a]\n\t"
        "sbcs	x12, x12, %x[a]\n\t"
        "sbc	x13, x13, x4\n\t"
        /* Sub */
        "subs	x19, x6, x19\n\t"
        "sbcs	x20, x7, x20\n\t"
        "sbcs	x21, x8, x21\n\t"
        "sbcs	x22, x9, x22\n\t"
        "mov	x3, #-19\n\t"
        "csetm	%x[a], cc\n\t"
        /*   Mask the modulus */
        "and	x3, %x[a], x3\n\t"
        "and	x4, %x[a], #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x19, x19, x3\n\t"
        "adcs	x20, x20, %x[a]\n\t"
        "adcs	x21, x21, %x[a]\n\t"
        "adc	x22, x22, x4\n\t"
        /* Square */
        /*  A[0] * A[1] */
        "mul	x7, x10, x11\n\t"
        "umulh	x8, x10, x11\n\t"
        /*  A[0] * A[2] */
        "mul	x3, x10, x12\n\t"
        "umulh	x9, x10, x12\n\t"
        "adds	x8, x8, x3\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x3, x10, x13\n\t"
        "umulh	%x[a], x10, x13\n\t"
        "adds	x9, x9, x3\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x3, x11, x12\n\t"
        "umulh	x4, x11, x12\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x3, x11, x13\n\t"
        "umulh	x4, x11, x13\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adc	x26, x26, x4\n\t"
        /*  A[2] * A[3] */
        "mul	x3, x12, x13\n\t"
        "umulh	x27, x12, x13\n\t"
        "adds	x26, x26, x3\n\t"
        "adc	x27, x27, xzr\n\t"
        /* Double */
        "adds	x7, x7, x7\n\t"
        "adcs	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	%x[a], %x[a], %x[a]\n\t"
        "adcs	x26, x26, x26\n\t"
        "adcs	x27, x27, x27\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x6, x10, x10\n\t"
        "umulh	x5, x10, x10\n\t"
        /*  A[1] * A[1] */
        "mul	x3, x11, x11\n\t"
        "umulh	x4, x11, x11\n\t"
        "adds	x7, x7, x5\n\t"
        "adcs	x8, x8, x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x3, x12, x12\n\t"
        "umulh	x4, x12, x12\n\t"
        "adds	x9, x9, x5\n\t"
        "adcs	%x[a], %x[a], x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x3, x13, x13\n\t"
        "umulh	x4, x13, x13\n\t"
        "adds	x26, x26, x5\n\t"
        "adcs	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x9, #63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x6, x6, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x7, x7, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x8, x8, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x7, x7, %x[a]\n\t"
        "adcs	x8, x8, x26\n\t"
        "adcs	x9, x9, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x9, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x9, asr 63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Store */
        "stp	x6, x7, [x29, #80]\n\t"
        "stp	x8, x9, [x29, #96]\n\t"
        /* Square */
        /*  A[0] * A[1] */
        "mul	x7, x19, x20\n\t"
        "umulh	x8, x19, x20\n\t"
        /*  A[0] * A[2] */
        "mul	x3, x19, x21\n\t"
        "umulh	x9, x19, x21\n\t"
        "adds	x8, x8, x3\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x3, x19, x22\n\t"
        "umulh	%x[a], x19, x22\n\t"
        "adds	x9, x9, x3\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x3, x20, x21\n\t"
        "umulh	x4, x20, x21\n\t"
        "adds	x9, x9, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x3, x20, x22\n\t"
        "umulh	x4, x20, x22\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adc	x26, x26, x4\n\t"
        /*  A[2] * A[3] */
        "mul	x3, x21, x22\n\t"
        "umulh	x27, x21, x22\n\t"
        "adds	x26, x26, x3\n\t"
        "adc	x27, x27, xzr\n\t"
        /* Double */
        "adds	x7, x7, x7\n\t"
        "adcs	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	%x[a], %x[a], %x[a]\n\t"
        "adcs	x26, x26, x26\n\t"
        "adcs	x27, x27, x27\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x6, x19, x19\n\t"
        "umulh	x5, x19, x19\n\t"
        /*  A[1] * A[1] */
        "mul	x3, x20, x20\n\t"
        "umulh	x4, x20, x20\n\t"
        "adds	x7, x7, x5\n\t"
        "adcs	x8, x8, x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x3, x21, x21\n\t"
        "umulh	x4, x21, x21\n\t"
        "adds	x9, x9, x5\n\t"
        "adcs	%x[a], %x[a], x3\n\t"
        "adc	x5, x4, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x3, x22, x22\n\t"
        "umulh	x4, x22, x22\n\t"
        "adds	x26, x26, x5\n\t"
        "adcs	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x9, #63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x6, x6, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x7, x7, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x8, x8, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x9, x9, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x7, x7, %x[a]\n\t"
        "adcs	x8, x8, x26\n\t"
        "adcs	x9, x9, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x9, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x9, asr 63\n\t"
        "and	x9, x9, #0x7fffffffffffffff\n\t"
        "adds	x6, x6, x5\n\t"
        "adcs	x7, x7, xzr\n\t"
        "adcs	x8, x8, xzr\n\t"
        "adc	x9, x9, xzr\n\t"
        /* Store */
        "ldr	%x[a], [x29, #184]\n\t"
        /* Multiply */
        "ldp	x14, x15, [%x[a]]\n\t"
        "ldp	x16, x17, [%x[a], #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x10, x14, x6\n\t"
        "umulh	x11, x14, x6\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x14, x7\n\t"
        "umulh	x12, x14, x7\n\t"
        "adds	x11, x11, x3\n\t"
        "adc	x12, x12, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x15, x6\n\t"
        "umulh	x4, x15, x6\n\t"
        "adds	x11, x11, x3\n\t"
        "adcs	x12, x12, x4\n\t"
        "adc	x13, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x14, x8\n\t"
        "umulh	x4, x14, x8\n\t"
        "adds	x12, x12, x3\n\t"
        "adc	x13, x13, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x15, x7\n\t"
        "umulh	x4, x15, x7\n\t"
        "adds	x12, x12, x3\n\t"
        "adcs	x13, x13, x4\n\t"
        "adc	%x[a], xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x16, x6\n\t"
        "umulh	x4, x16, x6\n\t"
        "adds	x12, x12, x3\n\t"
        "adcs	x13, x13, x4\n\t"
        "adc	%x[a], %x[a], xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x14, x9\n\t"
        "umulh	x4, x14, x9\n\t"
        "adds	x13, x13, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x15, x8\n\t"
        "umulh	x4, x15, x8\n\t"
        "adds	x13, x13, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x16, x7\n\t"
        "umulh	x4, x16, x7\n\t"
        "adds	x13, x13, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x17, x6\n\t"
        "umulh	x4, x17, x6\n\t"
        "adds	x13, x13, x3\n\t"
        "adcs	%x[a], %x[a], x4\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x15, x9\n\t"
        "umulh	x4, x15, x9\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x16, x8\n\t"
        "umulh	x4, x16, x8\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x17, x7\n\t"
        "umulh	x4, x17, x7\n\t"
        "adds	%x[a], %x[a], x3\n\t"
        "adcs	x26, x26, x4\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x16, x9\n\t"
        "umulh	x4, x16, x9\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x17, x8\n\t"
        "umulh	x4, x17, x8\n\t"
        "adds	x26, x26, x3\n\t"
        "adcs	x27, x27, x4\n\t"
        "adc	x28, x28, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x17, x9\n\t"
        "umulh	x4, x17, x9\n\t"
        "adds	x27, x27, x3\n\t"
        "adc	x28, x28, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x28, x28, x27, #63\n\t"
        "extr	x27, x27, x26, #63\n\t"
        "extr	x26, x26, %x[a], #63\n\t"
        "extr	%x[a], %x[a], x13, #63\n\t"
        "and	x13, x13, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, %x[a]\n\t"
        "umulh	%x[a], x3, %x[a]\n\t"
        "adds	x10, x10, x4\n\t"
        "mul	x4, x3, x26\n\t"
        "umulh	x26, x3, x26\n\t"
        "adcs	x11, x11, x4\n\t"
        "mul	x4, x3, x27\n\t"
        "umulh	x27, x3, x27\n\t"
        "adcs	x12, x12, x4\n\t"
        "mul	x4, x3, x28\n\t"
        "umulh	x5, x3, x28\n\t"
        "adcs	x13, x13, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x11, x11, %x[a]\n\t"
        "adcs	x12, x12, x26\n\t"
        "adcs	x13, x13, x27\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x13, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x13, x13, #0x7fffffffffffffff\n\t"
        "adds	x10, x10, x5\n\t"
        "adcs	x11, x11, xzr\n\t"
        "adcs	x12, x12, xzr\n\t"
        "adc	x13, x13, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x13, asr 63\n\t"
        "and	x13, x13, #0x7fffffffffffffff\n\t"
        "adds	x10, x10, x5\n\t"
        "adcs	x11, x11, xzr\n\t"
        "adcs	x12, x12, xzr\n\t"
        "adc	x13, x13, xzr\n\t"
        /* Store */
        "stp	x10, x11, [x29, #48]\n\t"
        "stp	x12, x13, [x29, #64]\n\t"
        "sub	x25, x25, #1\n\t"
        "cmp	x25, #0\n\t"
        "bge	L_curve25519_bits_%=\n\t"
        "mov	x25, #63\n\t"
        "sub	x24, x24, #8\n\t"
        "cmp	x24, #0\n\t"
        "bge	L_curve25519_words_%=\n\t"
        /* Invert */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
        "add	x1, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #16\n\t"
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #48\n\t"
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x70\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
        "add	x1, x29, #0x50\n\t"
        "add	x2, x29, #0x70\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x70\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x24, #3\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_curve25519_inv_1_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_1_%=\n\t"
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x70\n\t"
        "add	x1, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x24, #8\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_curve25519_inv_2_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_2_%=\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x90\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x24, #18\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x90\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x90\n\t"
        "\n"
    "L_curve25519_inv_3_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_3_%=\n\t"
        "add	x0, x29, #0x70\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x90\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x70\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x24, #9\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_curve25519_inv_4_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_4_%=\n\t"
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x70\n\t"
        "add	x1, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x24, #48\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_curve25519_inv_5_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_5_%=\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x90\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x24, #0x62\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x90\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x90\n\t"
        "\n"
    "L_curve25519_inv_6_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_6_%=\n\t"
        "add	x0, x29, #0x70\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x90\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x70\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x24, #49\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x70\n\t"
        "\n"
    "L_curve25519_inv_7_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_7_%=\n\t"
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x70\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #0x50\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x24, #4\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_curve25519_inv_8_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x24, x24, #1\n\t"
        "bcs	L_curve25519_inv_8_%=\n\t"
        "add	x0, x29, #16\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "ldr	%x[r], [x29, #176]\n\t"
        /* Multiply */
        "ldp	x6, x7, [%x[r]]\n\t"
        "ldp	x8, x9, [%x[r], #16]\n\t"
        "ldp	x10, x11, [x29, #16]\n\t"
        "ldp	x12, x13, [x29, #32]\n\t"
        /*  A[0] * B[0] */
        "mul	x14, x6, x10\n\t"
        "umulh	x15, x6, x10\n\t"
        /*  A[0] * B[1] */
        "mul	x3, x6, x11\n\t"
        "umulh	x16, x6, x11\n\t"
        "adds	x15, x15, x3\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x3, x7, x10\n\t"
        "umulh	x4, x7, x10\n\t"
        "adds	x15, x15, x3\n\t"
        "adcs	x16, x16, x4\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x3, x6, x12\n\t"
        "umulh	x4, x6, x12\n\t"
        "adds	x16, x16, x3\n\t"
        "adc	x17, x17, x4\n\t"
        /*  A[1] * B[1] */
        "mul	x3, x7, x11\n\t"
        "umulh	x4, x7, x11\n\t"
        "adds	x16, x16, x3\n\t"
        "adcs	x17, x17, x4\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x3, x8, x10\n\t"
        "umulh	x4, x8, x10\n\t"
        "adds	x16, x16, x3\n\t"
        "adcs	x17, x17, x4\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x3, x6, x13\n\t"
        "umulh	x4, x6, x13\n\t"
        "adds	x17, x17, x3\n\t"
        "adcs	x19, x19, x4\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x3, x7, x12\n\t"
        "umulh	x4, x7, x12\n\t"
        "adds	x17, x17, x3\n\t"
        "adcs	x19, x19, x4\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x3, x8, x11\n\t"
        "umulh	x4, x8, x11\n\t"
        "adds	x17, x17, x3\n\t"
        "adcs	x19, x19, x4\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x3, x9, x10\n\t"
        "umulh	x4, x9, x10\n\t"
        "adds	x17, x17, x3\n\t"
        "adcs	x19, x19, x4\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x3, x7, x13\n\t"
        "umulh	x4, x7, x13\n\t"
        "adds	x19, x19, x3\n\t"
        "adcs	x20, x20, x4\n\t"
        "adc	x21, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x3, x8, x12\n\t"
        "umulh	x4, x8, x12\n\t"
        "adds	x19, x19, x3\n\t"
        "adcs	x20, x20, x4\n\t"
        "adc	x21, x21, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x3, x9, x11\n\t"
        "umulh	x4, x9, x11\n\t"
        "adds	x19, x19, x3\n\t"
        "adcs	x20, x20, x4\n\t"
        "adc	x21, x21, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x3, x8, x13\n\t"
        "umulh	x4, x8, x13\n\t"
        "adds	x20, x20, x3\n\t"
        "adcs	x21, x21, x4\n\t"
        "adc	x22, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x3, x9, x12\n\t"
        "umulh	x4, x9, x12\n\t"
        "adds	x20, x20, x3\n\t"
        "adcs	x21, x21, x4\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x3, x9, x13\n\t"
        "umulh	x4, x9, x13\n\t"
        "adds	x21, x21, x3\n\t"
        "adc	x22, x22, x4\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x22, x22, x21, #63\n\t"
        "extr	x21, x21, x20, #63\n\t"
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "and	x17, x17, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x3, #19\n\t"
        "mul	x4, x3, x19\n\t"
        "umulh	x19, x3, x19\n\t"
        "adds	x14, x14, x4\n\t"
        "mul	x4, x3, x20\n\t"
        "umulh	x20, x3, x20\n\t"
        "adcs	x15, x15, x4\n\t"
        "mul	x4, x3, x21\n\t"
        "umulh	x21, x3, x21\n\t"
        "adcs	x16, x16, x4\n\t"
        "mul	x4, x3, x22\n\t"
        "umulh	x5, x3, x22\n\t"
        "adcs	x17, x17, x4\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x15, x15, x19\n\t"
        "adcs	x16, x16, x20\n\t"
        "adcs	x17, x17, x21\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  Overflow */
        "extr	x5, x5, x17, #63\n\t"
        "mul	x5, x5, x3\n\t"
        "and	x17, x17, #0x7fffffffffffffff\n\t"
        "adds	x14, x14, x5\n\t"
        "adcs	x15, x15, xzr\n\t"
        "adcs	x16, x16, xzr\n\t"
        "adc	x17, x17, xzr\n\t"
        /* Reduce if top bit set */
        "and	x5, x3, x17, asr 63\n\t"
        "and	x17, x17, #0x7fffffffffffffff\n\t"
        "adds	x14, x14, x5\n\t"
        "adcs	x15, x15, xzr\n\t"
        "adcs	x16, x16, xzr\n\t"
        "adc	x17, x17, xzr\n\t"
        "adds	x4, x14, x3\n\t"
        "adcs	x4, x15, xzr\n\t"
        "adcs	x4, x16, xzr\n\t"
        "adc	x4, x17, xzr\n\t"
        "and	x4, x3, x4, asr 63\n\t"
        "adds	x14, x14, x4\n\t"
        "adcs	x15, x15, xzr\n\t"
        "mov	x4, #0x7fffffffffffffff\n\t"
        "adcs	x16, x16, xzr\n\t"
        "adc	x17, x17, xzr\n\t"
        "and	x17, x17, x4\n\t"
        /* Store */
        "stp	x14, x15, [%x[r]]\n\t"
        "stp	x16, x17, [%x[r], #16]\n\t"
        "mov	x0, xzr\n\t"
        "ldp	x29, x30, [sp], #0xc0\n\t"
        : [r] "+r" (r), [n] "+r" (n), [a] "+r" (a)
        :
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
    return (uint32_t)(size_t)r;
}

void fe_pow22523(fe r, const fe a)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-128]!\n\t"
        "add	x29, sp, #0\n\t"
        /* pow22523 */
        "str	%x[r], [x29, #112]\n\t"
        "str	%x[a], [x29, #120]\n\t"
        "add	x0, x29, #16\n\t"
#ifndef NDEBUG
        "ldr	x1, [x29, #120]\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "ldr	x1, [x29, #120]\n\t"
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #16\n\t"
        "add	x1, x29, #16\n\t"
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #16\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #16\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
#ifndef NDEBUG
        "add	x0, x29, #16\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x23, #3\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "\n"
    "L_fe_pow22523_1_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_1_%=\n\t"
        "add	x0, x29, #16\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x23, #8\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "\n"
    "L_fe_pow22523_2_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_2_%=\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x23, #18\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_pow22523_3_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_3_%=\n\t"
        "add	x0, x29, #48\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x23, #9\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "\n"
    "L_fe_pow22523_4_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_4_%=\n\t"
        "add	x0, x29, #16\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #48\n\t"
        "add	x1, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x23, #48\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "\n"
    "L_fe_pow22523_5_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_5_%=\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "add	x0, x29, #0x50\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "mov	x23, #0x62\n\t"
#ifndef NDEBUG
        "add	x0, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #0x50\n\t"
        "\n"
    "L_fe_pow22523_6_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_6_%=\n\t"
        "add	x0, x29, #48\n\t"
#ifndef NDEBUG
        "add	x1, x29, #0x50\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #48\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x23, #49\n\t"
#ifndef NDEBUG
        "add	x0, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #48\n\t"
        "\n"
    "L_fe_pow22523_7_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_7_%=\n\t"
        "add	x0, x29, #16\n\t"
#ifndef NDEBUG
        "add	x1, x29, #48\n\t"
#endif /* !NDEBUG */
        "add	x2, x29, #16\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "mov	x23, #1\n\t"
#ifndef NDEBUG
        "add	x0, x29, #16\n\t"
#endif /* !NDEBUG */
        "add	x1, x29, #16\n\t"
        "\n"
    "L_fe_pow22523_8_%=: \n\t"
#ifndef __APPLE__
        "bl	fe_sq\n\t"
#else
        "bl	_fe_sq\n\t"
#endif /* __APPLE__ */
        "subs	x23, x23, #1\n\t"
        "bcs	L_fe_pow22523_8_%=\n\t"
        "ldr	x0, [x29, #112]\n\t"
#ifndef NDEBUG
        "add	x1, x29, #16\n\t"
#endif /* !NDEBUG */
        "ldr	x2, [x29, #120]\n\t"
#ifndef __APPLE__
        "bl	fe_mul\n\t"
#else
        "bl	_fe_mul\n\t"
#endif /* __APPLE__ */
        "ldp	x29, x30, [sp], #0x80\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "x2", "x23"
    );
}

void fe_ge_to_p2(fe rx, fe ry, fe rz, const fe px, const fe py, const fe pz, const fe pt)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-64]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[ry], [x29, #16]\n\t"
        "str	%x[rz], [x29, #24]\n\t"
        "str	%x[px], [x29, #32]\n\t"
        "str	%x[py], [x29, #40]\n\t"
        "str	%x[pz], [x29, #48]\n\t"
        "str	%x[pt], [x29, #56]\n\t"
        "ldr	x1, [x29, #32]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        /* Multiply */
        "ldp	x11, x12, [x1]\n\t"
        "ldp	x13, x14, [x1, #16]\n\t"
        "ldp	x15, x16, [x2]\n\t"
        "ldp	x17, x19, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x3, x11, x15\n\t"
        "umulh	x4, x11, x15\n\t"
        /*  A[0] * B[1] */
        "mul	x20, x11, x16\n\t"
        "umulh	x5, x11, x16\n\t"
        "adds	x4, x4, x20\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x20, x12, x15\n\t"
        "umulh	x21, x12, x15\n\t"
        "adds	x4, x4, x20\n\t"
        "adcs	x5, x5, x21\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x20, x11, x17\n\t"
        "umulh	x21, x11, x17\n\t"
        "adds	x5, x5, x20\n\t"
        "adc	x6, x6, x21\n\t"
        /*  A[1] * B[1] */
        "mul	x20, x12, x16\n\t"
        "umulh	x21, x12, x16\n\t"
        "adds	x5, x5, x20\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x20, x13, x15\n\t"
        "umulh	x21, x13, x15\n\t"
        "adds	x5, x5, x20\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x20, x11, x19\n\t"
        "umulh	x21, x11, x19\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x20, x12, x17\n\t"
        "umulh	x21, x12, x17\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x20, x13, x16\n\t"
        "umulh	x21, x13, x16\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x20, x14, x15\n\t"
        "umulh	x21, x14, x15\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x20, x12, x19\n\t"
        "umulh	x21, x12, x19\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x20, x13, x17\n\t"
        "umulh	x21, x13, x17\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x20, x14, x16\n\t"
        "umulh	x21, x14, x16\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x20, x13, x19\n\t"
        "umulh	x21, x13, x19\n\t"
        "adds	x8, x8, x20\n\t"
        "adcs	x9, x9, x21\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x20, x14, x17\n\t"
        "umulh	x21, x14, x17\n\t"
        "adds	x8, x8, x20\n\t"
        "adcs	x9, x9, x21\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x20, x14, x19\n\t"
        "umulh	x21, x14, x19\n\t"
        "adds	x9, x9, x20\n\t"
        "adc	x10, x10, x21\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x20, #19\n\t"
        "mul	x21, x20, x7\n\t"
        "umulh	x7, x20, x7\n\t"
        "adds	x3, x3, x21\n\t"
        "mul	x21, x20, x8\n\t"
        "umulh	x8, x20, x8\n\t"
        "adcs	x4, x4, x21\n\t"
        "mul	x21, x20, x9\n\t"
        "umulh	x9, x20, x9\n\t"
        "adcs	x5, x5, x21\n\t"
        "mul	x21, x20, x10\n\t"
        "umulh	x22, x20, x10\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  Overflow */
        "extr	x22, x22, x6, #63\n\t"
        "mul	x22, x22, x20\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x22\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x22, x20, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x22\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldr	x0, [x29, #16]\n\t"
        "ldr	x1, [x29, #40]\n\t"
        "ldr	x2, [x29, #48]\n\t"
        /* Multiply */
        "ldp	x11, x12, [x1]\n\t"
        "ldp	x13, x14, [x1, #16]\n\t"
        "ldp	x15, x16, [x2]\n\t"
        "ldp	x17, x19, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x3, x11, x15\n\t"
        "umulh	x4, x11, x15\n\t"
        /*  A[0] * B[1] */
        "mul	x20, x11, x16\n\t"
        "umulh	x5, x11, x16\n\t"
        "adds	x4, x4, x20\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x20, x12, x15\n\t"
        "umulh	x21, x12, x15\n\t"
        "adds	x4, x4, x20\n\t"
        "adcs	x5, x5, x21\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x20, x11, x17\n\t"
        "umulh	x21, x11, x17\n\t"
        "adds	x5, x5, x20\n\t"
        "adc	x6, x6, x21\n\t"
        /*  A[1] * B[1] */
        "mul	x20, x12, x16\n\t"
        "umulh	x21, x12, x16\n\t"
        "adds	x5, x5, x20\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x20, x13, x15\n\t"
        "umulh	x21, x13, x15\n\t"
        "adds	x5, x5, x20\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x20, x11, x19\n\t"
        "umulh	x21, x11, x19\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x20, x12, x17\n\t"
        "umulh	x21, x12, x17\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x20, x13, x16\n\t"
        "umulh	x21, x13, x16\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x20, x14, x15\n\t"
        "umulh	x21, x14, x15\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x20, x12, x19\n\t"
        "umulh	x21, x12, x19\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x20, x13, x17\n\t"
        "umulh	x21, x13, x17\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x20, x14, x16\n\t"
        "umulh	x21, x14, x16\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x20, x13, x19\n\t"
        "umulh	x21, x13, x19\n\t"
        "adds	x8, x8, x20\n\t"
        "adcs	x9, x9, x21\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x20, x14, x17\n\t"
        "umulh	x21, x14, x17\n\t"
        "adds	x8, x8, x20\n\t"
        "adcs	x9, x9, x21\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x20, x14, x19\n\t"
        "umulh	x21, x14, x19\n\t"
        "adds	x9, x9, x20\n\t"
        "adc	x10, x10, x21\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x20, #19\n\t"
        "mul	x21, x20, x7\n\t"
        "umulh	x7, x20, x7\n\t"
        "adds	x3, x3, x21\n\t"
        "mul	x21, x20, x8\n\t"
        "umulh	x8, x20, x8\n\t"
        "adcs	x4, x4, x21\n\t"
        "mul	x21, x20, x9\n\t"
        "umulh	x9, x20, x9\n\t"
        "adcs	x5, x5, x21\n\t"
        "mul	x21, x20, x10\n\t"
        "umulh	x22, x20, x10\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  Overflow */
        "extr	x22, x22, x6, #63\n\t"
        "mul	x22, x22, x20\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x22\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x22, x20, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x22\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        /* Multiply */
        "ldp	x11, x12, [x2]\n\t"
        "ldp	x13, x14, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x3, x15, x11\n\t"
        "umulh	x4, x15, x11\n\t"
        /*  A[0] * B[1] */
        "mul	x20, x15, x12\n\t"
        "umulh	x5, x15, x12\n\t"
        "adds	x4, x4, x20\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x20, x16, x11\n\t"
        "umulh	x21, x16, x11\n\t"
        "adds	x4, x4, x20\n\t"
        "adcs	x5, x5, x21\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x20, x15, x13\n\t"
        "umulh	x21, x15, x13\n\t"
        "adds	x5, x5, x20\n\t"
        "adc	x6, x6, x21\n\t"
        /*  A[1] * B[1] */
        "mul	x20, x16, x12\n\t"
        "umulh	x21, x16, x12\n\t"
        "adds	x5, x5, x20\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x20, x17, x11\n\t"
        "umulh	x21, x17, x11\n\t"
        "adds	x5, x5, x20\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x20, x15, x14\n\t"
        "umulh	x21, x15, x14\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x20, x16, x13\n\t"
        "umulh	x21, x16, x13\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x20, x17, x12\n\t"
        "umulh	x21, x17, x12\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x20, x19, x11\n\t"
        "umulh	x21, x19, x11\n\t"
        "adds	x6, x6, x20\n\t"
        "adcs	x7, x7, x21\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x20, x16, x14\n\t"
        "umulh	x21, x16, x14\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x20, x17, x13\n\t"
        "umulh	x21, x17, x13\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x20, x19, x12\n\t"
        "umulh	x21, x19, x12\n\t"
        "adds	x7, x7, x20\n\t"
        "adcs	x8, x8, x21\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x20, x17, x14\n\t"
        "umulh	x21, x17, x14\n\t"
        "adds	x8, x8, x20\n\t"
        "adcs	x9, x9, x21\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x20, x19, x13\n\t"
        "umulh	x21, x19, x13\n\t"
        "adds	x8, x8, x20\n\t"
        "adcs	x9, x9, x21\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x20, x19, x14\n\t"
        "umulh	x21, x19, x14\n\t"
        "adds	x9, x9, x20\n\t"
        "adc	x10, x10, x21\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x20, #19\n\t"
        "mul	x21, x20, x7\n\t"
        "umulh	x7, x20, x7\n\t"
        "adds	x3, x3, x21\n\t"
        "mul	x21, x20, x8\n\t"
        "umulh	x8, x20, x8\n\t"
        "adcs	x4, x4, x21\n\t"
        "mul	x21, x20, x9\n\t"
        "umulh	x9, x20, x9\n\t"
        "adcs	x5, x5, x21\n\t"
        "mul	x21, x20, x10\n\t"
        "umulh	x22, x20, x10\n\t"
        "adcs	x6, x6, x21\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x22, x22, xzr\n\t"
        /*  Overflow */
        "extr	x22, x22, x6, #63\n\t"
        "mul	x22, x22, x20\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x22\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x22, x20, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x22\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldp	x29, x30, [sp], #0x40\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz), [pt] "+r" (pt)
        :
        : "memory", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22"
    );
}

void fe_ge_to_p3(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py, const fe pz, const fe pt)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-96]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[ry], [x29, #16]\n\t"
        "str	%x[rz], [x29, #24]\n\t"
        "str	%x[rt], [x29, #32]\n\t"
        "str	%x[px], [x29, #40]\n\t"
        "str	%x[py], [x29, #48]\n\t"
        "str	%x[pz], [x29, #56]\n\t"
        "str	%x[pt], [x29, #64]\n\t"
        "ldr	x1, [x29, #40]\n\t"
        "ldr	x2, [x29, #64]\n\t"
        /* Multiply */
        "ldp	x11, x12, [x1]\n\t"
        "ldp	x13, x14, [x1, #16]\n\t"
        "ldp	x15, x16, [x2]\n\t"
        "ldp	x17, x19, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x3, x11, x15\n\t"
        "umulh	x4, x11, x15\n\t"
        /*  A[0] * B[1] */
        "mul	x24, x11, x16\n\t"
        "umulh	x5, x11, x16\n\t"
        "adds	x4, x4, x24\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x24, x12, x15\n\t"
        "umulh	x25, x12, x15\n\t"
        "adds	x4, x4, x24\n\t"
        "adcs	x5, x5, x25\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x24, x11, x17\n\t"
        "umulh	x25, x11, x17\n\t"
        "adds	x5, x5, x24\n\t"
        "adc	x6, x6, x25\n\t"
        /*  A[1] * B[1] */
        "mul	x24, x12, x16\n\t"
        "umulh	x25, x12, x16\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x24, x13, x15\n\t"
        "umulh	x25, x13, x15\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x24, x11, x19\n\t"
        "umulh	x25, x11, x19\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x24, x12, x17\n\t"
        "umulh	x25, x12, x17\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x24, x13, x16\n\t"
        "umulh	x25, x13, x16\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x24, x14, x15\n\t"
        "umulh	x25, x14, x15\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x24, x12, x19\n\t"
        "umulh	x25, x12, x19\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x24, x13, x17\n\t"
        "umulh	x25, x13, x17\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x24, x14, x16\n\t"
        "umulh	x25, x14, x16\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x24, x13, x19\n\t"
        "umulh	x25, x13, x19\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x24, x14, x17\n\t"
        "umulh	x25, x14, x17\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x24, x14, x19\n\t"
        "umulh	x25, x14, x19\n\t"
        "adds	x9, x9, x24\n\t"
        "adc	x10, x10, x25\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x24, #19\n\t"
        "mul	x25, x24, x7\n\t"
        "umulh	x7, x24, x7\n\t"
        "adds	x3, x3, x25\n\t"
        "mul	x25, x24, x8\n\t"
        "umulh	x8, x24, x8\n\t"
        "adcs	x4, x4, x25\n\t"
        "mul	x25, x24, x9\n\t"
        "umulh	x9, x24, x9\n\t"
        "adcs	x5, x5, x25\n\t"
        "mul	x25, x24, x10\n\t"
        "umulh	x26, x24, x10\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Overflow */
        "extr	x26, x26, x6, #63\n\t"
        "mul	x26, x26, x24\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x26, x24, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x2, [x29, #48]\n\t"
        /* Multiply */
        "ldp	x20, x21, [x2]\n\t"
        "ldp	x22, x23, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x3, x11, x20\n\t"
        "umulh	x4, x11, x20\n\t"
        /*  A[0] * B[1] */
        "mul	x24, x11, x21\n\t"
        "umulh	x5, x11, x21\n\t"
        "adds	x4, x4, x24\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x24, x12, x20\n\t"
        "umulh	x25, x12, x20\n\t"
        "adds	x4, x4, x24\n\t"
        "adcs	x5, x5, x25\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x24, x11, x22\n\t"
        "umulh	x25, x11, x22\n\t"
        "adds	x5, x5, x24\n\t"
        "adc	x6, x6, x25\n\t"
        /*  A[1] * B[1] */
        "mul	x24, x12, x21\n\t"
        "umulh	x25, x12, x21\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x24, x13, x20\n\t"
        "umulh	x25, x13, x20\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x24, x11, x23\n\t"
        "umulh	x25, x11, x23\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x24, x12, x22\n\t"
        "umulh	x25, x12, x22\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x24, x13, x21\n\t"
        "umulh	x25, x13, x21\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x24, x14, x20\n\t"
        "umulh	x25, x14, x20\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x24, x12, x23\n\t"
        "umulh	x25, x12, x23\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x24, x13, x22\n\t"
        "umulh	x25, x13, x22\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x24, x14, x21\n\t"
        "umulh	x25, x14, x21\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x24, x13, x23\n\t"
        "umulh	x25, x13, x23\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x24, x14, x22\n\t"
        "umulh	x25, x14, x22\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x24, x14, x23\n\t"
        "umulh	x25, x14, x23\n\t"
        "adds	x9, x9, x24\n\t"
        "adc	x10, x10, x25\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x24, #19\n\t"
        "mul	x25, x24, x7\n\t"
        "umulh	x7, x24, x7\n\t"
        "adds	x3, x3, x25\n\t"
        "mul	x25, x24, x8\n\t"
        "umulh	x8, x24, x8\n\t"
        "adcs	x4, x4, x25\n\t"
        "mul	x25, x24, x9\n\t"
        "umulh	x9, x24, x9\n\t"
        "adcs	x5, x5, x25\n\t"
        "mul	x25, x24, x10\n\t"
        "umulh	x26, x24, x10\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Overflow */
        "extr	x26, x26, x6, #63\n\t"
        "mul	x26, x26, x24\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x26, x24, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldr	x0, [x29, #16]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        /* Multiply */
        "ldp	x11, x12, [x2]\n\t"
        "ldp	x13, x14, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x3, x20, x11\n\t"
        "umulh	x4, x20, x11\n\t"
        /*  A[0] * B[1] */
        "mul	x24, x20, x12\n\t"
        "umulh	x5, x20, x12\n\t"
        "adds	x4, x4, x24\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x24, x21, x11\n\t"
        "umulh	x25, x21, x11\n\t"
        "adds	x4, x4, x24\n\t"
        "adcs	x5, x5, x25\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x24, x20, x13\n\t"
        "umulh	x25, x20, x13\n\t"
        "adds	x5, x5, x24\n\t"
        "adc	x6, x6, x25\n\t"
        /*  A[1] * B[1] */
        "mul	x24, x21, x12\n\t"
        "umulh	x25, x21, x12\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x24, x22, x11\n\t"
        "umulh	x25, x22, x11\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x24, x20, x14\n\t"
        "umulh	x25, x20, x14\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x24, x21, x13\n\t"
        "umulh	x25, x21, x13\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x24, x22, x12\n\t"
        "umulh	x25, x22, x12\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x24, x23, x11\n\t"
        "umulh	x25, x23, x11\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x24, x21, x14\n\t"
        "umulh	x25, x21, x14\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x24, x22, x13\n\t"
        "umulh	x25, x22, x13\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x24, x23, x12\n\t"
        "umulh	x25, x23, x12\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x24, x22, x14\n\t"
        "umulh	x25, x22, x14\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x24, x23, x13\n\t"
        "umulh	x25, x23, x13\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x24, x23, x14\n\t"
        "umulh	x25, x23, x14\n\t"
        "adds	x9, x9, x24\n\t"
        "adc	x10, x10, x25\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x24, #19\n\t"
        "mul	x25, x24, x7\n\t"
        "umulh	x7, x24, x7\n\t"
        "adds	x3, x3, x25\n\t"
        "mul	x25, x24, x8\n\t"
        "umulh	x8, x24, x8\n\t"
        "adcs	x4, x4, x25\n\t"
        "mul	x25, x24, x9\n\t"
        "umulh	x9, x24, x9\n\t"
        "adcs	x5, x5, x25\n\t"
        "mul	x25, x24, x10\n\t"
        "umulh	x26, x24, x10\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Overflow */
        "extr	x26, x26, x6, #63\n\t"
        "mul	x26, x26, x24\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x26, x24, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldr	x0, [x29, #24]\n\t"
        /* Multiply */
        /*  A[0] * B[0] */
        "mul	x3, x11, x15\n\t"
        "umulh	x4, x11, x15\n\t"
        /*  A[0] * B[1] */
        "mul	x24, x11, x16\n\t"
        "umulh	x5, x11, x16\n\t"
        "adds	x4, x4, x24\n\t"
        "adc	x5, x5, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x24, x12, x15\n\t"
        "umulh	x25, x12, x15\n\t"
        "adds	x4, x4, x24\n\t"
        "adcs	x5, x5, x25\n\t"
        "adc	x6, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x24, x11, x17\n\t"
        "umulh	x25, x11, x17\n\t"
        "adds	x5, x5, x24\n\t"
        "adc	x6, x6, x25\n\t"
        /*  A[1] * B[1] */
        "mul	x24, x12, x16\n\t"
        "umulh	x25, x12, x16\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x24, x13, x15\n\t"
        "umulh	x25, x13, x15\n\t"
        "adds	x5, x5, x24\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x24, x11, x19\n\t"
        "umulh	x25, x11, x19\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x24, x12, x17\n\t"
        "umulh	x25, x12, x17\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x24, x13, x16\n\t"
        "umulh	x25, x13, x16\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x24, x14, x15\n\t"
        "umulh	x25, x14, x15\n\t"
        "adds	x6, x6, x24\n\t"
        "adcs	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x24, x12, x19\n\t"
        "umulh	x25, x12, x19\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x24, x13, x17\n\t"
        "umulh	x25, x13, x17\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x24, x14, x16\n\t"
        "umulh	x25, x14, x16\n\t"
        "adds	x7, x7, x24\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x24, x13, x19\n\t"
        "umulh	x25, x13, x19\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x24, x14, x17\n\t"
        "umulh	x25, x14, x17\n\t"
        "adds	x8, x8, x24\n\t"
        "adcs	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x24, x14, x19\n\t"
        "umulh	x25, x14, x19\n\t"
        "adds	x9, x9, x24\n\t"
        "adc	x10, x10, x25\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x24, #19\n\t"
        "mul	x25, x24, x7\n\t"
        "umulh	x7, x24, x7\n\t"
        "adds	x3, x3, x25\n\t"
        "mul	x25, x24, x8\n\t"
        "umulh	x8, x24, x8\n\t"
        "adcs	x4, x4, x25\n\t"
        "mul	x25, x24, x9\n\t"
        "umulh	x9, x24, x9\n\t"
        "adcs	x5, x5, x25\n\t"
        "mul	x25, x24, x10\n\t"
        "umulh	x26, x24, x10\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x7\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adc	x26, x26, xzr\n\t"
        /*  Overflow */
        "extr	x26, x26, x6, #63\n\t"
        "mul	x26, x26, x24\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Reduce if top bit set */
        "and	x26, x24, x6, asr 63\n\t"
        "and	x6, x6, #0x7fffffffffffffff\n\t"
        "adds	x3, x3, x26\n\t"
        "adcs	x4, x4, xzr\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adc	x6, x6, xzr\n\t"
        /* Store */
        "stp	x3, x4, [x0]\n\t"
        "stp	x5, x6, [x0, #16]\n\t"
        "ldp	x29, x30, [sp], #0x60\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz), [pt] "+r" (pt)
        :
        : "memory", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26"
    );
}

void fe_ge_dbl(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py, const fe pz)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-80]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[rx], [x29, #16]\n\t"
        "str	%x[ry], [x29, #24]\n\t"
        "str	%x[rz], [x29, #32]\n\t"
        "str	%x[rt], [x29, #40]\n\t"
        "str	%x[px], [x29, #48]\n\t"
        "str	%x[py], [x29, #56]\n\t"
        "str	%x[pz], [x29, #64]\n\t"
        "ldr	x1, [x29, #48]\n\t"
        /* Square */
        "ldp	x12, x13, [x1]\n\t"
        "ldp	x14, x15, [x1, #16]\n\t"
        /*  A[0] * A[1] */
        "mul	x5, x12, x13\n\t"
        "umulh	x6, x12, x13\n\t"
        /*  A[0] * A[2] */
        "mul	x25, x12, x14\n\t"
        "umulh	x7, x12, x14\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x25, x12, x15\n\t"
        "umulh	x8, x12, x15\n\t"
        "adds	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x25, x13, x14\n\t"
        "umulh	x26, x13, x14\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x25, x13, x15\n\t"
        "umulh	x26, x13, x15\n\t"
        "adds	x8, x8, x25\n\t"
        "adc	x9, x9, x26\n\t"
        /*  A[2] * A[3] */
        "mul	x25, x14, x15\n\t"
        "umulh	x10, x14, x15\n\t"
        "adds	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /* Double */
        "adds	x5, x5, x5\n\t"
        "adcs	x6, x6, x6\n\t"
        "adcs	x7, x7, x7\n\t"
        "adcs	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	x10, x10, x10\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x4, x12, x12\n\t"
        "umulh	x27, x12, x12\n\t"
        /*  A[1] * A[1] */
        "mul	x25, x13, x13\n\t"
        "umulh	x26, x13, x13\n\t"
        "adds	x5, x5, x27\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x27, x26, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x25, x14, x14\n\t"
        "umulh	x26, x14, x14\n\t"
        "adds	x7, x7, x27\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x27, x26, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x25, x15, x15\n\t"
        "umulh	x26, x15, x15\n\t"
        "adds	x9, x9, x27\n\t"
        "adcs	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x8\n\t"
        "umulh	x8, x25, x8\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x9\n\t"
        "umulh	x9, x25, x9\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x10\n\t"
        "umulh	x10, x25, x10\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x11\n\t"
        "umulh	x27, x25, x11\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "stp	x4, x5, [x0]\n\t"
        "stp	x6, x7, [x0, #16]\n\t"
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x1, [x29, #56]\n\t"
        /* Square */
        "ldp	x21, x22, [x1]\n\t"
        "ldp	x23, x24, [x1, #16]\n\t"
        /*  A[0] * A[1] */
        "mul	x9, x21, x22\n\t"
        "umulh	x10, x21, x22\n\t"
        /*  A[0] * A[2] */
        "mul	x25, x21, x23\n\t"
        "umulh	x11, x21, x23\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x25, x21, x24\n\t"
        "umulh	x16, x21, x24\n\t"
        "adds	x11, x11, x25\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x25, x22, x23\n\t"
        "umulh	x26, x22, x23\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x25, x22, x24\n\t"
        "umulh	x26, x22, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adc	x17, x17, x26\n\t"
        /*  A[2] * A[3] */
        "mul	x25, x23, x24\n\t"
        "umulh	x19, x23, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adc	x19, x19, xzr\n\t"
        /* Double */
        "adds	x9, x9, x9\n\t"
        "adcs	x10, x10, x10\n\t"
        "adcs	x11, x11, x11\n\t"
        "adcs	x16, x16, x16\n\t"
        "adcs	x17, x17, x17\n\t"
        "adcs	x19, x19, x19\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x8, x21, x21\n\t"
        "umulh	x27, x21, x21\n\t"
        /*  A[1] * A[1] */
        "mul	x25, x22, x22\n\t"
        "umulh	x26, x22, x22\n\t"
        "adds	x9, x9, x27\n\t"
        "adcs	x10, x10, x25\n\t"
        "adc	x27, x26, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x25, x23, x23\n\t"
        "umulh	x26, x23, x23\n\t"
        "adds	x11, x11, x27\n\t"
        "adcs	x16, x16, x25\n\t"
        "adc	x27, x26, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x25, x24, x24\n\t"
        "umulh	x26, x24, x24\n\t"
        "adds	x17, x17, x27\n\t"
        "adcs	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x11, #63\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x8, x8, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x9, x9, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x10, x10, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x9, x9, x16\n\t"
        "adcs	x10, x10, x17\n\t"
        "adcs	x11, x11, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x11, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        "adds	x8, x8, x27\n\t"
        "adcs	x9, x9, xzr\n\t"
        "adcs	x10, x10, xzr\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x11, asr 63\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        "adds	x8, x8, x27\n\t"
        "adcs	x9, x9, xzr\n\t"
        "adcs	x10, x10, xzr\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Store */
        "stp	x8, x9, [x0]\n\t"
        "stp	x10, x11, [x0, #16]\n\t"
        "ldr	x0, [x29, #24]\n\t"
        /* Add */
        "adds	x12, x12, x21\n\t"
        "adcs	x13, x13, x22\n\t"
        "adcs	x14, x14, x23\n\t"
        "adc	x15, x15, x24\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x15, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x12, x12, x25\n\t"
        "sbcs	x13, x13, x28\n\t"
        "sbcs	x14, x14, x28\n\t"
        "sbc	x15, x15, x26\n\t"
        "ldr	x0, [x29, #40]\n\t"
        /* Square */
        /*  A[0] * A[1] */
        "mul	x17, x12, x13\n\t"
        "umulh	x19, x12, x13\n\t"
        /*  A[0] * A[2] */
        "mul	x25, x12, x14\n\t"
        "umulh	x20, x12, x14\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x25, x12, x15\n\t"
        "umulh	x21, x12, x15\n\t"
        "adds	x20, x20, x25\n\t"
        "adc	x21, x21, xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x25, x13, x14\n\t"
        "umulh	x26, x13, x14\n\t"
        "adds	x20, x20, x25\n\t"
        "adcs	x21, x21, x26\n\t"
        "adc	x22, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x25, x13, x15\n\t"
        "umulh	x26, x13, x15\n\t"
        "adds	x21, x21, x25\n\t"
        "adc	x22, x22, x26\n\t"
        /*  A[2] * A[3] */
        "mul	x25, x14, x15\n\t"
        "umulh	x23, x14, x15\n\t"
        "adds	x22, x22, x25\n\t"
        "adc	x23, x23, xzr\n\t"
        /* Double */
        "adds	x17, x17, x17\n\t"
        "adcs	x19, x19, x19\n\t"
        "adcs	x20, x20, x20\n\t"
        "adcs	x21, x21, x21\n\t"
        "adcs	x22, x22, x22\n\t"
        "adcs	x23, x23, x23\n\t"
        "adc	x24, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x16, x12, x12\n\t"
        "umulh	x27, x12, x12\n\t"
        /*  A[1] * A[1] */
        "mul	x25, x13, x13\n\t"
        "umulh	x26, x13, x13\n\t"
        "adds	x17, x17, x27\n\t"
        "adcs	x19, x19, x25\n\t"
        "adc	x27, x26, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x25, x14, x14\n\t"
        "umulh	x26, x14, x14\n\t"
        "adds	x20, x20, x27\n\t"
        "adcs	x21, x21, x25\n\t"
        "adc	x27, x26, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x25, x15, x15\n\t"
        "umulh	x26, x15, x15\n\t"
        "adds	x22, x22, x27\n\t"
        "adcs	x23, x23, x25\n\t"
        "adc	x24, x24, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x24, x24, x23, #63\n\t"
        "extr	x23, x23, x22, #63\n\t"
        "extr	x22, x22, x21, #63\n\t"
        "extr	x21, x21, x20, #63\n\t"
        "and	x20, x20, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x21\n\t"
        "umulh	x21, x25, x21\n\t"
        "adds	x16, x16, x26\n\t"
        "mul	x26, x25, x22\n\t"
        "umulh	x22, x25, x22\n\t"
        "adcs	x17, x17, x26\n\t"
        "mul	x26, x25, x23\n\t"
        "umulh	x23, x25, x23\n\t"
        "adcs	x19, x19, x26\n\t"
        "mul	x26, x25, x24\n\t"
        "umulh	x27, x25, x24\n\t"
        "adcs	x20, x20, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x17, x17, x21\n\t"
        "adcs	x19, x19, x22\n\t"
        "adcs	x20, x20, x23\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x20, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x20, x20, #0x7fffffffffffffff\n\t"
        "adds	x16, x16, x27\n\t"
        "adcs	x17, x17, xzr\n\t"
        "adcs	x19, x19, xzr\n\t"
        "adc	x20, x20, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x20, asr 63\n\t"
        "and	x20, x20, #0x7fffffffffffffff\n\t"
        "adds	x16, x16, x27\n\t"
        "adcs	x17, x17, xzr\n\t"
        "adcs	x19, x19, xzr\n\t"
        "adc	x20, x20, xzr\n\t"
        /* Store */
        "stp	x16, x17, [x0]\n\t"
        "stp	x19, x20, [x0, #16]\n\t"
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [x29, #32]\n\t"
        /* Add */
        "adds	x12, x8, x4\n\t"
        "adcs	x13, x9, x5\n\t"
        "adcs	x14, x10, x6\n\t"
        "adc	x15, x11, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x15, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x12, x12, x25\n\t"
        "sbcs	x13, x13, x28\n\t"
        "sbcs	x14, x14, x28\n\t"
        "sbc	x15, x15, x26\n\t"
        /* Sub */
        "subs	x21, x8, x4\n\t"
        "sbcs	x22, x9, x5\n\t"
        "sbcs	x23, x10, x6\n\t"
        "sbcs	x24, x11, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x21, x21, x25\n\t"
        "adcs	x22, x22, x28\n\t"
        "adcs	x23, x23, x28\n\t"
        "adc	x24, x24, x26\n\t"
        "stp	x12, x13, [x0]\n\t"
        "stp	x14, x15, [x0, #16]\n\t"
        "stp	x21, x22, [x1]\n\t"
        "stp	x23, x24, [x1, #16]\n\t"
        "ldr	x0, [x29, #16]\n\t"
        /* Sub */
        "subs	x16, x16, x12\n\t"
        "sbcs	x17, x17, x13\n\t"
        "sbcs	x19, x19, x14\n\t"
        "sbcs	x20, x20, x15\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x16, x17, [x0]\n\t"
        "stp	x19, x20, [x0, #16]\n\t"
        "ldr	x0, [x29, #40]\n\t"
        "ldr	x1, [x29, #64]\n\t"
        /* Square * 2 */
        "ldp	x12, x13, [x1]\n\t"
        "ldp	x14, x15, [x1, #16]\n\t"
        /*  A[0] * A[1] */
        "mul	x5, x12, x13\n\t"
        "umulh	x6, x12, x13\n\t"
        /*  A[0] * A[2] */
        "mul	x25, x12, x14\n\t"
        "umulh	x7, x12, x14\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, xzr\n\t"
        /*  A[0] * A[3] */
        "mul	x25, x12, x15\n\t"
        "umulh	x8, x12, x15\n\t"
        "adds	x7, x7, x25\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[1] * A[2] */
        "mul	x25, x13, x14\n\t"
        "umulh	x26, x13, x14\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[1] * A[3] */
        "mul	x25, x13, x15\n\t"
        "umulh	x26, x13, x15\n\t"
        "adds	x8, x8, x25\n\t"
        "adc	x9, x9, x26\n\t"
        /*  A[2] * A[3] */
        "mul	x25, x14, x15\n\t"
        "umulh	x10, x14, x15\n\t"
        "adds	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /* Double */
        "adds	x5, x5, x5\n\t"
        "adcs	x6, x6, x6\n\t"
        "adcs	x7, x7, x7\n\t"
        "adcs	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	x10, x10, x10\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[0] * A[0] */
        "mul	x4, x12, x12\n\t"
        "umulh	x28, x12, x12\n\t"
        /*  A[1] * A[1] */
        "mul	x25, x13, x13\n\t"
        "umulh	x26, x13, x13\n\t"
        "adds	x5, x5, x28\n\t"
        "adcs	x6, x6, x25\n\t"
        "adc	x28, x26, xzr\n\t"
        /*  A[2] * A[2] */
        "mul	x25, x14, x14\n\t"
        "umulh	x26, x14, x14\n\t"
        "adds	x7, x7, x28\n\t"
        "adcs	x8, x8, x25\n\t"
        "adc	x28, x26, xzr\n\t"
        /*  A[3] * A[3] */
        "mul	x25, x15, x15\n\t"
        "umulh	x26, x15, x15\n\t"
        "adds	x9, x9, x28\n\t"
        "adcs	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /* Double and Reduce */
        "mov	x25, #0x169\n\t"
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "lsr	x28, x11, #61\n\t"
        "extr	x11, x11, x10, #62\n\t"
        "extr	x10, x10, x9, #62\n\t"
        "extr	x9, x9, x8, #62\n\t"
        "extr	x8, x8, x7, #62\n\t"
        "extr	x7, x7, x6, #63\n\t"
        "extr	x6, x6, x5, #63\n\t"
        "extr	x5, x5, x4, #63\n\t"
        "lsl	x4, x4, #1\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Two left, only one right */
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        /*  Multiply top bits by 19*19 */
        "mul	x28, x28, x25\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x8\n\t"
        "umulh	x8, x25, x8\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x9\n\t"
        "umulh	x9, x25, x9\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x10\n\t"
        "umulh	x10, x25, x10\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x11\n\t"
        "umulh	x27, x25, x11\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x4, x4, x28\n\t"
        "adcs	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #40]\n\t"
        /* Sub */
        "subs	x4, x4, x21\n\t"
        "sbcs	x5, x5, x22\n\t"
        "sbcs	x6, x6, x23\n\t"
        "sbcs	x7, x7, x24\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x4, x4, x25\n\t"
        "adcs	x5, x5, x28\n\t"
        "adcs	x6, x6, x28\n\t"
        "adc	x7, x7, x26\n\t"
        "stp	x4, x5, [x0]\n\t"
        "stp	x6, x7, [x0, #16]\n\t"
        "ldp	x29, x30, [sp], #0x50\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz)
        :
        : "memory", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

void fe_ge_madd(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py, const fe pz, const fe pt, const fe qxy2d, const fe qyplusx, const fe qyminusx)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-112]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[qyminusx], [sp, #104]\n\t"
        "str	%x[qyplusx], [sp, #96]\n\t"
        "str	%x[qxy2d], [sp, #88]\n\t"
        "str	%x[rx], [x29, #16]\n\t"
        "str	%x[ry], [x29, #24]\n\t"
        "str	%x[rz], [x29, #32]\n\t"
        "str	%x[rt], [x29, #40]\n\t"
        "str	%x[px], [x29, #48]\n\t"
        "str	%x[py], [x29, #56]\n\t"
        "str	%x[pz], [x29, #64]\n\t"
        "str	%x[pt], [x29, #72]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        "ldr	x3, [x29, #48]\n\t"
        /* Add */
        "ldp	x12, x13, [x2]\n\t"
        "ldp	x14, x15, [x2, #16]\n\t"
        "ldp	x16, x17, [x3]\n\t"
        "ldp	x19, x20, [x3, #16]\n\t"
        "adds	x4, x12, x16\n\t"
        "adcs	x5, x13, x17\n\t"
        "adcs	x6, x14, x19\n\t"
        "adc	x7, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x7, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x4, x4, x25\n\t"
        "sbcs	x5, x5, x28\n\t"
        "sbcs	x6, x6, x28\n\t"
        "sbc	x7, x7, x26\n\t"
        /* Sub */
        "subs	x8, x12, x16\n\t"
        "sbcs	x9, x13, x17\n\t"
        "sbcs	x10, x14, x19\n\t"
        "sbcs	x11, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x28\n\t"
        "adcs	x10, x10, x28\n\t"
        "adc	x11, x11, x26\n\t"
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x2, [sp, #96]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x2]\n\t"
        "ldp	x23, x24, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x12, x4, x21\n\t"
        "umulh	x13, x4, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x4, x22\n\t"
        "umulh	x14, x4, x22\n\t"
        "adds	x13, x13, x25\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x5, x21\n\t"
        "umulh	x26, x5, x21\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x4, x23\n\t"
        "umulh	x26, x4, x23\n\t"
        "adds	x14, x14, x25\n\t"
        "adc	x15, x15, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x5, x22\n\t"
        "umulh	x26, x5, x22\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x6, x21\n\t"
        "umulh	x26, x6, x21\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x4, x24\n\t"
        "umulh	x26, x4, x24\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x5, x23\n\t"
        "umulh	x26, x5, x23\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x6, x22\n\t"
        "umulh	x26, x6, x22\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x7, x21\n\t"
        "umulh	x26, x7, x21\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x5, x24\n\t"
        "umulh	x26, x5, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x6, x23\n\t"
        "umulh	x26, x6, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x7, x22\n\t"
        "umulh	x26, x7, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x6, x24\n\t"
        "umulh	x26, x6, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x7, x23\n\t"
        "umulh	x26, x7, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x7, x24\n\t"
        "umulh	x26, x7, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x15, #63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x12, x12, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x13, x13, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x14, x14, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x13, x13, x16\n\t"
        "adcs	x14, x14, x17\n\t"
        "adcs	x15, x15, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x15, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x15, asr 63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [sp, #104]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x1]\n\t"
        "ldp	x23, x24, [x1, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x8, x21\n\t"
        "umulh	x5, x8, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x8, x22\n\t"
        "umulh	x6, x8, x22\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x9, x21\n\t"
        "umulh	x26, x9, x21\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x8, x23\n\t"
        "umulh	x26, x8, x23\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x9, x22\n\t"
        "umulh	x26, x9, x22\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x10, x21\n\t"
        "umulh	x26, x10, x21\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x8, x24\n\t"
        "umulh	x26, x8, x24\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x9, x23\n\t"
        "umulh	x26, x9, x23\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x10, x22\n\t"
        "umulh	x26, x10, x22\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x11, x21\n\t"
        "umulh	x26, x11, x21\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x9, x24\n\t"
        "umulh	x26, x9, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x10, x23\n\t"
        "umulh	x26, x10, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x11, x22\n\t"
        "umulh	x26, x11, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x10, x24\n\t"
        "umulh	x26, x10, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x11, x23\n\t"
        "umulh	x26, x11, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x11, x24\n\t"
        "umulh	x26, x11, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x16\n\t"
        "adcs	x6, x6, x17\n\t"
        "adcs	x7, x7, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [x29, #16]\n\t"
        /* Add */
        "adds	x8, x12, x4\n\t"
        "adcs	x9, x13, x5\n\t"
        "adcs	x10, x14, x6\n\t"
        "adc	x11, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x11, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x8, x8, x25\n\t"
        "sbcs	x9, x9, x28\n\t"
        "sbcs	x10, x10, x28\n\t"
        "sbc	x11, x11, x26\n\t"
        /* Sub */
        "subs	x16, x12, x4\n\t"
        "sbcs	x17, x13, x5\n\t"
        "sbcs	x19, x14, x6\n\t"
        "sbcs	x20, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x8, x9, [x0]\n\t"
        "stp	x10, x11, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldr	x0, [x29, #40]\n\t"
        "ldr	x1, [sp, #88]\n\t"
        "ldr	x3, [x29, #72]\n\t"
        /* Multiply */
        "ldp	x16, x17, [x1]\n\t"
        "ldp	x19, x20, [x1, #16]\n\t"
        "ldp	x21, x22, [x3]\n\t"
        "ldp	x23, x24, [x3, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x16, x21\n\t"
        "umulh	x5, x16, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x16, x22\n\t"
        "umulh	x6, x16, x22\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x17, x21\n\t"
        "umulh	x26, x17, x21\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x16, x23\n\t"
        "umulh	x26, x16, x23\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x17, x22\n\t"
        "umulh	x26, x17, x22\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x19, x21\n\t"
        "umulh	x26, x19, x21\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x16, x24\n\t"
        "umulh	x26, x16, x24\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x17, x23\n\t"
        "umulh	x26, x17, x23\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x19, x22\n\t"
        "umulh	x26, x19, x22\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x20, x21\n\t"
        "umulh	x26, x20, x21\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x17, x24\n\t"
        "umulh	x26, x17, x24\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x19, x23\n\t"
        "umulh	x26, x19, x23\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x20, x22\n\t"
        "umulh	x26, x20, x22\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x19, x24\n\t"
        "umulh	x26, x19, x24\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x20, x23\n\t"
        "umulh	x26, x20, x23\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x20, x24\n\t"
        "umulh	x26, x20, x24\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x8\n\t"
        "umulh	x8, x25, x8\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x9\n\t"
        "umulh	x9, x25, x9\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x10\n\t"
        "umulh	x10, x25, x10\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x11\n\t"
        "umulh	x27, x25, x11\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x1, [x29, #64]\n\t"
        /* Double */
        "ldp	x8, x9, [x1]\n\t"
        "ldp	x10, x11, [x1, #16]\n\t"
        "adds	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	x10, x10, x10\n\t"
        "adc	x11, x11, x11\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x11, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x8, x8, x25\n\t"
        "sbcs	x9, x9, x28\n\t"
        "sbcs	x10, x10, x28\n\t"
        "sbc	x11, x11, x26\n\t"
        "ldr	x1, [x29, #40]\n\t"
        /* Add */
        "adds	x12, x8, x4\n\t"
        "adcs	x13, x9, x5\n\t"
        "adcs	x14, x10, x6\n\t"
        "adc	x15, x11, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x15, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x12, x12, x25\n\t"
        "sbcs	x13, x13, x28\n\t"
        "sbcs	x14, x14, x28\n\t"
        "sbc	x15, x15, x26\n\t"
        /* Sub */
        "subs	x16, x8, x4\n\t"
        "sbcs	x17, x9, x5\n\t"
        "sbcs	x19, x10, x6\n\t"
        "sbcs	x20, x11, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x12, x13, [x0]\n\t"
        "stp	x14, x15, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldp	x29, x30, [sp], #0x70\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz), [pt] "+r" (pt), [qxy2d] "+r" (qxy2d), [qyplusx] "+r" (qyplusx), [qyminusx] "+r" (qyminusx)
        :
        : "memory", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

void fe_ge_msub(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py, const fe pz, const fe pt, const fe qxy2d, const fe qyplusx, const fe qyminusx)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-112]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[qyminusx], [sp, #104]\n\t"
        "str	%x[qyplusx], [sp, #96]\n\t"
        "str	%x[qxy2d], [sp, #88]\n\t"
        "str	%x[rx], [x29, #16]\n\t"
        "str	%x[ry], [x29, #24]\n\t"
        "str	%x[rz], [x29, #32]\n\t"
        "str	%x[rt], [x29, #40]\n\t"
        "str	%x[px], [x29, #48]\n\t"
        "str	%x[py], [x29, #56]\n\t"
        "str	%x[pz], [x29, #64]\n\t"
        "str	%x[pt], [x29, #72]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        "ldr	x3, [x29, #48]\n\t"
        /* Add */
        "ldp	x12, x13, [x2]\n\t"
        "ldp	x14, x15, [x2, #16]\n\t"
        "ldp	x16, x17, [x3]\n\t"
        "ldp	x19, x20, [x3, #16]\n\t"
        "adds	x4, x12, x16\n\t"
        "adcs	x5, x13, x17\n\t"
        "adcs	x6, x14, x19\n\t"
        "adc	x7, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x7, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x4, x4, x25\n\t"
        "sbcs	x5, x5, x28\n\t"
        "sbcs	x6, x6, x28\n\t"
        "sbc	x7, x7, x26\n\t"
        /* Sub */
        "subs	x8, x12, x16\n\t"
        "sbcs	x9, x13, x17\n\t"
        "sbcs	x10, x14, x19\n\t"
        "sbcs	x11, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x28\n\t"
        "adcs	x10, x10, x28\n\t"
        "adc	x11, x11, x26\n\t"
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x2, [sp, #104]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x2]\n\t"
        "ldp	x23, x24, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x12, x4, x21\n\t"
        "umulh	x13, x4, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x4, x22\n\t"
        "umulh	x14, x4, x22\n\t"
        "adds	x13, x13, x25\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x5, x21\n\t"
        "umulh	x26, x5, x21\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x4, x23\n\t"
        "umulh	x26, x4, x23\n\t"
        "adds	x14, x14, x25\n\t"
        "adc	x15, x15, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x5, x22\n\t"
        "umulh	x26, x5, x22\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x6, x21\n\t"
        "umulh	x26, x6, x21\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x4, x24\n\t"
        "umulh	x26, x4, x24\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x5, x23\n\t"
        "umulh	x26, x5, x23\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x6, x22\n\t"
        "umulh	x26, x6, x22\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x7, x21\n\t"
        "umulh	x26, x7, x21\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x5, x24\n\t"
        "umulh	x26, x5, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x6, x23\n\t"
        "umulh	x26, x6, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x7, x22\n\t"
        "umulh	x26, x7, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x6, x24\n\t"
        "umulh	x26, x6, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x7, x23\n\t"
        "umulh	x26, x7, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x7, x24\n\t"
        "umulh	x26, x7, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x15, #63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x12, x12, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x13, x13, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x14, x14, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x13, x13, x16\n\t"
        "adcs	x14, x14, x17\n\t"
        "adcs	x15, x15, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x15, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x15, asr 63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [sp, #96]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x1]\n\t"
        "ldp	x23, x24, [x1, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x8, x21\n\t"
        "umulh	x5, x8, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x8, x22\n\t"
        "umulh	x6, x8, x22\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x9, x21\n\t"
        "umulh	x26, x9, x21\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x8, x23\n\t"
        "umulh	x26, x8, x23\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x9, x22\n\t"
        "umulh	x26, x9, x22\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x10, x21\n\t"
        "umulh	x26, x10, x21\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x8, x24\n\t"
        "umulh	x26, x8, x24\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x9, x23\n\t"
        "umulh	x26, x9, x23\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x10, x22\n\t"
        "umulh	x26, x10, x22\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x11, x21\n\t"
        "umulh	x26, x11, x21\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x9, x24\n\t"
        "umulh	x26, x9, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x10, x23\n\t"
        "umulh	x26, x10, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x11, x22\n\t"
        "umulh	x26, x11, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x10, x24\n\t"
        "umulh	x26, x10, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x11, x23\n\t"
        "umulh	x26, x11, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x11, x24\n\t"
        "umulh	x26, x11, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x16\n\t"
        "adcs	x6, x6, x17\n\t"
        "adcs	x7, x7, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [x29, #16]\n\t"
        /* Add */
        "adds	x8, x12, x4\n\t"
        "adcs	x9, x13, x5\n\t"
        "adcs	x10, x14, x6\n\t"
        "adc	x11, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x11, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x8, x8, x25\n\t"
        "sbcs	x9, x9, x28\n\t"
        "sbcs	x10, x10, x28\n\t"
        "sbc	x11, x11, x26\n\t"
        /* Sub */
        "subs	x16, x12, x4\n\t"
        "sbcs	x17, x13, x5\n\t"
        "sbcs	x19, x14, x6\n\t"
        "sbcs	x20, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x8, x9, [x0]\n\t"
        "stp	x10, x11, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldr	x0, [x29, #40]\n\t"
        "ldr	x1, [sp, #88]\n\t"
        "ldr	x3, [x29, #72]\n\t"
        /* Multiply */
        "ldp	x16, x17, [x1]\n\t"
        "ldp	x19, x20, [x1, #16]\n\t"
        "ldp	x21, x22, [x3]\n\t"
        "ldp	x23, x24, [x3, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x16, x21\n\t"
        "umulh	x5, x16, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x16, x22\n\t"
        "umulh	x6, x16, x22\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x17, x21\n\t"
        "umulh	x26, x17, x21\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x16, x23\n\t"
        "umulh	x26, x16, x23\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x17, x22\n\t"
        "umulh	x26, x17, x22\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x19, x21\n\t"
        "umulh	x26, x19, x21\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x16, x24\n\t"
        "umulh	x26, x16, x24\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x17, x23\n\t"
        "umulh	x26, x17, x23\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x19, x22\n\t"
        "umulh	x26, x19, x22\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x20, x21\n\t"
        "umulh	x26, x20, x21\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x17, x24\n\t"
        "umulh	x26, x17, x24\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x19, x23\n\t"
        "umulh	x26, x19, x23\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x20, x22\n\t"
        "umulh	x26, x20, x22\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x19, x24\n\t"
        "umulh	x26, x19, x24\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x20, x23\n\t"
        "umulh	x26, x20, x23\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x20, x24\n\t"
        "umulh	x26, x20, x24\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x8\n\t"
        "umulh	x8, x25, x8\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x9\n\t"
        "umulh	x9, x25, x9\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x10\n\t"
        "umulh	x10, x25, x10\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x11\n\t"
        "umulh	x27, x25, x11\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x1, [x29, #64]\n\t"
        /* Double */
        "ldp	x8, x9, [x1]\n\t"
        "ldp	x10, x11, [x1, #16]\n\t"
        "adds	x8, x8, x8\n\t"
        "adcs	x9, x9, x9\n\t"
        "adcs	x10, x10, x10\n\t"
        "adc	x11, x11, x11\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x11, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x8, x8, x25\n\t"
        "sbcs	x9, x9, x28\n\t"
        "sbcs	x10, x10, x28\n\t"
        "sbc	x11, x11, x26\n\t"
        "ldr	x1, [x29, #40]\n\t"
        /* Add */
        "adds	x12, x8, x4\n\t"
        "adcs	x13, x9, x5\n\t"
        "adcs	x14, x10, x6\n\t"
        "adc	x15, x11, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x15, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x12, x12, x25\n\t"
        "sbcs	x13, x13, x28\n\t"
        "sbcs	x14, x14, x28\n\t"
        "sbc	x15, x15, x26\n\t"
        /* Sub */
        "subs	x16, x8, x4\n\t"
        "sbcs	x17, x9, x5\n\t"
        "sbcs	x19, x10, x6\n\t"
        "sbcs	x20, x11, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x12, x13, [x1]\n\t"
        "stp	x14, x15, [x1, #16]\n\t"
        "stp	x16, x17, [x0]\n\t"
        "stp	x19, x20, [x0, #16]\n\t"
        "ldp	x29, x30, [sp], #0x70\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz), [pt] "+r" (pt), [qxy2d] "+r" (qxy2d), [qyplusx] "+r" (qyplusx), [qyminusx] "+r" (qyminusx)
        :
        : "memory", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

void fe_ge_add(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py, const fe pz, const fe pt, const fe qz, const fe qt2d, const fe qyplusx, const fe qyminusx)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-128]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[qyminusx], [sp, #120]\n\t"
        "str	%x[qyplusx], [sp, #112]\n\t"
        "str	%x[qt2d], [sp, #104]\n\t"
        "str	%x[qz], [sp, #96]\n\t"
        "str	%x[rx], [x29, #16]\n\t"
        "str	%x[ry], [x29, #24]\n\t"
        "str	%x[rz], [x29, #32]\n\t"
        "str	%x[rt], [x29, #40]\n\t"
        "str	%x[px], [x29, #48]\n\t"
        "str	%x[py], [x29, #56]\n\t"
        "str	%x[pz], [x29, #64]\n\t"
        "str	%x[pt], [x29, #72]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        "ldr	x3, [x29, #48]\n\t"
        /* Add */
        "ldp	x12, x13, [x2]\n\t"
        "ldp	x14, x15, [x2, #16]\n\t"
        "ldp	x16, x17, [x3]\n\t"
        "ldp	x19, x20, [x3, #16]\n\t"
        "adds	x4, x12, x16\n\t"
        "adcs	x5, x13, x17\n\t"
        "adcs	x6, x14, x19\n\t"
        "adc	x7, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x7, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x4, x4, x25\n\t"
        "sbcs	x5, x5, x28\n\t"
        "sbcs	x6, x6, x28\n\t"
        "sbc	x7, x7, x26\n\t"
        /* Sub */
        "subs	x8, x12, x16\n\t"
        "sbcs	x9, x13, x17\n\t"
        "sbcs	x10, x14, x19\n\t"
        "sbcs	x11, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x28\n\t"
        "adcs	x10, x10, x28\n\t"
        "adc	x11, x11, x26\n\t"
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x2, [sp, #112]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x2]\n\t"
        "ldp	x23, x24, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x12, x4, x21\n\t"
        "umulh	x13, x4, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x4, x22\n\t"
        "umulh	x14, x4, x22\n\t"
        "adds	x13, x13, x25\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x5, x21\n\t"
        "umulh	x26, x5, x21\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x4, x23\n\t"
        "umulh	x26, x4, x23\n\t"
        "adds	x14, x14, x25\n\t"
        "adc	x15, x15, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x5, x22\n\t"
        "umulh	x26, x5, x22\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x6, x21\n\t"
        "umulh	x26, x6, x21\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x4, x24\n\t"
        "umulh	x26, x4, x24\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x5, x23\n\t"
        "umulh	x26, x5, x23\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x6, x22\n\t"
        "umulh	x26, x6, x22\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x7, x21\n\t"
        "umulh	x26, x7, x21\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x5, x24\n\t"
        "umulh	x26, x5, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x6, x23\n\t"
        "umulh	x26, x6, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x7, x22\n\t"
        "umulh	x26, x7, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x6, x24\n\t"
        "umulh	x26, x6, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x7, x23\n\t"
        "umulh	x26, x7, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x7, x24\n\t"
        "umulh	x26, x7, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x15, #63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x12, x12, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x13, x13, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x14, x14, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x13, x13, x16\n\t"
        "adcs	x14, x14, x17\n\t"
        "adcs	x15, x15, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x15, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x15, asr 63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [sp, #120]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x1]\n\t"
        "ldp	x23, x24, [x1, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x8, x21\n\t"
        "umulh	x5, x8, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x8, x22\n\t"
        "umulh	x6, x8, x22\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x9, x21\n\t"
        "umulh	x26, x9, x21\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x8, x23\n\t"
        "umulh	x26, x8, x23\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x9, x22\n\t"
        "umulh	x26, x9, x22\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x10, x21\n\t"
        "umulh	x26, x10, x21\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x8, x24\n\t"
        "umulh	x26, x8, x24\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x9, x23\n\t"
        "umulh	x26, x9, x23\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x10, x22\n\t"
        "umulh	x26, x10, x22\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x11, x21\n\t"
        "umulh	x26, x11, x21\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x9, x24\n\t"
        "umulh	x26, x9, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x10, x23\n\t"
        "umulh	x26, x10, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x11, x22\n\t"
        "umulh	x26, x11, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x10, x24\n\t"
        "umulh	x26, x10, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x11, x23\n\t"
        "umulh	x26, x11, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x11, x24\n\t"
        "umulh	x26, x11, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x16\n\t"
        "adcs	x6, x6, x17\n\t"
        "adcs	x7, x7, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [x29, #16]\n\t"
        /* Add */
        "adds	x8, x12, x4\n\t"
        "adcs	x9, x13, x5\n\t"
        "adcs	x10, x14, x6\n\t"
        "adc	x11, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x11, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x8, x8, x25\n\t"
        "sbcs	x9, x9, x28\n\t"
        "sbcs	x10, x10, x28\n\t"
        "sbc	x11, x11, x26\n\t"
        /* Sub */
        "subs	x16, x12, x4\n\t"
        "sbcs	x17, x13, x5\n\t"
        "sbcs	x19, x14, x6\n\t"
        "sbcs	x20, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x8, x9, [x0]\n\t"
        "stp	x10, x11, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldr	x0, [x29, #48]\n\t"
        "ldr	x1, [x29, #64]\n\t"
        "ldr	x2, [sp, #96]\n\t"
        /* Multiply */
        "ldp	x12, x13, [x1]\n\t"
        "ldp	x14, x15, [x1, #16]\n\t"
        "ldp	x16, x17, [x2]\n\t"
        "ldp	x19, x20, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x12, x16\n\t"
        "umulh	x5, x12, x16\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x12, x17\n\t"
        "umulh	x6, x12, x17\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x13, x16\n\t"
        "umulh	x26, x13, x16\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x12, x19\n\t"
        "umulh	x26, x12, x19\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x13, x17\n\t"
        "umulh	x26, x13, x17\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x14, x16\n\t"
        "umulh	x26, x14, x16\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x12, x20\n\t"
        "umulh	x26, x12, x20\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x13, x19\n\t"
        "umulh	x26, x13, x19\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x14, x17\n\t"
        "umulh	x26, x14, x17\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x15, x16\n\t"
        "umulh	x26, x15, x16\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x13, x20\n\t"
        "umulh	x26, x13, x20\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x14, x19\n\t"
        "umulh	x26, x14, x19\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x15, x17\n\t"
        "umulh	x26, x15, x17\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x14, x20\n\t"
        "umulh	x26, x14, x20\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x15, x19\n\t"
        "umulh	x26, x15, x19\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x15, x20\n\t"
        "umulh	x26, x15, x20\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x8\n\t"
        "umulh	x8, x25, x8\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x9\n\t"
        "umulh	x9, x25, x9\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x10\n\t"
        "umulh	x10, x25, x10\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x11\n\t"
        "umulh	x27, x25, x11\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #48]\n\t"
        /* Double */
        "adds	x4, x4, x4\n\t"
        "adcs	x5, x5, x5\n\t"
        "adcs	x6, x6, x6\n\t"
        "adc	x7, x7, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x7, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x4, x4, x25\n\t"
        "sbcs	x5, x5, x28\n\t"
        "sbcs	x6, x6, x28\n\t"
        "sbc	x7, x7, x26\n\t"
        "ldr	x0, [x29, #40]\n\t"
        "ldr	x1, [sp, #104]\n\t"
        "ldr	x2, [x29, #72]\n\t"
        /* Multiply */
        "ldp	x16, x17, [x1]\n\t"
        "ldp	x19, x20, [x1, #16]\n\t"
        "ldp	x21, x22, [x2]\n\t"
        "ldp	x23, x24, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x8, x16, x21\n\t"
        "umulh	x9, x16, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x16, x22\n\t"
        "umulh	x10, x16, x22\n\t"
        "adds	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x17, x21\n\t"
        "umulh	x26, x17, x21\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x16, x23\n\t"
        "umulh	x26, x16, x23\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x17, x22\n\t"
        "umulh	x26, x17, x22\n\t"
        "adds	x10, x10, x25\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x12, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x19, x21\n\t"
        "umulh	x26, x19, x21\n\t"
        "adds	x10, x10, x25\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x12, x12, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x16, x24\n\t"
        "umulh	x26, x16, x24\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x17, x23\n\t"
        "umulh	x26, x17, x23\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x19, x22\n\t"
        "umulh	x26, x19, x22\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x20, x21\n\t"
        "umulh	x26, x20, x21\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x17, x24\n\t"
        "umulh	x26, x17, x24\n\t"
        "adds	x12, x12, x25\n\t"
        "adcs	x13, x13, x26\n\t"
        "adc	x14, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x19, x23\n\t"
        "umulh	x26, x19, x23\n\t"
        "adds	x12, x12, x25\n\t"
        "adcs	x13, x13, x26\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x20, x22\n\t"
        "umulh	x26, x20, x22\n\t"
        "adds	x12, x12, x25\n\t"
        "adcs	x13, x13, x26\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x19, x24\n\t"
        "umulh	x26, x19, x24\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x20, x23\n\t"
        "umulh	x26, x20, x23\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, x15, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x20, x24\n\t"
        "umulh	x26, x20, x24\n\t"
        "adds	x14, x14, x25\n\t"
        "adc	x15, x15, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x15, x15, x14, #63\n\t"
        "extr	x14, x14, x13, #63\n\t"
        "extr	x13, x13, x12, #63\n\t"
        "extr	x12, x12, x11, #63\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x12\n\t"
        "umulh	x12, x25, x12\n\t"
        "adds	x8, x8, x26\n\t"
        "mul	x26, x25, x13\n\t"
        "umulh	x13, x25, x13\n\t"
        "adcs	x9, x9, x26\n\t"
        "mul	x26, x25, x14\n\t"
        "umulh	x14, x25, x14\n\t"
        "adcs	x10, x10, x26\n\t"
        "mul	x26, x25, x15\n\t"
        "umulh	x27, x25, x15\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x9, x9, x12\n\t"
        "adcs	x10, x10, x13\n\t"
        "adcs	x11, x11, x14\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x11, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        "adds	x8, x8, x27\n\t"
        "adcs	x9, x9, xzr\n\t"
        "adcs	x10, x10, xzr\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x11, asr 63\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        "adds	x8, x8, x27\n\t"
        "adcs	x9, x9, xzr\n\t"
        "adcs	x10, x10, xzr\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x1, [x29, #40]\n\t"
        /* Add */
        "adds	x12, x4, x8\n\t"
        "adcs	x13, x5, x9\n\t"
        "adcs	x14, x6, x10\n\t"
        "adc	x15, x7, x11\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x15, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x12, x12, x25\n\t"
        "sbcs	x13, x13, x28\n\t"
        "sbcs	x14, x14, x28\n\t"
        "sbc	x15, x15, x26\n\t"
        /* Sub */
        "subs	x16, x4, x8\n\t"
        "sbcs	x17, x5, x9\n\t"
        "sbcs	x19, x6, x10\n\t"
        "sbcs	x20, x7, x11\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x12, x13, [x0]\n\t"
        "stp	x14, x15, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldp	x29, x30, [sp], #0x80\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz), [pt] "+r" (pt), [qz] "+r" (qz), [qt2d] "+r" (qt2d), [qyplusx] "+r" (qyplusx), [qyminusx] "+r" (qyminusx)
        :
        : "memory", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

void fe_ge_sub(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py, const fe pz, const fe pt, const fe qz, const fe qt2d, const fe qyplusx, const fe qyminusx)
{
    __asm__ __volatile__ (
        "stp	x29, x30, [sp, #-128]!\n\t"
        "add	x29, sp, #0\n\t"
        "str	%x[qyminusx], [sp, #120]\n\t"
        "str	%x[qyplusx], [sp, #112]\n\t"
        "str	%x[qt2d], [sp, #104]\n\t"
        "str	%x[qz], [sp, #96]\n\t"
        "str	%x[rx], [x29, #16]\n\t"
        "str	%x[ry], [x29, #24]\n\t"
        "str	%x[rz], [x29, #32]\n\t"
        "str	%x[rt], [x29, #40]\n\t"
        "str	%x[px], [x29, #48]\n\t"
        "str	%x[py], [x29, #56]\n\t"
        "str	%x[pz], [x29, #64]\n\t"
        "str	%x[pt], [x29, #72]\n\t"
        "ldr	x2, [x29, #56]\n\t"
        "ldr	x3, [x29, #48]\n\t"
        /* Add */
        "ldp	x12, x13, [x2]\n\t"
        "ldp	x14, x15, [x2, #16]\n\t"
        "ldp	x16, x17, [x3]\n\t"
        "ldp	x19, x20, [x3, #16]\n\t"
        "adds	x4, x12, x16\n\t"
        "adcs	x5, x13, x17\n\t"
        "adcs	x6, x14, x19\n\t"
        "adc	x7, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x7, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x4, x4, x25\n\t"
        "sbcs	x5, x5, x28\n\t"
        "sbcs	x6, x6, x28\n\t"
        "sbc	x7, x7, x26\n\t"
        /* Sub */
        "subs	x8, x12, x16\n\t"
        "sbcs	x9, x13, x17\n\t"
        "sbcs	x10, x14, x19\n\t"
        "sbcs	x11, x15, x20\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x28\n\t"
        "adcs	x10, x10, x28\n\t"
        "adc	x11, x11, x26\n\t"
        "ldr	x0, [x29, #32]\n\t"
        "ldr	x2, [sp, #120]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x2]\n\t"
        "ldp	x23, x24, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x12, x4, x21\n\t"
        "umulh	x13, x4, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x4, x22\n\t"
        "umulh	x14, x4, x22\n\t"
        "adds	x13, x13, x25\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x5, x21\n\t"
        "umulh	x26, x5, x21\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x4, x23\n\t"
        "umulh	x26, x4, x23\n\t"
        "adds	x14, x14, x25\n\t"
        "adc	x15, x15, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x5, x22\n\t"
        "umulh	x26, x5, x22\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x6, x21\n\t"
        "umulh	x26, x6, x21\n\t"
        "adds	x14, x14, x25\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x4, x24\n\t"
        "umulh	x26, x4, x24\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x5, x23\n\t"
        "umulh	x26, x5, x23\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x6, x22\n\t"
        "umulh	x26, x6, x22\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x7, x21\n\t"
        "umulh	x26, x7, x21\n\t"
        "adds	x15, x15, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x5, x24\n\t"
        "umulh	x26, x5, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x6, x23\n\t"
        "umulh	x26, x6, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x7, x22\n\t"
        "umulh	x26, x7, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x6, x24\n\t"
        "umulh	x26, x6, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x7, x23\n\t"
        "umulh	x26, x7, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x7, x24\n\t"
        "umulh	x26, x7, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x15, #63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x12, x12, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x13, x13, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x14, x14, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x15, x15, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x13, x13, x16\n\t"
        "adcs	x14, x14, x17\n\t"
        "adcs	x15, x15, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x15, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x15, asr 63\n\t"
        "and	x15, x15, #0x7fffffffffffffff\n\t"
        "adds	x12, x12, x27\n\t"
        "adcs	x13, x13, xzr\n\t"
        "adcs	x14, x14, xzr\n\t"
        "adc	x15, x15, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [sp, #112]\n\t"
        /* Multiply */
        "ldp	x21, x22, [x1]\n\t"
        "ldp	x23, x24, [x1, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x8, x21\n\t"
        "umulh	x5, x8, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x8, x22\n\t"
        "umulh	x6, x8, x22\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x9, x21\n\t"
        "umulh	x26, x9, x21\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x8, x23\n\t"
        "umulh	x26, x8, x23\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x9, x22\n\t"
        "umulh	x26, x9, x22\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x10, x21\n\t"
        "umulh	x26, x10, x21\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x16, x16, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x8, x24\n\t"
        "umulh	x26, x8, x24\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x9, x23\n\t"
        "umulh	x26, x9, x23\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x10, x22\n\t"
        "umulh	x26, x10, x22\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x11, x21\n\t"
        "umulh	x26, x11, x21\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x16, x16, x26\n\t"
        "adc	x17, x17, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x9, x24\n\t"
        "umulh	x26, x9, x24\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x10, x23\n\t"
        "umulh	x26, x10, x23\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x11, x22\n\t"
        "umulh	x26, x11, x22\n\t"
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x26\n\t"
        "adc	x19, x19, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x10, x24\n\t"
        "umulh	x26, x10, x24\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x11, x23\n\t"
        "umulh	x26, x11, x23\n\t"
        "adds	x17, x17, x25\n\t"
        "adcs	x19, x19, x26\n\t"
        "adc	x20, x20, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x11, x24\n\t"
        "umulh	x26, x11, x24\n\t"
        "adds	x19, x19, x25\n\t"
        "adc	x20, x20, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x20, x20, x19, #63\n\t"
        "extr	x19, x19, x17, #63\n\t"
        "extr	x17, x17, x16, #63\n\t"
        "extr	x16, x16, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x16\n\t"
        "umulh	x16, x25, x16\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x17\n\t"
        "umulh	x17, x25, x17\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x19\n\t"
        "umulh	x19, x25, x19\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x20\n\t"
        "umulh	x27, x25, x20\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x16\n\t"
        "adcs	x6, x6, x17\n\t"
        "adcs	x7, x7, x19\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #24]\n\t"
        "ldr	x1, [x29, #16]\n\t"
        /* Add */
        "adds	x8, x12, x4\n\t"
        "adcs	x9, x13, x5\n\t"
        "adcs	x10, x14, x6\n\t"
        "adc	x11, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x11, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x8, x8, x25\n\t"
        "sbcs	x9, x9, x28\n\t"
        "sbcs	x10, x10, x28\n\t"
        "sbc	x11, x11, x26\n\t"
        /* Sub */
        "subs	x16, x12, x4\n\t"
        "sbcs	x17, x13, x5\n\t"
        "sbcs	x19, x14, x6\n\t"
        "sbcs	x20, x15, x7\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x8, x9, [x0]\n\t"
        "stp	x10, x11, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldr	x0, [x29, #48]\n\t"
        "ldr	x1, [x29, #64]\n\t"
        "ldr	x2, [sp, #96]\n\t"
        /* Multiply */
        "ldp	x12, x13, [x1]\n\t"
        "ldp	x14, x15, [x1, #16]\n\t"
        "ldp	x16, x17, [x2]\n\t"
        "ldp	x19, x20, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x4, x12, x16\n\t"
        "umulh	x5, x12, x16\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x12, x17\n\t"
        "umulh	x6, x12, x17\n\t"
        "adds	x5, x5, x25\n\t"
        "adc	x6, x6, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x13, x16\n\t"
        "umulh	x26, x13, x16\n\t"
        "adds	x5, x5, x25\n\t"
        "adcs	x6, x6, x26\n\t"
        "adc	x7, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x12, x19\n\t"
        "umulh	x26, x12, x19\n\t"
        "adds	x6, x6, x25\n\t"
        "adc	x7, x7, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x13, x17\n\t"
        "umulh	x26, x13, x17\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x14, x16\n\t"
        "umulh	x26, x14, x16\n\t"
        "adds	x6, x6, x25\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x8, x8, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x12, x20\n\t"
        "umulh	x26, x12, x20\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x13, x19\n\t"
        "umulh	x26, x13, x19\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x14, x17\n\t"
        "umulh	x26, x14, x17\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x15, x16\n\t"
        "umulh	x26, x15, x16\n\t"
        "adds	x7, x7, x25\n\t"
        "adcs	x8, x8, x26\n\t"
        "adc	x9, x9, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x13, x20\n\t"
        "umulh	x26, x13, x20\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x14, x19\n\t"
        "umulh	x26, x14, x19\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x15, x17\n\t"
        "umulh	x26, x15, x17\n\t"
        "adds	x8, x8, x25\n\t"
        "adcs	x9, x9, x26\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x14, x20\n\t"
        "umulh	x26, x14, x20\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x15, x19\n\t"
        "umulh	x26, x15, x19\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, x11, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x15, x20\n\t"
        "umulh	x26, x15, x20\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x11, x11, x10, #63\n\t"
        "extr	x10, x10, x9, #63\n\t"
        "extr	x9, x9, x8, #63\n\t"
        "extr	x8, x8, x7, #63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x8\n\t"
        "umulh	x8, x25, x8\n\t"
        "adds	x4, x4, x26\n\t"
        "mul	x26, x25, x9\n\t"
        "umulh	x9, x25, x9\n\t"
        "adcs	x5, x5, x26\n\t"
        "mul	x26, x25, x10\n\t"
        "umulh	x10, x25, x10\n\t"
        "adcs	x6, x6, x26\n\t"
        "mul	x26, x25, x11\n\t"
        "umulh	x27, x25, x11\n\t"
        "adcs	x7, x7, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x5, x5, x8\n\t"
        "adcs	x6, x6, x9\n\t"
        "adcs	x7, x7, x10\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x7, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x7, asr 63\n\t"
        "and	x7, x7, #0x7fffffffffffffff\n\t"
        "adds	x4, x4, x27\n\t"
        "adcs	x5, x5, xzr\n\t"
        "adcs	x6, x6, xzr\n\t"
        "adc	x7, x7, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #48]\n\t"
        /* Double */
        "adds	x4, x4, x4\n\t"
        "adcs	x5, x5, x5\n\t"
        "adcs	x6, x6, x6\n\t"
        "adc	x7, x7, x7\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x7, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x4, x4, x25\n\t"
        "sbcs	x5, x5, x28\n\t"
        "sbcs	x6, x6, x28\n\t"
        "sbc	x7, x7, x26\n\t"
        "ldr	x0, [x29, #40]\n\t"
        "ldr	x1, [sp, #104]\n\t"
        "ldr	x2, [x29, #72]\n\t"
        /* Multiply */
        "ldp	x16, x17, [x1]\n\t"
        "ldp	x19, x20, [x1, #16]\n\t"
        "ldp	x21, x22, [x2]\n\t"
        "ldp	x23, x24, [x2, #16]\n\t"
        /*  A[0] * B[0] */
        "mul	x8, x16, x21\n\t"
        "umulh	x9, x16, x21\n\t"
        /*  A[0] * B[1] */
        "mul	x25, x16, x22\n\t"
        "umulh	x10, x16, x22\n\t"
        "adds	x9, x9, x25\n\t"
        "adc	x10, x10, xzr\n\t"
        /*  A[1] * B[0] */
        "mul	x25, x17, x21\n\t"
        "umulh	x26, x17, x21\n\t"
        "adds	x9, x9, x25\n\t"
        "adcs	x10, x10, x26\n\t"
        "adc	x11, xzr, xzr\n\t"
        /*  A[0] * B[2] */
        "mul	x25, x16, x23\n\t"
        "umulh	x26, x16, x23\n\t"
        "adds	x10, x10, x25\n\t"
        "adc	x11, x11, x26\n\t"
        /*  A[1] * B[1] */
        "mul	x25, x17, x22\n\t"
        "umulh	x26, x17, x22\n\t"
        "adds	x10, x10, x25\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x12, xzr, xzr\n\t"
        /*  A[2] * B[0] */
        "mul	x25, x19, x21\n\t"
        "umulh	x26, x19, x21\n\t"
        "adds	x10, x10, x25\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x12, x12, xzr\n\t"
        /*  A[0] * B[3] */
        "mul	x25, x16, x24\n\t"
        "umulh	x26, x16, x24\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, xzr, xzr\n\t"
        /*  A[1] * B[2] */
        "mul	x25, x17, x23\n\t"
        "umulh	x26, x17, x23\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[2] * B[1] */
        "mul	x25, x19, x22\n\t"
        "umulh	x26, x19, x22\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[3] * B[0] */
        "mul	x25, x20, x21\n\t"
        "umulh	x26, x20, x21\n\t"
        "adds	x11, x11, x25\n\t"
        "adcs	x12, x12, x26\n\t"
        "adc	x13, x13, xzr\n\t"
        /*  A[1] * B[3] */
        "mul	x25, x17, x24\n\t"
        "umulh	x26, x17, x24\n\t"
        "adds	x12, x12, x25\n\t"
        "adcs	x13, x13, x26\n\t"
        "adc	x14, xzr, xzr\n\t"
        /*  A[2] * B[2] */
        "mul	x25, x19, x23\n\t"
        "umulh	x26, x19, x23\n\t"
        "adds	x12, x12, x25\n\t"
        "adcs	x13, x13, x26\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[3] * B[1] */
        "mul	x25, x20, x22\n\t"
        "umulh	x26, x20, x22\n\t"
        "adds	x12, x12, x25\n\t"
        "adcs	x13, x13, x26\n\t"
        "adc	x14, x14, xzr\n\t"
        /*  A[2] * B[3] */
        "mul	x25, x19, x24\n\t"
        "umulh	x26, x19, x24\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, xzr, xzr\n\t"
        /*  A[3] * B[2] */
        "mul	x25, x20, x23\n\t"
        "umulh	x26, x20, x23\n\t"
        "adds	x13, x13, x25\n\t"
        "adcs	x14, x14, x26\n\t"
        "adc	x15, x15, xzr\n\t"
        /*  A[3] * B[3] */
        "mul	x25, x20, x24\n\t"
        "umulh	x26, x20, x24\n\t"
        "adds	x14, x14, x25\n\t"
        "adc	x15, x15, x26\n\t"
        /* Reduce */
        /*  Move top half into t4-t7 and remove top bit from t3 */
        "extr	x15, x15, x14, #63\n\t"
        "extr	x14, x14, x13, #63\n\t"
        "extr	x13, x13, x12, #63\n\t"
        "extr	x12, x12, x11, #63\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        /*  Multiply top half by 19 */
        "mov	x25, #19\n\t"
        "mul	x26, x25, x12\n\t"
        "umulh	x12, x25, x12\n\t"
        "adds	x8, x8, x26\n\t"
        "mul	x26, x25, x13\n\t"
        "umulh	x13, x25, x13\n\t"
        "adcs	x9, x9, x26\n\t"
        "mul	x26, x25, x14\n\t"
        "umulh	x14, x25, x14\n\t"
        "adcs	x10, x10, x26\n\t"
        "mul	x26, x25, x15\n\t"
        "umulh	x27, x25, x15\n\t"
        "adcs	x11, x11, x26\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Add remaining product results in */
        "adds	x9, x9, x12\n\t"
        "adcs	x10, x10, x13\n\t"
        "adcs	x11, x11, x14\n\t"
        "adc	x27, x27, xzr\n\t"
        /*  Overflow */
        "extr	x27, x27, x11, #63\n\t"
        "mul	x27, x27, x25\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        "adds	x8, x8, x27\n\t"
        "adcs	x9, x9, xzr\n\t"
        "adcs	x10, x10, xzr\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Reduce if top bit set */
        "and	x27, x25, x11, asr 63\n\t"
        "and	x11, x11, #0x7fffffffffffffff\n\t"
        "adds	x8, x8, x27\n\t"
        "adcs	x9, x9, xzr\n\t"
        "adcs	x10, x10, xzr\n\t"
        "adc	x11, x11, xzr\n\t"
        /* Store */
        "ldr	x0, [x29, #40]\n\t"
        "ldr	x1, [x29, #32]\n\t"
        /* Add */
        "adds	x12, x4, x8\n\t"
        "adcs	x13, x5, x9\n\t"
        "adcs	x14, x6, x10\n\t"
        "adc	x15, x7, x11\n\t"
        "mov	x25, #-19\n\t"
        "asr	x28, x15, #63\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	x12, x12, x25\n\t"
        "sbcs	x13, x13, x28\n\t"
        "sbcs	x14, x14, x28\n\t"
        "sbc	x15, x15, x26\n\t"
        /* Sub */
        "subs	x16, x4, x8\n\t"
        "sbcs	x17, x5, x9\n\t"
        "sbcs	x19, x6, x10\n\t"
        "sbcs	x20, x7, x11\n\t"
        "mov	x25, #-19\n\t"
        "csetm	x28, cc\n\t"
        /*   Mask the modulus */
        "and	x25, x28, x25\n\t"
        "and	x26, x28, #0x7fffffffffffffff\n\t"
        /*   Add modulus (if underflow) */
        "adds	x16, x16, x25\n\t"
        "adcs	x17, x17, x28\n\t"
        "adcs	x19, x19, x28\n\t"
        "adc	x20, x20, x26\n\t"
        "stp	x12, x13, [x0]\n\t"
        "stp	x14, x15, [x0, #16]\n\t"
        "stp	x16, x17, [x1]\n\t"
        "stp	x19, x20, [x1, #16]\n\t"
        "ldp	x29, x30, [sp], #0x80\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt), [px] "+r" (px), [py] "+r" (py), [pz] "+r" (pz), [pt] "+r" (pt), [qz] "+r" (qz), [qt2d] "+r" (qt2d), [qyplusx] "+r" (qyplusx), [qyminusx] "+r" (qyminusx)
        :
        : "memory", "x12", "x13", "x14", "x15", "x16", "x17", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28"
    );
}

#endif /* HAVE_CURVE25519 */
#endif /* __aarch64__ */
#endif /* WOLFSSL_ARMASM */
