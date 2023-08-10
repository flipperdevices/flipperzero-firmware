/* esp32_mp.c
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
#include <string.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>

#include "wolfssl/wolfcrypt/logging.h"

#if !defined(NO_RSA) || defined(HAVE_ECC)

#if defined(WOLFSSL_ESP32_CRYPT_RSA_PRI) && \
   !defined(NO_WOLFSSL_ESP32_CRYPT_RSA_PRI)

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#include <wolfssl/wolfcrypt/wolfmath.h>

static const char* const TAG = "wolfssl_mp";

#define ESP_HW_RSAMAX_BIT           4096
#define ESP_HW_MULTI_RSAMAX_BITS    2048
#define ESP_HW_RSAMIN_BIT           512
#define BYTE_TO_WORDS(s)            (((s+3)>>2))           /* (s+(4-1))/ 4    */
#define BITS_TO_WORDS(s)            (((s+31)>>3)>>2)       /* (s+(32-1))/ 8/ 4*/
#define BITS_IN_ONE_WORD            32

#define MP_NG   -1

#define ESP_TIMEOUT(cnt)         (cnt >= ESP_RSA_TIMEOUT_CNT)

/* mutex */
static wolfSSL_Mutex mp_mutex;
static int espmp_CryptHwMutexInit = 0;
/*
* check if the HW is ready before accessing it
*
* When the RSA Accelerator is released from reset, the register RSA_CLEAN_REG
* reads 0 and an initialization process begins. Hardware initializes the four
* memory blocks by setting them to 0. After initialization is complete,
* RSA_CLEAN_REG reads 1. For this reason, software should query RSA_CLEAN_REG
* after being released from reset, and before writing to any RSA Accelerator
* memory blocks or registers for the first time.
*/
static int esp_mp_hw_wait_clean(void)
{
	int ret = MP_OKAY;
    word32 timeout = 0;

#if CONFIG_IDF_TARGET_ESP32S3

    while (!ESP_TIMEOUT(++timeout) && DPORT_REG_READ(RSA_QUERY_CLEAN_REG) != 1)
    {
      /*  wait. expected delay 1 to 2 uS  */
    }
#else
  /* RSA_CLEAN_REG is now called RSA_QUERY_CLEAN_REG. hwcrypto_reg.h maintains
   * RSA_CLEAN_REG for backwards compatibility so this block _might_ be not needed. */
    while(!ESP_TIMEOUT(++timeout) && DPORT_REG_READ(RSA_CLEAN_REG) != 1) {
        /*  wait. expected delay 1 to 2 uS  */
    }
#endif

    if (ESP_TIMEOUT(timeout)) {
        ESP_LOGE(TAG, "esp_mp_hw_wait_clean waiting HW ready timed out.");
        ret = MP_NG;
    }
    return ret;
}

/*
* esp_mp_hw_lock()
*
* Lock HW engine.
* This should be called before using engine.
*
* Returns 0 if the HW lock was initialized and mutex lock.
*
* See Chapter 24:
*  https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
*
* The RSA Accelerator is activated by enabling the corresponding peripheral
* clock, and by clearing the DPORT_RSA_PD bit in the DPORT_RSA_PD_CTRL_REG
* register. This releases the RSA Accelerator from reset.
*
* When the RSA Accelerator is released from reset, the register RSA_CLEAN_REG
* reads 0 and an initialization process begins. Hardware initializes the four
* memory blocks by setting them to 0. After initialization is complete,
* RSA_CLEAN_REG reads 1. For this reason, software should query RSA_CLEAN_REG
* after being released from reset, and before writing to any RSA Accelerator
* memory blocks or registers for the first time.
*/
static int esp_mp_hw_lock()
{
    int ret = 0;

    ESP_LOGV(TAG, "enter esp_mp_hw_lock");

    if (espmp_CryptHwMutexInit == 0) {
        ret = esp_CryptHwMutexInit(&mp_mutex);
        if (ret == 0) {
            /* flag esp mp as initialized */
            espmp_CryptHwMutexInit = 1;
        }
        else {
            ESP_LOGE(TAG, "mp mutex initialization failed.");
        }
    }
    else {
        /* ESP AES has already been initialized */
    }

    if (ret == 0) {
        /* lock hardware */
        ret = esp_CryptHwMutexLock(&mp_mutex, portMAX_DELAY);
        if (ret != 0) {
            ESP_LOGE(TAG, "mp engine lock failed.");
            ret = MP_NG;
        }
   }

#if CONFIG_IDF_TARGET_ESP32S3
    /* Activate the RSA accelerator. See 20.3 of ESP32-S3 technical manual.
     * periph_module_enable doesn't seem to be documented and in private folder
     * with v5 release. Maybe it will be deprecated? */
    if (ret == 0) {
        periph_module_enable(PERIPH_RSA_MODULE);

        /* clear bit to enable hardware operation; (set to disable) */
        DPORT_REG_CLR_BIT(SYSTEM_RSA_PD_CTRL_REG, SYSTEM_RSA_MEM_PD);
    }
#else
    /* Enable RSA hardware */
    if (ret == 0) {
        periph_module_enable(PERIPH_RSA_MODULE);

        /* clear bit to enable hardware operation; (set to disable) */
        DPORT_REG_CLR_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);
    }
