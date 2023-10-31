/* armv8-32-curve25519
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
 *   ruby ./x25519/x25519.rb arm32 ../wolfssl/wolfcrypt/src/port/arm/armv8-32-curve25519.c
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif /* HAVE_CONFIG_H */
#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_ARMASM
#ifndef __aarch64__
#include <stdint.h>
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif /* HAVE_CONFIG_H */
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/fe_operations.h>

#ifdef HAVE_CURVE25519

void fe_init()
{

    __asm__ __volatile__ (
        "\n\t"
        : 
        :
        : "memory"
    );
}

void fe_frombytes(fe out_p, const unsigned char* in_p)
{
    register fe out asm ("r0") = out_p;
    register const unsigned char* in asm ("r1") = in_p;

    __asm__ __volatile__ (
        "ldr	r2, [%[in]]\n\t"
        "ldr	r3, [%[in], #4]\n\t"
        "ldr	r4, [%[in], #8]\n\t"
        "ldr	r5, [%[in], #12]\n\t"
        "ldr	r6, [%[in], #16]\n\t"
        "ldr	r7, [%[in], #20]\n\t"
        "ldr	r8, [%[in], #24]\n\t"
        "ldr	r9, [%[in], #28]\n\t"
        "and	r9, r9, #0x7fffffff\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[out]]\n\t"
        "str	r3, [%[out], #4]\n\t"
#else
        "strd	r2, r3, [%[out]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[out], #8]\n\t"
        "str	r5, [%[out], #12]\n\t"
#else
        "strd	r4, r5, [%[out], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[out], #16]\n\t"
        "str	r7, [%[out], #20]\n\t"
#else
        "strd	r6, r7, [%[out], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[out], #24]\n\t"
        "str	r9, [%[out], #28]\n\t"
#else
        "strd	r8, r9, [%[out], #24]\n\t"
#endif
        : [out] "+r" (out), [in] "+r" (in)
        :
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );
}

void fe_tobytes(unsigned char* out_p, const fe n_p)
{
    register unsigned char* out asm ("r0") = out_p;
    register const fe n asm ("r1") = n_p;

    __asm__ __volatile__ (
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[n]]\n\t"
        "ldr	r3, [%[n], #4]\n\t"
#else
        "ldrd	r2, r3, [%[n]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[n], #8]\n\t"
        "ldr	r5, [%[n], #12]\n\t"
#else
        "ldrd	r4, r5, [%[n], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[n], #16]\n\t"
        "ldr	r7, [%[n], #20]\n\t"
#else
        "ldrd	r6, r7, [%[n], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[n], #24]\n\t"
        "ldr	r9, [%[n], #28]\n\t"
#else
        "ldrd	r8, r9, [%[n], #24]\n\t"
#endif
        "adds	r12, r2, #19\n\t"
        "adcs	r12, r3, #0\n\t"
        "adcs	r12, r4, #0\n\t"
        "adcs	r12, r5, #0\n\t"
        "adcs	r12, r6, #0\n\t"
        "adcs	r12, r7, #0\n\t"
        "adcs	r12, r8, #0\n\t"
        "adc	r12, r9, #0\n\t"
        "asr	r12, r12, #31\n\t"
        "and	r12, r12, #19\n\t"
        "adds	r2, r2, r12\n\t"
        "adcs	r3, r3, #0\n\t"
        "adcs	r4, r4, #0\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adc	r9, r9, #0\n\t"
        "and	r9, r9, #0x7fffffff\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[out]]\n\t"
        "str	r3, [%[out], #4]\n\t"
#else
        "strd	r2, r3, [%[out]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[out], #8]\n\t"
        "str	r5, [%[out], #12]\n\t"
#else
        "strd	r4, r5, [%[out], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[out], #16]\n\t"
        "str	r7, [%[out], #20]\n\t"
#else
        "strd	r6, r7, [%[out], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[out], #24]\n\t"
        "str	r9, [%[out], #28]\n\t"
#else
        "strd	r8, r9, [%[out], #24]\n\t"
#endif
        : [out] "+r" (out), [n] "+r" (n)
        :
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r12"
    );
}

void fe_1(fe n_p)
{
    register fe n asm ("r0") = n_p;

    __asm__ __volatile__ (
        /* Set one */
        "mov	r2, #1\n\t"
        "mov	r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n]]\n\t"
        "str	r3, [%[n], #4]\n\t"
#else
        "strd	r2, r3, [%[n]]\n\t"
#endif
        "mov	r2, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n], #8]\n\t"
        "str	r3, [%[n], #12]\n\t"
#else
        "strd	r2, r3, [%[n], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n], #16]\n\t"
        "str	r3, [%[n], #20]\n\t"
#else
        "strd	r2, r3, [%[n], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n], #24]\n\t"
        "str	r3, [%[n], #28]\n\t"
#else
        "strd	r2, r3, [%[n], #24]\n\t"
#endif
        : [n] "+r" (n)
        :
        : "memory", "r2", "r3"
    );
}

void fe_0(fe n_p)
{
    register fe n asm ("r0") = n_p;

    __asm__ __volatile__ (
        /* Set zero */
        "mov	r2, #0\n\t"
        "mov	r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n]]\n\t"
        "str	r3, [%[n], #4]\n\t"
#else
        "strd	r2, r3, [%[n]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n], #8]\n\t"
        "str	r3, [%[n], #12]\n\t"
#else
        "strd	r2, r3, [%[n], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n], #16]\n\t"
        "str	r3, [%[n], #20]\n\t"
#else
        "strd	r2, r3, [%[n], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[n], #24]\n\t"
        "str	r3, [%[n], #28]\n\t"
#else
        "strd	r2, r3, [%[n], #24]\n\t"
#endif
        : [n] "+r" (n)
        :
        : "memory", "r2", "r3"
    );
}

void fe_copy(fe r_p, const fe a_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        /* Copy */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a]]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
#else
        "ldrd	r2, r3, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
#else
        "ldrd	r4, r5, [%[a], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[r]]\n\t"
        "str	r3, [%[r], #4]\n\t"
#else
        "strd	r2, r3, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #8]\n\t"
        "str	r5, [%[r], #12]\n\t"
#else
        "strd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
#else
        "ldrd	r2, r3, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
#else
        "ldrd	r4, r5, [%[a], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[r], #16]\n\t"
        "str	r3, [%[r], #20]\n\t"
#else
        "strd	r2, r3, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #24]\n\t"
        "str	r5, [%[r], #28]\n\t"
#else
        "strd	r4, r5, [%[r], #24]\n\t"
#endif
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "r2", "r3", "r4", "r5"
    );
}

void fe_sub(fe r_p, const fe a_p, const fe b_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;
    register const fe b asm ("r2") = b_p;

    __asm__ __volatile__ (
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a]]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
#else
        "ldrd	r4, r5, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
#else
        "ldrd	r6, r7, [%[a], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[b]]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
#else
        "ldrd	r8, r9, [%[b]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r11, [%[b], #12]\n\t"
#else
        "ldrd	r10, r11, [%[b], #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r]]\n\t"
        "str	r9, [%[r], #4]\n\t"
#else
        "strd	r8, r9, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #8]\n\t"
        "str	r11, [%[r], #12]\n\t"
#else
        "strd	r10, r11, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
#else
        "ldrd	r4, r5, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
#else
        "ldrd	r6, r7, [%[a], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
#else
        "ldrd	r8, r9, [%[b], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r11, [%[b], #28]\n\t"
#else
        "ldrd	r10, r11, [%[b], #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	r3, r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, r3, r12\n\t"
        "and	lr, r3, #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r]]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
#else
        "ldrd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[r], #8]\n\t"
        "ldr	r7, [%[r], #12]\n\t"
#else
        "ldrd	r6, r7, [%[r], #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, r3\n\t"
        "adcs	r6, r6, r3\n\t"
        "adcs	r7, r7, r3\n\t"
        "adcs	r8, r8, r3\n\t"
        "adcs	r9, r9, r3\n\t"
        "adcs	r10, r10, r3\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
#else
        "strd	r6, r7, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
#else
        "strd	r8, r9, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #24]\n\t"
        "str	r11, [%[r], #28]\n\t"
#else
        "strd	r10, r11, [%[r], #24]\n\t"
#endif
        : [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
}

void fe_add(fe r_p, const fe a_p, const fe b_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;
    register const fe b asm ("r2") = b_p;

    __asm__ __volatile__ (
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a]]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
#else
        "ldrd	r4, r5, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
#else
        "ldrd	r6, r7, [%[a], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[b]]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
#else
        "ldrd	r8, r9, [%[b]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r11, [%[b], #12]\n\t"
#else
        "ldrd	r10, r11, [%[b], #8]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r]]\n\t"
        "str	r9, [%[r], #4]\n\t"
#else
        "strd	r8, r9, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #8]\n\t"
        "str	r11, [%[r], #12]\n\t"
#else
        "strd	r10, r11, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
#else
        "ldrd	r4, r5, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
#else
        "ldrd	r6, r7, [%[a], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
#else
        "ldrd	r8, r9, [%[b], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r11, [%[b], #28]\n\t"
#else
        "ldrd	r10, r11, [%[b], #24]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	r3, r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, r3, r12\n\t"
        "and	lr, r3, #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r]]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
#else
        "ldrd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[r], #8]\n\t"
        "ldr	r7, [%[r], #12]\n\t"
#else
        "ldrd	r6, r7, [%[r], #8]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, r3\n\t"
        "sbcs	r6, r6, r3\n\t"
        "sbcs	r7, r7, r3\n\t"
        "sbcs	r8, r8, r3\n\t"
        "sbcs	r9, r9, r3\n\t"
        "sbcs	r10, r10, r3\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
#else
        "strd	r6, r7, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
#else
        "strd	r8, r9, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #24]\n\t"
        "str	r11, [%[r], #28]\n\t"
#else
        "strd	r10, r11, [%[r], #24]\n\t"
#endif
        : [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
}

void fe_neg(fe r_p, const fe a_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        "mov	lr, #-1\n\t"
        "mov	r12, #-19\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a]]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
#else
        "ldrd	r2, r3, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
#else
        "ldrd	r4, r5, [%[a], #8]\n\t"
#endif
        "subs	r2, r12, r2\n\t"
        "sbcs	r3, lr, r3\n\t"
        "sbcs	r4, lr, r4\n\t"
        "sbcs	r5, lr, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[r]]\n\t"
        "str	r3, [%[r], #4]\n\t"
#else
        "strd	r2, r3, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #8]\n\t"
        "str	r5, [%[r], #12]\n\t"
#else
        "strd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r12, #0x7fffff\n\t"
        "lsl	r12, r12, #8\n\t"
        "add	r12, r12, #0xff\n\t"
#else
        "mov	r12, #0x7fffffff\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
#else
        "ldrd	r2, r3, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
#else
        "ldrd	r4, r5, [%[a], #24]\n\t"
#endif
        "sbcs	r2, lr, r2\n\t"
        "sbcs	r3, lr, r3\n\t"
        "sbcs	r4, lr, r4\n\t"
        "sbc	r5, r12, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[r], #16]\n\t"
        "str	r3, [%[r], #20]\n\t"
#else
        "strd	r2, r3, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #24]\n\t"
        "str	r5, [%[r], #28]\n\t"
#else
        "strd	r4, r5, [%[r], #24]\n\t"
#endif
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "r2", "r3", "r4", "r5", "r12", "lr"
    );
}

int fe_isnonzero(const fe a_p)
{
    register const fe a asm ("r0") = a_p;

    __asm__ __volatile__ (
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a]]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
#else
        "ldrd	r2, r3, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
#else
        "ldrd	r4, r5, [%[a], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
#else
        "ldrd	r6, r7, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[a], #28]\n\t"
#else
        "ldrd	r8, r9, [%[a], #24]\n\t"
#endif
        "adds	r1, r2, #19\n\t"
        "adcs	r1, r3, #0\n\t"
        "adcs	r1, r4, #0\n\t"
        "adcs	r1, r5, #0\n\t"
        "adcs	r1, r6, #0\n\t"
        "adcs	r1, r7, #0\n\t"
        "adcs	r1, r8, #0\n\t"
        "adc	r1, r9, #0\n\t"
        "asr	r1, r1, #31\n\t"
        "and	r1, r1, #19\n\t"
        "adds	r2, r2, r1\n\t"
        "adcs	r3, r3, #0\n\t"
        "adcs	r4, r4, #0\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adc	r9, r9, #0\n\t"
        "and	r9, r9, #0x7fffffff\n\t"
        "orr	r2, r2, r3\n\t"
        "orr	r4, r4, r5\n\t"
        "orr	r6, r6, r7\n\t"
        "orr	r8, r8, r9\n\t"
        "orr	r4, r4, r6\n\t"
        "orr	r2, r2, r8\n\t"
        "orr	%[a], r2, r4\n\t"
        : [a] "+r" (a)
        :
        : "memory", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r12"
    );
    return (uint32_t)(size_t)a;
}

int fe_isnegative(const fe a_p)
{
    register const fe a asm ("r0") = a_p;

    __asm__ __volatile__ (
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a]]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
#else
        "ldrd	r2, r3, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
#else
        "ldrd	r4, r5, [%[a], #8]\n\t"
#endif
        "adds	r1, r2, #19\n\t"
        "adcs	r1, r3, #0\n\t"
        "adcs	r1, r4, #0\n\t"
        "adcs	r1, r5, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
