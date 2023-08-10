/* armv8-32-sha512-asm
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
 *   ruby ./sha2/sha512.rb arm32 ../wolfssl/wolfcrypt/src/port/arm/armv8-32-sha512-asm.c
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
#ifdef WOLFSSL_SHA512
#include <wolfssl/wolfcrypt/sha512.h>

#ifdef WOLFSSL_ARMASM_NO_NEON
static const uint64_t L_SHA512_transform_len_k[] = {
    0x428a2f98d728ae22UL,
    0x7137449123ef65cdUL,
    0xb5c0fbcfec4d3b2fUL,
    0xe9b5dba58189dbbcUL,
    0x3956c25bf348b538UL,
    0x59f111f1b605d019UL,
    0x923f82a4af194f9bUL,
    0xab1c5ed5da6d8118UL,
    0xd807aa98a3030242UL,
    0x12835b0145706fbeUL,
    0x243185be4ee4b28cUL,
    0x550c7dc3d5ffb4e2UL,
    0x72be5d74f27b896fUL,
    0x80deb1fe3b1696b1UL,
    0x9bdc06a725c71235UL,
    0xc19bf174cf692694UL,
    0xe49b69c19ef14ad2UL,
    0xefbe4786384f25e3UL,
    0xfc19dc68b8cd5b5UL,
    0x240ca1cc77ac9c65UL,
    0x2de92c6f592b0275UL,
    0x4a7484aa6ea6e483UL,
    0x5cb0a9dcbd41fbd4UL,
    0x76f988da831153b5UL,
    0x983e5152ee66dfabUL,
    0xa831c66d2db43210UL,
    0xb00327c898fb213fUL,
    0xbf597fc7beef0ee4UL,
    0xc6e00bf33da88fc2UL,
    0xd5a79147930aa725UL,
    0x6ca6351e003826fUL,
    0x142929670a0e6e70UL,
    0x27b70a8546d22ffcUL,
    0x2e1b21385c26c926UL,
    0x4d2c6dfc5ac42aedUL,
    0x53380d139d95b3dfUL,
    0x650a73548baf63deUL,
    0x766a0abb3c77b2a8UL,
    0x81c2c92e47edaee6UL,
    0x92722c851482353bUL,
    0xa2bfe8a14cf10364UL,
    0xa81a664bbc423001UL,
    0xc24b8b70d0f89791UL,
    0xc76c51a30654be30UL,
    0xd192e819d6ef5218UL,
    0xd69906245565a910UL,
    0xf40e35855771202aUL,
    0x106aa07032bbd1b8UL,
    0x19a4c116b8d2d0c8UL,
    0x1e376c085141ab53UL,
    0x2748774cdf8eeb99UL,
    0x34b0bcb5e19b48a8UL,
    0x391c0cb3c5c95a63UL,
    0x4ed8aa4ae3418acbUL,
    0x5b9cca4f7763e373UL,
    0x682e6ff3d6b2b8a3UL,
    0x748f82ee5defb2fcUL,
    0x78a5636f43172f60UL,
    0x84c87814a1f0ab72UL,
    0x8cc702081a6439ecUL,
    0x90befffa23631e28UL,
    0xa4506cebde82bde9UL,
    0xbef9a3f7b2c67915UL,
    0xc67178f2e372532bUL,
    0xca273eceea26619cUL,
    0xd186b8c721c0c207UL,
    0xeada7dd6cde0eb1eUL,
    0xf57d4f7fee6ed178UL,
    0x6f067aa72176fbaUL,
    0xa637dc5a2c898a6UL,
    0x113f9804bef90daeUL,
    0x1b710b35131c471bUL,
    0x28db77f523047d84UL,
    0x32caab7b40c72493UL,
    0x3c9ebe0a15c9bebcUL,
    0x431d67c49c100d4cUL,
    0x4cc5d4becb3e42b6UL,
    0x597f299cfc657e2aUL,
    0x5fcb6fab3ad6faecUL,
    0x6c44198c4a475817UL,
};

void Transform_Sha512_Len(wc_Sha512* sha512_p, const byte* data_p, word32 len_p);
void Transform_Sha512_Len(wc_Sha512* sha512_p, const byte* data_p, word32 len_p)
{
    register wc_Sha512* sha512 asm ("r0") = sha512_p;
    register const byte* data asm ("r1") = data_p;
    register word32 len asm ("r2") = len_p;

    __asm__ __volatile__ (
        "sub	sp, sp, #0xc0\n\t"
        "mov	r3, %[L_SHA512_transform_len_k]\n\t"
        /* Copy digest to add in at end */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[sha512], #24]\n\t"
        "ldr	r11, [%[sha512], #28]\n\t"
#else
        "ldrd	r10, r11, [%[sha512], #24]\n\t"
#endif
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
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[sha512], #56]\n\t"
        "ldr	r11, [%[sha512], #60]\n\t"
#else
        "ldrd	r10, r11, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #160]\n\t"
        "str	r5, [sp, #164]\n\t"
#else
        "strd	r4, r5, [sp, #160]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #168]\n\t"
        "str	r7, [sp, #172]\n\t"
#else
        "strd	r6, r7, [sp, #168]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [sp, #176]\n\t"
        "str	r9, [sp, #180]\n\t"
#else
        "strd	r8, r9, [sp, #176]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r10, [sp, #184]\n\t"
        "str	r11, [sp, #188]\n\t"
#else
        "strd	r10, r11, [sp, #184]\n\t"
#endif
        /* Start of loop processing a block */
        "\n"
    "L_SHA512_transform_len_begin_%=: \n\t"
        /* Load, Reverse and Store W */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[data]]\n\t"
        "ldr	r5, [%[data], #4]\n\t"
#else
        "ldrd	r4, r5, [%[data]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[data], #8]\n\t"
        "ldr	r7, [%[data], #12]\n\t"
#else
        "ldrd	r6, r7, [%[data], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[data], #16]\n\t"
        "ldr	r9, [%[data], #20]\n\t"
#else
        "ldrd	r8, r9, [%[data], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[data], #24]\n\t"
        "ldr	r11, [%[data], #28]\n\t"
#else
        "ldrd	r10, r11, [%[data], #24]\n\t"
#endif
        "rev	r4, r4\n\t"
        "rev	r5, r5\n\t"
        "rev	r6, r6\n\t"
        "rev	r7, r7\n\t"
        "rev	r8, r8\n\t"
        "rev	r9, r9\n\t"
        "rev	r10, r10\n\t"
        "rev	r11, r11\n\t"
        "str	r5, [sp]\n\t"
        "str	r4, [sp, #4]\n\t"
        "str	r7, [sp, #8]\n\t"
        "str	r6, [sp, #12]\n\t"
        "str	r9, [sp, #16]\n\t"
        "str	r8, [sp, #20]\n\t"
        "str	r11, [sp, #24]\n\t"
        "str	r10, [sp, #28]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[data], #32]\n\t"
        "ldr	r5, [%[data], #36]\n\t"
#else
        "ldrd	r4, r5, [%[data], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[data], #40]\n\t"
        "ldr	r7, [%[data], #44]\n\t"
#else
        "ldrd	r6, r7, [%[data], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[data], #48]\n\t"
        "ldr	r9, [%[data], #52]\n\t"
#else
        "ldrd	r8, r9, [%[data], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[data], #56]\n\t"
        "ldr	r11, [%[data], #60]\n\t"
#else
        "ldrd	r10, r11, [%[data], #56]\n\t"
#endif
        "rev	r4, r4\n\t"
        "rev	r5, r5\n\t"
        "rev	r6, r6\n\t"
        "rev	r7, r7\n\t"
        "rev	r8, r8\n\t"
        "rev	r9, r9\n\t"
        "rev	r10, r10\n\t"
        "rev	r11, r11\n\t"
        "str	r5, [sp, #32]\n\t"
        "str	r4, [sp, #36]\n\t"
        "str	r7, [sp, #40]\n\t"
        "str	r6, [sp, #44]\n\t"
        "str	r9, [sp, #48]\n\t"
        "str	r8, [sp, #52]\n\t"
        "str	r11, [sp, #56]\n\t"
        "str	r10, [sp, #60]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[data], #64]\n\t"
        "ldr	r5, [%[data], #68]\n\t"
#else
        "ldrd	r4, r5, [%[data], #64]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[data], #72]\n\t"
        "ldr	r7, [%[data], #76]\n\t"
#else
        "ldrd	r6, r7, [%[data], #72]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[data], #80]\n\t"
        "ldr	r9, [%[data], #84]\n\t"
#else
        "ldrd	r8, r9, [%[data], #80]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[data], #88]\n\t"
        "ldr	r11, [%[data], #92]\n\t"
#else
        "ldrd	r10, r11, [%[data], #88]\n\t"
#endif
        "rev	r4, r4\n\t"
        "rev	r5, r5\n\t"
        "rev	r6, r6\n\t"
        "rev	r7, r7\n\t"
        "rev	r8, r8\n\t"
        "rev	r9, r9\n\t"
        "rev	r10, r10\n\t"
        "rev	r11, r11\n\t"
        "str	r5, [sp, #64]\n\t"
        "str	r4, [sp, #68]\n\t"
        "str	r7, [sp, #72]\n\t"
        "str	r6, [sp, #76]\n\t"
        "str	r9, [sp, #80]\n\t"
        "str	r8, [sp, #84]\n\t"
        "str	r11, [sp, #88]\n\t"
        "str	r10, [sp, #92]\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[data], #96]\n\t"
        "ldr	r5, [%[data], #100]\n\t"
#else
        "ldrd	r4, r5, [%[data], #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[data], #104]\n\t"
        "ldr	r7, [%[data], #108]\n\t"
#else
        "ldrd	r6, r7, [%[data], #104]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[data], #112]\n\t"
        "ldr	r9, [%[data], #116]\n\t"
#else
        "ldrd	r8, r9, [%[data], #112]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[data], #120]\n\t"
        "ldr	r11, [%[data], #124]\n\t"
#else
        "ldrd	r10, r11, [%[data], #120]\n\t"