#endif

    /* reminder: wait until RSA_CLEAN_REG reads 1
     *  see esp_mp_hw_wait_clean()
     */

    ESP_LOGV(TAG, "leave esp_mp_hw_lock");
    return ret;
}

/*
*   Release HW engine
*/
static void esp_mp_hw_unlock( void )
{
#if CONFIG_IDF_TARGET_ESP32S3
    /* Deactivate the RSA accelerator. See 20.3 of ESP32-S3 technical manual.
     * periph_module_enable doesn't seem to be documented and in private folder
     * with v5 release. Maybe it will be deprecated? */
    DPORT_REG_SET_BIT(SYSTEM_RSA_PD_CTRL_REG, SYSTEM_RSA_MEM_PD);
    periph_module_disable(PERIPH_RSA_MODULE);

#else
    /* set bit to disabled hardware operation; (clear to enable)
     */
    DPORT_REG_SET_BIT(DPORT_RSA_PD_CTRL_REG, DPORT_RSA_PD);

    /* Disable RSA hardware */
    periph_module_disable(PERIPH_RSA_MODULE);
#endif

    /* unlock */
    esp_CryptHwMutexUnLock(&mp_mutex);
}

/* this is based on an article by Cetin Kaya Koc,
 * A New Algorithm for Inversion: mod p^k, June 28 2017 */
static int esp_calc_Mdash(MATH_INT_T *M, word32 k, mp_digit* md)
{
    int i;
    int xi;
    int b0 = 1;
    int bi;
    word32  N = 0;
    word32  x;

    N = M->dp[0];
    bi = b0;
    x  = 0;

    for (i = 0; i < k; i++) {
        xi = bi % 2;
        if (xi < 0) {
            xi *= -1;
        }
        bi = (bi - N * xi) / 2;
        x |= (xi << i);
    }
    /* 2's complement */
    *md = ~x + 1;
    return MP_OKAY;
}

/* start HW process */
static void process_start(word32 reg)
{
     /* clear interrupt */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);
    /* start process  */
    DPORT_REG_WRITE(reg, 1);
}

/* wait until done */
static int wait_until_done(word32 reg)
{
    word32 timeout = 0;
    /* wait until done && not timeout */
    while (!ESP_TIMEOUT(++timeout) &&
                DPORT_REG_READ(reg) != 1) {
        /* wait */
    }

    /* clear interrupt */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 1);

    if (ESP_TIMEOUT(timeout)) {
        ESP_LOGE(TAG, "rsa operation is timed out.");
        return MP_NG;
    }

    return MP_OKAY;
}

/* read data from memory into mp_init          */
static void esp_memblock_to_mpint(word32 mem_address,
                                  MATH_INT_T* mp,
                                  word32 numwords)
{
    esp_dport_access_read_buffer((uint32_t*)mp->dp, mem_address, numwords);
    mp->used = numwords;
}

/* write mp_init into memory block
 */