#else
        "ldrd	r2, r3, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
#else
        "ldrd	r4, r5, [%[a], #24]\n\t"
#endif
        "adcs	r1, r2, #0\n\t"
        "adcs	r1, r3, #0\n\t"
        "adcs	r1, r4, #0\n\t"
        "ldr	r2, [%[a]]\n\t"
        "adc	r1, r5, #0\n\t"
        "and	%[a], r2, #1\n\t"
        "lsr	r1, r1, #31\n\t"
        "eor	%[a], %[a], r1\n\t"
        : [a] "+r" (a)
        :
        : "memory", "r1", "r2", "r3", "r4", "r5"
    );
    return (uint32_t)(size_t)a;
}

void fe_cmov_table(fe* r_p, fe* base_p, signed char b_p)
{
    register fe* r asm ("r0") = r_p;
    register fe* base asm ("r1") = base_p;
    register signed char b asm ("r2") = b_p;

    __asm__ __volatile__ (
        "sxtb	%[b], %[b]\n\t"
        "sbfx	r3, %[b], #7, #1\n\t"
        "eor	r12, %[b], r3\n\t"
        "sub	r12, r12, r3\n\t"
        "mov	r4, #1\n\t"
        "mov	r5, #0\n\t"
        "mov	r6, #1\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r9, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #31\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #30\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #29\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #28\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #27\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #26\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #25\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #24\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base]]\n\t"
        "ldr	r11, [%[base], #4]\n\t"
#else
        "ldrd	r10, r11, [%[base]]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #32]\n\t"
        "ldr	r11, [%[base], #36]\n\t"
#else
        "ldrd	r10, r11, [%[base], #32]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #64]\n\t"
        "ldr	r11, [%[base], #68]\n\t"
#else
        "ldrd	r10, r11, [%[base], #64]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "sub	%[base], %[base], #0x2a0\n\t"
        "mov	r10, #-19\n\t"
        "mov	r11, #-1\n\t"
        "subs	r10, r10, r8\n\t"
        "sbcs	r11, r11, r9\n\t"
        "sbc	lr, lr, lr\n\t"
        "asr	r12, %[b], #31\n\t"
        "eor	r3, r4, r6\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r4, r4, r3\n\t"
        "eor	r6, r6, r3\n\t"
        "eor	r3, r5, r7\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r5, r5, r3\n\t"
        "eor	r7, r7, r3\n\t"
        "eor	r10, r10, r8\n\t"
        "and	r10, r10, r12\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r11, r11, r12\n\t"
        "eor	r9, r9, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #32]\n\t"
        "str	r7, [%[r], #36]\n\t"
#else
        "strd	r6, r7, [%[r], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #64]\n\t"
        "str	r9, [%[r], #68]\n\t"
#else
        "strd	r8, r9, [%[r], #64]\n\t"
#endif
        "sbfx	r3, %[b], #7, #1\n\t"
        "eor	r12, %[b], r3\n\t"
        "sub	r12, r12, r3\n\t"
        "mov	r4, #0\n\t"
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r9, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #31\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #30\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #29\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #28\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #27\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #26\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #25\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #24\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #8]\n\t"
        "ldr	r11, [%[base], #12]\n\t"
#else
        "ldrd	r10, r11, [%[base], #8]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #40]\n\t"
        "ldr	r11, [%[base], #44]\n\t"
#else
        "ldrd	r10, r11, [%[base], #40]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #72]\n\t"
        "ldr	r11, [%[base], #76]\n\t"
#else
        "ldrd	r10, r11, [%[base], #72]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "sub	%[base], %[base], #0x2a0\n\t"
        "mov	r10, #-1\n\t"
        "mov	r11, #-1\n\t"
        "rsbs	lr, lr, #0\n\t"
        "sbcs	r10, r10, r8\n\t"
        "sbcs	r11, r11, r9\n\t"
        "sbc	lr, lr, lr\n\t"
        "asr	r12, %[b], #31\n\t"
        "eor	r3, r4, r6\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r4, r4, r3\n\t"
        "eor	r6, r6, r3\n\t"
        "eor	r3, r5, r7\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r5, r5, r3\n\t"
        "eor	r7, r7, r3\n\t"
        "eor	r10, r10, r8\n\t"
        "and	r10, r10, r12\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r11, r11, r12\n\t"
        "eor	r9, r9, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #8]\n\t"
        "str	r5, [%[r], #12]\n\t"
#else
        "strd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
#else
        "strd	r6, r7, [%[r], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #72]\n\t"
        "str	r9, [%[r], #76]\n\t"
#else
        "strd	r8, r9, [%[r], #72]\n\t"
#endif
        "sbfx	r3, %[b], #7, #1\n\t"
        "eor	r12, %[b], r3\n\t"
        "sub	r12, r12, r3\n\t"
        "mov	r4, #0\n\t"
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r9, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #31\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #30\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #29\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #28\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #27\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #26\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #25\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #24\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #16]\n\t"
        "ldr	r11, [%[base], #20]\n\t"
#else
        "ldrd	r10, r11, [%[base], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #48]\n\t"
        "ldr	r11, [%[base], #52]\n\t"
#else
        "ldrd	r10, r11, [%[base], #48]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #80]\n\t"
        "ldr	r11, [%[base], #84]\n\t"
#else
        "ldrd	r10, r11, [%[base], #80]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "sub	%[base], %[base], #0x2a0\n\t"
        "mov	r10, #-1\n\t"
        "mov	r11, #-1\n\t"
        "rsbs	lr, lr, #0\n\t"
        "sbcs	r10, r10, r8\n\t"
        "sbcs	r11, r11, r9\n\t"
        "sbc	lr, lr, lr\n\t"
        "asr	r12, %[b], #31\n\t"
        "eor	r3, r4, r6\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r4, r4, r3\n\t"
        "eor	r6, r6, r3\n\t"
        "eor	r3, r5, r7\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r5, r5, r3\n\t"
        "eor	r7, r7, r3\n\t"
        "eor	r10, r10, r8\n\t"
        "and	r10, r10, r12\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r11, r11, r12\n\t"
        "eor	r9, r9, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
#else
        "strd	r4, r5, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #48]\n\t"
        "str	r7, [%[r], #52]\n\t"
#else
        "strd	r6, r7, [%[r], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #80]\n\t"
        "str	r9, [%[r], #84]\n\t"
#else
        "strd	r8, r9, [%[r], #80]\n\t"
#endif
        "sbfx	r3, %[b], #7, #1\n\t"
        "eor	r12, %[b], r3\n\t"
        "sub	r12, r12, r3\n\t"
        "mov	r4, #0\n\t"
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r9, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #31\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #30\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #29\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #28\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #27\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #26\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #25\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "add	%[base], %[base], #0x60\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r3, #0x800000\n\t"
        "lsl	r3, r3, #8\n\t"
        "add	r3, r3, #0x0\n\t"
#else
        "mov	r3, #0x80000000\n\t"
#endif
        "ror	r3, r3, #24\n\t"
        "ror	r3, r3, r12\n\t"
        "asr	r3, r3, #31\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #24]\n\t"
        "ldr	r11, [%[base], #28]\n\t"
#else
        "ldrd	r10, r11, [%[base], #24]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r4, r4, r10\n\t"
        "eor	r5, r5, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #56]\n\t"
        "ldr	r11, [%[base], #60]\n\t"
#else
        "ldrd	r10, r11, [%[base], #56]\n\t"
#endif
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r6, r6, r10\n\t"
        "eor	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[base], #88]\n\t"
        "ldr	r11, [%[base], #92]\n\t"
#else
        "ldrd	r10, r11, [%[base], #88]\n\t"
#endif
        "eor	r10, r10, r8\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r10, r10, r3\n\t"
        "and	r11, r11, r3\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r9, r9, r11\n\t"
        "sub	%[base], %[base], #0x2a0\n\t"
        "mov	r10, #-1\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "mov	r11, #0x7fffff\n\t"
        "lsl	r11, r11, #8\n\t"
        "add	r11, r11, #0xff\n\t"
#else
        "mov	r11, #0x7fffffff\n\t"
#endif
        "rsbs	lr, lr, #0\n\t"
        "sbcs	r10, r10, r8\n\t"
        "sbc	r11, r11, r9\n\t"
        "asr	r12, %[b], #31\n\t"
        "eor	r3, r4, r6\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r4, r4, r3\n\t"
        "eor	r6, r6, r3\n\t"
        "eor	r3, r5, r7\n\t"
        "and	r3, r3, r12\n\t"
        "eor	r5, r5, r3\n\t"
        "eor	r7, r7, r3\n\t"
        "eor	r10, r10, r8\n\t"
        "and	r10, r10, r12\n\t"
        "eor	r8, r8, r10\n\t"
        "eor	r11, r11, r9\n\t"
        "and	r11, r11, r12\n\t"
        "eor	r9, r9, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #24]\n\t"
        "str	r5, [%[r], #28]\n\t"
#else
        "strd	r4, r5, [%[r], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
#else
        "strd	r6, r7, [%[r], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #88]\n\t"
        "str	r9, [%[r], #92]\n\t"
#else
        "strd	r8, r9, [%[r], #88]\n\t"
#endif
        : [r] "+r" (r), [base] "+r" (base), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r3", "r10", "r11", "r12", "lr"
    );
}