#endif
        "rev	r4, r4\n\t"
        "rev	r5, r5\n\t"
        "rev	r6, r6\n\t"
        "rev	r7, r7\n\t"
        "rev	r8, r8\n\t"
        "rev	r9, r9\n\t"
        "rev	r10, r10\n\t"
        "rev	r11, r11\n\t"
        "str	r5, [sp, #96]\n\t"
        "str	r4, [sp, #100]\n\t"
        "str	r7, [sp, #104]\n\t"
        "str	r6, [sp, #108]\n\t"
        "str	r9, [sp, #112]\n\t"
        "str	r8, [sp, #116]\n\t"
        "str	r11, [sp, #120]\n\t"
        "str	r10, [sp, #124]\n\t"
        /* Pre-calc: b ^ c */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [%[sha512], #8]\n\t"
        "ldr	r11, [%[sha512], #12]\n\t"
#else
        "ldrd	r10, r11, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r10, r10, r4\n\t"
        "eor	r11, r11, r5\n\t"
        "mov	r12, #4\n\t"
        /* Start of 16 rounds */
        "\n"
    "L_SHA512_transform_len_start_%=: \n\t"
        /* Round 0 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp]\n\t"
        "ldr	r9, [sp, #4]\n\t"
#else
        "ldrd	r8, r9, [sp]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3]\n\t"
        "ldr	r7, [r3, #4]\n\t"
#else
        "ldrd	r6, r7, [r3]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #24]\n\t"
        "str	r9, [%[sha512], #28]\n\t"
#else
        "strd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #56]\n\t"
        "str	r7, [%[sha512], #60]\n\t"
#else
        "strd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[0] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #112]\n\t"
        "ldr	r5, [sp, #116]\n\t"
#else
        "ldrd	r4, r5, [sp, #112]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #72]\n\t"
        "ldr	r9, [sp, #76]\n\t"
#else
        "ldrd	r8, r9, [sp, #72]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
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
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp]\n\t"
        "str	r5, [sp, #4]\n\t"
#else
        "strd	r4, r5, [sp]\n\t"
#endif
        /* Round 1 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #8]\n\t"
        "ldr	r9, [sp, #12]\n\t"
#else
        "ldrd	r8, r9, [sp, #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #8]\n\t"
        "ldr	r7, [r3, #12]\n\t"
#else
        "ldrd	r6, r7, [r3, #8]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #16]\n\t"
        "str	r9, [%[sha512], #20]\n\t"
#else
        "strd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #48]\n\t"
        "str	r7, [%[sha512], #52]\n\t"
#else
        "strd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[1] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #120]\n\t"
        "ldr	r5, [sp, #124]\n\t"
#else
        "ldrd	r4, r5, [sp, #120]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #8]\n\t"
        "ldr	r5, [sp, #12]\n\t"
#else
        "ldrd	r4, r5, [sp, #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #80]\n\t"
        "ldr	r9, [sp, #84]\n\t"
#else
        "ldrd	r8, r9, [sp, #80]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
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
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #8]\n\t"
        "ldr	r5, [sp, #12]\n\t"
#else
        "ldrd	r4, r5, [sp, #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #8]\n\t"
        "str	r5, [sp, #12]\n\t"
#else
        "strd	r4, r5, [sp, #8]\n\t"
#endif
        /* Round 2 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #16]\n\t"
        "ldr	r9, [sp, #20]\n\t"
#else
        "ldrd	r8, r9, [sp, #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #16]\n\t"
        "ldr	r7, [r3, #20]\n\t"
#else
        "ldrd	r6, r7, [r3, #16]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #8]\n\t"
        "str	r9, [%[sha512], #12]\n\t"
#else
        "strd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #40]\n\t"
        "str	r7, [%[sha512], #44]\n\t"
#else
        "strd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[2] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #16]\n\t"
        "ldr	r5, [sp, #20]\n\t"
#else
        "ldrd	r4, r5, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #88]\n\t"
        "ldr	r9, [sp, #92]\n\t"
#else
        "ldrd	r8, r9, [sp, #88]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #16]\n\t"
        "str	r5, [sp, #20]\n\t"
#else
        "strd	r4, r5, [sp, #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #24]\n\t"
        "ldr	r5, [sp, #28]\n\t"
#else
        "ldrd	r4, r5, [sp, #24]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #16]\n\t"
        "ldr	r5, [sp, #20]\n\t"
#else
        "ldrd	r4, r5, [sp, #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #16]\n\t"
        "str	r5, [sp, #20]\n\t"
#else
        "strd	r4, r5, [sp, #16]\n\t"
#endif
        /* Round 3 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #24]\n\t"
        "ldr	r9, [sp, #28]\n\t"
#else
        "ldrd	r8, r9, [sp, #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #24]\n\t"
        "ldr	r7, [r3, #28]\n\t"
#else
        "ldrd	r6, r7, [r3, #24]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512]]\n\t"
        "str	r9, [%[sha512], #4]\n\t"
#else
        "strd	r8, r9, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #32]\n\t"
        "str	r7, [%[sha512], #36]\n\t"
#else
        "strd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[3] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #8]\n\t"
        "ldr	r5, [sp, #12]\n\t"
#else
        "ldrd	r4, r5, [sp, #8]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #24]\n\t"
        "ldr	r5, [sp, #28]\n\t"
#else
        "ldrd	r4, r5, [sp, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #96]\n\t"
        "ldr	r9, [sp, #100]\n\t"
#else
        "ldrd	r8, r9, [sp, #96]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #24]\n\t"
        "str	r5, [sp, #28]\n\t"
#else
        "strd	r4, r5, [sp, #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #32]\n\t"
        "ldr	r5, [sp, #36]\n\t"
#else
        "ldrd	r4, r5, [sp, #32]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #24]\n\t"
        "ldr	r5, [sp, #28]\n\t"
#else
        "ldrd	r4, r5, [sp, #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #24]\n\t"
        "str	r5, [sp, #28]\n\t"
#else
        "strd	r4, r5, [sp, #24]\n\t"
#endif
        /* Round 4 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #32]\n\t"
        "ldr	r9, [sp, #36]\n\t"
#else
        "ldrd	r8, r9, [sp, #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #32]\n\t"
        "ldr	r7, [r3, #36]\n\t"
#else
        "ldrd	r6, r7, [r3, #32]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #56]\n\t"
        "str	r9, [%[sha512], #60]\n\t"
#else
        "strd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #24]\n\t"
        "str	r7, [%[sha512], #28]\n\t"
#else
        "strd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[4] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #16]\n\t"
        "ldr	r5, [sp, #20]\n\t"
#else
        "ldrd	r4, r5, [sp, #16]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #32]\n\t"
        "ldr	r5, [sp, #36]\n\t"
#else
        "ldrd	r4, r5, [sp, #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #104]\n\t"
        "ldr	r9, [sp, #108]\n\t"
#else
        "ldrd	r8, r9, [sp, #104]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #32]\n\t"
        "str	r5, [sp, #36]\n\t"
#else
        "strd	r4, r5, [sp, #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #40]\n\t"
        "ldr	r5, [sp, #44]\n\t"
#else
        "ldrd	r4, r5, [sp, #40]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #32]\n\t"
        "ldr	r5, [sp, #36]\n\t"
#else
        "ldrd	r4, r5, [sp, #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #32]\n\t"
        "str	r5, [sp, #36]\n\t"
#else
        "strd	r4, r5, [sp, #32]\n\t"
#endif
        /* Round 5 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #40]\n\t"
        "ldr	r9, [sp, #44]\n\t"
#else
        "ldrd	r8, r9, [sp, #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #40]\n\t"
        "ldr	r7, [r3, #44]\n\t"
#else
        "ldrd	r6, r7, [r3, #40]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #48]\n\t"
        "str	r9, [%[sha512], #52]\n\t"
#else
        "strd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #16]\n\t"
        "str	r7, [%[sha512], #20]\n\t"
#else
        "strd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[5] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #24]\n\t"
        "ldr	r5, [sp, #28]\n\t"
#else
        "ldrd	r4, r5, [sp, #24]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #40]\n\t"
        "ldr	r5, [sp, #44]\n\t"
#else
        "ldrd	r4, r5, [sp, #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #112]\n\t"
        "ldr	r9, [sp, #116]\n\t"
#else
        "ldrd	r8, r9, [sp, #112]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #40]\n\t"
        "str	r5, [sp, #44]\n\t"
#else
        "strd	r4, r5, [sp, #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #48]\n\t"
        "ldr	r5, [sp, #52]\n\t"
#else
        "ldrd	r4, r5, [sp, #48]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #40]\n\t"
        "ldr	r5, [sp, #44]\n\t"
#else
        "ldrd	r4, r5, [sp, #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #40]\n\t"
        "str	r5, [sp, #44]\n\t"
#else
        "strd	r4, r5, [sp, #40]\n\t"
#endif
        /* Round 6 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #48]\n\t"
        "ldr	r9, [sp, #52]\n\t"
#else
        "ldrd	r8, r9, [sp, #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #48]\n\t"
        "ldr	r7, [r3, #52]\n\t"
#else
        "ldrd	r6, r7, [r3, #48]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #40]\n\t"
        "str	r9, [%[sha512], #44]\n\t"
#else
        "strd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #8]\n\t"
        "str	r7, [%[sha512], #12]\n\t"
#else
        "strd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[6] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #32]\n\t"
        "ldr	r5, [sp, #36]\n\t"
#else
        "ldrd	r4, r5, [sp, #32]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #48]\n\t"
        "ldr	r5, [sp, #52]\n\t"
#else
        "ldrd	r4, r5, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #120]\n\t"
        "ldr	r9, [sp, #124]\n\t"
#else
        "ldrd	r8, r9, [sp, #120]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #48]\n\t"
        "str	r5, [sp, #52]\n\t"
#else
        "strd	r4, r5, [sp, #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #56]\n\t"
        "ldr	r5, [sp, #60]\n\t"
#else
        "ldrd	r4, r5, [sp, #56]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #48]\n\t"
        "ldr	r5, [sp, #52]\n\t"
#else
        "ldrd	r4, r5, [sp, #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #48]\n\t"
        "str	r5, [sp, #52]\n\t"
#else
        "strd	r4, r5, [sp, #48]\n\t"
#endif
        /* Round 7 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #56]\n\t"
        "ldr	r9, [sp, #60]\n\t"
#else
        "ldrd	r8, r9, [sp, #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #56]\n\t"
        "ldr	r7, [r3, #60]\n\t"
#else
        "ldrd	r6, r7, [r3, #56]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #32]\n\t"
        "str	r9, [%[sha512], #36]\n\t"
#else
        "strd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512]]\n\t"
        "str	r7, [%[sha512], #4]\n\t"
#else
        "strd	r6, r7, [%[sha512]]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[7] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #40]\n\t"
        "ldr	r5, [sp, #44]\n\t"
#else
        "ldrd	r4, r5, [sp, #40]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #56]\n\t"
        "ldr	r5, [sp, #60]\n\t"
#else
        "ldrd	r4, r5, [sp, #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp]\n\t"
        "ldr	r9, [sp, #4]\n\t"
#else
        "ldrd	r8, r9, [sp]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #56]\n\t"
        "str	r5, [sp, #60]\n\t"
#else
        "strd	r4, r5, [sp, #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #64]\n\t"
        "ldr	r5, [sp, #68]\n\t"
#else
        "ldrd	r4, r5, [sp, #64]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #56]\n\t"
        "ldr	r5, [sp, #60]\n\t"
#else
        "ldrd	r4, r5, [sp, #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #56]\n\t"
        "str	r5, [sp, #60]\n\t"
#else
        "strd	r4, r5, [sp, #56]\n\t"
#endif
        /* Round 8 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #64]\n\t"
        "ldr	r9, [sp, #68]\n\t"
#else
        "ldrd	r8, r9, [sp, #64]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #64]\n\t"
        "ldr	r7, [r3, #68]\n\t"
#else
        "ldrd	r6, r7, [r3, #64]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #24]\n\t"
        "str	r9, [%[sha512], #28]\n\t"
#else
        "strd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #56]\n\t"
        "str	r7, [%[sha512], #60]\n\t"
#else
        "strd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[8] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #48]\n\t"
        "ldr	r5, [sp, #52]\n\t"
#else
        "ldrd	r4, r5, [sp, #48]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #64]\n\t"
        "ldr	r5, [sp, #68]\n\t"
#else
        "ldrd	r4, r5, [sp, #64]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #8]\n\t"
        "ldr	r9, [sp, #12]\n\t"
#else
        "ldrd	r8, r9, [sp, #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
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
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #64]\n\t"
        "ldr	r5, [sp, #68]\n\t"
#else
        "ldrd	r4, r5, [sp, #64]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #64]\n\t"
        "str	r5, [sp, #68]\n\t"
#else
        "strd	r4, r5, [sp, #64]\n\t"
#endif
        /* Round 9 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #72]\n\t"
        "ldr	r9, [sp, #76]\n\t"
#else
        "ldrd	r8, r9, [sp, #72]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #72]\n\t"
        "ldr	r7, [r3, #76]\n\t"
#else
        "ldrd	r6, r7, [r3, #72]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #16]\n\t"
        "str	r9, [%[sha512], #20]\n\t"
#else
        "strd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #48]\n\t"
        "str	r7, [%[sha512], #52]\n\t"
#else
        "strd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[9] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #56]\n\t"
        "ldr	r5, [sp, #60]\n\t"
#else
        "ldrd	r4, r5, [sp, #56]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #72]\n\t"
        "ldr	r5, [sp, #76]\n\t"
#else
        "ldrd	r4, r5, [sp, #72]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #16]\n\t"
        "ldr	r9, [sp, #20]\n\t"
#else
        "ldrd	r8, r9, [sp, #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
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
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #72]\n\t"
        "ldr	r5, [sp, #76]\n\t"
#else
        "ldrd	r4, r5, [sp, #72]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #72]\n\t"
        "str	r5, [sp, #76]\n\t"
#else
        "strd	r4, r5, [sp, #72]\n\t"
#endif
        /* Round 10 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #80]\n\t"
        "ldr	r9, [sp, #84]\n\t"
#else
        "ldrd	r8, r9, [sp, #80]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #80]\n\t"
        "ldr	r7, [r3, #84]\n\t"
#else
        "ldrd	r6, r7, [r3, #80]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #8]\n\t"
        "str	r9, [%[sha512], #12]\n\t"
#else
        "strd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #40]\n\t"
        "str	r7, [%[sha512], #44]\n\t"
#else
        "strd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[10] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #64]\n\t"
        "ldr	r5, [sp, #68]\n\t"
#else
        "ldrd	r4, r5, [sp, #64]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #80]\n\t"
        "ldr	r5, [sp, #84]\n\t"
#else
        "ldrd	r4, r5, [sp, #80]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #24]\n\t"
        "ldr	r9, [sp, #28]\n\t"
#else
        "ldrd	r8, r9, [sp, #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #80]\n\t"
        "str	r5, [sp, #84]\n\t"
#else
        "strd	r4, r5, [sp, #80]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #88]\n\t"
        "ldr	r5, [sp, #92]\n\t"
#else
        "ldrd	r4, r5, [sp, #88]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #80]\n\t"
        "ldr	r5, [sp, #84]\n\t"
#else
        "ldrd	r4, r5, [sp, #80]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #80]\n\t"
        "str	r5, [sp, #84]\n\t"
#else
        "strd	r4, r5, [sp, #80]\n\t"
#endif
        /* Round 11 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #88]\n\t"
        "ldr	r9, [sp, #92]\n\t"
#else
        "ldrd	r8, r9, [sp, #88]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #88]\n\t"
        "ldr	r7, [r3, #92]\n\t"
#else
        "ldrd	r6, r7, [r3, #88]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512]]\n\t"
        "str	r9, [%[sha512], #4]\n\t"
#else
        "strd	r8, r9, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #32]\n\t"
        "str	r7, [%[sha512], #36]\n\t"
#else
        "strd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[11] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #72]\n\t"
        "ldr	r5, [sp, #76]\n\t"
#else
        "ldrd	r4, r5, [sp, #72]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #88]\n\t"
        "ldr	r5, [sp, #92]\n\t"
#else
        "ldrd	r4, r5, [sp, #88]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #32]\n\t"
        "ldr	r9, [sp, #36]\n\t"
#else
        "ldrd	r8, r9, [sp, #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #88]\n\t"
        "str	r5, [sp, #92]\n\t"
#else
        "strd	r4, r5, [sp, #88]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #88]\n\t"
        "ldr	r5, [sp, #92]\n\t"
#else
        "ldrd	r4, r5, [sp, #88]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #88]\n\t"
        "str	r5, [sp, #92]\n\t"
#else
        "strd	r4, r5, [sp, #88]\n\t"
#endif
        /* Round 12 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #96]\n\t"
        "ldr	r9, [sp, #100]\n\t"
#else
        "ldrd	r8, r9, [sp, #96]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #96]\n\t"
        "ldr	r7, [r3, #100]\n\t"
#else
        "ldrd	r6, r7, [r3, #96]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #56]\n\t"
        "str	r9, [%[sha512], #60]\n\t"
#else
        "strd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #24]\n\t"
        "str	r7, [%[sha512], #28]\n\t"
#else
        "strd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[12] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #80]\n\t"
        "ldr	r5, [sp, #84]\n\t"
#else
        "ldrd	r4, r5, [sp, #80]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #40]\n\t"
        "ldr	r9, [sp, #44]\n\t"
#else
        "ldrd	r8, r9, [sp, #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
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
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #96]\n\t"
        "str	r5, [sp, #100]\n\t"
#else
        "strd	r4, r5, [sp, #96]\n\t"
#endif
        /* Round 13 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #104]\n\t"
        "ldr	r9, [sp, #108]\n\t"
#else
        "ldrd	r8, r9, [sp, #104]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #104]\n\t"
        "ldr	r7, [r3, #108]\n\t"
#else
        "ldrd	r6, r7, [r3, #104]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #48]\n\t"
        "str	r9, [%[sha512], #52]\n\t"
#else
        "strd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #16]\n\t"
        "str	r7, [%[sha512], #20]\n\t"
#else
        "strd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[13] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #88]\n\t"
        "ldr	r5, [sp, #92]\n\t"
#else
        "ldrd	r4, r5, [sp, #88]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #104]\n\t"
        "ldr	r5, [sp, #108]\n\t"
#else
        "ldrd	r4, r5, [sp, #104]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #48]\n\t"
        "ldr	r9, [sp, #52]\n\t"
#else
        "ldrd	r8, r9, [sp, #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
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
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #104]\n\t"
        "ldr	r5, [sp, #108]\n\t"
#else
        "ldrd	r4, r5, [sp, #104]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #104]\n\t"
        "str	r5, [sp, #108]\n\t"
#else
        "strd	r4, r5, [sp, #104]\n\t"
#endif
        /* Round 14 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #112]\n\t"
        "ldr	r9, [sp, #116]\n\t"
#else
        "ldrd	r8, r9, [sp, #112]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #112]\n\t"
        "ldr	r7, [r3, #116]\n\t"
#else
        "ldrd	r6, r7, [r3, #112]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #40]\n\t"
        "str	r9, [%[sha512], #44]\n\t"
#else
        "strd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #8]\n\t"
        "str	r7, [%[sha512], #12]\n\t"
#else
        "strd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[14] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #96]\n\t"
        "ldr	r5, [sp, #100]\n\t"
#else
        "ldrd	r4, r5, [sp, #96]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #112]\n\t"
        "ldr	r5, [sp, #116]\n\t"
#else
        "ldrd	r4, r5, [sp, #112]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #56]\n\t"
        "ldr	r9, [sp, #60]\n\t"
#else
        "ldrd	r8, r9, [sp, #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #112]\n\t"
        "str	r5, [sp, #116]\n\t"
#else
        "strd	r4, r5, [sp, #112]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #120]\n\t"
        "ldr	r5, [sp, #124]\n\t"
#else
        "ldrd	r4, r5, [sp, #120]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #112]\n\t"
        "ldr	r5, [sp, #116]\n\t"
#else
        "ldrd	r4, r5, [sp, #112]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #112]\n\t"
        "str	r5, [sp, #116]\n\t"
#else
        "strd	r4, r5, [sp, #112]\n\t"
#endif
        /* Round 15 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #120]\n\t"
        "ldr	r9, [sp, #124]\n\t"
#else
        "ldrd	r8, r9, [sp, #120]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #120]\n\t"
        "ldr	r7, [r3, #124]\n\t"
#else
        "ldrd	r6, r7, [r3, #120]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #32]\n\t"
        "str	r9, [%[sha512], #36]\n\t"
#else
        "strd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512]]\n\t"
        "str	r7, [%[sha512], #4]\n\t"
#else
        "strd	r6, r7, [%[sha512]]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Calc new W[15] */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #104]\n\t"
        "ldr	r5, [sp, #108]\n\t"
#else
        "ldrd	r4, r5, [sp, #104]\n\t"
#endif
        "lsrs	r6, r4, #19\n\t"
        "lsrs	r7, r5, #19\n\t"
        "orr	r7, r7, r4, lsl #13\n\t"
        "orr	r6, r6, r5, lsl #13\n\t"
        "lsls	r8, r4, #3\n\t"
        "lsls	r9, r5, #3\n\t"
        "orr	r9, r9, r4, lsr #29\n\t"
        "orr	r8, r8, r5, lsr #29\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #6\n\t"
        "lsrs	r9, r5, #6\n\t"
        "orr	r8, r8, r5, lsl #26\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #120]\n\t"
        "ldr	r5, [sp, #124]\n\t"
#else
        "ldrd	r4, r5, [sp, #120]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #64]\n\t"
        "ldr	r9, [sp, #68]\n\t"
#else
        "ldrd	r8, r9, [sp, #64]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #120]\n\t"
        "str	r5, [sp, #124]\n\t"
#else
        "strd	r4, r5, [sp, #120]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp]\n\t"
        "ldr	r5, [sp, #4]\n\t"
#else
        "ldrd	r4, r5, [sp]\n\t"
#endif
        "lsrs	r6, r4, #1\n\t"
        "lsrs	r7, r5, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r6, r6, r5, lsl #31\n\t"
        "lsrs	r8, r4, #8\n\t"
        "lsrs	r9, r5, #8\n\t"
        "orr	r9, r9, r4, lsl #24\n\t"
        "orr	r8, r8, r5, lsl #24\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
        "lsrs	r8, r4, #7\n\t"
        "lsrs	r9, r5, #7\n\t"
        "orr	r8, r8, r5, lsl #25\n\t"
        "eor	r7, r7, r9\n\t"
        "eor	r6, r6, r8\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [sp, #120]\n\t"
        "ldr	r5, [sp, #124]\n\t"
#else
        "ldrd	r4, r5, [sp, #120]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #120]\n\t"
        "str	r5, [sp, #124]\n\t"
#else
        "strd	r4, r5, [sp, #120]\n\t"
#endif
        "add	r3, r3, #0x80\n\t"
        "subs	r12, r12, #1\n\t"
        "bne	L_SHA512_transform_len_start_%=\n\t"
        /* Round 0 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp]\n\t"
        "ldr	r9, [sp, #4]\n\t"
#else
        "ldrd	r8, r9, [sp]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3]\n\t"
        "ldr	r7, [r3, #4]\n\t"
#else
        "ldrd	r6, r7, [r3]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #24]\n\t"
        "str	r9, [%[sha512], #28]\n\t"
#else
        "strd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #56]\n\t"
        "str	r7, [%[sha512], #60]\n\t"
#else
        "strd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 1 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #8]\n\t"
        "ldr	r9, [sp, #12]\n\t"
#else
        "ldrd	r8, r9, [sp, #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #8]\n\t"
        "ldr	r7, [r3, #12]\n\t"
#else
        "ldrd	r6, r7, [r3, #8]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #16]\n\t"
        "str	r9, [%[sha512], #20]\n\t"
#else
        "strd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #48]\n\t"
        "str	r7, [%[sha512], #52]\n\t"
#else
        "strd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 2 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #16]\n\t"
        "ldr	r9, [sp, #20]\n\t"
#else
        "ldrd	r8, r9, [sp, #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #16]\n\t"
        "ldr	r7, [r3, #20]\n\t"
#else
        "ldrd	r6, r7, [r3, #16]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #8]\n\t"
        "str	r9, [%[sha512], #12]\n\t"
#else
        "strd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #40]\n\t"
        "str	r7, [%[sha512], #44]\n\t"
#else
        "strd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 3 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #24]\n\t"
        "ldr	r9, [sp, #28]\n\t"
#else
        "ldrd	r8, r9, [sp, #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #24]\n\t"
        "ldr	r7, [r3, #28]\n\t"
#else
        "ldrd	r6, r7, [r3, #24]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512]]\n\t"
        "str	r9, [%[sha512], #4]\n\t"
#else
        "strd	r8, r9, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #32]\n\t"
        "str	r7, [%[sha512], #36]\n\t"
#else
        "strd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 4 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #32]\n\t"
        "ldr	r9, [sp, #36]\n\t"
#else
        "ldrd	r8, r9, [sp, #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #32]\n\t"
        "ldr	r7, [r3, #36]\n\t"
#else
        "ldrd	r6, r7, [r3, #32]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #56]\n\t"
        "str	r9, [%[sha512], #60]\n\t"
#else
        "strd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #24]\n\t"
        "str	r7, [%[sha512], #28]\n\t"
#else
        "strd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 5 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #40]\n\t"
        "ldr	r9, [sp, #44]\n\t"
#else
        "ldrd	r8, r9, [sp, #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #40]\n\t"
        "ldr	r7, [r3, #44]\n\t"
#else
        "ldrd	r6, r7, [r3, #40]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #48]\n\t"
        "str	r9, [%[sha512], #52]\n\t"
#else
        "strd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #16]\n\t"
        "str	r7, [%[sha512], #20]\n\t"
#else
        "strd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 6 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #48]\n\t"
        "ldr	r9, [sp, #52]\n\t"
#else
        "ldrd	r8, r9, [sp, #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #48]\n\t"
        "ldr	r7, [r3, #52]\n\t"
#else
        "ldrd	r6, r7, [r3, #48]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #40]\n\t"
        "str	r9, [%[sha512], #44]\n\t"
#else
        "strd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #8]\n\t"
        "str	r7, [%[sha512], #12]\n\t"
#else
        "strd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 7 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #56]\n\t"
        "ldr	r9, [sp, #60]\n\t"
#else
        "ldrd	r8, r9, [sp, #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #56]\n\t"
        "ldr	r7, [r3, #60]\n\t"
#else
        "ldrd	r6, r7, [r3, #56]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #32]\n\t"
        "str	r9, [%[sha512], #36]\n\t"
#else
        "strd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512]]\n\t"
        "str	r7, [%[sha512], #4]\n\t"
#else
        "strd	r6, r7, [%[sha512]]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 8 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #64]\n\t"
        "ldr	r9, [sp, #68]\n\t"
#else
        "ldrd	r8, r9, [sp, #64]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #64]\n\t"
        "ldr	r7, [r3, #68]\n\t"
#else
        "ldrd	r6, r7, [r3, #64]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #24]\n\t"
        "str	r9, [%[sha512], #28]\n\t"
#else
        "strd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #56]\n\t"
        "str	r5, [%[sha512], #60]\n\t"
#else
        "strd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #56]\n\t"
        "str	r7, [%[sha512], #60]\n\t"
#else
        "strd	r6, r7, [%[sha512], #56]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 9 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #72]\n\t"
        "ldr	r9, [sp, #76]\n\t"
#else
        "ldrd	r8, r9, [sp, #72]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #72]\n\t"
        "ldr	r7, [r3, #76]\n\t"
#else
        "ldrd	r6, r7, [r3, #72]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #16]\n\t"
        "str	r9, [%[sha512], #20]\n\t"
#else
        "strd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #48]\n\t"
        "str	r7, [%[sha512], #52]\n\t"
#else
        "strd	r6, r7, [%[sha512], #48]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 10 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #80]\n\t"
        "ldr	r9, [sp, #84]\n\t"
#else
        "ldrd	r8, r9, [sp, #80]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #80]\n\t"
        "ldr	r7, [r3, #84]\n\t"
#else
        "ldrd	r6, r7, [r3, #80]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #8]\n\t"
        "str	r9, [%[sha512], #12]\n\t"
#else
        "strd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #40]\n\t"
        "str	r5, [%[sha512], #44]\n\t"
#else
        "strd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #40]\n\t"
        "str	r7, [%[sha512], #44]\n\t"
#else
        "strd	r6, r7, [%[sha512], #40]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 11 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #88]\n\t"
        "ldr	r9, [sp, #92]\n\t"
#else
        "ldrd	r8, r9, [sp, #88]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #88]\n\t"
        "ldr	r7, [r3, #92]\n\t"
#else
        "ldrd	r6, r7, [r3, #88]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512]]\n\t"
        "str	r9, [%[sha512], #4]\n\t"
#else
        "strd	r8, r9, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #32]\n\t"
        "str	r7, [%[sha512], #36]\n\t"
#else
        "strd	r6, r7, [%[sha512], #32]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 12 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #96]\n\t"
        "ldr	r9, [sp, #100]\n\t"
#else
        "ldrd	r8, r9, [sp, #96]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #96]\n\t"
        "ldr	r7, [r3, #100]\n\t"
#else
        "ldrd	r6, r7, [r3, #96]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #56]\n\t"
        "str	r9, [%[sha512], #60]\n\t"
#else
        "strd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #24]\n\t"
        "str	r5, [%[sha512], #28]\n\t"
#else
        "strd	r4, r5, [%[sha512], #24]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #24]\n\t"
        "str	r7, [%[sha512], #28]\n\t"
#else
        "strd	r6, r7, [%[sha512], #24]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 13 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #56]\n\t"
        "ldr	r5, [%[sha512], #60]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #104]\n\t"
        "ldr	r9, [sp, #108]\n\t"
#else
        "ldrd	r8, r9, [sp, #104]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #104]\n\t"
        "ldr	r7, [r3, #108]\n\t"
#else
        "ldrd	r6, r7, [r3, #104]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #48]\n\t"
        "ldr	r9, [%[sha512], #52]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #24]\n\t"
        "ldr	r5, [%[sha512], #28]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #48]\n\t"
        "str	r9, [%[sha512], #52]\n\t"
#else
        "strd	r8, r9, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #24]\n\t"
        "ldr	r9, [%[sha512], #28]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #32]\n\t"
        "ldr	r7, [%[sha512], #36]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #16]\n\t"
        "str	r7, [%[sha512], #20]\n\t"
#else
        "strd	r6, r7, [%[sha512], #16]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 14 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512]]\n\t"
        "ldr	r9, [%[sha512], #4]\n\t"
#else
        "ldrd	r8, r9, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #112]\n\t"
        "ldr	r9, [sp, #116]\n\t"
#else
        "ldrd	r8, r9, [sp, #112]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #112]\n\t"
        "ldr	r7, [r3, #116]\n\t"
#else
        "ldrd	r6, r7, [r3, #112]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #40]\n\t"
        "ldr	r9, [%[sha512], #44]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #40]\n\t"
        "str	r9, [%[sha512], #44]\n\t"
#else
        "strd	r8, r9, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #16]\n\t"
        "ldr	r9, [%[sha512], #20]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #8]\n\t"
        "str	r5, [%[sha512], #12]\n\t"
#else
        "strd	r4, r5, [%[sha512], #8]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #8]\n\t"
        "str	r7, [%[sha512], #12]\n\t"
#else
        "strd	r6, r7, [%[sha512], #8]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Round 15 */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
        "lsrs	r6, r4, #14\n\t"
        "lsrs	r7, r5, #14\n\t"
        "orr	r7, r7, r4, lsl #18\n\t"
        "orr	r6, r6, r5, lsl #18\n\t"
        "lsrs	r8, r4, #18\n\t"
        "lsrs	r9, r5, #18\n\t"
        "orr	r9, r9, r4, lsl #14\n\t"
        "orr	r8, r8, r5, lsl #14\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #23\n\t"
        "lsls	r9, r5, #23\n\t"
        "orr	r9, r9, r4, lsr #9\n\t"
        "orr	r8, r8, r5, lsr #9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #40]\n\t"
        "ldr	r5, [%[sha512], #44]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #48]\n\t"
        "ldr	r7, [%[sha512], #52]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #56]\n\t"
        "ldr	r9, [%[sha512], #60]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #56]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "and	r6, r6, r4\n\t"
        "and	r7, r7, r5\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #120]\n\t"
        "ldr	r9, [sp, #124]\n\t"
#else
        "ldrd	r8, r9, [sp, #120]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [r3, #120]\n\t"
        "ldr	r7, [r3, #124]\n\t"
#else
        "ldrd	r6, r7, [r3, #120]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #32]\n\t"
        "ldr	r9, [%[sha512], #36]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "adds	r8, r8, r4\n\t"
        "adc	r9, r9, r5\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #8]\n\t"
        "ldr	r5, [%[sha512], #12]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r8, [%[sha512], #32]\n\t"
        "str	r9, [%[sha512], #36]\n\t"
#else
        "strd	r8, r9, [%[sha512], #32]\n\t"
#endif
        "lsrs	r6, r4, #28\n\t"
        "lsrs	r7, r5, #28\n\t"
        "orr	r7, r7, r4, lsl #4\n\t"
        "orr	r6, r6, r5, lsl #4\n\t"
        "lsls	r8, r4, #30\n\t"
        "lsls	r9, r5, #30\n\t"
        "orr	r9, r9, r4, lsr #2\n\t"
        "orr	r8, r8, r5, lsr #2\n\t"
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "lsls	r8, r4, #25\n\t"
        "lsls	r9, r5, #25\n\t"
        "orr	r9, r9, r4, lsr #7\n\t"
        "orr	r8, r8, r5, lsr #7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r6, r6, r8\n\t"
        "eor	r7, r7, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [%[sha512], #8]\n\t"
        "ldr	r9, [%[sha512], #12]\n\t"
#else
        "ldrd	r8, r9, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #16]\n\t"
        "ldr	r7, [%[sha512], #20]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
        "eor	r8, r8, r6\n\t"
        "eor	r9, r9, r7\n\t"
        "and	r10, r10, r8\n\t"
        "and	r11, r11, r9\n\t"
        "eor	r10, r10, r6\n\t"
        "eor	r11, r11, r7\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512]]\n\t"
        "ldr	r7, [%[sha512], #4]\n\t"
#else
        "ldrd	r6, r7, [%[sha512]]\n\t"
#endif
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512]]\n\t"
        "str	r7, [%[sha512], #4]\n\t"