static void esp_mpint_to_memblock(word32 mem_address, const MATH_INT_T* mp,
                                                      const word32 bits,
                                                      const word32 hwords)
{
    /* init */
    word32 i;
    word32 len = (bits / 8 + ((bits & 7) != 0 ? 1 : 0));

    len = (len + sizeof(word32)-1) / sizeof(word32);

    for (i=0; i < hwords; i++) {
        if (i < len) {
            DPORT_REG_WRITE(mem_address + (i * sizeof(word32)), mp->dp[i]);
        }
        else {
            DPORT_REG_WRITE(mem_address + (i * sizeof(word32)), 0);
        }
    }
}

/* return needed HW words.
 * supported words length
 *  words : {16 ,  32,  48,    64,   80,   96, 112,   128}
 *  bits  : {512,1024, 1536, 2048, 2560, 3072, 3584, 4096}
 */
static word32 words2hwords(word32 wd)
{
    const word32 bit_shift  = 4;

    return (((wd + 0xf) >> bit_shift) << bit_shift);
}

/* count the number of words is needed for bits */
static word32 bits2words(word32 bits)
{
    /* 32 bits */
    const word32 d = sizeof(word32) * WOLFSSL_BIT_SIZE;

    return ((bits + (d - 1)) / d);
}

/* get rinv */
static int esp_get_rinv(MATH_INT_T *rinv, MATH_INT_T *M, word32 exp)
{
    int ret = 0;

    /* 2^(exp)*/
    if ((ret = mp_2expt(rinv, exp)) != MP_OKAY) {
        ESP_LOGE(TAG, "failed to calculate mp_2expt()");
        return ret;
    }

    /* r_inv = R^2 mod M(=P) */
    if (ret == 0 && (ret = mp_mod(rinv, M, rinv)) != MP_OKAY) {
        ESP_LOGE(TAG, "failed to calculate mp_mod()");
        return ret;
    }

    return ret;
}

/* Z = X * Y;  */
int esp_mp_mul(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* Z)
{
    int ret;

#ifdef WOLFSSL_SP_INT_NEGATIVE
    /* neg check: X*Y becomes negative */
    int neg;

    /* aka (X->sign == Y->sign) ? MP_ZPOS : MP_NEG; , but with mp_isneg(): */
    neg = (mp_isneg(X) == mp_isneg(Y)) ? MP_ZPOS : MP_NEG;
    if (neg) {
        /* Negative numbers are relatively infrequent.
         * May be interesting during verbose debugging: */
        ESP_LOGV(TAG, "mp_isneg(X) = %d; mp_isneg(Y) = %d; neg = %d ",
                       mp_isneg(X),      mp_isneg(Y),           neg);
    }
#endif
    ret = MP_OKAY; /* assume success until proven wrong */

#if CONFIG_IDF_TARGET_ESP32S3

    int BitsInX = mp_count_bits(X);
    int BitsInY = mp_count_bits(Y);

    /* X & Y must be represented by the same number of bits. Must be
     * enough to represent the larger one. */
    int MinXYBits = max(BitsInX, BitsInY);

    /* Figure out how many words we need to represent each operand & the result. */
    int WordsForOperand = bits2words(MinXYBits);
    int WordsForResult = bits2words(BitsInX + BitsInY);

    /* Make sure we are within capabilities of hardware. */
    if ( (WordsForOperand * BITS_IN_ONE_WORD) > ESP_HW_MULTI_RSAMAX_BITS ) {
        ESP_LOGW(TAG, "exceeds max bit length(2048)");
        return MP_VAL; /*  Error: value is not able to be used. */
    }

    /* Steps to perform large number multiplication. Calculates Z = X x Y. The number of
     * bits in the operands (X, Y) is N. N can be 32x, where x = {1,2,3,...64}, so the
     * maximum number of bits in the X and Y is 2048.
     * See 20.3.3 of ESP32-S3 technical manual
     *  1. Lock the hardware so no-one else uses it and wait until it is ready.
     *  2. Enable/disable interrupt that signals completion -- we don't use the interrupt.
     *  3. Write number of words required for result to the RSA_MODE_REG (now called RSA_LENGTH_REG).
     *     Number of words required for the result is 2 * words for operand - 1
     *  4. Load X, Y operands to memory blocks. Note the Y value must be written to
     *     right aligned.
     *  5. Start the operation by writing 1 to RSA_MULT_START_REG, then wait for it
     *     to complete by monitoring RSA_IDLE_REG (which is now called RSA_QUERY_INTERRUPT_REG).
     *  6. Read the result out.
     *  7. Release the hardware lock so others can use it.
     *  x. Clear the interrupt flag, if you used it (we don't). */

    /* 1. lock HW for use & wait until it is ready. */
    if ( ((ret = esp_mp_hw_lock()) != MP_OKAY) ||
         ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) ) {
        return ret;
    }

    /* 2. Disable completion interrupt signal; we don't use.
    **    0 => no interrupt; 1 => interrupt on completion. */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);

    /* 3. Write number of words required for result. */
    if ( (WordsForOperand * BITS_IN_ONE_WORD * 2) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGW(TAG, "result exceeds max bit length");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    DPORT_REG_WRITE(RSA_LENGTH_REG, (WordsForOperand * 2 - 1) );

    /* 4. Load X, Y operands. Maximum is 64 words (64*8*4 = 2048 bits) */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE,
                          X, BitsInX, WordsForOperand);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE + WordsForOperand * 4,
                          Y, BitsInY, WordsForOperand);


    /* 5. Start operation and wait until it completes. */
    process_start(RSA_MULT_START_REG);
    ret = wait_until_done(RSA_QUERY_INTERRUPT_REG);
    if (MP_OKAY != ret) {
        return ret;
    }

    /* 6. read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, WordsForResult);

    /* 7. clear and release HW                    */
    esp_mp_hw_unlock();

    /* end if CONFIG_IDF_TARGET_ESP32S3 */