void fe_mul(fe r_p, const fe a_p, const fe b_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;
    register const fe b asm ("r2") = b_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x40\n\t"
        /* Multiply */
        "ldr	r7, [%[a]]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b]]\n\t"
        "ldr	lr, [%[b], #4]\n\t"
        /*  A[0] * B[0] = 0 */
        "umull	r4, r5, r7, r9\n\t"
        "str	r4, [sp]\n\t"
        /*  A[0] * B[1] = 1 */
        "umull	r3, r6, r7, lr\n\t"
        "adds	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * B[0] = 1 */
        "umull	r3, r12, r8, r9\n\t"
        "adds	r5, r5, r3\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #4]\n\t"
        /*  A[2] * B[0] = 2 */
        "ldr	r10, [%[a], #8]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r6, r6, r3\n\t"
        "adc	r4, r4, r12\n\t"
        /*  A[1] * B[1] = 2 */
        "umull	r3, r12, r8, lr\n\t"
        "adds	r6, r6, r3\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[0] * B[2] = 2 */
        "ldr	r11, [%[b], #8]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #8]\n\t"
        /*  A[0] * B[3] = 3 */
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * B[2] = 3 */
        "ldr	r11, [%[b], #8]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[2] * B[1] = 3 */
        "umull	r3, r12, r10, lr\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[3] * B[0] = 3 */
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #12]\n\t"
        /*  A[4] * B[0] = 4 */
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r5, r5, r3\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * B[1] = 4 */
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r3, r12, r10, lr\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[2] * B[2] = 4 */
        "ldr	r10, [%[a], #8]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[1] * B[3] = 4 */
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[0] * B[4] = 4 */
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #16]\n\t"
        /*  A[0] * B[5] = 5 */
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[1] * B[4] = 5 */
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[2] * B[3] = 5 */
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[3] * B[2] = 5 */
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r11, [%[b], #8]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[4] * B[1] = 5 */
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r3, r12, r10, lr\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[5] * B[0] = 5 */
        "ldr	r10, [%[a], #20]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #20]\n\t"
        /*  A[6] * B[0] = 6 */
        "ldr	r10, [%[a], #24]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r4, r4, r3\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[5] * B[1] = 6 */
        "ldr	r10, [%[a], #20]\n\t"
        "umull	r3, r12, r10, lr\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[4] * B[2] = 6 */
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[3] * B[3] = 6 */
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[2] * B[4] = 6 */
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * B[5] = 6 */
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[0] * B[6] = 6 */
        "ldr	r11, [%[b], #24]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #24]\n\t"
        /*  A[0] * B[7] = 7 */
        "ldr	r11, [%[b], #28]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[1] * B[6] = 7 */
        "ldr	r11, [%[b], #24]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[2] * B[5] = 7 */
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * B[4] = 7 */
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[4] * B[3] = 7 */
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[5] * B[2] = 7 */
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r11, [%[b], #8]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[6] * B[1] = 7 */
        "ldr	r10, [%[a], #24]\n\t"
        "umull	r3, r12, r10, lr\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[7] * B[0] = 7 */
        "ldr	r10, [%[a], #28]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #28]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        /*  A[7] * B[1] = 8 */
        "umull	r3, r12, r10, lr\n\t"
        "adds	r6, r6, r3\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[6] * B[2] = 8 */
        "umull	r3, r12, r7, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[5] * B[3] = 8 */
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[4] * B[4] = 8 */
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[3] * B[5] = 8 */
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[2] * B[6] = 8 */
        "ldr	r10, [%[a], #8]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[1] * B[7] = 8 */
        "ldr	r11, [%[b], #28]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #32]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "mov	lr, r11\n\t"
        /*  A[2] * B[7] = 9 */
        "umull	r3, r12, r10, lr\n\t"
        "adds	r4, r4, r3\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[3] * B[6] = 9 */
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[4] * B[5] = 9 */
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[5] * B[4] = 9 */
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[6] * B[3] = 9 */
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[7] * B[2] = 9 */
        "ldr	r11, [%[b], #8]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #36]\n\t"
        /*  A[7] * B[3] = 10 */
        "ldr	r11, [%[b], #12]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[6] * B[4] = 10 */
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r7, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[5] * B[5] = 10 */
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r10, r11\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[4] * B[6] = 10 */
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * B[7] = 10 */
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r3, r12, r10, lr\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #40]\n\t"
        /*  A[4] * B[7] = 11 */
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r3, r12, r10, lr\n\t"
        "adds	r6, r6, r3\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[5] * B[6] = 11 */
        "ldr	r10, [%[a], #20]\n\t"
        "umull	r3, r12, r10, r9\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[6] * B[5] = 11 */
        "umull	r3, r12, r7, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[7] * B[4] = 11 */
        "ldr	r11, [%[b], #16]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r6, r6, r3\n\t"
        "adcs	r4, r4, r12\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #44]\n\t"
        /*  A[7] * B[5] = 12 */
        "ldr	r11, [%[b], #20]\n\t"
        "umull	r3, r12, r8, r11\n\t"
        "adds	r4, r4, r3\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[6] * B[6] = 12 */
        "umull	r3, r12, r7, r9\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[5] * B[7] = 12 */
        "umull	r3, r12, r10, lr\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #48]\n\t"
        /*  A[6] * B[7] = 13 */
        "umull	r3, r12, r7, lr\n\t"
        "adds	r5, r5, r3\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[7] * B[6] = 13 */
        "umull	r3, r12, r8, r9\n\t"
        "adds	r5, r5, r3\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #52]\n\t"
        /*  A[7] * B[7] = 14 */
        "umull	r3, r12, r8, lr\n\t"
        "adds	r6, r6, r3\n\t"
        "adc	r4, r4, r12\n\t"
        "str	r6, [sp, #56]\n\t"
        "str	r4, [sp, #60]\n\t"
        /* Reduce */
        /*  Load bottom half */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #8]\n\t"
        "ldr	r7, [sp, #12]\n\t"
#else
        "ldrd	r6, r7, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #16]\n\t"
        "ldr	r9, [sp, #20]\n\t"
#else
        "ldrd	r8, r9, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #24]\n\t"
        "ldr	r11, [sp, #28]\n\t"
#else
        "ldrd	r10, r11, [sp, #24]\n\t"
#endif
        "lsr	r3, r11, #31\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "mov	lr, #19\n\t"
        "ldr	%[a], [sp, #32]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "adds	r4, r4, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r5, r5, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #36]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "add	r12, r12, %[b]\n\t"
        "adds	r5, r5, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #40]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "add	r12, r12, %[b]\n\t"
        "adds	r6, r6, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r7, r7, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #44]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "add	r12, r12, %[b]\n\t"
        "adds	r7, r7, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r8, r8, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #48]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "add	r12, r12, %[b]\n\t"
        "adds	r8, r8, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r9, r9, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #52]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "add	r12, r12, %[b]\n\t"
        "adds	r9, r9, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r10, r10, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #56]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "add	r12, r12, %[b]\n\t"
        "adds	r10, r10, r3\n\t"
        "mov	%[b], #0\n\t"
        "adcs	r11, r11, r12\n\t"
        "adc	%[b], %[b], #0\n\t"
        "lsr	r3, %[a], #31\n\t"
        "ldr	%[a], [sp, #60]\n\t"
        "orr	r3, r3, %[a], lsl #1\n\t"
        "umull	r3, r12, lr, r3\n\t"
        "adds	r11, r11, r3\n\t"
        "adc	r3, r12, %[b]\n\t"
        /*  Overflow */
        "lsl	r3, r3, #1\n\t"
        "orr	r3, r3, r11, lsr #31\n\t"
        "mul	r3, r3, lr\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
        /* Reduce if top bit set */
        "asr	r3, r11, #31\n\t"
        "and	r3, r3, lr\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
        /* Store */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
#else
        "strd	r6, r7, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
#else
        "strd	r8, r9, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #24]\n\t"
        "str	r11, [%[r], #28]\n\t"
#else
        "strd	r10, r11, [%[r], #24]\n\t"
#endif
        "add	sp, sp, #0x40\n\t"
        : [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r12", "lr", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
    );
}

void fe_sq(fe r_p, const fe a_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x40\n\t"
        /* Square */
        "ldr	r7, [%[a]]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[a], #8]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r12, [%[a], #16]\n\t"
        /*  A[0] * A[0] = 0 */
        "umull	r4, r5, r7, r7\n\t"
        "str	r4, [sp]\n\t"
        /*  A[0] * A[1] = 1 */
        "umull	r2, r3, r7, r8\n\t"
        "mov	r6, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adc	r6, r6, r3\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #4]\n\t"
        /*  A[1] * A[1] = 2 */
        "umull	r2, r3, r8, r8\n\t"
        "adds	r6, r6, r2\n\t"
        "adc	r4, r4, r3\n\t"
        /*  A[0] * A[2] = 2 */
        "umull	r2, r3, r7, r9\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #8]\n\t"
        /*  A[0] * A[3] = 3 */
        "umull	r2, r3, r7, r10\n\t"
        "adds	r4, r4, r2\n\t"
        "adc	r5, r5, r3\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * A[2] = 3 */
        "umull	r2, r3, r8, r9\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #12]\n\t"
        /*  A[2] * A[2] = 4 */
        "umull	r2, r3, r9, r9\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[1] * A[3] = 4 */
        "umull	r2, r3, r8, r10\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[0] * A[4] = 4 */
        "umull	r2, r3, r7, r12\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #16]\n\t"
        /*  A[0] * A[5] = 5 */
        "ldr	r11, [%[a], #20]\n\t"
        "umull	r2, r3, r7, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[1] * A[4] = 5 */
        "umull	r2, r3, r8, r12\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[2] * A[3] = 5 */
        "umull	r2, r3, r9, r10\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #20]\n\t"
        /*  A[3] * A[3] = 6 */
        "umull	r2, r3, r10, r10\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[2] * A[4] = 6 */
        "umull	r2, r3, r9, r12\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * A[5] = 6 */
        "umull	r2, r3, r8, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[0] * A[6] = 6 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r7, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #24]\n\t"
        /*  A[0] * A[7] = 7 */
        "ldr	r11, [%[a], #28]\n\t"
        "umull	r2, r3, r7, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[1] * A[6] = 7 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r8, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[2] * A[5] = 7 */
        "ldr	r11, [%[a], #20]\n\t"
        "umull	r2, r3, r9, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * A[4] = 7 */
        "umull	r2, r3, r10, r12\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #28]\n\t"
        /*  A[4] * A[4] = 8 */
        "umull	r2, r3, r12, r12\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[3] * A[5] = 8 */
        "umull	r2, r3, r10, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[2] * A[6] = 8 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r9, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[1] * A[7] = 8 */
        "ldr	r11, [%[a], #28]\n\t"
        "umull	r2, r3, r8, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #32]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        /*  A[2] * A[7] = 9 */
        "umull	r2, r3, r9, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[3] * A[6] = 9 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r10, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[4] * A[5] = 9 */
        "umull	r2, r3, r12, r7\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #36]\n\t"
        "mov	r8, r11\n\t"
        /*  A[5] * A[5] = 10 */
        "umull	r2, r3, r7, r7\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[4] * A[6] = 10 */
        "umull	r2, r3, r12, r8\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * A[7] = 10 */
        "ldr	r11, [%[a], #28]\n\t"
        "umull	r2, r3, r10, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #40]\n\t"
        "mov	r9, r11\n\t"
        /*  A[4] * A[7] = 11 */
        "umull	r2, r3, r12, r9\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[5] * A[6] = 11 */
        "umull	r2, r3, r7, r8\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #44]\n\t"
        /*  A[6] * A[6] = 12 */
        "umull	r2, r3, r8, r8\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[5] * A[7] = 12 */
        "umull	r2, r3, r7, r9\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #48]\n\t"
        /*  A[6] * A[7] = 13 */
        "umull	r2, r3, r8, r9\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #52]\n\t"
        /*  A[7] * A[7] = 14 */
        "umull	r2, r3, r9, r9\n\t"
        "adds	r6, r6, r2\n\t"
        "adc	r4, r4, r3\n\t"
        "str	r6, [sp, #56]\n\t"
        "str	r4, [sp, #60]\n\t"
        /* Reduce */
        /*  Load bottom half */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #8]\n\t"
        "ldr	r7, [sp, #12]\n\t"
#else
        "ldrd	r6, r7, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #16]\n\t"
        "ldr	r9, [sp, #20]\n\t"
#else
        "ldrd	r8, r9, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #24]\n\t"
        "ldr	r11, [sp, #28]\n\t"
#else
        "ldrd	r10, r11, [sp, #24]\n\t"
#endif
        "lsr	r2, r11, #31\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "mov	r12, #19\n\t"
        "ldr	%[a], [sp, #32]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #36]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #40]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r7, r7, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #44]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r7, r7, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r8, r8, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #48]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r8, r8, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r9, r9, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #52]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r9, r9, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r10, r10, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #56]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r10, r10, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r11, r11, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #31\n\t"
        "ldr	%[a], [sp, #60]\n\t"
        "orr	r2, r2, %[a], lsl #1\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "adds	r11, r11, r2\n\t"
        "adc	r2, r3, lr\n\t"
        /*  Overflow */
        "lsl	r2, r2, #1\n\t"
        "orr	r2, r2, r11, lsr #31\n\t"
        "mul	r2, r2, r12\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
        /* Reduce if top bit set */
        "asr	r2, r11, #31\n\t"
        "and	r2, r2, r12\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
        /* Store */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
#else
        "strd	r6, r7, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
#else
        "strd	r8, r9, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #24]\n\t"
        "str	r11, [%[r], #28]\n\t"
#else
        "strd	r10, r11, [%[r], #24]\n\t"
#endif
        "add	sp, sp, #0x40\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "r2", "r3", "r12", "lr", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
    );
}

void fe_mul121666(fe r_p, fe a_p)
{
    register fe r asm ("r0") = r_p;
    register fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        /* Multiply by 121666 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r2, [%[a]]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
#else
        "ldrd	r2, r3, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
#else
        "ldrd	r4, r5, [%[a], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
#else
        "ldrd	r6, r7, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[a], #28]\n\t"
#else
        "ldrd	r8, r9, [%[a], #24]\n\t"
#endif
        "movw	lr, #0xdb42\n\t"
        "movt	lr, #1\n\t"
        "umull	r2, r10, r2, lr\n\t"
        "umull	r3, r12, r3, lr\n\t"
        "adds	r3, r3, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "umull	r4, r12, r4, lr\n\t"
        "adds	r4, r4, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "umull	r5, r12, r5, lr\n\t"
        "adds	r5, r5, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "umull	r6, r12, r6, lr\n\t"
        "adds	r6, r6, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "umull	r7, r12, r7, lr\n\t"
        "adds	r7, r7, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "umull	r8, r12, r8, lr\n\t"
        "adds	r8, r8, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "umull	r9, r12, r9, lr\n\t"
        "adds	r9, r9, r10\n\t"
        "adc	r10, r12, #0\n\t"
        "mov	lr, #19\n\t"
        "lsl	r10, r10, #1\n\t"
        "orr	r10, r10, r9, lsr #31\n\t"
        "mul	r10, r10, lr\n\t"
        "and	r9, r9, #0x7fffffff\n\t"
        "adds	r2, r2, r10\n\t"
        "adcs	r3, r3, #0\n\t"
        "adcs	r4, r4, #0\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adc	r9, r9, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r2, [%[r]]\n\t"
        "str	r3, [%[r], #4]\n\t"
#else
        "strd	r2, r3, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #8]\n\t"
        "str	r5, [%[r], #12]\n\t"
#else
        "strd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #16]\n\t"
        "str	r7, [%[r], #20]\n\t"
#else
        "strd	r6, r7, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #24]\n\t"
        "str	r9, [%[r], #28]\n\t"
#else
        "strd	r8, r9, [%[r], #24]\n\t"
#endif
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r12", "lr", "r10"
    );
}

void fe_sq2(fe r_p, const fe a_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x40\n\t"
        /* Square * 2 */
        "ldr	r7, [%[a]]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[a], #8]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r12, [%[a], #16]\n\t"
        /*  A[0] * A[0] = 0 */
        "umull	r4, r5, r7, r7\n\t"
        "str	r4, [sp]\n\t"
        /*  A[0] * A[1] = 1 */
        "umull	r2, r3, r7, r8\n\t"
        "mov	r6, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adc	r6, r6, r3\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #4]\n\t"
        /*  A[1] * A[1] = 2 */
        "umull	r2, r3, r8, r8\n\t"
        "adds	r6, r6, r2\n\t"
        "adc	r4, r4, r3\n\t"
        /*  A[0] * A[2] = 2 */
        "umull	r2, r3, r7, r9\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #8]\n\t"
        /*  A[0] * A[3] = 3 */
        "umull	r2, r3, r7, r10\n\t"
        "adds	r4, r4, r2\n\t"
        "adc	r5, r5, r3\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * A[2] = 3 */
        "umull	r2, r3, r8, r9\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #12]\n\t"
        /*  A[2] * A[2] = 4 */
        "umull	r2, r3, r9, r9\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[1] * A[3] = 4 */
        "umull	r2, r3, r8, r10\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[0] * A[4] = 4 */
        "umull	r2, r3, r7, r12\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #16]\n\t"
        /*  A[0] * A[5] = 5 */
        "ldr	r11, [%[a], #20]\n\t"
        "umull	r2, r3, r7, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[1] * A[4] = 5 */
        "umull	r2, r3, r8, r12\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[2] * A[3] = 5 */
        "umull	r2, r3, r9, r10\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #20]\n\t"
        /*  A[3] * A[3] = 6 */
        "umull	r2, r3, r10, r10\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[2] * A[4] = 6 */
        "umull	r2, r3, r9, r12\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[1] * A[5] = 6 */
        "umull	r2, r3, r8, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[0] * A[6] = 6 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r7, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #24]\n\t"
        /*  A[0] * A[7] = 7 */
        "ldr	r11, [%[a], #28]\n\t"
        "umull	r2, r3, r7, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[1] * A[6] = 7 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r8, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[2] * A[5] = 7 */
        "ldr	r11, [%[a], #20]\n\t"
        "umull	r2, r3, r9, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * A[4] = 7 */
        "umull	r2, r3, r10, r12\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #28]\n\t"
        /*  A[4] * A[4] = 8 */
        "umull	r2, r3, r12, r12\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[3] * A[5] = 8 */
        "umull	r2, r3, r10, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[2] * A[6] = 8 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r9, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[1] * A[7] = 8 */
        "ldr	r11, [%[a], #28]\n\t"
        "umull	r2, r3, r8, r11\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #32]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        /*  A[2] * A[7] = 9 */
        "umull	r2, r3, r9, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[3] * A[6] = 9 */
        "ldr	r11, [%[a], #24]\n\t"
        "umull	r2, r3, r10, r11\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[4] * A[5] = 9 */
        "umull	r2, r3, r12, r7\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #36]\n\t"
        "mov	r8, r11\n\t"
        /*  A[5] * A[5] = 10 */
        "umull	r2, r3, r7, r7\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[4] * A[6] = 10 */
        "umull	r2, r3, r12, r8\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        /*  A[3] * A[7] = 10 */
        "ldr	r11, [%[a], #28]\n\t"
        "umull	r2, r3, r10, r11\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #40]\n\t"
        "mov	r9, r11\n\t"
        /*  A[4] * A[7] = 11 */
        "umull	r2, r3, r12, r9\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	r5, #0\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        /*  A[5] * A[6] = 11 */
        "umull	r2, r3, r7, r8\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "adds	r6, r6, r2\n\t"
        "adcs	r4, r4, r3\n\t"
        "adc	r5, r5, #0\n\t"
        "str	r6, [sp, #44]\n\t"
        /*  A[6] * A[6] = 12 */
        "umull	r2, r3, r8, r8\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	r6, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        /*  A[5] * A[7] = 12 */
        "umull	r2, r3, r7, r9\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	r6, r6, #0\n\t"
        "str	r4, [sp, #48]\n\t"
        /*  A[6] * A[7] = 13 */
        "umull	r2, r3, r8, r9\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	r4, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "adds	r5, r5, r2\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	r4, r4, #0\n\t"
        "str	r5, [sp, #52]\n\t"
        /*  A[7] * A[7] = 14 */
        "umull	r2, r3, r9, r9\n\t"
        "adds	r6, r6, r2\n\t"
        "adc	r4, r4, r3\n\t"
        "str	r6, [sp, #56]\n\t"
        "str	r4, [sp, #60]\n\t"
        /* Double and Reduce */
        /*  Load bottom half */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #8]\n\t"
        "ldr	r7, [sp, #12]\n\t"
#else
        "ldrd	r6, r7, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #16]\n\t"
        "ldr	r9, [sp, #20]\n\t"
#else
        "ldrd	r8, r9, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #24]\n\t"
        "ldr	r11, [sp, #28]\n\t"
#else
        "ldrd	r10, r11, [sp, #24]\n\t"
#endif
        "lsr	r2, r11, #30\n\t"
        "lsl	r11, r11, #1\n\t"
        "orr	r11, r11, r10, lsr #31\n\t"
        "lsl	r10, r10, #1\n\t"
        "orr	r10, r10, r9, lsr #31\n\t"
        "lsl	r9, r9, #1\n\t"
        "orr	r9, r9, r8, lsr #31\n\t"
        "lsl	r8, r8, #1\n\t"
        "orr	r8, r8, r7, lsr #31\n\t"
        "lsl	r7, r7, #1\n\t"
        "orr	r7, r7, r6, lsr #31\n\t"
        "lsl	r6, r6, #1\n\t"
        "orr	r6, r6, r5, lsr #31\n\t"
        "lsl	r5, r5, #1\n\t"
        "orr	r5, r5, r4, lsr #31\n\t"
        "lsl	r4, r4, #1\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "mov	r12, #19\n\t"
        "ldr	%[a], [sp, #32]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "adds	r4, r4, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r5, r5, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #36]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r5, r5, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r6, r6, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #40]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r6, r6, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r7, r7, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #44]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r7, r7, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r8, r8, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #48]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r8, r8, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r9, r9, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #52]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r9, r9, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r10, r10, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #56]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "add	r3, r3, lr\n\t"
        "adds	r10, r10, r2\n\t"
        "mov	lr, #0\n\t"
        "adcs	r11, r11, r3\n\t"
        "adc	lr, lr, #0\n\t"
        "lsr	r2, %[a], #30\n\t"
        "ldr	%[a], [sp, #60]\n\t"
        "orr	r2, r2, %[a], lsl #2\n\t"
        "umull	r2, r3, r12, r2\n\t"
        "adds	r11, r11, r2\n\t"
        "adc	r2, r3, lr\n\t"
        /*  Overflow */
        "lsl	r2, r2, #1\n\t"
        "orr	r2, r2, r11, lsr #31\n\t"
        "mul	r2, r2, r12\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
        /* Reduce if top bit set */
        "asr	r2, r11, #31\n\t"
        "and	r2, r2, r12\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, r2\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
        /* Store */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
#else
        "strd	r6, r7, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
#else
        "strd	r8, r9, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #24]\n\t"
        "str	r11, [%[r], #28]\n\t"
#else
        "strd	r10, r11, [%[r], #24]\n\t"
#endif
        "add	sp, sp, #0x40\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "r2", "r3", "r12", "lr", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
    );
}

