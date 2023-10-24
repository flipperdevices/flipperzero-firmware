/* armv8-chacha.c
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

/*  The paper NEON crypto by Daniel J. Bernstein and Peter Schwabe was used to optimize for ARM
 *  https://cryptojedi.org/papers/neoncrypto-20120320.pdf
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_ARMASM
#ifdef HAVE_CHACHA

#include <wolfssl/wolfcrypt/chacha.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/cpuid.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef CHACHA_AEAD_TEST
    #include <stdio.h>
#endif

#ifdef CHACHA_TEST
    #include <stdio.h>
#endif

#ifdef BIG_ENDIAN_ORDER
    #define LITTLE32(x) ByteReverseWord32(x)
#else
    #define LITTLE32(x) (x)
#endif

/* Number of rounds */
#define ROUNDS  20

#define U32C(v) (v##U)
#define U32V(v) ((word32)(v) & U32C(0xFFFFFFFF))
#define U8TO32_LITTLE(p) LITTLE32(((word32*)(p))[0])

#define PLUS(v,w)   (U32V((v) + (w)))
#define PLUSONE(v)  (PLUS((v),1))

#define ARM_SIMD_LEN_BYTES 16

/**
  * Set up iv(nonce). Earlier versions used 64 bits instead of 96, this version
  * uses the typical AEAD 96 bit nonce and can do record sizes of 256 GB.
  */
int wc_Chacha_SetIV(ChaCha* ctx, const byte* inIv, word32 counter)
{
    word32 temp[CHACHA_IV_WORDS];/* used for alignment of memory */

#ifdef CHACHA_AEAD_TEST
    word32 i;
    printf("NONCE : ");
    for (i = 0; i < CHACHA_IV_BYTES; i++) {
        printf("%02x", inIv[i]);
    }
    printf("\n\n");
#endif

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(temp, inIv, CHACHA_IV_BYTES);

    ctx->left = 0;
    ctx->X[CHACHA_IV_BYTES+0] = counter;           /* block counter */
    ctx->X[CHACHA_IV_BYTES+1] = LITTLE32(temp[0]); /* fixed variable from nonce */
    ctx->X[CHACHA_IV_BYTES+2] = LITTLE32(temp[1]); /* counter from nonce */
    ctx->X[CHACHA_IV_BYTES+3] = LITTLE32(temp[2]); /* counter from nonce */

    return 0;
}