#else /* not CONFIG_IDF_TARGET_ESP32S3 */
    /* assumed to be regular Xtensa here */
    word32 Xs;
    word32 Ys;
    word32 Zs;
    word32 maxWords_sz;
    word32 hwWords_sz;

    /* ask bits number */
    Xs = mp_count_bits(X);
    Ys = mp_count_bits(Y);
    Zs = Xs + Ys;

    /* maximum bits and words for writing to HW */
    maxWords_sz = bits2words(max(Xs, Ys));
    hwWords_sz  = words2hwords(maxWords_sz);

    /* sanity check */
    if((hwWords_sz<<5) > ESP_HW_MULTI_RSAMAX_BITS) {
        ESP_LOGW(TAG, "exceeds max bit length(2048)");
        return MP_VAL; /*  Error: value is not able to be used. */
    }

    /*Steps to use HW in the following order:
    * 1. wait until clean HW engine
    * 2. Write(2*N/512bits - 1 + 8) to MULT_MODE_REG
    * 3. Write X and Y to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 4. Write 1  to MUL_START_REG
    * 5. Wait for the first operation to be done. Poll INTERRUPT_REG until it reads 1.
    *    (Or until the INTER interrupt is generated.)
    * 6. Write 1 to RSA_INTERRUPT_REG to clear the interrupt.
    * 7. Read the Z from RSA_Z_MEM
    * 8. Write 1 to RSA_INTERUPT_REG to clear the interrupt.
    * 9. Release the HW engine
    */
    /* lock HW for use */
    if ((ret = esp_mp_hw_lock()) != MP_OKAY) {
        return ret;
    }

    if((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        return ret;
    }

    /* step.1  (2*N/512) => N/256. 512 bits => 16 words */
    DPORT_REG_WRITE(RSA_MULT_MODE_REG, (hwWords_sz >> 3) - 1 + 8);
    /* step.2 write X, M and r_inv into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE,
                          X,
                          Xs,
                          hwWords_sz);
    /* Y(let-extend)                          */
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE + (hwWords_sz<<2),
                          Y,
                          Ys,
                          hwWords_sz);
    /* step.3 start process                           */
    process_start(RSA_MULT_START_REG);

    /* step.4,5 wait until done                       */
    ret = wait_until_done(RSA_INTERRUPT_REG);
    if (ret != MP_OKAY) {
        ESP_LOGE(TAG, "wait_until_done failed.");
        return ret;
    }
    /* step.6 read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(Zs));

    /* step.7 clear and release HW                    */
    esp_mp_hw_unlock();