void fe_invert(fe r_p, const fe a_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x88\n\t"
        /* Invert */
        "str	%[r], [sp, #128]\n\t"
        "str	%[a], [sp, #132]\n\t"
        "mov	r0, sp\n\t"
        "ldr	r1, [sp, #132]\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "ldr	r1, [sp, #132]\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "mov	r0, sp\n\t"
        "mov	r1, sp\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x40\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #4\n\t"
        "\n"
    "L_fe_invert1_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert1_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #9\n\t"
        "\n"
    "L_fe_invert2_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert2_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #19\n\t"
        "\n"
    "L_fe_invert3_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert3_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #10\n\t"
        "\n"
    "L_fe_invert4_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert4_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #49\n\t"
        "\n"
    "L_fe_invert5_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert5_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #0x63\n\t"
        "\n"
    "L_fe_invert6_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert6_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #50\n\t"
        "\n"
    "L_fe_invert7_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert7_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #5\n\t"
        "\n"
    "L_fe_invert8_%=: \n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_invert8_%=\n\t"
        "ldr	r0, [sp, #128]\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "ldr	%[a], [sp, #132]\n\t"
        "ldr	%[r], [sp, #128]\n\t"
        "add	sp, sp, #0x88\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "lr", "r4"
    );
}

int curve25519(byte* r_p, const byte* n_p, const byte* a_p)
{
    register byte* r asm ("r0") = r_p;
    register const byte* n asm ("r1") = n_p;
    register const byte* a asm ("r2") = a_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0xbc\n\t"
        "str	%[r], [sp, #160]\n\t"
        "str	%[n], [sp, #164]\n\t"
        "str	%[a], [sp, #168]\n\t"
        "mov	%[n], #0\n\t"
        "str	%[n], [sp, #172]\n\t"
        /* Set one */
        "mov	r10, #1\n\t"
        "mov	r11, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r]]\n\t"
        "str	r11, [%[r], #4]\n\t"
#else
        "strd	r10, r11, [%[r]]\n\t"
#endif
        "mov	r10, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #8]\n\t"
        "str	r11, [%[r], #12]\n\t"
#else
        "strd	r10, r11, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #16]\n\t"
        "str	r11, [%[r], #20]\n\t"
#else
        "strd	r10, r11, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [%[r], #24]\n\t"
        "str	r11, [%[r], #28]\n\t"
#else
        "strd	r10, r11, [%[r], #24]\n\t"
#endif
        /* Set zero */
        "mov	r10, #0\n\t"
        "mov	r11, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp]\n\t"
        "str	r11, [sp, #4]\n\t"
#else
        "strd	r10, r11, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #8]\n\t"
        "str	r11, [sp, #12]\n\t"
#else
        "strd	r10, r11, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #16]\n\t"
        "str	r11, [sp, #20]\n\t"
#else
        "strd	r10, r11, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #24]\n\t"
        "str	r11, [sp, #28]\n\t"
#else
        "strd	r10, r11, [sp, #24]\n\t"
#endif
        /* Set one */
        "mov	r10, #1\n\t"
        "mov	r11, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #32]\n\t"
        "str	r11, [sp, #36]\n\t"
#else
        "strd	r10, r11, [sp, #32]\n\t"
#endif
        "mov	r10, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #40]\n\t"
        "str	r11, [sp, #44]\n\t"
#else
        "strd	r10, r11, [sp, #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #48]\n\t"
        "str	r11, [sp, #52]\n\t"
#else
        "strd	r10, r11, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #56]\n\t"
        "str	r11, [sp, #60]\n\t"
#else
        "strd	r10, r11, [sp, #56]\n\t"
#endif
        /* Copy */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a]]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
#else
        "ldrd	r4, r5, [%[a]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
#else
        "ldrd	r6, r7, [%[a], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #64]\n\t"
        "str	r5, [sp, #68]\n\t"
#else
        "strd	r4, r5, [sp, #64]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #72]\n\t"
        "str	r7, [sp, #76]\n\t"
#else
        "strd	r6, r7, [sp, #72]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
#else
        "ldrd	r4, r5, [%[a], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
#else
        "ldrd	r6, r7, [%[a], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #80]\n\t"
        "str	r5, [sp, #84]\n\t"
#else
        "strd	r4, r5, [sp, #80]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #88]\n\t"
        "str	r7, [sp, #92]\n\t"
#else
        "strd	r6, r7, [sp, #88]\n\t"
#endif
        "mov	%[n], #30\n\t"
        "str	%[n], [sp, #180]\n\t"
        "mov	%[a], #28\n\t"
        "str	%[a], [sp, #176]\n\t"
        "\n"
    "L_curve25519_words_%=: \n\t"
        "\n"
    "L_curve25519_bits_%=: \n\t"
        "ldr	%[n], [sp, #164]\n\t"
        "ldr	%[a], [%[n], r2]\n\t"
        "ldr	%[n], [sp, #180]\n\t"
        "lsr	%[a], %[a], %[n]\n\t"
        "and	%[a], %[a], #1\n\t"
        "str	%[a], [sp, #184]\n\t"
        "ldr	%[n], [sp, #172]\n\t"
        "eor	%[n], %[n], %[a]\n\t"
        "str	%[n], [sp, #172]\n\t"
        "ldr	%[r], [sp, #160]\n\t"
        /* Conditional Swap */
        "neg	%[n], %[n]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r]]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
#else
        "ldrd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #64]\n\t"
        "ldr	r7, [sp, #68]\n\t"
#else
        "ldrd	r6, r7, [sp, #64]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #64]\n\t"
        "str	r7, [sp, #68]\n\t"
#else
        "strd	r6, r7, [sp, #64]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #8]\n\t"
        "ldr	r5, [%[r], #12]\n\t"
#else
        "ldrd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #72]\n\t"
        "ldr	r7, [sp, #76]\n\t"
#else
        "ldrd	r6, r7, [sp, #72]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #8]\n\t"
        "str	r5, [%[r], #12]\n\t"
#else
        "strd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #72]\n\t"
        "str	r7, [sp, #76]\n\t"
#else
        "strd	r6, r7, [sp, #72]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
#else
        "ldrd	r4, r5, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #80]\n\t"
        "ldr	r7, [sp, #84]\n\t"
#else
        "ldrd	r6, r7, [sp, #80]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
#else
        "strd	r4, r5, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #80]\n\t"
        "str	r7, [sp, #84]\n\t"
#else
        "strd	r6, r7, [sp, #80]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #24]\n\t"
        "ldr	r5, [%[r], #28]\n\t"
#else
        "ldrd	r4, r5, [%[r], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #88]\n\t"
        "ldr	r7, [sp, #92]\n\t"
#else
        "ldrd	r6, r7, [sp, #88]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #24]\n\t"
        "str	r5, [%[r], #28]\n\t"
#else
        "strd	r4, r5, [%[r], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #88]\n\t"
        "str	r7, [sp, #92]\n\t"
#else
        "strd	r6, r7, [sp, #88]\n\t"
#endif
        "ldr	%[n], [sp, #172]\n\t"
        /* Conditional Swap */
        "neg	%[n], %[n]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #32]\n\t"
        "ldr	r7, [sp, #36]\n\t"
#else
        "ldrd	r6, r7, [sp, #32]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp]\n\t"
        "str	r5, [sp, #4]\n\t"
#else
        "strd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #32]\n\t"
        "str	r7, [sp, #36]\n\t"
#else
        "strd	r6, r7, [sp, #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #8]\n\t"
        "ldr	r5, [sp, #12]\n\t"
#else
        "ldrd	r4, r5, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #40]\n\t"
        "ldr	r7, [sp, #44]\n\t"