#else
        "strd	r6, r7, [%[sha512]]\n\t"
#endif
        "mov	r10, r8\n\t"
        "mov	r11, r9\n\t"
        /* Add in digest from start */
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512]]\n\t"
        "ldr	r5, [%[sha512], #4]\n\t"
#else
        "ldrd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #8]\n\t"
        "ldr	r7, [%[sha512], #12]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #8]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #128]\n\t"
        "ldr	r9, [sp, #132]\n\t"
#else
        "ldrd	r8, r9, [sp, #128]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #136]\n\t"
        "ldr	r11, [sp, #140]\n\t"
#else
        "ldrd	r10, r11, [sp, #136]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512]]\n\t"
        "str	r5, [%[sha512], #4]\n\t"
#else
        "strd	r4, r5, [%[sha512]]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #8]\n\t"
        "str	r7, [%[sha512], #12]\n\t"
#else
        "strd	r6, r7, [%[sha512], #8]\n\t"
#endif
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
        "ldr	r4, [%[sha512], #16]\n\t"
        "ldr	r5, [%[sha512], #20]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #24]\n\t"
        "ldr	r7, [%[sha512], #28]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #24]\n\t"
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
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #16]\n\t"
        "str	r5, [%[sha512], #20]\n\t"
#else
        "strd	r4, r5, [%[sha512], #16]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #24]\n\t"
        "str	r7, [%[sha512], #28]\n\t"