#endif /* CONFIG_IDF_TARGET_ESP32S3 or not */

    /* common exit for all chipset types */
#ifdef WOLFSSL_SP_INT_NEGATIVE
    if (!mp_iszero(Z) && neg) {
        /* for non-zero negative numbers, set negative flag for our result:
         *   Z->sign = FP_NEG */
        mp_setneg(Z);
    }
#endif

    return ret;
}

/* Z = X * Y (mod M)                                  */
int esp_mp_mulmod(MATH_INT_T* X, MATH_INT_T* Y, MATH_INT_T* M, MATH_INT_T* Z)
{
    int ret = 0;
    int negcheck;
    word32 Xs;
    word32 Ys;
    word32 Ms;
    word32 maxWords_sz;
    word32 hwWords_sz;
    word32 zwords;

    MATH_INT_T r_inv;
    MATH_INT_T tmpZ;
    mp_digit mp;

    uint32_t Exponent;
#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t OperandBits;
    int WordsForOperand;
# endif

    /* neg check - X*Y becomes negative */
    negcheck = mp_isneg(X) != mp_isneg(Y) ? 1 : 0;

    /* ask bits number */
    Xs = mp_count_bits(X);
    Ys = mp_count_bits(Y);
    Ms = mp_count_bits(M);

    /* maximum bits and words for writing to HW */
    maxWords_sz = bits2words(max(Xs, max(Ys, Ms)));
    zwords      = bits2words(min(Ms, Xs + Ys));
    hwWords_sz  = words2hwords(maxWords_sz);

    if ((hwWords_sz << 5) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGE(TAG, "exceeds HW maximum bits");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    /* calculate r_inv = R^2 mode M
    *    where: R = b^n, and b = 2^32
    *    accordingly R^2 = 2^(n*32*2)
    */
#if CONFIG_IDF_TARGET_ESP32S3
    Exponent = maxWords_sz * BITS_IN_ONE_WORD * 2;
#else
    Exponent = hwWords_sz << 6;
#endif
    ret = mp_init_multi(&tmpZ, &r_inv, NULL, NULL, NULL, NULL);
    if (ret == 0 && (ret = esp_get_rinv(&r_inv, M, Exponent)) != MP_OKAY) {
        ESP_LOGE(TAG, "calculate r_inv failed.");
        mp_clear(&tmpZ);
        mp_clear(&r_inv);
        return ret;
    }

    /* lock HW for use */
    if ((ret = esp_mp_hw_lock()) != MP_OKAY) {
        mp_clear(&tmpZ);
        mp_clear(&r_inv);
        return ret;
    }
    /* Calculate M' */
    if ((ret = esp_calc_Mdash(M, 32/* bits */, &mp)) != MP_OKAY) {
        ESP_LOGE(TAG, "failed to calculate M dash");
        mp_clear(&tmpZ);
        mp_clear(&r_inv);
        return ret;
    }

#if CONFIG_IDF_TARGET_ESP32S3
    /* Steps to perform large number modular multiplication. Calculates Z = (X x Y) modulo M.
     * The number of bits in the operands (X, Y) is N. N can be 32x, where x = {1,2,3,...64}, so the
     * maximum number of bits in the X and Y is 2048. We must use the same number of words to represent
     * the bits in X, Y and M.
     * See 20.3.3 of ESP32-S3 technical manual
     *  1. Wait until the hardware is ready.
     *  2. Enable/disable interrupt that signals completion -- we don't use the interrupt.
     *  3. Write the number of words required to represent the operands to the
     *     RSA_MODE_REG (now called RSA_LENGTH_REG).
     *  4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG).
     *  5. Load X, Y, M, r' operands to memory blocks.
     *  6. Start the operation by writing 1 to RSA_MOD_MULT_START_REG, then wait for it
     *     to complete by monitoring RSA_IDLE_REG (which is now called RSA_QUERY_INTERRUPT_REG).
     *  7. Read the result out.
     *  8. Release the hardware lock so others can use it.
     *  x. Clear the interrupt flag, if you used it (we don't). */

    /* 1. Wait until hardware is ready. */
    if ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        return ret;
    }

    /* 2. Disable completion interrupt signal; we don't use.
    **    0 => no interrupt; 1 => interrupt on completion. */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);

    /* 3. Write (N_result_bits/32 - 1) to the RSA_MODE_REG. */
    OperandBits = max(max(Xs, Ys), Ms);
    if (OperandBits > ESP_HW_MULTI_RSAMAX_BITS) {
        ESP_LOGW(TAG, "result exceeds max bit length");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    WordsForOperand = bits2words(OperandBits);
    DPORT_REG_WRITE(RSA_LENGTH_REG, WordsForOperand - 1);

    /* 4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG) */
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);

    /* Select acceleration options. */
    DPORT_REG_WRITE(RSA_CONSTANT_TIME_REG, 0);

    /* 5. Load X, Y, M, r' operands.
     * Note RSA_MEM_RB_BLOCK_BASE == RSA_MEM_Z_BLOC_BASE on ESP32s3*/
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y, Ys, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_RB_BLOCK_BASE, &r_inv, mp_count_bits(&r_inv), hwWords_sz);

    /* 6. Start operation and wait until it completes. */
    process_start(RSA_MOD_MULT_START_REG);
    ret = wait_until_done(RSA_QUERY_INTERRUPT_REG);
    if (MP_OKAY != ret) {
        return ret;
    }

    /* 7. read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, &tmpZ, zwords);

    /* 8. clear and release HW                    */
    esp_mp_hw_unlock();

    if (negcheck) {
        mp_sub(M, &tmpZ, &tmpZ);
    }

    mp_copy(&tmpZ, Z);
    mp_clear(&tmpZ);
    mp_clear(&r_inv);

    return ret;
    /* end if CONFIG_IDF_TARGET_ESP32S3 */