#else
        "ldrd	r6, r7, [sp, #40]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #8]\n\t"
        "str	r5, [sp, #12]\n\t"
#else
        "strd	r4, r5, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #40]\n\t"
        "str	r7, [sp, #44]\n\t"
#else
        "strd	r6, r7, [sp, #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #16]\n\t"
        "ldr	r5, [sp, #20]\n\t"
#else
        "ldrd	r4, r5, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #48]\n\t"
        "ldr	r7, [sp, #52]\n\t"
#else
        "ldrd	r6, r7, [sp, #48]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #16]\n\t"
        "str	r5, [sp, #20]\n\t"
#else
        "strd	r4, r5, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #48]\n\t"
        "str	r7, [sp, #52]\n\t"
#else
        "strd	r6, r7, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #24]\n\t"
        "ldr	r5, [sp, #28]\n\t"
#else
        "ldrd	r4, r5, [sp, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #56]\n\t"
        "ldr	r7, [sp, #60]\n\t"
#else
        "ldrd	r6, r7, [sp, #56]\n\t"
#endif
        "eor	r8, r4, r6\n\t"
        "eor	r9, r5, r7\n\t"
        "and	r8, r8, %[n]\n\t"
        "and	r9, r9, %[n]\n\t"
        "eor	r4, r4, r8\n\t"
        "eor	r5, r5, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #24]\n\t"
        "str	r5, [sp, #28]\n\t"
#else
        "strd	r4, r5, [sp, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #56]\n\t"
        "str	r7, [sp, #60]\n\t"
#else
        "strd	r6, r7, [sp, #56]\n\t"
#endif
        "ldr	%[n], [sp, #184]\n\t"
        "str	%[n], [sp, #172]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r]]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
#else
        "ldrd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp]\n\t"
        "ldr	r7, [sp, #4]\n\t"
#else
        "ldrd	r6, r7, [sp]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r]]\n\t"
        "str	r9, [%[r], #4]\n\t"
#else
        "strd	r8, r9, [%[r]]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #128]\n\t"
        "str	r11, [sp, #132]\n\t"
#else
        "strd	r10, r11, [sp, #128]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #8]\n\t"
        "ldr	r5, [%[r], #12]\n\t"
#else
        "ldrd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #8]\n\t"
        "ldr	r7, [sp, #12]\n\t"
#else
        "ldrd	r6, r7, [sp, #8]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #8]\n\t"
        "str	r9, [%[r], #12]\n\t"
#else
        "strd	r8, r9, [%[r], #8]\n\t"
#endif
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #136]\n\t"
        "str	r11, [sp, #140]\n\t"
#else
        "strd	r10, r11, [sp, #136]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
#else
        "ldrd	r4, r5, [%[r], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #16]\n\t"
        "ldr	r7, [sp, #20]\n\t"
#else
        "ldrd	r6, r7, [sp, #16]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
#else
        "strd	r8, r9, [%[r], #16]\n\t"
#endif
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #144]\n\t"
        "str	r11, [sp, #148]\n\t"
#else
        "strd	r10, r11, [sp, #144]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #24]\n\t"
        "ldr	r5, [%[r], #28]\n\t"
#else
        "ldrd	r4, r5, [%[r], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #24]\n\t"
        "ldr	r7, [sp, #28]\n\t"
#else
        "ldrd	r6, r7, [sp, #24]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r3, #-19\n\t"
        "asr	%[a], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r]]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
#else
        "ldrd	r4, r5, [%[r]]\n\t"
#endif
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r]]\n\t"
        "str	r5, [%[r], #4]\n\t"
#else
        "strd	r4, r5, [%[r]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #8]\n\t"
        "ldr	r5, [%[r], #12]\n\t"
#else
        "ldrd	r4, r5, [%[r], #8]\n\t"
#endif
        "sbcs	r4, r4, %[a]\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #8]\n\t"
        "str	r5, [%[r], #12]\n\t"
#else
        "strd	r4, r5, [%[r], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
#else
        "ldrd	r4, r5, [%[r], #16]\n\t"
#endif
        "sbcs	r4, r4, %[a]\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
#else
        "strd	r4, r5, [%[r], #16]\n\t"
#endif
        "sbcs	r8, r8, %[a]\n\t"
        "sbc	r9, r9, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[r], #24]\n\t"
        "str	r9, [%[r], #28]\n\t"
#else
        "strd	r8, r9, [%[r], #24]\n\t"
#endif
        "mov	r3, #-19\n\t"
        "asr	%[a], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #128]\n\t"
        "ldr	r5, [sp, #132]\n\t"
#else
        "ldrd	r4, r5, [sp, #128]\n\t"
#endif
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #128]\n\t"
        "str	r5, [sp, #132]\n\t"
#else
        "strd	r4, r5, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #136]\n\t"
        "ldr	r5, [sp, #140]\n\t"
#else
        "ldrd	r4, r5, [sp, #136]\n\t"
#endif
        "adcs	r4, r4, %[a]\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #136]\n\t"
        "str	r5, [sp, #140]\n\t"
#else
        "strd	r4, r5, [sp, #136]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #144]\n\t"
        "ldr	r5, [sp, #148]\n\t"
#else
        "ldrd	r4, r5, [sp, #144]\n\t"
#endif
        "adcs	r4, r4, %[a]\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #144]\n\t"
        "str	r5, [sp, #148]\n\t"
#else
        "strd	r4, r5, [sp, #144]\n\t"
#endif
        "adcs	r10, r10, %[a]\n\t"
        "adc	r11, r11, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #152]\n\t"
        "str	r11, [sp, #156]\n\t"
#else
        "strd	r10, r11, [sp, #152]\n\t"
#endif
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #64]\n\t"
        "ldr	r5, [sp, #68]\n\t"
#else
        "ldrd	r4, r5, [sp, #64]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #32]\n\t"
        "ldr	r7, [sp, #36]\n\t"
#else
        "ldrd	r6, r7, [sp, #32]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp]\n\t"
        "str	r9, [sp, #4]\n\t"
#else
        "strd	r8, r9, [sp]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #96]\n\t"
        "str	r11, [sp, #100]\n\t"
#else
        "strd	r10, r11, [sp, #96]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #72]\n\t"
        "ldr	r5, [sp, #76]\n\t"
#else
        "ldrd	r4, r5, [sp, #72]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #40]\n\t"
        "ldr	r7, [sp, #44]\n\t"
#else
        "ldrd	r6, r7, [sp, #40]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #8]\n\t"
        "str	r9, [sp, #12]\n\t"
#else
        "strd	r8, r9, [sp, #8]\n\t"
#endif
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #104]\n\t"
        "str	r11, [sp, #108]\n\t"
#else
        "strd	r10, r11, [sp, #104]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #80]\n\t"
        "ldr	r5, [sp, #84]\n\t"
#else
        "ldrd	r4, r5, [sp, #80]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #48]\n\t"
        "ldr	r7, [sp, #52]\n\t"
#else
        "ldrd	r6, r7, [sp, #48]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #16]\n\t"
        "str	r9, [sp, #20]\n\t"
#else
        "strd	r8, r9, [sp, #16]\n\t"
#endif
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #112]\n\t"
        "str	r11, [sp, #116]\n\t"
#else
        "strd	r10, r11, [sp, #112]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #88]\n\t"
        "ldr	r5, [sp, #92]\n\t"
#else
        "ldrd	r4, r5, [sp, #88]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #56]\n\t"
        "ldr	r7, [sp, #60]\n\t"
#else
        "ldrd	r6, r7, [sp, #56]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r3, #-19\n\t"
        "asr	%[a], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp]\n\t"
        "str	r5, [sp, #4]\n\t"
#else
        "strd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #8]\n\t"
        "ldr	r5, [sp, #12]\n\t"
#else
        "ldrd	r4, r5, [sp, #8]\n\t"
#endif
        "sbcs	r4, r4, %[a]\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #8]\n\t"
        "str	r5, [sp, #12]\n\t"
#else
        "strd	r4, r5, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #16]\n\t"
        "ldr	r5, [sp, #20]\n\t"
#else
        "ldrd	r4, r5, [sp, #16]\n\t"
#endif
        "sbcs	r4, r4, %[a]\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #16]\n\t"
        "str	r5, [sp, #20]\n\t"
#else
        "strd	r4, r5, [sp, #16]\n\t"
#endif
        "sbcs	r8, r8, %[a]\n\t"
        "sbc	r9, r9, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #24]\n\t"
        "str	r9, [sp, #28]\n\t"
#else
        "strd	r8, r9, [sp, #24]\n\t"
#endif
        "mov	r3, #-19\n\t"
        "asr	%[a], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #96]\n\t"
        "str	r5, [sp, #100]\n\t"
#else
        "strd	r4, r5, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #104]\n\t"
        "ldr	r5, [sp, #108]\n\t"
#else
        "ldrd	r4, r5, [sp, #104]\n\t"
#endif
        "adcs	r4, r4, %[a]\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #104]\n\t"
        "str	r5, [sp, #108]\n\t"
#else
        "strd	r4, r5, [sp, #104]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #112]\n\t"
        "ldr	r5, [sp, #116]\n\t"
#else
        "ldrd	r4, r5, [sp, #112]\n\t"
#endif
        "adcs	r4, r4, %[a]\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #112]\n\t"
        "str	r5, [sp, #116]\n\t"
#else
        "strd	r4, r5, [sp, #112]\n\t"
#endif
        "adcs	r10, r10, %[a]\n\t"
        "adc	r11, r11, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #120]\n\t"
        "str	r11, [sp, #124]\n\t"
#else
        "strd	r10, r11, [sp, #120]\n\t"