#else
        "strd	r6, r7, [%[sha512], #24]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #144]\n\t"
        "str	r5, [sp, #148]\n\t"
#else
        "strd	r4, r5, [sp, #144]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #152]\n\t"
        "str	r7, [sp, #156]\n\t"
#else
        "strd	r6, r7, [sp, #152]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #32]\n\t"
        "ldr	r5, [%[sha512], #36]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #40]\n\t"
        "ldr	r7, [%[sha512], #44]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #160]\n\t"
        "ldr	r9, [sp, #164]\n\t"
#else
        "ldrd	r8, r9, [sp, #160]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #168]\n\t"
        "ldr	r11, [sp, #172]\n\t"
#else
        "ldrd	r10, r11, [sp, #168]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #32]\n\t"
        "str	r5, [%[sha512], #36]\n\t"
#else
        "strd	r4, r5, [%[sha512], #32]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #40]\n\t"
        "str	r7, [%[sha512], #44]\n\t"
#else
        "strd	r6, r7, [%[sha512], #40]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #160]\n\t"
        "str	r5, [sp, #164]\n\t"
#else
        "strd	r4, r5, [sp, #160]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #168]\n\t"
        "str	r7, [sp, #172]\n\t"
#else
        "strd	r6, r7, [sp, #168]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r4, [%[sha512], #48]\n\t"
        "ldr	r5, [%[sha512], #52]\n\t"
#else
        "ldrd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r6, [%[sha512], #56]\n\t"
        "ldr	r7, [%[sha512], #60]\n\t"
#else
        "ldrd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r8, [sp, #176]\n\t"
        "ldr	r9, [sp, #180]\n\t"
#else
        "ldrd	r8, r9, [sp, #176]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "ldr	r10, [sp, #184]\n\t"
        "ldr	r11, [sp, #188]\n\t"
#else
        "ldrd	r10, r11, [sp, #184]\n\t"
#endif
        "adds	r4, r4, r8\n\t"
        "adc	r5, r5, r9\n\t"
        "adds	r6, r6, r10\n\t"
        "adc	r7, r7, r11\n\t"
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [%[sha512], #48]\n\t"
        "str	r5, [%[sha512], #52]\n\t"
#else
        "strd	r4, r5, [%[sha512], #48]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [%[sha512], #56]\n\t"
        "str	r7, [%[sha512], #60]\n\t"
#else
        "strd	r6, r7, [%[sha512], #56]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r4, [sp, #176]\n\t"
        "str	r5, [sp, #180]\n\t"
#else
        "strd	r4, r5, [sp, #176]\n\t"
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
        "str	r6, [sp, #184]\n\t"
        "str	r7, [sp, #188]\n\t"
#else
        "strd	r6, r7, [sp, #184]\n\t"
#endif
        "subs	%[len], %[len], #0x80\n\t"
        "sub	r3, r3, #0x200\n\t"
        "add	%[data], %[data], #0x80\n\t"
        "bne	L_SHA512_transform_len_begin_%=\n\t"
        "eor	r0, r0, r0\n\t"
        "add	sp, sp, #0xc0\n\t"
        : [sha512] "+r" (sha512), [data] "+r" (data), [len] "+r" (len)
        : [L_SHA512_transform_len_k] "r" (L_SHA512_transform_len_k)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12"
    );
}

#endif /* WOLFSSL_ARMASM_NO_NEON */
#include <wolfssl/wolfcrypt/sha512.h>