#else
    /* non-S3 Xtensa */

    /*Steps to use HW in the following order:
    * 1. wait until clean HW engine
    * 2. Write(N/512bits - 1) to MULT_MODE_REG
    * 3. Write X,M(=G, X, P) to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 4. Write M' to M_PRIME_REG
    * 5. Write 1  to MODEXP_START_REG
    * 6. Wait for the first operation to be done. Poll INTERRUPT_REG until it reads 1.
    *    (Or until the INTER interrupt is generated.)
    * 7. Write 1 to RSA_INTERRUPT_REG to clear the interrupt.
    * 8. Write Y to RSA_X_MEM
    * 9. Write 1 to RSA_MULT_START_REG
    * 10. Wait for the second operation to be completed. Poll INTERRUPT_REG until it reads 1.
    * 11. Read the Z from RSA_Z_MEM
    * 12. Write 1 to RSA_INTERUPT_REG to clear the interrupt.
    * 13. Release the HW engine
    */

    if ( (ret = esp_mp_hw_wait_clean()) != MP_OKAY ) {
        return ret;
    }
    /* step.1                     512 bits => 16 words */
    DPORT_REG_WRITE(RSA_MULT_MODE_REG, (hwWords_sz >> 4) - 1);

    /* step.2 write X, M and r_inv into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE,
                          &r_inv,
                          mp_count_bits(&r_inv),
                          hwWords_sz);

    /* step.3 write M' into memory                   */
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);

    /* step.4 start process                           */
    process_start(RSA_MULT_START_REG);

    /* step.5,6 wait until done                       */
    wait_until_done(RSA_INTERRUPT_REG);
    /* step.7 Y to MEM_X                              */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, Y, Ys, hwWords_sz);

    /* step.8 start process                           */
    process_start(RSA_MULT_START_REG);

    /* step.9,11 wait until done                      */
    wait_until_done(RSA_INTERRUPT_REG);

    /* step.12 read the result from MEM_Z             */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, &tmpZ, zwords);

    /* step.13 clear and release HW                   */
    esp_mp_hw_unlock();

    /* additional steps                               */
    /* this needs for known issue when Z is greater than M */
    if (mp_cmp(&tmpZ, M) == MP_GT) {
        /*  Z -= M  */
        mp_sub(&tmpZ, M, &tmpZ);
    }
    if (negcheck) {
        mp_sub(M, &tmpZ, &tmpZ);
    }

    mp_copy(&tmpZ, Z);

    mp_clear(&tmpZ);
    mp_clear(&r_inv);

    return ret;