#endif
        "ldr	r2, [sp, #160]\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r0, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r2, sp, #0x80\n\t"
        "add	r1, sp, #0\n\t"
        "add	r0, sp, #0\n\t"
        "bl	fe_mul\n\t"
        "add	r1, sp, #0x80\n\t"
        "add	r0, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "ldr	r1, [sp, #160]\n\t"
        "add	r0, sp, #0x80\n\t"
        "bl	fe_sq\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #32]\n\t"
        "ldr	r5, [sp, #36]\n\t"
#else
        "ldrd	r4, r5, [sp, #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp]\n\t"
        "ldr	r7, [sp, #4]\n\t"
#else
        "ldrd	r6, r7, [sp]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #64]\n\t"
        "str	r9, [sp, #68]\n\t"
#else
        "strd	r8, r9, [sp, #64]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp]\n\t"
        "str	r11, [sp, #4]\n\t"
#else
        "strd	r10, r11, [sp]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #40]\n\t"
        "ldr	r5, [sp, #44]\n\t"
#else
        "ldrd	r4, r5, [sp, #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #8]\n\t"
        "ldr	r7, [sp, #12]\n\t"
#else
        "ldrd	r6, r7, [sp, #8]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #72]\n\t"
        "str	r9, [sp, #76]\n\t"
#else
        "strd	r8, r9, [sp, #72]\n\t"
#endif
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #8]\n\t"
        "str	r11, [sp, #12]\n\t"
#else
        "strd	r10, r11, [sp, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #48]\n\t"
        "ldr	r5, [sp, #52]\n\t"
#else
        "ldrd	r4, r5, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #16]\n\t"
        "ldr	r7, [sp, #20]\n\t"
#else
        "ldrd	r6, r7, [sp, #16]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r3, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r3, r3, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #80]\n\t"
        "str	r9, [sp, #84]\n\t"
#else
        "strd	r8, r9, [sp, #80]\n\t"
#endif
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #16]\n\t"
        "str	r11, [sp, #20]\n\t"
#else
        "strd	r10, r11, [sp, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #56]\n\t"
        "ldr	r5, [sp, #60]\n\t"
#else
        "ldrd	r4, r5, [sp, #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #24]\n\t"
        "ldr	r7, [sp, #28]\n\t"
#else
        "ldrd	r6, r7, [sp, #24]\n\t"
#endif
        "adds	r3, r3, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	r12, r12, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r3, #-19\n\t"
        "asr	%[a], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #64]\n\t"
        "ldr	r5, [sp, #68]\n\t"
#else
        "ldrd	r4, r5, [sp, #64]\n\t"
#endif
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #64]\n\t"
        "str	r5, [sp, #68]\n\t"
#else
        "strd	r4, r5, [sp, #64]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #72]\n\t"
        "ldr	r5, [sp, #76]\n\t"
#else
        "ldrd	r4, r5, [sp, #72]\n\t"
#endif
        "sbcs	r4, r4, %[a]\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #72]\n\t"
        "str	r5, [sp, #76]\n\t"
#else
        "strd	r4, r5, [sp, #72]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #80]\n\t"
        "ldr	r5, [sp, #84]\n\t"
#else
        "ldrd	r4, r5, [sp, #80]\n\t"
#endif
        "sbcs	r4, r4, %[a]\n\t"
        "sbcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #80]\n\t"
        "str	r5, [sp, #84]\n\t"
#else
        "strd	r4, r5, [sp, #80]\n\t"
#endif
        "sbcs	r8, r8, %[a]\n\t"
        "sbc	r9, r9, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #88]\n\t"
        "str	r9, [sp, #92]\n\t"
#else
        "strd	r8, r9, [sp, #88]\n\t"
#endif
        "mov	r3, #-19\n\t"
        "asr	%[a], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp]\n\t"
        "str	r5, [sp, #4]\n\t"
#else
        "strd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #8]\n\t"
        "ldr	r5, [sp, #12]\n\t"
#else
        "ldrd	r4, r5, [sp, #8]\n\t"
#endif
        "adcs	r4, r4, %[a]\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #8]\n\t"
        "str	r5, [sp, #12]\n\t"
#else
        "strd	r4, r5, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #16]\n\t"
        "ldr	r5, [sp, #20]\n\t"
#else
        "ldrd	r4, r5, [sp, #16]\n\t"
#endif
        "adcs	r4, r4, %[a]\n\t"
        "adcs	r5, r5, %[a]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #16]\n\t"
        "str	r5, [sp, #20]\n\t"
#else
        "strd	r4, r5, [sp, #16]\n\t"
#endif
        "adcs	r10, r10, %[a]\n\t"
        "adc	r11, r11, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #24]\n\t"
        "str	r11, [sp, #28]\n\t"
#else
        "strd	r10, r11, [sp, #24]\n\t"
#endif
        "add	r2, sp, #0x60\n\t"
        "add	r1, sp, #0x80\n\t"
        "ldr	r0, [sp, #160]\n\t"
        "bl	fe_mul\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #128]\n\t"
        "ldr	r5, [sp, #132]\n\t"
#else
        "ldrd	r4, r5, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #136]\n\t"
        "ldr	r7, [sp, #140]\n\t"
#else
        "ldrd	r6, r7, [sp, #136]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #96]\n\t"
        "ldr	r9, [sp, #100]\n\t"
#else
        "ldrd	r8, r9, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #104]\n\t"
        "ldr	r11, [sp, #108]\n\t"
#else
        "ldrd	r10, r11, [sp, #104]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #128]\n\t"
        "str	r9, [sp, #132]\n\t"
#else
        "strd	r8, r9, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #136]\n\t"
        "str	r11, [sp, #140]\n\t"
#else
        "strd	r10, r11, [sp, #136]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #144]\n\t"
        "ldr	r5, [sp, #148]\n\t"
#else
        "ldrd	r4, r5, [sp, #144]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #152]\n\t"
        "ldr	r7, [sp, #156]\n\t"
#else
        "ldrd	r6, r7, [sp, #152]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #112]\n\t"
        "ldr	r9, [sp, #116]\n\t"
#else
        "ldrd	r8, r9, [sp, #112]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #120]\n\t"
        "ldr	r11, [sp, #124]\n\t"
#else
        "ldrd	r10, r11, [sp, #120]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r3, #-19\n\t"
        "asr	%[a], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #128]\n\t"
        "ldr	r5, [sp, #132]\n\t"
#else
        "ldrd	r4, r5, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #136]\n\t"
        "ldr	r7, [sp, #140]\n\t"
#else
        "ldrd	r6, r7, [sp, #136]\n\t"
#endif
        "adds	r4, r4, r3\n\t"
        "adcs	r5, r5, %[a]\n\t"
        "adcs	r6, r6, %[a]\n\t"
        "adcs	r7, r7, %[a]\n\t"
        "adcs	r8, r8, %[a]\n\t"
        "adcs	r9, r9, %[a]\n\t"
        "adcs	r10, r10, %[a]\n\t"
        "adc	r11, r11, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #128]\n\t"
        "str	r5, [sp, #132]\n\t"
#else
        "strd	r4, r5, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #136]\n\t"
        "str	r7, [sp, #140]\n\t"
#else
        "strd	r6, r7, [sp, #136]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #144]\n\t"
        "str	r9, [sp, #148]\n\t"
#else
        "strd	r8, r9, [sp, #144]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #152]\n\t"
        "str	r11, [sp, #156]\n\t"
#else
        "strd	r10, r11, [sp, #152]\n\t"
#endif
        "add	r1, sp, #0\n\t"
        "add	r0, sp, #0\n\t"
        "bl	fe_sq\n\t"
        /* Multiply by 121666 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #128]\n\t"
        "ldr	r5, [sp, #132]\n\t"
#else
        "ldrd	r4, r5, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #136]\n\t"
        "ldr	r7, [sp, #140]\n\t"
#else
        "ldrd	r6, r7, [sp, #136]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #144]\n\t"
        "ldr	r9, [sp, #148]\n\t"
#else
        "ldrd	r8, r9, [sp, #144]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #152]\n\t"
        "ldr	r11, [sp, #156]\n\t"
#else
        "ldrd	r10, r11, [sp, #152]\n\t"
#endif
        "movw	r12, #0xdb42\n\t"
        "movt	r12, #1\n\t"
        "umull	r4, %[a], r4, r12\n\t"
        "umull	r5, r3, r5, r12\n\t"
        "adds	r5, r5, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "umull	r6, r3, r6, r12\n\t"
        "adds	r6, r6, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "umull	r7, r3, r7, r12\n\t"
        "adds	r7, r7, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "umull	r8, r3, r8, r12\n\t"
        "adds	r8, r8, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "umull	r9, r3, r9, r12\n\t"
        "adds	r9, r9, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "umull	r10, r3, r10, r12\n\t"
        "adds	r10, r10, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "umull	r11, r3, r11, r12\n\t"
        "adds	r11, r11, %[a]\n\t"
        "adc	%[a], r3, #0\n\t"
        "mov	r12, #19\n\t"
        "lsl	%[a], %[a], #1\n\t"
        "orr	%[a], %[a], r11, lsr #31\n\t"
        "mul	%[a], %[a], r12\n\t"
        "and	r11, r11, #0x7fffffff\n\t"
        "adds	r4, r4, %[a]\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adcs	r7, r7, #0\n\t"
        "adcs	r8, r8, #0\n\t"
        "adcs	r9, r9, #0\n\t"
        "adcs	r10, r10, #0\n\t"
        "adc	r11, r11, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #32]\n\t"
        "str	r5, [sp, #36]\n\t"
#else
        "strd	r4, r5, [sp, #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #40]\n\t"
        "str	r7, [sp, #44]\n\t"
#else
        "strd	r6, r7, [sp, #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #48]\n\t"
        "str	r9, [sp, #52]\n\t"
#else
        "strd	r8, r9, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #56]\n\t"
        "str	r11, [sp, #60]\n\t"
#else
        "strd	r10, r11, [sp, #56]\n\t"
#endif
        "add	r1, sp, #0x40\n\t"
        "add	r0, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #104]\n\t"
        "ldr	r7, [sp, #108]\n\t"
#else
        "ldrd	r6, r7, [sp, #104]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #32]\n\t"
        "ldr	r9, [sp, #36]\n\t"
#else
        "ldrd	r8, r9, [sp, #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #40]\n\t"
        "ldr	r11, [sp, #44]\n\t"
#else
        "ldrd	r10, r11, [sp, #40]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #96]\n\t"
        "str	r9, [sp, #100]\n\t"
#else
        "strd	r8, r9, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #104]\n\t"
        "str	r11, [sp, #108]\n\t"
#else
        "strd	r10, r11, [sp, #104]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #112]\n\t"
        "ldr	r5, [sp, #116]\n\t"
#else
        "ldrd	r4, r5, [sp, #112]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #120]\n\t"
        "ldr	r7, [sp, #124]\n\t"
#else
        "ldrd	r6, r7, [sp, #120]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #48]\n\t"
        "ldr	r9, [sp, #52]\n\t"
#else
        "ldrd	r8, r9, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #56]\n\t"
        "ldr	r11, [sp, #60]\n\t"
#else
        "ldrd	r10, r11, [sp, #56]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r3, #-19\n\t"
        "asr	%[a], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r3, %[a], r3\n\t"
        "and	r12, %[a], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [sp, #104]\n\t"
        "ldr	r7, [sp, #108]\n\t"
#else
        "ldrd	r6, r7, [sp, #104]\n\t"
#endif
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, %[a]\n\t"
        "sbcs	r6, r6, %[a]\n\t"
        "sbcs	r7, r7, %[a]\n\t"
        "sbcs	r8, r8, %[a]\n\t"
        "sbcs	r9, r9, %[a]\n\t"
        "sbcs	r10, r10, %[a]\n\t"
        "sbc	r11, r11, r12\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #96]\n\t"
        "str	r5, [sp, #100]\n\t"
#else
        "strd	r4, r5, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #104]\n\t"
        "str	r7, [sp, #108]\n\t"
#else
        "strd	r6, r7, [sp, #104]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #112]\n\t"
        "str	r9, [sp, #116]\n\t"
#else
        "strd	r8, r9, [sp, #112]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #120]\n\t"
        "str	r11, [sp, #124]\n\t"
#else
        "strd	r10, r11, [sp, #120]\n\t"
#endif
        "add	r2, sp, #0\n\t"
        "ldr	r1, [sp, #168]\n\t"
        "add	r0, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r2, sp, #0x60\n\t"
        "add	r1, sp, #0x80\n\t"
        "add	r0, sp, #0\n\t"
        "bl	fe_mul\n\t"
        "ldr	%[a], [sp, #176]\n\t"
        "ldr	%[n], [sp, #180]\n\t"
        "subs	%[n], %[n], #1\n\t"
        "str	%[n], [sp, #180]\n\t"
        "bge	L_curve25519_bits_%=\n\t"
        "mov	%[n], #31\n\t"
        "str	%[n], [sp, #180]\n\t"
        "subs	%[a], %[a], #4\n\t"
        "str	%[a], [sp, #176]\n\t"
        "bge	L_curve25519_words_%=\n\t"
        /* Invert */
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #0\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #0x60\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #4\n\t"
        "\n"
    "L_curve25519_inv_1_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_1_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #9\n\t"
        "\n"
    "L_curve25519_inv_2_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_2_%=\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x80\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #19\n\t"
        "\n"
    "L_curve25519_inv_3_%=: \n\t"
        "add	r0, sp, #0x80\n\t"
        "add	r1, sp, #0x80\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_3_%=\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x80\n\t"
        "add	r2, sp, #0x60\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #10\n\t"
        "\n"
    "L_curve25519_inv_4_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_4_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #49\n\t"
        "\n"
    "L_curve25519_inv_5_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_5_%=\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x80\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #0x63\n\t"
        "\n"
    "L_curve25519_inv_6_%=: \n\t"
        "add	r0, sp, #0x80\n\t"
        "add	r1, sp, #0x80\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_6_%=\n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x80\n\t"
        "add	r2, sp, #0x60\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #50\n\t"
        "\n"
    "L_curve25519_inv_7_%=: \n\t"
        "add	r0, sp, #0x60\n\t"
        "add	r1, sp, #0x60\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_7_%=\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x60\n\t"
        "add	r2, sp, #0x40\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #5\n\t"
        "\n"
    "L_curve25519_inv_8_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_curve25519_inv_8_%=\n\t"
        "add	r0, sp, #0\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "add	r2, sp, #0\n\t"
        "ldr	r1, [sp, #160]\n\t"
        "ldr	r0, [sp, #160]\n\t"
        "bl	fe_mul\n\t"
        "mov	r0, #0\n\t"
        "add	sp, sp, #0xbc\n\t"
        : [r] "+r" (r), [n] "+r" (n), [a] "+r" (a)
        :
        : "memory", "r3", "r12", "lr", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11"
    );
    return (uint32_t)(size_t)r;
}

void fe_pow22523(fe r_p, const fe a_p)
{
    register fe r asm ("r0") = r_p;
    register const fe a asm ("r1") = a_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x68\n\t"
        /* pow22523 */
        "str	%[r], [sp, #96]\n\t"
        "str	%[a], [sp, #100]\n\t"
        "mov	r0, sp\n\t"
        "ldr	r1, [sp, #100]\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "add	r0, sp, #32\n\t"
        "ldr	r1, [sp, #100]\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "mov	r0, sp\n\t"
        "mov	r1, sp\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "mov	r0, sp\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "mov	r0, sp\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #32\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #4\n\t"
        "\n"
    "L_fe_pow22523_1_%=: \n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_1_%=\n\t"
        "mov	r0, sp\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #32\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #9\n\t"
        "\n"
    "L_fe_pow22523_2_%=: \n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_2_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #19\n\t"
        "\n"
    "L_fe_pow22523_3_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_3_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #10\n\t"
        "\n"
    "L_fe_pow22523_4_%=: \n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_4_%=\n\t"
        "mov	r0, sp\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #32\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #49\n\t"
        "\n"
    "L_fe_pow22523_5_%=: \n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_5_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "mov	r4, #0x63\n\t"
        "\n"
    "L_fe_pow22523_6_%=: \n\t"
        "add	r0, sp, #0x40\n\t"
        "add	r1, sp, #0x40\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_6_%=\n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #0x40\n\t"
        "add	r2, sp, #32\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #50\n\t"
        "\n"
    "L_fe_pow22523_7_%=: \n\t"
        "add	r0, sp, #32\n\t"
        "add	r1, sp, #32\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_7_%=\n\t"
        "mov	r0, sp\n\t"
        "add	r1, sp, #32\n\t"
        "mov	r2, sp\n\t"
        "bl	fe_mul\n\t"
        "mov	r4, #2\n\t"
        "\n"
    "L_fe_pow22523_8_%=: \n\t"
        "mov	r0, sp\n\t"
        "mov	r1, sp\n\t"
        "bl	fe_sq\n\t"
        "sub	r4, r4, #1\n\t"
        "cmp	r4, #0\n\t"
        "bne	L_fe_pow22523_8_%=\n\t"
        "ldr	r0, [sp, #96]\n\t"
        "mov	r1, sp\n\t"
        "ldr	r2, [sp, #100]\n\t"
        "bl	fe_mul\n\t"
        "ldr	%[a], [sp, #100]\n\t"
        "ldr	%[r], [sp, #96]\n\t"
        "add	sp, sp, #0x68\n\t"
        : [r] "+r" (r), [a] "+r" (a)
        :
        : "memory", "lr", "r4"
    );
}