#ifndef WOLFSSL_ARMASM_NO_NEON
static const uint64_t L_SHA512_transform_neon_len_k[] = {
    0x428a2f98d728ae22UL,
    0x7137449123ef65cdUL,
    0xb5c0fbcfec4d3b2fUL,
    0xe9b5dba58189dbbcUL,
    0x3956c25bf348b538UL,
    0x59f111f1b605d019UL,
    0x923f82a4af194f9bUL,
    0xab1c5ed5da6d8118UL,
    0xd807aa98a3030242UL,
    0x12835b0145706fbeUL,
    0x243185be4ee4b28cUL,
    0x550c7dc3d5ffb4e2UL,
    0x72be5d74f27b896fUL,
    0x80deb1fe3b1696b1UL,
    0x9bdc06a725c71235UL,
    0xc19bf174cf692694UL,
    0xe49b69c19ef14ad2UL,
    0xefbe4786384f25e3UL,
    0xfc19dc68b8cd5b5UL,
    0x240ca1cc77ac9c65UL,
    0x2de92c6f592b0275UL,
    0x4a7484aa6ea6e483UL,
    0x5cb0a9dcbd41fbd4UL,
    0x76f988da831153b5UL,
    0x983e5152ee66dfabUL,
    0xa831c66d2db43210UL,
    0xb00327c898fb213fUL,
    0xbf597fc7beef0ee4UL,
    0xc6e00bf33da88fc2UL,
    0xd5a79147930aa725UL,
    0x6ca6351e003826fUL,
    0x142929670a0e6e70UL,
    0x27b70a8546d22ffcUL,
    0x2e1b21385c26c926UL,
    0x4d2c6dfc5ac42aedUL,
    0x53380d139d95b3dfUL,
    0x650a73548baf63deUL,
    0x766a0abb3c77b2a8UL,
    0x81c2c92e47edaee6UL,
    0x92722c851482353bUL,
    0xa2bfe8a14cf10364UL,
    0xa81a664bbc423001UL,
    0xc24b8b70d0f89791UL,
    0xc76c51a30654be30UL,
    0xd192e819d6ef5218UL,
    0xd69906245565a910UL,
    0xf40e35855771202aUL,
    0x106aa07032bbd1b8UL,
    0x19a4c116b8d2d0c8UL,
    0x1e376c085141ab53UL,
    0x2748774cdf8eeb99UL,
    0x34b0bcb5e19b48a8UL,
    0x391c0cb3c5c95a63UL,
    0x4ed8aa4ae3418acbUL,
    0x5b9cca4f7763e373UL,
    0x682e6ff3d6b2b8a3UL,
    0x748f82ee5defb2fcUL,
    0x78a5636f43172f60UL,
    0x84c87814a1f0ab72UL,
    0x8cc702081a6439ecUL,
    0x90befffa23631e28UL,
    0xa4506cebde82bde9UL,
    0xbef9a3f7b2c67915UL,
    0xc67178f2e372532bUL,
    0xca273eceea26619cUL,
    0xd186b8c721c0c207UL,
    0xeada7dd6cde0eb1eUL,
    0xf57d4f7fee6ed178UL,
    0x6f067aa72176fbaUL,
    0xa637dc5a2c898a6UL,
    0x113f9804bef90daeUL,
    0x1b710b35131c471bUL,
    0x28db77f523047d84UL,
    0x32caab7b40c72493UL,
    0x3c9ebe0a15c9bebcUL,
    0x431d67c49c100d4cUL,
    0x4cc5d4becb3e42b6UL,
    0x597f299cfc657e2aUL,
    0x5fcb6fab3ad6faecUL,
    0x6c44198c4a475817UL,
};