#endif
}

/* Large Number Modular Exponentiation
 *
 *    Z = X^Y mod M
 *
 * See:
 *  ESP32, Chapter 24, https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
 *  ESP32s3, section 20.3.1, https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf
 * The operation is based on Montgomery multiplication. Aside from the
 * arguments X, Y , and M, two additional ones are needed —r and M′
.* These arguments are calculated in advance by software.
.*
.* The RSA Accelerator supports operand lengths of N ∈ {512, 1024, 1536, 2048,
.* 2560, 3072, 3584, 4096} bits on the ESP32 and N ∈ [32, 4096] bits on the ESP32s3.
.* The bit length of arguments Z, X, Y , M, and r can be any one from the N set,
.* but all numbers in a calculation must be of the same length.
.* The bit length of M′ is always 32.
.*
.* Note some DH references may use: Y = (G ^ X) mod P
 */
int esp_mp_exptmod(MATH_INT_T* X, MATH_INT_T* Y, word32 Ys, MATH_INT_T* M, MATH_INT_T* Z)
{
    int ret = 0;

    word32 Xs;
    word32 Ms;
    word32 maxWords_sz;
    word32 hwWords_sz;

    MATH_INT_T r_inv;
    mp_digit mp;

#if CONFIG_IDF_TARGET_ESP32S3
    uint32_t OperandBits;
    uint32_t WordsForOperand;
#endif

    /* ask bits number */
    Xs = mp_count_bits(X);
    Ms = mp_count_bits(M);
    /* maximum bits and words for writing to HW */
    maxWords_sz = bits2words(max(Xs, max(Ys, Ms)));
    hwWords_sz  = words2hwords(maxWords_sz);

    if ((hwWords_sz << 5) > ESP_HW_RSAMAX_BIT) {
        ESP_LOGE(TAG, "exceeds HW maximum bits");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    /* calculate r_inv = R^2 mode M
    *    where: R = b^n, and b = 2^32
    *    accordingly R^2 = 2^(n*32*2)
    */
    ret = mp_init(&r_inv);
    if ( (ret == 0) &&
         ((ret = esp_get_rinv(&r_inv, M, (hwWords_sz << 6))) != MP_OKAY) ) {
        ESP_LOGE(TAG, "calculate r_inv failed.");
        mp_clear(&r_inv);
        return ret;
    }
    /* lock and init the HW                           */
    if ( (ret = esp_mp_hw_lock()) != MP_OKAY ) {
        mp_clear(&r_inv);
        return ret;
    }
    /* calc M' */
    /* if Pm is odd, uses mp_montgomery_setup() */
    if ( (ret = esp_calc_Mdash(M, 32/* bits */, &mp)) != MP_OKAY ) {
        ESP_LOGE(TAG, "failed to calculate M dash");
        mp_clear(&r_inv);
        return ret;
    }

#if CONFIG_IDF_TARGET_ESP32S3
    /* Steps to perform large number modular exponentiation. Calculates Z = (X ^ Y) modulo M.
     * The number of bits in the operands (X, Y) is N. N can be 32x, where x = {1,2,3,...64}, so the
     * maximum number of bits in the X and Y is 2048.
     * See 20.3.3 of ESP32-S3 technical manual
     *  1. Wait until the hardware is ready.
     *  2. Enable/disable interrupt that signals completion -- we don't use the interrupt.
     *  3. Write (N_bits/32 - 1) to the RSA_MODE_REG (now called RSA_LENGTH_REG).
     *     Here N_bits is the maximum number of bits in X, Y and M.
     *  4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG).
     *  5. Load X, Y, M, r' operands to memory blocks.
     *  6. Start the operation by writing 1 to RSA_MODEXP_START_REG, then wait for it
     *     to complete by monitoring RSA_IDLE_REG (which is now called RSA_QUERY_INTERRUPT_REG).
     *  7. Read the result out.
     *  8. Release the hardware lock so others can use it.
     *  x. Clear the interrupt flag, if you used it (we don't). */

    /* 1. Wait until hardware is ready. */
    if ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        return ret;
    }

    /* 2. Disable completion interrupt signal; we don't use.
    **    0 => no interrupt; 1 => interrupt on completion. */
    DPORT_REG_WRITE(RSA_INTERRUPT_REG, 0);

    /* 3. Write (N_result_bits/32 - 1) to the RSA_MODE_REG. */
    OperandBits = max(max(Xs, Ys), Ms);
    if (OperandBits > ESP_HW_MULTI_RSAMAX_BITS) {
        ESP_LOGW(TAG, "result exceeds max bit length");
        return MP_VAL; /*  Error: value is not able to be used. */
    }
    WordsForOperand = bits2words(OperandBits);
    DPORT_REG_WRITE(RSA_LENGTH_REG, WordsForOperand - 1);

    /* 4. Write M' value into RSA_M_PRIME_REG (now called RSA_M_DASH_REG) */
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);

    /* 5. Load X, Y, M, r' operands. */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y, Ys, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE, &r_inv,
                          mp_count_bits(&r_inv), hwWords_sz);

    /* 6. Start operation and wait until it completes. */
    process_start(RSA_MODEXP_START_REG);
    ret = wait_until_done(RSA_QUERY_INTERRUPT_REG);
    if (MP_OKAY != ret) {
        return ret;
    }

    /* 7. read the result form MEM_Z              */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(Ms));

    /* 8. clear and release HW                    */
    esp_mp_hw_unlock();

    mp_clear(&r_inv);

    return ret;
    /* end if CONFIG_IDF_TARGET_ESP32S3 */