void fe_ge_to_p2(fe rx_p, fe ry_p, fe rz_p, const fe px_p, const fe py_p, const fe pz_p, const fe pt_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register const fe px asm ("r3") = px_p;
    register const fe py asm ("r4") = py_p;
    register const fe pz asm ("r5") = pz_p;
    register const fe pt asm ("r6") = pt_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #16\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[px], [sp, #12]\n\t"
        "ldr	r2, [sp, #28]\n\t"
        "ldr	r1, [sp, #12]\n\t"
        "ldr	r0, [sp]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #24]\n\t"
        "ldr	r1, [sp, #20]\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #28]\n\t"
        "ldr	r1, [sp, #24]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_mul\n\t"
        "add	sp, sp, #16\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [px] "+r" (px)
        :
        : "memory", "lr"
    );
    (void)py_p;
    (void)pz_p;
    (void)pt_p;
}

void fe_ge_to_p3(fe rx_p, fe ry_p, fe rz_p, fe rt_p, const fe px_p, const fe py_p, const fe pz_p, const fe pt_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register fe rt asm ("r3") = rt_p;
    register const fe px asm ("r4") = px_p;
    register const fe py asm ("r5") = py_p;
    register const fe pz asm ("r6") = pz_p;
    register const fe pt asm ("r7") = pt_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #16\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[rt], [sp, #12]\n\t"
        "ldr	r2, [sp, #32]\n\t"
        "ldr	r1, [sp, #20]\n\t"
        "ldr	r0, [sp]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #28]\n\t"
        "ldr	r1, [sp, #24]\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #32]\n\t"
        "ldr	r1, [sp, #28]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #24]\n\t"
        "ldr	r1, [sp, #20]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_mul\n\t"
        "add	sp, sp, #16\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt)
        :
        : "memory", "lr"
    );
    (void)px_p;
    (void)py_p;
    (void)pz_p;
    (void)pt_p;
}