void Transform_Sha512_Len(wc_Sha512* sha512_p, const byte* data_p, word32 len_p);
void Transform_Sha512_Len(wc_Sha512* sha512_p, const byte* data_p, word32 len_p)
{
    register wc_Sha512* sha512 asm ("r0") = sha512_p;
    register const byte* data asm ("r1") = data_p;
    register word32 len asm ("r2") = len_p;

    __asm__ __volatile__ (
        /* Load digest into working vars */
        "vldm.64	%[sha512], {d0-d7}\n\t"
        /* Start of loop processing a block */
        "\n"
    "L_SHA512_transform_neon_len_begin_%=: \n\t"
        /* Load W */
        "vld1.8	{q8-q9}, [%[data]]!\n\t"
        "vld1.8	{q10-q11}, [%[data]]!\n\t"
        "vld1.8	{q12-q13}, [%[data]]!\n\t"
        "vld1.8	{q14-q15}, [%[data]]!\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        "vrev64.8	q8, q8\n\t"
        "vrev64.8	q9, q9\n\t"
        "vrev64.8	q10, q10\n\t"
        "vrev64.8	q11, q11\n\t"
        "vrev64.8	q12, q12\n\t"
        "vrev64.8	q13, q13\n\t"
        "vrev64.8	q14, q14\n\t"
        "vrev64.8	q15, q15\n\t"
#else
        "vrev64.8	d16, d16\n\t"
        "vrev64.8	d17, d17\n\t"
        "vrev64.8	d18, d18\n\t"
        "vrev64.8	d19, d19\n\t"
        "vrev64.8	d20, d20\n\t"
        "vrev64.8	d21, d21\n\t"
        "vrev64.8	d22, d22\n\t"
        "vrev64.8	d23, d23\n\t"
        "vrev64.8	d24, d24\n\t"
        "vrev64.8	d25, d25\n\t"
        "vrev64.8	d26, d26\n\t"
        "vrev64.8	d27, d27\n\t"
        "vrev64.8	d28, d28\n\t"
        "vrev64.8	d29, d29\n\t"
        "vrev64.8	d30, d30\n\t"
        "vrev64.8	d31, d31\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        "mov	r3, %[L_SHA512_transform_neon_len_k]\n\t"
        "mov	r12, #4\n\t"
        /* Start of 16 rounds */
        "\n"
    "L_SHA512_transform_neon_len_start_%=: \n\t"
        /* Round 0 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d4, #50\n\t"
        "vsri.u64	d8, d4, #14\n\t"
        "vshl.u64	d9, d0, #36\n\t"
        "vsri.u64	d9, d0, #28\n\t"
        "vshl.u64	d10, d4, #46\n\t"
        "vsri.u64	d10, d4, #18\n\t"
        "vshl.u64	d11, d0, #30\n\t"
        "vsri.u64	d11, d0, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d4, #23\n\t"
        "vsri.u64	d10, d4, #41\n\t"
        "vshl.u64	d11, d0, #25\n\t"
        "vsri.u64	d11, d0, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d12, d16\n\t"
        "vmov	d8, d4\n\t"
        "veor	d10, d1, d2\n\t"
        "vadd.i64	d7, d12\n\t"
        "vbsl	d8, d5, d6\n\t"
        "vbsl	d10, d0, d2\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d3, d7\n\t"
        "vadd.i64	d7, d10\n\t"
        /* Round 1 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d3, #50\n\t"
        "vsri.u64	d8, d3, #14\n\t"
        "vshl.u64	d9, d7, #36\n\t"
        "vsri.u64	d9, d7, #28\n\t"
        "vshl.u64	d10, d3, #46\n\t"
        "vsri.u64	d10, d3, #18\n\t"
        "vshl.u64	d11, d7, #30\n\t"
        "vsri.u64	d11, d7, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d3, #23\n\t"
        "vsri.u64	d10, d3, #41\n\t"
        "vshl.u64	d11, d7, #25\n\t"
        "vsri.u64	d11, d7, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d12, d17\n\t"
        "vmov	d8, d3\n\t"
        "veor	d10, d0, d1\n\t"
        "vadd.i64	d6, d12\n\t"
        "vbsl	d8, d4, d5\n\t"
        "vbsl	d10, d7, d1\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d2, d6\n\t"
        "vadd.i64	d6, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[0]-W[1] */
        "vext.8	q6, q8, q9, #8\n\t"
        "vshl.u64	q4, q15, #45\n\t"
        "vsri.u64	q4, q15, #19\n\t"
        "vshl.u64	q5, q15, #3\n\t"
        "vsri.u64	q5, q15, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q15, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q8, q5\n\t"
        "vext.8	q7, q12, q13, #8\n\t"
        "vadd.i64	q8, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q8, q5\n\t"
#else
        /* Calc new W[0]-W[1] */
        "vmov	d12, d17\n\t"
        "vmov	d13, d18\n\t"
        "vshl.u64	d8, d30, #45\n\t"
        "vshl.u64	d9, d31, #45\n\t"
        "vsri.u64	d8, d30, #19\n\t"
        "vsri.u64	d9, d31, #19\n\t"
        "vshl.u64	d10, d30, #3\n\t"
        "vshl.u64	d11, d31, #3\n\t"
        "vsri.u64	d10, d30, #61\n\t"
        "vsri.u64	d11, d31, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d30, #6\n\t"
        "vshr.u64	d9, d31, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d16, d10\n\t"
        "vadd.i64	d17, d11\n\t"
        "vmov	d14, d25\n\t"
        "vmov	d15, d26\n\t"
        "vadd.i64	d16, d14\n\t"
        "vadd.i64	d17, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d16, d10\n\t"
        "vadd.i64	d17, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 2 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d2, #50\n\t"
        "vsri.u64	d8, d2, #14\n\t"
        "vshl.u64	d9, d6, #36\n\t"
        "vsri.u64	d9, d6, #28\n\t"
        "vshl.u64	d10, d2, #46\n\t"
        "vsri.u64	d10, d2, #18\n\t"
        "vshl.u64	d11, d6, #30\n\t"
        "vsri.u64	d11, d6, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d2, #23\n\t"
        "vsri.u64	d10, d2, #41\n\t"
        "vshl.u64	d11, d6, #25\n\t"
        "vsri.u64	d11, d6, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d12, d18\n\t"
        "vmov	d8, d2\n\t"
        "veor	d10, d7, d0\n\t"
        "vadd.i64	d5, d12\n\t"
        "vbsl	d8, d3, d4\n\t"
        "vbsl	d10, d6, d0\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d1, d5\n\t"
        "vadd.i64	d5, d10\n\t"
        /* Round 3 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d1, #50\n\t"
        "vsri.u64	d8, d1, #14\n\t"
        "vshl.u64	d9, d5, #36\n\t"
        "vsri.u64	d9, d5, #28\n\t"
        "vshl.u64	d10, d1, #46\n\t"
        "vsri.u64	d10, d1, #18\n\t"
        "vshl.u64	d11, d5, #30\n\t"
        "vsri.u64	d11, d5, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d1, #23\n\t"
        "vsri.u64	d10, d1, #41\n\t"
        "vshl.u64	d11, d5, #25\n\t"
        "vsri.u64	d11, d5, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d12, d19\n\t"
        "vmov	d8, d1\n\t"
        "veor	d10, d6, d7\n\t"
        "vadd.i64	d4, d12\n\t"
        "vbsl	d8, d2, d3\n\t"
        "vbsl	d10, d5, d7\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d0, d4\n\t"
        "vadd.i64	d4, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[2]-W[3] */
        "vext.8	q6, q9, q10, #8\n\t"
        "vshl.u64	q4, q8, #45\n\t"
        "vsri.u64	q4, q8, #19\n\t"
        "vshl.u64	q5, q8, #3\n\t"
        "vsri.u64	q5, q8, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q8, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q9, q5\n\t"
        "vext.8	q7, q13, q14, #8\n\t"
        "vadd.i64	q9, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q9, q5\n\t"
#else
        /* Calc new W[2]-W[3] */
        "vmov	d12, d19\n\t"
        "vmov	d13, d20\n\t"
        "vshl.u64	d8, d16, #45\n\t"
        "vshl.u64	d9, d17, #45\n\t"
        "vsri.u64	d8, d16, #19\n\t"
        "vsri.u64	d9, d17, #19\n\t"
        "vshl.u64	d10, d16, #3\n\t"
        "vshl.u64	d11, d17, #3\n\t"
        "vsri.u64	d10, d16, #61\n\t"
        "vsri.u64	d11, d17, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d16, #6\n\t"
        "vshr.u64	d9, d17, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d18, d10\n\t"
        "vadd.i64	d19, d11\n\t"
        "vmov	d14, d27\n\t"
        "vmov	d15, d28\n\t"
        "vadd.i64	d18, d14\n\t"
        "vadd.i64	d19, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d18, d10\n\t"
        "vadd.i64	d19, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 4 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d0, #50\n\t"
        "vsri.u64	d8, d0, #14\n\t"
        "vshl.u64	d9, d4, #36\n\t"
        "vsri.u64	d9, d4, #28\n\t"
        "vshl.u64	d10, d0, #46\n\t"
        "vsri.u64	d10, d0, #18\n\t"
        "vshl.u64	d11, d4, #30\n\t"
        "vsri.u64	d11, d4, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d0, #23\n\t"
        "vsri.u64	d10, d0, #41\n\t"
        "vshl.u64	d11, d4, #25\n\t"
        "vsri.u64	d11, d4, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d12, d20\n\t"
        "vmov	d8, d0\n\t"
        "veor	d10, d5, d6\n\t"
        "vadd.i64	d3, d12\n\t"
        "vbsl	d8, d1, d2\n\t"
        "vbsl	d10, d4, d6\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d7, d3\n\t"
        "vadd.i64	d3, d10\n\t"
        /* Round 5 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d7, #50\n\t"
        "vsri.u64	d8, d7, #14\n\t"
        "vshl.u64	d9, d3, #36\n\t"
        "vsri.u64	d9, d3, #28\n\t"
        "vshl.u64	d10, d7, #46\n\t"
        "vsri.u64	d10, d7, #18\n\t"
        "vshl.u64	d11, d3, #30\n\t"
        "vsri.u64	d11, d3, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d7, #23\n\t"
        "vsri.u64	d10, d7, #41\n\t"
        "vshl.u64	d11, d3, #25\n\t"
        "vsri.u64	d11, d3, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d12, d21\n\t"
        "vmov	d8, d7\n\t"
        "veor	d10, d4, d5\n\t"
        "vadd.i64	d2, d12\n\t"
        "vbsl	d8, d0, d1\n\t"
        "vbsl	d10, d3, d5\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d6, d2\n\t"
        "vadd.i64	d2, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[4]-W[5] */
        "vext.8	q6, q10, q11, #8\n\t"
        "vshl.u64	q4, q9, #45\n\t"
        "vsri.u64	q4, q9, #19\n\t"
        "vshl.u64	q5, q9, #3\n\t"
        "vsri.u64	q5, q9, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q9, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q10, q5\n\t"
        "vext.8	q7, q14, q15, #8\n\t"
        "vadd.i64	q10, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q10, q5\n\t"
#else
        /* Calc new W[4]-W[5] */
        "vmov	d12, d21\n\t"
        "vmov	d13, d22\n\t"
        "vshl.u64	d8, d18, #45\n\t"
        "vshl.u64	d9, d19, #45\n\t"
        "vsri.u64	d8, d18, #19\n\t"
        "vsri.u64	d9, d19, #19\n\t"
        "vshl.u64	d10, d18, #3\n\t"
        "vshl.u64	d11, d19, #3\n\t"
        "vsri.u64	d10, d18, #61\n\t"
        "vsri.u64	d11, d19, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d18, #6\n\t"
        "vshr.u64	d9, d19, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d20, d10\n\t"
        "vadd.i64	d21, d11\n\t"
        "vmov	d14, d29\n\t"
        "vmov	d15, d30\n\t"
        "vadd.i64	d20, d14\n\t"
        "vadd.i64	d21, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d20, d10\n\t"
        "vadd.i64	d21, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 6 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d6, #50\n\t"
        "vsri.u64	d8, d6, #14\n\t"
        "vshl.u64	d9, d2, #36\n\t"
        "vsri.u64	d9, d2, #28\n\t"
        "vshl.u64	d10, d6, #46\n\t"
        "vsri.u64	d10, d6, #18\n\t"
        "vshl.u64	d11, d2, #30\n\t"
        "vsri.u64	d11, d2, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d6, #23\n\t"
        "vsri.u64	d10, d6, #41\n\t"
        "vshl.u64	d11, d2, #25\n\t"
        "vsri.u64	d11, d2, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d12, d22\n\t"
        "vmov	d8, d6\n\t"
        "veor	d10, d3, d4\n\t"
        "vadd.i64	d1, d12\n\t"
        "vbsl	d8, d7, d0\n\t"
        "vbsl	d10, d2, d4\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d5, d1\n\t"
        "vadd.i64	d1, d10\n\t"
        /* Round 7 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d5, #50\n\t"
        "vsri.u64	d8, d5, #14\n\t"
        "vshl.u64	d9, d1, #36\n\t"
        "vsri.u64	d9, d1, #28\n\t"
        "vshl.u64	d10, d5, #46\n\t"
        "vsri.u64	d10, d5, #18\n\t"
        "vshl.u64	d11, d1, #30\n\t"
        "vsri.u64	d11, d1, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d5, #23\n\t"
        "vsri.u64	d10, d5, #41\n\t"
        "vshl.u64	d11, d1, #25\n\t"
        "vsri.u64	d11, d1, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d12, d23\n\t"
        "vmov	d8, d5\n\t"
        "veor	d10, d2, d3\n\t"
        "vadd.i64	d0, d12\n\t"
        "vbsl	d8, d6, d7\n\t"
        "vbsl	d10, d1, d3\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d4, d0\n\t"
        "vadd.i64	d0, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[6]-W[7] */
        "vext.8	q6, q11, q12, #8\n\t"
        "vshl.u64	q4, q10, #45\n\t"
        "vsri.u64	q4, q10, #19\n\t"
        "vshl.u64	q5, q10, #3\n\t"
        "vsri.u64	q5, q10, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q10, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q11, q5\n\t"
        "vext.8	q7, q15, q8, #8\n\t"
        "vadd.i64	q11, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q11, q5\n\t"
#else
        /* Calc new W[6]-W[7] */
        "vmov	d12, d23\n\t"
        "vmov	d13, d24\n\t"
        "vshl.u64	d8, d20, #45\n\t"
        "vshl.u64	d9, d21, #45\n\t"
        "vsri.u64	d8, d20, #19\n\t"
        "vsri.u64	d9, d21, #19\n\t"
        "vshl.u64	d10, d20, #3\n\t"
        "vshl.u64	d11, d21, #3\n\t"
        "vsri.u64	d10, d20, #61\n\t"
        "vsri.u64	d11, d21, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d20, #6\n\t"
        "vshr.u64	d9, d21, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d22, d10\n\t"
        "vadd.i64	d23, d11\n\t"
        "vmov	d14, d31\n\t"
        "vmov	d15, d16\n\t"
        "vadd.i64	d22, d14\n\t"
        "vadd.i64	d23, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d22, d10\n\t"
        "vadd.i64	d23, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 8 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d4, #50\n\t"
        "vsri.u64	d8, d4, #14\n\t"
        "vshl.u64	d9, d0, #36\n\t"
        "vsri.u64	d9, d0, #28\n\t"
        "vshl.u64	d10, d4, #46\n\t"
        "vsri.u64	d10, d4, #18\n\t"
        "vshl.u64	d11, d0, #30\n\t"
        "vsri.u64	d11, d0, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d4, #23\n\t"
        "vsri.u64	d10, d4, #41\n\t"
        "vshl.u64	d11, d0, #25\n\t"
        "vsri.u64	d11, d0, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d12, d24\n\t"
        "vmov	d8, d4\n\t"
        "veor	d10, d1, d2\n\t"
        "vadd.i64	d7, d12\n\t"
        "vbsl	d8, d5, d6\n\t"
        "vbsl	d10, d0, d2\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d3, d7\n\t"
        "vadd.i64	d7, d10\n\t"
        /* Round 9 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d3, #50\n\t"
        "vsri.u64	d8, d3, #14\n\t"
        "vshl.u64	d9, d7, #36\n\t"
        "vsri.u64	d9, d7, #28\n\t"
        "vshl.u64	d10, d3, #46\n\t"
        "vsri.u64	d10, d3, #18\n\t"
        "vshl.u64	d11, d7, #30\n\t"
        "vsri.u64	d11, d7, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d3, #23\n\t"
        "vsri.u64	d10, d3, #41\n\t"
        "vshl.u64	d11, d7, #25\n\t"
        "vsri.u64	d11, d7, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d12, d25\n\t"
        "vmov	d8, d3\n\t"
        "veor	d10, d0, d1\n\t"
        "vadd.i64	d6, d12\n\t"
        "vbsl	d8, d4, d5\n\t"
        "vbsl	d10, d7, d1\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d2, d6\n\t"
        "vadd.i64	d6, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[8]-W[9] */
        "vext.8	q6, q12, q13, #8\n\t"
        "vshl.u64	q4, q11, #45\n\t"
        "vsri.u64	q4, q11, #19\n\t"
        "vshl.u64	q5, q11, #3\n\t"
        "vsri.u64	q5, q11, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q11, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q12, q5\n\t"
        "vext.8	q7, q8, q9, #8\n\t"
        "vadd.i64	q12, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q12, q5\n\t"
#else
        /* Calc new W[8]-W[9] */
        "vmov	d12, d25\n\t"
        "vmov	d13, d26\n\t"
        "vshl.u64	d8, d22, #45\n\t"
        "vshl.u64	d9, d23, #45\n\t"
        "vsri.u64	d8, d22, #19\n\t"
        "vsri.u64	d9, d23, #19\n\t"
        "vshl.u64	d10, d22, #3\n\t"
        "vshl.u64	d11, d23, #3\n\t"
        "vsri.u64	d10, d22, #61\n\t"
        "vsri.u64	d11, d23, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d22, #6\n\t"
        "vshr.u64	d9, d23, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d24, d10\n\t"
        "vadd.i64	d25, d11\n\t"
        "vmov	d14, d17\n\t"
        "vmov	d15, d18\n\t"
        "vadd.i64	d24, d14\n\t"
        "vadd.i64	d25, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d24, d10\n\t"
        "vadd.i64	d25, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 10 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d2, #50\n\t"
        "vsri.u64	d8, d2, #14\n\t"
        "vshl.u64	d9, d6, #36\n\t"
        "vsri.u64	d9, d6, #28\n\t"
        "vshl.u64	d10, d2, #46\n\t"
        "vsri.u64	d10, d2, #18\n\t"
        "vshl.u64	d11, d6, #30\n\t"
        "vsri.u64	d11, d6, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d2, #23\n\t"
        "vsri.u64	d10, d2, #41\n\t"
        "vshl.u64	d11, d6, #25\n\t"
        "vsri.u64	d11, d6, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d12, d26\n\t"
        "vmov	d8, d2\n\t"
        "veor	d10, d7, d0\n\t"
        "vadd.i64	d5, d12\n\t"
        "vbsl	d8, d3, d4\n\t"
        "vbsl	d10, d6, d0\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d1, d5\n\t"
        "vadd.i64	d5, d10\n\t"
        /* Round 11 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d1, #50\n\t"
        "vsri.u64	d8, d1, #14\n\t"
        "vshl.u64	d9, d5, #36\n\t"
        "vsri.u64	d9, d5, #28\n\t"
        "vshl.u64	d10, d1, #46\n\t"
        "vsri.u64	d10, d1, #18\n\t"
        "vshl.u64	d11, d5, #30\n\t"
        "vsri.u64	d11, d5, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d1, #23\n\t"
        "vsri.u64	d10, d1, #41\n\t"
        "vshl.u64	d11, d5, #25\n\t"
        "vsri.u64	d11, d5, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d12, d27\n\t"
        "vmov	d8, d1\n\t"
        "veor	d10, d6, d7\n\t"
        "vadd.i64	d4, d12\n\t"
        "vbsl	d8, d2, d3\n\t"
        "vbsl	d10, d5, d7\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d0, d4\n\t"
        "vadd.i64	d4, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[10]-W[11] */
        "vext.8	q6, q13, q14, #8\n\t"
        "vshl.u64	q4, q12, #45\n\t"
        "vsri.u64	q4, q12, #19\n\t"
        "vshl.u64	q5, q12, #3\n\t"
        "vsri.u64	q5, q12, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q12, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q13, q5\n\t"
        "vext.8	q7, q9, q10, #8\n\t"
        "vadd.i64	q13, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q13, q5\n\t"
#else
        /* Calc new W[10]-W[11] */
        "vmov	d12, d27\n\t"
        "vmov	d13, d28\n\t"
        "vshl.u64	d8, d24, #45\n\t"
        "vshl.u64	d9, d25, #45\n\t"
        "vsri.u64	d8, d24, #19\n\t"
        "vsri.u64	d9, d25, #19\n\t"
        "vshl.u64	d10, d24, #3\n\t"
        "vshl.u64	d11, d25, #3\n\t"
        "vsri.u64	d10, d24, #61\n\t"
        "vsri.u64	d11, d25, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d24, #6\n\t"
        "vshr.u64	d9, d25, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d26, d10\n\t"
        "vadd.i64	d27, d11\n\t"
        "vmov	d14, d19\n\t"
        "vmov	d15, d20\n\t"
        "vadd.i64	d26, d14\n\t"
        "vadd.i64	d27, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d26, d10\n\t"
        "vadd.i64	d27, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 12 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d0, #50\n\t"
        "vsri.u64	d8, d0, #14\n\t"
        "vshl.u64	d9, d4, #36\n\t"
        "vsri.u64	d9, d4, #28\n\t"
        "vshl.u64	d10, d0, #46\n\t"
        "vsri.u64	d10, d0, #18\n\t"
        "vshl.u64	d11, d4, #30\n\t"
        "vsri.u64	d11, d4, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d0, #23\n\t"
        "vsri.u64	d10, d0, #41\n\t"
        "vshl.u64	d11, d4, #25\n\t"
        "vsri.u64	d11, d4, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d12, d28\n\t"
        "vmov	d8, d0\n\t"
        "veor	d10, d5, d6\n\t"
        "vadd.i64	d3, d12\n\t"
        "vbsl	d8, d1, d2\n\t"
        "vbsl	d10, d4, d6\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d7, d3\n\t"
        "vadd.i64	d3, d10\n\t"
        /* Round 13 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d7, #50\n\t"
        "vsri.u64	d8, d7, #14\n\t"
        "vshl.u64	d9, d3, #36\n\t"
        "vsri.u64	d9, d3, #28\n\t"
        "vshl.u64	d10, d7, #46\n\t"
        "vsri.u64	d10, d7, #18\n\t"
        "vshl.u64	d11, d3, #30\n\t"
        "vsri.u64	d11, d3, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d7, #23\n\t"
        "vsri.u64	d10, d7, #41\n\t"
        "vshl.u64	d11, d3, #25\n\t"
        "vsri.u64	d11, d3, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d12, d29\n\t"
        "vmov	d8, d7\n\t"
        "veor	d10, d4, d5\n\t"
        "vadd.i64	d2, d12\n\t"
        "vbsl	d8, d0, d1\n\t"
        "vbsl	d10, d3, d5\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d6, d2\n\t"
        "vadd.i64	d2, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[12]-W[13] */
        "vext.8	q6, q14, q15, #8\n\t"
        "vshl.u64	q4, q13, #45\n\t"
        "vsri.u64	q4, q13, #19\n\t"
        "vshl.u64	q5, q13, #3\n\t"
        "vsri.u64	q5, q13, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q13, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q14, q5\n\t"
        "vext.8	q7, q10, q11, #8\n\t"
        "vadd.i64	q14, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q14, q5\n\t"
#else
        /* Calc new W[12]-W[13] */
        "vmov	d12, d29\n\t"
        "vmov	d13, d30\n\t"
        "vshl.u64	d8, d26, #45\n\t"
        "vshl.u64	d9, d27, #45\n\t"
        "vsri.u64	d8, d26, #19\n\t"
        "vsri.u64	d9, d27, #19\n\t"
        "vshl.u64	d10, d26, #3\n\t"
        "vshl.u64	d11, d27, #3\n\t"
        "vsri.u64	d10, d26, #61\n\t"
        "vsri.u64	d11, d27, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d26, #6\n\t"
        "vshr.u64	d9, d27, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d28, d10\n\t"
        "vadd.i64	d29, d11\n\t"
        "vmov	d14, d21\n\t"
        "vmov	d15, d22\n\t"
        "vadd.i64	d28, d14\n\t"
        "vadd.i64	d29, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d28, d10\n\t"
        "vadd.i64	d29, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        /* Round 14 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d6, #50\n\t"
        "vsri.u64	d8, d6, #14\n\t"
        "vshl.u64	d9, d2, #36\n\t"
        "vsri.u64	d9, d2, #28\n\t"
        "vshl.u64	d10, d6, #46\n\t"
        "vsri.u64	d10, d6, #18\n\t"
        "vshl.u64	d11, d2, #30\n\t"
        "vsri.u64	d11, d2, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d6, #23\n\t"
        "vsri.u64	d10, d6, #41\n\t"
        "vshl.u64	d11, d2, #25\n\t"
        "vsri.u64	d11, d2, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d12, d30\n\t"
        "vmov	d8, d6\n\t"
        "veor	d10, d3, d4\n\t"
        "vadd.i64	d1, d12\n\t"
        "vbsl	d8, d7, d0\n\t"
        "vbsl	d10, d2, d4\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d5, d1\n\t"
        "vadd.i64	d1, d10\n\t"
        /* Round 15 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d5, #50\n\t"
        "vsri.u64	d8, d5, #14\n\t"
        "vshl.u64	d9, d1, #36\n\t"
        "vsri.u64	d9, d1, #28\n\t"
        "vshl.u64	d10, d5, #46\n\t"
        "vsri.u64	d10, d5, #18\n\t"
        "vshl.u64	d11, d1, #30\n\t"
        "vsri.u64	d11, d1, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d5, #23\n\t"
        "vsri.u64	d10, d5, #41\n\t"
        "vshl.u64	d11, d1, #25\n\t"
        "vsri.u64	d11, d1, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d12, d31\n\t"
        "vmov	d8, d5\n\t"
        "veor	d10, d2, d3\n\t"
        "vadd.i64	d0, d12\n\t"
        "vbsl	d8, d6, d7\n\t"
        "vbsl	d10, d1, d3\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d4, d0\n\t"
        "vadd.i64	d0, d10\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        /* Calc new W[14]-W[15] */
        "vext.8	q6, q15, q8, #8\n\t"
        "vshl.u64	q4, q14, #45\n\t"
        "vsri.u64	q4, q14, #19\n\t"
        "vshl.u64	q5, q14, #3\n\t"
        "vsri.u64	q5, q14, #61\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q4, q14, #6\n\t"
        "veor	q5, q4\n\t"
        "vadd.i64	q15, q5\n\t"
        "vext.8	q7, q11, q12, #8\n\t"
        "vadd.i64	q15, q7\n\t"
        "vshl.u64	q4, q6, #63\n\t"
        "vsri.u64	q4, q6, #1\n\t"
        "vshl.u64	q5, q6, #56\n\t"
        "vsri.u64	q5, q6, #8\n\t"
        "veor	q5, q4\n\t"
        "vshr.u64	q6, #7\n\t"
        "veor	q5, q6\n\t"
        "vadd.i64	q15, q5\n\t"
#else
        /* Calc new W[14]-W[15] */
        "vmov	d12, d31\n\t"
        "vmov	d13, d16\n\t"
        "vshl.u64	d8, d28, #45\n\t"
        "vshl.u64	d9, d29, #45\n\t"
        "vsri.u64	d8, d28, #19\n\t"
        "vsri.u64	d9, d29, #19\n\t"
        "vshl.u64	d10, d28, #3\n\t"
        "vshl.u64	d11, d29, #3\n\t"
        "vsri.u64	d10, d28, #61\n\t"
        "vsri.u64	d11, d29, #61\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d8, d28, #6\n\t"
        "vshr.u64	d9, d29, #6\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vadd.i64	d30, d10\n\t"
        "vadd.i64	d31, d11\n\t"
        "vmov	d14, d23\n\t"
        "vmov	d15, d24\n\t"
        "vadd.i64	d30, d14\n\t"
        "vadd.i64	d31, d15\n\t"
        "vshl.u64	d8, d12, #63\n\t"
        "vshl.u64	d9, d13, #63\n\t"
        "vsri.u64	d8, d12, #1\n\t"
        "vsri.u64	d9, d13, #1\n\t"
        "vshl.u64	d10, d12, #56\n\t"
        "vshl.u64	d11, d13, #56\n\t"
        "vsri.u64	d10, d12, #8\n\t"
        "vsri.u64	d11, d13, #8\n\t"
        "veor	d10, d8\n\t"
        "veor	d11, d9\n\t"
        "vshr.u64	d12, #7\n\t"
        "vshr.u64	d13, #7\n\t"
        "veor	d10, d12\n\t"
        "veor	d11, d13\n\t"
        "vadd.i64	d30, d10\n\t"
        "vadd.i64	d31, d11\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        "subs	r12, r12, #1\n\t"
        "bne	L_SHA512_transform_neon_len_start_%=\n\t"
        /* Round 0 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d4, #50\n\t"
        "vsri.u64	d8, d4, #14\n\t"
        "vshl.u64	d9, d0, #36\n\t"
        "vsri.u64	d9, d0, #28\n\t"
        "vshl.u64	d10, d4, #46\n\t"
        "vsri.u64	d10, d4, #18\n\t"
        "vshl.u64	d11, d0, #30\n\t"
        "vsri.u64	d11, d0, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d4, #23\n\t"
        "vsri.u64	d10, d4, #41\n\t"
        "vshl.u64	d11, d0, #25\n\t"
        "vsri.u64	d11, d0, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d12, d16\n\t"
        "vmov	d8, d4\n\t"
        "veor	d10, d1, d2\n\t"
        "vadd.i64	d7, d12\n\t"
        "vbsl	d8, d5, d6\n\t"
        "vbsl	d10, d0, d2\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d3, d7\n\t"
        "vadd.i64	d7, d10\n\t"
        /* Round 1 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d3, #50\n\t"
        "vsri.u64	d8, d3, #14\n\t"
        "vshl.u64	d9, d7, #36\n\t"
        "vsri.u64	d9, d7, #28\n\t"
        "vshl.u64	d10, d3, #46\n\t"
        "vsri.u64	d10, d3, #18\n\t"
        "vshl.u64	d11, d7, #30\n\t"
        "vsri.u64	d11, d7, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d3, #23\n\t"
        "vsri.u64	d10, d3, #41\n\t"
        "vshl.u64	d11, d7, #25\n\t"
        "vsri.u64	d11, d7, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d12, d17\n\t"
        "vmov	d8, d3\n\t"
        "veor	d10, d0, d1\n\t"
        "vadd.i64	d6, d12\n\t"
        "vbsl	d8, d4, d5\n\t"
        "vbsl	d10, d7, d1\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d2, d6\n\t"
        "vadd.i64	d6, d10\n\t"
        /* Round 2 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d2, #50\n\t"
        "vsri.u64	d8, d2, #14\n\t"
        "vshl.u64	d9, d6, #36\n\t"
        "vsri.u64	d9, d6, #28\n\t"
        "vshl.u64	d10, d2, #46\n\t"
        "vsri.u64	d10, d2, #18\n\t"
        "vshl.u64	d11, d6, #30\n\t"
        "vsri.u64	d11, d6, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d2, #23\n\t"
        "vsri.u64	d10, d2, #41\n\t"
        "vshl.u64	d11, d6, #25\n\t"
        "vsri.u64	d11, d6, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d12, d18\n\t"
        "vmov	d8, d2\n\t"
        "veor	d10, d7, d0\n\t"
        "vadd.i64	d5, d12\n\t"
        "vbsl	d8, d3, d4\n\t"
        "vbsl	d10, d6, d0\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d1, d5\n\t"
        "vadd.i64	d5, d10\n\t"
        /* Round 3 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d1, #50\n\t"
        "vsri.u64	d8, d1, #14\n\t"
        "vshl.u64	d9, d5, #36\n\t"
        "vsri.u64	d9, d5, #28\n\t"
        "vshl.u64	d10, d1, #46\n\t"
        "vsri.u64	d10, d1, #18\n\t"
        "vshl.u64	d11, d5, #30\n\t"
        "vsri.u64	d11, d5, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d1, #23\n\t"
        "vsri.u64	d10, d1, #41\n\t"
        "vshl.u64	d11, d5, #25\n\t"
        "vsri.u64	d11, d5, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d12, d19\n\t"
        "vmov	d8, d1\n\t"
        "veor	d10, d6, d7\n\t"
        "vadd.i64	d4, d12\n\t"
        "vbsl	d8, d2, d3\n\t"
        "vbsl	d10, d5, d7\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d0, d4\n\t"
        "vadd.i64	d4, d10\n\t"
        /* Round 4 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d0, #50\n\t"
        "vsri.u64	d8, d0, #14\n\t"
        "vshl.u64	d9, d4, #36\n\t"
        "vsri.u64	d9, d4, #28\n\t"
        "vshl.u64	d10, d0, #46\n\t"
        "vsri.u64	d10, d0, #18\n\t"
        "vshl.u64	d11, d4, #30\n\t"
        "vsri.u64	d11, d4, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d0, #23\n\t"
        "vsri.u64	d10, d0, #41\n\t"
        "vshl.u64	d11, d4, #25\n\t"
        "vsri.u64	d11, d4, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d12, d20\n\t"
        "vmov	d8, d0\n\t"
        "veor	d10, d5, d6\n\t"
        "vadd.i64	d3, d12\n\t"
        "vbsl	d8, d1, d2\n\t"
        "vbsl	d10, d4, d6\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d7, d3\n\t"
        "vadd.i64	d3, d10\n\t"
        /* Round 5 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d7, #50\n\t"
        "vsri.u64	d8, d7, #14\n\t"
        "vshl.u64	d9, d3, #36\n\t"
        "vsri.u64	d9, d3, #28\n\t"
        "vshl.u64	d10, d7, #46\n\t"
        "vsri.u64	d10, d7, #18\n\t"
        "vshl.u64	d11, d3, #30\n\t"
        "vsri.u64	d11, d3, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d7, #23\n\t"
        "vsri.u64	d10, d7, #41\n\t"
        "vshl.u64	d11, d3, #25\n\t"
        "vsri.u64	d11, d3, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d12, d21\n\t"
        "vmov	d8, d7\n\t"
        "veor	d10, d4, d5\n\t"
        "vadd.i64	d2, d12\n\t"
        "vbsl	d8, d0, d1\n\t"
        "vbsl	d10, d3, d5\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d6, d2\n\t"
        "vadd.i64	d2, d10\n\t"
        /* Round 6 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d6, #50\n\t"
        "vsri.u64	d8, d6, #14\n\t"
        "vshl.u64	d9, d2, #36\n\t"
        "vsri.u64	d9, d2, #28\n\t"
        "vshl.u64	d10, d6, #46\n\t"
        "vsri.u64	d10, d6, #18\n\t"
        "vshl.u64	d11, d2, #30\n\t"
        "vsri.u64	d11, d2, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d6, #23\n\t"
        "vsri.u64	d10, d6, #41\n\t"
        "vshl.u64	d11, d2, #25\n\t"
        "vsri.u64	d11, d2, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d12, d22\n\t"
        "vmov	d8, d6\n\t"
        "veor	d10, d3, d4\n\t"
        "vadd.i64	d1, d12\n\t"
        "vbsl	d8, d7, d0\n\t"
        "vbsl	d10, d2, d4\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d5, d1\n\t"
        "vadd.i64	d1, d10\n\t"
        /* Round 7 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d5, #50\n\t"
        "vsri.u64	d8, d5, #14\n\t"
        "vshl.u64	d9, d1, #36\n\t"
        "vsri.u64	d9, d1, #28\n\t"
        "vshl.u64	d10, d5, #46\n\t"
        "vsri.u64	d10, d5, #18\n\t"
        "vshl.u64	d11, d1, #30\n\t"
        "vsri.u64	d11, d1, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d5, #23\n\t"
        "vsri.u64	d10, d5, #41\n\t"
        "vshl.u64	d11, d1, #25\n\t"
        "vsri.u64	d11, d1, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d12, d23\n\t"
        "vmov	d8, d5\n\t"
        "veor	d10, d2, d3\n\t"
        "vadd.i64	d0, d12\n\t"
        "vbsl	d8, d6, d7\n\t"
        "vbsl	d10, d1, d3\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d4, d0\n\t"
        "vadd.i64	d0, d10\n\t"
        /* Round 8 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d4, #50\n\t"
        "vsri.u64	d8, d4, #14\n\t"
        "vshl.u64	d9, d0, #36\n\t"
        "vsri.u64	d9, d0, #28\n\t"
        "vshl.u64	d10, d4, #46\n\t"
        "vsri.u64	d10, d4, #18\n\t"
        "vshl.u64	d11, d0, #30\n\t"
        "vsri.u64	d11, d0, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d4, #23\n\t"
        "vsri.u64	d10, d4, #41\n\t"
        "vshl.u64	d11, d0, #25\n\t"
        "vsri.u64	d11, d0, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d12, d24\n\t"
        "vmov	d8, d4\n\t"
        "veor	d10, d1, d2\n\t"
        "vadd.i64	d7, d12\n\t"
        "vbsl	d8, d5, d6\n\t"
        "vbsl	d10, d0, d2\n\t"
        "vadd.i64	d7, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d3, d7\n\t"
        "vadd.i64	d7, d10\n\t"
        /* Round 9 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d3, #50\n\t"
        "vsri.u64	d8, d3, #14\n\t"
        "vshl.u64	d9, d7, #36\n\t"
        "vsri.u64	d9, d7, #28\n\t"
        "vshl.u64	d10, d3, #46\n\t"
        "vsri.u64	d10, d3, #18\n\t"
        "vshl.u64	d11, d7, #30\n\t"
        "vsri.u64	d11, d7, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d3, #23\n\t"
        "vsri.u64	d10, d3, #41\n\t"
        "vshl.u64	d11, d7, #25\n\t"
        "vsri.u64	d11, d7, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d12, d25\n\t"
        "vmov	d8, d3\n\t"
        "veor	d10, d0, d1\n\t"
        "vadd.i64	d6, d12\n\t"
        "vbsl	d8, d4, d5\n\t"
        "vbsl	d10, d7, d1\n\t"
        "vadd.i64	d6, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d2, d6\n\t"
        "vadd.i64	d6, d10\n\t"
        /* Round 10 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d2, #50\n\t"
        "vsri.u64	d8, d2, #14\n\t"
        "vshl.u64	d9, d6, #36\n\t"
        "vsri.u64	d9, d6, #28\n\t"
        "vshl.u64	d10, d2, #46\n\t"
        "vsri.u64	d10, d2, #18\n\t"
        "vshl.u64	d11, d6, #30\n\t"
        "vsri.u64	d11, d6, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d2, #23\n\t"
        "vsri.u64	d10, d2, #41\n\t"
        "vshl.u64	d11, d6, #25\n\t"
        "vsri.u64	d11, d6, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d12, d26\n\t"
        "vmov	d8, d2\n\t"
        "veor	d10, d7, d0\n\t"
        "vadd.i64	d5, d12\n\t"
        "vbsl	d8, d3, d4\n\t"
        "vbsl	d10, d6, d0\n\t"
        "vadd.i64	d5, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d1, d5\n\t"
        "vadd.i64	d5, d10\n\t"
        /* Round 11 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d1, #50\n\t"
        "vsri.u64	d8, d1, #14\n\t"
        "vshl.u64	d9, d5, #36\n\t"
        "vsri.u64	d9, d5, #28\n\t"
        "vshl.u64	d10, d1, #46\n\t"
        "vsri.u64	d10, d1, #18\n\t"
        "vshl.u64	d11, d5, #30\n\t"
        "vsri.u64	d11, d5, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d1, #23\n\t"
        "vsri.u64	d10, d1, #41\n\t"
        "vshl.u64	d11, d5, #25\n\t"
        "vsri.u64	d11, d5, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d12, d27\n\t"
        "vmov	d8, d1\n\t"
        "veor	d10, d6, d7\n\t"
        "vadd.i64	d4, d12\n\t"
        "vbsl	d8, d2, d3\n\t"
        "vbsl	d10, d5, d7\n\t"
        "vadd.i64	d4, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d0, d4\n\t"
        "vadd.i64	d4, d10\n\t"
        /* Round 12 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d0, #50\n\t"
        "vsri.u64	d8, d0, #14\n\t"
        "vshl.u64	d9, d4, #36\n\t"
        "vsri.u64	d9, d4, #28\n\t"
        "vshl.u64	d10, d0, #46\n\t"
        "vsri.u64	d10, d0, #18\n\t"
        "vshl.u64	d11, d4, #30\n\t"
        "vsri.u64	d11, d4, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d0, #23\n\t"
        "vsri.u64	d10, d0, #41\n\t"
        "vshl.u64	d11, d4, #25\n\t"
        "vsri.u64	d11, d4, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d12, d28\n\t"
        "vmov	d8, d0\n\t"
        "veor	d10, d5, d6\n\t"
        "vadd.i64	d3, d12\n\t"
        "vbsl	d8, d1, d2\n\t"
        "vbsl	d10, d4, d6\n\t"
        "vadd.i64	d3, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d7, d3\n\t"
        "vadd.i64	d3, d10\n\t"
        /* Round 13 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d7, #50\n\t"
        "vsri.u64	d8, d7, #14\n\t"
        "vshl.u64	d9, d3, #36\n\t"
        "vsri.u64	d9, d3, #28\n\t"
        "vshl.u64	d10, d7, #46\n\t"
        "vsri.u64	d10, d7, #18\n\t"
        "vshl.u64	d11, d3, #30\n\t"
        "vsri.u64	d11, d3, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d7, #23\n\t"
        "vsri.u64	d10, d7, #41\n\t"
        "vshl.u64	d11, d3, #25\n\t"
        "vsri.u64	d11, d3, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d12, d29\n\t"
        "vmov	d8, d7\n\t"
        "veor	d10, d4, d5\n\t"
        "vadd.i64	d2, d12\n\t"
        "vbsl	d8, d0, d1\n\t"
        "vbsl	d10, d3, d5\n\t"
        "vadd.i64	d2, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d6, d2\n\t"
        "vadd.i64	d2, d10\n\t"
        /* Round 14 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d6, #50\n\t"
        "vsri.u64	d8, d6, #14\n\t"
        "vshl.u64	d9, d2, #36\n\t"
        "vsri.u64	d9, d2, #28\n\t"
        "vshl.u64	d10, d6, #46\n\t"
        "vsri.u64	d10, d6, #18\n\t"
        "vshl.u64	d11, d2, #30\n\t"
        "vsri.u64	d11, d2, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d6, #23\n\t"
        "vsri.u64	d10, d6, #41\n\t"
        "vshl.u64	d11, d2, #25\n\t"
        "vsri.u64	d11, d2, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d12, d30\n\t"
        "vmov	d8, d6\n\t"
        "veor	d10, d3, d4\n\t"
        "vadd.i64	d1, d12\n\t"
        "vbsl	d8, d7, d0\n\t"
        "vbsl	d10, d2, d4\n\t"
        "vadd.i64	d1, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d5, d1\n\t"
        "vadd.i64	d1, d10\n\t"
        /* Round 15 */
        "vld1.64	{d12}, [r3]!\n\t"
        "vshl.u64	d8, d5, #50\n\t"
        "vsri.u64	d8, d5, #14\n\t"
        "vshl.u64	d9, d1, #36\n\t"
        "vsri.u64	d9, d1, #28\n\t"
        "vshl.u64	d10, d5, #46\n\t"
        "vsri.u64	d10, d5, #18\n\t"
        "vshl.u64	d11, d1, #30\n\t"
        "vsri.u64	d11, d1, #34\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vshl.u64	d10, d5, #23\n\t"
        "vsri.u64	d10, d5, #41\n\t"
        "vshl.u64	d11, d1, #25\n\t"
        "vsri.u64	d11, d1, #39\n\t"
        "veor	d8, d10\n\t"
        "veor	d9, d11\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d12, d31\n\t"
        "vmov	d8, d5\n\t"
        "veor	d10, d2, d3\n\t"
        "vadd.i64	d0, d12\n\t"
        "vbsl	d8, d6, d7\n\t"
        "vbsl	d10, d1, d3\n\t"
        "vadd.i64	d0, d8\n\t"
        "vadd.i64	d10, d9\n\t"
        "vadd.i64	d4, d0\n\t"
        "vadd.i64	d0, d10\n\t"
        /* Add in digest from start */
        "vldm.64	%[sha512], {d8-d15}\n\t"
#ifndef WOLFSSL_ARM_ARCH_NEON_64BIT
        "vadd.i64	q0, q0, q4\n\t"
        "vadd.i64	q1, q1, q5\n\t"
        "vadd.i64	q2, q2, q6\n\t"
        "vadd.i64	q3, q3, q7\n\t"
#else
        "vadd.i64	d0, d0, d8\n\t"
        "vadd.i64	d1, d1, d9\n\t"
        "vadd.i64	d2, d2, d10\n\t"
        "vadd.i64	d3, d3, d11\n\t"
        "vadd.i64	d4, d4, d12\n\t"
        "vadd.i64	d5, d5, d13\n\t"
        "vadd.i64	d6, d6, d14\n\t"
        "vadd.i64	d7, d7, d15\n\t"
#endif /* WOLFSSL_ARM_ARCH_NEON_64BIT */
        "vstm.64	%[sha512], {d0-d7}\n\t"
        "subs	%[len], %[len], #0x80\n\t"
        "bne	L_SHA512_transform_neon_len_begin_%=\n\t"
        : [sha512] "+r" (sha512), [data] "+r" (data), [len] "+r" (len)
        : [L_SHA512_transform_neon_len_k] "r" (L_SHA512_transform_neon_len_k)
        : "memory", "r3", "r12", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "d10", "d11", "d12", "d13", "d14", "d15", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
    );
}

#endif /* !WOLFSSL_ARMASM_NO_NEON */
#endif /* WOLFSSL_SHA512 */
#endif /* !__aarch64__ */
#endif /* WOLFSSL_ARMASM */