#else
    /* non-ESP32S3 Xtensa (regular ESP32) */

    /* Steps to use HW in the following order:
    * 1. Write(N/512bits - 1) to MODEXP_MODE_REG
    * 2. Write X, Y, M and r_inv to memory blocks
    *    need to write data to each memory block only according to the length
    *    of the number.
    * 3. Write M' to M_PRIME_REG
    * 4. Write 1  to MODEXP_START_REG
    * 5. Wait for the operation to be done. Poll INTERRUPT_REG until it reads 1.
    *    (Or until the INTER interrupt is generated.)
    * 6. Read the result Z(=Y) from Z_MEM
    * 7. Write 1 to INTERRUPT_REG to clear the interrupt.
    */
    if ((ret = esp_mp_hw_wait_clean()) != MP_OKAY) {
        return ret;
    }

    /* step.1                                         */
    DPORT_REG_WRITE(RSA_MODEXP_MODE_REG, (hwWords_sz >> 4) - 1);
    /* step.2 write G, X, P, r_inv and M' into memory */
    esp_mpint_to_memblock(RSA_MEM_X_BLOCK_BASE, X, Xs, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Y_BLOCK_BASE, Y, Ys, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_M_BLOCK_BASE, M, Ms, hwWords_sz);
    esp_mpint_to_memblock(RSA_MEM_Z_BLOCK_BASE,
                          &r_inv,
                          mp_count_bits(&r_inv),
                          hwWords_sz);
    /* step.3 write M' into memory                    */
    DPORT_REG_WRITE(RSA_M_DASH_REG, mp);
    /* step.4 start process                           */
    process_start(RSA_START_MODEXP_REG);

    /* step.5 wait until done                         */
    wait_until_done(RSA_INTERRUPT_REG);
    /* step.6 read a result form memory               */
    esp_memblock_to_mpint(RSA_MEM_Z_BLOCK_BASE, Z, BITS_TO_WORDS(Ms));
    /* step.7 clear and release HW                    */
    esp_mp_hw_unlock();

    mp_clear(&r_inv);

    return ret;
#endif
}

#endif /* WOLFSSL_ESP32_CRYPT_RSA_PRI) &&
        * !NO_WOLFSSL_ESP32_CRYPT_RSA_PRI */

#endif /* !NO_RSA || HAVE_ECC */