void fe_ge_dbl(fe rx_p, fe ry_p, fe rz_p, fe rt_p, const fe px_p, const fe py_p, const fe pz_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register fe rt asm ("r3") = rt_p;
    register const fe px asm ("r4") = px_p;
    register const fe py asm ("r5") = py_p;
    register const fe pz asm ("r6") = pz_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #16\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[rt], [sp, #12]\n\t"
        "ldr	r1, [sp, #52]\n\t"
        "ldr	r0, [sp]\n\t"
        "bl	fe_sq\n\t"
        "ldr	r1, [sp, #56]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_sq\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp, #52]\n\t"
        "ldr	r2, [sp, #56]\n\t"
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r1, [sp, #4]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_sq\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp, #8]\n\t"
        "ldr	r2, [sp]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r2]\n\t"
        "ldr	r7, [r2, #4]\n\t"
#else
        "ldrd	r6, r7, [r2]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r2, #8]\n\t"
        "ldr	r7, [r2, #12]\n\t"
#else
        "ldrd	r6, r7, [r2, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r2, #16]\n\t"
        "ldr	r7, [r2, #20]\n\t"
#else
        "ldrd	r6, r7, [r2, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #24]\n\t"
        "ldr	r5, [r1, #28]\n\t"
#else
        "ldrd	r4, r5, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r2, #24]\n\t"
        "ldr	r7, [r2, #28]\n\t"
#else
        "ldrd	r6, r7, [r2, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "ldr	r0, [sp]\n\t"
        "ldr	r1, [sp, #12]\n\t"
        "ldr	r2, [sp, #4]\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
        "adcs	r6, r6, %[rt]\n\t"
        "adcs	r7, r7, %[rt]\n\t"
        "adcs	r8, r8, %[rt]\n\t"
        "adcs	r9, r9, %[rt]\n\t"
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r1, [sp, #60]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_sq2\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "ldr	r1, [sp, #8]\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r1]\n\t"
        "ldr	r9, [r1, #4]\n\t"
#else
        "ldrd	r8, r9, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r1, #8]\n\t"
        "ldr	r11, [r1, #12]\n\t"
#else
        "ldrd	r10, r11, [r1, #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r1, #16]\n\t"
        "ldr	r9, [r1, #20]\n\t"
#else
        "ldrd	r8, r9, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r1, #24]\n\t"
        "ldr	r11, [r1, #28]\n\t"
#else
        "ldrd	r10, r11, [r1, #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
        "adcs	r6, r6, %[rt]\n\t"
        "adcs	r7, r7, %[rt]\n\t"
        "adcs	r8, r8, %[rt]\n\t"
        "adcs	r9, r9, %[rt]\n\t"
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "add	sp, sp, #16\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
    (void)px_p;
    (void)py_p;
    (void)pz_p;
}

void fe_ge_madd(fe rx_p, fe ry_p, fe rz_p, fe rt_p, const fe px_p, const fe py_p, const fe pz_p, const fe pt_p, const fe qxy2d_p, const fe qyplusx_p, const fe qyminusx_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register fe rt asm ("r3") = rt_p;
    register const fe px asm ("r4") = px_p;
    register const fe py asm ("r5") = py_p;
    register const fe pz asm ("r6") = pz_p;
    register const fe pt asm ("r7") = pt_p;
    register const fe qxy2d asm ("r8") = qxy2d_p;
    register const fe qyplusx asm ("r9") = qyplusx_p;
    register const fe qyminusx asm ("r10") = qyminusx_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #32\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[rt], [sp, #12]\n\t"
        "ldr	r0, [sp]\n\t"
        "ldr	r1, [sp, #72]\n\t"
        "ldr	r2, [sp, #68]\n\t"
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp, #72]\n\t"
        "ldr	r2, [sp, #68]\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
        "adcs	r6, r6, %[rt]\n\t"
        "adcs	r7, r7, %[rt]\n\t"
        "adcs	r8, r8, %[rt]\n\t"
        "adcs	r9, r9, %[rt]\n\t"
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r2, [sp, #88]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #92]\n\t"
        "ldr	r1, [sp, #4]\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #80]\n\t"
        "ldr	r1, [sp, #84]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r2, [sp, #8]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2]\n\t"
        "ldr	r5, [r2, #4]\n\t"
#else
        "ldrd	r4, r5, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0]\n\t"
        "ldr	r7, [r0, #4]\n\t"
#else
        "ldrd	r6, r7, [r0]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #8]\n\t"
        "ldr	r5, [r2, #12]\n\t"
#else
        "ldrd	r4, r5, [r2, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #16]\n\t"
        "ldr	r5, [r2, #20]\n\t"
#else
        "ldrd	r4, r5, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #16]\n\t"
        "ldr	r7, [r0, #20]\n\t"
#else
        "ldrd	r6, r7, [r0, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #24]\n\t"
        "ldr	r5, [r2, #28]\n\t"
#else
        "ldrd	r4, r5, [r2, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "ldr	r0, [sp, #8]\n\t"
        "ldr	r1, [sp, #76]\n\t"
        /* Double */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r1, #16]\n\t"
        "ldr	r9, [r1, #20]\n\t"
#else
        "ldrd	r8, r9, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r1, #24]\n\t"
        "ldr	r11, [r1, #28]\n\t"
#else
        "ldrd	r10, r11, [r1, #24]\n\t"
#endif
        "adds	r4, r4, r4\n\t"
        "adcs	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adc	r11, r11, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #8]\n\t"
        "ldr	r1, [sp, #12]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1]\n\t"
        "ldr	r7, [r1, #4]\n\t"
#else
        "ldrd	r6, r7, [r1]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #16]\n\t"
        "ldr	r7, [r1, #20]\n\t"
#else
        "ldrd	r6, r7, [r1, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #24]\n\t"
        "ldr	r5, [r0, #28]\n\t"
#else
        "ldrd	r4, r5, [r0, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "add	sp, sp, #32\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
    (void)px_p;
    (void)py_p;
    (void)pz_p;
    (void)pt_p;
    (void)qxy2d_p;
    (void)qyplusx_p;
    (void)qyminusx_p;
}

void fe_ge_msub(fe rx_p, fe ry_p, fe rz_p, fe rt_p, const fe px_p, const fe py_p, const fe pz_p, const fe pt_p, const fe qxy2d_p, const fe qyplusx_p, const fe qyminusx_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register fe rt asm ("r3") = rt_p;
    register const fe px asm ("r4") = px_p;
    register const fe py asm ("r5") = py_p;
    register const fe pz asm ("r6") = pz_p;
    register const fe pt asm ("r7") = pt_p;
    register const fe qxy2d asm ("r8") = qxy2d_p;
    register const fe qyplusx asm ("r9") = qyplusx_p;
    register const fe qyminusx asm ("r10") = qyminusx_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #32\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[rt], [sp, #12]\n\t"
        "ldr	r0, [sp]\n\t"
        "ldr	r1, [sp, #72]\n\t"
        "ldr	r2, [sp, #68]\n\t"
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp, #72]\n\t"
        "ldr	r2, [sp, #68]\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
        "adcs	r6, r6, %[rt]\n\t"
        "adcs	r7, r7, %[rt]\n\t"
        "adcs	r8, r8, %[rt]\n\t"
        "adcs	r9, r9, %[rt]\n\t"
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r2, [sp, #92]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #88]\n\t"
        "ldr	r1, [sp, #4]\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #80]\n\t"
        "ldr	r1, [sp, #84]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r2, [sp, #8]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2]\n\t"
        "ldr	r5, [r2, #4]\n\t"
#else
        "ldrd	r4, r5, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0]\n\t"
        "ldr	r7, [r0, #4]\n\t"
#else
        "ldrd	r6, r7, [r0]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #8]\n\t"
        "ldr	r5, [r2, #12]\n\t"
#else
        "ldrd	r4, r5, [r2, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #16]\n\t"
        "ldr	r5, [r2, #20]\n\t"
#else
        "ldrd	r4, r5, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #16]\n\t"
        "ldr	r7, [r0, #20]\n\t"
#else
        "ldrd	r6, r7, [r0, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #24]\n\t"
        "ldr	r5, [r2, #28]\n\t"
#else
        "ldrd	r4, r5, [r2, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "ldr	r0, [sp, #8]\n\t"
        "ldr	r1, [sp, #76]\n\t"
        /* Double */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r1, #16]\n\t"
        "ldr	r9, [r1, #20]\n\t"
#else
        "ldrd	r8, r9, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r1, #24]\n\t"
        "ldr	r11, [r1, #28]\n\t"
#else
        "ldrd	r10, r11, [r1, #24]\n\t"
#endif
        "adds	r4, r4, r4\n\t"
        "adcs	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adc	r11, r11, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #12]\n\t"
        "ldr	r1, [sp, #8]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0]\n\t"
        "ldr	r7, [r0, #4]\n\t"
#else
        "ldrd	r6, r7, [r0]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #16]\n\t"
        "ldr	r7, [r0, #20]\n\t"
#else
        "ldrd	r6, r7, [r0, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #24]\n\t"
        "ldr	r5, [r1, #28]\n\t"
#else
        "ldrd	r4, r5, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "add	sp, sp, #32\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
    (void)px_p;
    (void)py_p;
    (void)pz_p;
    (void)pt_p;
    (void)qxy2d_p;
    (void)qyplusx_p;
    (void)qyminusx_p;
}

void fe_ge_add(fe rx_p, fe ry_p, fe rz_p, fe rt_p, const fe px_p, const fe py_p, const fe pz_p, const fe pt_p, const fe qz_p, const fe qt2d_p, const fe qyplusx_p, const fe qyminusx_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register fe rt asm ("r3") = rt_p;
    register const fe px asm ("r4") = px_p;
    register const fe py asm ("r5") = py_p;
    register const fe pz asm ("r6") = pz_p;
    register const fe pt asm ("r7") = pt_p;
    register const fe qz asm ("r8") = qz_p;
    register const fe qt2d asm ("r9") = qt2d_p;
    register const fe qyplusx asm ("r10") = qyplusx_p;
    register const fe qyminusx asm ("r11") = qyminusx_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x60\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[rt], [sp, #12]\n\t"
        "ldr	r0, [sp]\n\t"
        "ldr	r1, [sp, #136]\n\t"
        "ldr	r2, [sp, #132]\n\t"
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp, #136]\n\t"
        "ldr	r2, [sp, #132]\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
        "adcs	r6, r6, %[rt]\n\t"
        "adcs	r7, r7, %[rt]\n\t"
        "adcs	r8, r8, %[rt]\n\t"
        "adcs	r9, r9, %[rt]\n\t"
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r2, [sp, #156]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #160]\n\t"
        "ldr	r1, [sp, #4]\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #144]\n\t"
        "ldr	r1, [sp, #152]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #148]\n\t"
        "ldr	r1, [sp, #140]\n\t"
        "ldr	r0, [sp]\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #16\n\t"
        "ldr	r1, [sp]\n\t"
        /* Double */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r1, #16]\n\t"
        "ldr	r9, [r1, #20]\n\t"
#else
        "ldrd	r8, r9, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r1, #24]\n\t"
        "ldr	r11, [r1, #28]\n\t"
#else
        "ldrd	r10, r11, [r1, #24]\n\t"
#endif
        "adds	r4, r4, r4\n\t"
        "adcs	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adc	r11, r11, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r2, [sp, #8]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2]\n\t"
        "ldr	r5, [r2, #4]\n\t"
#else
        "ldrd	r4, r5, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0]\n\t"
        "ldr	r7, [r0, #4]\n\t"
#else
        "ldrd	r6, r7, [r0]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #8]\n\t"
        "ldr	r5, [r2, #12]\n\t"
#else
        "ldrd	r4, r5, [r2, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #16]\n\t"
        "ldr	r5, [r2, #20]\n\t"
#else
        "ldrd	r4, r5, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #16]\n\t"
        "ldr	r7, [r0, #20]\n\t"
#else
        "ldrd	r6, r7, [r0, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #24]\n\t"
        "ldr	r5, [r2, #28]\n\t"
#else
        "ldrd	r4, r5, [r2, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "ldr	r0, [sp, #8]\n\t"
        "ldr	r1, [sp, #12]\n\t"
        "add	r2, sp, #16\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2]\n\t"
        "ldr	r5, [r2, #4]\n\t"
#else
        "ldrd	r4, r5, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1]\n\t"
        "ldr	r7, [r1, #4]\n\t"
#else
        "ldrd	r6, r7, [r1]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #8]\n\t"
        "ldr	r5, [r2, #12]\n\t"
#else
        "ldrd	r4, r5, [r2, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #16]\n\t"
        "ldr	r5, [r2, #20]\n\t"
#else
        "ldrd	r4, r5, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #16]\n\t"
        "ldr	r7, [r1, #20]\n\t"
#else
        "ldrd	r6, r7, [r1, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #24]\n\t"
        "ldr	r5, [r2, #28]\n\t"
#else
        "ldrd	r4, r5, [r2, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "add	sp, sp, #0x60\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
    (void)px_p;
    (void)py_p;
    (void)pz_p;
    (void)pt_p;
    (void)qz_p;
    (void)qt2d_p;
    (void)qyplusx_p;
    (void)qyminusx_p;
}

void fe_ge_sub(fe rx_p, fe ry_p, fe rz_p, fe rt_p, const fe px_p, const fe py_p, const fe pz_p, const fe pt_p, const fe qz_p, const fe qt2d_p, const fe qyplusx_p, const fe qyminusx_p)
{
    register fe rx asm ("r0") = rx_p;
    register fe ry asm ("r1") = ry_p;
    register fe rz asm ("r2") = rz_p;
    register fe rt asm ("r3") = rt_p;
    register const fe px asm ("r4") = px_p;
    register const fe py asm ("r5") = py_p;
    register const fe pz asm ("r6") = pz_p;
    register const fe pt asm ("r7") = pt_p;
    register const fe qz asm ("r8") = qz_p;
    register const fe qt2d asm ("r9") = qt2d_p;
    register const fe qyplusx asm ("r10") = qyplusx_p;
    register const fe qyminusx asm ("r11") = qyminusx_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0x60\n\t"
        "str	%[rx], [sp]\n\t"
        "str	%[ry], [sp, #4]\n\t"
        "str	%[rz], [sp, #8]\n\t"
        "str	%[rt], [sp, #12]\n\t"
        "ldr	r0, [sp]\n\t"
        "ldr	r1, [sp, #136]\n\t"
        "ldr	r2, [sp, #132]\n\t"
        /* Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "adds	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "adcs	r8, r4, r8\n\t"
        "adcs	r9, r5, r9\n\t"
        "adcs	r10, r6, r10\n\t"
        "adc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp, #136]\n\t"
        "ldr	r2, [sp, #132]\n\t"
        /* Sub */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2]\n\t"
        "ldr	r9, [r2, #4]\n\t"
#else
        "ldrd	r8, r9, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #8]\n\t"
        "ldr	r11, [r2, #12]\n\t"
#else
        "ldrd	r10, r11, [r2, #8]\n\t"
#endif
        "subs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbcs	r11, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #8]\n\t"
        "str	r11, [r0, #12]\n\t"
#else
        "strd	r10, r11, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #24]\n\t"
        "ldr	r7, [r1, #28]\n\t"
#else
        "ldrd	r6, r7, [r1, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r2, #16]\n\t"
        "ldr	r9, [r2, #20]\n\t"
#else
        "ldrd	r8, r9, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r2, #24]\n\t"
        "ldr	r11, [r2, #28]\n\t"
#else
        "ldrd	r10, r11, [r2, #24]\n\t"
#endif
        "sbcs	r8, r4, r8\n\t"
        "sbcs	r9, r5, r9\n\t"
        "sbcs	r10, r6, r10\n\t"
        "sbc	r11, r7, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
        "adcs	r6, r6, %[rt]\n\t"
        "adcs	r7, r7, %[rt]\n\t"
        "adcs	r8, r8, %[rt]\n\t"
        "adcs	r9, r9, %[rt]\n\t"
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r2, [sp, #160]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r0, [sp, #8]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #156]\n\t"
        "ldr	r1, [sp, #4]\n\t"
        "ldr	r0, [sp, #4]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #144]\n\t"
        "ldr	r1, [sp, #152]\n\t"
        "ldr	r0, [sp, #12]\n\t"
        "bl	fe_mul\n\t"
        "ldr	r2, [sp, #148]\n\t"
        "ldr	r1, [sp, #140]\n\t"
        "ldr	r0, [sp]\n\t"
        "bl	fe_mul\n\t"
        "add	r0, sp, #16\n\t"
        "ldr	r1, [sp]\n\t"
        /* Double */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r1, #8]\n\t"
        "ldr	r7, [r1, #12]\n\t"
#else
        "ldrd	r6, r7, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [r1, #16]\n\t"
        "ldr	r9, [r1, #20]\n\t"
#else
        "ldrd	r8, r9, [r1, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [r1, #24]\n\t"
        "ldr	r11, [r1, #28]\n\t"
#else
        "ldrd	r10, r11, [r1, #24]\n\t"
#endif
        "adds	r4, r4, r4\n\t"
        "adcs	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adc	r11, r11, r11\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
        "sbcs	r6, r6, %[rt]\n\t"
        "sbcs	r7, r7, %[rt]\n\t"
        "sbcs	r8, r8, %[rt]\n\t"
        "sbcs	r9, r9, %[rt]\n\t"
        "sbcs	r10, r10, %[rt]\n\t"
        "sbc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [r0, #8]\n\t"
        "str	r7, [r0, #12]\n\t"
#else
        "strd	r6, r7, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r0, #24]\n\t"
        "str	r11, [r0, #28]\n\t"
#else
        "strd	r10, r11, [r0, #24]\n\t"
#endif
        "ldr	r0, [sp, #4]\n\t"
        "ldr	r1, [sp]\n\t"
        "ldr	r2, [sp, #8]\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2]\n\t"
        "ldr	r5, [r2, #4]\n\t"
#else
        "ldrd	r4, r5, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0]\n\t"
        "ldr	r7, [r0, #4]\n\t"
#else
        "ldrd	r6, r7, [r0]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #8]\n\t"
        "ldr	r5, [r2, #12]\n\t"
#else
        "ldrd	r4, r5, [r2, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #16]\n\t"
        "ldr	r5, [r2, #20]\n\t"
#else
        "ldrd	r4, r5, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #16]\n\t"
        "ldr	r7, [r0, #20]\n\t"
#else
        "ldrd	r6, r7, [r0, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #24]\n\t"
        "ldr	r5, [r2, #28]\n\t"
#else
        "ldrd	r4, r5, [r2, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "ldr	r0, [sp, #12]\n\t"
        "ldr	r1, [sp, #8]\n\t"
        "add	r2, sp, #16\n\t"
        /* Add-Sub */
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2]\n\t"
        "ldr	r5, [r2, #4]\n\t"
#else
        "ldrd	r4, r5, [r2]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0]\n\t"
        "ldr	r7, [r0, #4]\n\t"
#else
        "ldrd	r6, r7, [r0]\n\t"
#endif
        "adds	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0]\n\t"
        "str	r9, [r0, #4]\n\t"
#else
        "strd	r8, r9, [r0]\n\t"
#endif
        /*  Sub */
        "subs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1]\n\t"
        "str	r11, [r1, #4]\n\t"
#else
        "strd	r10, r11, [r1]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #8]\n\t"
        "ldr	r5, [r2, #12]\n\t"
#else
        "ldrd	r4, r5, [r2, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #8]\n\t"
        "ldr	r7, [r0, #12]\n\t"
#else
        "ldrd	r6, r7, [r0, #8]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #8]\n\t"
        "str	r9, [r0, #12]\n\t"
#else
        "strd	r8, r9, [r0, #8]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #8]\n\t"
        "str	r11, [r1, #12]\n\t"
#else
        "strd	r10, r11, [r1, #8]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #16]\n\t"
        "ldr	r5, [r2, #20]\n\t"
#else
        "ldrd	r4, r5, [r2, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #16]\n\t"
        "ldr	r7, [r0, #20]\n\t"
#else
        "ldrd	r6, r7, [r0, #16]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "mov	r12, #0\n\t"
        "adcs	r9, r5, r7\n\t"
        "adc	r12, r12, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #16]\n\t"
        "str	r9, [r0, #20]\n\t"
#else
        "strd	r8, r9, [r0, #16]\n\t"
#endif
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "mov	lr, #0\n\t"
        "sbcs	r11, r5, r7\n\t"
        "adc	lr, lr, #0\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #16]\n\t"
        "str	r11, [r1, #20]\n\t"
#else
        "strd	r10, r11, [r1, #16]\n\t"
#endif
        /*  Add */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r2, #24]\n\t"
        "ldr	r5, [r2, #28]\n\t"
#else
        "ldrd	r4, r5, [r2, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r0, #24]\n\t"
        "ldr	r7, [r0, #28]\n\t"
#else
        "ldrd	r6, r7, [r0, #24]\n\t"
#endif
        "adds	r12, r12, #-1\n\t"
        "adcs	r8, r4, r6\n\t"
        "adc	r9, r5, r7\n\t"
        /*  Sub */
        "adds	lr, lr, #-1\n\t"
        "sbcs	r10, r4, r6\n\t"
        "sbc	r11, r5, r7\n\t"
        "mov	r12, #-19\n\t"
        "asr	%[rt], r9, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Sub modulus (if overflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0]\n\t"
        "ldr	r5, [r0, #4]\n\t"
#else
        "ldrd	r4, r5, [r0]\n\t"
#endif
        "subs	r4, r4, r12\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0]\n\t"
        "str	r5, [r0, #4]\n\t"
#else
        "strd	r4, r5, [r0]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #8]\n\t"
        "ldr	r5, [r0, #12]\n\t"
#else
        "ldrd	r4, r5, [r0, #8]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #8]\n\t"
        "str	r5, [r0, #12]\n\t"
#else
        "strd	r4, r5, [r0, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r0, #16]\n\t"
        "ldr	r5, [r0, #20]\n\t"
#else
        "ldrd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r4, r4, %[rt]\n\t"
        "sbcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r0, #16]\n\t"
        "str	r5, [r0, #20]\n\t"
#else
        "strd	r4, r5, [r0, #16]\n\t"
#endif
        "sbcs	r8, r8, %[rt]\n\t"
        "sbc	r9, r9, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [r0, #24]\n\t"
        "str	r9, [r0, #28]\n\t"
#else
        "strd	r8, r9, [r0, #24]\n\t"
#endif
        "mov	r12, #-19\n\t"
        "asr	%[rt], r11, #31\n\t"
        /*   Mask the modulus */
        "and	r12, %[rt], r12\n\t"
        "and	lr, %[rt], #0x7fffffff\n\t"
        /*   Add modulus (if underflow) */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1]\n\t"
        "ldr	r5, [r1, #4]\n\t"
#else
        "ldrd	r4, r5, [r1]\n\t"
#endif
        "adds	r4, r4, r12\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1]\n\t"
        "str	r5, [r1, #4]\n\t"
#else
        "strd	r4, r5, [r1]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #8]\n\t"
        "ldr	r5, [r1, #12]\n\t"
#else
        "ldrd	r4, r5, [r1, #8]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #8]\n\t"
        "str	r5, [r1, #12]\n\t"
#else
        "strd	r4, r5, [r1, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [r1, #16]\n\t"
        "ldr	r5, [r1, #20]\n\t"
#else
        "ldrd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r4, r4, %[rt]\n\t"
        "adcs	r5, r5, %[rt]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [r1, #16]\n\t"
        "str	r5, [r1, #20]\n\t"
#else
        "strd	r4, r5, [r1, #16]\n\t"
#endif
        "adcs	r10, r10, %[rt]\n\t"
        "adc	r11, r11, lr\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [r1, #24]\n\t"
        "str	r11, [r1, #28]\n\t"
#else
        "strd	r10, r11, [r1, #24]\n\t"
#endif
        "add	sp, sp, #0x60\n\t"
        : [rx] "+r" (rx), [ry] "+r" (ry), [rz] "+r" (rz), [rt] "+r" (rt)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "lr"
    );
    (void)px_p;
    (void)py_p;
    (void)pz_p;
    (void)pt_p;
    (void)qz_p;
    (void)qt2d_p;
    (void)qyplusx_p;
    (void)qyminusx_p;
}


#endif /* HAVE_CURVE25519 */
#endif /* !__aarch64__ */
#endif /* WOLFSSL_ARMASM */