/* "expand 32-byte k" as unsigned 32 byte */
static const word32 sigma[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
/* "expand 16-byte k" as unsigned 16 byte */
static const word32 tau[4] = {0x61707865, 0x3120646e, 0x79622d36, 0x6b206574};

/**
  * Key setup. 8 word iv (nonce)
  */
int wc_Chacha_SetKey(ChaCha* ctx, const byte* key, word32 keySz)
{
    const word32* constants;
    const byte*   k;

#ifdef XSTREAM_ALIGN
    word32 alignKey[8];
#endif

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    if (keySz != (CHACHA_MAX_KEY_SZ/2) && keySz != CHACHA_MAX_KEY_SZ)
        return BAD_FUNC_ARG;

#ifdef XSTREAM_ALIGN
    if ((wc_ptr_t)key % 4) {
        WOLFSSL_MSG("wc_ChachaSetKey unaligned key");
        XMEMCPY(alignKey, key, keySz);
        k = (byte*)alignKey;
    }
    else {
        k = key;
    }
#else
    k = key;
#endif /* XSTREAM_ALIGN */

#ifdef CHACHA_AEAD_TEST
    word32 i;
    printf("ChaCha key used :\n");
    for (i = 0; i < keySz; i++) {
        printf("%02x", key[i]);
        if ((i + 1) % 8 == 0)
           printf("\n");
    }
    printf("\n\n");
#endif

    ctx->X[4] = U8TO32_LITTLE(k +  0);
    ctx->X[5] = U8TO32_LITTLE(k +  4);
    ctx->X[6] = U8TO32_LITTLE(k +  8);
    ctx->X[7] = U8TO32_LITTLE(k + 12);
    if (keySz == CHACHA_MAX_KEY_SZ) {
        k += 16;
        constants = sigma;
    }
    else {
        constants = tau;
    }
    ctx->X[ 8] = U8TO32_LITTLE(k +  0);
    ctx->X[ 9] = U8TO32_LITTLE(k +  4);
    ctx->X[10] = U8TO32_LITTLE(k +  8);
    ctx->X[11] = U8TO32_LITTLE(k + 12);
    ctx->X[ 0] = constants[0];
    ctx->X[ 1] = constants[1];
    ctx->X[ 2] = constants[2];
    ctx->X[ 3] = constants[3];
    ctx->left = 0;

    return 0;
}

static const word32 L_chacha20_neon_inc_first_word[] = {
    0x1,
    0x0,
    0x0,
    0x0,
};

#ifdef __aarch64__

static const word32 L_chacha20_neon_add_all_counters[] = {
    0x0,
    0x1,
    0x2,
    0x3,
};

static const word32 L_chacha20_neon_rol8[] = {
    0x2010003,
    0x6050407,
    0xa09080b,
    0xe0d0c0f,
};

static WC_INLINE void wc_Chacha_encrypt_320(const word32* input, const byte* m, byte* c, word32 bytes)
{
#ifdef CHACHA_TEST
    printf("Entering wc_Chacha_encrypt_320 with %d bytes\n", bytes);
#endif /*CHACHA_TEST */
    word64 bytes64 = (word64) bytes;
    __asm__ __volatile__ (
        /*
         * The layout of used registers is:
         * ARM
         * w4-w19: these registers hold the fifth Chacha block for calculation in regular ARM
         * w20: loop counter for how many even-odd rounds need to be executed
         * w21: the counter offset for the block in ARM registers
         * NEON
         * v0-v15: the vi'th register holds the i'th word of four blocks during the quarter rounds.
         *         these registers are later transposed make ADDing the input and XORing the message easier.
         * v16-v19: these are helper registers that are used as temporary location to store data
         * v20-v23: load the next message block
         * v24-v27: the 64 byte initial Chacha block
         * v28: vector to increment the counter words of each block
         * v29: vector of 5's to increment counters between L_chacha20_arm64_outer_%= loops
         * v30: table lookup indices to rotate values by 8
         */

        /* Load counter-add values for each block */
        "LD1    {v28.4s}, [%[L_chacha20_neon_add_all_counters]] \n\t"
        /* Load index look-up for rotating left 8 bits */
        "LD1    {v30.16b}, [%[L_chacha20_neon_rol8]] \n\t"
        /* For adding 5 to each counter-add for next 320-byte chunk */
        "MOVI   v29.4s, #5 \n\t"
        /* Counter for 5th block in regular registers */
        "MOV    w21, #4 \n\t"
        /* Load state to encrypt */
        "LD1    {v24.4s-v27.4s}, [%[input]] \n\t"
        "\n"
    "L_chacha20_arm64_outer_%=: \n\t"
        /* Move state into regular registers */
        "MOV    x4, v24.d[0] \n\t"
        "MOV    x6, v24.d[1] \n\t"
        "MOV    x8, v25.d[0] \n\t"
        "MOV    x10, v25.d[1] \n\t"
        "MOV    x12, v26.d[0] \n\t"
        "MOV    x14, v26.d[1] \n\t"
        "MOV    x16, v27.d[0] \n\t"
        "MOV    x22, v27.d[1] \n\t"
        /* Move state into vector registers (x4) */
        "DUP    v0.4s, v24.s[0] \n\t"
        "DUP    v1.4s, v24.s[1] \n\t"
        "LSR    x5, x4, #32 \n\t"
        "DUP    v2.4s, v24.s[2] \n\t"
        "DUP    v3.4s, v24.s[3] \n\t"
        "LSR    x7, x6, #32 \n\t"
        "DUP    v4.4s, v25.s[0] \n\t"
        "DUP    v5.4s, v25.s[1] \n\t"
        "LSR    x9, x8, #32 \n\t"
        "DUP    v6.4s, v25.s[2] \n\t"
        "DUP    v7.4s, v25.s[3] \n\t"
        "LSR    x11, x10, #32 \n\t"
        "DUP    v8.4s, v26.s[0] \n\t"
        "DUP    v9.4s, v26.s[1] \n\t"
        "LSR    x13, x12, #32 \n\t"
        "DUP    v10.4s, v26.s[2] \n\t"
        "DUP    v11.4s, v26.s[3] \n\t"
        "LSR    x15, x14, #32 \n\t"
        "DUP    v12.4s, v27.s[0] \n\t"
        "DUP    v13.4s, v27.s[1] \n\t"
        "LSR    x17, x16, #32 \n\t"
        "DUP    v14.4s, v27.s[2] \n\t"
        "DUP    v15.4s, v27.s[3] \n\t"
        "LSR    x19, x22, #32 \n\t"
        /* Add to counter word */
        "ADD    v12.4s, v12.4s, v28.4s \n\t"
        "ADD    w16, w16, w21 \n\t"
        /* Set number of odd+even rounds to perform */
        "MOV    w20, #10 \n\t"
        "\n"
    "L_chacha20_arm64_inner_%=: \n\t"
        "SUBS   w20, w20, #1 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4s, v0.4s, v4.4s \n\t"
        "ADD    w4, w4, w8 \n\t"
        "ADD    v1.4s, v1.4s, v5.4s \n\t"
        "ADD    w5, w5, w9 \n\t"
        "ADD    v2.4s, v2.4s, v6.4s \n\t"
        "ADD    w6, w6, w10 \n\t"
        "ADD    v3.4s, v3.4s, v7.4s \n\t"
        "ADD    w7, w7, w11 \n\t"
        "EOR    v12.16b, v12.16b, v0.16b \n\t"
        "EOR    w16, w16, w4 \n\t"
        "EOR    v13.16b, v13.16b, v1.16b \n\t"
        "EOR    w17, w17, w5 \n\t"
        "EOR    v14.16b, v14.16b, v2.16b \n\t"
        "EOR    w22, w22, w6 \n\t"
        "EOR    v15.16b, v15.16b, v3.16b \n\t"
        "EOR    w19, w19, w7 \n\t"
        "REV32  v12.8h, v12.8h \n\t"
        "ROR    w16, w16, #16 \n\t"
        "REV32  v13.8h, v13.8h \n\t"
        "ROR    w17, w17, #16 \n\t"
        "REV32  v14.8h, v14.8h \n\t"
        "ROR    w22, w22, #16 \n\t"
        "REV32  v15.8h, v15.8h \n\t"
        "ROR    w19, w19, #16 \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v8.4s, v8.4s, v12.4s \n\t"
        "ADD    w12, w12, w16 \n\t"
        "ADD    v9.4s, v9.4s, v13.4s \n\t"
        "ADD    w13, w13, w17 \n\t"
        "ADD    v10.4s, v10.4s, v14.4s \n\t"
        "ADD    w14, w14, w22 \n\t"
        "ADD    v11.4s, v11.4s, v15.4s \n\t"
        "ADD    w15, w15, w19 \n\t"
        "EOR    v16.16b, v4.16b, v8.16b \n\t"
        "EOR    w8, w8, w12 \n\t"
        "EOR    v17.16b, v5.16b, v9.16b \n\t"
        "EOR    w9, w9, w13 \n\t"
        "EOR    v18.16b, v6.16b, v10.16b \n\t"
        "EOR    w10, w10, w14 \n\t"
        "EOR    v19.16b, v7.16b, v11.16b \n\t"
        "EOR    w11, w11, w15 \n\t"
        "SHL    v4.4s, v16.4s, #12 \n\t"
        "ROR    w8, w8, #20 \n\t"
        "SHL    v5.4s, v17.4s, #12 \n\t"
        "ROR    w9, w9, #20 \n\t"
        "SHL    v6.4s, v18.4s, #12 \n\t"
        "ROR    w10, w10, #20 \n\t"
        "SHL    v7.4s, v19.4s, #12 \n\t"
        "ROR    w11, w11, #20 \n\t"
        "SRI    v4.4s, v16.4s, #20 \n\t"
        "SRI    v5.4s, v17.4s, #20 \n\t"
        "SRI    v6.4s, v18.4s, #20 \n\t"
        "SRI    v7.4s, v19.4s, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4s, v0.4s, v4.4s \n\t"
        "ADD    w4, w4, w8 \n\t"
        "ADD    v1.4s, v1.4s, v5.4s \n\t"
        "ADD    w5, w5, w9 \n\t"
        "ADD    v2.4s, v2.4s, v6.4s \n\t"
        "ADD    w6, w6, w10 \n\t"
        "ADD    v3.4s, v3.4s, v7.4s \n\t"
        "ADD    w7, w7, w11 \n\t"
        "EOR    v12.16b, v12.16b, v0.16b \n\t"
        "EOR    w16, w16, w4 \n\t"
        "EOR    v13.16b, v13.16b, v1.16b \n\t"
        "EOR    w17, w17, w5 \n\t"
        "EOR    v14.16b, v14.16b, v2.16b \n\t"
        "EOR    w22, w22, w6 \n\t"
        "EOR    v15.16b, v15.16b, v3.16b \n\t"
        "EOR    w19, w19, w7 \n\t"
        "TBL    v12.16b, { v12.16b }, v30.16b \n\t"
        "ROR    w16, w16, #24 \n\t"
        "TBL    v13.16b, { v13.16b }, v30.16b \n\t"
        "ROR    w17, w17, #24 \n\t"
        "TBL    v14.16b, { v14.16b }, v30.16b \n\t"
        "ROR    w22, w22, #24 \n\t"
        "TBL    v15.16b, { v15.16b }, v30.16b \n\t"
        "ROR    w19, w19, #24 \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v8.4s, v8.4s, v12.4s \n\t"
        "ADD    w12, w12, w16 \n\t"
        "ADD    v9.4s, v9.4s, v13.4s \n\t"
        "ADD    w13, w13, w17 \n\t"
        "ADD    v10.4s, v10.4s, v14.4s \n\t"
        "ADD    w14, w14, w22 \n\t"
        "ADD    v11.4s, v11.4s, v15.4s \n\t"
        "ADD    w15, w15, w19 \n\t"
        "EOR    v16.16b, v4.16b, v8.16b \n\t"
        "EOR    w8, w8, w12 \n\t"
        "EOR    v17.16b, v5.16b, v9.16b \n\t"
        "EOR    w9, w9, w13 \n\t"
        "EOR    v18.16b, v6.16b, v10.16b \n\t"
        "EOR    w10, w10, w14 \n\t"
        "EOR    v19.16b, v7.16b, v11.16b \n\t"
        "EOR    w11, w11, w15 \n\t"
        "SHL    v4.4s, v16.4s, #7 \n\t"
        "ROR    w8, w8, #25 \n\t"
        "SHL    v5.4s, v17.4s, #7 \n\t"
        "ROR    w9, w9, #25 \n\t"
        "SHL    v6.4s, v18.4s, #7 \n\t"
        "ROR    w10, w10, #25 \n\t"
        "SHL    v7.4s, v19.4s, #7 \n\t"
        "ROR    w11, w11, #25 \n\t"
        "SRI    v4.4s, v16.4s, #25 \n\t"
        "SRI    v5.4s, v17.4s, #25 \n\t"
        "SRI    v6.4s, v18.4s, #25 \n\t"
        "SRI    v7.4s, v19.4s, #25 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4s, v0.4s, v5.4s \n\t"
        "ADD    w4, w4, w9 \n\t"
        "ADD    v1.4s, v1.4s, v6.4s \n\t"
        "ADD    w5, w5, w10 \n\t"
        "ADD    v2.4s, v2.4s, v7.4s \n\t"
        "ADD    w6, w6, w11 \n\t"
        "ADD    v3.4s, v3.4s, v4.4s \n\t"
        "ADD    w7, w7, w8 \n\t"
        "EOR    v15.16b, v15.16b, v0.16b \n\t"
        "EOR    w19, w19, w4 \n\t"
        "EOR    v12.16b, v12.16b, v1.16b \n\t"
        "EOR    w16, w16, w5 \n\t"
        "EOR    v13.16b, v13.16b, v2.16b \n\t"
        "EOR    w17, w17, w6 \n\t"
        "EOR    v14.16b, v14.16b, v3.16b \n\t"
        "EOR    w22, w22, w7 \n\t"
        "REV32  v15.8h, v15.8h \n\t"
        "ROR    w19, w19, #16 \n\t"
        "REV32  v12.8h, v12.8h \n\t"
        "ROR    w16, w16, #16 \n\t"
        "REV32  v13.8h, v13.8h \n\t"
        "ROR    w17, w17, #16 \n\t"
        "REV32  v14.8h, v14.8h \n\t"
        "ROR    w22, w22, #16 \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v10.4s, v10.4s, v15.4s \n\t"
        "ADD    w14, w14, w19 \n\t"
        "ADD    v11.4s, v11.4s, v12.4s \n\t"
        "ADD    w15, w15, w16 \n\t"
        "ADD    v8.4s, v8.4s, v13.4s \n\t"
        "ADD    w12, w12, w17 \n\t"
        "ADD    v9.4s, v9.4s, v14.4s \n\t"
        "ADD    w13, w13, w22 \n\t"
        "EOR    v16.16b, v5.16b, v10.16b \n\t"
        "EOR    w9, w9, w14 \n\t"
        "EOR    v17.16b, v6.16b, v11.16b \n\t"
        "EOR    w10, w10, w15 \n\t"
        "EOR    v18.16b, v7.16b, v8.16b \n\t"
        "EOR    w11, w11, w12 \n\t"
        "EOR    v19.16b, v4.16b, v9.16b \n\t"
        "EOR    w8, w8, w13 \n\t"
        "SHL    v5.4s, v16.4s, #12 \n\t"
        "ROR    w9, w9, #20 \n\t"
        "SHL    v6.4s, v17.4s, #12 \n\t"
        "ROR    w10, w10, #20 \n\t"
        "SHL    v7.4s, v18.4s, #12 \n\t"
        "ROR    w11, w11, #20 \n\t"
        "SHL    v4.4s, v19.4s, #12 \n\t"
        "ROR    w8, w8, #20 \n\t"
        "SRI    v5.4s, v16.4s, #20 \n\t"
        "SRI    v6.4s, v17.4s, #20 \n\t"
        "SRI    v7.4s, v18.4s, #20 \n\t"
        "SRI    v4.4s, v19.4s, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4s, v0.4s, v5.4s \n\t"
        "ADD    w4, w4, w9 \n\t"
        "ADD    v1.4s, v1.4s, v6.4s \n\t"
        "ADD    w5, w5, w10 \n\t"
        "ADD    v2.4s, v2.4s, v7.4s \n\t"
        "ADD    w6, w6, w11 \n\t"
        "ADD    v3.4s, v3.4s, v4.4s \n\t"
        "ADD    w7, w7, w8 \n\t"
        "EOR    v15.16b, v15.16b, v0.16b \n\t"
        "EOR    w19, w19, w4 \n\t"
        "EOR    v12.16b, v12.16b, v1.16b \n\t"
        "EOR    w16, w16, w5 \n\t"
        "EOR    v13.16b, v13.16b, v2.16b \n\t"
        "EOR    w17, w17, w6 \n\t"
        "EOR    v14.16b, v14.16b, v3.16b \n\t"
        "EOR    w22, w22, w7 \n\t"
        "TBL    v15.16b, { v15.16b }, v30.16b \n\t"
        "ROR    w19, w19, #24 \n\t"
        "TBL    v12.16b, { v12.16b }, v30.16b \n\t"
        "ROR    w16, w16, #24 \n\t"
        "TBL    v13.16b, { v13.16b }, v30.16b \n\t"
        "ROR    w17, w17, #24 \n\t"
        "TBL    v14.16b, { v14.16b }, v30.16b \n\t"
        "ROR    w22, w22, #24 \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v10.4s, v10.4s, v15.4s \n\t"
        "ADD    w14, w14, w19 \n\t"
        "ADD    v11.4s, v11.4s, v12.4s \n\t"
        "ADD    w15, w15, w16 \n\t"
        "ADD    v8.4s, v8.4s, v13.4s \n\t"
        "ADD    w12, w12, w17 \n\t"
        "ADD    v9.4s, v9.4s, v14.4s \n\t"
        "ADD    w13, w13, w22 \n\t"
        "EOR    v16.16b, v5.16b, v10.16b \n\t"
        "EOR    w9, w9, w14 \n\t"
        "EOR    v17.16b, v6.16b, v11.16b \n\t"
        "EOR    w10, w10, w15 \n\t"
        "EOR    v18.16b, v7.16b, v8.16b \n\t"
        "EOR    w11, w11, w12 \n\t"
        "EOR    v19.16b, v4.16b, v9.16b \n\t"
        "EOR    w8, w8, w13 \n\t"
        "SHL    v5.4s, v16.4s, #7 \n\t"
        "ROR    w9, w9, #25 \n\t"
        "SHL    v6.4s, v17.4s, #7 \n\t"
        "ROR    w10, w10, #25 \n\t"
        "SHL    v7.4s, v18.4s, #7 \n\t"
        "ROR    w11, w11, #25 \n\t"
        "SHL    v4.4s, v19.4s, #7 \n\t"
        "ROR    w8, w8, #25 \n\t"
        "SRI    v5.4s, v16.4s, #25 \n\t"
        "SRI    v6.4s, v17.4s, #25 \n\t"
        "SRI    v7.4s, v18.4s, #25 \n\t"
        "SRI    v4.4s, v19.4s, #25 \n\t"
        "BNE    L_chacha20_arm64_inner_%= \n\t"
        /* Add counter now rather than after transposed */
        "ADD    v12.4s, v12.4s, v28.4s \n\t"
        "ADD    w16, w16, w21 \n\t"
        /* Load message */
        "LD1    {v20.4s-v23.4s}, [%[m]], #64 \n\t"
        /* Transpose vectors */
        "TRN1   v16.4s, v0.4s, v1.4s \n\t"
        "TRN1   v18.4s, v2.4s, v3.4s \n\t"
        "TRN2   v17.4s, v0.4s, v1.4s \n\t"
        "TRN2   v19.4s, v2.4s, v3.4s \n\t"
        "TRN1   v0.2d, v16.2d, v18.2d \n\t"
        "TRN1   v1.2d, v17.2d, v19.2d \n\t"
        "TRN2   v2.2d, v16.2d, v18.2d \n\t"
        "TRN2   v3.2d, v17.2d, v19.2d \n\t"
        "TRN1   v16.4s, v4.4s, v5.4s \n\t"
        "TRN1   v18.4s, v6.4s, v7.4s \n\t"
        "TRN2   v17.4s, v4.4s, v5.4s \n\t"
        "TRN2   v19.4s, v6.4s, v7.4s \n\t"
        "TRN1   v4.2d, v16.2d, v18.2d \n\t"
        "TRN1   v5.2d, v17.2d, v19.2d \n\t"
        "TRN2   v6.2d, v16.2d, v18.2d \n\t"
        "TRN2   v7.2d, v17.2d, v19.2d \n\t"
        "TRN1   v16.4s, v8.4s, v9.4s \n\t"
        "TRN1   v18.4s, v10.4s, v11.4s \n\t"
        "TRN2   v17.4s, v8.4s, v9.4s \n\t"
        "TRN2   v19.4s, v10.4s, v11.4s \n\t"
        "TRN1   v8.2d, v16.2d, v18.2d \n\t"
        "TRN1   v9.2d, v17.2d, v19.2d \n\t"
        "TRN2   v10.2d, v16.2d, v18.2d \n\t"
        "TRN2   v11.2d, v17.2d, v19.2d \n\t"
        "TRN1   v16.4s, v12.4s, v13.4s \n\t"
        "TRN1   v18.4s, v14.4s, v15.4s \n\t"
        "TRN2   v17.4s, v12.4s, v13.4s \n\t"
        "TRN2   v19.4s, v14.4s, v15.4s \n\t"
        "TRN1   v12.2d, v16.2d, v18.2d \n\t"
        "TRN1   v13.2d, v17.2d, v19.2d \n\t"
        "TRN2   v14.2d, v16.2d, v18.2d \n\t"
        "TRN2   v15.2d, v17.2d, v19.2d \n\t"
        /* Add back state, XOR in message and store (load next block) */
        "ADD    v16.4s, v0.4s, v24.4s \n\t"
        "ADD    v17.4s, v4.4s, v25.4s \n\t"
        "ADD    v18.4s, v8.4s, v26.4s \n\t"
        "ADD    v19.4s, v12.4s, v27.4s \n\t"
        "EOR    v16.16b, v16.16b, v20.16b \n\t"
        "EOR    v17.16b, v17.16b, v21.16b \n\t"
        "EOR    v18.16b, v18.16b, v22.16b \n\t"
        "EOR    v19.16b, v19.16b, v23.16b \n\t"
        "LD1    {v20.4s-v23.4s}, [%[m]], #64 \n\t"
        "ST1    {v16.4s-v19.4s}, [%[c]], #64 \n\t"
        "ADD    v16.4s, v1.4s, v24.4s \n\t"
        "ADD    v17.4s, v5.4s, v25.4s \n\t"
        "ADD    v18.4s, v9.4s, v26.4s \n\t"
        "ADD    v19.4s, v13.4s, v27.4s \n\t"
        "EOR    v16.16b, v16.16b, v20.16b \n\t"
        "EOR    v17.16b, v17.16b, v21.16b \n\t"
        "EOR    v18.16b, v18.16b, v22.16b \n\t"
        "EOR    v19.16b, v19.16b, v23.16b \n\t"
        "LD1    {v20.4s-v23.4s}, [%[m]], #64 \n\t"
        "ST1    {v16.4s-v19.4s}, [%[c]], #64 \n\t"
        "ADD    v16.4s, v2.4s, v24.4s \n\t"
        "ADD    v17.4s, v6.4s, v25.4s \n\t"
        "ADD    v18.4s, v10.4s, v26.4s \n\t"
        "ADD    v19.4s, v14.4s, v27.4s \n\t"
        "EOR    v16.16b, v16.16b, v20.16b \n\t"
        "EOR    v17.16b, v17.16b, v21.16b \n\t"
        "EOR    v18.16b, v18.16b, v22.16b \n\t"
        "EOR    v19.16b, v19.16b, v23.16b \n\t"
        "LD1    {v20.4s-v23.4s}, [%[m]], #64 \n\t"
        "ST1    {v16.4s-v19.4s}, [%[c]], #64 \n\t"
        "ADD    v16.4s, v3.4s, v24.4s \n\t"
        "ADD    v17.4s, v7.4s, v25.4s \n\t"
        "ADD    v18.4s, v11.4s, v26.4s \n\t"
        "ADD    v19.4s, v15.4s, v27.4s \n\t"
        "EOR    v16.16b, v16.16b, v20.16b \n\t"
        "EOR    v17.16b, v17.16b, v21.16b \n\t"
        "EOR    v18.16b, v18.16b, v22.16b \n\t"
        "EOR    v19.16b, v19.16b, v23.16b \n\t"
        "LD1    {v20.4s-v23.4s}, [%[m]], #64 \n\t"
        "ST1    {v16.4s-v19.4s}, [%[c]], #64 \n\t"
        /* Move regular registers into vector registers for adding and xor */
        "ORR    x4, x4, x5, LSL #32 \n\t"
        "ORR    x6, x6, x7, LSL #32 \n\t"
        "ORR    x8, x8, x9, LSL #32 \n\t"
        "MOV    v16.d[0], x4 \n\t"
        "ORR    x10, x10, x11, LSL #32 \n\t"
        "MOV    v16.d[1], x6 \n\t"
        "ORR    x12, x12, x13, LSL #32 \n\t"
        "MOV    v17.d[0], x8 \n\t"
        "ORR    x14, x14, x15, LSL #32 \n\t"
        "MOV    v17.d[1], x10 \n\t"
        "ORR    x16, x16, x17, LSL #32 \n\t"
        "MOV    v18.d[0], x12 \n\t"
        "ORR    x22, x22, x19, LSL #32 \n\t"
        "MOV    v18.d[1], x14 \n\t"
        "MOV    v19.d[0], x16 \n\t"
        "MOV    v19.d[1], x22 \n\t"
        /* Add back state, XOR in message and store */
        "ADD    v16.4s, v16.4s, v24.4s \n\t"
        "ADD    v17.4s, v17.4s, v25.4s \n\t"
        "ADD    v18.4s, v18.4s, v26.4s \n\t"
        "ADD    v19.4s, v19.4s, v27.4s \n\t"
        "EOR    v16.16b, v16.16b, v20.16b \n\t"
        "EOR    v17.16b, v17.16b, v21.16b \n\t"
        "EOR    v18.16b, v18.16b, v22.16b \n\t"
        "EOR    v19.16b, v19.16b, v23.16b \n\t"
        "ADD    w21, w21, #5 \n\t"
        "ST1    {v16.4s-v19.4s}, [%[c]], #64 \n\t"
        "SUBS   %[bytes], %[bytes], #320 \n\t"
        "ADD    v28.4s, v28.4s, v29.4s \n\t"
        "BNE    L_chacha20_arm64_outer_%= \n\t"
        : [input] "+r" (input), [m] "+r" (m), [c] "+r" (c),
          [bytes] "+r" (bytes64)
        : [L_chacha20_neon_add_all_counters] "r" (L_chacha20_neon_add_all_counters),
          [L_chacha20_neon_rol8] "r" (L_chacha20_neon_rol8)
        : "memory", "cc",
          "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12",
          "x13", "x14", "x15", "x16", "x17", "x22", "x19", "x20", "x21",
          "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
          "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18",
          "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27"
    );
}
#endif /* __aarch64__ */

/**
  * Converts word into bytes with rotations having been done.
  */
static WC_INLINE int wc_Chacha_encrypt_256(const word32 input[CHACHA_CHUNK_WORDS], const byte* m, byte* c)
{
#ifdef CHACHA_TEST
    printf("Entering wc_Chacha_encrypt_256\n");
#endif /*CHACHA_TEST */

#ifdef __aarch64__
    __asm__ __volatile__ (
        // v0-v3 - first block
        // v12 first block helper
        // v4-v7 - second block
        // v13 second block helper
        // v8-v11 - third block
        // v14 third block helper
        // w4-w19 - fourth block

        // v0  0  1  2  3
        // v1  4  5  6  7
        // v2  8  9 10 11
        // v3 12 13 14 15
        // load CHACHA state with indices placed as shown above
        /* Load state to encrypt */
        "LD1    {v20.4S-v23.4S}, [%[input]] \n\t"
        /* Load index look-up for rotating left 8 bits */
        "LD1    {v24.16B}, [%[L_chacha20_neon_rol8]] \n\t"
        /* Move state into regular registers */
        "MOV    x4, v20.D[0] \n\t"
        "MOV    x6, v20.D[1] \n\t"
        "MOV    x8, v21.D[0] \n\t"
        "MOV    x10, v21.D[1] \n\t"
        "MOV    x12, v22.D[0] \n\t"
        "MOV    x14, v22.D[1] \n\t"
        "MOV    x16, v23.D[0] \n\t"
        "MOV    x22, v23.D[1] \n\t"
        /* Move state into vector registers (x3) */
        "MOV    v0.16B, v20.16B \n\t"
        "MOV    v1.16B, v21.16B \n\t"
        "LSR    x19, x22, #32 \n\t"
        "MOV    v2.16B, v22.16B \n\t"
        "ADD    w20, w16, #1 \n\t"
        "MOV    v3.16B, v23.16B \n\t"
        "LSR    x17, x16, #32 \n\t"
        "MOV    v4.16B, v20.16B \n\t"
        "MOV    v5.16B, v21.16B \n\t"
        "LSR    x15, x14, #32 \n\t"
        "MOV    v6.16B, v22.16B \n\t"
        "ADD    w21, w16, #2 \n\t"
        "MOV    v7.16B, v23.16B \n\t"
        "LSR    x13, x12, #32 \n\t"
        "MOV    v8.16B, v20.16B \n\t"
        "MOV    v9.16B, v21.16B \n\t"
        "LSR    x11, x10, #32 \n\t"
        "MOV    v10.16B, v22.16B \n\t"
        "ADD    w16, w16, #3 \n\t"
        "MOV    v11.16B, v23.16B \n\t"
        "LSR    x9, x8, #32 \n\t"
        /* Set counter word */
        "MOV    v7.S[0], w20 \n\t"
        "LSR    x7, x6, #32 \n\t"
        "MOV    v11.S[0], w21 \n\t"
        "LSR    x5, x4, #32 \n\t"
        /* Set number of odd+even rounds to perform */
        "MOV    w3, #10 \n\t"
        "\n"
    "L_chacha20_arm64_256_loop_%=: \n\t"
        "SUBS   w3, w3, #1 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    w4, w4, w8 \n\t"
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ADD    w5, w5, w9 \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "ADD    w6, w6, w10 \n\t"
        "ADD    v8.4S, v8.4S, v9.4S \n\t"
        "ADD    w7, w7, w11 \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "EOR    w16, w16, w4 \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "EOR    w17, w17, w5 \n\t"
        "EOR    v11.16B, v11.16B, v8.16B \n\t"
        "EOR    w22, w22, w6 \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        "EOR    w19, w19, w7 \n\t"
        "REV32  v7.8H, v7.8H \n\t"
        "ROR    w16, w16, #16 \n\t"
        "REV32  v11.8H, v11.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ROR    w17, w17, #16 \n\t"
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "ROR    w22, w22, #16 \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "ROR    w19, w19, #16 \n\t"
        "ADD    v10.4S, v10.4S, v11.4S \n\t"
        "ADD    w12, w12, w16 \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "ADD    w13, w13, w17 \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "ADD    w14, w14, w22 \n\t"
        "EOR    v14.16B, v9.16B, v10.16B \n\t"
        "ADD    w15, w15, w19 \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "EOR    w8, w8, w12 \n\t"
        "SHL    v5.4S, v13.4S, #12 \n\t"
        "EOR    w9, w9, w13 \n\t"
        "SHL    v9.4S, v14.4S, #12 \n\t"
        "EOR    w10, w10, w14 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        "EOR    w11, w11, w15 \n\t"
        "SRI    v5.4S, v13.4S, #20 \n\t"
        "ROR    w8, w8, #20 \n\t"
        "SRI    v9.4S, v14.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ROR    w9, w9, #20 \n\t"
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ROR    w10, w10, #20 \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "ROR    w11, w11, #20 \n\t"
        "ADD    v8.4S, v8.4S, v9.4S \n\t"
        "ADD    w4, w4, w8 \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "ADD    w5, w5, w9 \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "ADD    w6, w6, w10 \n\t"
        "EOR    v11.16B, v11.16B, v8.16B \n\t"
        "ADD    w7, w7, w11 \n\t"
        "TBL    v3.16B, { v3.16B }, v24.16B \n\t"
        "EOR    w16, w16, w4 \n\t"
        "TBL    v7.16B, { v7.16B }, v24.16B \n\t"
        "EOR    w17, w17, w5 \n\t"
        "TBL    v11.16B, { v11.16B }, v24.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "EOR    w22, w22, w6 \n\t"
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    w19, w19, w7 \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "ROR    w16, w16, #24 \n\t"
        "ADD    v10.4S, v10.4S, v11.4S \n\t"
        "ROR    w17, w17, #24 \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "ROR    w22, w22, #24 \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "ROR    w19, w19, #24 \n\t"
        "EOR    v14.16B, v9.16B, v10.16B \n\t"
        "ADD    w12, w12, w16 \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "ADD    w13, w13, w17 \n\t"
        "SHL    v5.4S, v13.4S, #7 \n\t"
        "ADD    w14, w14, w22 \n\t"
        "SHL    v9.4S, v14.4S, #7 \n\t"
        "ADD    w15, w15, w19 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EOR    w8, w8, w12 \n\t"
        "SRI    v5.4S, v13.4S, #25 \n\t"
        "EOR    w9, w9, w13 \n\t"
        "SRI    v9.4S, v14.4S, #25 \n\t"
        "EOR    w10, w10, w14 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EOR    w11, w11, w15 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        "ROR    w8, w8, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "ROR    w9, w9, #25 \n\t"
        "EXT    v5.16B, v5.16B, v5.16B, #4 \n\t"
        "ROR    w10, w10, #25 \n\t"
        "EXT    v6.16B, v6.16B, v6.16B, #8 \n\t"
        "ROR    w11, w11, #25 \n\t"
        "EXT    v7.16B, v7.16B, v7.16B, #12 \n\t"
        "EXT    v9.16B, v9.16B, v9.16B, #4 \n\t"
        "EXT    v10.16B, v10.16B, v10.16B, #8 \n\t"
        "EXT    v11.16B, v11.16B, v11.16B, #12 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    w4, w4, w9 \n\t"
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ADD    w5, w5, w10 \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "ADD    w6, w6, w11 \n\t"
        "ADD    v8.4S, v8.4S, v9.4S \n\t"
        "ADD    w7, w7, w8 \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "EOR    w19, w19, w4 \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "EOR    w16, w16, w5 \n\t"
        "EOR    v11.16B, v11.16B, v8.16B \n\t"
        "EOR    w17, w17, w6 \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        "EOR    w22, w22, w7 \n\t"
        "REV32  v7.8H, v7.8H \n\t"
        "ROR    w19, w19, #16 \n\t"
        "REV32  v11.8H, v11.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ROR    w16, w16, #16 \n\t"
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "ROR    w17, w17, #16 \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "ROR    w22, w22, #16 \n\t"
        "ADD    v10.4S, v10.4S, v11.4S \n\t"
        "ADD    w14, w14, w19 \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "ADD    w15, w15, w16 \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "ADD    w12, w12, w17 \n\t"
        "EOR    v14.16B, v9.16B, v10.16B \n\t"
        "ADD    w13, w13, w22 \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "EOR    w9, w9, w14 \n\t"
        "SHL    v5.4S, v13.4S, #12 \n\t"
        "EOR    w10, w10, w15 \n\t"
        "SHL    v9.4S, v14.4S, #12 \n\t"
        "EOR    w11, w11, w12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        "EOR    w8, w8, w13 \n\t"
        "SRI    v5.4S, v13.4S, #20 \n\t"
        "ROR    w9, w9, #20 \n\t"
        "SRI    v9.4S, v14.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ROR    w10, w10, #20 \n\t"
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ROR    w11, w11, #20 \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "ROR    w8, w8, #20 \n\t"
        "ADD    v8.4S, v8.4S, v9.4S \n\t"
        "ADD    w4, w4, w9 \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "ADD    w5, w5, w10 \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "ADD    w6, w6, w11 \n\t"
        "EOR    v11.16B, v11.16B, v8.16B \n\t"
        "ADD    w7, w7, w8 \n\t"
        "TBL    v3.16B, { v3.16B }, v24.16B \n\t"
        "EOR    w19, w19, w4 \n\t"
        "TBL    v7.16B, { v7.16B }, v24.16B \n\t"
        "EOR    w16, w16, w5 \n\t"
        "TBL    v11.16B, { v11.16B }, v24.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "EOR    w17, w17, w6 \n\t"
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    w22, w22, w7 \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "ROR    w19, w19, #24 \n\t"
        "ADD    v10.4S, v10.4S, v11.4S \n\t"
        "ROR    w16, w16, #24 \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "ROR    w17, w17, #24 \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "ROR    w22, w22, #24 \n\t"
        "EOR    v14.16B, v9.16B, v10.16B \n\t"
        "ADD    w14, w14, w19 \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "ADD    w15, w15, w16 \n\t"
        "SHL    v5.4S, v13.4S, #7 \n\t"
        "ADD    w12, w12, w17 \n\t"
        "SHL    v9.4S, v14.4S, #7 \n\t"
        "ADD    w13, w13, w22 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EOR    w9, w9, w14 \n\t"
        "SRI    v5.4S, v13.4S, #25 \n\t"
        "EOR    w10, w10, w15 \n\t"
        "SRI    v9.4S, v14.4S, #25 \n\t"
        "EOR    w11, w11, w12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EOR    w8, w8, w13 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        "ROR    w9, w9, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "ROR    w10, w10, #25 \n\t"
        "EXT    v5.16B, v5.16B, v5.16B, #12 \n\t"
        "ROR    w11, w11, #25 \n\t"
        "EXT    v6.16B, v6.16B, v6.16B, #8 \n\t"
        "ROR    w8, w8, #25 \n\t"
        "EXT    v7.16B, v7.16B, v7.16B, #4 \n\t"
        "EXT    v9.16B, v9.16B, v9.16B, #12 \n\t"
        "EXT    v10.16B, v10.16B, v10.16B, #8 \n\t"
        "EXT    v11.16B, v11.16B, v11.16B, #4 \n\t"
        "BNE    L_chacha20_arm64_256_loop_%= \n\t"
        /* Load message */
        "LD1    {v16.4S-v19.4S}, [%[m]], #64 \n\t"
        /* Add one (2 added during calculating vector results) */
        "ADD    w16, w16, #1 \n\t"
        /* Add back state, XOR in message and store (load next block) */
        "ADD    v0.4S, v0.4S, v20.4S \n\t"
        "ADD    v1.4S, v1.4S, v21.4S \n\t"
        "ADD    v2.4S, v2.4S, v22.4S \n\t"
        "ADD    v3.4S, v3.4S, v23.4S \n\t"
        "EOR    v0.16B, v0.16B, v16.16B \n\t"
        "EOR    v1.16B, v1.16B, v17.16B \n\t"
        "EOR    v2.16B, v2.16B, v18.16B \n\t"
        "EOR    v3.16B, v3.16B, v19.16B \n\t"
        "LD1    {v16.4S-v19.4S}, [%[m]], #64 \n\t"
        "ST1    {v0.4S-v3.4S}, [%[c]], #64 \n\t"
        "MOV    v23.S[0], w20 \n\t"
        "ADD    v4.4S, v4.4S, v20.4S \n\t"
        "ADD    v5.4S, v5.4S, v21.4S \n\t"
        "ADD    v6.4S, v6.4S, v22.4S \n\t"
        "ADD    v7.4S, v7.4S, v23.4S \n\t"
        "EOR    v4.16B, v4.16B, v16.16B \n\t"
        "EOR    v5.16B, v5.16B, v17.16B \n\t"
        "EOR    v6.16B, v6.16B, v18.16B \n\t"
        "EOR    v7.16B, v7.16B, v19.16B \n\t"
        "LD1    {v16.4S-v19.4S}, [%[m]], #64 \n\t"
        "ST1    {v4.4S-v7.4S}, [%[c]], #64 \n\t"
        "MOV    v23.S[0], w21 \n\t"
        "ADD    v8.4S, v8.4S, v20.4S \n\t"
        "ADD    v9.4S, v9.4S, v21.4S \n\t"
        "ADD    v10.4S, v10.4S, v22.4S \n\t"
        "ADD    v11.4S, v11.4S, v23.4S \n\t"
        "EOR    v8.16B, v8.16B, v16.16B \n\t"
        "EOR    v9.16B, v9.16B, v17.16B \n\t"
        "EOR    v10.16B, v10.16B, v18.16B \n\t"
        "EOR    v11.16B, v11.16B, v19.16B \n\t"
        "LD1    {v16.4S-v19.4S}, [%[m]], #64 \n\t"
        "ST1    {v8.4S-v11.4S}, [%[c]], #64 \n\t"
        /* Move regular registers into vector registers for adding and xor */
        "ORR    x4, x4, x5, lsl #32 \n\t"
        "ORR    x6, x6, x7, lsl #32 \n\t"
        "ORR    x8, x8, x9, lsl #32 \n\t"
        "MOV    v12.D[0], x4 \n\t"
        "ORR    x10, x10, x11, lsl #32 \n\t"
        "MOV    v12.D[1], x6 \n\t"
        "ORR    x12, x12, x13, lsl #32 \n\t"
        "MOV    v13.D[0], x8 \n\t"
        "ORR    x14, x14, x15, lsl #32 \n\t"
        "MOV    v13.D[1], x10 \n\t"
        "ORR    x16, x16, x17, lsl #32 \n\t"
        "MOV    v14.D[0], x12 \n\t"
        "ORR    x22, x22, x19, lsl #32 \n\t"
        "MOV    v14.D[1], x14 \n\t"
        "MOV    v15.D[0], x16 \n\t"
        "MOV    v15.D[1], x22 \n\t"
        /* Add back state, XOR in message and store */
        "ADD    v12.4S, v12.4S, v20.4S \n\t"
        "ADD    v13.4S, v13.4S, v21.4S \n\t"
        "ADD    v14.4S, v14.4S, v22.4S \n\t"
        "ADD    v15.4S, v15.4S, v23.4S \n\t"
        "EOR    v12.16B, v12.16B, v16.16B \n\t"
        "EOR    v13.16B, v13.16B, v17.16B \n\t"
        "EOR    v14.16B, v14.16B, v18.16B \n\t"
        "EOR    v15.16B, v15.16B, v19.16B \n\t"
        "ST1    {v12.4S-v15.4S}, [%[c]], #64 \n\t"
        : [input] "+r" (input), [m] "+r" (m), [c] "+r" (c)
        : [L_chacha20_neon_rol8] "r" (L_chacha20_neon_rol8)
        : "memory", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
          "x10", "x11", "x12", "x13", "x14", "x15", "x16",
          "x17", "x22", "x19", "x20", "x21", "v0", "v1",
          "v2", "v3", "v4", "v5", "v6", "v7", "v8",
          "v9", "v10", "v11", "v12", "v13", "v14",
          "v15", "v16", "v17", "v18", "v19", "v20",
          "v21", "v22", "v23"
    );
#else
    __asm__ __volatile__ (
        // The paper NEON crypto by Daniel J. Bernstein and Peter Schwabe was used to optimize for ARM
        // https://cryptojedi.org/papers/neoncrypto-20120320.pdf

        ".align 2 \n\t"
        "LDR r14, %[input] \n\t" // load input address
    #ifndef NDEBUG
        "PUSH { r7 } \n\t"
    #endif
        "SUB sp, sp, #16*4 \n\t"

        "LDM r14, { r0-r12 } \n\t"
        // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12
        //  0  1  2  3  4  5  6  7  8  9  10  11  12
        "VMOV d0, r0, r1 \n\t"
        "VMOV d1, r2, r3 \n\t"
        "VMOV d2, r4, r5 \n\t"
        "VMOV d3, r6, r7 \n\t"
        "VMOV d4, r8, r9 \n\t"
        "STRD r10, r11, [sp, #4*10] \n\t"
        "VMOV d5, r10, r11 \n\t"
#if defined(WOLFSSL_ARM_ARCH) && (WOLFSSL_ARM_ARCH < 7)
        "LDR r10, [r14, #4*14] \n\t"
        "LDR r11, [r14, #4*15] \n\t"
#else
        "LDRD r10, r11, [r14, #4*14] \n\t"
#endif
        "VMOV q4, q0 \n\t"
        "VMOV q5, q1 \n\t"
        "VMOV q6, q2 \n\t"
        "VMOV q8, q0 \n\t"
        "VMOV q9, q1 \n\t"
        "VMOV q10, q2 \n\t"
        // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12
        //  0  1  2  3  4  5  6  7  8  9  15  14  12
        "VMOV d7, r10, r11 \n\t"
        "STR r11, [sp, #4*15] \n\t"
        "VMOV d15, r10, r11 \n\t"
        "VMOV d23, r10, r11 \n\t"
        "MOV r11, r10 \n\t"
        "MOV r10, r12 \n\t"
        "MOV r12, r11 \n\t"
        "LDR r11, [r14, #4*13] \n\t"
        // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12
        //  0  1  2  3  4  5  6  7  8  9  12  13  14

        "MOV r14, %[rounds] \n\t"

        "VMOV d6, r10, r11 \n\t"
        "ADD r10, r10, #1 \n\t"
        "VMOV d14, r10, r11 \n\t"
        "ADD r10, r10, #1 \n\t"
        "VMOV d22, r10, r11 \n\t"
        "ADD r10, r10, #1 \n\t" // ARM calculates the fourth block (two was already added earlier)
        "\n"
    "L_chacha20_arm32_256_loop_%=: \n\t"
        "SUBS r14, r14, #1 \n\t"

        // 0, 4,  8, 12
        // 1, 5,  9, 13

        // ODD ROUND
        "ADD r0, r0, r4 \n\t" // 0 0 4
        "VADD.I32 q0, q0, q1 \n\t"
        "ADD r1, r1, r5 \n\t" // 1 1 5
        "VADD.I32 q4, q4, q5 \n\t"
        "EOR r10, r10, r0 \n\t" // 12 12 0
        "VADD.I32 q8, q8, q9 \n\t"
        "EOR r11, r11, r1 \n\t" // 13 13 1
        "VEOR q12, q3, q0 \n\t"
        "ROR r10, r10, #16 \n\t" // 12 12
        "VEOR q13, q7, q4 \n\t"
        "ROR r11, r11, #16 \n\t" // 13 13
        "VEOR q14, q11, q8 \n\t"
        "ADD r8, r8, r10 \n\t" // 8 8 12
        // rotation by 16 bits may be done by reversing the 16 bit elements in 32 bit words
        "VREV32.16 q3, q12 \n\t"
        "ADD r9, r9, r11 \n\t" //  9 9 13
        "VREV32.16 q7, q13 \n\t"
        "EOR r4, r4, r8 \n\t" // 4 4 8
        "VREV32.16 q11, q14 \n\t"

        "EOR r5, r5, r9 \n\t" // 5 5 9
        "VADD.I32 q2, q2, q3 \n\t"
        "ROR r4, r4, #20 \n\t" // 4 4
        "VADD.I32 q6, q6, q7 \n\t"
        "ROR r5, r5, #20 \n\t" // 5 5
        "VADD.I32 q10, q10, q11 \n\t"
        "ADD r0, r0, r4 \n\t" // 0 0 4
        "VEOR q12, q1, q2 \n\t"
        "ADD r1, r1, r5 \n\t" // 1 1 5
        "VEOR q13, q5, q6 \n\t"
        "EOR r10, r10, r0 \n\t" // 12 12 0
        "VEOR q14, q9, q10 \n\t"
        "EOR r11, r11, r1 \n\t" // 13 13 1
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q12, #12 \n\t"
        "ROR r10, r10, #24 \n\t" // 12 12
        "VSHL.I32 q5, q13, #12 \n\t"
        "ROR r11, r11, #24 \n\t" // 13 13
        "VSHL.I32 q9, q14, #12 \n\t"
        "ADD r8, r8, r10 \n\t" // 8 8 12
        "VSRI.I32 q1, q12, #20 \n\t"
        "ADD r9, r9, r11 \n\t" // 9 9 13
        "VSRI.I32 q5, q13, #20 \n\t"
        "STR r11, [sp, #4*13] \n\t"
        "VSRI.I32 q9, q14, #20 \n\t"

        "LDR r11, [sp, #4*15] \n\t"
        "VADD.I32 q0, q0, q1 \n\t"
        "EOR r4, r4, r8 \n\t" // 4 4 8
        "VADD.I32 q4, q4, q5 \n\t"
        "STR r8, [sp, #4*8] \n\t"
        "VADD.I32 q8, q8, q9 \n\t"
        "LDR r8, [sp, #4*10] \n\t"
        "VEOR q12, q3, q0 \n\t"
        "EOR r5, r5, r9 \n\t" // 5 5 9
        "VEOR q13, q7, q4 \n\t"
        "STR r9, [sp, #4*9] \n\t"
        "VEOR q14, q11, q8 \n\t"
        "LDR r9, [sp, #4*11] \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q3, q12, #8 \n\t"
        "ROR r4, r4, #25 \n\t" // 4 4
        "VSHL.I32 q7, q13, #8 \n\t"
        "ROR r5, r5, #25 \n\t" // 5 5
        "VSHL.I32 q11, q14, #8 \n\t"

        // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12
        //  0  1  2  3  4  5  6  7 10 11  12  15  14

        // 2, 6, 10, 14
        // 3, 7, 11, 15

        "ADD r2, r2, r6 \n\t" // 2 2 6
        "VSRI.I32 q3, q12, #24 \n\t"
        "ADD r3, r3, r7 \n\t" // 3 3 7
        "VSRI.I32 q7, q13, #24 \n\t"
        "EOR r12, r12, r2 \n\t" // 14 14 2
        "VSRI.I32 q11, q14, #24 \n\t"

        "EOR r11, r11, r3 \n\t" // 15 15 3
        "VADD.I32 q2, q2, q3 \n\t"
        "ROR r12, r12, #16 \n\t" // 14 14
        "VADD.I32 q6, q6, q7 \n\t"
        "ROR r11, r11, #16 \n\t" // 15 15
        "VADD.I32 q10, q10, q11 \n\t"
        "ADD r8, r8, r12 \n\t" // 10 10 14
        "VEOR q12, q1, q2 \n\t"
        "ADD r9, r9, r11 \n\t" // 11 11 15
        "VEOR q13, q5, q6 \n\t"
        "EOR r6, r6, r8 \n\t" // 6 6 10
        "VEOR q14, q9, q10 \n\t"
        "EOR r7, r7, r9 \n\t" // 7 7 11
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q12, #7 \n\t"
        "ROR r6, r6, #20 \n\t" // 6 6
        "VSHL.I32 q5, q13, #7 \n\t"
        "ROR r7, r7, #20 \n\t" // 7 7
        "VSHL.I32 q9, q14, #7 \n\t"
        "ADD r2, r2, r6 \n\t" // 2 2 6
        "VSRI.I32 q1, q12, #25 \n\t"
        "ADD r3, r3, r7 \n\t" // 3 3 7
        "VSRI.I32 q5, q13, #25 \n\t"
        "EOR r12, r12, r2 \n\t" // 14 14 2
        "VSRI.I32 q9, q14, #25 \n\t"

        // EVEN ROUND

        "EOR r11, r11, r3 \n\t" // 15 15 3
        "VEXT.8 q1, q1, q1, #4 \n\t" // permute elements left by one
        "ROR r12, r12, #24 \n\t" // 14 14
        "VEXT.8 q2, q2, q2, #8 \n\t" // permute elements left by two
        "ROR r11, r11, #24 \n\t" // 15 15
        "VEXT.8 q3, q3, q3, #12 \n\t" // permute elements left by three

        "ADD r8, r8, r12 \n\t" // 10 10 14
        "VEXT.8 q5, q5, q5, #4 \n\t" // permute elements left by one
        "ADD r9, r9, r11 \n\t" // 11 11 15
        "VEXT.8 q6, q6, q6, #8 \n\t" // permute elements left by two
        "EOR r6, r6, r8 \n\t" // 6 6 10
        "VEXT.8 q7, q7, q7, #12 \n\t" // permute elements left by three

        "EOR r7, r7, r9 \n\t" // 7 7 11
        "VEXT.8 q9, q9, q9, #4 \n\t" // permute elements left by one
        "ROR r6, r6, #25 \n\t" // 6 6
        "VEXT.8 q10, q10, q10, #8 \n\t" // permute elements left by two
        "ROR r7, r7, #25 \n\t" // 7 7
        "VEXT.8 q11, q11, q11, #12 \n\t" // permute elements left by three

        // 0, 5, 10, 15
        // 1, 6, 11, 12

        "ADD r0, r0, r5 \n\t" // 0 0 5
        "VADD.I32 q0, q0, q1 \n\t"
        "ADD r1, r1, r6 \n\t" // 1 1 6
        "VADD.I32 q4, q4, q5 \n\t"
        "EOR r11, r11, r0 \n\t" // 15 15 0
        "VADD.I32 q8, q8, q9 \n\t"
        "EOR r10, r10, r1 \n\t" // 12 12 1
        "VEOR q12, q3, q0 \n\t"
        "ROR r11, r11, #16 \n\t" // 15 15
        "VEOR q13, q7, q4 \n\t"
        "ROR r10, r10, #16 \n\t" // 12 12
        "VEOR q14, q11, q8 \n\t"
        "ADD r8, r8, r11 \n\t" // 10 10 15
        // rotation by 16 bits may be done by reversing the 16 bit elements in 32 bit words
        "VREV32.16 q3, q12 \n\t"
        "ADD r9, r9, r10 \n\t" // 11 11 12
        "VREV32.16 q7, q13 \n\t"
        "EOR r5, r5, r8 \n\t" // 5 5 10
        "VREV32.16 q11, q14 \n\t"

        "EOR r6, r6, r9 \n\t" // 6 6 11
        "VADD.I32 q2, q2, q3 \n\t"
        "ROR r5, r5, #20 \n\t" // 5 5
        "VADD.I32 q6, q6, q7 \n\t"
        "ROR r6, r6, #20 \n\t" // 6 6
        "VADD.I32 q10, q10, q11 \n\t"
        "ADD r0, r0, r5 \n\t" // 0 0 5
        "VEOR q12, q1, q2 \n\t"
        "ADD r1, r1, r6 \n\t" // 1 1 6
        "VEOR q13, q5, q6 \n\t"
        "EOR r11, r11, r0 \n\t" // 15 15 0
        "VEOR q14, q9, q10 \n\t"
        "EOR r10, r10, r1 \n\t" // 12 12 1
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q12, #12 \n\t"
        "ROR r11, r11, #24 \n\t" // 15 15
        "VSHL.I32 q5, q13, #12 \n\t"
        "ROR r10, r10, #24 \n\t" // 12 12
        "VSHL.I32 q9, q14, #12 \n\t"
        "ADD r8, r8, r11 \n\t" // 10 10 15
        "VSRI.I32 q1, q12, #20 \n\t"
        "STR r11, [sp, #4*15] \n\t"
        "VSRI.I32 q5, q13, #20 \n\t"
        "LDR r11, [sp, #4*13] \n\t"
        "VSRI.I32 q9, q14, #20 \n\t"

        "ADD r9, r9, r10 \n\t" // 11 11 12
        "VADD.I32 q0, q0, q1 \n\t"
        "EOR r5, r5, r8 \n\t" // 5 5 10
        "VADD.I32 q4, q4, q5 \n\t"
        "STR r8, [sp, #4*10] \n\t"
        "VADD.I32 q8, q8, q9 \n\t"
        "LDR r8, [sp, #4*8] \n\t"
        "VEOR q12, q3, q0 \n\t"
        "EOR r6, r6, r9 \n\t" // 6 6 11
        "VEOR q13, q7, q4 \n\t"
        "STR r9, [sp, #4*11] \n\t"
        "VEOR q14, q11, q8 \n\t"
        "LDR r9, [sp, #4*9] \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q3, q12, #8 \n\t"
        "ROR r5, r5, #25 \n\t" // 5 5
        "VSHL.I32 q7, q13, #8 \n\t"
        "ROR r6, r6, #25 \n\t" // 6 6
        "VSHL.I32 q11, q14, #8 \n\t"

        // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12
        //  0  1  2  3  4  5  6  7  8  9  12  13  14

        // 2, 7,  8, 13
        // 3, 4,  9, 14

        "ADD r2, r2, r7 \n\t" // 2 2 7
        "VSRI.I32 q3, q12, #24 \n\t"
        "ADD r3, r3, r4 \n\t" // 3 3 4
        "VSRI.I32 q7, q13, #24 \n\t"
        "EOR r11, r11, r2 \n\t" // 13 13 2
        "VSRI.I32 q11, q14, #24 \n\t"

        "EOR r12, r12, r3 \n\t" // 14 14 3
        "VADD.I32 q2, q2, q3 \n\t"
        "ROR r11, r11, #16 \n\t" // 13 13
        "VADD.I32 q6, q6, q7 \n\t"
        "ROR r12, r12, #16 \n\t" // 14 14
        "VADD.I32 q10, q10, q11 \n\t"
        "ADD r8, r8, r11 \n\t" // 8 8 13
        "VEOR q12, q1, q2 \n\t"
        "ADD r9, r9, r12 \n\t" // 9 9 14
        "VEOR q13, q5, q6 \n\t"
        "EOR r7, r7, r8 \n\t" // 7 7 8
        "VEOR q14, q9, q10 \n\t"
        "EOR r4, r4, r9 \n\t" // 4 4 9
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q12, #7 \n\t"
        "ROR r7, r7, #20 \n\t" // 7 7
        "VSHL.I32 q5, q13, #7 \n\t"
        "ROR r4, r4, #20 \n\t" // 4 4
        "VSHL.I32 q9, q14, #7 \n\t"
        "ADD r2, r2, r7 \n\t" // 2 2 7
        "VSRI.I32 q1, q12, #25 \n\t"
        "ADD r3, r3, r4 \n\t" // 3 3 4
        "VSRI.I32 q5, q13, #25 \n\t"
        "EOR r11, r11, r2 \n\t" // 13 13 2
        "VSRI.I32 q9, q14, #25 \n\t"

        "EOR r12, r12, r3 \n\t" // 14 14 3
        "VEXT.8 q1, q1, q1, #12 \n\t" // permute elements left by three
        "ROR r11, r11, #24 \n\t" // 13 13
        "VEXT.8 q2, q2, q2, #8 \n\t" // permute elements left by two
        "ROR r12, r12, #24 \n\t" // 14 14
        "VEXT.8 q3, q3, q3, #4 \n\t" // permute elements left by one

        "ADD r8, r8, r11 \n\t" // 8 8 13
        "VEXT.8 q5, q5, q5, #12 \n\t" // permute elements left by three
        "ADD r9, r9, r12 \n\t" // 9 9 14
        "VEXT.8 q6, q6, q6, #8 \n\t" // permute elements left by two
        "EOR r7, r7, r8 \n\t" // 7 7 8
        "VEXT.8 q7, q7, q7, #4 \n\t" // permute elements left by one

        "EOR r4, r4, r9 \n\t" // 4 4 9
        "VEXT.8 q9, q9, q9, #12 \n\t" // permute elements left by three
        "ROR r7, r7, #25 \n\t" // 7 7
        "VEXT.8 q10, q10, q10, #8 \n\t" // permute elements left by two
        "ROR r4, r4, #25 \n\t" // 4 4
        "VEXT.8 q11, q11, q11, #4 \n\t" // permute elements left by one

        "BNE L_chacha20_arm32_256_loop_%= \n\t"

        // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12
        //  0  1  2  3  4  5  6  7  8  9  12  13  14
        "ADD r10, r10, #3 \n\t" // add three here to make later NEON easier
        "STM sp, { r0-r9 } \n\t"
        "STRD r10, r11, [sp, #4*12] \n\t"
        "STR r12, [sp, #4*14] \n\t"
        "ADD sp, sp, #16*4 \n\t"
    #ifndef NDEBUG
        "POP { r7 } \n\t"
    #endif
        "LDR r9, %[input] \n\t" // load input address
        "LDR r10, %[c] \n\t" // load c address

        "VLDM r9, { q12-q15 } \n\t"
        "LDR r12, %[m] \n\t" // load m address
    #ifndef NDEBUG
        "SUB sp, sp, #17*4 \n\t"
    #else
        "SUB sp, sp, #16*4 \n\t"
    #endif

        "VADD.I32 q0, q0, q12 \n\t"
        "VADD.I32 q1, q1, q13 \n\t"
        "VADD.I32 q2, q2, q14 \n\t"
        "VADD.I32 q3, q3, q15 \n\t"

        "VADD.I32 q4, q4, q12 \n\t"
        "VADD.I32 q5, q5, q13 \n\t"
        "VADD.I32 q6, q6, q14 \n\t"
        "VADD.I32 q7, q7, q15 \n\t"

        "MOV r11, #1 \n\t"

        "VADD.I32 q8, q8, q12 \n\t"
        "VMOV.I32 q12, #0 \n\t"
        "VADD.I32 q9, q9, q13 \n\t"
        "VMOV.I32 d24[0], r11 \n\t"
        "VADD.I32 q10, q10, q14 \n\t"
        "VADD.I32 q11, q11, q15 \n\t"

        "VADD.I32 q11, q11, q12 \n\t" // add one to counter
        "VADD.I32 q7, q7, q12 \n\t" // add one to counter
        "VADD.I32 q11, q11, q12 \n\t" // add one to counter

        "VLDM r12!, { q12-q15 } \n\t" // load m
        "VEOR q0, q0, q12 \n\t"
        "VEOR q1, q1, q13 \n\t"
        "VEOR q2, q2, q14 \n\t"
        "VEOR q3, q3, q15 \n\t"
        "VSTM r10!, { q0-q3 } \n\t" // store to c

        "VLDM sp, { q0-q3 } \n\t " // load final block from x

        "VLDM r12!, { q12-q15 } \n\t" // load m
        "VEOR q4, q4, q12 \n\t"
        "VEOR q5, q5, q13 \n\t"
        "VEOR q6, q6, q14 \n\t"
        "VEOR q7, q7, q15 \n\t"
        "VSTM r10!, { q4-q7 } \n\t" // store to c

        "VLDM r9, { q4-q7 } \n\t" // load input

        "VLDM r12!, { q12-q15 } \n\t" // load m
        "VEOR q8, q8, q12 \n\t"
        "VEOR q9, q9, q13 \n\t"
        "VEOR q10, q10, q14 \n\t"
        "VEOR q11, q11, q15 \n\t"
        "VSTM r10!, { q8-q11 } \n\t" // store to c

        "VLDM r12!, { q12-q15 } \n\t" // load m
        "VADD.I32 q0, q0, q4 \n\t"
        "VADD.I32 q1, q1, q5 \n\t"
        "VADD.I32 q2, q2, q6 \n\t"
        "VADD.I32 q3, q3, q7 \n\t" // three was added earlier
        "VEOR q0, q0, q12 \n\t"
        "VEOR q1, q1, q13 \n\t"
        "VEOR q2, q2, q14 \n\t"
        "VEOR q3, q3, q15 \n\t"
        "VSTM r10!, { q0-q3 } \n\t" // store to c

    #ifndef NDEBUG
        "ADD sp, sp, #17*4 \n\t"
    #else
        "ADD sp, sp, #16*4 \n\t"
    #endif
            : [c] "+m" (c)
            : [rounds] "I" (ROUNDS/2), [input] "m" (input),
              [chacha_chunk_bytes] "I" (CHACHA_CHUNK_BYTES),
              [m] "m" (m)
            : "memory", "cc",
              "r0", "r1", "r2", "r3",
              "r4", "r5", "r6",
        #ifdef NDEBUG
              "r7",
        #endif
              "r8", "r9", "r10", "r11", "r12", "r14",
              "q0",  "q1",  "q2", "q3", "q4",
              "q5",  "q6",  "q7", "q8", "q9",
              "q10", "q11", "q12", "q13", "q14", "q15"
    );

#endif /* __aarch64__ */
    return CHACHA_CHUNK_BYTES * 4;
}


static WC_INLINE int wc_Chacha_encrypt_128(const word32 input[CHACHA_CHUNK_WORDS], const byte* m, byte* c)
{
#ifdef CHACHA_TEST
    printf("Entering wc_Chacha_encrypt_128\n");
#endif /*CHACHA_TEST */

#ifdef __aarch64__
    __asm__ __volatile__ (
        /* Load incrementer register to modify counter */
        "LD1    {v22.16B}, [%[L_chacha20_neon_inc_first_word]] \n\t"
        /* Load index look-up for rotating left 8 bits */
        "LD1    {v23.16B}, [%[L_chacha20_neon_rol8]] \n\t"
        /* Load state to encrypt */
        "LD1    {v18.4S-v21.4S}, [%[input]] \n\t"
        /* Load message */
        "LD1    {v14.4S-v17.4S}, [%[m]], #64 \n\t"
        /* Move state into vector registers (x3) */
        "MOV    v0.16B, v18.16B \n\t"
        "MOV    v1.16B, v19.16B \n\t"
        "MOV    v2.16B, v20.16B \n\t"
        "MOV    v3.16B, v21.16B \n\t"
        "MOV    v4.16B, v18.16B \n\t"
        "MOV    v5.16B, v19.16B \n\t"
        "MOV    v6.16B, v20.16B \n\t"
        "MOV    v7.16B, v21.16B \n\t"
        /* Add counter word */
        "ADD    v7.4S, v7.4S, v22.4S \n\t"
        /* Set number of odd+even rounds to perform */
        "MOV    w3, #10 \n\t"
        "\n"
    "L_chacha20_arm64_128_loop_%=: \n\t"
        "SUBS   w3, w3, #1 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        "REV32  v7.8H, v7.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SHL    v5.4S, v13.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        "SRI    v5.4S, v13.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v23.16B \n\t"
        "TBL    v7.16B, { v7.16B }, v23.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SHL    v5.4S, v13.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "SRI    v5.4S, v13.4S, #25 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v5.16B, v5.16B, v5.16B, #4 \n\t"
        "EXT    v6.16B, v6.16B, v6.16B, #8 \n\t"
        "EXT    v7.16B, v7.16B, v7.16B, #12 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        "REV32  v7.8H, v7.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SHL    v5.4S, v13.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        "SRI    v5.4S, v13.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "ADD    v4.4S, v4.4S, v5.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "EOR    v7.16B, v7.16B, v4.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v23.16B \n\t"
        "TBL    v7.16B, { v7.16B }, v23.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "ADD    v6.4S, v6.4S, v7.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "EOR    v13.16B, v5.16B, v6.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SHL    v5.4S, v13.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "SRI    v5.4S, v13.4S, #25 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v5.16B, v5.16B, v5.16B, #12 \n\t"
        "EXT    v6.16B, v6.16B, v6.16B, #8 \n\t"
        "EXT    v7.16B, v7.16B, v7.16B, #4 \n\t"
        "BNE    L_chacha20_arm64_128_loop_%= \n\t"
        /* Add back state, XOR in message and store (load next block) */
        "ADD    v0.4S, v0.4S, v18.4S \n\t"
        "ADD    v1.4S, v1.4S, v19.4S \n\t"
        "ADD    v2.4S, v2.4S, v20.4S \n\t"
        "ADD    v3.4S, v3.4S, v21.4S \n\t"
        "EOR    v0.16B, v0.16B, v14.16B \n\t"
        "EOR    v1.16B, v1.16B, v15.16B \n\t"
        "EOR    v2.16B, v2.16B, v16.16B \n\t"
        "EOR    v3.16B, v3.16B, v17.16B \n\t"
        "LD1    {v14.4S-v17.4S}, [%[m]], #64 \n\t"
        "ST1    {v0.4S-v3.4S}, [%[c]], #64 \n\t"
        "ADD    v21.4S, v21.4S, v22.4S \n\t"
        "ADD    v4.4S, v4.4S, v18.4S \n\t"
        "ADD    v5.4S, v5.4S, v19.4S \n\t"
        "ADD    v6.4S, v6.4S, v20.4S \n\t"
        "ADD    v7.4S, v7.4S, v21.4S \n\t"
        "EOR    v4.16B, v4.16B, v14.16B \n\t"
        "EOR    v5.16B, v5.16B, v15.16B \n\t"
        "EOR    v6.16B, v6.16B, v16.16B \n\t"
        "EOR    v7.16B, v7.16B, v17.16B \n\t"
        "ST1    {v4.4S-v7.4S}, [%[c]], #64 \n\t"
        : [input] "+r" (input), [m] "+r" (m), [c] "+r" (c)
        : [L_chacha20_neon_rol8] "r" (L_chacha20_neon_rol8),
          [L_chacha20_neon_inc_first_word] "r" (L_chacha20_neon_inc_first_word)
        : "memory", "x3", "v0", "v1", "v2", "v3", "v4", "v5", "v6",
          "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
          "v16", "v17", "v18", "v19", "v20", "v21"
    );
#else
    __asm__ __volatile__ (
        "MOV r11, %[rounds] \n\t"
        "MOV r12, #1 \n\t"
        "VLDM %[input], { q0-q3 } \n\t"
        "VMOV.I32 q8, #0 \n\t"
        "VMOV q4, q0 \n\t"
        "VMOV.I32 d16[0], r12 \n\t"
        "VMOV q5, q1 \n\t"
        "VMOV q6, q2 \n\t"
        "VADD.I32 q7, q3, q8 \n\t" // add one to counter

        // store input
        "VMOV q10, q0 \n\t"
        "VMOV q11, q1 \n\t"
        "VMOV q12, q2 \n\t"
        "VMOV q13, q3 \n\t"
        "\n"
    "L_chacha20_arm32_128_loop_%=: \n\t"
        "SUBS r11, r11, #1 \n\t"

        // ODD ROUND
        "VADD.I32 q0, q0, q1 \n\t"
        "VADD.I32 q4, q4, q5 \n\t"
        "VEOR q8, q3, q0 \n\t"
        "VEOR q9, q7, q4 \n\t"
        // rotation by 16 bits may be done by reversing the 16 bit elements in 32 bit words
        "VREV32.16 q3, q8 \n\t"
        "VREV32.16 q7, q9 \n\t"

        "VADD.I32 q2, q2, q3 \n\t"
        "VADD.I32 q6, q6, q7 \n\t"
        "VEOR q8, q1, q2 \n\t"
        "VEOR q9, q5, q6 \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q8, #12 \n\t"
        "VSHL.I32 q5, q9, #12 \n\t"
        "VSRI.I32 q1, q8, #20 \n\t"
        "VSRI.I32 q5, q9, #20 \n\t"

        "VADD.I32 q0, q0, q1 \n\t"
        "VADD.I32 q4, q4, q5 \n\t"
        "VEOR q8, q3, q0 \n\t"
        "VEOR q9, q7, q4 \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q3, q8, #8 \n\t"
        "VSHL.I32 q7, q9, #8 \n\t"
        "VSRI.I32 q3, q8, #24 \n\t"
        "VSRI.I32 q7, q9, #24 \n\t"

        "VADD.I32 q2, q2, q3 \n\t"
        "VADD.I32 q6, q6, q7 \n\t"
        "VEOR q8, q1, q2 \n\t"
        "VEOR q9, q5, q6 \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q8, #7 \n\t"
        "VSHL.I32 q5, q9, #7 \n\t"
        "VSRI.I32 q1, q8, #25 \n\t"
        "VSRI.I32 q5, q9, #25 \n\t"

        // EVEN ROUND

        "VEXT.8 q1, q1, q1, #4 \n\t" // permute elements left by one
        "VEXT.8 q2, q2, q2, #8 \n\t" // permute elements left by two
        "VEXT.8 q3, q3, q3, #12 \n\t" // permute elements left by three

        "VEXT.8 q5, q5, q5, #4 \n\t" // permute elements left by one
        "VEXT.8 q6, q6, q6, #8 \n\t" // permute elements left by two
        "VEXT.8 q7, q7, q7, #12 \n\t" // permute elements left by three

        "VADD.I32 q0, q0, q1 \n\t"
        "VADD.I32 q4, q4, q5 \n\t"
        "VEOR q8, q3, q0 \n\t"
        "VEOR q9, q7, q4 \n\t"
        // rotation by 16 bits may be done by reversing the 16 bit elements in 32 bit words
        "VREV32.16 q3, q8 \n\t"
        "VREV32.16 q7, q9 \n\t"

        "VADD.I32 q2, q2, q3 \n\t"
        "VADD.I32 q6, q6, q7 \n\t"
        "VEOR q8, q1, q2 \n\t"
        "VEOR q9, q5, q6 \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q8, #12 \n\t"
        "VSHL.I32 q5, q9, #12 \n\t"
        "VSRI.I32 q1, q8, #20 \n\t"
        "VSRI.I32 q5, q9, #20 \n\t"

        "VADD.I32 q0, q0, q1 \n\t"
        "VADD.I32 q4, q4, q5 \n\t"
        "VEOR q8, q3, q0 \n\t"
        "VEOR q9, q7, q4 \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q3, q8, #8 \n\t"
        "VSHL.I32 q7, q9, #8 \n\t"
        "VSRI.I32 q3, q8, #24 \n\t"
        "VSRI.I32 q7, q9, #24 \n\t"

        "VADD.I32 q2, q2, q3 \n\t"
        "VADD.I32 q6, q6, q7 \n\t"
        "VEOR q8, q1, q2 \n\t"
        "VEOR q9, q5, q6 \n\t"
        // SIMD instructions don't support rotation so we have to cheat using shifts and a help register
        "VSHL.I32 q1, q8, #7 \n\t"
        "VSHL.I32 q5, q9, #7 \n\t"
        "VSRI.I32 q1, q8, #25 \n\t"
        "VSRI.I32 q5, q9, #25 \n\t"

        "VEXT.8 q1, q1, q1, #12 \n\t" // permute elements left by three
        "VEXT.8 q2, q2, q2, #8 \n\t" // permute elements left by two
        "VEXT.8 q3, q3, q3, #4 \n\t" // permute elements left by one

        "VEXT.8 q5, q5, q5, #12 \n\t" // permute elements left by three
        "VEXT.8 q6, q6, q6, #8 \n\t" // permute elements left by two
        "VEXT.8 q7, q7, q7, #4 \n\t" // permute elements left by one

        "BNE L_chacha20_arm32_128_loop_%= \n\t"

        "VMOV.I32 q8, #0 \n\t"
        "VADD.I32 q0, q0, q10 \n\t"
        "VADD.I32 q1, q1, q11 \n\t"
        "VMOV.I32 d16[0], r12 \n\t"
        "VADD.I32 q2, q2, q12 \n\t"
        "VADD.I32 q3, q3, q13 \n\t"

        "VADD.I32 q13, q13, q8 \n\t" // add one to counter

        "VADD.I32 q4, q4, q10 \n\t"
        "VADD.I32 q5, q5, q11 \n\t"
        "VADD.I32 q6, q6, q12 \n\t"
        "VADD.I32 q7, q7, q13 \n\t"

        "VLDM %[m], { q8-q15 } \n\t"
        "VEOR q0, q0, q8 \n\t"
        "VEOR q1, q1, q9 \n\t"
        "VEOR q2, q2, q10 \n\t"
        "VEOR q3, q3, q11 \n\t"
        "VEOR q4, q4, q12 \n\t"
        "VEOR q5, q5, q13 \n\t"
        "VEOR q6, q6, q14 \n\t"
        "VEOR q7, q7, q15 \n\t"
        "VSTM %[c], { q0-q7 } \n\t"

        : [c] "+r" (c), [m] "+r" (m)
        : [rounds] "I" (ROUNDS/2), [input] "r" (input),
          [chacha_chunk_bytes] "I" (CHACHA_CHUNK_BYTES)
        : "memory", "cc",
          "r11", "r12",
          "q0",  "q1",  "q2", "q3", "q4",
          "q5",  "q6",  "q7", "q8", "q9",
          "q10", "q11", "q12", "q13", "q14", "q15"
    );
#endif /* __aarch64__ */
    return CHACHA_CHUNK_BYTES * 2;
}

static WC_INLINE void wc_Chacha_encrypt_64(const word32* input, const byte* m,
                                           byte* c, word32 bytes, byte* over)
{
#ifdef CHACHA_TEST
    printf("Entering wc_Chacha_encrypt_64 with %d bytes\n", bytes);
#endif /*CHACHA_TEST */

#ifdef __aarch64__
    word64 bytes64 = (word64) bytes;
    __asm__ __volatile__ (
        /* Load index look-up for rotating left 8 bits */
        "LD1    {v13.16B}, [%[L_chacha20_neon_rol8]] \n\t"
        "LD1    {v14.4S}, [%[L_chacha20_neon_inc_first_word]] \n\t"
        /* Load state to encrypt */
        "LD1    {v8.4S-v11.4S}, [%[input]] \n\t"
        "\n"
    "L_chacha20_arm64_64_loop_%=: \n\t"
        /* Move state into vector registers (x3) */
        "MOV    v0.16B, v8.16B \n\t"
        "MOV    v1.16B, v9.16B \n\t"
        "MOV    v2.16B, v10.16B \n\t"
        "MOV    v3.16B, v11.16B \n\t"
        /* Add counter word */
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #12 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #4 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "REV32  v3.8H, v3.8H \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #12 \n\t"
        "SRI    v1.4S, v12.4S, #20 \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "ADD    v0.4S, v0.4S, v1.4S \n\t"
        "EOR    v3.16B, v3.16B, v0.16B \n\t"
        "TBL    v3.16B, { v3.16B }, v13.16B \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "ADD    v2.4S, v2.4S, v3.4S \n\t"
        "EOR    v12.16B, v1.16B, v2.16B \n\t"
        "SHL    v1.4S, v12.4S, #7 \n\t"
        "SRI    v1.4S, v12.4S, #25 \n\t"
        "EXT    v3.16B, v3.16B, v3.16B, #4 \n\t"
        "EXT    v1.16B, v1.16B, v1.16B, #12 \n\t"
        "EXT    v2.16B, v2.16B, v2.16B, #8 \n\t"
        /* Add back state */
        "ADD    v0.4S, v0.4S, v8.4S \n\t"
        "ADD    v1.4S, v1.4S, v9.4S \n\t"
        "ADD    v2.4S, v2.4S, v10.4S \n\t"
        "ADD    v3.4S, v3.4S, v11.4S \n\t"
        "CMP    %[bytes], #64 \n\t"
        "BLT    L_chacha20_arm64_64_lt_64_%= \n\t"
        "LD1    {v4.4S-v7.4S}, [%[m]], #64 \n\t"
        "EOR    v4.16B, v4.16B, v0.16B \n\t"
        "EOR    v5.16B, v5.16B, v1.16B \n\t"
        "EOR    v6.16B, v6.16B, v2.16B \n\t"
        "EOR    v7.16B, v7.16B, v3.16B \n\t"
        "ST1    {v4.4S-v7.4S}, [%[c]], #64 \n\t"
        "SUBS   %[bytes], %[bytes], #64 \n\t"
        "ADD    v11.4S, v11.4S, v14.4S \n\t"
        "BNE    L_chacha20_arm64_64_loop_%= \n\t"
        "B      L_chacha20_arm64_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm64_64_lt_64_%=: \n\t"
        "ST1	{v0.4s-v3.4s}, [%[over]]\n\t"
        "CMP    %[bytes], #32 \n\t"
        "BLT    L_chacha20_arm64_64_lt_32_%= \n\t"
        "LD1    {v4.4S, v5.4S}, [%[m]], #32 \n\t"
        "EOR    v4.16B, v4.16B, v0.16B \n\t"
        "EOR    v5.16B, v5.16B, v1.16B \n\t"
        "ST1    {v4.4S, v5.4S}, [%[c]], #32 \n\t"
        "SUBS   %[bytes], %[bytes], #32 \n\t"
        "MOV    v0.16B, v2.16B \n\t"
        "MOV    v1.16B, v3.16B \n\t"
        "BEQ    L_chacha20_arm64_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm64_64_lt_32_%=: \n\t"
        "CMP    %[bytes], #16 \n\t"
        "BLT    L_chacha20_arm64_64_lt_16_%= \n\t"
        "LD1    {v4.4S}, [%[m]], #16 \n\t"
        "EOR    v4.16B, v4.16B, v0.16B \n\t"
        "ST1    {v4.4S}, [%[c]], #16 \n\t"
        "SUBS   %[bytes], %[bytes], #16 \n\t"
        "MOV    v0.16B, v1.16B \n\t"
        "BEQ    L_chacha20_arm64_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm64_64_lt_16_%=: \n\t"
        "CMP    %[bytes], #8 \n\t"
        "BLT    L_chacha20_arm64_64_lt_8_%= \n\t"
        "LD1    {v4.2S}, [%[m]], #8 \n\t"
        "EOR    v4.8B, v4.8B, v0.8B \n\t"
        "ST1    {v4.2S}, [%[c]], #8 \n\t"
        "SUBS   %[bytes], %[bytes], #8 \n\t"
        "MOV    v0.D[0], v0.D[1] \n\t"
        "BEQ    L_chacha20_arm64_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm64_64_lt_8_%=: \n\t"
        "MOV	x4, v0.D[0] \n\t"
        "\n"
    "L_chacha20_arm64_64_loop_lt_8_%=: \n\t"
        "LDRB	w6, [%[m]], #1 \n\t"
        "EOR	w6, w6, w4 \n\t"
        "STRB	w6, [%[c]], #1 \n\t"
        "SUBS	%[bytes], %[bytes], #1 \n\t"
        "LSR	x4, x4, #8 \n\t"
        "BGT	L_chacha20_arm64_64_loop_lt_8_%= \n\t"
        "\n"
    "L_chacha20_arm64_64_done_%=: \n\t"
        : [input] "+r" (input), [m] "+r" (m), [c] "+r" (c), [bytes] "+r" (bytes64)
        : [L_chacha20_neon_rol8] "r" (L_chacha20_neon_rol8),
          [L_chacha20_neon_inc_first_word] "r" (L_chacha20_neon_inc_first_word),
          [over] "r" (over)
        : "memory", "x4", "x5", "x6", "x7", "v0", "v1", "v2", "v3",
          "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11"
    );
#else
    __asm__ __volatile__ (
        /* Get the input state */
        "VLDM       %[input], { q8-q11 } \n\t"
        /* Get the incrementer register */
        "VLDM       %[L_chacha20_neon_inc_first_word], { q14 } \n\t"
        "\n"
    "L_chacha20_arm32_64_outer_loop_%=: \n\t"
        /* Copy over the input state */
        "VMOV       q0, q8               \n\t"
        "VMOV       q1, q9               \n\t"
        "VMOV       q2, q10              \n\t"
        "VMOV       q3, q11              \n\t"
        /* Compute quarter rounds */
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Odd Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Odd->Even */
        "VEXT.8     q1, q1, q1, #4       \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #12      \n\t"
        /* Even Round */
        /* a += b; d ^= a; d <<<= 16; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VREV32.16  q3, q4               \n\t"
        /* c += d; b ^= c; b <<<= 12; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #12          \n\t"
        "VSRI.I32   q1, q4, #20          \n\t"
        /* a += b; d ^= a; d <<<= 8; */
        "VADD.I32   q0, q0, q1           \n\t"
        "VEOR       q4, q3, q0           \n\t"
        "VSHL.I32   q3, q4, #8           \n\t"
        "VSRI.I32   q3, q4, #24          \n\t"
        /* c += d; b ^= c; b <<<= 7; */
        "VADD.I32   q2, q2, q3           \n\t"
        "VEOR       q4, q1, q2           \n\t"
        "VSHL.I32   q1, q4, #7           \n\t"
        "VSRI.I32   q1, q4, #25          \n\t"
        /* Permute Even->Odd */
        "VEXT.8     q1, q1, q1, #12      \n\t"
        "VEXT.8     q2, q2, q2, #8       \n\t"
        "VEXT.8     q3, q3, q3, #4       \n\t"
        /* Add back state */
        "VADD.I32   q0, q0, q8           \n\t"
        "VADD.I32   q1, q1, q9           \n\t"
        "VADD.I32   q2, q2, q10          \n\t"
        "VADD.I32   q3, q3, q11          \n\t"
        "CMP        %[bytes], #64        \n\t"
        "BLT        L_chacha20_arm32_64_lt_64_%= \n\t"
        /* XOR full 64 byte block */
        "VLDM       %[m], { q4-q7 }      \n\t"
        "ADD        %[m], %[m], #64      \n\t"
        "VEOR       q0, q0, q4           \n\t"
        "VEOR       q1, q1, q5           \n\t"
        "VEOR       q2, q2, q6           \n\t"
        "VEOR       q3, q3, q7           \n\t"
        "VSTM       %[c], { q0-q3 }      \n\t"
        "ADD        %[c], %[c], #64      \n\t"
        "SUBS       %[bytes], %[bytes], #64 \n\t"
        "VADD.I32   q11, q11, q14        \n\t"
        "BNE        L_chacha20_arm32_64_outer_loop_%= \n\t"
        "B          L_chacha20_arm32_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm32_64_lt_64_%=: \n\t"
        "VSTM       %[over], {q0-q3}     \n\t"
        /* XOR 32 bytes */
        "CMP        %[bytes], #32        \n\t"
        "BLT        L_chacha20_arm32_64_lt_32_%= \n\t"
        "VLDM       %[m], { q4-q5 }      \n\t"
        "ADD        %[m], %[m], #32      \n\t"
        "VEOR       q4, q4, q0           \n\t"
        "VEOR       q5, q5, q1           \n\t"
        "VSTM       %[c], { q4-q5 }      \n\t"
        "ADD        %[c], %[c], #32      \n\t"
        "SUBS       %[bytes], %[bytes], #32 \n\t"
        "VMOV       q0, q2               \n\t"
        "VMOV       q1, q3               \n\t"
        "BEQ        L_chacha20_arm32_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm32_64_lt_32_%=: \n\t"
        /* XOR 16 bytes */
        "CMP        %[bytes], #16        \n\t"
        "BLT        L_chacha20_arm32_64_lt_16_%= \n\t"
        "VLDM       %[m], { q4 }         \n\t"
        "ADD        %[m], %[m], #16      \n\t"
        "VEOR       q4, q4, q0           \n\t"
        "VSTM       %[c], { q4 }         \n\t"
        "ADD        %[c], %[c], #16      \n\t"
        "SUBS       %[bytes], %[bytes], #16 \n\t"
        "VMOV       q0, q1               \n\t"
        "BEQ        L_chacha20_arm32_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm32_64_lt_16_%=: \n\t"
        /* XOR 8 bytes */
        "CMP        %[bytes], #8         \n\t"
        "BLT        L_chacha20_arm32_64_lt_8_%= \n\t"
        "VLD1.64    { d8 }, [%[m]]!      \n\t"
        "VEOR       d8, d8, d0           \n\t"
        "VST1.64    { d8 }, [%[c]]!      \n\t"
        "SUBS       %[bytes], %[bytes], #8 \n\t"
        "VMOV       d0, d1               \n\t"
        "BEQ        L_chacha20_arm32_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm32_64_lt_8_%=: \n\t"
        /* XOR 4 bytes */
        "CMP        %[bytes], #4         \n\t"
        "BLT        L_chacha20_arm32_64_lt_4_%= \n\t"
        "LDR        r12, [%[m]], #4      \n\t"
        "VMOV       r14, d0[0]           \n\t"
        "EOR        r12, r12, r14        \n\t"
        "STR        r12, [%[c]], #4      \n\t"
        "SUBS       %[bytes], %[bytes], #4 \n\t"
        "VSHR.U64   d0, d0, #32          \n\t"
        "BEQ        L_chacha20_arm32_64_done_%= \n\t"
        "\n"
    "L_chacha20_arm32_64_lt_4_%=: \n\t"
        /* XOR remaining bytes */
        "VMOV       r14, d0[0]           \n\t"
        "\n"
    "L_chacha20_arm32_64_lt_4_loop_%=: \n\t"
        "LDRB       r12, [%[m]], #1      \n\t"
        "EOR        r12, r12, r14        \n\t"
        "STRB       r12, [%[c]], #1      \n\t"
        "SUBS       %[bytes], %[bytes], #1 \n\t"
        "LSR        r14, r14, #8         \n\t"
        "BGT        L_chacha20_arm32_64_lt_4_loop_%= \n\t"
        "\n"
    "L_chacha20_arm32_64_done_%=: \n\t"
        : [input] "+r" (input), [m] "+r" (m), [c] "+r" (c), [bytes] "+r" (bytes)
        : [L_chacha20_neon_inc_first_word] "r" (L_chacha20_neon_inc_first_word),
          [over] "r" (over)
        : "memory", "cc",
          "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "q14", "r12", "r14"
    );
#endif /* __aarch64__ */
}



/**
  * Encrypt a stream of bytes
  */
static void wc_Chacha_encrypt_bytes(ChaCha* ctx, const byte* m, byte* c,
                                    word32 bytes)
{
    int    processed;

#ifdef __aarch64__
    if (bytes >= CHACHA_CHUNK_BYTES * 5) {
        processed = (bytes / (CHACHA_CHUNK_BYTES * 5)) * CHACHA_CHUNK_BYTES * 5;
        wc_Chacha_encrypt_320(ctx->X, m, c, processed);

        bytes -= processed;
        c += processed;
        m += processed;
        ctx->X[CHACHA_IV_BYTES] = PLUS(ctx->X[CHACHA_IV_BYTES], processed / CHACHA_CHUNK_BYTES);
    }
    if (bytes >= CHACHA_CHUNK_BYTES * 4) {
#else
    while (bytes >= CHACHA_CHUNK_BYTES * 4) {
#endif /*__aarch64__ */
        processed = wc_Chacha_encrypt_256(ctx->X, m, c);

        bytes -= processed;
        c += processed;
        m += processed;
        ctx->X[CHACHA_IV_BYTES] = PLUS(ctx->X[CHACHA_IV_BYTES], processed / CHACHA_CHUNK_BYTES);
    }
    if (bytes >= CHACHA_CHUNK_BYTES * 2) {
        processed = wc_Chacha_encrypt_128(ctx->X, m, c);

        bytes -= processed;
        c += processed;
        m += processed;
        ctx->X[CHACHA_IV_BYTES] = PLUS(ctx->X[CHACHA_IV_BYTES], processed / CHACHA_CHUNK_BYTES);
    }
    if (bytes > 0) {
        wc_Chacha_encrypt_64(ctx->X, m, c, bytes, (byte*)ctx->over);
        if (bytes > CHACHA_CHUNK_BYTES)
            ctx->X[CHACHA_IV_BYTES] = PLUSONE(ctx->X[CHACHA_IV_BYTES]);
        ctx->left = CHACHA_CHUNK_BYTES - (bytes & (CHACHA_CHUNK_BYTES - 1));
        ctx->left &= CHACHA_CHUNK_BYTES - 1;
        ctx->X[CHACHA_IV_BYTES] = PLUSONE(ctx->X[CHACHA_IV_BYTES]);
    }
}

/**
  * API to encrypt/decrypt a message of any size.
  */
int wc_Chacha_Process(ChaCha* ctx, byte* output, const byte* input,
                      word32 msglen)
{
    if (ctx == NULL || output == NULL || input == NULL)
        return BAD_FUNC_ARG;

    /* handle left overs */
    if (msglen > 0 && ctx->left > 0) {
        byte*  out;
        word32 i;

        out = (byte*)ctx->over + CHACHA_CHUNK_BYTES - ctx->left;
        for (i = 0; i < msglen && i < ctx->left; i++) {
            output[i] = (byte)(input[i] ^ out[i]);
        }
        ctx->left -= i;

        msglen -= i;
        output += i;
        input += i;
    }

    if (msglen == 0) {
        return 0;
    }

    wc_Chacha_encrypt_bytes(ctx, input, output, msglen);

    return 0;
}

#endif /* HAVE_CHACHA */
#endif /* WOLFSSL_ARMASM */
