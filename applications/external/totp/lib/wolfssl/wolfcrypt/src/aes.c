/* aes.c
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
This library provides the interfaces to the Advanced Encryption Standard (AES)
for encrypting and decrypting data. AES is the standard known for a symmetric
block cipher mechanism that uses n-bit binary string parameter key with 128-bits,
192-bits, and 256-bits of key sizes.

*/
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#if !defined(NO_AES)

/* Tip: Locate the software cipher modes by searching for "Software AES" */

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)

    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

    #ifdef USE_WINDOWS_API
        #pragma code_seg(".fipsA$g")
        #pragma const_seg(".fipsB$g")
    #endif
#endif

#include <wolfssl/wolfcrypt/aes.h>

#ifdef WOLFSSL_AESNI
#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#endif /* WOLFSSL_AESNI */

#include <wolfssl/wolfcrypt/cpuid.h>

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef WOLFSSL_SECO_CAAM
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#endif

#ifdef WOLFSSL_IMXRT_DCP
    #include <wolfssl/wolfcrypt/port/nxp/dcp_port.h>
#endif
#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    #include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif

#ifdef WOLFSSL_AES_SIV
    #include <wolfssl/wolfcrypt/cmac.h>
#endif

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    #include <wolfssl/wolfcrypt/port/psa/psa.h>
#endif

#if defined(WOLFSSL_TI_CRYPT)
    #include <wolfcrypt/src/port/ti/ti-aes.c>
#else

#include <wolfssl/wolfcrypt/logging.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifndef WOLFSSL_ARMASM

#ifdef WOLFSSL_IMX6_CAAM_BLOB
    /* case of possibly not using hardware acceleration for AES but using key
       blobs */
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#endif

#ifdef DEBUG_AESNI
    #include <stdio.h>
#endif

#ifdef _MSC_VER
    /* 4127 warning constant while(1)  */
    #pragma warning(disable: 4127)
#endif


/* Define AES implementation includes and functions */
#if defined(STM32_CRYPTO)
     /* STM32F2/F4/F7/L4/L5/H7/WB55 hardware AES support for ECB, CBC, CTR and GCM modes */

#if defined(WOLFSSL_AES_DIRECT) || defined(HAVE_AESGCM) || defined(HAVE_AESCCM)

    static WARN_UNUSED_RESULT int wc_AesEncrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        int ret = 0;
    #ifdef WOLFSSL_STM32_CUBEMX
        CRYP_HandleTypeDef hcryp;
    #else
        CRYP_InitTypeDef cryptInit;
        CRYP_KeyInitTypeDef keyInit;
    #endif

    #ifdef WOLFSSL_STM32_CUBEMX
        ret = wc_Stm32_Aes_Init(aes, &hcryp);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0)
            return ret;

    #if defined(STM32_HAL_V2)
        hcryp.Init.Algorithm  = CRYP_AES_ECB;
    #elif defined(STM32_CRYPTO_AES_ONLY)
        hcryp.Init.OperatingMode = CRYP_ALGOMODE_ENCRYPT;
        hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_ECB;
        hcryp.Init.KeyWriteFlag  = CRYP_KEY_WRITE_ENABLE;
    #endif
        HAL_CRYP_Init(&hcryp);

    #if defined(STM32_HAL_V2)
        ret = HAL_CRYP_Encrypt(&hcryp, (uint32_t*)inBlock, AES_BLOCK_SIZE,
            (uint32_t*)outBlock, STM32_HAL_TIMEOUT);
    #elif defined(STM32_CRYPTO_AES_ONLY)
        ret = HAL_CRYPEx_AES(&hcryp, (uint8_t*)inBlock, AES_BLOCK_SIZE,
            outBlock, STM32_HAL_TIMEOUT);
    #else
        ret = HAL_CRYP_AESECB_Encrypt(&hcryp, (uint8_t*)inBlock, AES_BLOCK_SIZE,
            outBlock, STM32_HAL_TIMEOUT);
    #endif
        if (ret != HAL_OK) {
            ret = WC_TIMEOUT_E;
        }
        HAL_CRYP_DeInit(&hcryp);

    #else /* Standard Peripheral Library */
        ret = wc_Stm32_Aes_Init(aes, &cryptInit, &keyInit);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0)
            return ret;

        /* reset registers to their default values */
        CRYP_DeInit();

        /* setup key */
        CRYP_KeyInit(&keyInit);

        /* set direction and mode */
        cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Encrypt;
        cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_ECB;
        CRYP_Init(&cryptInit);

        /* enable crypto processor */
        CRYP_Cmd(ENABLE);

        /* flush IN/OUT FIFOs */
        CRYP_FIFOFlush();

        CRYP_DataIn(*(uint32_t*)&inBlock[0]);
        CRYP_DataIn(*(uint32_t*)&inBlock[4]);
        CRYP_DataIn(*(uint32_t*)&inBlock[8]);
        CRYP_DataIn(*(uint32_t*)&inBlock[12]);

        /* wait until the complete message has been processed */
        while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

        *(uint32_t*)&outBlock[0]  = CRYP_DataOut();
        *(uint32_t*)&outBlock[4]  = CRYP_DataOut();
        *(uint32_t*)&outBlock[8]  = CRYP_DataOut();
        *(uint32_t*)&outBlock[12] = CRYP_DataOut();

        /* disable crypto processor */
        CRYP_Cmd(DISABLE);
    #endif /* WOLFSSL_STM32_CUBEMX */
        wolfSSL_CryptHwMutexUnLock();
        wc_Stm32_Aes_Cleanup();

        return ret;
    }
#endif /* WOLFSSL_AES_DIRECT || HAVE_AESGCM || HAVE_AESCCM */

#ifdef HAVE_AES_DECRYPT
    #if defined(WOLFSSL_AES_DIRECT) || defined(HAVE_AESCCM)
    static WARN_UNUSED_RESULT int wc_AesDecrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        int ret = 0;
    #ifdef WOLFSSL_STM32_CUBEMX
        CRYP_HandleTypeDef hcryp;
    #else
        CRYP_InitTypeDef cryptInit;
        CRYP_KeyInitTypeDef keyInit;
    #endif

    #ifdef WOLFSSL_STM32_CUBEMX
        ret = wc_Stm32_Aes_Init(aes, &hcryp);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0)
            return ret;

    #if defined(STM32_HAL_V2)
        hcryp.Init.Algorithm  = CRYP_AES_ECB;
    #elif defined(STM32_CRYPTO_AES_ONLY)
        hcryp.Init.OperatingMode = CRYP_ALGOMODE_KEYDERIVATION_DECRYPT;
        hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_ECB;
        hcryp.Init.KeyWriteFlag  = CRYP_KEY_WRITE_ENABLE;
    #endif
        HAL_CRYP_Init(&hcryp);

    #if defined(STM32_HAL_V2)
        ret = HAL_CRYP_Decrypt(&hcryp, (uint32_t*)inBlock, AES_BLOCK_SIZE,
            (uint32_t*)outBlock, STM32_HAL_TIMEOUT);
    #elif defined(STM32_CRYPTO_AES_ONLY)
        ret = HAL_CRYPEx_AES(&hcryp, (uint8_t*)inBlock, AES_BLOCK_SIZE,
            outBlock, STM32_HAL_TIMEOUT);
    #else
        ret = HAL_CRYP_AESECB_Decrypt(&hcryp, (uint8_t*)inBlock, AES_BLOCK_SIZE,
            outBlock, STM32_HAL_TIMEOUT);
    #endif
        if (ret != HAL_OK) {
            ret = WC_TIMEOUT_E;
        }
        HAL_CRYP_DeInit(&hcryp);

    #else /* Standard Peripheral Library */
        ret = wc_Stm32_Aes_Init(aes, &cryptInit, &keyInit);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0)
            return ret;

        /* reset registers to their default values */
        CRYP_DeInit();

        /* set direction and key */
        CRYP_KeyInit(&keyInit);
        cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Decrypt;
        cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_Key;
        CRYP_Init(&cryptInit);

        /* enable crypto processor */
        CRYP_Cmd(ENABLE);

        /* wait until decrypt key has been initialized */
        while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

        /* set direction and mode */
        cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Decrypt;
        cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_ECB;
        CRYP_Init(&cryptInit);

        /* enable crypto processor */
        CRYP_Cmd(ENABLE);

        /* flush IN/OUT FIFOs */
        CRYP_FIFOFlush();

        CRYP_DataIn(*(uint32_t*)&inBlock[0]);
        CRYP_DataIn(*(uint32_t*)&inBlock[4]);
        CRYP_DataIn(*(uint32_t*)&inBlock[8]);
        CRYP_DataIn(*(uint32_t*)&inBlock[12]);

        /* wait until the complete message has been processed */
        while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

        *(uint32_t*)&outBlock[0]  = CRYP_DataOut();
        *(uint32_t*)&outBlock[4]  = CRYP_DataOut();
        *(uint32_t*)&outBlock[8]  = CRYP_DataOut();
        *(uint32_t*)&outBlock[12] = CRYP_DataOut();

        /* disable crypto processor */
        CRYP_Cmd(DISABLE);
    #endif /* WOLFSSL_STM32_CUBEMX */
        wolfSSL_CryptHwMutexUnLock();
        wc_Stm32_Aes_Cleanup();

        return ret;
    }
    #endif /* WOLFSSL_AES_DIRECT || HAVE_AESCCM */
#endif /* HAVE_AES_DECRYPT */

#elif defined(HAVE_COLDFIRE_SEC)
    /* Freescale Coldfire SEC support for CBC mode.
     * NOTE: no support for AES-CTR/GCM/CCM/Direct */
    #include <wolfssl/wolfcrypt/types.h>
    #include "sec.h"
    #include "mcf5475_sec.h"
    #include "mcf5475_siu.h"
#elif defined(FREESCALE_LTC)
    #include "fsl_ltc.h"
    #if defined(FREESCALE_LTC_AES_GCM)
        #undef NEED_AES_TABLES
        #undef GCM_TABLE
    #endif

        /* if LTC doesn't have GCM, use software with LTC AES ECB mode */
        static WARN_UNUSED_RESULT int wc_AesEncrypt(
            Aes* aes, const byte* inBlock, byte* outBlock)
        {
            word32 keySize = 0;
            byte* key = (byte*)aes->key;
            int ret = wc_AesGetKeySize(aes, &keySize);
            if (ret != 0)
                return ret;

            if (wolfSSL_CryptHwMutexLock() == 0) {
                LTC_AES_EncryptEcb(LTC_BASE, inBlock, outBlock, AES_BLOCK_SIZE,
                    key, keySize);
                wolfSSL_CryptHwMutexUnLock();
            }
            return 0;
        }
        #ifdef HAVE_AES_DECRYPT
        static WARN_UNUSED_RESULT int wc_AesDecrypt(
            Aes* aes, const byte* inBlock, byte* outBlock)
        {
            word32 keySize = 0;
            byte* key = (byte*)aes->key;
            int ret = wc_AesGetKeySize(aes, &keySize);
            if (ret != 0)
                return ret;

            if (wolfSSL_CryptHwMutexLock() == 0) {
                LTC_AES_DecryptEcb(LTC_BASE, inBlock, outBlock, AES_BLOCK_SIZE,
                    key, keySize, kLTC_EncryptKey);
                wolfSSL_CryptHwMutexUnLock();
            }
            return 0;
        }
        #endif

#elif defined(FREESCALE_MMCAU)
    /* Freescale mmCAU hardware AES support for Direct, CBC, CCM, GCM modes
     * through the CAU/mmCAU library. Documentation located in
     * ColdFire/ColdFire+ CAU and Kinetis mmCAU Software Library User
     * Guide (See note in README). */
    #ifdef FREESCALE_MMCAU_CLASSIC
        /* MMCAU 1.4 library used with non-KSDK / classic MQX builds */
        #include "cau_api.h"
    #else
        #include "fsl_mmcau.h"
    #endif

    static WARN_UNUSED_RESULT int wc_AesEncrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        if (wolfSSL_CryptHwMutexLock() == 0) {
        #ifdef FREESCALE_MMCAU_CLASSIC
            if ((wc_ptr_t)outBlock % WOLFSSL_MMCAU_ALIGNMENT) {
                WOLFSSL_MSG("Bad cau_aes_encrypt alignment");
                return BAD_ALIGN_E;
            }
            cau_aes_encrypt(inBlock, (byte*)aes->key, aes->rounds, outBlock);
        #else
            MMCAU_AES_EncryptEcb(inBlock, (byte*)aes->key, aes->rounds,
                                 outBlock);
        #endif
            wolfSSL_CryptHwMutexUnLock();
        }
        return 0;
    }
    #ifdef HAVE_AES_DECRYPT
    static WARN_UNUSED_RESULT int wc_AesDecrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        if (wolfSSL_CryptHwMutexLock() == 0) {
        #ifdef FREESCALE_MMCAU_CLASSIC
            if ((wc_ptr_t)outBlock % WOLFSSL_MMCAU_ALIGNMENT) {
                WOLFSSL_MSG("Bad cau_aes_decrypt alignment");
                return BAD_ALIGN_E;
            }
            cau_aes_decrypt(inBlock, (byte*)aes->key, aes->rounds, outBlock);
        #else
            MMCAU_AES_DecryptEcb(inBlock, (byte*)aes->key, aes->rounds,
                                 outBlock);
        #endif
            wolfSSL_CryptHwMutexUnLock();
        }
        return 0;
    }
    #endif /* HAVE_AES_DECRYPT */

#elif defined(WOLFSSL_PIC32MZ_CRYPT)

    #include <wolfssl/wolfcrypt/port/pic32/pic32mz-crypt.h>

    #if defined(HAVE_AESGCM) || defined(WOLFSSL_AES_DIRECT)
    static WARN_UNUSED_RESULT int wc_AesEncrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        /* Thread mutex protection handled in Pic32Crypto */
        return wc_Pic32AesCrypt(aes->key, aes->keylen, NULL, 0,
            outBlock, inBlock, AES_BLOCK_SIZE,
            PIC32_ENCRYPTION, PIC32_ALGO_AES, PIC32_CRYPTOALGO_RECB);
    }
    #endif

    #if defined(HAVE_AES_DECRYPT) && defined(WOLFSSL_AES_DIRECT)
    static WARN_UNUSED_RESULT int wc_AesDecrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        /* Thread mutex protection handled in Pic32Crypto */
        return wc_Pic32AesCrypt(aes->key, aes->keylen, NULL, 0,
            outBlock, inBlock, AES_BLOCK_SIZE,
            PIC32_DECRYPTION, PIC32_ALGO_AES, PIC32_CRYPTOALGO_RECB);
    }
    #endif

#elif defined(WOLFSSL_NRF51_AES)
    /* Use built-in AES hardware - AES 128 ECB Encrypt Only */
    #include "wolfssl/wolfcrypt/port/nrf51.h"

    static WARN_UNUSED_RESULT int wc_AesEncrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        int ret;
        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = nrf51_aes_encrypt(inBlock, (byte*)aes->key, aes->rounds,
                                    outBlock);
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

    #ifdef HAVE_AES_DECRYPT
        #error nRF51 AES Hardware does not support decrypt
    #endif /* HAVE_AES_DECRYPT */

#elif defined(WOLFSSL_ESP32_CRYPT) && \
    !defined(NO_WOLFSSL_ESP32_CRYPT_AES)

    #include "wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h"

    #if defined(HAVE_AESGCM) || defined(WOLFSSL_AES_DIRECT)
    static WARN_UNUSED_RESULT int wc_AesEncrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        /* Thread mutex protection handled in esp_aes_hw_InUse */
        return wc_esp32AesEncrypt(aes, inBlock, outBlock);
    }
    #endif

    #if defined(HAVE_AES_DECRYPT) && defined(WOLFSSL_AES_DIRECT)
    static WARN_UNUSED_RESULT int wc_AesDecrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        /* Thread mutex protection handled in esp_aes_hw_InUse */
       return wc_esp32AesDecrypt(aes, inBlock, outBlock);
    }
    #endif

#elif defined(WOLFSSL_AESNI)

    #define NEED_AES_TABLES

    /* Each platform needs to query info type 1 from cpuid to see if aesni is
     * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
     */

    #ifndef AESNI_ALIGN
        #define AESNI_ALIGN 16
    #endif

    static int checkAESNI = 0;
    static int haveAESNI  = 0;
    static word32 intel_flags = 0;

    static WARN_UNUSED_RESULT int Check_CPU_support_AES(void)
    {
        intel_flags = cpuid_get_flags();

        return IS_INTEL_AESNI(intel_flags) != 0;
    }


    /* tell C compiler these are asm functions in case any mix up of ABI underscore
       prefix between clang/gcc/llvm etc */
    #ifdef HAVE_AES_CBC
        void AES_CBC_encrypt(const unsigned char* in, unsigned char* out,
                             unsigned char* ivec, unsigned long length,
                             const unsigned char* KS, int nr)
                             XASM_LINK("AES_CBC_encrypt");

        #ifdef HAVE_AES_DECRYPT
            #if defined(WOLFSSL_AESNI_BY4) || defined(WOLFSSL_X86_BUILD)
                void AES_CBC_decrypt_by4(const unsigned char* in, unsigned char* out,
                                         unsigned char* ivec, unsigned long length,
                                         const unsigned char* KS, int nr)
                                         XASM_LINK("AES_CBC_decrypt_by4");
            #elif defined(WOLFSSL_AESNI_BY6)
                void AES_CBC_decrypt_by6(const unsigned char* in, unsigned char* out,
                                         unsigned char* ivec, unsigned long length,
                                         const unsigned char* KS, int nr)
                                         XASM_LINK("AES_CBC_decrypt_by6");
            #else /* WOLFSSL_AESNI_BYx */
                void AES_CBC_decrypt_by8(const unsigned char* in, unsigned char* out,
                                         unsigned char* ivec, unsigned long length,
                                         const unsigned char* KS, int nr)
                                         XASM_LINK("AES_CBC_decrypt_by8");
            #endif /* WOLFSSL_AESNI_BYx */
        #endif /* HAVE_AES_DECRYPT */
    #endif /* HAVE_AES_CBC */

    void AES_ECB_encrypt(const unsigned char* in, unsigned char* out,
                         unsigned long length, const unsigned char* KS, int nr)
                         XASM_LINK("AES_ECB_encrypt");

    #ifdef HAVE_AES_DECRYPT
        void AES_ECB_decrypt(const unsigned char* in, unsigned char* out,
                             unsigned long length, const unsigned char* KS, int nr)
                             XASM_LINK("AES_ECB_decrypt");
    #endif

    void AES_128_Key_Expansion(const unsigned char* userkey,
                               unsigned char* key_schedule)
                               XASM_LINK("AES_128_Key_Expansion");

    void AES_192_Key_Expansion(const unsigned char* userkey,
                               unsigned char* key_schedule)
                               XASM_LINK("AES_192_Key_Expansion");

    void AES_256_Key_Expansion(const unsigned char* userkey,
                               unsigned char* key_schedule)
                               XASM_LINK("AES_256_Key_Expansion");


    static WARN_UNUSED_RESULT int AES_set_encrypt_key(
        const unsigned char *userKey, const int bits, Aes* aes)
    {
        int ret;

        if (!userKey || !aes)
            return BAD_FUNC_ARG;

        switch (bits) {
            case 128:
               AES_128_Key_Expansion (userKey,(byte*)aes->key); aes->rounds = 10;
               return 0;
            case 192:
               AES_192_Key_Expansion (userKey,(byte*)aes->key); aes->rounds = 12;
               return 0;
            case 256:
               AES_256_Key_Expansion (userKey,(byte*)aes->key); aes->rounds = 14;
               return 0;
            default:
                ret = BAD_FUNC_ARG;
        }

        return ret;
    }

    #ifdef HAVE_AES_DECRYPT
        static WARN_UNUSED_RESULT int AES_set_decrypt_key(
            const unsigned char* userKey, const int bits, Aes* aes)
        {
            word32 nr;
#ifdef WOLFSSL_SMALL_STACK
            Aes *temp_key;
#else
            Aes temp_key[1];
#endif
            __m128i *Key_Schedule;
            __m128i *Temp_Key_Schedule;

            if (!userKey || !aes)
                return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
            if ((temp_key = (Aes *)XMALLOC(sizeof *aes, aes->heap,
                                           DYNAMIC_TYPE_AES)) == NULL)
                return MEMORY_E;
#endif

            if (AES_set_encrypt_key(userKey,bits,temp_key) == BAD_FUNC_ARG) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(temp_key, aes->heap, DYNAMIC_TYPE_AES);
#endif
                return BAD_FUNC_ARG;
            }

            Key_Schedule = (__m128i*)aes->key;
            Temp_Key_Schedule = (__m128i*)temp_key->key;

            nr = temp_key->rounds;
            aes->rounds = nr;

#ifdef WOLFSSL_SMALL_STACK
            SAVE_VECTOR_REGISTERS(XFREE(temp_key, aes->heap, DYNAMIC_TYPE_AES); return _svr_ret;);
#else
            SAVE_VECTOR_REGISTERS(return _svr_ret;);
#endif

            Key_Schedule[nr] = Temp_Key_Schedule[0];
            Key_Schedule[nr-1] = _mm_aesimc_si128(Temp_Key_Schedule[1]);
            Key_Schedule[nr-2] = _mm_aesimc_si128(Temp_Key_Schedule[2]);
            Key_Schedule[nr-3] = _mm_aesimc_si128(Temp_Key_Schedule[3]);
            Key_Schedule[nr-4] = _mm_aesimc_si128(Temp_Key_Schedule[4]);
            Key_Schedule[nr-5] = _mm_aesimc_si128(Temp_Key_Schedule[5]);
            Key_Schedule[nr-6] = _mm_aesimc_si128(Temp_Key_Schedule[6]);
            Key_Schedule[nr-7] = _mm_aesimc_si128(Temp_Key_Schedule[7]);
            Key_Schedule[nr-8] = _mm_aesimc_si128(Temp_Key_Schedule[8]);
            Key_Schedule[nr-9] = _mm_aesimc_si128(Temp_Key_Schedule[9]);

            if (nr>10) {
                Key_Schedule[nr-10] = _mm_aesimc_si128(Temp_Key_Schedule[10]);
                Key_Schedule[nr-11] = _mm_aesimc_si128(Temp_Key_Schedule[11]);
            }

            if (nr>12) {
                Key_Schedule[nr-12] = _mm_aesimc_si128(Temp_Key_Schedule[12]);
                Key_Schedule[nr-13] = _mm_aesimc_si128(Temp_Key_Schedule[13]);
            }

            Key_Schedule[0] = Temp_Key_Schedule[nr];

            RESTORE_VECTOR_REGISTERS();

#ifdef WOLFSSL_SMALL_STACK
            XFREE(temp_key, aes->heap, DYNAMIC_TYPE_AES);
#endif

            return 0;
        }
    #endif /* HAVE_AES_DECRYPT */

#elif (defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) \
        && !defined(WOLFSSL_QNX_CAAM)) || \
      ((defined(WOLFSSL_AFALG) || defined(WOLFSSL_DEVCRYPTO_AES)) && \
        defined(HAVE_AESCCM))
        static WARN_UNUSED_RESULT int wc_AesEncrypt(
            Aes* aes, const byte* inBlock, byte* outBlock)
        {
            return wc_AesEncryptDirect(aes, outBlock, inBlock);
        }

#elif defined(WOLFSSL_AFALG)
    /* implemented in wolfcrypt/src/port/af_alg/afalg_aes.c */

#elif defined(WOLFSSL_DEVCRYPTO_AES)
    /* implemented in wolfcrypt/src/port/devcrypto/devcrypto_aes.c */

#elif defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_AES)
    #include "hal_data.h"

    #ifndef WOLFSSL_SCE_AES256_HANDLE
        #define WOLFSSL_SCE_AES256_HANDLE g_sce_aes_256
    #endif

    #ifndef WOLFSSL_SCE_AES192_HANDLE
        #define WOLFSSL_SCE_AES192_HANDLE g_sce_aes_192
    #endif

    #ifndef WOLFSSL_SCE_AES128_HANDLE
        #define WOLFSSL_SCE_AES128_HANDLE g_sce_aes_128
    #endif

    static WARN_UNUSED_RESULT int AES_ECB_encrypt(
        Aes* aes, const byte* inBlock, byte* outBlock, int sz)
    {
        word32 ret;

        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag ==
                CRYPTO_WORD_ENDIAN_BIG) {
            ByteReverseWords((word32*)inBlock, (word32*)inBlock, sz);
        }

        switch (aes->keylen) {
        #ifdef WOLFSSL_AES_128
            case AES_128_KEY_SIZE:
                ret = WOLFSSL_SCE_AES128_HANDLE.p_api->encrypt(
                        WOLFSSL_SCE_AES128_HANDLE.p_ctrl, aes->key,
                        NULL, (sz / sizeof(word32)), (word32*)inBlock,
                        (word32*)outBlock);
                break;
        #endif
        #ifdef WOLFSSL_AES_192
            case AES_192_KEY_SIZE:
                ret = WOLFSSL_SCE_AES192_HANDLE.p_api->encrypt(
                        WOLFSSL_SCE_AES192_HANDLE.p_ctrl, aes->key,
                        NULL, (sz / sizeof(word32)), (word32*)inBlock,
                        (word32*)outBlock);
                break;
        #endif
        #ifdef WOLFSSL_AES_256
            case AES_256_KEY_SIZE:
                ret = WOLFSSL_SCE_AES256_HANDLE.p_api->encrypt(
                        WOLFSSL_SCE_AES256_HANDLE.p_ctrl, aes->key,
                        NULL, (sz / sizeof(word32)), (word32*)inBlock,
                        (word32*)outBlock);
                break;
        #endif
            default:
                WOLFSSL_MSG("Unknown key size");
                return BAD_FUNC_ARG;
        }

        if (ret != SSP_SUCCESS) {
            /* revert input */
            ByteReverseWords((word32*)inBlock, (word32*)inBlock, sz);
            return WC_HW_E;
        }

        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag ==
                CRYPTO_WORD_ENDIAN_BIG) {
            ByteReverseWords((word32*)outBlock, (word32*)outBlock, sz);
            if (inBlock != outBlock) {
                /* revert input */
                ByteReverseWords((word32*)inBlock, (word32*)inBlock, sz);
            }
        }
        return 0;
    }

    #if defined(HAVE_AES_DECRYPT)
    static WARN_UNUSED_RESULT int AES_ECB_decrypt(
        Aes* aes, const byte* inBlock, byte* outBlock, int sz)
    {
        word32 ret;

        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag ==
                CRYPTO_WORD_ENDIAN_BIG) {
            ByteReverseWords((word32*)inBlock, (word32*)inBlock, sz);
        }

        switch (aes->keylen) {
        #ifdef WOLFSSL_AES_128
            case AES_128_KEY_SIZE:
                ret = WOLFSSL_SCE_AES128_HANDLE.p_api->decrypt(
                        WOLFSSL_SCE_AES128_HANDLE.p_ctrl, aes->key, aes->reg,
                        (sz / sizeof(word32)), (word32*)inBlock,
                        (word32*)outBlock);
                break;
        #endif
        #ifdef WOLFSSL_AES_192
            case AES_192_KEY_SIZE:
                ret = WOLFSSL_SCE_AES192_HANDLE.p_api->decrypt(
                        WOLFSSL_SCE_AES192_HANDLE.p_ctrl, aes->key, aes->reg,
                        (sz / sizeof(word32)), (word32*)inBlock,
                        (word32*)outBlock);
                break;
        #endif
        #ifdef WOLFSSL_AES_256
            case AES_256_KEY_SIZE:
                ret = WOLFSSL_SCE_AES256_HANDLE.p_api->decrypt(
                        WOLFSSL_SCE_AES256_HANDLE.p_ctrl, aes->key, aes->reg,
                        (sz / sizeof(word32)), (word32*)inBlock,
                        (word32*)outBlock);
                break;
        #endif
            default:
                WOLFSSL_MSG("Unknown key size");
                return BAD_FUNC_ARG;
        }
        if (ret != SSP_SUCCESS) {
            return WC_HW_E;
        }

        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag ==
                CRYPTO_WORD_ENDIAN_BIG) {
            ByteReverseWords((word32*)outBlock, (word32*)outBlock, sz);
            if (inBlock != outBlock) {
                /* revert input */
                ByteReverseWords((word32*)inBlock, (word32*)inBlock, sz);
            }
        }

        return 0;
    }
    #endif /* HAVE_AES_DECRYPT */

    #if defined(HAVE_AESGCM) || defined(WOLFSSL_AES_DIRECT)
    static WARN_UNUSED_RESULT int wc_AesEncrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        return AES_ECB_encrypt(aes, inBlock, outBlock, AES_BLOCK_SIZE);
    }
    #endif

    #if defined(HAVE_AES_DECRYPT) && defined(WOLFSSL_AES_DIRECT)
    static WARN_UNUSED_RESULT int wc_AesDecrypt(
        Aes* aes, const byte* inBlock, byte* outBlock)
    {
        return AES_ECB_decrypt(aes, inBlock, outBlock, AES_BLOCK_SIZE);
    }
    #endif

#elif defined(WOLFSSL_KCAPI_AES)
    /* Only CBC and GCM that are in wolfcrypt/src/port/kcapi/kcapi_aes.c */
    #if defined(WOLFSSL_AES_COUNTER) || defined(HAVE_AESCCM) || \
        defined(WOLFSSL_CMAC) || defined(WOLFSSL_AES_OFB) || \
        defined(WOLFSSL_AES_CFB) || defined(HAVE_AES_ECB) || \
        defined(WOLFSSL_AES_DIRECT) || \
        (defined(HAVE_AES_CBC) && defined(WOLFSSL_NO_KCAPI_AES_CBC))

        #define NEED_AES_TABLES
    #endif
#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
/* implemented in wolfcrypt/src/port/psa/psa_aes.c */

#else

    /* using wolfCrypt software implementation */
    #define NEED_AES_TABLES
#endif



#ifdef NEED_AES_TABLES

#if !defined(WOLFSSL_SILABS_SE_ACCEL)
static const FLASH_QUALIFIER word32 rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000,
    /* for 128-bit blocks, Rijndael never uses more than 10 rcon values */
};
#endif

#ifndef WOLFSSL_AES_SMALL_TABLES
static const FLASH_QUALIFIER word32 Te[4][256] = {
{
    0xc66363a5U, 0xf87c7c84U, 0xee777799U, 0xf67b7b8dU,
    0xfff2f20dU, 0xd66b6bbdU, 0xde6f6fb1U, 0x91c5c554U,
    0x60303050U, 0x02010103U, 0xce6767a9U, 0x562b2b7dU,
    0xe7fefe19U, 0xb5d7d762U, 0x4dababe6U, 0xec76769aU,
    0x8fcaca45U, 0x1f82829dU, 0x89c9c940U, 0xfa7d7d87U,
    0xeffafa15U, 0xb25959ebU, 0x8e4747c9U, 0xfbf0f00bU,
    0x41adadecU, 0xb3d4d467U, 0x5fa2a2fdU, 0x45afafeaU,
    0x239c9cbfU, 0x53a4a4f7U, 0xe4727296U, 0x9bc0c05bU,
    0x75b7b7c2U, 0xe1fdfd1cU, 0x3d9393aeU, 0x4c26266aU,
    0x6c36365aU, 0x7e3f3f41U, 0xf5f7f702U, 0x83cccc4fU,
    0x6834345cU, 0x51a5a5f4U, 0xd1e5e534U, 0xf9f1f108U,
    0xe2717193U, 0xabd8d873U, 0x62313153U, 0x2a15153fU,
    0x0804040cU, 0x95c7c752U, 0x46232365U, 0x9dc3c35eU,
    0x30181828U, 0x379696a1U, 0x0a05050fU, 0x2f9a9ab5U,
    0x0e070709U, 0x24121236U, 0x1b80809bU, 0xdfe2e23dU,
    0xcdebeb26U, 0x4e272769U, 0x7fb2b2cdU, 0xea75759fU,
    0x1209091bU, 0x1d83839eU, 0x582c2c74U, 0x341a1a2eU,
    0x361b1b2dU, 0xdc6e6eb2U, 0xb45a5aeeU, 0x5ba0a0fbU,
    0xa45252f6U, 0x763b3b4dU, 0xb7d6d661U, 0x7db3b3ceU,
    0x5229297bU, 0xdde3e33eU, 0x5e2f2f71U, 0x13848497U,
    0xa65353f5U, 0xb9d1d168U, 0x00000000U, 0xc1eded2cU,
    0x40202060U, 0xe3fcfc1fU, 0x79b1b1c8U, 0xb65b5bedU,
    0xd46a6abeU, 0x8dcbcb46U, 0x67bebed9U, 0x7239394bU,
    0x944a4adeU, 0x984c4cd4U, 0xb05858e8U, 0x85cfcf4aU,
    0xbbd0d06bU, 0xc5efef2aU, 0x4faaaae5U, 0xedfbfb16U,
    0x864343c5U, 0x9a4d4dd7U, 0x66333355U, 0x11858594U,
    0x8a4545cfU, 0xe9f9f910U, 0x04020206U, 0xfe7f7f81U,
    0xa05050f0U, 0x783c3c44U, 0x259f9fbaU, 0x4ba8a8e3U,
    0xa25151f3U, 0x5da3a3feU, 0x804040c0U, 0x058f8f8aU,
    0x3f9292adU, 0x219d9dbcU, 0x70383848U, 0xf1f5f504U,
    0x63bcbcdfU, 0x77b6b6c1U, 0xafdada75U, 0x42212163U,
    0x20101030U, 0xe5ffff1aU, 0xfdf3f30eU, 0xbfd2d26dU,
    0x81cdcd4cU, 0x180c0c14U, 0x26131335U, 0xc3ecec2fU,
    0xbe5f5fe1U, 0x359797a2U, 0x884444ccU, 0x2e171739U,
    0x93c4c457U, 0x55a7a7f2U, 0xfc7e7e82U, 0x7a3d3d47U,
    0xc86464acU, 0xba5d5de7U, 0x3219192bU, 0xe6737395U,
    0xc06060a0U, 0x19818198U, 0x9e4f4fd1U, 0xa3dcdc7fU,
    0x44222266U, 0x542a2a7eU, 0x3b9090abU, 0x0b888883U,
    0x8c4646caU, 0xc7eeee29U, 0x6bb8b8d3U, 0x2814143cU,
    0xa7dede79U, 0xbc5e5ee2U, 0x160b0b1dU, 0xaddbdb76U,
    0xdbe0e03bU, 0x64323256U, 0x743a3a4eU, 0x140a0a1eU,
    0x924949dbU, 0x0c06060aU, 0x4824246cU, 0xb85c5ce4U,
    0x9fc2c25dU, 0xbdd3d36eU, 0x43acacefU, 0xc46262a6U,
    0x399191a8U, 0x319595a4U, 0xd3e4e437U, 0xf279798bU,
    0xd5e7e732U, 0x8bc8c843U, 0x6e373759U, 0xda6d6db7U,
    0x018d8d8cU, 0xb1d5d564U, 0x9c4e4ed2U, 0x49a9a9e0U,
    0xd86c6cb4U, 0xac5656faU, 0xf3f4f407U, 0xcfeaea25U,
    0xca6565afU, 0xf47a7a8eU, 0x47aeaee9U, 0x10080818U,
    0x6fbabad5U, 0xf0787888U, 0x4a25256fU, 0x5c2e2e72U,
    0x381c1c24U, 0x57a6a6f1U, 0x73b4b4c7U, 0x97c6c651U,
    0xcbe8e823U, 0xa1dddd7cU, 0xe874749cU, 0x3e1f1f21U,
    0x964b4bddU, 0x61bdbddcU, 0x0d8b8b86U, 0x0f8a8a85U,
    0xe0707090U, 0x7c3e3e42U, 0x71b5b5c4U, 0xcc6666aaU,
    0x904848d8U, 0x06030305U, 0xf7f6f601U, 0x1c0e0e12U,
    0xc26161a3U, 0x6a35355fU, 0xae5757f9U, 0x69b9b9d0U,
    0x17868691U, 0x99c1c158U, 0x3a1d1d27U, 0x279e9eb9U,
    0xd9e1e138U, 0xebf8f813U, 0x2b9898b3U, 0x22111133U,
    0xd26969bbU, 0xa9d9d970U, 0x078e8e89U, 0x339494a7U,
    0x2d9b9bb6U, 0x3c1e1e22U, 0x15878792U, 0xc9e9e920U,
    0x87cece49U, 0xaa5555ffU, 0x50282878U, 0xa5dfdf7aU,
    0x038c8c8fU, 0x59a1a1f8U, 0x09898980U, 0x1a0d0d17U,
    0x65bfbfdaU, 0xd7e6e631U, 0x844242c6U, 0xd06868b8U,
    0x824141c3U, 0x299999b0U, 0x5a2d2d77U, 0x1e0f0f11U,
    0x7bb0b0cbU, 0xa85454fcU, 0x6dbbbbd6U, 0x2c16163aU,
},
{
    0xa5c66363U, 0x84f87c7cU, 0x99ee7777U, 0x8df67b7bU,
    0x0dfff2f2U, 0xbdd66b6bU, 0xb1de6f6fU, 0x5491c5c5U,
    0x50603030U, 0x03020101U, 0xa9ce6767U, 0x7d562b2bU,
    0x19e7fefeU, 0x62b5d7d7U, 0xe64dababU, 0x9aec7676U,
    0x458fcacaU, 0x9d1f8282U, 0x4089c9c9U, 0x87fa7d7dU,
    0x15effafaU, 0xebb25959U, 0xc98e4747U, 0x0bfbf0f0U,
    0xec41adadU, 0x67b3d4d4U, 0xfd5fa2a2U, 0xea45afafU,
    0xbf239c9cU, 0xf753a4a4U, 0x96e47272U, 0x5b9bc0c0U,
    0xc275b7b7U, 0x1ce1fdfdU, 0xae3d9393U, 0x6a4c2626U,
    0x5a6c3636U, 0x417e3f3fU, 0x02f5f7f7U, 0x4f83ccccU,
    0x5c683434U, 0xf451a5a5U, 0x34d1e5e5U, 0x08f9f1f1U,
    0x93e27171U, 0x73abd8d8U, 0x53623131U, 0x3f2a1515U,
    0x0c080404U, 0x5295c7c7U, 0x65462323U, 0x5e9dc3c3U,
    0x28301818U, 0xa1379696U, 0x0f0a0505U, 0xb52f9a9aU,
    0x090e0707U, 0x36241212U, 0x9b1b8080U, 0x3ddfe2e2U,
    0x26cdebebU, 0x694e2727U, 0xcd7fb2b2U, 0x9fea7575U,
    0x1b120909U, 0x9e1d8383U, 0x74582c2cU, 0x2e341a1aU,
    0x2d361b1bU, 0xb2dc6e6eU, 0xeeb45a5aU, 0xfb5ba0a0U,
    0xf6a45252U, 0x4d763b3bU, 0x61b7d6d6U, 0xce7db3b3U,
    0x7b522929U, 0x3edde3e3U, 0x715e2f2fU, 0x97138484U,
    0xf5a65353U, 0x68b9d1d1U, 0x00000000U, 0x2cc1ededU,
    0x60402020U, 0x1fe3fcfcU, 0xc879b1b1U, 0xedb65b5bU,
    0xbed46a6aU, 0x468dcbcbU, 0xd967bebeU, 0x4b723939U,
    0xde944a4aU, 0xd4984c4cU, 0xe8b05858U, 0x4a85cfcfU,
    0x6bbbd0d0U, 0x2ac5efefU, 0xe54faaaaU, 0x16edfbfbU,
    0xc5864343U, 0xd79a4d4dU, 0x55663333U, 0x94118585U,
    0xcf8a4545U, 0x10e9f9f9U, 0x06040202U, 0x81fe7f7fU,
    0xf0a05050U, 0x44783c3cU, 0xba259f9fU, 0xe34ba8a8U,
    0xf3a25151U, 0xfe5da3a3U, 0xc0804040U, 0x8a058f8fU,
    0xad3f9292U, 0xbc219d9dU, 0x48703838U, 0x04f1f5f5U,
    0xdf63bcbcU, 0xc177b6b6U, 0x75afdadaU, 0x63422121U,
    0x30201010U, 0x1ae5ffffU, 0x0efdf3f3U, 0x6dbfd2d2U,
    0x4c81cdcdU, 0x14180c0cU, 0x35261313U, 0x2fc3ececU,
    0xe1be5f5fU, 0xa2359797U, 0xcc884444U, 0x392e1717U,
    0x5793c4c4U, 0xf255a7a7U, 0x82fc7e7eU, 0x477a3d3dU,
    0xacc86464U, 0xe7ba5d5dU, 0x2b321919U, 0x95e67373U,
    0xa0c06060U, 0x98198181U, 0xd19e4f4fU, 0x7fa3dcdcU,
    0x66442222U, 0x7e542a2aU, 0xab3b9090U, 0x830b8888U,
    0xca8c4646U, 0x29c7eeeeU, 0xd36bb8b8U, 0x3c281414U,
    0x79a7dedeU, 0xe2bc5e5eU, 0x1d160b0bU, 0x76addbdbU,
    0x3bdbe0e0U, 0x56643232U, 0x4e743a3aU, 0x1e140a0aU,
    0xdb924949U, 0x0a0c0606U, 0x6c482424U, 0xe4b85c5cU,
    0x5d9fc2c2U, 0x6ebdd3d3U, 0xef43acacU, 0xa6c46262U,
    0xa8399191U, 0xa4319595U, 0x37d3e4e4U, 0x8bf27979U,
    0x32d5e7e7U, 0x438bc8c8U, 0x596e3737U, 0xb7da6d6dU,
    0x8c018d8dU, 0x64b1d5d5U, 0xd29c4e4eU, 0xe049a9a9U,
    0xb4d86c6cU, 0xfaac5656U, 0x07f3f4f4U, 0x25cfeaeaU,
    0xafca6565U, 0x8ef47a7aU, 0xe947aeaeU, 0x18100808U,
    0xd56fbabaU, 0x88f07878U, 0x6f4a2525U, 0x725c2e2eU,
    0x24381c1cU, 0xf157a6a6U, 0xc773b4b4U, 0x5197c6c6U,
    0x23cbe8e8U, 0x7ca1ddddU, 0x9ce87474U, 0x213e1f1fU,
    0xdd964b4bU, 0xdc61bdbdU, 0x860d8b8bU, 0x850f8a8aU,
    0x90e07070U, 0x427c3e3eU, 0xc471b5b5U, 0xaacc6666U,
    0xd8904848U, 0x05060303U, 0x01f7f6f6U, 0x121c0e0eU,
    0xa3c26161U, 0x5f6a3535U, 0xf9ae5757U, 0xd069b9b9U,
    0x91178686U, 0x5899c1c1U, 0x273a1d1dU, 0xb9279e9eU,
    0x38d9e1e1U, 0x13ebf8f8U, 0xb32b9898U, 0x33221111U,
    0xbbd26969U, 0x70a9d9d9U, 0x89078e8eU, 0xa7339494U,
    0xb62d9b9bU, 0x223c1e1eU, 0x92158787U, 0x20c9e9e9U,
    0x4987ceceU, 0xffaa5555U, 0x78502828U, 0x7aa5dfdfU,
    0x8f038c8cU, 0xf859a1a1U, 0x80098989U, 0x171a0d0dU,
    0xda65bfbfU, 0x31d7e6e6U, 0xc6844242U, 0xb8d06868U,
    0xc3824141U, 0xb0299999U, 0x775a2d2dU, 0x111e0f0fU,
    0xcb7bb0b0U, 0xfca85454U, 0xd66dbbbbU, 0x3a2c1616U,
},
{
    0x63a5c663U, 0x7c84f87cU, 0x7799ee77U, 0x7b8df67bU,
    0xf20dfff2U, 0x6bbdd66bU, 0x6fb1de6fU, 0xc55491c5U,
    0x30506030U, 0x01030201U, 0x67a9ce67U, 0x2b7d562bU,
    0xfe19e7feU, 0xd762b5d7U, 0xabe64dabU, 0x769aec76U,
    0xca458fcaU, 0x829d1f82U, 0xc94089c9U, 0x7d87fa7dU,
    0xfa15effaU, 0x59ebb259U, 0x47c98e47U, 0xf00bfbf0U,
    0xadec41adU, 0xd467b3d4U, 0xa2fd5fa2U, 0xafea45afU,
    0x9cbf239cU, 0xa4f753a4U, 0x7296e472U, 0xc05b9bc0U,
    0xb7c275b7U, 0xfd1ce1fdU, 0x93ae3d93U, 0x266a4c26U,
    0x365a6c36U, 0x3f417e3fU, 0xf702f5f7U, 0xcc4f83ccU,
    0x345c6834U, 0xa5f451a5U, 0xe534d1e5U, 0xf108f9f1U,
    0x7193e271U, 0xd873abd8U, 0x31536231U, 0x153f2a15U,
    0x040c0804U, 0xc75295c7U, 0x23654623U, 0xc35e9dc3U,
    0x18283018U, 0x96a13796U, 0x050f0a05U, 0x9ab52f9aU,
    0x07090e07U, 0x12362412U, 0x809b1b80U, 0xe23ddfe2U,
    0xeb26cdebU, 0x27694e27U, 0xb2cd7fb2U, 0x759fea75U,
    0x091b1209U, 0x839e1d83U, 0x2c74582cU, 0x1a2e341aU,
    0x1b2d361bU, 0x6eb2dc6eU, 0x5aeeb45aU, 0xa0fb5ba0U,
    0x52f6a452U, 0x3b4d763bU, 0xd661b7d6U, 0xb3ce7db3U,
    0x297b5229U, 0xe33edde3U, 0x2f715e2fU, 0x84971384U,
    0x53f5a653U, 0xd168b9d1U, 0x00000000U, 0xed2cc1edU,
    0x20604020U, 0xfc1fe3fcU, 0xb1c879b1U, 0x5bedb65bU,
    0x6abed46aU, 0xcb468dcbU, 0xbed967beU, 0x394b7239U,
    0x4ade944aU, 0x4cd4984cU, 0x58e8b058U, 0xcf4a85cfU,
    0xd06bbbd0U, 0xef2ac5efU, 0xaae54faaU, 0xfb16edfbU,
    0x43c58643U, 0x4dd79a4dU, 0x33556633U, 0x85941185U,
    0x45cf8a45U, 0xf910e9f9U, 0x02060402U, 0x7f81fe7fU,
    0x50f0a050U, 0x3c44783cU, 0x9fba259fU, 0xa8e34ba8U,
    0x51f3a251U, 0xa3fe5da3U, 0x40c08040U, 0x8f8a058fU,
    0x92ad3f92U, 0x9dbc219dU, 0x38487038U, 0xf504f1f5U,
    0xbcdf63bcU, 0xb6c177b6U, 0xda75afdaU, 0x21634221U,
    0x10302010U, 0xff1ae5ffU, 0xf30efdf3U, 0xd26dbfd2U,
    0xcd4c81cdU, 0x0c14180cU, 0x13352613U, 0xec2fc3ecU,
    0x5fe1be5fU, 0x97a23597U, 0x44cc8844U, 0x17392e17U,
    0xc45793c4U, 0xa7f255a7U, 0x7e82fc7eU, 0x3d477a3dU,
    0x64acc864U, 0x5de7ba5dU, 0x192b3219U, 0x7395e673U,
    0x60a0c060U, 0x81981981U, 0x4fd19e4fU, 0xdc7fa3dcU,
    0x22664422U, 0x2a7e542aU, 0x90ab3b90U, 0x88830b88U,
    0x46ca8c46U, 0xee29c7eeU, 0xb8d36bb8U, 0x143c2814U,
    0xde79a7deU, 0x5ee2bc5eU, 0x0b1d160bU, 0xdb76addbU,
    0xe03bdbe0U, 0x32566432U, 0x3a4e743aU, 0x0a1e140aU,
    0x49db9249U, 0x060a0c06U, 0x246c4824U, 0x5ce4b85cU,
    0xc25d9fc2U, 0xd36ebdd3U, 0xacef43acU, 0x62a6c462U,
    0x91a83991U, 0x95a43195U, 0xe437d3e4U, 0x798bf279U,
    0xe732d5e7U, 0xc8438bc8U, 0x37596e37U, 0x6db7da6dU,
    0x8d8c018dU, 0xd564b1d5U, 0x4ed29c4eU, 0xa9e049a9U,
    0x6cb4d86cU, 0x56faac56U, 0xf407f3f4U, 0xea25cfeaU,
    0x65afca65U, 0x7a8ef47aU, 0xaee947aeU, 0x08181008U,
    0xbad56fbaU, 0x7888f078U, 0x256f4a25U, 0x2e725c2eU,
    0x1c24381cU, 0xa6f157a6U, 0xb4c773b4U, 0xc65197c6U,
    0xe823cbe8U, 0xdd7ca1ddU, 0x749ce874U, 0x1f213e1fU,
    0x4bdd964bU, 0xbddc61bdU, 0x8b860d8bU, 0x8a850f8aU,
    0x7090e070U, 0x3e427c3eU, 0xb5c471b5U, 0x66aacc66U,
    0x48d89048U, 0x03050603U, 0xf601f7f6U, 0x0e121c0eU,
    0x61a3c261U, 0x355f6a35U, 0x57f9ae57U, 0xb9d069b9U,
    0x86911786U, 0xc15899c1U, 0x1d273a1dU, 0x9eb9279eU,
    0xe138d9e1U, 0xf813ebf8U, 0x98b32b98U, 0x11332211U,
    0x69bbd269U, 0xd970a9d9U, 0x8e89078eU, 0x94a73394U,
    0x9bb62d9bU, 0x1e223c1eU, 0x87921587U, 0xe920c9e9U,
    0xce4987ceU, 0x55ffaa55U, 0x28785028U, 0xdf7aa5dfU,
    0x8c8f038cU, 0xa1f859a1U, 0x89800989U, 0x0d171a0dU,
    0xbfda65bfU, 0xe631d7e6U, 0x42c68442U, 0x68b8d068U,
    0x41c38241U, 0x99b02999U, 0x2d775a2dU, 0x0f111e0fU,
    0xb0cb7bb0U, 0x54fca854U, 0xbbd66dbbU, 0x163a2c16U,
},
{
    0x6363a5c6U, 0x7c7c84f8U, 0x777799eeU, 0x7b7b8df6U,
    0xf2f20dffU, 0x6b6bbdd6U, 0x6f6fb1deU, 0xc5c55491U,
    0x30305060U, 0x01010302U, 0x6767a9ceU, 0x2b2b7d56U,
    0xfefe19e7U, 0xd7d762b5U, 0xababe64dU, 0x76769aecU,
    0xcaca458fU, 0x82829d1fU, 0xc9c94089U, 0x7d7d87faU,
    0xfafa15efU, 0x5959ebb2U, 0x4747c98eU, 0xf0f00bfbU,
    0xadadec41U, 0xd4d467b3U, 0xa2a2fd5fU, 0xafafea45U,
    0x9c9cbf23U, 0xa4a4f753U, 0x727296e4U, 0xc0c05b9bU,
    0xb7b7c275U, 0xfdfd1ce1U, 0x9393ae3dU, 0x26266a4cU,
    0x36365a6cU, 0x3f3f417eU, 0xf7f702f5U, 0xcccc4f83U,
    0x34345c68U, 0xa5a5f451U, 0xe5e534d1U, 0xf1f108f9U,
    0x717193e2U, 0xd8d873abU, 0x31315362U, 0x15153f2aU,
    0x04040c08U, 0xc7c75295U, 0x23236546U, 0xc3c35e9dU,
    0x18182830U, 0x9696a137U, 0x05050f0aU, 0x9a9ab52fU,
    0x0707090eU, 0x12123624U, 0x80809b1bU, 0xe2e23ddfU,
    0xebeb26cdU, 0x2727694eU, 0xb2b2cd7fU, 0x75759feaU,
    0x09091b12U, 0x83839e1dU, 0x2c2c7458U, 0x1a1a2e34U,
    0x1b1b2d36U, 0x6e6eb2dcU, 0x5a5aeeb4U, 0xa0a0fb5bU,
    0x5252f6a4U, 0x3b3b4d76U, 0xd6d661b7U, 0xb3b3ce7dU,
    0x29297b52U, 0xe3e33eddU, 0x2f2f715eU, 0x84849713U,
    0x5353f5a6U, 0xd1d168b9U, 0x00000000U, 0xeded2cc1U,
    0x20206040U, 0xfcfc1fe3U, 0xb1b1c879U, 0x5b5bedb6U,
    0x6a6abed4U, 0xcbcb468dU, 0xbebed967U, 0x39394b72U,
    0x4a4ade94U, 0x4c4cd498U, 0x5858e8b0U, 0xcfcf4a85U,
    0xd0d06bbbU, 0xefef2ac5U, 0xaaaae54fU, 0xfbfb16edU,
    0x4343c586U, 0x4d4dd79aU, 0x33335566U, 0x85859411U,
    0x4545cf8aU, 0xf9f910e9U, 0x02020604U, 0x7f7f81feU,
    0x5050f0a0U, 0x3c3c4478U, 0x9f9fba25U, 0xa8a8e34bU,
    0x5151f3a2U, 0xa3a3fe5dU, 0x4040c080U, 0x8f8f8a05U,
    0x9292ad3fU, 0x9d9dbc21U, 0x38384870U, 0xf5f504f1U,
    0xbcbcdf63U, 0xb6b6c177U, 0xdada75afU, 0x21216342U,
    0x10103020U, 0xffff1ae5U, 0xf3f30efdU, 0xd2d26dbfU,
    0xcdcd4c81U, 0x0c0c1418U, 0x13133526U, 0xecec2fc3U,
    0x5f5fe1beU, 0x9797a235U, 0x4444cc88U, 0x1717392eU,
    0xc4c45793U, 0xa7a7f255U, 0x7e7e82fcU, 0x3d3d477aU,
    0x6464acc8U, 0x5d5de7baU, 0x19192b32U, 0x737395e6U,
    0x6060a0c0U, 0x81819819U, 0x4f4fd19eU, 0xdcdc7fa3U,
    0x22226644U, 0x2a2a7e54U, 0x9090ab3bU, 0x8888830bU,
    0x4646ca8cU, 0xeeee29c7U, 0xb8b8d36bU, 0x14143c28U,
    0xdede79a7U, 0x5e5ee2bcU, 0x0b0b1d16U, 0xdbdb76adU,
    0xe0e03bdbU, 0x32325664U, 0x3a3a4e74U, 0x0a0a1e14U,
    0x4949db92U, 0x06060a0cU, 0x24246c48U, 0x5c5ce4b8U,
    0xc2c25d9fU, 0xd3d36ebdU, 0xacacef43U, 0x6262a6c4U,
    0x9191a839U, 0x9595a431U, 0xe4e437d3U, 0x79798bf2U,
    0xe7e732d5U, 0xc8c8438bU, 0x3737596eU, 0x6d6db7daU,
    0x8d8d8c01U, 0xd5d564b1U, 0x4e4ed29cU, 0xa9a9e049U,
    0x6c6cb4d8U, 0x5656faacU, 0xf4f407f3U, 0xeaea25cfU,
    0x6565afcaU, 0x7a7a8ef4U, 0xaeaee947U, 0x08081810U,
    0xbabad56fU, 0x787888f0U, 0x25256f4aU, 0x2e2e725cU,
    0x1c1c2438U, 0xa6a6f157U, 0xb4b4c773U, 0xc6c65197U,
    0xe8e823cbU, 0xdddd7ca1U, 0x74749ce8U, 0x1f1f213eU,
    0x4b4bdd96U, 0xbdbddc61U, 0x8b8b860dU, 0x8a8a850fU,
    0x707090e0U, 0x3e3e427cU, 0xb5b5c471U, 0x6666aaccU,
    0x4848d890U, 0x03030506U, 0xf6f601f7U, 0x0e0e121cU,
    0x6161a3c2U, 0x35355f6aU, 0x5757f9aeU, 0xb9b9d069U,
    0x86869117U, 0xc1c15899U, 0x1d1d273aU, 0x9e9eb927U,
    0xe1e138d9U, 0xf8f813ebU, 0x9898b32bU, 0x11113322U,
    0x6969bbd2U, 0xd9d970a9U, 0x8e8e8907U, 0x9494a733U,
    0x9b9bb62dU, 0x1e1e223cU, 0x87879215U, 0xe9e920c9U,
    0xcece4987U, 0x5555ffaaU, 0x28287850U, 0xdfdf7aa5U,
    0x8c8c8f03U, 0xa1a1f859U, 0x89898009U, 0x0d0d171aU,
    0xbfbfda65U, 0xe6e631d7U, 0x4242c684U, 0x6868b8d0U,
    0x4141c382U, 0x9999b029U, 0x2d2d775aU, 0x0f0f111eU,
    0xb0b0cb7bU, 0x5454fca8U, 0xbbbbd66dU, 0x16163a2cU,
}
};

#if defined(HAVE_AES_DECRYPT) && !defined(WOLFSSL_SILABS_SE_ACCEL)
static const FLASH_QUALIFIER word32 Td[4][256] = {
{
    0x51f4a750U, 0x7e416553U, 0x1a17a4c3U, 0x3a275e96U,
    0x3bab6bcbU, 0x1f9d45f1U, 0xacfa58abU, 0x4be30393U,
    0x2030fa55U, 0xad766df6U, 0x88cc7691U, 0xf5024c25U,
    0x4fe5d7fcU, 0xc52acbd7U, 0x26354480U, 0xb562a38fU,
    0xdeb15a49U, 0x25ba1b67U, 0x45ea0e98U, 0x5dfec0e1U,
    0xc32f7502U, 0x814cf012U, 0x8d4697a3U, 0x6bd3f9c6U,
    0x038f5fe7U, 0x15929c95U, 0xbf6d7aebU, 0x955259daU,
    0xd4be832dU, 0x587421d3U, 0x49e06929U, 0x8ec9c844U,
    0x75c2896aU, 0xf48e7978U, 0x99583e6bU, 0x27b971ddU,
    0xbee14fb6U, 0xf088ad17U, 0xc920ac66U, 0x7dce3ab4U,
    0x63df4a18U, 0xe51a3182U, 0x97513360U, 0x62537f45U,
    0xb16477e0U, 0xbb6bae84U, 0xfe81a01cU, 0xf9082b94U,
    0x70486858U, 0x8f45fd19U, 0x94de6c87U, 0x527bf8b7U,
    0xab73d323U, 0x724b02e2U, 0xe31f8f57U, 0x6655ab2aU,
    0xb2eb2807U, 0x2fb5c203U, 0x86c57b9aU, 0xd33708a5U,
    0x302887f2U, 0x23bfa5b2U, 0x02036abaU, 0xed16825cU,
    0x8acf1c2bU, 0xa779b492U, 0xf307f2f0U, 0x4e69e2a1U,
    0x65daf4cdU, 0x0605bed5U, 0xd134621fU, 0xc4a6fe8aU,
    0x342e539dU, 0xa2f355a0U, 0x058ae132U, 0xa4f6eb75U,
    0x0b83ec39U, 0x4060efaaU, 0x5e719f06U, 0xbd6e1051U,
    0x3e218af9U, 0x96dd063dU, 0xdd3e05aeU, 0x4de6bd46U,
    0x91548db5U, 0x71c45d05U, 0x0406d46fU, 0x605015ffU,
    0x1998fb24U, 0xd6bde997U, 0x894043ccU, 0x67d99e77U,
    0xb0e842bdU, 0x07898b88U, 0xe7195b38U, 0x79c8eedbU,
    0xa17c0a47U, 0x7c420fe9U, 0xf8841ec9U, 0x00000000U,
    0x09808683U, 0x322bed48U, 0x1e1170acU, 0x6c5a724eU,
    0xfd0efffbU, 0x0f853856U, 0x3daed51eU, 0x362d3927U,
    0x0a0fd964U, 0x685ca621U, 0x9b5b54d1U, 0x24362e3aU,
    0x0c0a67b1U, 0x9357e70fU, 0xb4ee96d2U, 0x1b9b919eU,
    0x80c0c54fU, 0x61dc20a2U, 0x5a774b69U, 0x1c121a16U,
    0xe293ba0aU, 0xc0a02ae5U, 0x3c22e043U, 0x121b171dU,
    0x0e090d0bU, 0xf28bc7adU, 0x2db6a8b9U, 0x141ea9c8U,
    0x57f11985U, 0xaf75074cU, 0xee99ddbbU, 0xa37f60fdU,
    0xf701269fU, 0x5c72f5bcU, 0x44663bc5U, 0x5bfb7e34U,
    0x8b432976U, 0xcb23c6dcU, 0xb6edfc68U, 0xb8e4f163U,
    0xd731dccaU, 0x42638510U, 0x13972240U, 0x84c61120U,
    0x854a247dU, 0xd2bb3df8U, 0xaef93211U, 0xc729a16dU,
    0x1d9e2f4bU, 0xdcb230f3U, 0x0d8652ecU, 0x77c1e3d0U,
    0x2bb3166cU, 0xa970b999U, 0x119448faU, 0x47e96422U,
    0xa8fc8cc4U, 0xa0f03f1aU, 0x567d2cd8U, 0x223390efU,
    0x87494ec7U, 0xd938d1c1U, 0x8ccaa2feU, 0x98d40b36U,
    0xa6f581cfU, 0xa57ade28U, 0xdab78e26U, 0x3fadbfa4U,
    0x2c3a9de4U, 0x5078920dU, 0x6a5fcc9bU, 0x547e4662U,
    0xf68d13c2U, 0x90d8b8e8U, 0x2e39f75eU, 0x82c3aff5U,
    0x9f5d80beU, 0x69d0937cU, 0x6fd52da9U, 0xcf2512b3U,
    0xc8ac993bU, 0x10187da7U, 0xe89c636eU, 0xdb3bbb7bU,
    0xcd267809U, 0x6e5918f4U, 0xec9ab701U, 0x834f9aa8U,
    0xe6956e65U, 0xaaffe67eU, 0x21bccf08U, 0xef15e8e6U,
    0xbae79bd9U, 0x4a6f36ceU, 0xea9f09d4U, 0x29b07cd6U,
    0x31a4b2afU, 0x2a3f2331U, 0xc6a59430U, 0x35a266c0U,
    0x744ebc37U, 0xfc82caa6U, 0xe090d0b0U, 0x33a7d815U,
    0xf104984aU, 0x41ecdaf7U, 0x7fcd500eU, 0x1791f62fU,
    0x764dd68dU, 0x43efb04dU, 0xccaa4d54U, 0xe49604dfU,
    0x9ed1b5e3U, 0x4c6a881bU, 0xc12c1fb8U, 0x4665517fU,
    0x9d5eea04U, 0x018c355dU, 0xfa877473U, 0xfb0b412eU,
    0xb3671d5aU, 0x92dbd252U, 0xe9105633U, 0x6dd64713U,
    0x9ad7618cU, 0x37a10c7aU, 0x59f8148eU, 0xeb133c89U,
    0xcea927eeU, 0xb761c935U, 0xe11ce5edU, 0x7a47b13cU,
    0x9cd2df59U, 0x55f2733fU, 0x1814ce79U, 0x73c737bfU,
    0x53f7cdeaU, 0x5ffdaa5bU, 0xdf3d6f14U, 0x7844db86U,
    0xcaaff381U, 0xb968c43eU, 0x3824342cU, 0xc2a3405fU,
    0x161dc372U, 0xbce2250cU, 0x283c498bU, 0xff0d9541U,
    0x39a80171U, 0x080cb3deU, 0xd8b4e49cU, 0x6456c190U,
    0x7bcb8461U, 0xd532b670U, 0x486c5c74U, 0xd0b85742U,
},
{
    0x5051f4a7U, 0x537e4165U, 0xc31a17a4U, 0x963a275eU,
    0xcb3bab6bU, 0xf11f9d45U, 0xabacfa58U, 0x934be303U,
    0x552030faU, 0xf6ad766dU, 0x9188cc76U, 0x25f5024cU,
    0xfc4fe5d7U, 0xd7c52acbU, 0x80263544U, 0x8fb562a3U,
    0x49deb15aU, 0x6725ba1bU, 0x9845ea0eU, 0xe15dfec0U,
    0x02c32f75U, 0x12814cf0U, 0xa38d4697U, 0xc66bd3f9U,
    0xe7038f5fU, 0x9515929cU, 0xebbf6d7aU, 0xda955259U,
    0x2dd4be83U, 0xd3587421U, 0x2949e069U, 0x448ec9c8U,
    0x6a75c289U, 0x78f48e79U, 0x6b99583eU, 0xdd27b971U,
    0xb6bee14fU, 0x17f088adU, 0x66c920acU, 0xb47dce3aU,
    0x1863df4aU, 0x82e51a31U, 0x60975133U, 0x4562537fU,
    0xe0b16477U, 0x84bb6baeU, 0x1cfe81a0U, 0x94f9082bU,
    0x58704868U, 0x198f45fdU, 0x8794de6cU, 0xb7527bf8U,
    0x23ab73d3U, 0xe2724b02U, 0x57e31f8fU, 0x2a6655abU,
    0x07b2eb28U, 0x032fb5c2U, 0x9a86c57bU, 0xa5d33708U,
    0xf2302887U, 0xb223bfa5U, 0xba02036aU, 0x5ced1682U,
    0x2b8acf1cU, 0x92a779b4U, 0xf0f307f2U, 0xa14e69e2U,
    0xcd65daf4U, 0xd50605beU, 0x1fd13462U, 0x8ac4a6feU,
    0x9d342e53U, 0xa0a2f355U, 0x32058ae1U, 0x75a4f6ebU,
    0x390b83ecU, 0xaa4060efU, 0x065e719fU, 0x51bd6e10U,
    0xf93e218aU, 0x3d96dd06U, 0xaedd3e05U, 0x464de6bdU,
    0xb591548dU, 0x0571c45dU, 0x6f0406d4U, 0xff605015U,
    0x241998fbU, 0x97d6bde9U, 0xcc894043U, 0x7767d99eU,
    0xbdb0e842U, 0x8807898bU, 0x38e7195bU, 0xdb79c8eeU,
    0x47a17c0aU, 0xe97c420fU, 0xc9f8841eU, 0x00000000U,
    0x83098086U, 0x48322bedU, 0xac1e1170U, 0x4e6c5a72U,
    0xfbfd0effU, 0x560f8538U, 0x1e3daed5U, 0x27362d39U,
    0x640a0fd9U, 0x21685ca6U, 0xd19b5b54U, 0x3a24362eU,
    0xb10c0a67U, 0x0f9357e7U, 0xd2b4ee96U, 0x9e1b9b91U,
    0x4f80c0c5U, 0xa261dc20U, 0x695a774bU, 0x161c121aU,
    0x0ae293baU, 0xe5c0a02aU, 0x433c22e0U, 0x1d121b17U,
    0x0b0e090dU, 0xadf28bc7U, 0xb92db6a8U, 0xc8141ea9U,
    0x8557f119U, 0x4caf7507U, 0xbbee99ddU, 0xfda37f60U,
    0x9ff70126U, 0xbc5c72f5U, 0xc544663bU, 0x345bfb7eU,
    0x768b4329U, 0xdccb23c6U, 0x68b6edfcU, 0x63b8e4f1U,
    0xcad731dcU, 0x10426385U, 0x40139722U, 0x2084c611U,
    0x7d854a24U, 0xf8d2bb3dU, 0x11aef932U, 0x6dc729a1U,
    0x4b1d9e2fU, 0xf3dcb230U, 0xec0d8652U, 0xd077c1e3U,
    0x6c2bb316U, 0x99a970b9U, 0xfa119448U, 0x2247e964U,
    0xc4a8fc8cU, 0x1aa0f03fU, 0xd8567d2cU, 0xef223390U,
    0xc787494eU, 0xc1d938d1U, 0xfe8ccaa2U, 0x3698d40bU,
    0xcfa6f581U, 0x28a57adeU, 0x26dab78eU, 0xa43fadbfU,
    0xe42c3a9dU, 0x0d507892U, 0x9b6a5fccU, 0x62547e46U,
    0xc2f68d13U, 0xe890d8b8U, 0x5e2e39f7U, 0xf582c3afU,
    0xbe9f5d80U, 0x7c69d093U, 0xa96fd52dU, 0xb3cf2512U,
    0x3bc8ac99U, 0xa710187dU, 0x6ee89c63U, 0x7bdb3bbbU,
    0x09cd2678U, 0xf46e5918U, 0x01ec9ab7U, 0xa8834f9aU,
    0x65e6956eU, 0x7eaaffe6U, 0x0821bccfU, 0xe6ef15e8U,
    0xd9bae79bU, 0xce4a6f36U, 0xd4ea9f09U, 0xd629b07cU,
    0xaf31a4b2U, 0x312a3f23U, 0x30c6a594U, 0xc035a266U,
    0x37744ebcU, 0xa6fc82caU, 0xb0e090d0U, 0x1533a7d8U,
    0x4af10498U, 0xf741ecdaU, 0x0e7fcd50U, 0x2f1791f6U,
    0x8d764dd6U, 0x4d43efb0U, 0x54ccaa4dU, 0xdfe49604U,
    0xe39ed1b5U, 0x1b4c6a88U, 0xb8c12c1fU, 0x7f466551U,
    0x049d5eeaU, 0x5d018c35U, 0x73fa8774U, 0x2efb0b41U,
    0x5ab3671dU, 0x5292dbd2U, 0x33e91056U, 0x136dd647U,
    0x8c9ad761U, 0x7a37a10cU, 0x8e59f814U, 0x89eb133cU,
    0xeecea927U, 0x35b761c9U, 0xede11ce5U, 0x3c7a47b1U,
    0x599cd2dfU, 0x3f55f273U, 0x791814ceU, 0xbf73c737U,
    0xea53f7cdU, 0x5b5ffdaaU, 0x14df3d6fU, 0x867844dbU,
    0x81caaff3U, 0x3eb968c4U, 0x2c382434U, 0x5fc2a340U,
    0x72161dc3U, 0x0cbce225U, 0x8b283c49U, 0x41ff0d95U,
    0x7139a801U, 0xde080cb3U, 0x9cd8b4e4U, 0x906456c1U,
    0x617bcb84U, 0x70d532b6U, 0x74486c5cU, 0x42d0b857U,
},
{
    0xa75051f4U, 0x65537e41U, 0xa4c31a17U, 0x5e963a27U,
    0x6bcb3babU, 0x45f11f9dU, 0x58abacfaU, 0x03934be3U,
    0xfa552030U, 0x6df6ad76U, 0x769188ccU, 0x4c25f502U,
    0xd7fc4fe5U, 0xcbd7c52aU, 0x44802635U, 0xa38fb562U,
    0x5a49deb1U, 0x1b6725baU, 0x0e9845eaU, 0xc0e15dfeU,
    0x7502c32fU, 0xf012814cU, 0x97a38d46U, 0xf9c66bd3U,
    0x5fe7038fU, 0x9c951592U, 0x7aebbf6dU, 0x59da9552U,
    0x832dd4beU, 0x21d35874U, 0x692949e0U, 0xc8448ec9U,
    0x896a75c2U, 0x7978f48eU, 0x3e6b9958U, 0x71dd27b9U,
    0x4fb6bee1U, 0xad17f088U, 0xac66c920U, 0x3ab47dceU,
    0x4a1863dfU, 0x3182e51aU, 0x33609751U, 0x7f456253U,
    0x77e0b164U, 0xae84bb6bU, 0xa01cfe81U, 0x2b94f908U,
    0x68587048U, 0xfd198f45U, 0x6c8794deU, 0xf8b7527bU,
    0xd323ab73U, 0x02e2724bU, 0x8f57e31fU, 0xab2a6655U,
    0x2807b2ebU, 0xc2032fb5U, 0x7b9a86c5U, 0x08a5d337U,
    0x87f23028U, 0xa5b223bfU, 0x6aba0203U, 0x825ced16U,
    0x1c2b8acfU, 0xb492a779U, 0xf2f0f307U, 0xe2a14e69U,
    0xf4cd65daU, 0xbed50605U, 0x621fd134U, 0xfe8ac4a6U,
    0x539d342eU, 0x55a0a2f3U, 0xe132058aU, 0xeb75a4f6U,
    0xec390b83U, 0xefaa4060U, 0x9f065e71U, 0x1051bd6eU,

    0x8af93e21U, 0x063d96ddU, 0x05aedd3eU, 0xbd464de6U,
    0x8db59154U, 0x5d0571c4U, 0xd46f0406U, 0x15ff6050U,
    0xfb241998U, 0xe997d6bdU, 0x43cc8940U, 0x9e7767d9U,
    0x42bdb0e8U, 0x8b880789U, 0x5b38e719U, 0xeedb79c8U,
    0x0a47a17cU, 0x0fe97c42U, 0x1ec9f884U, 0x00000000U,
    0x86830980U, 0xed48322bU, 0x70ac1e11U, 0x724e6c5aU,
    0xfffbfd0eU, 0x38560f85U, 0xd51e3daeU, 0x3927362dU,
    0xd9640a0fU, 0xa621685cU, 0x54d19b5bU, 0x2e3a2436U,
    0x67b10c0aU, 0xe70f9357U, 0x96d2b4eeU, 0x919e1b9bU,
    0xc54f80c0U, 0x20a261dcU, 0x4b695a77U, 0x1a161c12U,
    0xba0ae293U, 0x2ae5c0a0U, 0xe0433c22U, 0x171d121bU,
    0x0d0b0e09U, 0xc7adf28bU, 0xa8b92db6U, 0xa9c8141eU,
    0x198557f1U, 0x074caf75U, 0xddbbee99U, 0x60fda37fU,
    0x269ff701U, 0xf5bc5c72U, 0x3bc54466U, 0x7e345bfbU,
    0x29768b43U, 0xc6dccb23U, 0xfc68b6edU, 0xf163b8e4U,
    0xdccad731U, 0x85104263U, 0x22401397U, 0x112084c6U,
    0x247d854aU, 0x3df8d2bbU, 0x3211aef9U, 0xa16dc729U,
    0x2f4b1d9eU, 0x30f3dcb2U, 0x52ec0d86U, 0xe3d077c1U,
    0x166c2bb3U, 0xb999a970U, 0x48fa1194U, 0x642247e9U,
    0x8cc4a8fcU, 0x3f1aa0f0U, 0x2cd8567dU, 0x90ef2233U,
    0x4ec78749U, 0xd1c1d938U, 0xa2fe8ccaU, 0x0b3698d4U,
    0x81cfa6f5U, 0xde28a57aU, 0x8e26dab7U, 0xbfa43fadU,
    0x9de42c3aU, 0x920d5078U, 0xcc9b6a5fU, 0x4662547eU,
    0x13c2f68dU, 0xb8e890d8U, 0xf75e2e39U, 0xaff582c3U,
    0x80be9f5dU, 0x937c69d0U, 0x2da96fd5U, 0x12b3cf25U,
    0x993bc8acU, 0x7da71018U, 0x636ee89cU, 0xbb7bdb3bU,
    0x7809cd26U, 0x18f46e59U, 0xb701ec9aU, 0x9aa8834fU,
    0x6e65e695U, 0xe67eaaffU, 0xcf0821bcU, 0xe8e6ef15U,
    0x9bd9bae7U, 0x36ce4a6fU, 0x09d4ea9fU, 0x7cd629b0U,
    0xb2af31a4U, 0x23312a3fU, 0x9430c6a5U, 0x66c035a2U,
    0xbc37744eU, 0xcaa6fc82U, 0xd0b0e090U, 0xd81533a7U,
    0x984af104U, 0xdaf741ecU, 0x500e7fcdU, 0xf62f1791U,
    0xd68d764dU, 0xb04d43efU, 0x4d54ccaaU, 0x04dfe496U,
    0xb5e39ed1U, 0x881b4c6aU, 0x1fb8c12cU, 0x517f4665U,
    0xea049d5eU, 0x355d018cU, 0x7473fa87U, 0x412efb0bU,
    0x1d5ab367U, 0xd25292dbU, 0x5633e910U, 0x47136dd6U,
    0x618c9ad7U, 0x0c7a37a1U, 0x148e59f8U, 0x3c89eb13U,
    0x27eecea9U, 0xc935b761U, 0xe5ede11cU, 0xb13c7a47U,
    0xdf599cd2U, 0x733f55f2U, 0xce791814U, 0x37bf73c7U,
    0xcdea53f7U, 0xaa5b5ffdU, 0x6f14df3dU, 0xdb867844U,
    0xf381caafU, 0xc43eb968U, 0x342c3824U, 0x405fc2a3U,
    0xc372161dU, 0x250cbce2U, 0x498b283cU, 0x9541ff0dU,
    0x017139a8U, 0xb3de080cU, 0xe49cd8b4U, 0xc1906456U,
    0x84617bcbU, 0xb670d532U, 0x5c74486cU, 0x5742d0b8U,
},
{
    0xf4a75051U, 0x4165537eU, 0x17a4c31aU, 0x275e963aU,
    0xab6bcb3bU, 0x9d45f11fU, 0xfa58abacU, 0xe303934bU,
    0x30fa5520U, 0x766df6adU, 0xcc769188U, 0x024c25f5U,
    0xe5d7fc4fU, 0x2acbd7c5U, 0x35448026U, 0x62a38fb5U,
    0xb15a49deU, 0xba1b6725U, 0xea0e9845U, 0xfec0e15dU,
    0x2f7502c3U, 0x4cf01281U, 0x4697a38dU, 0xd3f9c66bU,
    0x8f5fe703U, 0x929c9515U, 0x6d7aebbfU, 0x5259da95U,
    0xbe832dd4U, 0x7421d358U, 0xe0692949U, 0xc9c8448eU,
    0xc2896a75U, 0x8e7978f4U, 0x583e6b99U, 0xb971dd27U,
    0xe14fb6beU, 0x88ad17f0U, 0x20ac66c9U, 0xce3ab47dU,
    0xdf4a1863U, 0x1a3182e5U, 0x51336097U, 0x537f4562U,
    0x6477e0b1U, 0x6bae84bbU, 0x81a01cfeU, 0x082b94f9U,
    0x48685870U, 0x45fd198fU, 0xde6c8794U, 0x7bf8b752U,
    0x73d323abU, 0x4b02e272U, 0x1f8f57e3U, 0x55ab2a66U,
    0xeb2807b2U, 0xb5c2032fU, 0xc57b9a86U, 0x3708a5d3U,
    0x2887f230U, 0xbfa5b223U, 0x036aba02U, 0x16825cedU,
    0xcf1c2b8aU, 0x79b492a7U, 0x07f2f0f3U, 0x69e2a14eU,
    0xdaf4cd65U, 0x05bed506U, 0x34621fd1U, 0xa6fe8ac4U,
    0x2e539d34U, 0xf355a0a2U, 0x8ae13205U, 0xf6eb75a4U,
    0x83ec390bU, 0x60efaa40U, 0x719f065eU, 0x6e1051bdU,
    0x218af93eU, 0xdd063d96U, 0x3e05aeddU, 0xe6bd464dU,
    0x548db591U, 0xc45d0571U, 0x06d46f04U, 0x5015ff60U,
    0x98fb2419U, 0xbde997d6U, 0x4043cc89U, 0xd99e7767U,
    0xe842bdb0U, 0x898b8807U, 0x195b38e7U, 0xc8eedb79U,
    0x7c0a47a1U, 0x420fe97cU, 0x841ec9f8U, 0x00000000U,
    0x80868309U, 0x2bed4832U, 0x1170ac1eU, 0x5a724e6cU,
    0x0efffbfdU, 0x8538560fU, 0xaed51e3dU, 0x2d392736U,
    0x0fd9640aU, 0x5ca62168U, 0x5b54d19bU, 0x362e3a24U,
    0x0a67b10cU, 0x57e70f93U, 0xee96d2b4U, 0x9b919e1bU,
    0xc0c54f80U, 0xdc20a261U, 0x774b695aU, 0x121a161cU,
    0x93ba0ae2U, 0xa02ae5c0U, 0x22e0433cU, 0x1b171d12U,
    0x090d0b0eU, 0x8bc7adf2U, 0xb6a8b92dU, 0x1ea9c814U,
    0xf1198557U, 0x75074cafU, 0x99ddbbeeU, 0x7f60fda3U,
    0x01269ff7U, 0x72f5bc5cU, 0x663bc544U, 0xfb7e345bU,
    0x4329768bU, 0x23c6dccbU, 0xedfc68b6U, 0xe4f163b8U,
    0x31dccad7U, 0x63851042U, 0x97224013U, 0xc6112084U,
    0x4a247d85U, 0xbb3df8d2U, 0xf93211aeU, 0x29a16dc7U,
    0x9e2f4b1dU, 0xb230f3dcU, 0x8652ec0dU, 0xc1e3d077U,
    0xb3166c2bU, 0x70b999a9U, 0x9448fa11U, 0xe9642247U,
    0xfc8cc4a8U, 0xf03f1aa0U, 0x7d2cd856U, 0x3390ef22U,
    0x494ec787U, 0x38d1c1d9U, 0xcaa2fe8cU, 0xd40b3698U,
    0xf581cfa6U, 0x7ade28a5U, 0xb78e26daU, 0xadbfa43fU,
    0x3a9de42cU, 0x78920d50U, 0x5fcc9b6aU, 0x7e466254U,
    0x8d13c2f6U, 0xd8b8e890U, 0x39f75e2eU, 0xc3aff582U,
    0x5d80be9fU, 0xd0937c69U, 0xd52da96fU, 0x2512b3cfU,
    0xac993bc8U, 0x187da710U, 0x9c636ee8U, 0x3bbb7bdbU,
    0x267809cdU, 0x5918f46eU, 0x9ab701ecU, 0x4f9aa883U,
    0x956e65e6U, 0xffe67eaaU, 0xbccf0821U, 0x15e8e6efU,
    0xe79bd9baU, 0x6f36ce4aU, 0x9f09d4eaU, 0xb07cd629U,
    0xa4b2af31U, 0x3f23312aU, 0xa59430c6U, 0xa266c035U,
    0x4ebc3774U, 0x82caa6fcU, 0x90d0b0e0U, 0xa7d81533U,
    0x04984af1U, 0xecdaf741U, 0xcd500e7fU, 0x91f62f17U,
    0x4dd68d76U, 0xefb04d43U, 0xaa4d54ccU, 0x9604dfe4U,
    0xd1b5e39eU, 0x6a881b4cU, 0x2c1fb8c1U, 0x65517f46U,
    0x5eea049dU, 0x8c355d01U, 0x877473faU, 0x0b412efbU,
    0x671d5ab3U, 0xdbd25292U, 0x105633e9U, 0xd647136dU,
    0xd7618c9aU, 0xa10c7a37U, 0xf8148e59U, 0x133c89ebU,
    0xa927eeceU, 0x61c935b7U, 0x1ce5ede1U, 0x47b13c7aU,
    0xd2df599cU, 0xf2733f55U, 0x14ce7918U, 0xc737bf73U,
    0xf7cdea53U, 0xfdaa5b5fU, 0x3d6f14dfU, 0x44db8678U,
    0xaff381caU, 0x68c43eb9U, 0x24342c38U, 0xa3405fc2U,
    0x1dc37216U, 0xe2250cbcU, 0x3c498b28U, 0x0d9541ffU,
    0xa8017139U, 0x0cb3de08U, 0xb4e49cd8U, 0x56c19064U,
    0xcb84617bU, 0x32b670d5U, 0x6c5c7448U, 0xb85742d0U,
}
};
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_SMALL_TABLES */

#ifdef HAVE_AES_DECRYPT
#if (defined(HAVE_AES_CBC) && !defined(WOLFSSL_DEVCRYPTO_CBC) && \
                              !defined(WOLFSSL_SILABS_SE_ACCEL)) || \
    defined(WOLFSSL_AES_DIRECT)
static const FLASH_QUALIFIER byte Td4[256] =
{
    0x52U, 0x09U, 0x6aU, 0xd5U, 0x30U, 0x36U, 0xa5U, 0x38U,
    0xbfU, 0x40U, 0xa3U, 0x9eU, 0x81U, 0xf3U, 0xd7U, 0xfbU,
    0x7cU, 0xe3U, 0x39U, 0x82U, 0x9bU, 0x2fU, 0xffU, 0x87U,
    0x34U, 0x8eU, 0x43U, 0x44U, 0xc4U, 0xdeU, 0xe9U, 0xcbU,
    0x54U, 0x7bU, 0x94U, 0x32U, 0xa6U, 0xc2U, 0x23U, 0x3dU,
    0xeeU, 0x4cU, 0x95U, 0x0bU, 0x42U, 0xfaU, 0xc3U, 0x4eU,
    0x08U, 0x2eU, 0xa1U, 0x66U, 0x28U, 0xd9U, 0x24U, 0xb2U,
    0x76U, 0x5bU, 0xa2U, 0x49U, 0x6dU, 0x8bU, 0xd1U, 0x25U,
    0x72U, 0xf8U, 0xf6U, 0x64U, 0x86U, 0x68U, 0x98U, 0x16U,
    0xd4U, 0xa4U, 0x5cU, 0xccU, 0x5dU, 0x65U, 0xb6U, 0x92U,
    0x6cU, 0x70U, 0x48U, 0x50U, 0xfdU, 0xedU, 0xb9U, 0xdaU,
    0x5eU, 0x15U, 0x46U, 0x57U, 0xa7U, 0x8dU, 0x9dU, 0x84U,
    0x90U, 0xd8U, 0xabU, 0x00U, 0x8cU, 0xbcU, 0xd3U, 0x0aU,
    0xf7U, 0xe4U, 0x58U, 0x05U, 0xb8U, 0xb3U, 0x45U, 0x06U,
    0xd0U, 0x2cU, 0x1eU, 0x8fU, 0xcaU, 0x3fU, 0x0fU, 0x02U,
    0xc1U, 0xafU, 0xbdU, 0x03U, 0x01U, 0x13U, 0x8aU, 0x6bU,
    0x3aU, 0x91U, 0x11U, 0x41U, 0x4fU, 0x67U, 0xdcU, 0xeaU,
    0x97U, 0xf2U, 0xcfU, 0xceU, 0xf0U, 0xb4U, 0xe6U, 0x73U,
    0x96U, 0xacU, 0x74U, 0x22U, 0xe7U, 0xadU, 0x35U, 0x85U,
    0xe2U, 0xf9U, 0x37U, 0xe8U, 0x1cU, 0x75U, 0xdfU, 0x6eU,
    0x47U, 0xf1U, 0x1aU, 0x71U, 0x1dU, 0x29U, 0xc5U, 0x89U,
    0x6fU, 0xb7U, 0x62U, 0x0eU, 0xaaU, 0x18U, 0xbeU, 0x1bU,
    0xfcU, 0x56U, 0x3eU, 0x4bU, 0xc6U, 0xd2U, 0x79U, 0x20U,
    0x9aU, 0xdbU, 0xc0U, 0xfeU, 0x78U, 0xcdU, 0x5aU, 0xf4U,
    0x1fU, 0xddU, 0xa8U, 0x33U, 0x88U, 0x07U, 0xc7U, 0x31U,
    0xb1U, 0x12U, 0x10U, 0x59U, 0x27U, 0x80U, 0xecU, 0x5fU,
    0x60U, 0x51U, 0x7fU, 0xa9U, 0x19U, 0xb5U, 0x4aU, 0x0dU,
    0x2dU, 0xe5U, 0x7aU, 0x9fU, 0x93U, 0xc9U, 0x9cU, 0xefU,
    0xa0U, 0xe0U, 0x3bU, 0x4dU, 0xaeU, 0x2aU, 0xf5U, 0xb0U,
    0xc8U, 0xebU, 0xbbU, 0x3cU, 0x83U, 0x53U, 0x99U, 0x61U,
    0x17U, 0x2bU, 0x04U, 0x7eU, 0xbaU, 0x77U, 0xd6U, 0x26U,
    0xe1U, 0x69U, 0x14U, 0x63U, 0x55U, 0x21U, 0x0cU, 0x7dU,
};
#endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT */
#endif /* HAVE_AES_DECRYPT */

#define GETBYTE(x, y) (word32)((byte)((x) >> (8 * (y))))

#ifdef WOLFSSL_AES_SMALL_TABLES
static const byte Tsbox[256] = {
    0x63U, 0x7cU, 0x77U, 0x7bU, 0xf2U, 0x6bU, 0x6fU, 0xc5U,
    0x30U, 0x01U, 0x67U, 0x2bU, 0xfeU, 0xd7U, 0xabU, 0x76U,
    0xcaU, 0x82U, 0xc9U, 0x7dU, 0xfaU, 0x59U, 0x47U, 0xf0U,
    0xadU, 0xd4U, 0xa2U, 0xafU, 0x9cU, 0xa4U, 0x72U, 0xc0U,
    0xb7U, 0xfdU, 0x93U, 0x26U, 0x36U, 0x3fU, 0xf7U, 0xccU,
    0x34U, 0xa5U, 0xe5U, 0xf1U, 0x71U, 0xd8U, 0x31U, 0x15U,
    0x04U, 0xc7U, 0x23U, 0xc3U, 0x18U, 0x96U, 0x05U, 0x9aU,
    0x07U, 0x12U, 0x80U, 0xe2U, 0xebU, 0x27U, 0xb2U, 0x75U,
    0x09U, 0x83U, 0x2cU, 0x1aU, 0x1bU, 0x6eU, 0x5aU, 0xa0U,
    0x52U, 0x3bU, 0xd6U, 0xb3U, 0x29U, 0xe3U, 0x2fU, 0x84U,
    0x53U, 0xd1U, 0x00U, 0xedU, 0x20U, 0xfcU, 0xb1U, 0x5bU,
    0x6aU, 0xcbU, 0xbeU, 0x39U, 0x4aU, 0x4cU, 0x58U, 0xcfU,
    0xd0U, 0xefU, 0xaaU, 0xfbU, 0x43U, 0x4dU, 0x33U, 0x85U,
    0x45U, 0xf9U, 0x02U, 0x7fU, 0x50U, 0x3cU, 0x9fU, 0xa8U,
    0x51U, 0xa3U, 0x40U, 0x8fU, 0x92U, 0x9dU, 0x38U, 0xf5U,
    0xbcU, 0xb6U, 0xdaU, 0x21U, 0x10U, 0xffU, 0xf3U, 0xd2U,
    0xcdU, 0x0cU, 0x13U, 0xecU, 0x5fU, 0x97U, 0x44U, 0x17U,
    0xc4U, 0xa7U, 0x7eU, 0x3dU, 0x64U, 0x5dU, 0x19U, 0x73U,
    0x60U, 0x81U, 0x4fU, 0xdcU, 0x22U, 0x2aU, 0x90U, 0x88U,
    0x46U, 0xeeU, 0xb8U, 0x14U, 0xdeU, 0x5eU, 0x0bU, 0xdbU,
    0xe0U, 0x32U, 0x3aU, 0x0aU, 0x49U, 0x06U, 0x24U, 0x5cU,
    0xc2U, 0xd3U, 0xacU, 0x62U, 0x91U, 0x95U, 0xe4U, 0x79U,
    0xe7U, 0xc8U, 0x37U, 0x6dU, 0x8dU, 0xd5U, 0x4eU, 0xa9U,
    0x6cU, 0x56U, 0xf4U, 0xeaU, 0x65U, 0x7aU, 0xaeU, 0x08U,
    0xbaU, 0x78U, 0x25U, 0x2eU, 0x1cU, 0xa6U, 0xb4U, 0xc6U,
    0xe8U, 0xddU, 0x74U, 0x1fU, 0x4bU, 0xbdU, 0x8bU, 0x8aU,
    0x70U, 0x3eU, 0xb5U, 0x66U, 0x48U, 0x03U, 0xf6U, 0x0eU,
    0x61U, 0x35U, 0x57U, 0xb9U, 0x86U, 0xc1U, 0x1dU, 0x9eU,
    0xe1U, 0xf8U, 0x98U, 0x11U, 0x69U, 0xd9U, 0x8eU, 0x94U,
    0x9bU, 0x1eU, 0x87U, 0xe9U, 0xceU, 0x55U, 0x28U, 0xdfU,
    0x8cU, 0xa1U, 0x89U, 0x0dU, 0xbfU, 0xe6U, 0x42U, 0x68U,
    0x41U, 0x99U, 0x2dU, 0x0fU, 0xb0U, 0x54U, 0xbbU, 0x16U
};

#define AES_XTIME(x)    ((byte)((byte)((x) << 1) ^ ((0 - ((x) >> 7)) & 0x1b)))

static WARN_UNUSED_RESULT word32 col_mul(
    word32 t, int i2, int i3, int ia, int ib)
{
    byte t3 = GETBYTE(t, i3);
    byte tm = AES_XTIME(GETBYTE(t, i2) ^ t3);

    return GETBYTE(t, ia) ^ GETBYTE(t, ib) ^ t3 ^ tm;
}

#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT)
static WARN_UNUSED_RESULT word32 inv_col_mul(
    word32 t, int i9, int ib, int id, int ie)
{
    byte t9 = GETBYTE(t, i9);
    byte tb = GETBYTE(t, ib);
    byte td = GETBYTE(t, id);
    byte te = GETBYTE(t, ie);
    byte t0 = t9 ^ tb ^ td;
    return t0 ^ AES_XTIME(AES_XTIME(AES_XTIME(t0 ^ te) ^ td ^ te) ^ tb ^ te);
}
#endif
#endif

#if defined(HAVE_AES_CBC) || defined(WOLFSSL_AES_DIRECT) || \
                                    defined(HAVE_AESCCM) || defined(HAVE_AESGCM)

#ifndef WC_CACHE_LINE_SZ
    #if defined(__x86_64__) || defined(_M_X64) || \
       (defined(__ILP32__) && (__ILP32__ >= 1))
        #define WC_CACHE_LINE_SZ 64
    #else
        /* default cache line size */
        #define WC_CACHE_LINE_SZ 32
    #endif
#endif

#ifndef WC_NO_CACHE_RESISTANT

#if defined(__riscv) && !defined(WOLFSSL_AES_TOUCH_LINES)
    #define WOLFSSL_AES_TOUCH_LINES
#endif

#ifndef WOLFSSL_AES_SMALL_TABLES
/* load 4 Te Tables into cache by cache line stride */
static WARN_UNUSED_RESULT WC_INLINE word32 PreFetchTe(void)
{
#ifndef WOLFSSL_AES_TOUCH_LINES
    word32 x = 0;
    int i,j;

    for (i = 0; i < 4; i++) {
        /* 256 elements, each one is 4 bytes */
        for (j = 0; j < 256; j += WC_CACHE_LINE_SZ/4) {
            x &= Te[i][j];
        }
    }
    return x;
#else
    return 0;
#endif
}
#else
/* load sbox into cache by cache line stride */
static WARN_UNUSED_RESULT WC_INLINE word32 PreFetchSBox(void)
{
#ifndef WOLFSSL_AES_TOUCH_LINES
    word32 x = 0;
    int i;

    for (i = 0; i < 256; i += WC_CACHE_LINE_SZ/4) {
        x &= Tsbox[i];
    }
    return x;
#else
    return 0;
#endif
}
#endif
#endif

#ifdef WOLFSSL_AES_TOUCH_LINES
#if WC_CACHE_LINE_SZ == 128
    #define WC_CACHE_LINE_BITS 5
    #define WC_CACHE_LINE_MASK_HI 0xe0
    #define WC_CACHE_LINE_MASK_LO 0x1f
    #define WC_CACHE_LINE_ADD     0x20
#elif WC_CACHE_LINE_SZ == 64
    #define WC_CACHE_LINE_BITS 4
    #define WC_CACHE_LINE_MASK_HI 0xf0
    #define WC_CACHE_LINE_MASK_LO 0x0f
    #define WC_CACHE_LINE_ADD     0x10
#elif WC_CACHE_LINE_SZ == 32
    #define WC_CACHE_LINE_BITS 3
    #define WC_CACHE_LINE_MASK_HI 0xf8
    #define WC_CACHE_LINE_MASK_LO 0x07
    #define WC_CACHE_LINE_ADD     0x08
#elif WC_CACHE_LINE_SZ = 16
    #define WC_CACHE_LINE_BITS 2
    #define WC_CACHE_LINE_MASK_HI 0xfc
    #define WC_CACHE_LINE_MASK_LO 0x03
    #define WC_CACHE_LINE_ADD     0x04
#else
    #error Cache line size not supported
#endif

#ifndef WOLFSSL_AES_SMALL_TABLES
static word32 GetTable(const word32* t, byte o)
{
#if WC_CACHE_LINE_SZ == 64
    word32 e;
    byte hi = o & 0xf0;
    byte lo = o & 0x0f;

    e  = t[lo + 0x00] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x10] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x20] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x30] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x40] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x50] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x60] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x70] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x80] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x90] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xa0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xb0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xc0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xd0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xe0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xf0] & ((word32)0 - (((word32)hi - 0x01) >> 31));

    return e;
#else
    word32 e = 0;
    int i;
    byte hi = o & WC_CACHE_LINE_MASK_HI;
    byte lo = o & WC_CACHE_LINE_MASK_LO;

    for (i = 0; i < 256; i += (1 << WC_CACHE_LINE_BITS)) {
        e |= t[lo + i] & ((word32)0 - (((word32)hi - 0x01) >> 31));
        hi -= WC_CACHE_LINE_ADD;
    }

    return e;
#endif
}
#endif

#ifdef WOLFSSL_AES_SMALL_TABLES
static byte GetTable8(const byte* t, byte o)
{
#if WC_CACHE_LINE_SZ == 64
    byte e;
    byte hi = o & 0xf0;
    byte lo = o & 0x0f;

    e  = t[lo + 0x00] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x10] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x20] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x30] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x40] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x50] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x60] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x70] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x80] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0x90] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xa0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xb0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xc0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xd0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xe0] & ((word32)0 - (((word32)hi - 0x01) >> 31)); hi -= 0x10;
    e |= t[lo + 0xf0] & ((word32)0 - (((word32)hi - 0x01) >> 31));

    return e;
#else
    byte e = 0;
    int i;
    byte hi = o & WC_CACHE_LINE_MASK_HI;
    byte lo = o & WC_CACHE_LINE_MASK_LO;

    for (i = 0; i < 256; i += (1 << WC_CACHE_LINE_BITS)) {
        e |= t[lo + i] & ((word32)0 - (((word32)hi - 0x01) >> 31));
        hi -= WC_CACHE_LINE_ADD;
    }

    return e;
#endif
}
#endif

#ifndef WOLFSSL_AES_SMALL_TABLES
static void GetTable_Multi(const word32* t, word32* t0, byte o0,
    word32* t1, byte o1, word32* t2, byte o2, word32* t3, byte o3)
{
    word32 e0 = 0;
    word32 e1 = 0;
    word32 e2 = 0;
    word32 e3 = 0;
    byte hi0 = o0 & WC_CACHE_LINE_MASK_HI;
    byte lo0 = o0 & WC_CACHE_LINE_MASK_LO;
    byte hi1 = o1 & WC_CACHE_LINE_MASK_HI;
    byte lo1 = o1 & WC_CACHE_LINE_MASK_LO;
    byte hi2 = o2 & WC_CACHE_LINE_MASK_HI;
    byte lo2 = o2 & WC_CACHE_LINE_MASK_LO;
    byte hi3 = o3 & WC_CACHE_LINE_MASK_HI;
    byte lo3 = o3 & WC_CACHE_LINE_MASK_LO;
    int i;

    for (i = 0; i < 256; i += (1 << WC_CACHE_LINE_BITS)) {
        e0 |= t[lo0 + i] & ((word32)0 - (((word32)hi0 - 0x01) >> 31));
        hi0 -= WC_CACHE_LINE_ADD;
        e1 |= t[lo1 + i] & ((word32)0 - (((word32)hi1 - 0x01) >> 31));
        hi1 -= WC_CACHE_LINE_ADD;
        e2 |= t[lo2 + i] & ((word32)0 - (((word32)hi2 - 0x01) >> 31));
        hi2 -= WC_CACHE_LINE_ADD;
        e3 |= t[lo3 + i] & ((word32)0 - (((word32)hi3 - 0x01) >> 31));
        hi3 -= WC_CACHE_LINE_ADD;
    }
    *t0 = e0;
    *t1 = e1;
    *t2 = e2;
    *t3 = e3;
}
static void XorTable_Multi(const word32* t, word32* t0, byte o0,
    word32* t1, byte o1, word32* t2, byte o2, word32* t3, byte o3)
{
    word32 e0 = 0;
    word32 e1 = 0;
    word32 e2 = 0;
    word32 e3 = 0;
    byte hi0 = o0 & 0xf0;
    byte lo0 = o0 & 0x0f;
    byte hi1 = o1 & 0xf0;
    byte lo1 = o1 & 0x0f;
    byte hi2 = o2 & 0xf0;
    byte lo2 = o2 & 0x0f;
    byte hi3 = o3 & 0xf0;
    byte lo3 = o3 & 0x0f;
    int i;

    for (i = 0; i < 256; i += (1 << WC_CACHE_LINE_BITS)) {
        e0 |= t[lo0 + i] & ((word32)0 - (((word32)hi0 - 0x01) >> 31));
        hi0 -= WC_CACHE_LINE_ADD;
        e1 |= t[lo1 + i] & ((word32)0 - (((word32)hi1 - 0x01) >> 31));
        hi1 -= WC_CACHE_LINE_ADD;
        e2 |= t[lo2 + i] & ((word32)0 - (((word32)hi2 - 0x01) >> 31));
        hi2 -= WC_CACHE_LINE_ADD;
        e3 |= t[lo3 + i] & ((word32)0 - (((word32)hi3 - 0x01) >> 31));
        hi3 -= WC_CACHE_LINE_ADD;
    }
    *t0 ^= e0;
    *t1 ^= e1;
    *t2 ^= e2;
    *t3 ^= e3;
}
static word32 GetTable8_4(const byte* t, byte o0, byte o1, byte o2, byte o3)
{
    word32 e = 0;
    int i;
    byte hi0 = o0 & WC_CACHE_LINE_MASK_HI;
    byte lo0 = o0 & WC_CACHE_LINE_MASK_LO;
    byte hi1 = o1 & WC_CACHE_LINE_MASK_HI;
    byte lo1 = o1 & WC_CACHE_LINE_MASK_LO;
    byte hi2 = o2 & WC_CACHE_LINE_MASK_HI;
    byte lo2 = o2 & WC_CACHE_LINE_MASK_LO;
    byte hi3 = o3 & WC_CACHE_LINE_MASK_HI;
    byte lo3 = o3 & WC_CACHE_LINE_MASK_LO;

    for (i = 0; i < 256; i += (1 << WC_CACHE_LINE_BITS)) {
        e |= (word32)(t[lo0 + i] & ((word32)0 - (((word32)hi0 - 0x01) >> 31)))
             << 24;
        hi0 -= WC_CACHE_LINE_ADD;
        e |= (word32)(t[lo1 + i] & ((word32)0 - (((word32)hi1 - 0x01) >> 31)))
             << 16;
        hi1 -= WC_CACHE_LINE_ADD;
        e |= (word32)(t[lo2 + i] & ((word32)0 - (((word32)hi2 - 0x01) >> 31)))
             <<  8;
        hi2 -= WC_CACHE_LINE_ADD;
        e |= (word32)(t[lo3 + i] & ((word32)0 - (((word32)hi3 - 0x01) >> 31)))
             <<  0;
        hi3 -= WC_CACHE_LINE_ADD;
    }

    return e;
}
#endif
#else
#define GetTable(t, o)  t[o]
#define GetTable8(t, o) t[o]
#define GetTable_Multi(t, t0, o0, t1, o1, t2, o2, t3, o3)  \
    *(t0) = (t)[o0]; *(t1) = (t)[o1]; *(t2) = (t)[o2]; *(t3) = (t)[o3]
#define XorTable_Multi(t, t0, o0, t1, o1, t2, o2, t3, o3)  \
    *(t0) ^= (t)[o0]; *(t1) ^= (t)[o1]; *(t2) ^= (t)[o2]; *(t3) ^= (t)[o3]
#define GetTable8_4(t, o0, o1, o2, o3) \
    (((word32)(t)[o0] << 24) | ((word32)(t)[o1] << 16) |   \
     ((word32)(t)[o2] <<  8) | ((word32)(t)[o3] <<  0))
#endif

/* Software AES - ECB Encrypt */
static WARN_UNUSED_RESULT int wc_AesEncrypt(
    Aes* aes, const byte* inBlock, byte* outBlock)
{
    word32 s0, s1, s2, s3;
    word32 t0, t1, t2, t3;
    word32 r = aes->rounds >> 1;
    const word32* rk = aes->key;

    if (r > 7 || r == 0) {
        WOLFSSL_ERROR_VERBOSE(KEYUSAGE_E);
        return KEYUSAGE_E;
    }

#ifdef WOLFSSL_AESNI
    if (haveAESNI && aes->use_aesni) {
        #ifdef DEBUG_AESNI
            printf("about to aes encrypt\n");
            printf("in  = %p\n", inBlock);
            printf("out = %p\n", outBlock);
            printf("aes->key = %p\n", aes->key);
            printf("aes->rounds = %d\n", aes->rounds);
            printf("sz = %d\n", AES_BLOCK_SIZE);
        #endif

        /* check alignment, decrypt doesn't need alignment */
        if ((wc_ptr_t)inBlock % AESNI_ALIGN) {
        #ifndef NO_WOLFSSL_ALLOC_ALIGN
            byte* tmp = (byte*)XMALLOC(AES_BLOCK_SIZE + AESNI_ALIGN, aes->heap,
                                                      DYNAMIC_TYPE_TMP_BUFFER);
            byte* tmp_align;
            if (tmp == NULL)
                return MEMORY_E;

            tmp_align = tmp + (AESNI_ALIGN - ((wc_ptr_t)tmp % AESNI_ALIGN));

            XMEMCPY(tmp_align, inBlock, AES_BLOCK_SIZE);
            AES_ECB_encrypt(tmp_align, tmp_align, AES_BLOCK_SIZE,
                    (byte*)aes->key, (int)aes->rounds);
            XMEMCPY(outBlock, tmp_align, AES_BLOCK_SIZE);
            XFREE(tmp, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return 0;
        #else
            WOLFSSL_MSG("AES-ECB encrypt with bad alignment");
            WOLFSSL_ERROR_VERBOSE(BAD_ALIGN_E);
            return BAD_ALIGN_E;
        #endif
        }

        AES_ECB_encrypt(inBlock, outBlock, AES_BLOCK_SIZE, (byte*)aes->key,
                        (int)aes->rounds);

        return 0;
    }
    else {
        #ifdef DEBUG_AESNI
            printf("Skipping AES-NI\n");
        #endif
    }
#endif
#if defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_AES)
    AES_ECB_encrypt(aes, inBlock, outBlock, AES_BLOCK_SIZE);
    return 0;
#endif

#if defined(WOLFSSL_IMXRT_DCP)
    if (aes->keylen == 16) {
        DCPAesEcbEncrypt(aes, outBlock, inBlock, AES_BLOCK_SIZE);
        return 0;
    }
#endif

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    if (aes->useSWCrypt == 0) {
        return se050_aes_crypt(aes, inBlock, outBlock, AES_BLOCK_SIZE,
                               AES_ENCRYPTION, kAlgorithm_SSS_AES_ECB);
    }
#endif

    /*
     * map byte array block to cipher state
     * and add initial round key:
     */
    XMEMCPY(&s0, inBlock,                  sizeof(s0));
    XMEMCPY(&s1, inBlock +     sizeof(s0), sizeof(s1));
    XMEMCPY(&s2, inBlock + 2 * sizeof(s0), sizeof(s2));
    XMEMCPY(&s3, inBlock + 3 * sizeof(s0), sizeof(s3));

#ifdef LITTLE_ENDIAN_ORDER
    s0 = ByteReverseWord32(s0);
    s1 = ByteReverseWord32(s1);
    s2 = ByteReverseWord32(s2);
    s3 = ByteReverseWord32(s3);
#endif

    /* AddRoundKey */
    s0 ^= rk[0];
    s1 ^= rk[1];
    s2 ^= rk[2];
    s3 ^= rk[3];

#ifndef WOLFSSL_AES_SMALL_TABLES
#ifndef WC_NO_CACHE_RESISTANT
    s0 |= PreFetchTe();
#endif

#ifndef WOLFSSL_AES_TOUCH_LINES
#define ENC_ROUND_T_S(o)                                                       \
    t0 = GetTable(Te[0], GETBYTE(s0, 3)) ^ GetTable(Te[1], GETBYTE(s1, 2)) ^   \
         GetTable(Te[2], GETBYTE(s2, 1)) ^ GetTable(Te[3], GETBYTE(s3, 0)) ^   \
         rk[(o)+4];                                                            \
    t1 = GetTable(Te[0], GETBYTE(s1, 3)) ^ GetTable(Te[1], GETBYTE(s2, 2)) ^   \
         GetTable(Te[2], GETBYTE(s3, 1)) ^ GetTable(Te[3], GETBYTE(s0, 0)) ^   \
         rk[(o)+5];                                                            \
    t2 = GetTable(Te[0], GETBYTE(s2, 3)) ^ GetTable(Te[1], GETBYTE(s3, 2)) ^   \
         GetTable(Te[2], GETBYTE(s0, 1)) ^ GetTable(Te[3], GETBYTE(s1, 0)) ^   \
         rk[(o)+6];                                                            \
    t3 = GetTable(Te[0], GETBYTE(s3, 3)) ^ GetTable(Te[1], GETBYTE(s0, 2)) ^   \
         GetTable(Te[2], GETBYTE(s1, 1)) ^ GetTable(Te[3], GETBYTE(s2, 0)) ^   \
         rk[(o)+7]
#define ENC_ROUND_S_T(o)                                                       \
    s0 = GetTable(Te[0], GETBYTE(t0, 3)) ^ GetTable(Te[1], GETBYTE(t1, 2)) ^   \
         GetTable(Te[2], GETBYTE(t2, 1)) ^ GetTable(Te[3], GETBYTE(t3, 0)) ^   \
         rk[(o)+0];                                                            \
    s1 = GetTable(Te[0], GETBYTE(t1, 3)) ^ GetTable(Te[1], GETBYTE(t2, 2)) ^   \
         GetTable(Te[2], GETBYTE(t3, 1)) ^ GetTable(Te[3], GETBYTE(t0, 0)) ^   \
         rk[(o)+1];                                                            \
    s2 = GetTable(Te[0], GETBYTE(t2, 3)) ^ GetTable(Te[1], GETBYTE(t3, 2)) ^   \
         GetTable(Te[2], GETBYTE(t0, 1)) ^ GetTable(Te[3], GETBYTE(t1, 0)) ^   \
         rk[(o)+2];                                                            \
    s3 = GetTable(Te[0], GETBYTE(t3, 3)) ^ GetTable(Te[1], GETBYTE(t0, 2)) ^   \
         GetTable(Te[2], GETBYTE(t1, 1)) ^ GetTable(Te[3], GETBYTE(t2, 0)) ^   \
         rk[(o)+3]
#else
#define ENC_ROUND_T_S(o)                                                       \
    GetTable_Multi(Te[0], &t0, GETBYTE(s0, 3), &t1, GETBYTE(s1, 3),            \
                          &t2, GETBYTE(s2, 3), &t3, GETBYTE(s3, 3));           \
    XorTable_Multi(Te[1], &t0, GETBYTE(s1, 2), &t1, GETBYTE(s2, 2),            \
                          &t2, GETBYTE(s3, 2), &t3, GETBYTE(s0, 2));           \
    XorTable_Multi(Te[2], &t0, GETBYTE(s2, 1), &t1, GETBYTE(s3, 1),            \
                          &t2, GETBYTE(s0, 1), &t3, GETBYTE(s1, 1));           \
    XorTable_Multi(Te[3], &t0, GETBYTE(s3, 0), &t1, GETBYTE(s0, 0),            \
                          &t2, GETBYTE(s1, 0), &t3, GETBYTE(s2, 0));           \
    t0 ^= rk[(o)+4]; t1 ^= rk[(o)+5]; t2 ^= rk[(o)+6]; t3 ^= rk[(o)+7];

#define ENC_ROUND_S_T(o)                                                       \
    GetTable_Multi(Te[0], &s0, GETBYTE(t0, 3), &s1, GETBYTE(t1, 3),            \
                          &s2, GETBYTE(t2, 3), &s3, GETBYTE(t3, 3));           \
    XorTable_Multi(Te[1], &s0, GETBYTE(t1, 2), &s1, GETBYTE(t2, 2),            \
                          &s2, GETBYTE(t3, 2), &s3, GETBYTE(t0, 2));           \
    XorTable_Multi(Te[2], &s0, GETBYTE(t2, 1), &s1, GETBYTE(t3, 1),            \
                          &s2, GETBYTE(t0, 1), &s3, GETBYTE(t1, 1));           \
    XorTable_Multi(Te[3], &s0, GETBYTE(t3, 0), &s1, GETBYTE(t0, 0),            \
                          &s2, GETBYTE(t1, 0), &s3, GETBYTE(t2, 0));           \
    s0 ^= rk[(o)+0]; s1 ^= rk[(o)+1]; s2 ^= rk[(o)+2]; s3 ^= rk[(o)+3];
#endif

#ifndef WOLFSSL_AES_NO_UNROLL
/* Unroll the loop. */
                       ENC_ROUND_T_S( 0);
    ENC_ROUND_S_T( 8); ENC_ROUND_T_S( 8);
    ENC_ROUND_S_T(16); ENC_ROUND_T_S(16);
    ENC_ROUND_S_T(24); ENC_ROUND_T_S(24);
    ENC_ROUND_S_T(32); ENC_ROUND_T_S(32);
    if (r > 5) {
        ENC_ROUND_S_T(40); ENC_ROUND_T_S(40);
        if (r > 6) {
            ENC_ROUND_S_T(48); ENC_ROUND_T_S(48);
        }
    }
    rk += r * 8;
#else
    /*
     * Nr - 1 full rounds:
     */

    for (;;) {
        ENC_ROUND_T_S(0);

        rk += 8;
        if (--r == 0) {
            break;
        }

        ENC_ROUND_S_T(0);
    }
#endif

    /*
     * apply last round and
     * map cipher state to byte array block:
     */

#ifndef WOLFSSL_AES_TOUCH_LINES
    s0 =
        (GetTable(Te[2], GETBYTE(t0, 3)) & 0xff000000) ^
        (GetTable(Te[3], GETBYTE(t1, 2)) & 0x00ff0000) ^
        (GetTable(Te[0], GETBYTE(t2, 1)) & 0x0000ff00) ^
        (GetTable(Te[1], GETBYTE(t3, 0)) & 0x000000ff) ^
        rk[0];
    s1 =
        (GetTable(Te[2], GETBYTE(t1, 3)) & 0xff000000) ^
        (GetTable(Te[3], GETBYTE(t2, 2)) & 0x00ff0000) ^
        (GetTable(Te[0], GETBYTE(t3, 1)) & 0x0000ff00) ^
        (GetTable(Te[1], GETBYTE(t0, 0)) & 0x000000ff) ^
        rk[1];
    s2 =
        (GetTable(Te[2], GETBYTE(t2, 3)) & 0xff000000) ^
        (GetTable(Te[3], GETBYTE(t3, 2)) & 0x00ff0000) ^
        (GetTable(Te[0], GETBYTE(t0, 1)) & 0x0000ff00) ^
        (GetTable(Te[1], GETBYTE(t1, 0)) & 0x000000ff) ^
        rk[2];
    s3 =
        (GetTable(Te[2], GETBYTE(t3, 3)) & 0xff000000) ^
        (GetTable(Te[3], GETBYTE(t0, 2)) & 0x00ff0000) ^
        (GetTable(Te[0], GETBYTE(t1, 1)) & 0x0000ff00) ^
        (GetTable(Te[1], GETBYTE(t2, 0)) & 0x000000ff) ^
        rk[3];
#else
{
    word32 u0;
    word32 u1;
    word32 u2;
    word32 u3;

    s0 = rk[0]; s1 = rk[1]; s2 = rk[2]; s3 = rk[3];
    GetTable_Multi(Te[2], &u0, GETBYTE(t0, 3), &u1, GETBYTE(t1, 3),
                          &u2, GETBYTE(t2, 3), &u3, GETBYTE(t3, 3));
    s0 ^= u0 & 0xff000000; s1 ^= u1 & 0xff000000;
    s2 ^= u2 & 0xff000000; s3 ^= u3 & 0xff000000;
    GetTable_Multi(Te[3], &u0, GETBYTE(t1, 2), &u1, GETBYTE(t2, 2),
                          &u2, GETBYTE(t3, 2), &u3, GETBYTE(t0, 2));
    s0 ^= u0 & 0x00ff0000; s1 ^= u1 & 0x00ff0000;
    s2 ^= u2 & 0x00ff0000; s3 ^= u3 & 0x00ff0000;
    GetTable_Multi(Te[0], &u0, GETBYTE(t2, 1), &u1, GETBYTE(t3, 1),
                          &u2, GETBYTE(t0, 1), &u3, GETBYTE(t1, 1));
    s0 ^= u0 & 0x0000ff00; s1 ^= u1 & 0x0000ff00;
    s2 ^= u2 & 0x0000ff00; s3 ^= u3 & 0x0000ff00;
    GetTable_Multi(Te[1], &u0, GETBYTE(t3, 0), &u1, GETBYTE(t0, 0),
                          &u2, GETBYTE(t1, 0), &u3, GETBYTE(t2, 0));
    s0 ^= u0 & 0x000000ff; s1 ^= u1 & 0x000000ff;
    s2 ^= u2 & 0x000000ff; s3 ^= u3 & 0x000000ff;
}
#endif
#else
#ifndef WC_NO_CACHE_RESISTANT
    s0 |= PreFetchSBox();
#endif

    r *= 2;
    /* Two rounds at a time */
    for (rk += 4; r > 1; r--, rk += 4) {
        t0 =
            ((word32)GetTable8(Tsbox, GETBYTE(s0, 3)) << 24) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s1, 2)) << 16) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s2, 1)) <<  8) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s3, 0)));
        t1 =
            ((word32)GetTable8(Tsbox, GETBYTE(s1, 3)) << 24) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s2, 2)) << 16) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s3, 1)) <<  8) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s0, 0)));
        t2 =
            ((word32)GetTable8(Tsbox, GETBYTE(s2, 3)) << 24) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s3, 2)) << 16) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s0, 1)) <<  8) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s1, 0)));
        t3 =
            ((word32)GetTable8(Tsbox, GETBYTE(s3, 3)) << 24) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s0, 2)) << 16) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s1, 1)) <<  8) ^
            ((word32)GetTable8(Tsbox, GETBYTE(s2, 0)));

        s0 =
            (col_mul(t0, 3, 2, 0, 1) << 24) ^
            (col_mul(t0, 2, 1, 0, 3) << 16) ^
            (col_mul(t0, 1, 0, 2, 3) <<  8) ^
            (col_mul(t0, 0, 3, 2, 1)      ) ^
            rk[0];
        s1 =
            (col_mul(t1, 3, 2, 0, 1) << 24) ^
            (col_mul(t1, 2, 1, 0, 3) << 16) ^
            (col_mul(t1, 1, 0, 2, 3) <<  8) ^
            (col_mul(t1, 0, 3, 2, 1)      ) ^
            rk[1];
        s2 =
            (col_mul(t2, 3, 2, 0, 1) << 24) ^
            (col_mul(t2, 2, 1, 0, 3) << 16) ^
            (col_mul(t2, 1, 0, 2, 3) <<  8) ^
            (col_mul(t2, 0, 3, 2, 1)      ) ^
            rk[2];
        s3 =
            (col_mul(t3, 3, 2, 0, 1) << 24) ^
            (col_mul(t3, 2, 1, 0, 3) << 16) ^
            (col_mul(t3, 1, 0, 2, 3) <<  8) ^
            (col_mul(t3, 0, 3, 2, 1)      ) ^
            rk[3];
    }

    t0 =
        ((word32)GetTable8(Tsbox, GETBYTE(s0, 3)) << 24) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s1, 2)) << 16) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s2, 1)) <<  8) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s3, 0)));
    t1 =
        ((word32)GetTable8(Tsbox, GETBYTE(s1, 3)) << 24) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s2, 2)) << 16) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s3, 1)) <<  8) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s0, 0)));
    t2 =
        ((word32)GetTable8(Tsbox, GETBYTE(s2, 3)) << 24) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s3, 2)) << 16) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s0, 1)) <<  8) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s1, 0)));
    t3 =
        ((word32)GetTable8(Tsbox, GETBYTE(s3, 3)) << 24) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s0, 2)) << 16) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s1, 1)) <<  8) ^
        ((word32)GetTable8(Tsbox, GETBYTE(s2, 0)));
    s0 = t0 ^ rk[0];
    s1 = t1 ^ rk[1];
    s2 = t2 ^ rk[2];
    s3 = t3 ^ rk[3];
#endif

    /* write out */
#ifdef LITTLE_ENDIAN_ORDER
    s0 = ByteReverseWord32(s0);
    s1 = ByteReverseWord32(s1);
    s2 = ByteReverseWord32(s2);
    s3 = ByteReverseWord32(s3);
#endif

    XMEMCPY(outBlock,                  &s0, sizeof(s0));
    XMEMCPY(outBlock +     sizeof(s0), &s1, sizeof(s1));
    XMEMCPY(outBlock + 2 * sizeof(s0), &s2, sizeof(s2));
    XMEMCPY(outBlock + 3 * sizeof(s0), &s3, sizeof(s3));

    return 0;
}
#endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT || HAVE_AESGCM */

#if defined(HAVE_AES_DECRYPT)
#if (defined(HAVE_AES_CBC) && !defined(WOLFSSL_DEVCRYPTO_CBC) && \
                              !defined(WOLFSSL_SILABS_SE_ACCEL)) || \
    defined(WOLFSSL_AES_DIRECT)

#ifndef WC_NO_CACHE_RESISTANT
#ifndef WOLFSSL_AES_SMALL_TABLES
/* load 4 Td Tables into cache by cache line stride */
static WARN_UNUSED_RESULT WC_INLINE word32 PreFetchTd(void)
{
    word32 x = 0;
    int i,j;

    for (i = 0; i < 4; i++) {
        /* 256 elements, each one is 4 bytes */
        for (j = 0; j < 256; j += WC_CACHE_LINE_SZ/4) {
            x &= Td[i][j];
        }
    }
    return x;
}
#endif

/* load Td Table4 into cache by cache line stride */
static WARN_UNUSED_RESULT WC_INLINE word32 PreFetchTd4(void)
{
#ifndef WOLFSSL_AES_TOUCH_LINES
    word32 x = 0;
    int i;

    for (i = 0; i < 256; i += WC_CACHE_LINE_SZ) {
        x &= (word32)Td4[i];
    }
    return x;
#else
    return 0;
#endif
}
#endif

/* Software AES - ECB Decrypt */
static WARN_UNUSED_RESULT int wc_AesDecrypt(
    Aes* aes, const byte* inBlock, byte* outBlock)
{
    word32 s0, s1, s2, s3;
    word32 t0, t1, t2, t3;
    word32 r = aes->rounds >> 1;
    const word32* rk = aes->key;

    if (r > 7 || r == 0) {
        WOLFSSL_ERROR_VERBOSE(KEYUSAGE_E);
        return KEYUSAGE_E;
    }

#ifdef WOLFSSL_AESNI
    if (haveAESNI && aes->use_aesni) {
        #ifdef DEBUG_AESNI
            printf("about to aes decrypt\n");
            printf("in  = %p\n", inBlock);
            printf("out = %p\n", outBlock);
            printf("aes->key = %p\n", aes->key);
            printf("aes->rounds = %d\n", aes->rounds);
            printf("sz = %d\n", AES_BLOCK_SIZE);
        #endif

        /* if input and output same will overwrite input iv */
        if ((const byte*)aes->tmp != inBlock)
            XMEMCPY(aes->tmp, inBlock, AES_BLOCK_SIZE);
        AES_ECB_decrypt(inBlock, outBlock, AES_BLOCK_SIZE, (byte*)aes->key,
                        (int)aes->rounds);
        return 0;
    }
    else {
        #ifdef DEBUG_AESNI
            printf("Skipping AES-NI\n");
        #endif
    }
#endif /* WOLFSSL_AESNI */
#if defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_AES)
    return AES_ECB_decrypt(aes, inBlock, outBlock, AES_BLOCK_SIZE);
#endif
#if defined(WOLFSSL_IMXRT_DCP)
    if (aes->keylen == 16) {
        DCPAesEcbDecrypt(aes, outBlock, inBlock, AES_BLOCK_SIZE);
        return 0;
    }
#endif
#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    if (aes->useSWCrypt == 0) {
        return se050_aes_crypt(aes, inBlock, outBlock, AES_BLOCK_SIZE,
                               AES_DECRYPTION, kAlgorithm_SSS_AES_ECB);
    }
#endif

    /*
     * map byte array block to cipher state
     * and add initial round key:
     */
    XMEMCPY(&s0, inBlock,                  sizeof(s0));
    XMEMCPY(&s1, inBlock + sizeof(s0),     sizeof(s1));
    XMEMCPY(&s2, inBlock + 2 * sizeof(s0), sizeof(s2));
    XMEMCPY(&s3, inBlock + 3 * sizeof(s0), sizeof(s3));

#ifdef LITTLE_ENDIAN_ORDER
    s0 = ByteReverseWord32(s0);
    s1 = ByteReverseWord32(s1);
    s2 = ByteReverseWord32(s2);
    s3 = ByteReverseWord32(s3);
#endif

    s0 ^= rk[0];
    s1 ^= rk[1];
    s2 ^= rk[2];
    s3 ^= rk[3];

#ifndef WOLFSSL_AES_SMALL_TABLES
#ifndef WC_NO_CACHE_RESISTANT
    s0 |= PreFetchTd();
#endif

#ifndef WOLFSSL_AES_TOUCH_LINES
/* Unroll the loop. */
#define DEC_ROUND_T_S(o)                                            \
    t0 = GetTable(Td[0], GETBYTE(s0, 3)) ^ GetTable(Td[1], GETBYTE(s3, 2)) ^            \
         GetTable(Td[2], GETBYTE(s2, 1)) ^ GetTable(Td[3], GETBYTE(s1, 0)) ^ rk[(o)+4]; \
    t1 = GetTable(Td[0], GETBYTE(s1, 3)) ^ GetTable(Td[1], GETBYTE(s0, 2)) ^            \
         GetTable(Td[2], GETBYTE(s3, 1)) ^ GetTable(Td[3], GETBYTE(s2, 0)) ^ rk[(o)+5]; \
    t2 = GetTable(Td[0], GETBYTE(s2, 3)) ^ GetTable(Td[1], GETBYTE(s1, 2)) ^            \
         GetTable(Td[2], GETBYTE(s0, 1)) ^ GetTable(Td[3], GETBYTE(s3, 0)) ^ rk[(o)+6]; \
    t3 = GetTable(Td[0], GETBYTE(s3, 3)) ^ GetTable(Td[1], GETBYTE(s2, 2)) ^            \
         GetTable(Td[2], GETBYTE(s1, 1)) ^ GetTable(Td[3], GETBYTE(s0, 0)) ^ rk[(o)+7]
#define DEC_ROUND_S_T(o)                                            \
    s0 = GetTable(Td[0], GETBYTE(t0, 3)) ^ GetTable(Td[1], GETBYTE(t3, 2)) ^            \
         GetTable(Td[2], GETBYTE(t2, 1)) ^ GetTable(Td[3], GETBYTE(t1, 0)) ^ rk[(o)+0]; \
    s1 = GetTable(Td[0], GETBYTE(t1, 3)) ^ GetTable(Td[1], GETBYTE(t0, 2)) ^            \
         GetTable(Td[2], GETBYTE(t3, 1)) ^ GetTable(Td[3], GETBYTE(t2, 0)) ^ rk[(o)+1]; \
    s2 = GetTable(Td[0], GETBYTE(t2, 3)) ^ GetTable(Td[1], GETBYTE(t1, 2)) ^            \
         GetTable(Td[2], GETBYTE(t0, 1)) ^ GetTable(Td[3], GETBYTE(t3, 0)) ^ rk[(o)+2]; \
    s3 = GetTable(Td[0], GETBYTE(t3, 3)) ^ GetTable(Td[1], GETBYTE(t2, 2)) ^            \
         GetTable(Td[2], GETBYTE(t1, 1)) ^ GetTable(Td[3], GETBYTE(t0, 0)) ^ rk[(o)+3]
#else
#define DEC_ROUND_T_S(o)                                                       \
    GetTable_Multi(Td[0], &t0, GETBYTE(s0, 3), &t1, GETBYTE(s1, 3),            \
                          &t2, GETBYTE(s2, 3), &t3, GETBYTE(s3, 3));           \
    XorTable_Multi(Td[1], &t0, GETBYTE(s3, 2), &t1, GETBYTE(s0, 2),            \
                          &t2, GETBYTE(s1, 2), &t3, GETBYTE(s2, 2));           \
    XorTable_Multi(Td[2], &t0, GETBYTE(s2, 1), &t1, GETBYTE(s3, 1),            \
                          &t2, GETBYTE(s0, 1), &t3, GETBYTE(s1, 1));           \
    XorTable_Multi(Td[3], &t0, GETBYTE(s1, 0), &t1, GETBYTE(s2, 0),            \
                          &t2, GETBYTE(s3, 0), &t3, GETBYTE(s0, 0));           \
    t0 ^= rk[(o)+4]; t1 ^= rk[(o)+5]; t2 ^= rk[(o)+6]; t3 ^= rk[(o)+7];

#define DEC_ROUND_S_T(o)                                                       \
    GetTable_Multi(Td[0], &s0, GETBYTE(t0, 3), &s1, GETBYTE(t1, 3),            \
                          &s2, GETBYTE(t2, 3), &s3, GETBYTE(t3, 3));           \
    XorTable_Multi(Td[1], &s0, GETBYTE(t3, 2), &s1, GETBYTE(t0, 2),            \
                          &s2, GETBYTE(t1, 2), &s3, GETBYTE(t2, 2));           \
    XorTable_Multi(Td[2], &s0, GETBYTE(t2, 1), &s1, GETBYTE(t3, 1),            \
                          &s2, GETBYTE(t0, 1), &s3, GETBYTE(t1, 1));           \
    XorTable_Multi(Td[3], &s0, GETBYTE(t1, 0), &s1, GETBYTE(t2, 0),            \
                          &s2, GETBYTE(t3, 0), &s3, GETBYTE(t0, 0));           \
    s0 ^= rk[(o)+0]; s1 ^= rk[(o)+1]; s2 ^= rk[(o)+2]; s3 ^= rk[(o)+3];
#endif

#ifndef WOLFSSL_AES_NO_UNROLL
                       DEC_ROUND_T_S( 0);
    DEC_ROUND_S_T( 8); DEC_ROUND_T_S( 8);
    DEC_ROUND_S_T(16); DEC_ROUND_T_S(16);
    DEC_ROUND_S_T(24); DEC_ROUND_T_S(24);
    DEC_ROUND_S_T(32); DEC_ROUND_T_S(32);
    if (r > 5) {
        DEC_ROUND_S_T(40); DEC_ROUND_T_S(40);
        if (r > 6) {
            DEC_ROUND_S_T(48); DEC_ROUND_T_S(48);
        }
    }
    rk += r * 8;
#else

    /*
     * Nr - 1 full rounds:
     */

    for (;;) {
        DEC_ROUND_T_S(0);

        rk += 8;
        if (--r == 0) {
            break;
        }

        DEC_ROUND_S_T(0);
    }
#endif
    /*
     * apply last round and
     * map cipher state to byte array block:
     */

#ifndef WC_NO_CACHE_RESISTANT
    t0 |= PreFetchTd4();
#endif

    s0 = GetTable8_4(Td4, GETBYTE(t0, 3), GETBYTE(t3, 2),
                          GETBYTE(t2, 1), GETBYTE(t1, 0)) ^ rk[0];
    s1 = GetTable8_4(Td4, GETBYTE(t1, 3), GETBYTE(t0, 2),
                          GETBYTE(t3, 1), GETBYTE(t2, 0)) ^ rk[1];
    s2 = GetTable8_4(Td4, GETBYTE(t2, 3), GETBYTE(t1, 2),
                          GETBYTE(t0, 1), GETBYTE(t3, 0)) ^ rk[2];
    s3 = GetTable8_4(Td4, GETBYTE(t3, 3), GETBYTE(t2, 2),
                          GETBYTE(t1, 1), GETBYTE(t0, 0)) ^ rk[3];
#else
#ifndef WC_NO_CACHE_RESISTANT
    s0 |= PreFetchTd4();
#endif

    r *= 2;
    for (rk += 4; r > 1; r--, rk += 4) {
        t0 =
            ((word32)GetTable8(Td4, GETBYTE(s0, 3)) << 24) ^
            ((word32)GetTable8(Td4, GETBYTE(s3, 2)) << 16) ^
            ((word32)GetTable8(Td4, GETBYTE(s2, 1)) <<  8) ^
            ((word32)GetTable8(Td4, GETBYTE(s1, 0))) ^
            rk[0];
        t1 =
            ((word32)GetTable8(Td4, GETBYTE(s1, 3)) << 24) ^
            ((word32)GetTable8(Td4, GETBYTE(s0, 2)) << 16) ^
            ((word32)GetTable8(Td4, GETBYTE(s3, 1)) <<  8) ^
            ((word32)GetTable8(Td4, GETBYTE(s2, 0))) ^
            rk[1];
        t2 =
            ((word32)GetTable8(Td4, GETBYTE(s2, 3)) << 24) ^
            ((word32)GetTable8(Td4, GETBYTE(s1, 2)) << 16) ^
            ((word32)GetTable8(Td4, GETBYTE(s0, 1)) <<  8) ^
            ((word32)GetTable8(Td4, GETBYTE(s3, 0))) ^
            rk[2];
        t3 =
            ((word32)GetTable8(Td4, GETBYTE(s3, 3)) << 24) ^
            ((word32)GetTable8(Td4, GETBYTE(s2, 2)) << 16) ^
            ((word32)GetTable8(Td4, GETBYTE(s1, 1)) <<  8) ^
            ((word32)GetTable8(Td4, GETBYTE(s0, 0))) ^
            rk[3];

        s0 =
            (inv_col_mul(t0, 0, 2, 1, 3) << 24) ^
            (inv_col_mul(t0, 3, 1, 0, 2) << 16) ^
            (inv_col_mul(t0, 2, 0, 3, 1) <<  8) ^
            (inv_col_mul(t0, 1, 3, 2, 0)      );
        s1 =
            (inv_col_mul(t1, 0, 2, 1, 3) << 24) ^
            (inv_col_mul(t1, 3, 1, 0, 2) << 16) ^
            (inv_col_mul(t1, 2, 0, 3, 1) <<  8) ^
            (inv_col_mul(t1, 1, 3, 2, 0)      );
        s2 =
            (inv_col_mul(t2, 0, 2, 1, 3) << 24) ^
            (inv_col_mul(t2, 3, 1, 0, 2) << 16) ^
            (inv_col_mul(t2, 2, 0, 3, 1) <<  8) ^
            (inv_col_mul(t2, 1, 3, 2, 0)      );
        s3 =
            (inv_col_mul(t3, 0, 2, 1, 3) << 24) ^
            (inv_col_mul(t3, 3, 1, 0, 2) << 16) ^
            (inv_col_mul(t3, 2, 0, 3, 1) <<  8) ^
            (inv_col_mul(t3, 1, 3, 2, 0)      );
    }

    t0 =
        ((word32)GetTable8(Td4, GETBYTE(s0, 3)) << 24) ^
        ((word32)GetTable8(Td4, GETBYTE(s3, 2)) << 16) ^
        ((word32)GetTable8(Td4, GETBYTE(s2, 1)) <<  8) ^
        ((word32)GetTable8(Td4, GETBYTE(s1, 0)));
    t1 =
        ((word32)GetTable8(Td4, GETBYTE(s1, 3)) << 24) ^
        ((word32)GetTable8(Td4, GETBYTE(s0, 2)) << 16) ^
        ((word32)GetTable8(Td4, GETBYTE(s3, 1)) <<  8) ^
        ((word32)GetTable8(Td4, GETBYTE(s2, 0)));
    t2 =
        ((word32)GetTable8(Td4, GETBYTE(s2, 3)) << 24) ^
        ((word32)GetTable8(Td4, GETBYTE(s1, 2)) << 16) ^
        ((word32)GetTable8(Td4, GETBYTE(s0, 1)) <<  8) ^
        ((word32)GetTable8(Td4, GETBYTE(s3, 0)));
    t3 =
        ((word32)GetTable8(Td4, GETBYTE(s3, 3)) << 24) ^
        ((word32)GetTable8(Td4, GETBYTE(s2, 2)) << 16) ^
        ((word32)GetTable8(Td4, GETBYTE(s1, 1)) <<  8) ^
        ((word32)GetTable8(Td4, GETBYTE(s0, 0)));
    s0 = t0 ^ rk[0];
    s1 = t1 ^ rk[1];
    s2 = t2 ^ rk[2];
    s3 = t3 ^ rk[3];
#endif

    /* write out */
#ifdef LITTLE_ENDIAN_ORDER
    s0 = ByteReverseWord32(s0);
    s1 = ByteReverseWord32(s1);
    s2 = ByteReverseWord32(s2);
    s3 = ByteReverseWord32(s3);
#endif

    XMEMCPY(outBlock,                  &s0, sizeof(s0));
    XMEMCPY(outBlock + sizeof(s0),     &s1, sizeof(s1));
    XMEMCPY(outBlock + 2 * sizeof(s0), &s2, sizeof(s2));
    XMEMCPY(outBlock + 3 * sizeof(s0), &s3, sizeof(s3));

    return 0;
}
#endif /* HAVE_AES_CBC || WOLFSSL_AES_DIRECT */
#endif /* HAVE_AES_DECRYPT */

#endif /* NEED_AES_TABLES */



/* wc_AesSetKey */
#if defined(STM32_CRYPTO)

    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
            const byte* iv, int dir)
    {
        word32 *rk;

        (void)dir;

        if (aes == NULL || (keylen != 16 &&
        #ifdef WOLFSSL_AES_192
            keylen != 24 &&
        #endif
            keylen != 32)) {
            return BAD_FUNC_ARG;
        }

        rk = aes->key;
        aes->keylen = keylen;
        aes->rounds = keylen/4 + 6;
        XMEMCPY(rk, userKey, keylen);
    #if !defined(WOLFSSL_STM32_CUBEMX) || defined(STM32_HAL_V2)
        ByteReverseWords(rk, rk, keylen);
    #endif
    #if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_COUNTER) || \
        defined(WOLFSSL_AES_OFB)
        aes->left = 0;
    #endif
        return wc_AesSetIV(aes, iv);
    }
    #if defined(WOLFSSL_AES_DIRECT)
        int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                            const byte* iv, int dir)
        {
            return wc_AesSetKey(aes, userKey, keylen, iv, dir);
        }
    #endif

#elif defined(HAVE_COLDFIRE_SEC)
    #if defined (HAVE_THREADX)
        #include "memory_pools.h"
        extern TX_BYTE_POOL mp_ncached;  /* Non Cached memory pool */
    #endif

    #define AES_BUFFER_SIZE (AES_BLOCK_SIZE * 64)
    static unsigned char *AESBuffIn = NULL;
    static unsigned char *AESBuffOut = NULL;
    static byte *secReg;
    static byte *secKey;
    static volatile SECdescriptorType *secDesc;

    static wolfSSL_Mutex Mutex_AesSEC;

    #define SEC_DESC_AES_CBC_ENCRYPT 0x60300010
    #define SEC_DESC_AES_CBC_DECRYPT 0x60200010

    extern volatile unsigned char __MBAR[];

    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
        const byte* iv, int dir)
    {
        if (AESBuffIn == NULL) {
        #if defined (HAVE_THREADX)
            int s1, s2, s3, s4, s5;
            s5 = tx_byte_allocate(&mp_ncached,(void *)&secDesc,
                                  sizeof(SECdescriptorType), TX_NO_WAIT);
            s1 = tx_byte_allocate(&mp_ncached, (void *)&AESBuffIn,
                                  AES_BUFFER_SIZE, TX_NO_WAIT);
            s2 = tx_byte_allocate(&mp_ncached, (void *)&AESBuffOut,
                                  AES_BUFFER_SIZE, TX_NO_WAIT);
            s3 = tx_byte_allocate(&mp_ncached, (void *)&secKey,
                                  AES_BLOCK_SIZE*2, TX_NO_WAIT);
            s4 = tx_byte_allocate(&mp_ncached, (void *)&secReg,
                                  AES_BLOCK_SIZE, TX_NO_WAIT);

            if (s1 || s2 || s3 || s4 || s5)
                return BAD_FUNC_ARG;
        #else
            #warning "Allocate non-Cache buffers"
        #endif

            wc_InitMutex(&Mutex_AesSEC);
        }

        if (!((keylen == 16) || (keylen == 24) || (keylen == 32)))
            return BAD_FUNC_ARG;

        if (aes == NULL)
            return BAD_FUNC_ARG;

        aes->keylen = keylen;
        aes->rounds = keylen/4 + 6;
        XMEMCPY(aes->key, userKey, keylen);

        if (iv)
            XMEMCPY(aes->reg, iv, AES_BLOCK_SIZE);

    #if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_COUNTER) || \
        defined(WOLFSSL_AES_OFB)
        aes->left = 0;
    #endif

        return 0;
    }
#elif defined(FREESCALE_LTC)
    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen, const byte* iv,
                  int dir)
    {
        if (aes == NULL || !((keylen == 16) || (keylen == 24) || (keylen == 32)))
            return BAD_FUNC_ARG;

        aes->rounds = keylen/4 + 6;
        XMEMCPY(aes->key, userKey, keylen);

    #if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_COUNTER) || \
        defined(WOLFSSL_AES_OFB)
        aes->left = 0;
    #endif

        return wc_AesSetIV(aes, iv);
    }

    int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                        const byte* iv, int dir)
    {
        return wc_AesSetKey(aes, userKey, keylen, iv, dir);
    }
#elif defined(FREESCALE_MMCAU)
    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
        const byte* iv, int dir)
    {
        int ret;
        byte* rk;
        byte* tmpKey = (byte*)userKey;
        int tmpKeyDynamic = 0;
        word32 alignOffset = 0;

        (void)dir;

        if (!((keylen == 16) || (keylen == 24) || (keylen == 32)))
            return BAD_FUNC_ARG;
        if (aes == NULL)
            return BAD_FUNC_ARG;

        rk = (byte*)aes->key;
        if (rk == NULL)
            return BAD_FUNC_ARG;

    #if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_COUNTER) || \
        defined(WOLFSSL_AES_OFB)
        aes->left = 0;
    #endif

        aes->rounds = keylen/4 + 6;

    #ifdef FREESCALE_MMCAU_CLASSIC
        if ((wc_ptr_t)userKey % WOLFSSL_MMCAU_ALIGNMENT) {
        #ifndef NO_WOLFSSL_ALLOC_ALIGN
            byte* tmp = (byte*)XMALLOC(keylen + WOLFSSL_MMCAU_ALIGNMENT,
                                       aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
            if (tmp == NULL) {
                return MEMORY_E;
            }
            alignOffset = WOLFSSL_MMCAU_ALIGNMENT -
                          ((wc_ptr_t)tmp % WOLFSSL_MMCAU_ALIGNMENT);
            tmpKey = tmp + alignOffset;
            XMEMCPY(tmpKey, userKey, keylen);
            tmpKeyDynamic = 1;
        #else
            WOLFSSL_MSG("Bad cau_aes_set_key alignment");
            return BAD_ALIGN_E;
        #endif
        }
    #endif

        ret = wolfSSL_CryptHwMutexLock();
        if(ret == 0) {
        #ifdef FREESCALE_MMCAU_CLASSIC
            cau_aes_set_key(tmpKey, keylen*8, rk);
        #else
            MMCAU_AES_SetKey(tmpKey, keylen, rk);
        #endif
            wolfSSL_CryptHwMutexUnLock();

            ret = wc_AesSetIV(aes, iv);
        }

        if (tmpKeyDynamic == 1) {
            XFREE(tmpKey - alignOffset, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
        }

        return ret;
    }

    int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                        const byte* iv, int dir)
    {
        return wc_AesSetKey(aes, userKey, keylen, iv, dir);
    }

#elif defined(WOLFSSL_NRF51_AES)
    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
        const byte* iv, int dir)
    {
        int ret;

        (void)dir;
        (void)iv;

        if (aes == NULL || keylen != 16)
            return BAD_FUNC_ARG;

        aes->keylen = keylen;
        aes->rounds = keylen/4 + 6;
        XMEMCPY(aes->key, userKey, keylen);
        ret = nrf51_aes_set_key(userKey);

    #if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_COUNTER) || \
        defined(WOLFSSL_AES_OFB)
        aes->left = 0;
    #endif

        return ret;
    }

    int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                        const byte* iv, int dir)
    {
        return wc_AesSetKey(aes, userKey, keylen, iv, dir);
    }
#elif defined(WOLFSSL_ESP32_CRYPT) && \
    !defined(NO_WOLFSSL_ESP32_CRYPT_AES)

    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
        const byte* iv, int dir)
    {
        (void)dir;
        (void)iv;

        if (aes == NULL || (keylen != 16 && keylen != 24 && keylen != 32)) {
            return BAD_FUNC_ARG;
        }

    #if !defined(WOLFSSL_AES_128)
        if (keylen == 16) {
            return BAD_FUNC_ARG;
        }
    #endif

    #if !defined(WOLFSSL_AES_192)
        if (keylen == 24) {
            return BAD_FUNC_ARG;
        }
    #endif

    #if !defined(WOLFSSL_AES_256)
        if (keylen == 32) {
            return BAD_FUNC_ARG;
        }
    #endif

        aes->keylen = keylen;
        aes->rounds = keylen/4 + 6;

        XMEMCPY(aes->key, userKey, keylen);
        #if defined(WOLFSSL_AES_COUNTER)
            aes->left = 0;
        #endif
        return wc_AesSetIV(aes, iv);
    }

    int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                        const byte* iv, int dir)
    {
        return wc_AesSetKey(aes, userKey, keylen, iv, dir);
    }
#elif defined(WOLFSSL_CRYPTOCELL) && defined(WOLFSSL_CRYPTOCELL_AES)

    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen, const byte* iv,
                    int dir)
    {
        SaSiError_t ret = SASI_OK;
        SaSiAesIv_t iv_aes;

        if (aes == NULL ||
           (keylen != AES_128_KEY_SIZE &&
            keylen != AES_192_KEY_SIZE &&
            keylen != AES_256_KEY_SIZE)) {
            return BAD_FUNC_ARG;
        }
    #if defined(AES_MAX_KEY_SIZE)
        if (keylen > (AES_MAX_KEY_SIZE/8)) {
            return BAD_FUNC_ARG;
        }
    #endif
        if (dir != AES_ENCRYPTION &&
            dir != AES_DECRYPTION) {
            return BAD_FUNC_ARG;
        }

        if (dir == AES_ENCRYPTION) {
            aes->ctx.mode = SASI_AES_ENCRYPT;
            SaSi_AesInit(&aes->ctx.user_ctx,
                         SASI_AES_ENCRYPT,
                         SASI_AES_MODE_CBC,
                         SASI_AES_PADDING_NONE);
        }
        else {
            aes->ctx.mode = SASI_AES_DECRYPT;
            SaSi_AesInit(&aes->ctx.user_ctx,
                         SASI_AES_DECRYPT,
                         SASI_AES_MODE_CBC,
                         SASI_AES_PADDING_NONE);
        }

        aes->keylen = keylen;
        aes->rounds = keylen/4 + 6;
        XMEMCPY(aes->key, userKey, keylen);

        aes->ctx.key.pKey = (byte*)aes->key;
        aes->ctx.key.keySize= keylen;

        ret = SaSi_AesSetKey(&aes->ctx.user_ctx,
                             SASI_AES_USER_KEY,
                             &aes->ctx.key,
                             sizeof(aes->ctx.key));
        if (ret != SASI_OK) {
            return BAD_FUNC_ARG;
        }

        ret = wc_AesSetIV(aes, iv);

        if (iv)
            XMEMCPY(iv_aes, iv, AES_BLOCK_SIZE);
        else
            XMEMSET(iv_aes,  0, AES_BLOCK_SIZE);


        ret = SaSi_AesSetIv(&aes->ctx.user_ctx, iv_aes);
        if (ret != SASI_OK) {
            return ret;
        }
       return ret;
    }
    #if defined(WOLFSSL_AES_DIRECT)
        int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                            const byte* iv, int dir)
        {
            return wc_AesSetKey(aes, userKey, keylen, iv, dir);
        }
    #endif

#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) \
    && !defined(WOLFSSL_QNX_CAAM)
      /* implemented in wolfcrypt/src/port/caam/caam_aes.c */

#elif defined(WOLFSSL_AFALG)
    /* implemented in wolfcrypt/src/port/af_alg/afalg_aes.c */

#elif defined(WOLFSSL_DEVCRYPTO_AES)
    /* implemented in wolfcrypt/src/port/devcrypto/devcrypto_aes.c */

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    /* implemented in wolfcrypt/src/port/silabs/silabs_aes.c */

#else

    /* Software AES - SetKey */
    static WARN_UNUSED_RESULT int wc_AesSetKeyLocal(
        Aes* aes, const byte* userKey, word32 keylen, const byte* iv, int dir,
        int checkKeyLen)
    {
        int ret;
        word32 *rk;
    #ifdef NEED_AES_TABLES
        word32 temp;
        unsigned int i = 0;
    #endif
    #ifdef WOLFSSL_IMX6_CAAM_BLOB
        byte   local[32];
        word32 localSz = 32;
    #endif

    #ifdef WOLFSSL_MAXQ10XX_CRYPTO
        if (wc_MAXQ10XX_AesSetKey(aes, userKey, keylen) != 0) {
            return WC_HW_E;
        }
    #endif

    #ifdef WOLFSSL_IMX6_CAAM_BLOB
        if (keylen == (16 + WC_CAAM_BLOB_SZ) ||
            keylen == (24 + WC_CAAM_BLOB_SZ) ||
            keylen == (32 + WC_CAAM_BLOB_SZ)) {
            if (wc_caamOpenBlob((byte*)userKey, keylen, local, &localSz) != 0) {
                return BAD_FUNC_ARG;
            }

            /* set local values */
            userKey = local;
            keylen = localSz;
        }
    #endif

    #ifdef WOLFSSL_SECO_CAAM
        /* if set to use hardware than import the key */
        if (aes->devId == WOLFSSL_SECO_DEVID) {
            int keyGroup = 1; /* group one was chosen arbitrarily */
            unsigned int keyIdOut;
            byte importiv[GCM_NONCE_MID_SZ];
            int importivSz = GCM_NONCE_MID_SZ;
            int keyType = 0;
            WC_RNG rng;

            if (wc_InitRng(&rng) != 0) {
                WOLFSSL_MSG("RNG init for IV failed");
                return WC_HW_E;
            }

            if (wc_RNG_GenerateBlock(&rng, importiv, importivSz) != 0) {
                WOLFSSL_MSG("Generate IV failed");
                wc_FreeRng(&rng);
                return WC_HW_E;
            }
            wc_FreeRng(&rng);

            if (iv)
                XMEMCPY(aes->reg, iv, AES_BLOCK_SIZE);
            else
                XMEMSET(aes->reg, 0, AES_BLOCK_SIZE);

            switch (keylen) {
                case AES_128_KEY_SIZE: keyType = CAAM_KEYTYPE_AES128; break;
                case AES_192_KEY_SIZE: keyType = CAAM_KEYTYPE_AES192; break;
                case AES_256_KEY_SIZE: keyType = CAAM_KEYTYPE_AES256; break;
            }

            keyIdOut = wc_SECO_WrapKey(0, (byte*)userKey, keylen, importiv,
                importivSz, keyType, CAAM_KEY_TRANSIENT, keyGroup);
            if (keyIdOut == 0) {
                return WC_HW_E;
            }
            aes->blackKey = keyIdOut;
            return 0;
        }
    #endif

    #if defined(WOLF_CRYPTO_CB) || (defined(WOLFSSL_DEVCRYPTO) && \
        (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))) || \
        (defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES))
        #ifdef WOLF_CRYPTO_CB
        if (aes->devId != INVALID_DEVID)
        #endif
        {
            if (keylen > sizeof(aes->devKey)) {
                return BAD_FUNC_ARG;
            }
            XMEMCPY(aes->devKey, userKey, keylen);
        }
    #endif

        if (checkKeyLen) {
            if (keylen != 16 && keylen != 24 && keylen != 32) {
                return BAD_FUNC_ARG;
            }
        #if defined(AES_MAX_KEY_SIZE) && AES_MAX_KEY_SIZE < 256
            /* Check key length only when AES_MAX_KEY_SIZE doesn't allow
             * all key sizes. Otherwise this condition is never true. */
            if (keylen > (AES_MAX_KEY_SIZE / 8)) {
                return BAD_FUNC_ARG;
            }
        #endif
        }

    #if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_COUNTER) || \
        defined(WOLFSSL_AES_OFB)
        aes->left = 0;
    #endif

        aes->keylen = (int)keylen;
        aes->rounds = (keylen/4) + 6;

    #ifdef WOLFSSL_AESNI
        aes->use_aesni = 0;
        if (checkAESNI == 0) {
            haveAESNI  = Check_CPU_support_AES();
            checkAESNI = 1;
        }
        if (haveAESNI) {
            #ifdef WOLFSSL_LINUXKM
            /* runtime alignment check */
            if ((wc_ptr_t)&aes->key & (wc_ptr_t)0xf) {
                return BAD_ALIGN_E;
            }
            #endif
            aes->use_aesni = 1;
            if (iv)
                XMEMCPY(aes->reg, iv, AES_BLOCK_SIZE);
            else
                XMEMSET(aes->reg, 0, AES_BLOCK_SIZE);
            if (dir == AES_ENCRYPTION)
                return AES_set_encrypt_key(userKey, (int)keylen * 8, aes);
        #ifdef HAVE_AES_DECRYPT
            else
                return AES_set_decrypt_key(userKey, (int)keylen * 8, aes);
        #endif
        }
    #endif /* WOLFSSL_AESNI */

    #ifdef WOLFSSL_KCAPI_AES
        XMEMCPY(aes->devKey, userKey, keylen);
        if (aes->init != 0) {
            kcapi_cipher_destroy(aes->handle);
            aes->handle = NULL;
            aes->init = 0;
        }
        (void)dir;
    #endif

        if (keylen > sizeof(aes->key)) {
            return BAD_FUNC_ARG;
        }
#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
        return wc_psa_aes_set_key(aes, userKey, keylen, (uint8_t*)iv,
                                  ((psa_algorithm_t)0), dir);
#endif

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
        /* wolfSSL HostCrypto in SE05x SDK can request to use SW crypto
         * instead of SE05x crypto by setting useSWCrypt */
        if (aes->useSWCrypt == 0) {
            ret = se050_aes_set_key(aes, userKey, keylen, iv, dir);
            if (ret == 0) {
                ret = wc_AesSetIV(aes, iv);
            }
            return ret;
        }
#endif

        rk = aes->key;
        XMEMCPY(rk, userKey, keylen);
    #if defined(LITTLE_ENDIAN_ORDER) && !defined(WOLFSSL_PIC32MZ_CRYPT) && \
        (!defined(WOLFSSL_ESP32_CRYPT) || \
          defined(NO_WOLFSSL_ESP32_CRYPT_AES))
        ByteReverseWords(rk, rk, keylen);
    #endif

    #ifdef WOLFSSL_IMXRT_DCP
        /* Implemented in wolfcrypt/src/port/nxp/dcp_port.c */
        temp = 0;
        if (keylen == 16)
            temp = DCPAesSetKey(aes, userKey, keylen, iv, dir);
        if (temp != 0)
            return WC_HW_E;
    #endif

#ifdef NEED_AES_TABLES
        switch (keylen) {
    #if defined(AES_MAX_KEY_SIZE) && AES_MAX_KEY_SIZE >= 128 && \
            defined(WOLFSSL_AES_128)
        case 16:
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            temp = (word32)-1;
            wc_MemZero_Add("wc_AesSetKeyLocal temp", &temp, sizeof(temp));
        #endif
            while (1)
            {
                temp  = rk[3];
                rk[4] = rk[0] ^
            #ifndef WOLFSSL_AES_SMALL_TABLES
                    (GetTable(Te[2], GETBYTE(temp, 2)) & 0xff000000) ^
                    (GetTable(Te[3], GETBYTE(temp, 1)) & 0x00ff0000) ^
                    (GetTable(Te[0], GETBYTE(temp, 0)) & 0x0000ff00) ^
                    (GetTable(Te[1], GETBYTE(temp, 3)) & 0x000000ff) ^
            #else
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 2)) << 24) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 1)) << 16) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 0)) <<  8) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 3))) ^
            #endif
                    rcon[i];
                rk[5] = rk[1] ^ rk[4];
                rk[6] = rk[2] ^ rk[5];
                rk[7] = rk[3] ^ rk[6];
                if (++i == 10)
                    break;
                rk += 4;
            }
            break;
    #endif /* 128 */

    #if defined(AES_MAX_KEY_SIZE) && AES_MAX_KEY_SIZE >= 192 && \
            defined(WOLFSSL_AES_192)
        case 24:
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            temp = (word32)-1;
            wc_MemZero_Add("wc_AesSetKeyLocal temp", &temp, sizeof(temp));
        #endif
            /* for (;;) here triggers a bug in VC60 SP4 w/ Pro Pack */
            while (1)
            {
                temp = rk[ 5];
                rk[ 6] = rk[ 0] ^
            #ifndef WOLFSSL_AES_SMALL_TABLES
                    (GetTable(Te[2], GETBYTE(temp, 2)) & 0xff000000) ^
                    (GetTable(Te[3], GETBYTE(temp, 1)) & 0x00ff0000) ^
                    (GetTable(Te[0], GETBYTE(temp, 0)) & 0x0000ff00) ^
                    (GetTable(Te[1], GETBYTE(temp, 3)) & 0x000000ff) ^
            #else
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 2)) << 24) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 1)) << 16) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 0)) <<  8) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 3))) ^
            #endif
                    rcon[i];
                rk[ 7] = rk[ 1] ^ rk[ 6];
                rk[ 8] = rk[ 2] ^ rk[ 7];
                rk[ 9] = rk[ 3] ^ rk[ 8];
                if (++i == 8)
                    break;
                rk[10] = rk[ 4] ^ rk[ 9];
                rk[11] = rk[ 5] ^ rk[10];
                rk += 6;
            }
            break;
    #endif /* 192 */

    #if defined(AES_MAX_KEY_SIZE) && AES_MAX_KEY_SIZE >= 256 && \
            defined(WOLFSSL_AES_256)
        case 32:
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            temp = (word32)-1;
            wc_MemZero_Add("wc_AesSetKeyLocal temp", &temp, sizeof(temp));
        #endif
            while (1)
            {
                temp = rk[ 7];
                rk[ 8] = rk[ 0] ^
            #ifndef WOLFSSL_AES_SMALL_TABLES
                    (GetTable(Te[2], GETBYTE(temp, 2)) & 0xff000000) ^
                    (GetTable(Te[3], GETBYTE(temp, 1)) & 0x00ff0000) ^
                    (GetTable(Te[0], GETBYTE(temp, 0)) & 0x0000ff00) ^
                    (GetTable(Te[1], GETBYTE(temp, 3)) & 0x000000ff) ^
            #else
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 2)) << 24) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 1)) << 16) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 0)) <<  8) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 3))) ^
            #endif
                    rcon[i];
                rk[ 9] = rk[ 1] ^ rk[ 8];
                rk[10] = rk[ 2] ^ rk[ 9];
                rk[11] = rk[ 3] ^ rk[10];
                if (++i == 7)
                    break;
                temp = rk[11];
                rk[12] = rk[ 4] ^
            #ifndef WOLFSSL_AES_SMALL_TABLES
                    (GetTable(Te[2], GETBYTE(temp, 3)) & 0xff000000) ^
                    (GetTable(Te[3], GETBYTE(temp, 2)) & 0x00ff0000) ^
                    (GetTable(Te[0], GETBYTE(temp, 1)) & 0x0000ff00) ^
                    (GetTable(Te[1], GETBYTE(temp, 0)) & 0x000000ff);
            #else
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 3)) << 24) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 2)) << 16) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 1)) <<  8) ^
                    ((word32)GetTable8(Tsbox, GETBYTE(temp, 0)));
            #endif
                rk[13] = rk[ 5] ^ rk[12];
                rk[14] = rk[ 6] ^ rk[13];
                rk[15] = rk[ 7] ^ rk[14];

                rk += 8;
            }
            break;
    #endif /* 256 */

        default:
            return BAD_FUNC_ARG;
        } /* switch */
        ForceZero(&temp, sizeof(temp));

    #if defined(HAVE_AES_DECRYPT)
        if (dir == AES_DECRYPTION) {
            unsigned int j;
            rk = aes->key;

            /* invert the order of the round keys: */
            for (i = 0, j = 4* aes->rounds; i < j; i += 4, j -= 4) {
                temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
                temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
                temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
                temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
            }
            ForceZero(&temp, sizeof(temp));
        #if !defined(WOLFSSL_AES_SMALL_TABLES)
            /* apply the inverse MixColumn transform to all round keys but the
               first and the last: */
            for (i = 1; i < aes->rounds; i++) {
                rk += 4;
                rk[0] =
                    GetTable(Td[0], GetTable(Te[1], GETBYTE(rk[0], 3)) & 0xff) ^
                    GetTable(Td[1], GetTable(Te[1], GETBYTE(rk[0], 2)) & 0xff) ^
                    GetTable(Td[2], GetTable(Te[1], GETBYTE(rk[0], 1)) & 0xff) ^
                    GetTable(Td[3], GetTable(Te[1], GETBYTE(rk[0], 0)) & 0xff);
                rk[1] =
                    GetTable(Td[0], GetTable(Te[1], GETBYTE(rk[1], 3)) & 0xff) ^
                    GetTable(Td[1], GetTable(Te[1], GETBYTE(rk[1], 2)) & 0xff) ^
                    GetTable(Td[2], GetTable(Te[1], GETBYTE(rk[1], 1)) & 0xff) ^
                    GetTable(Td[3], GetTable(Te[1], GETBYTE(rk[1], 0)) & 0xff);
                rk[2] =
                    GetTable(Td[0], GetTable(Te[1], GETBYTE(rk[2], 3)) & 0xff) ^
                    GetTable(Td[1], GetTable(Te[1], GETBYTE(rk[2], 2)) & 0xff) ^
                    GetTable(Td[2], GetTable(Te[1], GETBYTE(rk[2], 1)) & 0xff) ^
                    GetTable(Td[3], GetTable(Te[1], GETBYTE(rk[2], 0)) & 0xff);
                rk[3] =
                    GetTable(Td[0], GetTable(Te[1], GETBYTE(rk[3], 3)) & 0xff) ^
                    GetTable(Td[1], GetTable(Te[1], GETBYTE(rk[3], 2)) & 0xff) ^
                    GetTable(Td[2], GetTable(Te[1], GETBYTE(rk[3], 1)) & 0xff) ^
                    GetTable(Td[3], GetTable(Te[1], GETBYTE(rk[3], 0)) & 0xff);
            }
        #endif
        }
    #else
        (void)dir;
    #endif /* HAVE_AES_DECRYPT */
        (void)temp;
#endif /* NEED_AES_TABLES */

#if defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_AES)
        XMEMCPY((byte*)aes->key, userKey, keylen);
        if (WOLFSSL_SCE_GSCE_HANDLE.p_cfg->endian_flag == CRYPTO_WORD_ENDIAN_BIG) {
            ByteReverseWords(aes->key, aes->key, 32);
        }
#endif

        ret = wc_AesSetIV(aes, iv);

    #if defined(WOLFSSL_DEVCRYPTO) && \
        (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))
        aes->ctx.cfd = -1;
    #endif
    #ifdef WOLFSSL_IMX6_CAAM_BLOB
        ForceZero(local, sizeof(local));
    #endif
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(&temp, sizeof(temp));
    #endif
        return ret;
    }

    int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
        const byte* iv, int dir)
    {
        if (aes == NULL) {
            return BAD_FUNC_ARG;
        }
        if (keylen > sizeof(aes->key)) {
            return BAD_FUNC_ARG;
        }

        return wc_AesSetKeyLocal(aes, userKey, keylen, iv, dir, 1);
    }

    #if defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER)
        /* AES-CTR and AES-DIRECT need to use this for key setup */
        /* This function allows key sizes that are not 128/192/256 bits */
        int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                            const byte* iv, int dir)
        {
            if (aes == NULL) {
                return BAD_FUNC_ARG;
            }
            if (keylen > sizeof(aes->key)) {
                return BAD_FUNC_ARG;
            }

            return wc_AesSetKeyLocal(aes, userKey, keylen, iv, dir, 0);
        }
    #endif /* WOLFSSL_AES_DIRECT || WOLFSSL_AES_COUNTER */
#endif /* wc_AesSetKey block */


/* wc_AesSetIV is shared between software and hardware */
int wc_AesSetIV(Aes* aes, const byte* iv)
{
    if (aes == NULL)
        return BAD_FUNC_ARG;

    if (iv)
        XMEMCPY(aes->reg, iv, AES_BLOCK_SIZE);
    else
        XMEMSET(aes->reg,  0, AES_BLOCK_SIZE);

#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB) || \
    defined(WOLFSSL_AES_OFB) || defined(WOLFSSL_AES_XTS)
    /* Clear any unused bytes from last cipher op. */
    aes->left = 0;
#endif

    return 0;
}

/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT)
    #if defined(HAVE_COLDFIRE_SEC)
        #error "Coldfire SEC doesn't yet support AES direct"

    #elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) && \
        !defined(WOLFSSL_QNX_CAAM)
        /* implemented in wolfcrypt/src/port/caam/caam_aes.c */

    #elif defined(WOLFSSL_AFALG)
        /* implemented in wolfcrypt/src/port/af_alg/afalg_aes.c */

    #elif defined(WOLFSSL_DEVCRYPTO_AES)
        /* implemented in wolfcrypt/src/port/devcrypt/devcrypto_aes.c */

    #elif defined(WOLFSSL_LINUXKM) && defined(WOLFSSL_AESNI)

        WARN_UNUSED_RESULT int wc_AesEncryptDirect(
            Aes* aes, byte* out, const byte* in)
        {
            int ret;
            if (haveAESNI && aes->use_aesni)
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
            ret = wc_AesEncrypt(aes, in, out);
            if (haveAESNI && aes->use_aesni)
                RESTORE_VECTOR_REGISTERS();
            return ret;
        }
        /* vector reg save/restore is explicit in all below calls to
         * wc_Aes{En,De}cryptDirect(), so bypass the public version with a
         * macro.
         */
        #define wc_AesEncryptDirect(aes, out, in) wc_AesEncrypt(aes, in, out)
        #ifdef HAVE_AES_DECRYPT
        /* Allow direct access to one block decrypt */
        WARN_UNUSED_RESULT int wc_AesDecryptDirect(
            Aes* aes, byte* out, const byte* in)
        {
            int ret;
            if (haveAESNI && aes->use_aesni)
                SAVE_VECTOR_REGISTERS(return _svr_ret;);
            ret = wc_AesDecrypt(aes, in, out);
            if (haveAESNI && aes->use_aesni)
                RESTORE_VECTOR_REGISTERS();
            return ret;
        }
        #define wc_AesDecryptDirect(aes, out, in) wc_AesDecrypt(aes, in, out)
        #endif /* HAVE_AES_DECRYPT */

    #else

        /* Allow direct access to one block encrypt */
        int wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in)
        {
            return wc_AesEncrypt(aes, in, out);
        }
        #ifdef HAVE_AES_DECRYPT
        /* Allow direct access to one block decrypt */
        int wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in)
        {
            return wc_AesDecrypt(aes, in, out);
        }
        #endif /* HAVE_AES_DECRYPT */
    #endif /* AES direct block */
#endif /* WOLFSSL_AES_DIRECT */


/* AES-CBC */
#ifdef HAVE_AES_CBC
#if defined(STM32_CRYPTO)

#ifdef WOLFSSL_STM32_CUBEMX
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int ret = 0;
        CRYP_HandleTypeDef hcryp;
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        ret = wc_Stm32_Aes_Init(aes, &hcryp);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

    #if defined(STM32_HAL_V2)
        hcryp.Init.Algorithm  = CRYP_AES_CBC;
        ByteReverseWords(aes->reg, aes->reg, AES_BLOCK_SIZE);
    #elif defined(STM32_CRYPTO_AES_ONLY)
        hcryp.Init.OperatingMode = CRYP_ALGOMODE_ENCRYPT;
        hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_CBC;
        hcryp.Init.KeyWriteFlag  = CRYP_KEY_WRITE_ENABLE;
    #endif
        hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)aes->reg;
        HAL_CRYP_Init(&hcryp);

    #if defined(STM32_HAL_V2)
        ret = HAL_CRYP_Encrypt(&hcryp, (uint32_t*)in, blocks * AES_BLOCK_SIZE,
            (uint32_t*)out, STM32_HAL_TIMEOUT);
    #elif defined(STM32_CRYPTO_AES_ONLY)
        ret = HAL_CRYPEx_AES(&hcryp, (uint8_t*)in, blocks * AES_BLOCK_SIZE,
            out, STM32_HAL_TIMEOUT);
    #else
        ret = HAL_CRYP_AESCBC_Encrypt(&hcryp, (uint8_t*)in,
                                      blocks * AES_BLOCK_SIZE,
                                      out, STM32_HAL_TIMEOUT);
    #endif
        if (ret != HAL_OK) {
            ret = WC_TIMEOUT_E;
        }

        /* store iv for next call */
        XMEMCPY(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

        HAL_CRYP_DeInit(&hcryp);

        wolfSSL_CryptHwMutexUnLock();
        wc_Stm32_Aes_Cleanup();

        return ret;
    }
    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int ret = 0;
        CRYP_HandleTypeDef hcryp;
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        ret = wc_Stm32_Aes_Init(aes, &hcryp);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

        /* if input and output same will overwrite input iv */
        XMEMCPY(aes->tmp, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

    #if defined(STM32_HAL_V2)
        hcryp.Init.Algorithm  = CRYP_AES_CBC;
        ByteReverseWords(aes->reg, aes->reg, AES_BLOCK_SIZE);
    #elif defined(STM32_CRYPTO_AES_ONLY)
        hcryp.Init.OperatingMode = CRYP_ALGOMODE_KEYDERIVATION_DECRYPT;
        hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_CBC;
        hcryp.Init.KeyWriteFlag  = CRYP_KEY_WRITE_ENABLE;
    #endif

        hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)aes->reg;
        HAL_CRYP_Init(&hcryp);

    #if defined(STM32_HAL_V2)
        ret = HAL_CRYP_Decrypt(&hcryp, (uint32_t*)in, blocks * AES_BLOCK_SIZE,
            (uint32_t*)out, STM32_HAL_TIMEOUT);
    #elif defined(STM32_CRYPTO_AES_ONLY)
        ret = HAL_CRYPEx_AES(&hcryp, (uint8_t*)in, blocks * AES_BLOCK_SIZE,
            out, STM32_HAL_TIMEOUT);
    #else
        ret = HAL_CRYP_AESCBC_Decrypt(&hcryp, (uint8_t*)in,
                                      blocks * AES_BLOCK_SIZE,
            out, STM32_HAL_TIMEOUT);
    #endif
        if (ret != HAL_OK) {
            ret = WC_TIMEOUT_E;
        }

        /* store iv for next call */
        XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);

        HAL_CRYP_DeInit(&hcryp);
        wolfSSL_CryptHwMutexUnLock();
        wc_Stm32_Aes_Cleanup();

        return ret;
    }
    #endif /* HAVE_AES_DECRYPT */

#else /* Standard Peripheral Library */
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int ret;
        word32 *iv;
        CRYP_InitTypeDef cryptInit;
        CRYP_KeyInitTypeDef keyInit;
        CRYP_IVInitTypeDef ivInit;
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        ret = wc_Stm32_Aes_Init(aes, &cryptInit, &keyInit);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

        /* reset registers to their default values */
        CRYP_DeInit();

        /* set key */
        CRYP_KeyInit(&keyInit);

        /* set iv */
        iv = aes->reg;
        CRYP_IVStructInit(&ivInit);
        ByteReverseWords(iv, iv, AES_BLOCK_SIZE);
        ivInit.CRYP_IV0Left  = iv[0];
        ivInit.CRYP_IV0Right = iv[1];
        ivInit.CRYP_IV1Left  = iv[2];
        ivInit.CRYP_IV1Right = iv[3];
        CRYP_IVInit(&ivInit);

        /* set direction and mode */
        cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Encrypt;
        cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_CBC;
        CRYP_Init(&cryptInit);

        /* enable crypto processor */
        CRYP_Cmd(ENABLE);

        while (blocks--) {
            /* flush IN/OUT FIFOs */
            CRYP_FIFOFlush();

            CRYP_DataIn(*(uint32_t*)&in[0]);
            CRYP_DataIn(*(uint32_t*)&in[4]);
            CRYP_DataIn(*(uint32_t*)&in[8]);
            CRYP_DataIn(*(uint32_t*)&in[12]);

            /* wait until the complete message has been processed */
            while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

            *(uint32_t*)&out[0]  = CRYP_DataOut();
            *(uint32_t*)&out[4]  = CRYP_DataOut();
            *(uint32_t*)&out[8]  = CRYP_DataOut();
            *(uint32_t*)&out[12] = CRYP_DataOut();

            /* store iv for next call */
            XMEMCPY(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

            sz  -= AES_BLOCK_SIZE;
            in  += AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
        }

        /* disable crypto processor */
        CRYP_Cmd(DISABLE);
        wolfSSL_CryptHwMutexUnLock();
        wc_Stm32_Aes_Cleanup();

        return ret;
    }

    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int ret;
        word32 *iv;
        CRYP_InitTypeDef cryptInit;
        CRYP_KeyInitTypeDef keyInit;
        CRYP_IVInitTypeDef ivInit;
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        ret = wc_Stm32_Aes_Init(aes, &cryptInit, &keyInit);
        if (ret != 0)
            return ret;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }

        /* if input and output same will overwrite input iv */
        XMEMCPY(aes->tmp, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

        /* reset registers to their default values */
        CRYP_DeInit();

        /* set direction and key */
        CRYP_KeyInit(&keyInit);
        cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Decrypt;
        cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_Key;
        CRYP_Init(&cryptInit);

        /* enable crypto processor */
        CRYP_Cmd(ENABLE);

        /* wait until key has been prepared */
        while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

        /* set direction and mode */
        cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Decrypt;
        cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_CBC;
        CRYP_Init(&cryptInit);

        /* set iv */
        iv = aes->reg;
        CRYP_IVStructInit(&ivInit);
        ByteReverseWords(iv, iv, AES_BLOCK_SIZE);
        ivInit.CRYP_IV0Left  = iv[0];
        ivInit.CRYP_IV0Right = iv[1];
        ivInit.CRYP_IV1Left  = iv[2];
        ivInit.CRYP_IV1Right = iv[3];
        CRYP_IVInit(&ivInit);

        /* enable crypto processor */
        CRYP_Cmd(ENABLE);

        while (blocks--) {
            /* flush IN/OUT FIFOs */
            CRYP_FIFOFlush();

            CRYP_DataIn(*(uint32_t*)&in[0]);
            CRYP_DataIn(*(uint32_t*)&in[4]);
            CRYP_DataIn(*(uint32_t*)&in[8]);
            CRYP_DataIn(*(uint32_t*)&in[12]);

            /* wait until the complete message has been processed */
            while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

            *(uint32_t*)&out[0]  = CRYP_DataOut();
            *(uint32_t*)&out[4]  = CRYP_DataOut();
            *(uint32_t*)&out[8]  = CRYP_DataOut();
            *(uint32_t*)&out[12] = CRYP_DataOut();

            /* store iv for next call */
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);

            in  += AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
        }

        /* disable crypto processor */
        CRYP_Cmd(DISABLE);
        wolfSSL_CryptHwMutexUnLock();
        wc_Stm32_Aes_Cleanup();

        return ret;
    }
    #endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_STM32_CUBEMX */

#elif defined(HAVE_COLDFIRE_SEC)
    static WARN_UNUSED_RESULT int wc_AesCbcCrypt(
        Aes* aes, byte* po, const byte* pi, word32 sz, word32 descHeader)
    {
        #ifdef DEBUG_WOLFSSL
            int i; int stat1, stat2; int ret;
        #endif

        int size;
        volatile int v;

        if ((pi == NULL) || (po == NULL))
            return BAD_FUNC_ARG;    /*wrong pointer*/

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif

        wc_LockMutex(&Mutex_AesSEC);

        /* Set descriptor for SEC */
        secDesc->length1 = 0x0;
        secDesc->pointer1 = NULL;

        secDesc->length2 = AES_BLOCK_SIZE;
        secDesc->pointer2 = (byte *)secReg; /* Initial Vector */

        switch(aes->rounds) {
            case 10: secDesc->length3 = 16; break;
            case 12: secDesc->length3 = 24; break;
            case 14: secDesc->length3 = 32; break;
        }
        XMEMCPY(secKey, aes->key, secDesc->length3);

        secDesc->pointer3 = (byte *)secKey;
        secDesc->pointer4 = AESBuffIn;
        secDesc->pointer5 = AESBuffOut;
        secDesc->length6 = 0x0;
        secDesc->pointer6 = NULL;
        secDesc->length7 = 0x0;
        secDesc->pointer7 = NULL;
        secDesc->nextDescriptorPtr = NULL;

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        size = AES_BUFFER_SIZE;
#endif
        while (sz) {
            secDesc->header = descHeader;
            XMEMCPY(secReg, aes->reg, AES_BLOCK_SIZE);
#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
            sz -= AES_BUFFER_SIZE;
#else
            if (sz < AES_BUFFER_SIZE) {
                size = sz;
                sz = 0;
            } else {
                size = AES_BUFFER_SIZE;
                sz -= AES_BUFFER_SIZE;
            }
#endif

            secDesc->length4 = size;
            secDesc->length5 = size;

            XMEMCPY(AESBuffIn, pi, size);
            if(descHeader == SEC_DESC_AES_CBC_DECRYPT) {
                XMEMCPY((void*)aes->tmp, (void*)&(pi[size-AES_BLOCK_SIZE]),
                        AES_BLOCK_SIZE);
            }

            /* Point SEC to the location of the descriptor */
            MCF_SEC_FR0 = (uint32)secDesc;
            /* Initialize SEC and wait for encryption to complete */
            MCF_SEC_CCCR0 = 0x0000001a;
            /* poll SISR to determine when channel is complete */
            v=0;

            while ((secDesc->header>> 24) != 0xff) v++;

            #ifdef DEBUG_WOLFSSL
                ret = MCF_SEC_SISRH;
                stat1 = MCF_SEC_AESSR;
                stat2 = MCF_SEC_AESISR;
                if (ret & 0xe0000000) {
                    db_printf("Aes_Cbc(i=%d):ISRH=%08x, AESSR=%08x, "
                              "AESISR=%08x\n", i, ret, stat1, stat2);
                }
            #endif

            XMEMCPY(po, AESBuffOut, size);

            if (descHeader == SEC_DESC_AES_CBC_ENCRYPT) {
                XMEMCPY((void*)aes->reg, (void*)&(po[size-AES_BLOCK_SIZE]),
                        AES_BLOCK_SIZE);
            } else {
                XMEMCPY((void*)aes->reg, (void*)aes->tmp, AES_BLOCK_SIZE);
            }

            pi += size;
            po += size;
        }

        wc_UnLockMutex(&Mutex_AesSEC);
        return 0;
    }

    int wc_AesCbcEncrypt(Aes* aes, byte* po, const byte* pi, word32 sz)
    {
        return (wc_AesCbcCrypt(aes, po, pi, sz, SEC_DESC_AES_CBC_ENCRYPT));
    }

    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* po, const byte* pi, word32 sz)
    {
        return (wc_AesCbcCrypt(aes, po, pi, sz, SEC_DESC_AES_CBC_DECRYPT));
    }
    #endif /* HAVE_AES_DECRYPT */

#elif defined(FREESCALE_LTC)
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        word32 keySize;
        status_t status;
        byte *iv, *enc_key;
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        iv      = (byte*)aes->reg;
        enc_key = (byte*)aes->key;

        status = wc_AesGetKeySize(aes, &keySize);
        if (status != 0) {
            return status;
        }

        status = wolfSSL_CryptHwMutexLock();
        if (status != 0)
            return status;
        status = LTC_AES_EncryptCbc(LTC_BASE, in, out, blocks * AES_BLOCK_SIZE,
            iv, enc_key, keySize);
        wolfSSL_CryptHwMutexUnLock();

        /* store iv for next call */
        if (status == kStatus_Success) {
            XMEMCPY(iv, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        }

        return (status == kStatus_Success) ? 0 : -1;
    }

    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        word32 keySize;
        status_t status;
        byte* iv, *dec_key;
        byte temp_block[AES_BLOCK_SIZE];
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        iv      = (byte*)aes->reg;
        dec_key = (byte*)aes->key;

        status = wc_AesGetKeySize(aes, &keySize);
        if (status != 0) {
            return status;
        }

        /* get IV for next call */
        XMEMCPY(temp_block, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

        status = wolfSSL_CryptHwMutexLock();
        if (status != 0)
            return status;
        status = LTC_AES_DecryptCbc(LTC_BASE, in, out, blocks * AES_BLOCK_SIZE,
            iv, dec_key, keySize, kLTC_EncryptKey);
        wolfSSL_CryptHwMutexUnLock();

        /* store IV for next call */
        if (status == kStatus_Success) {
            XMEMCPY(iv, temp_block, AES_BLOCK_SIZE);
        }

        return (status == kStatus_Success) ? 0 : -1;
    }
    #endif /* HAVE_AES_DECRYPT */

#elif defined(FREESCALE_MMCAU)
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int offset = 0;
        byte *iv;
        byte temp_block[AES_BLOCK_SIZE];
        word32 blocks = (sz / AES_BLOCK_SIZE);
        int ret;

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        iv = (byte*)aes->reg;

        while (blocks--) {
            XMEMCPY(temp_block, in + offset, AES_BLOCK_SIZE);

            /* XOR block with IV for CBC */
            xorbuf(temp_block, iv, AES_BLOCK_SIZE);

            ret = wc_AesEncrypt(aes, temp_block, out + offset);
            if (ret != 0)
                return ret;

            offset += AES_BLOCK_SIZE;

            /* store IV for next block */
            XMEMCPY(iv, out + offset - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        }

        return 0;
    }
    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int offset = 0;
        byte* iv;
        byte temp_block[AES_BLOCK_SIZE];
        word32 blocks = (sz / AES_BLOCK_SIZE);

#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            return BAD_LENGTH_E;
        }
#endif
        if (blocks == 0)
            return 0;

        iv = (byte*)aes->reg;

        while (blocks--) {
            XMEMCPY(temp_block, in + offset, AES_BLOCK_SIZE);

            wc_AesDecrypt(aes, in + offset, out + offset);

            /* XOR block with IV for CBC */
            xorbuf(out + offset, iv, AES_BLOCK_SIZE);

            /* store IV for next block */
            XMEMCPY(iv, temp_block, AES_BLOCK_SIZE);

            offset += AES_BLOCK_SIZE;
        }

        return 0;
    }
    #endif /* HAVE_AES_DECRYPT */

#elif defined(WOLFSSL_PIC32MZ_CRYPT)

    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int ret;

        if (sz == 0)
            return 0;

        /* hardware fails on input that is not a multiple of AES block size */
        if (sz % AES_BLOCK_SIZE != 0) {
#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
            return BAD_LENGTH_E;
#else
            return BAD_FUNC_ARG;
#endif
        }

        ret = wc_Pic32AesCrypt(
            aes->key, aes->keylen, aes->reg, AES_BLOCK_SIZE,
            out, in, sz, PIC32_ENCRYPTION,
            PIC32_ALGO_AES, PIC32_CRYPTOALGO_RCBC);

        /* store iv for next call */
        if (ret == 0) {
            XMEMCPY(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        }

        return ret;
    }
    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        int ret;
        byte scratch[AES_BLOCK_SIZE];

        if (sz == 0)
            return 0;

        /* hardware fails on input that is not a multiple of AES block size */
        if (sz % AES_BLOCK_SIZE != 0) {
#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
            return BAD_LENGTH_E;
#else
            return BAD_FUNC_ARG;
#endif
        }
        XMEMCPY(scratch, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

        ret = wc_Pic32AesCrypt(
            aes->key, aes->keylen, aes->reg, AES_BLOCK_SIZE,
            out, in, sz, PIC32_DECRYPTION,
            PIC32_ALGO_AES, PIC32_CRYPTOALGO_RCBC);

        /* store iv for next call */
        if (ret == 0) {
            XMEMCPY((byte*)aes->reg, scratch, AES_BLOCK_SIZE);
        }

        return ret;
    }
    #endif /* HAVE_AES_DECRYPT */
#elif defined(WOLFSSL_ESP32_CRYPT) && \
    !defined(NO_WOLFSSL_ESP32_CRYPT_AES)

    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        return wc_esp32AesCbcEncrypt(aes, out, in, sz);
    }
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        return wc_esp32AesCbcDecrypt(aes, out, in, sz);
    }
#elif defined(WOLFSSL_CRYPTOCELL) && defined(WOLFSSL_CRYPTOCELL_AES)
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        return SaSi_AesBlock(&aes->ctx.user_ctx, (uint8_t*)in, sz, out);
    }
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        return SaSi_AesBlock(&aes->ctx.user_ctx, (uint8_t*)in, sz, out);
    }
#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) && \
        !defined(WOLFSSL_QNX_CAAM)
      /* implemented in wolfcrypt/src/port/caam/caam_aes.c */

#elif defined(WOLFSSL_AFALG)
    /* implemented in wolfcrypt/src/port/af_alg/afalg_aes.c */

#elif defined(WOLFSSL_KCAPI_AES) && !defined(WOLFSSL_NO_KCAPI_AES_CBC)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_aes.c */

#elif defined(WOLFSSL_DEVCRYPTO_CBC)
    /* implemented in wolfcrypt/src/port/devcrypt/devcrypto_aes.c */

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    /* implemented in wolfcrypt/src/port/silabs/silabs_aes.c */

#elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    /* implemented in wolfcrypt/src/port/psa/psa_aes.c */

#else

    /* Software AES - CBC Encrypt */
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        word32 blocks;

        if (aes == NULL || out == NULL || in == NULL) {
            return BAD_FUNC_ARG;
        }

        if (sz == 0) {
            return 0;
        }

        blocks = sz / AES_BLOCK_SIZE;
#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
        if (sz % AES_BLOCK_SIZE) {
            WOLFSSL_ERROR_VERBOSE(BAD_LENGTH_E);
            return BAD_LENGTH_E;
        }
#endif

    #ifdef WOLFSSL_IMXRT_DCP
        /* Implemented in wolfcrypt/src/port/nxp/dcp_port.c */
        if (aes->keylen == 16)
            return DCPAesCbcEncrypt(aes, out, in, sz);
    #endif

    #ifdef WOLF_CRYPTO_CB
        #ifndef WOLF_CRYPTO_CB_FIND
        if (aes->devId != INVALID_DEVID)
        #endif
        {
            int crypto_cb_ret = wc_CryptoCb_AesCbcEncrypt(aes, out, in, sz);
            if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
                return crypto_cb_ret;
            /* fall-through when unavailable */
        }
    #endif
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)
        /* if async and byte count above threshold */
        if (aes->asyncDev.marker == WOLFSSL_ASYNC_MARKER_AES &&
                                                sz >= WC_ASYNC_THRESH_AES_CBC) {
        #if defined(HAVE_CAVIUM)
            return NitroxAesCbcEncrypt(aes, out, in, sz);
        #elif defined(HAVE_INTEL_QA)
            return IntelQaSymAesCbcEncrypt(&aes->asyncDev, out, in, sz,
                (const byte*)aes->devKey, aes->keylen,
                (byte*)aes->reg, AES_BLOCK_SIZE);
        #else /* WOLFSSL_ASYNC_CRYPT_SW */
            if (wc_AsyncSwInit(&aes->asyncDev, ASYNC_SW_AES_CBC_ENCRYPT)) {
                WC_ASYNC_SW* sw = &aes->asyncDev.sw;
                sw->aes.aes = aes;
                sw->aes.out = out;
                sw->aes.in = in;
                sw->aes.sz = sz;
                return WC_PENDING_E;
            }
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

    #if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
        /* Implemented in wolfcrypt/src/port/nxp/se050_port.c */
        if (aes->useSWCrypt == 0) {
            return se050_aes_crypt(aes, in, out, sz, AES_ENCRYPTION,
                                   kAlgorithm_SSS_AES_CBC);
        }
    #endif

    #ifdef WOLFSSL_AESNI
        if (haveAESNI) {
            #ifdef DEBUG_AESNI
                printf("about to aes cbc encrypt\n");
                printf("in  = %p\n", in);
                printf("out = %p\n", out);
                printf("aes->key = %p\n", aes->key);
                printf("aes->reg = %p\n", aes->reg);
                printf("aes->rounds = %d\n", aes->rounds);
                printf("sz = %d\n", sz);
            #endif

            /* check alignment, decrypt doesn't need alignment */
            if ((wc_ptr_t)in % AESNI_ALIGN) {
            #ifndef NO_WOLFSSL_ALLOC_ALIGN
                byte* tmp = (byte*)XMALLOC(sz + AES_BLOCK_SIZE + AESNI_ALIGN,
                                            aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
                byte* tmp_align;
                if (tmp == NULL) return MEMORY_E;

                tmp_align = tmp + (AESNI_ALIGN - ((wc_ptr_t)tmp % AESNI_ALIGN));
                XMEMCPY(tmp_align, in, sz);
                SAVE_VECTOR_REGISTERS(XFREE(tmp, aes->heap, DYNAMIC_TYPE_TMP_BUFFER); return _svr_ret;);
                AES_CBC_encrypt(tmp_align, tmp_align, (byte*)aes->reg, sz,
                                             (byte*)aes->key, (int)aes->rounds);
                RESTORE_VECTOR_REGISTERS();
                /* store iv for next call */
                XMEMCPY(aes->reg, tmp_align + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

                XMEMCPY(out, tmp_align, sz);
                XFREE(tmp, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
                return 0;
            #else
                WOLFSSL_MSG("AES-CBC encrypt with bad alignment");
                WOLFSSL_ERROR_VERBOSE(BAD_ALIGN_E);
                return BAD_ALIGN_E;
            #endif
            }

            SAVE_VECTOR_REGISTERS(return _svr_ret;);
            AES_CBC_encrypt(in, out, (byte*)aes->reg, sz, (byte*)aes->key,
                            (int)aes->rounds);
            RESTORE_VECTOR_REGISTERS();
            /* store iv for next call */
            XMEMCPY(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

            return 0;
        }
    #endif

        while (blocks--) {
            int ret;
            xorbuf((byte*)aes->reg, in, AES_BLOCK_SIZE);
            ret = wc_AesEncrypt(aes, (byte*)aes->reg, (byte*)aes->reg);
            if (ret != 0)
                return ret;
            XMEMCPY(out, aes->reg, AES_BLOCK_SIZE);

            out += AES_BLOCK_SIZE;
            in  += AES_BLOCK_SIZE;
        }

        return 0;
    }

    #ifdef HAVE_AES_DECRYPT
    /* Software AES - CBC Decrypt */
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        word32 blocks;

        if (aes == NULL || out == NULL || in == NULL) {
            return BAD_FUNC_ARG;
        }

        if (sz == 0) {
            return 0;
        }

        blocks = sz / AES_BLOCK_SIZE;
        if (sz % AES_BLOCK_SIZE) {
#ifdef WOLFSSL_AES_CBC_LENGTH_CHECKS
            return BAD_LENGTH_E;
#else
            return BAD_FUNC_ARG;
#endif
        }

    #ifdef WOLFSSL_IMXRT_DCP
        /* Implemented in wolfcrypt/src/port/nxp/dcp_port.c */
        if (aes->keylen == 16)
            return DCPAesCbcDecrypt(aes, out, in, sz);
    #endif

    #ifdef WOLF_CRYPTO_CB
        #ifndef WOLF_CRYPTO_CB_FIND
        if (aes->devId != INVALID_DEVID)
        #endif
        {
            int crypto_cb_ret = wc_CryptoCb_AesCbcDecrypt(aes, out, in, sz);
            if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
                return crypto_cb_ret;
            /* fall-through when unavailable */
        }
    #endif
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)
        /* if async and byte count above threshold */
        if (aes->asyncDev.marker == WOLFSSL_ASYNC_MARKER_AES &&
                                                sz >= WC_ASYNC_THRESH_AES_CBC) {
        #if defined(HAVE_CAVIUM)
            return NitroxAesCbcDecrypt(aes, out, in, sz);
        #elif defined(HAVE_INTEL_QA)
            return IntelQaSymAesCbcDecrypt(&aes->asyncDev, out, in, sz,
                (const byte*)aes->devKey, aes->keylen,
                (byte*)aes->reg, AES_BLOCK_SIZE);
        #else /* WOLFSSL_ASYNC_CRYPT_SW */
            if (wc_AsyncSwInit(&aes->asyncDev, ASYNC_SW_AES_CBC_DECRYPT)) {
                WC_ASYNC_SW* sw = &aes->asyncDev.sw;
                sw->aes.aes = aes;
                sw->aes.out = out;
                sw->aes.in = in;
                sw->aes.sz = sz;
                return WC_PENDING_E;
            }
        #endif
        }
    #endif

    #if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
        /* Implemented in wolfcrypt/src/port/nxp/se050_port.c */
        if (aes->useSWCrypt == 0) {
            return se050_aes_crypt(aes, in, out, sz, AES_DECRYPTION,
                                   kAlgorithm_SSS_AES_CBC);
        }
    #endif

    #ifdef WOLFSSL_AESNI
        if (haveAESNI) {
            #ifdef DEBUG_AESNI
                printf("about to aes cbc decrypt\n");
                printf("in  = %p\n", in);
                printf("out = %p\n", out);
                printf("aes->key = %p\n", aes->key);
                printf("aes->reg = %p\n", aes->reg);
                printf("aes->rounds = %d\n", aes->rounds);
                printf("sz = %d\n", sz);
            #endif

            /* if input and output same will overwrite input iv */
            XMEMCPY(aes->tmp, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            SAVE_VECTOR_REGISTERS(return _svr_ret;);
            #if defined(WOLFSSL_AESNI_BY4) || defined(WOLFSSL_X86_BUILD)
            AES_CBC_decrypt_by4(in, out, (byte*)aes->reg, sz, (byte*)aes->key,
                            aes->rounds);
            #elif defined(WOLFSSL_AESNI_BY6)
            AES_CBC_decrypt_by6(in, out, (byte*)aes->reg, sz, (byte*)aes->key,
                            aes->rounds);
            #else /* WOLFSSL_AESNI_BYx */
            AES_CBC_decrypt_by8(in, out, (byte*)aes->reg, sz, (byte*)aes->key,
                            (int)aes->rounds);
            #endif /* WOLFSSL_AESNI_BYx */
            /* store iv for next call */
            RESTORE_VECTOR_REGISTERS();
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);
            return 0;
        }
    #endif

        while (blocks--) {
            int ret;
            XMEMCPY(aes->tmp, in, AES_BLOCK_SIZE);
            ret = wc_AesDecrypt(aes, (byte*)aes->tmp, out);
            if (ret != 0)
                return ret;
            xorbuf(out, (byte*)aes->reg, AES_BLOCK_SIZE);
            /* store iv for next call */
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);

            out += AES_BLOCK_SIZE;
            in  += AES_BLOCK_SIZE;
        }

        return 0;
    }
    #endif /* HAVE_AES_DECRYPT */

#endif /* AES-CBC block */
#endif /* HAVE_AES_CBC */

/* AES-CTR */
#if defined(WOLFSSL_AES_COUNTER)

    #ifdef STM32_CRYPTO
        #define NEED_AES_CTR_SOFT
        #define XTRANSFORM_AESCTRBLOCK wc_AesCtrEncryptBlock

        int wc_AesCtrEncryptBlock(Aes* aes, byte* out, const byte* in)
        {
            int ret = 0;
        #ifdef WOLFSSL_STM32_CUBEMX
            CRYP_HandleTypeDef hcryp;
            #ifdef STM32_HAL_V2
            word32 iv[AES_BLOCK_SIZE/sizeof(word32)];
            #endif
        #else
            word32 *iv;
            CRYP_InitTypeDef cryptInit;
            CRYP_KeyInitTypeDef keyInit;
            CRYP_IVInitTypeDef ivInit;
        #endif

        #ifdef WOLFSSL_STM32_CUBEMX
            ret = wc_Stm32_Aes_Init(aes, &hcryp);
            if (ret != 0) {
                return ret;
            }

            ret = wolfSSL_CryptHwMutexLock();
            if (ret != 0) {
                return ret;
            }

        #if defined(STM32_HAL_V2)
            hcryp.Init.Algorithm  = CRYP_AES_CTR;
            ByteReverseWords(iv, aes->reg, AES_BLOCK_SIZE);
            hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)iv;
        #elif defined(STM32_CRYPTO_AES_ONLY)
            hcryp.Init.OperatingMode = CRYP_ALGOMODE_ENCRYPT;
            hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_CTR;
            hcryp.Init.KeyWriteFlag  = CRYP_KEY_WRITE_ENABLE;
            hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)aes->reg;
        #else
            hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)aes->reg;
        #endif
            HAL_CRYP_Init(&hcryp);

        #if defined(STM32_HAL_V2)
            ret = HAL_CRYP_Encrypt(&hcryp, (uint32_t*)in, AES_BLOCK_SIZE,
                (uint32_t*)out, STM32_HAL_TIMEOUT);
        #elif defined(STM32_CRYPTO_AES_ONLY)
            ret = HAL_CRYPEx_AES(&hcryp, (byte*)in, AES_BLOCK_SIZE,
                out, STM32_HAL_TIMEOUT);
        #else
            ret = HAL_CRYP_AESCTR_Encrypt(&hcryp, (byte*)in, AES_BLOCK_SIZE,
                out, STM32_HAL_TIMEOUT);
        #endif
            if (ret != HAL_OK) {
                ret = WC_TIMEOUT_E;
            }
            HAL_CRYP_DeInit(&hcryp);

        #else /* Standard Peripheral Library */
            ret = wc_Stm32_Aes_Init(aes, &cryptInit, &keyInit);
            if (ret != 0) {
                return ret;
            }

            ret = wolfSSL_CryptHwMutexLock();
            if (ret != 0) {
                return ret;
            }

            /* reset registers to their default values */
            CRYP_DeInit();

            /* set key */
            CRYP_KeyInit(&keyInit);

            /* set iv */
            iv = aes->reg;
            CRYP_IVStructInit(&ivInit);
            ivInit.CRYP_IV0Left  = ByteReverseWord32(iv[0]);
            ivInit.CRYP_IV0Right = ByteReverseWord32(iv[1]);
            ivInit.CRYP_IV1Left  = ByteReverseWord32(iv[2]);
            ivInit.CRYP_IV1Right = ByteReverseWord32(iv[3]);
            CRYP_IVInit(&ivInit);

            /* set direction and mode */
            cryptInit.CRYP_AlgoDir  = CRYP_AlgoDir_Encrypt;
            cryptInit.CRYP_AlgoMode = CRYP_AlgoMode_AES_CTR;
            CRYP_Init(&cryptInit);

            /* enable crypto processor */
            CRYP_Cmd(ENABLE);

            /* flush IN/OUT FIFOs */
            CRYP_FIFOFlush();

            CRYP_DataIn(*(uint32_t*)&in[0]);
            CRYP_DataIn(*(uint32_t*)&in[4]);
            CRYP_DataIn(*(uint32_t*)&in[8]);
            CRYP_DataIn(*(uint32_t*)&in[12]);

            /* wait until the complete message has been processed */
            while (CRYP_GetFlagStatus(CRYP_FLAG_BUSY) != RESET) {}

            *(uint32_t*)&out[0]  = CRYP_DataOut();
            *(uint32_t*)&out[4]  = CRYP_DataOut();
            *(uint32_t*)&out[8]  = CRYP_DataOut();
            *(uint32_t*)&out[12] = CRYP_DataOut();

            /* disable crypto processor */
            CRYP_Cmd(DISABLE);
        #endif /* WOLFSSL_STM32_CUBEMX */

            wolfSSL_CryptHwMutexUnLock();
            wc_Stm32_Aes_Cleanup();
            return ret;
        }


    #elif defined(WOLFSSL_PIC32MZ_CRYPT)

        #define NEED_AES_CTR_SOFT
        #define XTRANSFORM_AESCTRBLOCK wc_AesCtrEncryptBlock

        int wc_AesCtrEncryptBlock(Aes* aes, byte* out, const byte* in)
        {
            word32 tmpIv[AES_BLOCK_SIZE / sizeof(word32)];
            XMEMCPY(tmpIv, aes->reg, AES_BLOCK_SIZE);
            return wc_Pic32AesCrypt(
                aes->key, aes->keylen, tmpIv, AES_BLOCK_SIZE,
                out, in, AES_BLOCK_SIZE,
                PIC32_ENCRYPTION, PIC32_ALGO_AES, PIC32_CRYPTOALGO_RCTR);
        }

    #elif defined(HAVE_COLDFIRE_SEC)
        #error "Coldfire SEC doesn't currently support AES-CTR mode"

    #elif defined(FREESCALE_LTC)
        int wc_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
        {
            int ret = 0;
            word32 keySize;
            byte *iv, *enc_key;
            byte* tmp;

            if (aes == NULL || out == NULL || in == NULL) {
                return BAD_FUNC_ARG;
            }

            /* consume any unused bytes left in aes->tmp */
            tmp = (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left;
            while (aes->left && sz) {
                *(out++) = *(in++) ^ *(tmp++);
                aes->left--;
                sz--;
            }

            if (sz) {
                iv      = (byte*)aes->reg;
                enc_key = (byte*)aes->key;

                ret = wc_AesGetKeySize(aes, &keySize);
                if (ret != 0)
                    return ret;

                ret = wolfSSL_CryptHwMutexLock();
                if (ret != 0)
                    return ret;
                LTC_AES_CryptCtr(LTC_BASE, in, out, sz,
                    iv, enc_key, keySize, (byte*)aes->tmp,
                    (uint32_t*)&aes->left);
                wolfSSL_CryptHwMutexUnLock();
            }

            return ret;
        }

    #elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) && \
        !defined(WOLFSSL_QNX_CAAM)
        /* implemented in wolfcrypt/src/port/caam/caam_aes.c */

    #elif defined(WOLFSSL_AFALG)
        /* implemented in wolfcrypt/src/port/af_alg/afalg_aes.c */

    #elif defined(WOLFSSL_DEVCRYPTO_AES)
        /* implemented in wolfcrypt/src/port/devcrypt/devcrypto_aes.c */

    #elif defined(WOLFSSL_ESP32_CRYPT) && \
        !defined(NO_WOLFSSL_ESP32_CRYPT_AES)
        /* esp32 doesn't support CRT mode by hw.     */
        /* use aes ecnryption plus sw implementation */
        #define NEED_AES_CTR_SOFT

    #elif defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    /* implemented in wolfcrypt/src/port/psa/psa_aes.c */
    #else

        /* Use software based AES counter */
        #define NEED_AES_CTR_SOFT
    #endif

    #ifdef NEED_AES_CTR_SOFT
        /* Increment AES counter */
        static WC_INLINE void IncrementAesCounter(byte* inOutCtr)
        {
            /* in network byte order so start at end and work back */
            int i;
            for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
                if (++inOutCtr[i])  /* we're done unless we overflow */
                    return;
            }
        }

        /* Software AES - CTR Encrypt */
        int wc_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
        {
            byte scratch[AES_BLOCK_SIZE];
            int ret;
            word32 processed;

            if (aes == NULL || out == NULL || in == NULL) {
                return BAD_FUNC_ARG;
            }

        #ifdef WOLF_CRYPTO_CB
            #ifndef WOLF_CRYPTO_CB_FIND
            if (aes->devId != INVALID_DEVID)
            #endif
            {
                int crypto_cb_ret = wc_CryptoCb_AesCtrEncrypt(aes, out, in, sz);
                if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
                    return crypto_cb_ret;
                /* fall-through when unavailable */
            }
        #endif

            /* consume any unused bytes left in aes->tmp */
            processed = min(aes->left, sz);
            xorbufout(out, in, (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left,
                      processed);
            out += processed;
            in += processed;
            aes->left -= processed;
            sz -= processed;

        #if defined(HAVE_AES_ECB) && !defined(WOLFSSL_PIC32MZ_CRYPT) && \
            !defined(XTRANSFORM_AESCTRBLOCK)
            if (in != out && sz >= AES_BLOCK_SIZE) {
                word32 blocks = sz / AES_BLOCK_SIZE;
                byte* counter = (byte*)aes->reg;
                byte* c = out;
                while (blocks--) {
                    XMEMCPY(c, counter, AES_BLOCK_SIZE);
                    c += AES_BLOCK_SIZE;
                    IncrementAesCounter(counter);
                }

                /* reset number of blocks and then do encryption */
                blocks = sz / AES_BLOCK_SIZE;
                wc_AesEcbEncrypt(aes, out, out, AES_BLOCK_SIZE * blocks);
                xorbuf(out, in, AES_BLOCK_SIZE * blocks);
                in += AES_BLOCK_SIZE * blocks;
                out += AES_BLOCK_SIZE * blocks;
                sz -= blocks * AES_BLOCK_SIZE;
            }
            else
        #endif
            {
            #ifdef WOLFSSL_CHECK_MEM_ZERO
                wc_MemZero_Add("wc_AesCtrEncrypt scratch", scratch,
                    AES_BLOCK_SIZE);
            #endif
                /* do as many block size ops as possible */
                while (sz >= AES_BLOCK_SIZE) {
                #ifdef XTRANSFORM_AESCTRBLOCK
                    XTRANSFORM_AESCTRBLOCK(aes, out, in);
                #else
                    ret = wc_AesEncrypt(aes, (byte*)aes->reg, scratch);
                    if (ret != 0) {
                        ForceZero(scratch, AES_BLOCK_SIZE);
                    #ifdef WOLFSSL_CHECK_MEM_ZERO
                        wc_MemZero_Check(scratch, AES_BLOCK_SIZE);
                    #endif
                        return ret;
                    }
                    xorbuf(scratch, in, AES_BLOCK_SIZE);
                    XMEMCPY(out, scratch, AES_BLOCK_SIZE);
                #endif
                    IncrementAesCounter((byte*)aes->reg);

                    out += AES_BLOCK_SIZE;
                    in  += AES_BLOCK_SIZE;
                    sz  -= AES_BLOCK_SIZE;
                    aes->left = 0;
                }
                ForceZero(scratch, AES_BLOCK_SIZE);
            }

            /* handle non block size remaining and store unused byte count in left */
            if (sz) {
                ret = wc_AesEncrypt(aes, (byte*)aes->reg, (byte*)aes->tmp);
                if (ret != 0) {
                    ForceZero(scratch, AES_BLOCK_SIZE);
                #ifdef WOLFSSL_CHECK_MEM_ZERO
                    wc_MemZero_Check(scratch, AES_BLOCK_SIZE);
                #endif
                    return ret;
                }
                IncrementAesCounter((byte*)aes->reg);

                aes->left = AES_BLOCK_SIZE - sz;
                xorbufout(out, in, aes->tmp, sz);
            }

        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(scratch, AES_BLOCK_SIZE);
        #endif
            return 0;
        }

    #endif /* NEED_AES_CTR_SOFT */

#endif /* WOLFSSL_AES_COUNTER */
#endif /* !WOLFSSL_ARMASM */


/*
 * The IV for AES GCM and CCM, stored in struct Aes's member reg, is comprised
 * of two parts in order:
 *   1. The fixed field which may be 0 or 4 bytes long. In TLS, this is set
 *      to the implicit IV.
 *   2. The explicit IV is generated by wolfCrypt. It needs to be managed
 *      by wolfCrypt to ensure the IV is unique for each call to encrypt.
 * The IV may be a 96-bit random value, or the 32-bit fixed value and a
 * 64-bit set of 0 or random data. The final 32-bits of reg is used as a
 * block counter during the encryption.
 */

#if (defined(HAVE_AESGCM) && !defined(WC_NO_RNG)) || defined(HAVE_AESCCM)
static WC_INLINE void IncCtr(byte* ctr, word32 ctrSz)
{
    int i;
    for (i = (int)ctrSz - 1; i >= 0; i--) {
        if (++ctr[i])
            break;
    }
}
#endif /* HAVE_AESGCM || HAVE_AESCCM */


#ifdef HAVE_AESGCM

#ifdef WOLFSSL_AESGCM_STREAM
    /* Access initialization counter data. */
    #define AES_INITCTR(aes)        ((aes)->streamData + 0 * AES_BLOCK_SIZE)
    /* Access counter data. */
    #define AES_COUNTER(aes)        ((aes)->streamData + 1 * AES_BLOCK_SIZE)
    /* Access tag data. */
    #define AES_TAG(aes)            ((aes)->streamData + 2 * AES_BLOCK_SIZE)
    /* Access last GHASH block. */
    #define AES_LASTGBLOCK(aes)     ((aes)->streamData + 3 * AES_BLOCK_SIZE)
    /* Access last encrypted block. */
    #define AES_LASTBLOCK(aes)      ((aes)->streamData + 4 * AES_BLOCK_SIZE)
#endif

#if defined(HAVE_COLDFIRE_SEC)
    #error "Coldfire SEC doesn't currently support AES-GCM mode"

#endif

#ifdef WOLFSSL_ARMASM
    /* implementation is located in wolfcrypt/src/port/arm/armv8-aes.c */

#elif defined(WOLFSSL_AFALG)
    /* implemented in wolfcrypt/src/port/afalg/afalg_aes.c */

#elif defined(WOLFSSL_KCAPI_AES)
    /* implemented in wolfcrypt/src/port/kcapi/kcapi_aes.c */

#elif defined(WOLFSSL_DEVCRYPTO_AES)
    /* implemented in wolfcrypt/src/port/devcrypt/devcrypto_aes.c */

#else /* software + AESNI implementation */

#if !defined(FREESCALE_LTC_AES_GCM)
static WC_INLINE void IncrementGcmCounter(byte* inOutCtr)
{
    int i;

    /* in network byte order so start at end and work back */
    for (i = AES_BLOCK_SIZE - 1; i >= AES_BLOCK_SIZE - CTR_SZ; i--) {
        if (++inOutCtr[i])  /* we're done unless we overflow */
            return;
    }
}
#endif /* !FREESCALE_LTC_AES_GCM */

#if defined(GCM_SMALL) || defined(GCM_TABLE) || defined(GCM_TABLE_4BIT)

static WC_INLINE void FlattenSzInBits(byte* buf, word32 sz)
{
    /* Multiply the sz by 8 */
    word32 szHi = (sz >> (8*sizeof(sz) - 3));
    sz <<= 3;

    /* copy over the words of the sz into the destination buffer */
    buf[0] = (byte)(szHi >> 24);
    buf[1] = (byte)(szHi >> 16);
    buf[2] = (byte)(szHi >>  8);
    buf[3] = (byte)szHi;
    buf[4] = (byte)(sz >> 24);
    buf[5] = (byte)(sz >> 16);
    buf[6] = (byte)(sz >>  8);
    buf[7] = (byte)sz;
}


static WC_INLINE void RIGHTSHIFTX(byte* x)
{
    int i;
    int carryIn = 0;
    byte borrow = (0x00 - (x[15] & 0x01)) & 0xE1;

    for (i = 0; i < AES_BLOCK_SIZE; i++) {
        int carryOut = (x[i] & 0x01) << 7;
        x[i] = (byte) ((x[i] >> 1) | carryIn);
        carryIn = carryOut;
    }
    x[0] ^= borrow;
}

#endif /* defined(GCM_SMALL) || defined(GCM_TABLE) || defined(GCM_TABLE_4BIT) */


#ifdef GCM_TABLE

void GenerateM0(Gcm* gcm)
{
    int i, j;
    byte (*m)[AES_BLOCK_SIZE] = gcm->M0;

    XMEMCPY(m[128], gcm->H, AES_BLOCK_SIZE);

    for (i = 64; i > 0; i /= 2) {
        XMEMCPY(m[i], m[i*2], AES_BLOCK_SIZE);
        RIGHTSHIFTX(m[i]);
    }

    for (i = 2; i < 256; i *= 2) {
        for (j = 1; j < i; j++) {
            XMEMCPY(m[i+j], m[i], AES_BLOCK_SIZE);
            xorbuf(m[i+j], m[j], AES_BLOCK_SIZE);
        }
    }

    XMEMSET(m[0], 0, AES_BLOCK_SIZE);
}

#elif defined(GCM_TABLE_4BIT)

#if !defined(BIG_ENDIAN_ORDER) && !defined(WC_16BIT_CPU)
static WC_INLINE void Shift4_M0(byte *r8, byte *z8)
{
    int i;
    for (i = 15; i > 0; i--)
        r8[i] = (byte)(z8[i-1] << 4) | (byte)(z8[i] >> 4);
    r8[0] = (byte)(z8[0] >> 4);
}
#endif

void GenerateM0(Gcm* gcm)
{
#if !defined(BIG_ENDIAN_ORDER) && !defined(WC_16BIT_CPU)
    int i;
#endif
    byte (*m)[AES_BLOCK_SIZE] = gcm->M0;

    /* 0 times -> 0x0 */
    XMEMSET(m[0x0], 0, AES_BLOCK_SIZE);
    /* 1 times -> 0x8 */
    XMEMCPY(m[0x8], gcm->H, AES_BLOCK_SIZE);
    /* 2 times -> 0x4 */
    XMEMCPY(m[0x4], m[0x8], AES_BLOCK_SIZE);
    RIGHTSHIFTX(m[0x4]);
    /* 4 times -> 0x2 */
    XMEMCPY(m[0x2], m[0x4], AES_BLOCK_SIZE);
    RIGHTSHIFTX(m[0x2]);
    /* 8 times -> 0x1 */
    XMEMCPY(m[0x1], m[0x2], AES_BLOCK_SIZE);
    RIGHTSHIFTX(m[0x1]);

    /* 0x3 */
    XMEMCPY(m[0x3], m[0x2], AES_BLOCK_SIZE);
    xorbuf (m[0x3], m[0x1], AES_BLOCK_SIZE);

    /* 0x5 -> 0x7 */
    XMEMCPY(m[0x5], m[0x4], AES_BLOCK_SIZE);
    xorbuf (m[0x5], m[0x1], AES_BLOCK_SIZE);
    XMEMCPY(m[0x6], m[0x4], AES_BLOCK_SIZE);
    xorbuf (m[0x6], m[0x2], AES_BLOCK_SIZE);
    XMEMCPY(m[0x7], m[0x4], AES_BLOCK_SIZE);
    xorbuf (m[0x7], m[0x3], AES_BLOCK_SIZE);

    /* 0x9 -> 0xf */
    XMEMCPY(m[0x9], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0x9], m[0x1], AES_BLOCK_SIZE);
    XMEMCPY(m[0xa], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0xa], m[0x2], AES_BLOCK_SIZE);
    XMEMCPY(m[0xb], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0xb], m[0x3], AES_BLOCK_SIZE);
    XMEMCPY(m[0xc], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0xc], m[0x4], AES_BLOCK_SIZE);
    XMEMCPY(m[0xd], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0xd], m[0x5], AES_BLOCK_SIZE);
    XMEMCPY(m[0xe], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0xe], m[0x6], AES_BLOCK_SIZE);
    XMEMCPY(m[0xf], m[0x8], AES_BLOCK_SIZE);
    xorbuf (m[0xf], m[0x7], AES_BLOCK_SIZE);

#if !defined(BIG_ENDIAN_ORDER) && !defined(WC_16BIT_CPU)
    for (i = 0; i < 16; i++) {
        Shift4_M0(m[16+i], m[i]);
    }
#endif
}

#endif /* GCM_TABLE */

/* Software AES - GCM SetKey */
int wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len)
{
    int  ret;
    byte iv[AES_BLOCK_SIZE];

    #ifdef WOLFSSL_IMX6_CAAM_BLOB
        byte   local[32];
        word32 localSz = 32;

        if (len == (16 + WC_CAAM_BLOB_SZ) ||
          len == (24 + WC_CAAM_BLOB_SZ) ||
          len == (32 + WC_CAAM_BLOB_SZ)) {
            if (wc_caamOpenBlob((byte*)key, len, local, &localSz) != 0) {
                 return BAD_FUNC_ARG;
            }

            /* set local values */
            key = local;
            len = localSz;
        }
    #endif

    if (!((len == 16) || (len == 24) || (len == 32)))
        return BAD_FUNC_ARG;
    if (aes == NULL) {
#ifdef WOLFSSL_IMX6_CAAM_BLOB
        ForceZero(local, sizeof(local));
#endif
        return BAD_FUNC_ARG;
    }

#ifdef OPENSSL_EXTRA
    XMEMSET(aes->gcm.aadH, 0, sizeof(aes->gcm.aadH));
    aes->gcm.aadLen = 0;
#endif
    XMEMSET(iv, 0, AES_BLOCK_SIZE);
    ret = wc_AesSetKey(aes, key, len, iv, AES_ENCRYPTION);
#ifdef WOLFSSL_AESGCM_STREAM
    aes->gcmKeySet = 1;
#endif

    #ifdef WOLFSSL_AESNI
        /* AES-NI code generates its own H value. */
        if (haveAESNI)
            return ret;
    #endif /* WOLFSSL_AESNI */
    #if defined(WOLFSSL_SECO_CAAM)
        if (aes->devId == WOLFSSL_SECO_DEVID) {
            return ret;
        }
    #endif /* WOLFSSL_SECO_CAAM */

#if !defined(FREESCALE_LTC_AES_GCM)
    if (ret == 0)
        ret = wc_AesEncrypt(aes, iv, aes->gcm.H);
    if (ret == 0) {
    #if defined(GCM_TABLE) || defined(GCM_TABLE_4BIT)
        GenerateM0(&aes->gcm);
    #endif /* GCM_TABLE */
    }
#endif /* FREESCALE_LTC_AES_GCM */

#if defined(WOLFSSL_XILINX_CRYPT) || defined(WOLFSSL_AFALG_XILINX_AES)
    wc_AesGcmSetKey_ex(aes, key, len, WOLFSSL_XILINX_AES_KEY_SRC);
#endif

#ifdef WOLF_CRYPTO_CB
    if (aes->devId != INVALID_DEVID) {
        XMEMCPY(aes->devKey, key, len);
    }
#endif

#ifdef WOLFSSL_IMX6_CAAM_BLOB
    ForceZero(local, sizeof(local));
#endif
    return ret;
}


#ifdef WOLFSSL_AESNI

#if defined(USE_INTEL_SPEEDUP)
    #define HAVE_INTEL_AVX1
    #define HAVE_INTEL_AVX2
#endif /* USE_INTEL_SPEEDUP */

void AES_GCM_encrypt(const unsigned char *in, unsigned char *out,
                     const unsigned char* addt, const unsigned char* ivec,
                     unsigned char *tag, word32 nbytes,
                     word32 abytes, word32 ibytes,
                     word32 tbytes, const unsigned char* key, int nr)
                     XASM_LINK("AES_GCM_encrypt");
#ifdef HAVE_INTEL_AVX1
void AES_GCM_encrypt_avx1(const unsigned char *in, unsigned char *out,
                          const unsigned char* addt, const unsigned char* ivec,
                          unsigned char *tag, word32 nbytes,
                          word32 abytes, word32 ibytes,
                          word32 tbytes, const unsigned char* key,
                          int nr)
                          XASM_LINK("AES_GCM_encrypt_avx1");
#ifdef HAVE_INTEL_AVX2
void AES_GCM_encrypt_avx2(const unsigned char *in, unsigned char *out,
                          const unsigned char* addt, const unsigned char* ivec,
                          unsigned char *tag, word32 nbytes,
                          word32 abytes, word32 ibytes,
                          word32 tbytes, const unsigned char* key,
                          int nr)
                          XASM_LINK("AES_GCM_encrypt_avx2");
#endif /* HAVE_INTEL_AVX2 */
#endif /* HAVE_INTEL_AVX1 */

#ifdef HAVE_AES_DECRYPT
void AES_GCM_decrypt(const unsigned char *in, unsigned char *out,
                     const unsigned char* addt, const unsigned char* ivec,
                     const unsigned char *tag, word32 nbytes, word32 abytes,
                     word32 ibytes, word32 tbytes, const unsigned char* key,
                     int nr, int* res)
                     XASM_LINK("AES_GCM_decrypt");
#ifdef HAVE_INTEL_AVX1
void AES_GCM_decrypt_avx1(const unsigned char *in, unsigned char *out,
                          const unsigned char* addt, const unsigned char* ivec,
                          const unsigned char *tag, word32 nbytes,
                          word32 abytes, word32 ibytes, word32 tbytes,
                          const unsigned char* key, int nr, int* res)
                          XASM_LINK("AES_GCM_decrypt_avx1");
#ifdef HAVE_INTEL_AVX2
void AES_GCM_decrypt_avx2(const unsigned char *in, unsigned char *out,
                          const unsigned char* addt, const unsigned char* ivec,
                          const unsigned char *tag, word32 nbytes,
                          word32 abytes, word32 ibytes, word32 tbytes,
                          const unsigned char* key, int nr, int* res)
                          XASM_LINK("AES_GCM_decrypt_avx2");
#endif /* HAVE_INTEL_AVX2 */
#endif /* HAVE_INTEL_AVX1 */
#endif /* HAVE_AES_DECRYPT */

#endif /* WOLFSSL_AESNI */

#if defined(GCM_SMALL)
static void GMULT(byte* X, byte* Y)
{
    byte Z[AES_BLOCK_SIZE];
    byte V[AES_BLOCK_SIZE];
    int i, j;

    XMEMSET(Z, 0, AES_BLOCK_SIZE);
    XMEMCPY(V, X, AES_BLOCK_SIZE);
    for (i = 0; i < AES_BLOCK_SIZE; i++)
    {
        byte y = Y[i];
        for (j = 0; j < 8; j++)
        {
            if (y & 0x80) {
                xorbuf(Z, V, AES_BLOCK_SIZE);
            }

            RIGHTSHIFTX(V);
            y = y << 1;
        }
    }
    XMEMCPY(X, Z, AES_BLOCK_SIZE);
}


void GHASH(Gcm* gcm, const byte* a, word32 aSz, const byte* c,
    word32 cSz, byte* s, word32 sSz)
{
    byte x[AES_BLOCK_SIZE];
    byte scratch[AES_BLOCK_SIZE];
    word32 blocks, partial;
    byte* h;

    if (gcm == NULL) {
        return;
    }

    h = gcm->H;
    XMEMSET(x, 0, AES_BLOCK_SIZE);

    /* Hash in A, the Additional Authentication Data */
    if (aSz != 0 && a != NULL) {
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        while (blocks--) {
            xorbuf(x, a, AES_BLOCK_SIZE);
            GMULT(x, h);
            a += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(scratch, 0, AES_BLOCK_SIZE);
            XMEMCPY(scratch, a, partial);
            xorbuf(x, scratch, AES_BLOCK_SIZE);
            GMULT(x, h);
        }
    }

    /* Hash in C, the Ciphertext */
    if (cSz != 0 && c != NULL) {
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        while (blocks--) {
            xorbuf(x, c, AES_BLOCK_SIZE);
            GMULT(x, h);
            c += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(scratch, 0, AES_BLOCK_SIZE);
            XMEMCPY(scratch, c, partial);
            xorbuf(x, scratch, AES_BLOCK_SIZE);
            GMULT(x, h);
        }
    }

    /* Hash in the lengths of A and C in bits */
    FlattenSzInBits(&scratch[0], aSz);
    FlattenSzInBits(&scratch[8], cSz);
    xorbuf(x, scratch, AES_BLOCK_SIZE);
    GMULT(x, h);

    /* Copy the result into s. */
    XMEMCPY(s, x, sSz);
}

#ifdef WOLFSSL_AESGCM_STREAM
/* No extra initialization for small implementation.
 *
 * @param [in] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes) WC_DO_NOTHING

/* GHASH one block of data..
 *
 * XOR block into tag and GMULT with H.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                     \
    do {                                                \
        xorbuf(AES_TAG(aes), block, AES_BLOCK_SIZE);    \
        GMULT(AES_TAG(aes), aes->gcm.H);                \
    }                                                   \
    while (0)
#endif /* WOLFSSL_AESGCM_STREAM */
/* end GCM_SMALL */
#elif defined(GCM_TABLE)

static const byte R[256][2] = {
    {0x00, 0x00}, {0x01, 0xc2}, {0x03, 0x84}, {0x02, 0x46},
    {0x07, 0x08}, {0x06, 0xca}, {0x04, 0x8c}, {0x05, 0x4e},
    {0x0e, 0x10}, {0x0f, 0xd2}, {0x0d, 0x94}, {0x0c, 0x56},
    {0x09, 0x18}, {0x08, 0xda}, {0x0a, 0x9c}, {0x0b, 0x5e},
    {0x1c, 0x20}, {0x1d, 0xe2}, {0x1f, 0xa4}, {0x1e, 0x66},
    {0x1b, 0x28}, {0x1a, 0xea}, {0x18, 0xac}, {0x19, 0x6e},
    {0x12, 0x30}, {0x13, 0xf2}, {0x11, 0xb4}, {0x10, 0x76},
    {0x15, 0x38}, {0x14, 0xfa}, {0x16, 0xbc}, {0x17, 0x7e},
    {0x38, 0x40}, {0x39, 0x82}, {0x3b, 0xc4}, {0x3a, 0x06},
    {0x3f, 0x48}, {0x3e, 0x8a}, {0x3c, 0xcc}, {0x3d, 0x0e},
    {0x36, 0x50}, {0x37, 0x92}, {0x35, 0xd4}, {0x34, 0x16},
    {0x31, 0x58}, {0x30, 0x9a}, {0x32, 0xdc}, {0x33, 0x1e},
    {0x24, 0x60}, {0x25, 0xa2}, {0x27, 0xe4}, {0x26, 0x26},
    {0x23, 0x68}, {0x22, 0xaa}, {0x20, 0xec}, {0x21, 0x2e},
    {0x2a, 0x70}, {0x2b, 0xb2}, {0x29, 0xf4}, {0x28, 0x36},
    {0x2d, 0x78}, {0x2c, 0xba}, {0x2e, 0xfc}, {0x2f, 0x3e},
    {0x70, 0x80}, {0x71, 0x42}, {0x73, 0x04}, {0x72, 0xc6},
    {0x77, 0x88}, {0x76, 0x4a}, {0x74, 0x0c}, {0x75, 0xce},
    {0x7e, 0x90}, {0x7f, 0x52}, {0x7d, 0x14}, {0x7c, 0xd6},
    {0x79, 0x98}, {0x78, 0x5a}, {0x7a, 0x1c}, {0x7b, 0xde},
    {0x6c, 0xa0}, {0x6d, 0x62}, {0x6f, 0x24}, {0x6e, 0xe6},
    {0x6b, 0xa8}, {0x6a, 0x6a}, {0x68, 0x2c}, {0x69, 0xee},
    {0x62, 0xb0}, {0x63, 0x72}, {0x61, 0x34}, {0x60, 0xf6},
    {0x65, 0xb8}, {0x64, 0x7a}, {0x66, 0x3c}, {0x67, 0xfe},
    {0x48, 0xc0}, {0x49, 0x02}, {0x4b, 0x44}, {0x4a, 0x86},
    {0x4f, 0xc8}, {0x4e, 0x0a}, {0x4c, 0x4c}, {0x4d, 0x8e},
    {0x46, 0xd0}, {0x47, 0x12}, {0x45, 0x54}, {0x44, 0x96},
    {0x41, 0xd8}, {0x40, 0x1a}, {0x42, 0x5c}, {0x43, 0x9e},
    {0x54, 0xe0}, {0x55, 0x22}, {0x57, 0x64}, {0x56, 0xa6},
    {0x53, 0xe8}, {0x52, 0x2a}, {0x50, 0x6c}, {0x51, 0xae},
    {0x5a, 0xf0}, {0x5b, 0x32}, {0x59, 0x74}, {0x58, 0xb6},
    {0x5d, 0xf8}, {0x5c, 0x3a}, {0x5e, 0x7c}, {0x5f, 0xbe},
    {0xe1, 0x00}, {0xe0, 0xc2}, {0xe2, 0x84}, {0xe3, 0x46},
    {0xe6, 0x08}, {0xe7, 0xca}, {0xe5, 0x8c}, {0xe4, 0x4e},
    {0xef, 0x10}, {0xee, 0xd2}, {0xec, 0x94}, {0xed, 0x56},
    {0xe8, 0x18}, {0xe9, 0xda}, {0xeb, 0x9c}, {0xea, 0x5e},
    {0xfd, 0x20}, {0xfc, 0xe2}, {0xfe, 0xa4}, {0xff, 0x66},
    {0xfa, 0x28}, {0xfb, 0xea}, {0xf9, 0xac}, {0xf8, 0x6e},
    {0xf3, 0x30}, {0xf2, 0xf2}, {0xf0, 0xb4}, {0xf1, 0x76},
    {0xf4, 0x38}, {0xf5, 0xfa}, {0xf7, 0xbc}, {0xf6, 0x7e},
    {0xd9, 0x40}, {0xd8, 0x82}, {0xda, 0xc4}, {0xdb, 0x06},
    {0xde, 0x48}, {0xdf, 0x8a}, {0xdd, 0xcc}, {0xdc, 0x0e},
    {0xd7, 0x50}, {0xd6, 0x92}, {0xd4, 0xd4}, {0xd5, 0x16},
    {0xd0, 0x58}, {0xd1, 0x9a}, {0xd3, 0xdc}, {0xd2, 0x1e},
    {0xc5, 0x60}, {0xc4, 0xa2}, {0xc6, 0xe4}, {0xc7, 0x26},
    {0xc2, 0x68}, {0xc3, 0xaa}, {0xc1, 0xec}, {0xc0, 0x2e},
    {0xcb, 0x70}, {0xca, 0xb2}, {0xc8, 0xf4}, {0xc9, 0x36},
    {0xcc, 0x78}, {0xcd, 0xba}, {0xcf, 0xfc}, {0xce, 0x3e},
    {0x91, 0x80}, {0x90, 0x42}, {0x92, 0x04}, {0x93, 0xc6},
    {0x96, 0x88}, {0x97, 0x4a}, {0x95, 0x0c}, {0x94, 0xce},
    {0x9f, 0x90}, {0x9e, 0x52}, {0x9c, 0x14}, {0x9d, 0xd6},
    {0x98, 0x98}, {0x99, 0x5a}, {0x9b, 0x1c}, {0x9a, 0xde},
    {0x8d, 0xa0}, {0x8c, 0x62}, {0x8e, 0x24}, {0x8f, 0xe6},
    {0x8a, 0xa8}, {0x8b, 0x6a}, {0x89, 0x2c}, {0x88, 0xee},
    {0x83, 0xb0}, {0x82, 0x72}, {0x80, 0x34}, {0x81, 0xf6},
    {0x84, 0xb8}, {0x85, 0x7a}, {0x87, 0x3c}, {0x86, 0xfe},
    {0xa9, 0xc0}, {0xa8, 0x02}, {0xaa, 0x44}, {0xab, 0x86},
    {0xae, 0xc8}, {0xaf, 0x0a}, {0xad, 0x4c}, {0xac, 0x8e},
    {0xa7, 0xd0}, {0xa6, 0x12}, {0xa4, 0x54}, {0xa5, 0x96},
    {0xa0, 0xd8}, {0xa1, 0x1a}, {0xa3, 0x5c}, {0xa2, 0x9e},
    {0xb5, 0xe0}, {0xb4, 0x22}, {0xb6, 0x64}, {0xb7, 0xa6},
    {0xb2, 0xe8}, {0xb3, 0x2a}, {0xb1, 0x6c}, {0xb0, 0xae},
    {0xbb, 0xf0}, {0xba, 0x32}, {0xb8, 0x74}, {0xb9, 0xb6},
    {0xbc, 0xf8}, {0xbd, 0x3a}, {0xbf, 0x7c}, {0xbe, 0xbe} };


static void GMULT(byte *x, byte m[256][AES_BLOCK_SIZE])
{
#if !defined(WORD64_AVAILABLE) || defined(BIG_ENDIAN_ORDER)
    int i, j;
    byte Z[AES_BLOCK_SIZE];
    byte a;

    XMEMSET(Z, 0, sizeof(Z));

    for (i = 15; i > 0; i--) {
        xorbuf(Z, m[x[i]], AES_BLOCK_SIZE);
        a = Z[15];

        for (j = 15; j > 0; j--) {
            Z[j] = Z[j-1];
        }

        Z[0]  = R[a][0];
        Z[1] ^= R[a][1];
    }
    xorbuf(Z, m[x[0]], AES_BLOCK_SIZE);

    XMEMCPY(x, Z, AES_BLOCK_SIZE);
#else
    byte Z[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    byte a;
    word64* pZ;
    word64* pm;
    word64* px = (word64*)(x);
    int i;

    pZ = (word64*)(Z + 15 + 1);
    pm = (word64*)(m[x[15]]);
    pZ[0] = pm[0];
    pZ[1] = pm[1];
    a = Z[16 + 15];
    Z[15]  = R[a][0];
    Z[16] ^= R[a][1];
    for (i = 14; i > 0; i--) {
        pZ = (word64*)(Z + i + 1);
        pm = (word64*)(m[x[i]]);
        pZ[0] ^= pm[0];
        pZ[1] ^= pm[1];
        a = Z[16 + i];
        Z[i]    = R[a][0];
        Z[i+1] ^= R[a][1];
    }
    pZ = (word64*)(Z + 1);
    pm = (word64*)(m[x[0]]);
    px[0] = pZ[0] ^ pm[0]; px[1] = pZ[1] ^ pm[1];
#endif
}

void GHASH(Gcm* gcm, const byte* a, word32 aSz, const byte* c,
    word32 cSz, byte* s, word32 sSz)
{
    byte x[AES_BLOCK_SIZE];
    byte scratch[AES_BLOCK_SIZE];
    word32 blocks, partial;

    if (gcm == NULL) {
        return;
    }

    XMEMSET(x, 0, AES_BLOCK_SIZE);

    /* Hash in A, the Additional Authentication Data */
    if (aSz != 0 && a != NULL) {
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        while (blocks--) {
            xorbuf(x, a, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
            a += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(scratch, 0, AES_BLOCK_SIZE);
            XMEMCPY(scratch, a, partial);
            xorbuf(x, scratch, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
        }
    }

    /* Hash in C, the Ciphertext */
    if (cSz != 0 && c != NULL) {
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        while (blocks--) {
            xorbuf(x, c, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
            c += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(scratch, 0, AES_BLOCK_SIZE);
            XMEMCPY(scratch, c, partial);
            xorbuf(x, scratch, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
        }
    }

    /* Hash in the lengths of A and C in bits */
    FlattenSzInBits(&scratch[0], aSz);
    FlattenSzInBits(&scratch[8], cSz);
    xorbuf(x, scratch, AES_BLOCK_SIZE);
    GMULT(x, gcm->M0);

    /* Copy the result into s. */
    XMEMCPY(s, x, sSz);
}

#ifdef WOLFSSL_AESGCM_STREAM
/* No extra initialization for table implementation.
 *
 * @param [in] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes) WC_DO_NOTHING

/* GHASH one block of data..
 *
 * XOR block into tag and GMULT with H using pre-computed table.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                     \
    do {                                                \
        xorbuf(AES_TAG(aes), block, AES_BLOCK_SIZE);    \
        GMULT(AES_TAG(aes), aes->gcm.M0);               \
    }                                                   \
    while (0)
#endif /* WOLFSSL_AESGCM_STREAM */
/* end GCM_TABLE */
#elif defined(GCM_TABLE_4BIT)

/* remainder = x^7 + x^2 + x^1 + 1 => 0xe1
 *  R shifts right a reverse bit pair of bytes such that:
 *     R(b0, b1) => b1 = (b1 >> 1) | (b0 << 7); b0 >>= 1
 *  0 => 0, 0, 0, 0 => R(R(R(00,00) ^ 00,00) ^ 00,00) ^ 00,00 = 00,00
 *  8 => 0, 0, 0, 1 => R(R(R(00,00) ^ 00,00) ^ 00,00) ^ e1,00 = e1,00
 *  4 => 0, 0, 1, 0 => R(R(R(00,00) ^ 00,00) ^ e1,00) ^ 00,00 = 70,80
 *  2 => 0, 1, 0, 0 => R(R(R(00,00) ^ e1,00) ^ 00,00) ^ 00,00 = 38,40
 *  1 => 1, 0, 0, 0 => R(R(R(e1,00) ^ 00,00) ^ 00,00) ^ 00,00 = 1c,20
 *  To calculate te rest, XOR result for each bit.
 *   e.g. 6 = 4 ^ 2 => 48,c0
 *
 * Second half is same values rotated by 4-bits.
 */
#if defined(BIG_ENDIAN_ORDER) || defined(WC_16BIT_CPU)
static const byte R[16][2] = {
    {0x00, 0x00}, {0x1c, 0x20}, {0x38, 0x40}, {0x24, 0x60},
    {0x70, 0x80}, {0x6c, 0xa0}, {0x48, 0xc0}, {0x54, 0xe0},
    {0xe1, 0x00}, {0xfd, 0x20}, {0xd9, 0x40}, {0xc5, 0x60},
    {0x91, 0x80}, {0x8d, 0xa0}, {0xa9, 0xc0}, {0xb5, 0xe0},
};
#else
static const word16 R[32] = {
          0x0000,       0x201c,       0x4038,       0x6024,
          0x8070,       0xa06c,       0xc048,       0xe054,
          0x00e1,       0x20fd,       0x40d9,       0x60c5,
          0x8091,       0xa08d,       0xc0a9,       0xe0b5,

          0x0000,       0xc201,       0x8403,       0x4602,
          0x0807,       0xca06,       0x8c04,       0x4e05,
          0x100e,       0xd20f,       0x940d,       0x560c,
          0x1809,       0xda08,       0x9c0a,       0x5e0b,
};
#endif

/* Multiply in GF(2^128) defined by polynomial:
 *   x^128 + x^7 + x^2 + x^1 + 1.
 *
 * H: hash key = encrypt(key, 0)
 * x = x * H in field
 *
 * x: cumulative result
 * m: 4-bit table
 *    [0..15] * H
 */
#if defined(BIG_ENDIAN_ORDER) || defined(WC_16BIT_CPU)
static void GMULT(byte *x, byte m[16][AES_BLOCK_SIZE])
{
    int i, j, n;
    byte Z[AES_BLOCK_SIZE];
    byte a;

    XMEMSET(Z, 0, sizeof(Z));

    for (i = 15; i >= 0; i--) {
        for (n = 0; n < 2; n++) {
            if (n == 0)
                xorbuf(Z, m[x[i] & 0xf], AES_BLOCK_SIZE);
            else {
                xorbuf(Z, m[x[i] >> 4], AES_BLOCK_SIZE);
                if (i == 0)
                    break;
            }
            a = Z[15] & 0xf;

            for (j = 15; j > 0; j--)
                Z[j] = (Z[j-1] << 4) | (Z[j] >> 4);
            Z[0] >>= 4;

            Z[0] ^= R[a][0];
            Z[1] ^= R[a][1];
        }
    }

    XMEMCPY(x, Z, AES_BLOCK_SIZE);
}
#elif defined(WC_32BIT_CPU)
static WC_INLINE void GMULT(byte *x, byte m[32][AES_BLOCK_SIZE])
{
    int i;
    word32 z8[4] = {0, 0, 0, 0};
    byte a;
    word32* x8 = (word32*)x;
    word32* m8;
    byte xi;
    word32 n7, n6, n5, n4, n3, n2, n1, n0;

    for (i = 15; i > 0; i--) {
        xi = x[i];

        /* XOR in (msn * H) */
        m8 = (word32*)m[xi & 0xf];
        z8[0] ^= m8[0]; z8[1] ^= m8[1]; z8[2] ^= m8[2]; z8[3] ^= m8[3];

        /* Cache top byte for remainder calculations - lost in rotate. */
        a = (byte)(z8[3] >> 24);

        /* Rotate Z by 8-bits */
        z8[3] = (z8[2] >> 24) | (z8[3] << 8);
        z8[2] = (z8[1] >> 24) | (z8[2] << 8);
        z8[1] = (z8[0] >> 24) | (z8[1] << 8);
        z8[0] <<= 8;

        /* XOR in (msn * remainder) [pre-rotated by 4 bits] */
        z8[0] ^= (word32)R[16 + (a & 0xf)];

        xi >>= 4;
        /* XOR in next significant nibble (XORed with H) * remainder */
        m8 = (word32*)m[xi];
        a ^= (byte)(m8[3] >> 20);
        z8[0] ^= (word32)R[a >> 4];

        /* XOR in (next significant nibble * H) [pre-rotated by 4 bits] */
        m8 = (word32*)m[16 + xi];
        z8[0] ^= m8[0]; z8[1] ^= m8[1];
        z8[2] ^= m8[2]; z8[3] ^= m8[3];
    }

    xi = x[0];

    /* XOR in most significant nibble * H */
    m8 = (word32*)m[xi & 0xf];
    z8[0] ^= m8[0]; z8[1] ^= m8[1]; z8[2] ^= m8[2]; z8[3] ^= m8[3];

    /* Cache top byte for remainder calculations - lost in rotate. */
    a = (z8[3] >> 24) & 0xf;

    /* Rotate z by 4-bits */
    n7 = z8[3] & 0xf0f0f0f0ULL;
    n6 = z8[3] & 0x0f0f0f0fULL;
    n5 = z8[2] & 0xf0f0f0f0ULL;
    n4 = z8[2] & 0x0f0f0f0fULL;
    n3 = z8[1] & 0xf0f0f0f0ULL;
    n2 = z8[1] & 0x0f0f0f0fULL;
    n1 = z8[0] & 0xf0f0f0f0ULL;
    n0 = z8[0] & 0x0f0f0f0fULL;
    z8[3] = (n7 >> 4) | (n6 << 12) | (n4 >> 20);
    z8[2] = (n5 >> 4) | (n4 << 12) | (n2 >> 20);
    z8[1] = (n3 >> 4) | (n2 << 12) | (n0 >> 20);
    z8[0] = (n1 >> 4) | (n0 << 12);

    /* XOR in most significant nibble * remainder */
    z8[0] ^= (word32)R[a];
    /* XOR in next significant nibble * H */
    m8 = (word32*)m[xi >> 4];
    z8[0] ^= m8[0]; z8[1] ^= m8[1]; z8[2] ^= m8[2]; z8[3] ^= m8[3];

    /* Write back result. */
    x8[0] = z8[0]; x8[1] = z8[1]; x8[2] = z8[2]; x8[3] = z8[3];
}
#else
static WC_INLINE void GMULT(byte *x, byte m[32][AES_BLOCK_SIZE])
{
    int i;
    word64 z8[2] = {0, 0};
    byte a;
    word64* x8 = (word64*)x;
    word64* m8;
    word64 n0, n1, n2, n3;
    byte xi;

    for (i = 15; i > 0; i--) {
        xi = x[i];

        /* XOR in (msn * H) */
        m8 = (word64*)m[xi & 0xf];
        z8[0] ^= m8[0];
        z8[1] ^= m8[1];

        /* Cache top byte for remainder calculations - lost in rotate. */
        a = (byte)(z8[1] >> 56);

        /* Rotate Z by 8-bits */
        z8[1] = (z8[0] >> 56) | (z8[1] << 8);
        z8[0] <<= 8;

        /* XOR in (next significant nibble * H) [pre-rotated by 4 bits] */
        m8 = (word64*)m[16 + (xi >> 4)];
        z8[0] ^= m8[0];
        z8[1] ^= m8[1];

        /* XOR in (msn * remainder) [pre-rotated by 4 bits] */
        z8[0] ^= (word64)R[16 + (a & 0xf)];
        /* XOR in next significant nibble (XORed with H) * remainder */
        m8 = (word64*)m[xi >> 4];
        a ^= (byte)(m8[1] >> 52);
        z8[0] ^= (word64)R[a >> 4];
    }

    xi = x[0];

    /* XOR in most significant nibble * H */
    m8 = (word64*)m[xi & 0xf];
    z8[0] ^= m8[0];
    z8[1] ^= m8[1];

    /* Cache top byte for remainder calculations - lost in rotate. */
    a = (z8[1] >> 56) & 0xf;

    /* Rotate z by 4-bits */
    n3 = z8[1] & W64LIT(0xf0f0f0f0f0f0f0f0);
    n2 = z8[1] & W64LIT(0x0f0f0f0f0f0f0f0f);
    n1 = z8[0] & W64LIT(0xf0f0f0f0f0f0f0f0);
    n0 = z8[0] & W64LIT(0x0f0f0f0f0f0f0f0f);
    z8[1] = (n3 >> 4) | (n2 << 12) | (n0 >> 52);
    z8[0] = (n1 >> 4) | (n0 << 12);

    /* XOR in next significant nibble * H */
    m8 = (word64*)m[xi >> 4];
    z8[0] ^= m8[0];
    z8[1] ^= m8[1];
    /* XOR in most significant nibble * remainder */
    z8[0] ^= (word64)R[a];

    /* Write back result. */
    x8[0] = z8[0];
    x8[1] = z8[1];
}
#endif

void GHASH(Gcm* gcm, const byte* a, word32 aSz, const byte* c,
    word32 cSz, byte* s, word32 sSz)
{
    byte x[AES_BLOCK_SIZE];
    byte scratch[AES_BLOCK_SIZE];
    word32 blocks, partial;

    if (gcm == NULL) {
        return;
    }

    XMEMSET(x, 0, AES_BLOCK_SIZE);

    /* Hash in A, the Additional Authentication Data */
    if (aSz != 0 && a != NULL) {
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        while (blocks--) {
            xorbuf(x, a, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
            a += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(scratch, 0, AES_BLOCK_SIZE);
            XMEMCPY(scratch, a, partial);
            xorbuf(x, scratch, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
        }
    }

    /* Hash in C, the Ciphertext */
    if (cSz != 0 && c != NULL) {
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        while (blocks--) {
            xorbuf(x, c, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
            c += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(scratch, 0, AES_BLOCK_SIZE);
            XMEMCPY(scratch, c, partial);
            xorbuf(x, scratch, AES_BLOCK_SIZE);
            GMULT(x, gcm->M0);
        }
    }

    /* Hash in the lengths of A and C in bits */
    FlattenSzInBits(&scratch[0], aSz);
    FlattenSzInBits(&scratch[8], cSz);
    xorbuf(x, scratch, AES_BLOCK_SIZE);
    GMULT(x, gcm->M0);

    /* Copy the result into s. */
    XMEMCPY(s, x, sSz);
}

#ifdef WOLFSSL_AESGCM_STREAM
/* No extra initialization for 4-bit table implementation.
 *
 * @param [in] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes) WC_DO_NOTHING

/* GHASH one block of data..
 *
 * XOR block into tag and GMULT with H using pre-computed table.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                     \
    do {                                                \
        xorbuf(AES_TAG(aes), block, AES_BLOCK_SIZE);    \
        GMULT(AES_TAG(aes), (aes)->gcm.M0);             \
    }                                                   \
    while (0)
#endif /* WOLFSSL_AESGCM_STREAM */
#elif defined(WORD64_AVAILABLE) && !defined(GCM_WORD32)

#if !defined(FREESCALE_LTC_AES_GCM)
static void GMULT(word64* X, word64* Y)
{
    word64 Z[2] = {0,0};
    word64 V[2];
    int i, j;
    word64 v1;
    V[0] = X[0];  V[1] = X[1];

    for (i = 0; i < 2; i++)
    {
        word64 y = Y[i];
        for (j = 0; j < 64; j++)
        {
#ifndef AES_GCM_GMULT_NCT
            word64 mask = 0 - (y >> 63);
            Z[0] ^= V[0] & mask;
            Z[1] ^= V[1] & mask;
#else
            if (y & 0x8000000000000000ULL) {
                Z[0] ^= V[0];
                Z[1] ^= V[1];
            }
#endif

            v1 = (0 - (V[1] & 1)) & 0xE100000000000000ULL;
            V[1] >>= 1;
            V[1] |= V[0] << 63;
            V[0] >>= 1;
            V[0] ^= v1;
            y <<= 1;
        }
    }
    X[0] = Z[0];
    X[1] = Z[1];
}


void GHASH(Gcm* gcm, const byte* a, word32 aSz, const byte* c,
    word32 cSz, byte* s, word32 sSz)
{
    word64 x[2] = {0,0};
    word32 blocks, partial;
    word64 bigH[2];

    if (gcm == NULL) {
        return;
    }

    XMEMCPY(bigH, gcm->H, AES_BLOCK_SIZE);
    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords64(bigH, bigH, AES_BLOCK_SIZE);
    #endif

    /* Hash in A, the Additional Authentication Data */
    if (aSz != 0 && a != NULL) {
        word64 bigA[2];
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        while (blocks--) {
            XMEMCPY(bigA, a, AES_BLOCK_SIZE);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords64(bigA, bigA, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigA[0];
            x[1] ^= bigA[1];
            GMULT(x, bigH);
            a += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(bigA, 0, AES_BLOCK_SIZE);
            XMEMCPY(bigA, a, partial);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords64(bigA, bigA, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigA[0];
            x[1] ^= bigA[1];
            GMULT(x, bigH);
        }
#ifdef OPENSSL_EXTRA
        /* store AAD partial tag for next call */
        gcm->aadH[0] = (word32)((x[0] & 0xFFFFFFFF00000000ULL) >> 32);
        gcm->aadH[1] = (word32)(x[0] & 0xFFFFFFFF);
        gcm->aadH[2] = (word32)((x[1] & 0xFFFFFFFF00000000ULL) >> 32);
        gcm->aadH[3] = (word32)(x[1] & 0xFFFFFFFF);
#endif
    }

    /* Hash in C, the Ciphertext */
    if (cSz != 0 && c != NULL) {
        word64 bigC[2];
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
#ifdef OPENSSL_EXTRA
        /* Start from last AAD partial tag */
        if(gcm->aadLen) {
            x[0] = ((word64)gcm->aadH[0]) << 32 | gcm->aadH[1];
            x[1] = ((word64)gcm->aadH[2]) << 32 | gcm->aadH[3];
         }
#endif
        while (blocks--) {
            XMEMCPY(bigC, c, AES_BLOCK_SIZE);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords64(bigC, bigC, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigC[0];
            x[1] ^= bigC[1];
            GMULT(x, bigH);
            c += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(bigC, 0, AES_BLOCK_SIZE);
            XMEMCPY(bigC, c, partial);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords64(bigC, bigC, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigC[0];
            x[1] ^= bigC[1];
            GMULT(x, bigH);
        }
    }

    /* Hash in the lengths in bits of A and C */
    {
        word64 len[2];
        len[0] = aSz; len[1] = cSz;
#ifdef OPENSSL_EXTRA
        if (gcm->aadLen)
            len[0] = (word64)gcm->aadLen;
#endif
        /* Lengths are in bytes. Convert to bits. */
        len[0] *= 8;
        len[1] *= 8;

        x[0] ^= len[0];
        x[1] ^= len[1];
        GMULT(x, bigH);
    }
    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords64(x, x, AES_BLOCK_SIZE);
    #endif
    XMEMCPY(s, x, sSz);
}
#endif /* !FREESCALE_LTC_AES_GCM */

#ifdef WOLFSSL_AESGCM_STREAM

#ifdef LITTLE_ENDIAN_ORDER

/* No extra initialization for small implementation.
 *
 * @param [in] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes)                                               \
    ByteReverseWords64((word64*)aes->gcm.H, (word64*)aes->gcm.H, AES_BLOCK_SIZE)

/* GHASH one block of data..
 *
 * XOR block into tag and GMULT with H.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                             \
    do {                                                        \
        word64* x = (word64*)AES_TAG(aes);                      \
        word64* h = (word64*)aes->gcm.H;                        \
        word64 block64[2];                                      \
        XMEMCPY(block64, block, AES_BLOCK_SIZE);                \
        ByteReverseWords64(block64, block64, AES_BLOCK_SIZE);   \
        x[0] ^= block64[0];                                     \
        x[1] ^= block64[1];                                     \
        GMULT(x, h);                                            \
    }                                                           \
    while (0)

#ifdef OPENSSL_EXTRA
/* GHASH in AAD and cipher text lengths in bits.
 *
 * Convert tag back to little-endian.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                            \
    do {                                                \
        word64* x = (word64*)AES_TAG(aes);              \
        word64* h = (word64*)aes->gcm.H;                \
        word64 len[2];                                  \
        len[0] = aes->aSz; len[1] = aes->cSz;           \
        if (aes->gcm.aadLen)                            \
            len[0] = (word64)aes->gcm.aadLen;           \
        /* Lengths are in bytes. Convert to bits. */    \
        len[0] *= 8;                                    \
        len[1] *= 8;                                    \
                                                        \
        x[0] ^= len[0];                                 \
        x[1] ^= len[1];                                 \
        GMULT(x, h);                                    \
        ByteReverseWords64(x, x, AES_BLOCK_SIZE);       \
    }                                                   \
    while (0)
#else
/* GHASH in AAD and cipher text lengths in bits.
 *
 * Convert tag back to little-endian.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                            \
    do {                                                \
        word64* x = (word64*)AES_TAG(aes);              \
        word64* h = (word64*)aes->gcm.H;                \
        word64 len[2];                                  \
        len[0] = aes->aSz; len[1] = aes->cSz;           \
        /* Lengths are in bytes. Convert to bits. */    \
        len[0] *= 8;                                    \
        len[1] *= 8;                                    \
                                                        \
        x[0] ^= len[0];                                 \
        x[1] ^= len[1];                                 \
        GMULT(x, h);                                    \
        ByteReverseWords64(x, x, AES_BLOCK_SIZE);       \
    }                                                   \
    while (0)
#endif

#else

/* No extra initialization for small implementation.
 *
 * @param [in] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes) WC_DO_NOTHING

/* GHASH one block of data..
 *
 * XOR block into tag and GMULT with H.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                     \
    do {                                                \
        word64* x = (word64*)AES_TAG(aes);              \
        word64* h = (word64*)aes->gcm.H;                \
        word64 block64[2];                              \
        XMEMCPY(block64, block, AES_BLOCK_SIZE);        \
        x[0] ^= block64[0];                             \
        x[1] ^= block64[1];                             \
        GMULT(x, h);                                    \
    }                                                   \
    while (0)

#ifdef OPENSSL_EXTRA
/* GHASH in AAD and cipher text lengths in bits.
 *
 * Convert tag back to little-endian.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                            \
    do {                                                \
        word64* x = (word64*)AES_TAG(aes);              \
        word64* h = (word64*)aes->gcm.H;                \
        word64 len[2];                                  \
        len[0] = aes->aSz; len[1] = aes->cSz;           \
        if (aes->gcm.aadLen)                            \
            len[0] = (word64)aes->gcm.aadLen;           \
        /* Lengths are in bytes. Convert to bits. */    \
        len[0] *= 8;                                    \
        len[1] *= 8;                                    \
                                                        \
        x[0] ^= len[0];                                 \
        x[1] ^= len[1];                                 \
        GMULT(x, h);                                    \
    }                                                   \
    while (0)
#else
/* GHASH in AAD and cipher text lengths in bits.
 *
 * Convert tag back to little-endian.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                            \
    do {                                                \
        word64* x = (word64*)AES_TAG(aes);              \
        word64* h = (word64*)aes->gcm.H;                \
        word64 len[2];                                  \
        len[0] = aes->aSz; len[1] = aes->cSz;           \
        /* Lengths are in bytes. Convert to bits. */    \
        len[0] *= 8;                                    \
        len[1] *= 8;                                    \
                                                        \
        x[0] ^= len[0];                                 \
        x[1] ^= len[1];                                 \
        GMULT(x, h);                                    \
    }                                                   \
    while (0)
#endif

#endif /* !LITTLE_ENDIAN_ORDER */

#endif /* WOLFSSL_AESGCM_STREAM */
/* end defined(WORD64_AVAILABLE) && !defined(GCM_WORD32) */
#else /* GCM_WORD32 */

static void GMULT(word32* X, word32* Y)
{
    word32 Z[4] = {0,0,0,0};
    word32 V[4];
    int i, j;

    V[0] = X[0];  V[1] = X[1]; V[2] =  X[2]; V[3] =  X[3];

    for (i = 0; i < 4; i++)
    {
        word32 y = Y[i];
        for (j = 0; j < 32; j++)
        {
            if (y & 0x80000000) {
                Z[0] ^= V[0];
                Z[1] ^= V[1];
                Z[2] ^= V[2];
                Z[3] ^= V[3];
            }

            if (V[3] & 0x00000001) {
                V[3] >>= 1;
                V[3] |= ((V[2] & 0x00000001) ? 0x80000000 : 0);
                V[2] >>= 1;
                V[2] |= ((V[1] & 0x00000001) ? 0x80000000 : 0);
                V[1] >>= 1;
                V[1] |= ((V[0] & 0x00000001) ? 0x80000000 : 0);
                V[0] >>= 1;
                V[0] ^= 0xE1000000;
            } else {
                V[3] >>= 1;
                V[3] |= ((V[2] & 0x00000001) ? 0x80000000 : 0);
                V[2] >>= 1;
                V[2] |= ((V[1] & 0x00000001) ? 0x80000000 : 0);
                V[1] >>= 1;
                V[1] |= ((V[0] & 0x00000001) ? 0x80000000 : 0);
                V[0] >>= 1;
            }
            y <<= 1;
        }
    }
    X[0] = Z[0];
    X[1] = Z[1];
    X[2] = Z[2];
    X[3] = Z[3];
}


void GHASH(Gcm* gcm, const byte* a, word32 aSz, const byte* c,
    word32 cSz, byte* s, word32 sSz)
{
    word32 x[4] = {0,0,0,0};
    word32 blocks, partial;
    word32 bigH[4];

    if (gcm == NULL) {
        return;
    }

    XMEMCPY(bigH, gcm->H, AES_BLOCK_SIZE);
    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords(bigH, bigH, AES_BLOCK_SIZE);
    #endif

    /* Hash in A, the Additional Authentication Data */
    if (aSz != 0 && a != NULL) {
        word32 bigA[4];
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        while (blocks--) {
            XMEMCPY(bigA, a, AES_BLOCK_SIZE);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords(bigA, bigA, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigA[0];
            x[1] ^= bigA[1];
            x[2] ^= bigA[2];
            x[3] ^= bigA[3];
            GMULT(x, bigH);
            a += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(bigA, 0, AES_BLOCK_SIZE);
            XMEMCPY(bigA, a, partial);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords(bigA, bigA, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigA[0];
            x[1] ^= bigA[1];
            x[2] ^= bigA[2];
            x[3] ^= bigA[3];
            GMULT(x, bigH);
        }
    }

    /* Hash in C, the Ciphertext */
    if (cSz != 0 && c != NULL) {
        word32 bigC[4];
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        while (blocks--) {
            XMEMCPY(bigC, c, AES_BLOCK_SIZE);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords(bigC, bigC, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigC[0];
            x[1] ^= bigC[1];
            x[2] ^= bigC[2];
            x[3] ^= bigC[3];
            GMULT(x, bigH);
            c += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            XMEMSET(bigC, 0, AES_BLOCK_SIZE);
            XMEMCPY(bigC, c, partial);
            #ifdef LITTLE_ENDIAN_ORDER
                ByteReverseWords(bigC, bigC, AES_BLOCK_SIZE);
            #endif
            x[0] ^= bigC[0];
            x[1] ^= bigC[1];
            x[2] ^= bigC[2];
            x[3] ^= bigC[3];
            GMULT(x, bigH);
        }
    }

    /* Hash in the lengths in bits of A and C */
    {
        word32 len[4];

        /* Lengths are in bytes. Convert to bits. */
        len[0] = (aSz >> (8*sizeof(aSz) - 3));
        len[1] = aSz << 3;
        len[2] = (cSz >> (8*sizeof(cSz) - 3));
        len[3] = cSz << 3;

        x[0] ^= len[0];
        x[1] ^= len[1];
        x[2] ^= len[2];
        x[3] ^= len[3];
        GMULT(x, bigH);
    }
    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords(x, x, AES_BLOCK_SIZE);
    #endif
    XMEMCPY(s, x, sSz);
}

#ifdef WOLFSSL_AESGCM_STREAM
#ifdef LITTLE_ENDIAN_ORDER
/* Little-endian 32-bit word implementation requires byte reversal of H.
 *
 * H is all-zeros block encrypted with key.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes) \
    ByteReverseWords((word32*)aes->gcm.H, (word32*)aes->gcm.H, AES_BLOCK_SIZE)

/* GHASH one block of data..
 *
 * XOR block, in big-endian form, into tag and GMULT with H.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                         \
    do {                                                    \
        word32* x = (word32*)AES_TAG(aes);                  \
        word32* h = (word32*)aes->gcm.H;                    \
        word32 bigEnd[4];                                   \
        XMEMCPY(bigEnd, block, AES_BLOCK_SIZE);             \
        ByteReverseWords(bigEnd, bigEnd, AES_BLOCK_SIZE);   \
        x[0] ^= bigEnd[0];                                  \
        x[1] ^= bigEnd[1];                                  \
        x[2] ^= bigEnd[2];                                  \
        x[3] ^= bigEnd[3];                                  \
        GMULT(x, h);                                        \
    }                                                       \
    while (0)

/* GHASH in AAD and cipher text lengths in bits.
 *
 * Convert tag back to little-endian.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                                \
    do {                                                    \
        word32 len[4];                                      \
        word32* x = (word32*)AES_TAG(aes);                  \
        word32* h = (word32*)aes->gcm.H;                    \
        len[0] = (aes->aSz >> (8*sizeof(aes->aSz) - 3));    \
        len[1] = aes->aSz << 3;                             \
        len[2] = (aes->cSz >> (8*sizeof(aes->cSz) - 3));    \
        len[3] = aes->cSz << 3;                             \
        x[0] ^= len[0];                                     \
        x[1] ^= len[1];                                     \
        x[2] ^= len[2];                                     \
        x[3] ^= len[3];                                     \
        GMULT(x, h);                                        \
        ByteReverseWords(x, x, AES_BLOCK_SIZE);             \
    }                                                       \
    while (0)
#else
/* No extra initialization for 32-bit word implementation.
 *
 * @param [in] aes  AES GCM object.
 */
#define GHASH_INIT_EXTRA(aes) WC_DO_NOTHING

/* GHASH one block of data..
 *
 * XOR block into tag and GMULT with H.
 *
 * @param [in, out] aes    AES GCM object.
 * @param [in]      block  Block of AAD or cipher text.
 */
#define GHASH_ONE_BLOCK(aes, block)                         \
    do {                                                    \
        word32* x = (word32*)AES_TAG(aes);                  \
        word32* h = (word32*)aes->gcm.H;                    \
        word32 block32[4];                                  \
        XMEMCPY(block32, block, AES_BLOCK_SIZE);            \
        x[0] ^= block32[0];                                 \
        x[1] ^= block32[1];                                 \
        x[2] ^= block32[2];                                 \
        x[3] ^= block32[3];                                 \
        GMULT(x, h);                                        \
    }                                                       \
    while (0)

/* GHASH in AAD and cipher text lengths in bits.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                                \
    do {                                                    \
        word32 len[4];                                      \
        word32* x = (word32*)AES_TAG(aes);                  \
        word32* h = (word32*)aes->gcm.H;                    \
        len[0] = (aes->aSz >> (8*sizeof(aes->aSz) - 3));    \
        len[1] = aes->aSz << 3;                             \
        len[2] = (aes->cSz >> (8*sizeof(aes->cSz) - 3));    \
        len[3] = aes->cSz << 3;                             \
        x[0] ^= len[0];                                     \
        x[1] ^= len[1];                                     \
        x[2] ^= len[2];                                     \
        x[3] ^= len[3];                                     \
        GMULT(x, h);                                        \
    }                                                       \
    while (0)
#endif /* LITTLE_ENDIAN_ORDER */
#endif /* WOLFSSL_AESGCM_STREAM */
#endif /* end GCM_WORD32 */

#if !defined(WOLFSSL_XILINX_CRYPT) && !defined(WOLFSSL_AFALG_XILINX_AES)
#ifdef WOLFSSL_AESGCM_STREAM
#ifndef GHASH_LEN_BLOCK
/* Hash in the lengths of the AAD and cipher text in bits.
 *
 * Default implementation.
 *
 * @param [in, out] aes  AES GCM object.
 */
#define GHASH_LEN_BLOCK(aes)                      \
    do {                                          \
        byte scratch[AES_BLOCK_SIZE];             \
        FlattenSzInBits(&scratch[0], (aes)->aSz); \
        FlattenSzInBits(&scratch[8], (aes)->cSz); \
        GHASH_ONE_BLOCK(aes, scratch);            \
    }                                             \
    while (0)
#endif

/* Initialize a GHASH for streaming operations.
 *
 * @param [in, out] aes  AES GCM object.
 */
static void GHASH_INIT(Aes* aes) {
    /* Set tag to all zeros as initial value. */
    XMEMSET(AES_TAG(aes), 0, AES_BLOCK_SIZE);
    /* Reset counts of AAD and cipher text. */
    aes->aOver = 0;
    aes->cOver = 0;
    /* Extra initialization based on implementation. */
    GHASH_INIT_EXTRA(aes);
}

/* Update the GHASH with AAD and/or cipher text.
 *
 * @param [in,out] aes   AES GCM object.
 * @param [in]     a     Additional authentication data buffer.
 * @param [in]     aSz   Size of data in AAD buffer.
 * @param [in]     c     Cipher text buffer.
 * @param [in]     cSz   Size of data in cipher text buffer.
 */
static void GHASH_UPDATE(Aes* aes, const byte* a, word32 aSz, const byte* c,
    word32 cSz)
{
    word32 blocks;
    word32 partial;

    /* Hash in A, the Additional Authentication Data */
    if (aSz != 0 && a != NULL) {
        /* Update count of AAD we have hashed. */
        aes->aSz += aSz;
        /* Check if we have unprocessed data. */
        if (aes->aOver > 0) {
            /* Calculate amount we can use - fill up the block. */
            byte sz = AES_BLOCK_SIZE - aes->aOver;
            if (sz > aSz) {
                sz = (byte)aSz;
            }
            /* Copy extra into last GHASH block array and update count. */
            XMEMCPY(AES_LASTGBLOCK(aes) + aes->aOver, a, sz);
            aes->aOver += sz;
            if (aes->aOver == AES_BLOCK_SIZE) {
                /* We have filled up the block and can process. */
                GHASH_ONE_BLOCK(aes, AES_LASTGBLOCK(aes));
                /* Reset count. */
                aes->aOver = 0;
            }
            /* Used up some data. */
            aSz -= sz;
            a += sz;
        }

        /* Calculate number of blocks of AAD and the leftover. */
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        /* GHASH full blocks now. */
        while (blocks--) {
            GHASH_ONE_BLOCK(aes, a);
            a += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            /* Cache the partial block. */
            XMEMCPY(AES_LASTGBLOCK(aes), a, partial);
            aes->aOver = (byte)partial;
        }
    }
    if (aes->aOver > 0 && cSz > 0 && c != NULL) {
        /* No more AAD coming and we have a partial block. */
        /* Fill the rest of the block with zeros. */
        byte sz = AES_BLOCK_SIZE - aes->aOver;
        XMEMSET(AES_LASTGBLOCK(aes) + aes->aOver, 0, sz);
        /* GHASH last AAD block. */
        GHASH_ONE_BLOCK(aes, AES_LASTGBLOCK(aes));
        /* Clear partial count for next time through. */
        aes->aOver = 0;
    }

    /* Hash in C, the Ciphertext */
    if (cSz != 0 && c != NULL) {
        /* Update count of cipher text we have hashed. */
        aes->cSz += cSz;
        if (aes->cOver > 0) {
            /* Calculate amount we can use - fill up the block. */
            byte sz = AES_BLOCK_SIZE - aes->cOver;
            if (sz > cSz) {
                sz = (byte)cSz;
            }
            XMEMCPY(AES_LASTGBLOCK(aes) + aes->cOver, c, sz);
            /* Update count of unused encrypted counter. */
            aes->cOver += sz;
            if (aes->cOver == AES_BLOCK_SIZE) {
                /* We have filled up the block and can process. */
                GHASH_ONE_BLOCK(aes, AES_LASTGBLOCK(aes));
                /* Reset count. */
                aes->cOver = 0;
            }
            /* Used up some data. */
            cSz -= sz;
            c += sz;
        }

        /* Calculate number of blocks of cipher text and the leftover. */
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        /* GHASH full blocks now. */
        while (blocks--) {
            GHASH_ONE_BLOCK(aes, c);
            c += AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            /* Cache the partial block. */
            XMEMCPY(AES_LASTGBLOCK(aes), c, partial);
            aes->cOver = (byte)partial;
        }
    }
}

/* Finalize the GHASH calculation.
 *
 * Complete hashing cipher text and hash the AAD and cipher text lengths.
 *
 * @param [in, out] aes  AES GCM object.
 * @param [out]     s    Authentication tag.
 * @param [in]      sSz  Size of authentication tag required.
 */
static void GHASH_FINAL(Aes* aes, byte* s, word32 sSz)
{
    /* AAD block incomplete when > 0 */
    byte over = aes->aOver;

    if (aes->cOver > 0) {
        /* Cipher text block incomplete. */
        over = aes->cOver;
    }
    if (over > 0) {
        /* Zeroize the unused part of the block. */
        XMEMSET(AES_LASTGBLOCK(aes) + over, 0, AES_BLOCK_SIZE - over);
        /* Hash the last block of cipher text. */
        GHASH_ONE_BLOCK(aes, AES_LASTGBLOCK(aes));
    }
    /* Hash in the lengths of AAD and cipher text in bits */
    GHASH_LEN_BLOCK(aes);
    /* Copy the result into s. */
    XMEMCPY(s, AES_TAG(aes), sSz);
}
#endif /* WOLFSSL_AESGCM_STREAM */


#ifdef FREESCALE_LTC_AES_GCM
int wc_AesGcmEncrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    status_t status;
    word32 keySize;

    /* argument checks */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE || ivSz == 0) {
        return BAD_FUNC_ARG;
    }

    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

    status = wc_AesGetKeySize(aes, &keySize);
    if (status)
        return status;

    status = wolfSSL_CryptHwMutexLock();
    if (status != 0)
        return status;

    status = LTC_AES_EncryptTagGcm(LTC_BASE, in, out, sz, iv, ivSz,
        authIn, authInSz, (byte*)aes->key, keySize, authTag, authTagSz);
    wolfSSL_CryptHwMutexUnLock();

    return (status == kStatus_Success) ? 0 : AES_GCM_AUTH_E;
}

#else

#ifdef STM32_CRYPTO_AES_GCM

/* this function supports inline encrypt */
/* define STM32_AESGCM_PARTIAL for STM HW that does not support authentication
 * on byte multiples (see CRYP_HEADERWIDTHUNIT_BYTE) */
static WARN_UNUSED_RESULT int wc_AesGcmEncrypt_STM32(
                                  Aes* aes, byte* out, const byte* in, word32 sz,
                                  const byte* iv, word32 ivSz,
                                  byte* authTag, word32 authTagSz,
                                  const byte* authIn, word32 authInSz)
{
    int ret;
#ifdef WOLFSSL_STM32_CUBEMX
    CRYP_HandleTypeDef hcryp;
#else
    word32 keyCopy[AES_256_KEY_SIZE/sizeof(word32)];
#endif
    word32 keySize;
#ifdef WOLFSSL_STM32_CUBEMX
    int status = HAL_OK;
    word32 blocks = sz / AES_BLOCK_SIZE;
    word32 partialBlock[AES_BLOCK_SIZE/sizeof(word32)];
#else
    int status = SUCCESS;
#endif
    word32 partial = sz % AES_BLOCK_SIZE;
    word32 tag[AES_BLOCK_SIZE/sizeof(word32)];
    word32 ctrInit[AES_BLOCK_SIZE/sizeof(word32)];
    word32 ctr[AES_BLOCK_SIZE/sizeof(word32)];
    word32 authhdr[AES_BLOCK_SIZE/sizeof(word32)];
    byte* authInPadded = NULL;
    int authPadSz, wasAlloc = 0, useSwGhash = 0;

    ret = wc_AesGetKeySize(aes, &keySize);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_STM32_CUBEMX
    ret = wc_Stm32_Aes_Init(aes, &hcryp);
    if (ret != 0)
        return ret;
#endif

    XMEMSET(ctr, 0, AES_BLOCK_SIZE);
    if (ivSz == GCM_NONCE_MID_SZ) {
        byte* pCtr = (byte*)ctr;
        XMEMCPY(ctr, iv, ivSz);
        pCtr[AES_BLOCK_SIZE - 1] = 1;
    }
    else {
        GHASH(&aes->gcm, NULL, 0, iv, ivSz, (byte*)ctr, AES_BLOCK_SIZE);
    }
    XMEMCPY(ctrInit, ctr, sizeof(ctr)); /* save off initial counter for GMAC */

    /* Authentication buffer - must be 4-byte multiple zero padded */
    authPadSz = authInSz % sizeof(word32);
    if (authPadSz != 0) {
        authPadSz = authInSz + sizeof(word32) - authPadSz;
        if (authPadSz <= sizeof(authhdr)) {
            authInPadded = (byte*)authhdr;
        }
        else {
            authInPadded = (byte*)XMALLOC(authPadSz, aes->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (authInPadded == NULL) {
                wolfSSL_CryptHwMutexUnLock();
                return MEMORY_E;
            }
            wasAlloc = 1;
        }
        XMEMSET(authInPadded, 0, authPadSz);
        XMEMCPY(authInPadded, authIn, authInSz);
    } else {
        authPadSz = authInSz;
        authInPadded = (byte*)authIn;
    }

    /* for cases where hardware cannot be used for authTag calculate it */
    /* if IV is not 12 calculate GHASH using software */
    if (ivSz != GCM_NONCE_MID_SZ
    #ifndef CRYP_HEADERWIDTHUNIT_BYTE
        /* or hardware that does not support partial block */
        || sz == 0 || partial != 0
    #endif
    #if !defined(CRYP_HEADERWIDTHUNIT_BYTE) && !defined(STM32_AESGCM_PARTIAL)
        /* or authIn is not a multiple of 4  */
        || authPadSz != authInSz
    #endif
    ) {
        useSwGhash = 1;
    }

    /* Hardware requires counter + 1 */
    IncrementGcmCounter((byte*)ctr);

    ret = wolfSSL_CryptHwMutexLock();
    if (ret != 0) {
        return ret;
    }
#ifdef WOLFSSL_STM32_CUBEMX
    hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)ctr;
    hcryp.Init.Header = (STM_CRYPT_TYPE*)authInPadded;

#if defined(STM32_HAL_V2)
    hcryp.Init.Algorithm = CRYP_AES_GCM;
    #ifdef CRYP_HEADERWIDTHUNIT_BYTE
    /* V2 with CRYP_HEADERWIDTHUNIT_BYTE uses byte size for header */
    hcryp.Init.HeaderSize = authInSz;
    #else
    hcryp.Init.HeaderSize = authPadSz/sizeof(word32);
    #endif
    #ifdef CRYP_KEYIVCONFIG_ONCE
    /* allows repeated calls to HAL_CRYP_Encrypt */
    hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ONCE;
    #endif
    ByteReverseWords(ctr, ctr, AES_BLOCK_SIZE);
    hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)ctr;
    HAL_CRYP_Init(&hcryp);

    #ifndef CRYP_KEYIVCONFIG_ONCE
    /* GCM payload phase - can handle partial blocks */
    status = HAL_CRYP_Encrypt(&hcryp, (uint32_t*)in,
        (blocks * AES_BLOCK_SIZE) + partial, (uint32_t*)out, STM32_HAL_TIMEOUT);
    #else
    /* GCM payload phase - blocks */
    if (blocks) {
        status = HAL_CRYP_Encrypt(&hcryp, (uint32_t*)in,
            (blocks * AES_BLOCK_SIZE), (uint32_t*)out, STM32_HAL_TIMEOUT);
    }
    /* GCM payload phase - partial remainder */
    if (status == HAL_OK && (partial != 0 || blocks == 0)) {
        XMEMSET(partialBlock, 0, sizeof(partialBlock));
        XMEMCPY(partialBlock, in + (blocks * AES_BLOCK_SIZE), partial);
        status = HAL_CRYP_Encrypt(&hcryp, (uint32_t*)partialBlock, partial,
            (uint32_t*)partialBlock, STM32_HAL_TIMEOUT);
        XMEMCPY(out + (blocks * AES_BLOCK_SIZE), partialBlock, partial);
    }
    #endif
    if (status == HAL_OK && !useSwGhash) {
        /* Compute the authTag */
        status = HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, (uint32_t*)tag,
            STM32_HAL_TIMEOUT);
    }
#elif defined(STM32_CRYPTO_AES_ONLY)
    /* Set the CRYP parameters */
    hcryp.Init.HeaderSize = authPadSz;
    if (authPadSz == 0)
        hcryp.Init.Header = NULL; /* cannot pass pointer here when authIn == 0 */
    hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_GCM_GMAC;
    hcryp.Init.OperatingMode = CRYP_ALGOMODE_ENCRYPT;
    hcryp.Init.GCMCMACPhase  = CRYP_INIT_PHASE;
    HAL_CRYP_Init(&hcryp);

    /* GCM init phase */
    status = HAL_CRYPEx_AES_Auth(&hcryp, NULL, 0, NULL, STM32_HAL_TIMEOUT);
    if (status == HAL_OK) {
        /* GCM header phase */
        hcryp.Init.GCMCMACPhase = CRYP_HEADER_PHASE;
        status = HAL_CRYPEx_AES_Auth(&hcryp, NULL, 0, NULL, STM32_HAL_TIMEOUT);
    }
    if (status == HAL_OK) {
        /* GCM payload phase - blocks */
        hcryp.Init.GCMCMACPhase = CRYP_PAYLOAD_PHASE;
        if (blocks) {
            status = HAL_CRYPEx_AES_Auth(&hcryp, (byte*)in,
                (blocks * AES_BLOCK_SIZE), out, STM32_HAL_TIMEOUT);
        }
    }
    if (status == HAL_OK && (partial != 0 || (sz > 0 && blocks == 0))) {
        /* GCM payload phase - partial remainder */
        XMEMSET(partialBlock, 0, sizeof(partialBlock));
        XMEMCPY(partialBlock, in + (blocks * AES_BLOCK_SIZE), partial);
        status = HAL_CRYPEx_AES_Auth(&hcryp, (uint8_t*)partialBlock, partial,
                (uint8_t*)partialBlock, STM32_HAL_TIMEOUT);
        XMEMCPY(out + (blocks * AES_BLOCK_SIZE), partialBlock, partial);
    }
    if (status == HAL_OK && !useSwGhash) {
        /* GCM final phase */
        hcryp.Init.GCMCMACPhase  = CRYP_FINAL_PHASE;
        status = HAL_CRYPEx_AES_Auth(&hcryp, NULL, sz, (uint8_t*)tag, STM32_HAL_TIMEOUT);
    }
#else
    hcryp.Init.HeaderSize = authPadSz;
    HAL_CRYP_Init(&hcryp);
    if (blocks) {
        /* GCM payload phase - blocks */
        status = HAL_CRYPEx_AESGCM_Encrypt(&hcryp, (byte*)in,
            (blocks * AES_BLOCK_SIZE), out, STM32_HAL_TIMEOUT);
    }
    if (status == HAL_OK && (partial != 0 || blocks == 0)) {
        /* GCM payload phase - partial remainder */
        XMEMSET(partialBlock, 0, sizeof(partialBlock));
        XMEMCPY(partialBlock, in + (blocks * AES_BLOCK_SIZE), partial);
        status = HAL_CRYPEx_AESGCM_Encrypt(&hcryp, (uint8_t*)partialBlock, partial,
            (uint8_t*)partialBlock, STM32_HAL_TIMEOUT);
        XMEMCPY(out + (blocks * AES_BLOCK_SIZE), partialBlock, partial);
    }
    if (status == HAL_OK && !useSwGhash) {
        /* Compute the authTag */
        status = HAL_CRYPEx_AESGCM_Finish(&hcryp, sz, (uint8_t*)tag, STM32_HAL_TIMEOUT);
    }
#endif

    if (status != HAL_OK)
        ret = AES_GCM_AUTH_E;
    HAL_CRYP_DeInit(&hcryp);

#else /* Standard Peripheral Library */
    ByteReverseWords(keyCopy, (word32*)aes->key, keySize);
    status = CRYP_AES_GCM(MODE_ENCRYPT, (uint8_t*)ctr,
                         (uint8_t*)keyCopy,      keySize * 8,
                         (uint8_t*)in,           sz,
                         (uint8_t*)authInPadded, authInSz,
                         (uint8_t*)out,          (uint8_t*)tag);
    if (status != SUCCESS)
        ret = AES_GCM_AUTH_E;
#endif /* WOLFSSL_STM32_CUBEMX */
    wolfSSL_CryptHwMutexUnLock();
    wc_Stm32_Aes_Cleanup();

    if (ret == 0) {
        /* return authTag */
        if (authTag) {
            if (useSwGhash) {
                GHASH(&aes->gcm, authIn, authInSz, out, sz, authTag, authTagSz);
                ret = wc_AesEncrypt(aes, (byte*)ctrInit, (byte*)tag);
                if (ret == 0) {
                    xorbuf(authTag, tag, authTagSz);
                }
            }
            else {
                /* use hardware calculated tag */
                XMEMCPY(authTag, tag, authTagSz);
            }
        }
    }

    /* Free memory */
    if (wasAlloc) {
        XFREE(authInPadded, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

#endif /* STM32_CRYPTO_AES_GCM */

#ifdef WOLFSSL_AESNI
/* For performance reasons, this code needs to be not inlined. */
WARN_UNUSED_RESULT int AES_GCM_encrypt_C(
                      Aes* aes, byte* out, const byte* in, word32 sz,
                      const byte* iv, word32 ivSz,
                      byte* authTag, word32 authTagSz,
                      const byte* authIn, word32 authInSz);
#else
static
#endif
WARN_UNUSED_RESULT int AES_GCM_encrypt_C(
                      Aes* aes, byte* out, const byte* in, word32 sz,
                      const byte* iv, word32 ivSz,
                      byte* authTag, word32 authTagSz,
                      const byte* authIn, word32 authInSz)
{
    int ret = 0;
    word32 blocks = sz / AES_BLOCK_SIZE;
    word32 partial = sz % AES_BLOCK_SIZE;
    const byte* p = in;
    byte* c = out;
    ALIGN16 byte counter[AES_BLOCK_SIZE];
    ALIGN16 byte initialCounter[AES_BLOCK_SIZE];
    ALIGN16 byte scratch[AES_BLOCK_SIZE];

    if (ivSz == GCM_NONCE_MID_SZ) {
        /* Counter is IV with bottom 4 bytes set to: 0x00,0x00,0x00,0x01. */
        XMEMCPY(counter, iv, ivSz);
        XMEMSET(counter + GCM_NONCE_MID_SZ, 0,
                                         AES_BLOCK_SIZE - GCM_NONCE_MID_SZ - 1);
        counter[AES_BLOCK_SIZE - 1] = 1;
    }
    else {
        /* Counter is GHASH of IV. */
#ifdef OPENSSL_EXTRA
        word32 aadTemp = aes->gcm.aadLen;
        aes->gcm.aadLen = 0;
#endif
        GHASH(&aes->gcm, NULL, 0, iv, ivSz, counter, AES_BLOCK_SIZE);
#ifdef OPENSSL_EXTRA
        aes->gcm.aadLen = aadTemp;
#endif
    }
    XMEMCPY(initialCounter, counter, AES_BLOCK_SIZE);

#ifdef WOLFSSL_PIC32MZ_CRYPT
    if (blocks) {
        /* use initial IV for HW, but don't use it below */
        XMEMCPY(aes->reg, counter, AES_BLOCK_SIZE);

        ret = wc_Pic32AesCrypt(
            aes->key, aes->keylen, aes->reg, AES_BLOCK_SIZE,
            out, in, (blocks * AES_BLOCK_SIZE),
            PIC32_ENCRYPTION, PIC32_ALGO_AES, PIC32_CRYPTOALGO_AES_GCM);
        if (ret != 0)
            return ret;
    }
    /* process remainder using partial handling */
#endif

#if defined(HAVE_AES_ECB) && !defined(WOLFSSL_PIC32MZ_CRYPT)
    /* some hardware acceleration can gain performance from doing AES encryption
     * of the whole buffer at once */
    if (c != p && blocks > 0) { /* can not handle inline encryption */
        while (blocks--) {
            IncrementGcmCounter(counter);
            XMEMCPY(c, counter, AES_BLOCK_SIZE);
            c += AES_BLOCK_SIZE;
        }

        /* reset number of blocks and then do encryption */
        blocks = sz / AES_BLOCK_SIZE;
        wc_AesEcbEncrypt(aes, out, out, AES_BLOCK_SIZE * blocks);
        xorbuf(out, p, AES_BLOCK_SIZE * blocks);
        p += AES_BLOCK_SIZE * blocks;
    }
    else
#endif /* HAVE_AES_ECB && !WOLFSSL_PIC32MZ_CRYPT */
    {
        while (blocks--) {
            IncrementGcmCounter(counter);
        #if !defined(WOLFSSL_PIC32MZ_CRYPT)
            ret = wc_AesEncrypt(aes, counter, scratch);
            if (ret != 0)
                return ret;
            xorbufout(c, scratch, p, AES_BLOCK_SIZE);
        #endif
            p += AES_BLOCK_SIZE;
            c += AES_BLOCK_SIZE;
        }
    }

    if (partial != 0) {
        IncrementGcmCounter(counter);
        ret = wc_AesEncrypt(aes, counter, scratch);
        if (ret != 0)
            return ret;
        xorbufout(c, scratch, p, partial);
    }
    if (authTag) {
        GHASH(&aes->gcm, authIn, authInSz, out, sz, authTag, authTagSz);
        ret = wc_AesEncrypt(aes, initialCounter, scratch);
        if (ret != 0)
            return ret;
        xorbuf(authTag, scratch, authTagSz);
#ifdef OPENSSL_EXTRA
        if (!in && !sz)
            /* store AAD size for next call */
            aes->gcm.aadLen = authInSz;
#endif
    }

    return ret;
}

/* Software AES - GCM Encrypt */
int wc_AesGcmEncrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    /* argument checks */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE || ivSz == 0) {
        return BAD_FUNC_ARG;
    }

    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (aes->devId != INVALID_DEVID)
    #endif
    {
        int crypto_cb_ret =
            wc_CryptoCb_AesGcmEncrypt(aes, out, in, sz, iv, ivSz, authTag,
                                      authTagSz, authIn, authInSz);
        if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
            return crypto_cb_ret;
        /* fall-through when unavailable */
    }
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)
    /* if async and byte count above threshold */
    /* only 12-byte IV is supported in HW */
    if (aes->asyncDev.marker == WOLFSSL_ASYNC_MARKER_AES &&
                    sz >= WC_ASYNC_THRESH_AES_GCM && ivSz == GCM_NONCE_MID_SZ) {
    #if defined(HAVE_CAVIUM)
        #ifdef HAVE_CAVIUM_V
        if (authInSz == 20) { /* Nitrox V GCM is only working with 20 byte AAD */
            return NitroxAesGcmEncrypt(aes, out, in, sz,
                (const byte*)aes->devKey, aes->keylen, iv, ivSz,
                authTag, authTagSz, authIn, authInSz);
        }
        #endif
    #elif defined(HAVE_INTEL_QA)
        return IntelQaSymAesGcmEncrypt(&aes->asyncDev, out, in, sz,
            (const byte*)aes->devKey, aes->keylen, iv, ivSz,
            authTag, authTagSz, authIn, authInSz);
    #else /* WOLFSSL_ASYNC_CRYPT_SW */
        if (wc_AsyncSwInit(&aes->asyncDev, ASYNC_SW_AES_GCM_ENCRYPT)) {
            WC_ASYNC_SW* sw = &aes->asyncDev.sw;
            sw->aes.aes = aes;
            sw->aes.out = out;
            sw->aes.in = in;
            sw->aes.sz = sz;
            sw->aes.iv = iv;
            sw->aes.ivSz = ivSz;
            sw->aes.authTag = authTag;
            sw->aes.authTagSz = authTagSz;
            sw->aes.authIn = authIn;
            sw->aes.authInSz = authInSz;
            return WC_PENDING_E;
        }
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

#ifdef WOLFSSL_SILABS_SE_ACCEL
    return wc_AesGcmEncrypt_silabs(
        aes, out, in, sz,
        iv, ivSz,
        authTag, authTagSz,
        authIn, authInSz);
#endif

#ifdef STM32_CRYPTO_AES_GCM
    return wc_AesGcmEncrypt_STM32(
        aes, out, in, sz, iv, ivSz,
        authTag, authTagSz, authIn, authInSz);
#endif /* STM32_CRYPTO_AES_GCM */

#ifdef WOLFSSL_AESNI
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_encrypt_avx2(in, out, authIn, iv, authTag, sz, authInSz, ivSz,
                            authTagSz, (const byte*)aes->key, (int)aes->rounds);
        RESTORE_VECTOR_REGISTERS();
        return 0;
    }
    else
    #endif
    #if defined(HAVE_INTEL_AVX1)
    if (IS_INTEL_AVX1(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_encrypt_avx1(in, out, authIn, iv, authTag, sz, authInSz, ivSz,
                            authTagSz, (const byte*)aes->key, (int)aes->rounds);
        RESTORE_VECTOR_REGISTERS();
        return 0;
    }
    else
    #endif
    if (haveAESNI) {
        AES_GCM_encrypt(in, out, authIn, iv, authTag, sz, authInSz, ivSz,
                            authTagSz, (const byte*)aes->key, (int)aes->rounds);
        return 0;
    }
    else
#endif
    {
        return AES_GCM_encrypt_C(aes, out, in, sz, iv, ivSz, authTag, authTagSz,
                                                              authIn, authInSz);
    }
}
#endif


/* AES GCM Decrypt */
#if defined(HAVE_AES_DECRYPT) || defined(HAVE_AESGCM_DECRYPT)
#ifdef FREESCALE_LTC_AES_GCM
int  wc_AesGcmDecrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   const byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    int ret;
    word32 keySize;
    status_t status;

    /* argument checks */
    /* If the sz is non-zero, both in and out must be set. If sz is 0,
     * in and out are don't cares, as this is is the GMAC case. */
    if (aes == NULL || iv == NULL || (sz != 0 && (in == NULL || out == NULL)) ||
        authTag == NULL || authTagSz > AES_BLOCK_SIZE || authTagSz == 0 ||
        ivSz == 0) {

        return BAD_FUNC_ARG;
    }

    ret = wc_AesGetKeySize(aes, &keySize);
    if (ret != 0) {
        return ret;
    }

    status = wolfSSL_CryptHwMutexLock();
    if (status != 0)
        return status;

    status = LTC_AES_DecryptTagGcm(LTC_BASE, in, out, sz, iv, ivSz,
        authIn, authInSz, (byte*)aes->key, keySize, authTag, authTagSz);
    wolfSSL_CryptHwMutexUnLock();

    return (status == kStatus_Success) ? 0 : AES_GCM_AUTH_E;
}

#else

#ifdef STM32_CRYPTO_AES_GCM
/* this function supports inline decrypt */
static WARN_UNUSED_RESULT int wc_AesGcmDecrypt_STM32(
                                  Aes* aes, byte* out,
                                  const byte* in, word32 sz,
                                  const byte* iv, word32 ivSz,
                                  const byte* authTag, word32 authTagSz,
                                  const byte* authIn, word32 authInSz)
{
    int ret;
#ifdef WOLFSSL_STM32_CUBEMX
    int status = HAL_OK;
    CRYP_HandleTypeDef hcryp;
    word32 blocks = sz / AES_BLOCK_SIZE;
#else
    int status = SUCCESS;
    word32 keyCopy[AES_256_KEY_SIZE/sizeof(word32)];
#endif
    word32 keySize;
    word32 partial = sz % AES_BLOCK_SIZE;
    word32 tag[AES_BLOCK_SIZE/sizeof(word32)];
    word32 tagExpected[AES_BLOCK_SIZE/sizeof(word32)];
    word32 partialBlock[AES_BLOCK_SIZE/sizeof(word32)];
    word32 ctr[AES_BLOCK_SIZE/sizeof(word32)];
    word32 authhdr[AES_BLOCK_SIZE/sizeof(word32)];
    byte* authInPadded = NULL;
    int authPadSz, wasAlloc = 0, tagComputed = 0;

    ret = wc_AesGetKeySize(aes, &keySize);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_STM32_CUBEMX
    ret = wc_Stm32_Aes_Init(aes, &hcryp);
    if (ret != 0)
        return ret;
#endif

    XMEMSET(ctr, 0, AES_BLOCK_SIZE);
    if (ivSz == GCM_NONCE_MID_SZ) {
        byte* pCtr = (byte*)ctr;
        XMEMCPY(ctr, iv, ivSz);
        pCtr[AES_BLOCK_SIZE - 1] = 1;
    }
    else {
        GHASH(&aes->gcm, NULL, 0, iv, ivSz, (byte*)ctr, AES_BLOCK_SIZE);
    }

    /* Make copy of expected authTag, which could get corrupted in some
     * Cube HAL versions without proper partial block support.
     * For TLS blocks the authTag is after the output buffer, so save it */
    XMEMCPY(tagExpected, authTag, authTagSz);

    /* Authentication buffer - must be 4-byte multiple zero padded */
    authPadSz = authInSz % sizeof(word32);
    if (authPadSz != 0) {
        authPadSz = authInSz + sizeof(word32) - authPadSz;
    }
    else {
        authPadSz = authInSz;
    }

    /* for cases where hardware cannot be used for authTag calculate it */
    /* if IV is not 12 calculate GHASH using software */
    if (ivSz != GCM_NONCE_MID_SZ
    #ifndef CRYP_HEADERWIDTHUNIT_BYTE
        /* or hardware that does not support partial block */
        || sz == 0 || partial != 0
    #endif
    #if !defined(CRYP_HEADERWIDTHUNIT_BYTE) && !defined(STM32_AESGCM_PARTIAL)
        /* or authIn is not a multiple of 4  */
        || authPadSz != authInSz
    #endif
    ) {
        GHASH(&aes->gcm, authIn, authInSz, in, sz, (byte*)tag, sizeof(tag));
        ret = wc_AesEncrypt(aes, (byte*)ctr, (byte*)partialBlock);
        if (ret != 0)
            return ret;
        xorbuf(tag, partialBlock, sizeof(tag));
        tagComputed = 1;
    }

    /* if using hardware for authentication tag make sure its aligned and zero padded */
    if (authPadSz != authInSz && !tagComputed) {
        if (authPadSz <= sizeof(authhdr)) {
            authInPadded = (byte*)authhdr;
        }
        else {
            authInPadded = (byte*)XMALLOC(authPadSz, aes->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (authInPadded == NULL) {
                wolfSSL_CryptHwMutexUnLock();
                return MEMORY_E;
            }
            wasAlloc = 1;
        }
        XMEMSET(authInPadded, 0, authPadSz);
        XMEMCPY(authInPadded, authIn, authInSz);
    } else {
        authInPadded = (byte*)authIn;
    }

    /* Hardware requires counter + 1 */
    IncrementGcmCounter((byte*)ctr);

    ret = wolfSSL_CryptHwMutexLock();
    if (ret != 0) {
        return ret;
    }
#ifdef WOLFSSL_STM32_CUBEMX
    hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)ctr;
    hcryp.Init.Header = (STM_CRYPT_TYPE*)authInPadded;

#if defined(STM32_HAL_V2)
    hcryp.Init.Algorithm = CRYP_AES_GCM;
    #ifdef CRYP_HEADERWIDTHUNIT_BYTE
    /* V2 with CRYP_HEADERWIDTHUNIT_BYTE uses byte size for header */
    hcryp.Init.HeaderSize = authInSz;
    #else
    hcryp.Init.HeaderSize = authPadSz/sizeof(word32);
    #endif
    #ifdef CRYP_KEYIVCONFIG_ONCE
    /* allows repeated calls to HAL_CRYP_Decrypt */
    hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ONCE;
    #endif
    ByteReverseWords(ctr, ctr, AES_BLOCK_SIZE);
    hcryp.Init.pInitVect = (STM_CRYPT_TYPE*)ctr;
    HAL_CRYP_Init(&hcryp);

    #ifndef CRYP_KEYIVCONFIG_ONCE
    status = HAL_CRYP_Decrypt(&hcryp, (uint32_t*)in,
        (blocks * AES_BLOCK_SIZE) + partial, (uint32_t*)out, STM32_HAL_TIMEOUT);
    #else
    /* GCM payload phase - blocks */
    if (blocks) {
        status = HAL_CRYP_Decrypt(&hcryp, (uint32_t*)in,
            (blocks * AES_BLOCK_SIZE), (uint32_t*)out, STM32_HAL_TIMEOUT);
    }
    /* GCM payload phase - partial remainder */
    if (status == HAL_OK && (partial != 0 || blocks == 0)) {
        XMEMSET(partialBlock, 0, sizeof(partialBlock));
        XMEMCPY(partialBlock, in + (blocks * AES_BLOCK_SIZE), partial);
        status = HAL_CRYP_Decrypt(&hcryp, (uint32_t*)partialBlock, partial,
            (uint32_t*)partialBlock, STM32_HAL_TIMEOUT);
        XMEMCPY(out + (blocks * AES_BLOCK_SIZE), partialBlock, partial);
    }
    #endif
    if (status == HAL_OK && !tagComputed) {
        /* Compute the authTag */
        status = HAL_CRYPEx_AESGCM_GenerateAuthTAG(&hcryp, (uint32_t*)tag,
            STM32_HAL_TIMEOUT);
    }
#elif defined(STM32_CRYPTO_AES_ONLY)
    /* Set the CRYP parameters */
    hcryp.Init.HeaderSize = authPadSz;
    if (authPadSz == 0)
        hcryp.Init.Header = NULL; /* cannot pass pointer when authIn == 0 */
    hcryp.Init.ChainingMode  = CRYP_CHAINMODE_AES_GCM_GMAC;
    hcryp.Init.OperatingMode = CRYP_ALGOMODE_DECRYPT;
    hcryp.Init.GCMCMACPhase  = CRYP_INIT_PHASE;
    HAL_CRYP_Init(&hcryp);

    /* GCM init phase */
    status = HAL_CRYPEx_AES_Auth(&hcryp, NULL, 0, NULL, STM32_HAL_TIMEOUT);
    if (status == HAL_OK) {
        /* GCM header phase */
        hcryp.Init.GCMCMACPhase = CRYP_HEADER_PHASE;
        status = HAL_CRYPEx_AES_Auth(&hcryp, NULL, 0, NULL, STM32_HAL_TIMEOUT);
    }
    if (status == HAL_OK) {
        /* GCM payload phase - blocks */
        hcryp.Init.GCMCMACPhase = CRYP_PAYLOAD_PHASE;
        if (blocks) {
            status = HAL_CRYPEx_AES_Auth(&hcryp, (byte*)in,
                (blocks * AES_BLOCK_SIZE), out, STM32_HAL_TIMEOUT);
        }
    }
    if (status == HAL_OK && (partial != 0 || (sz > 0 && blocks == 0))) {
        /* GCM payload phase - partial remainder */
        XMEMSET(partialBlock, 0, sizeof(partialBlock));
        XMEMCPY(partialBlock, in + (blocks * AES_BLOCK_SIZE), partial);
        status = HAL_CRYPEx_AES_Auth(&hcryp, (byte*)partialBlock, partial,
            (byte*)partialBlock, STM32_HAL_TIMEOUT);
        XMEMCPY(out + (blocks * AES_BLOCK_SIZE), partialBlock, partial);
    }
    if (status == HAL_OK && tagComputed == 0) {
        /* GCM final phase */
        hcryp.Init.GCMCMACPhase = CRYP_FINAL_PHASE;
        status = HAL_CRYPEx_AES_Auth(&hcryp, NULL, sz, (byte*)tag, STM32_HAL_TIMEOUT);
    }
#else
    hcryp.Init.HeaderSize = authPadSz;
    HAL_CRYP_Init(&hcryp);
    if (blocks) {
        /* GCM payload phase - blocks */
        status = HAL_CRYPEx_AESGCM_Decrypt(&hcryp, (byte*)in,
            (blocks * AES_BLOCK_SIZE), out, STM32_HAL_TIMEOUT);
    }
    if (status == HAL_OK && (partial != 0 || blocks == 0)) {
        /* GCM payload phase - partial remainder */
        XMEMSET(partialBlock, 0, sizeof(partialBlock));
        XMEMCPY(partialBlock, in + (blocks * AES_BLOCK_SIZE), partial);
        status = HAL_CRYPEx_AESGCM_Decrypt(&hcryp, (byte*)partialBlock, partial,
            (byte*)partialBlock, STM32_HAL_TIMEOUT);
        XMEMCPY(out + (blocks * AES_BLOCK_SIZE), partialBlock, partial);
    }
    if (status == HAL_OK && tagComputed == 0) {
        /* Compute the authTag */
        status = HAL_CRYPEx_AESGCM_Finish(&hcryp, sz, (byte*)tag, STM32_HAL_TIMEOUT);
    }
#endif

    if (status != HAL_OK)
        ret = AES_GCM_AUTH_E;

    HAL_CRYP_DeInit(&hcryp);

#else /* Standard Peripheral Library */
    ByteReverseWords(keyCopy, (word32*)aes->key, aes->keylen);

    /* Input size and auth size need to be the actual sizes, even though
     * they are not block aligned, because this length (in bits) is used
     * in the final GHASH. */
    XMEMSET(partialBlock, 0, sizeof(partialBlock)); /* use this to get tag */
    status = CRYP_AES_GCM(MODE_DECRYPT, (uint8_t*)ctr,
                         (uint8_t*)keyCopy,      keySize * 8,
                         (uint8_t*)in,           sz,
                         (uint8_t*)authInPadded, authInSz,
                         (uint8_t*)out,          (uint8_t*)partialBlock);
    if (status != SUCCESS)
        ret = AES_GCM_AUTH_E;
    if (tagComputed == 0)
        XMEMCPY(tag, partialBlock, authTagSz);
#endif /* WOLFSSL_STM32_CUBEMX */
    wolfSSL_CryptHwMutexUnLock();
    wc_Stm32_Aes_Cleanup();

    /* Check authentication tag */
    if (ConstantCompare((const byte*)tagExpected, (byte*)tag, authTagSz) != 0) {
        ret = AES_GCM_AUTH_E;
    }

    /* Free memory */
    if (wasAlloc) {
        XFREE(authInPadded, aes->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

#endif /* STM32_CRYPTO_AES_GCM */

#ifdef WOLFSSL_AESNI
/* For performance reasons, this code needs to be not inlined. */
int WARN_UNUSED_RESULT AES_GCM_decrypt_C(
                      Aes* aes, byte* out, const byte* in, word32 sz,
                      const byte* iv, word32 ivSz,
                      const byte* authTag, word32 authTagSz,
                      const byte* authIn, word32 authInSz);
#else
static
#endif
int WARN_UNUSED_RESULT AES_GCM_decrypt_C(
                      Aes* aes, byte* out, const byte* in, word32 sz,
                      const byte* iv, word32 ivSz,
                      const byte* authTag, word32 authTagSz,
                      const byte* authIn, word32 authInSz)
{
    int ret;
    word32 blocks = sz / AES_BLOCK_SIZE;
    word32 partial = sz % AES_BLOCK_SIZE;
    const byte* c = in;
    byte* p = out;
    ALIGN16 byte counter[AES_BLOCK_SIZE];
    ALIGN16 byte scratch[AES_BLOCK_SIZE];
    ALIGN16 byte Tprime[AES_BLOCK_SIZE];
    ALIGN16 byte EKY0[AES_BLOCK_SIZE];
    sword32 res;

    if (ivSz == GCM_NONCE_MID_SZ) {
        /* Counter is IV with bottom 4 bytes set to: 0x00,0x00,0x00,0x01. */
        XMEMCPY(counter, iv, ivSz);
        XMEMSET(counter + GCM_NONCE_MID_SZ, 0,
                                         AES_BLOCK_SIZE - GCM_NONCE_MID_SZ - 1);
        counter[AES_BLOCK_SIZE - 1] = 1;
    }
    else {
        /* Counter is GHASH of IV. */
#ifdef OPENSSL_EXTRA
        word32 aadTemp = aes->gcm.aadLen;
        aes->gcm.aadLen = 0;
#endif
        GHASH(&aes->gcm, NULL, 0, iv, ivSz, counter, AES_BLOCK_SIZE);
#ifdef OPENSSL_EXTRA
        aes->gcm.aadLen = aadTemp;
#endif
    }

    /* Calc the authTag again using received auth data and the cipher text */
    GHASH(&aes->gcm, authIn, authInSz, in, sz, Tprime, sizeof(Tprime));
    ret = wc_AesEncrypt(aes, counter, EKY0);
    if (ret != 0)
        return ret;
    xorbuf(Tprime, EKY0, sizeof(Tprime));
#ifdef WC_AES_GCM_DEC_AUTH_EARLY
    /* ConstantCompare returns the cumulative bitwise or of the bitwise xor of
     * the pairwise bytes in the strings.
     */
    res = ConstantCompare(authTag, Tprime, authTagSz);
    /* convert positive retval from ConstantCompare() to all-1s word, in
     * constant time.
     */
    res = 0 - (sword32)(((word32)(0 - res)) >> 31U);
    ret = res & AES_GCM_AUTH_E;
    if (ret != 0)
        return ret;
#endif

#ifdef OPENSSL_EXTRA
    if (!out) {
        /* authenticated, non-confidential data */
        /* store AAD size for next call */
        aes->gcm.aadLen = authInSz;
    }
#endif

#if defined(WOLFSSL_PIC32MZ_CRYPT)
    if (blocks) {
        /* use initial IV for HW, but don't use it below */
        XMEMCPY(aes->reg, counter, AES_BLOCK_SIZE);

        ret = wc_Pic32AesCrypt(
            aes->key, aes->keylen, aes->reg, AES_BLOCK_SIZE,
            out, in, (blocks * AES_BLOCK_SIZE),
            PIC32_DECRYPTION, PIC32_ALGO_AES, PIC32_CRYPTOALGO_AES_GCM);
        if (ret != 0)
            return ret;
    }
    /* process remainder using partial handling */
#endif

#if defined(HAVE_AES_ECB) && !defined(WOLFSSL_PIC32MZ_CRYPT)
    /* some hardware acceleration can gain performance from doing AES encryption
     * of the whole buffer at once */
    if (c != p && blocks > 0) { /* can not handle inline decryption */
        while (blocks--) {
            IncrementGcmCounter(counter);
            XMEMCPY(p, counter, AES_BLOCK_SIZE);
            p += AES_BLOCK_SIZE;
        }

        /* reset number of blocks and then do encryption */
        blocks = sz / AES_BLOCK_SIZE;

        wc_AesEcbEncrypt(aes, out, out, AES_BLOCK_SIZE * blocks);
        xorbuf(out, c, AES_BLOCK_SIZE * blocks);
        c += AES_BLOCK_SIZE * blocks;
    }
    else
#endif /* HAVE_AES_ECB && !PIC32MZ */
    {
        while (blocks--) {
            IncrementGcmCounter(counter);
        #if !defined(WOLFSSL_PIC32MZ_CRYPT)
            ret = wc_AesEncrypt(aes, counter, scratch);
            if (ret != 0)
                return ret;
            xorbufout(p, scratch, c, AES_BLOCK_SIZE);
        #endif
            p += AES_BLOCK_SIZE;
            c += AES_BLOCK_SIZE;
        }
    }

    if (partial != 0) {
        IncrementGcmCounter(counter);
        ret = wc_AesEncrypt(aes, counter, scratch);
        if (ret != 0)
            return ret;
        xorbuf(scratch, c, partial);
        XMEMCPY(p, scratch, partial);
    }

#ifndef WC_AES_GCM_DEC_AUTH_EARLY
    /* ConstantCompare returns the cumulative bitwise or of the bitwise xor of
     * the pairwise bytes in the strings.
     */
    res = ConstantCompare(authTag, Tprime, (int)authTagSz);
    /* convert positive retval from ConstantCompare() to all-1s word, in
     * constant time.
     */
    res = 0 - (sword32)(((word32)(0 - res)) >> 31U);
    /* now use res as a mask for constant time return of ret, unless tag
     * mismatch, whereupon AES_GCM_AUTH_E is returned.
     */
    ret = (ret & ~res) | (res & AES_GCM_AUTH_E);
#endif
    return ret;
}

/* Software AES - GCM Decrypt */
int wc_AesGcmDecrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                     const byte* iv, word32 ivSz,
                     const byte* authTag, word32 authTagSz,
                     const byte* authIn, word32 authInSz)
{
#ifdef WOLFSSL_AESNI
    int res = AES_GCM_AUTH_E;
#endif

    /* argument checks */
    /* If the sz is non-zero, both in and out must be set. If sz is 0,
     * in and out are don't cares, as this is is the GMAC case. */
    if (aes == NULL || iv == NULL || (sz != 0 && (in == NULL || out == NULL)) ||
        authTag == NULL || authTagSz > AES_BLOCK_SIZE || authTagSz == 0 ||
        ivSz == 0) {

        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (aes->devId != INVALID_DEVID)
    #endif
    {
        int crypto_cb_ret =
            wc_CryptoCb_AesGcmDecrypt(aes, out, in, sz, iv, ivSz,
                                      authTag, authTagSz, authIn, authInSz);
        if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
            return crypto_cb_ret;
        /* fall-through when unavailable */
    }
#endif

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)
    /* if async and byte count above threshold */
    /* only 12-byte IV is supported in HW */
    if (aes->asyncDev.marker == WOLFSSL_ASYNC_MARKER_AES &&
                    sz >= WC_ASYNC_THRESH_AES_GCM && ivSz == GCM_NONCE_MID_SZ) {
    #if defined(HAVE_CAVIUM)
        #ifdef HAVE_CAVIUM_V
        if (authInSz == 20) { /* Nitrox V GCM is only working with 20 byte AAD */
            return NitroxAesGcmDecrypt(aes, out, in, sz,
                (const byte*)aes->devKey, aes->keylen, iv, ivSz,
                authTag, authTagSz, authIn, authInSz);
        }
        #endif
    #elif defined(HAVE_INTEL_QA)
        return IntelQaSymAesGcmDecrypt(&aes->asyncDev, out, in, sz,
            (const byte*)aes->devKey, aes->keylen, iv, ivSz,
            authTag, authTagSz, authIn, authInSz);
    #else /* WOLFSSL_ASYNC_CRYPT_SW */
        if (wc_AsyncSwInit(&aes->asyncDev, ASYNC_SW_AES_GCM_DECRYPT)) {
            WC_ASYNC_SW* sw = &aes->asyncDev.sw;
            sw->aes.aes = aes;
            sw->aes.out = out;
            sw->aes.in = in;
            sw->aes.sz = sz;
            sw->aes.iv = iv;
            sw->aes.ivSz = ivSz;
            sw->aes.authTag = (byte*)authTag;
            sw->aes.authTagSz = authTagSz;
            sw->aes.authIn = authIn;
            sw->aes.authInSz = authInSz;
            return WC_PENDING_E;
        }
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

#ifdef WOLFSSL_SILABS_SE_ACCEL
    return wc_AesGcmDecrypt_silabs(
        aes, out, in, sz, iv, ivSz,
        authTag, authTagSz, authIn, authInSz);

#endif

#ifdef STM32_CRYPTO_AES_GCM
    /* The STM standard peripheral library API's doesn't support partial blocks */
    return wc_AesGcmDecrypt_STM32(
        aes, out, in, sz, iv, ivSz,
        authTag, authTagSz, authIn, authInSz);
#endif /* STM32_CRYPTO_AES_GCM */

#ifdef WOLFSSL_AESNI
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_decrypt_avx2(in, out, authIn, iv, authTag, sz, authInSz, ivSz,
                            authTagSz, (byte*)aes->key, (int)aes->rounds, &res);
        RESTORE_VECTOR_REGISTERS();
        if (res == 0)
            return AES_GCM_AUTH_E;
        return 0;
    }
    else
    #endif
    #if defined(HAVE_INTEL_AVX1)
    if (IS_INTEL_AVX1(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_decrypt_avx1(in, out, authIn, iv, authTag, sz, authInSz, ivSz,
                            authTagSz, (byte*)aes->key, (int)aes->rounds, &res);
        RESTORE_VECTOR_REGISTERS();
        if (res == 0)
            return AES_GCM_AUTH_E;
        return 0;
    }
    else
    #endif
    if (haveAESNI) {
        AES_GCM_decrypt(in, out, authIn, iv, authTag, sz, authInSz, ivSz,
                            authTagSz, (byte*)aes->key, (int)aes->rounds, &res);
        if (res == 0)
            return AES_GCM_AUTH_E;
        return 0;
    }
    else
#endif
    {
        return AES_GCM_decrypt_C(aes, out, in, sz, iv, ivSz, authTag, authTagSz,
                                                              authIn, authInSz);
    }
}
#endif
#endif /* HAVE_AES_DECRYPT || HAVE_AESGCM_DECRYPT */

#ifdef WOLFSSL_AESGCM_STREAM
/* Initialize the AES GCM cipher with an IV. C implementation.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      iv    IV/nonce buffer.
 * @param [in]      ivSz  Length of IV/nonce data.
 */
static WARN_UNUSED_RESULT int AesGcmInit_C(Aes* aes, const byte* iv, word32 ivSz)
{
    ALIGN32 byte counter[AES_BLOCK_SIZE];
    int ret;

    if (ivSz == GCM_NONCE_MID_SZ) {
        /* Counter is IV with bottom 4 bytes set to: 0x00,0x00,0x00,0x01. */
        XMEMCPY(counter, iv, ivSz);
        XMEMSET(counter + GCM_NONCE_MID_SZ, 0,
                                         AES_BLOCK_SIZE - GCM_NONCE_MID_SZ - 1);
        counter[AES_BLOCK_SIZE - 1] = 1;
    }
    else {
        /* Counter is GHASH of IV. */
    #ifdef OPENSSL_EXTRA
        word32 aadTemp = aes->gcm.aadLen;
        aes->gcm.aadLen = 0;
    #endif
        GHASH(&aes->gcm, NULL, 0, iv, ivSz, counter, AES_BLOCK_SIZE);
    #ifdef OPENSSL_EXTRA
        aes->gcm.aadLen = aadTemp;
    #endif
    }

    /* Copy in the counter for use with cipher. */
    XMEMCPY(AES_COUNTER(aes), counter, AES_BLOCK_SIZE);
    /* Encrypt initial counter into a buffer for GCM. */
    ret = wc_AesEncrypt(aes, counter, AES_INITCTR(aes));
    if (ret != 0)
        return ret;
    /* Reset state fields. */
    aes->over = 0;
    aes->aSz = 0;
    aes->cSz = 0;
    /* Initialization for GHASH. */
    GHASH_INIT(aes);

    return 0;
}

/* Update the AES GCM cipher with data. C implementation.
 *
 * Only enciphers data.
 *
 * @param [in, out] aes  AES object.
 * @param [in]      out  Cipher text or plaintext buffer.
 * @param [in]      in   Plaintext or cipher text buffer.
 * @param [in]      sz   Length of data.
 */
static WARN_UNUSED_RESULT int AesGcmCryptUpdate_C(
    Aes* aes, byte* out, const byte* in, word32 sz)
{
    word32 blocks;
    word32 partial;
    int ret;

    /* Check if previous encrypted block was not used up. */
    if (aes->over > 0) {
        byte pSz = AES_BLOCK_SIZE - aes->over;
        if (pSz > sz) pSz = (byte)sz;

        /* Use some/all of last encrypted block. */
        xorbufout(out, AES_LASTBLOCK(aes) + aes->over, in, pSz);
        aes->over = (aes->over + pSz) & (AES_BLOCK_SIZE - 1);

        /* Some data used. */
        sz  -= pSz;
        in  += pSz;
        out += pSz;
    }

    /* Calculate the number of blocks needing to be encrypted and any leftover.
     */
    blocks  = sz / AES_BLOCK_SIZE;
    partial = sz & (AES_BLOCK_SIZE - 1);

#if defined(HAVE_AES_ECB)
    /* Some hardware acceleration can gain performance from doing AES encryption
     * of the whole buffer at once.
     * Overwrites the cipher text before using plaintext - no inline encryption.
     */
    if ((out != in) && blocks > 0) {
        word32 b;
        /* Place incrementing counter blocks into cipher text. */
        for (b = 0; b < blocks; b++) {
            IncrementGcmCounter(AES_COUNTER(aes));
            XMEMCPY(out + b * AES_BLOCK_SIZE, AES_COUNTER(aes), AES_BLOCK_SIZE);
        }

        /* Encrypt counter blocks. */
        wc_AesEcbEncrypt(aes, out, out, AES_BLOCK_SIZE * blocks);
        /* XOR in plaintext. */
        xorbuf(out, in, AES_BLOCK_SIZE * blocks);
        /* Skip over processed data. */
        in += AES_BLOCK_SIZE * blocks;
        out += AES_BLOCK_SIZE * blocks;
    }
    else
#endif /* HAVE_AES_ECB */
    {
        /* Encrypt block by block. */
        while (blocks--) {
            ALIGN32 byte scratch[AES_BLOCK_SIZE];
            IncrementGcmCounter(AES_COUNTER(aes));
            /* Encrypt counter into a buffer. */
            ret = wc_AesEncrypt(aes, AES_COUNTER(aes), scratch);
            if (ret != 0)
                return ret;
            /* XOR plain text into encrypted counter into cipher text buffer. */
            xorbufout(out, scratch, in, AES_BLOCK_SIZE);
            /* Data complete. */
            in  += AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
        }
    }

    if (partial != 0) {
        /* Generate an extra block and use up as much as needed. */
        IncrementGcmCounter(AES_COUNTER(aes));
        /* Encrypt counter into cache. */
        ret = wc_AesEncrypt(aes, AES_COUNTER(aes), AES_LASTBLOCK(aes));
        if (ret != 0)
            return ret;
        /* XOR plain text into encrypted counter into cipher text buffer. */
        xorbufout(out, AES_LASTBLOCK(aes), in, partial);
        /* Keep amount of encrypted block used. */
        aes->over = (byte)partial;
    }

    return 0;
}

/* Calculates authentication tag for AES GCM. C implementation.
 *
 * @param [in, out] aes        AES object.
 * @param [out]     authTag    Buffer to store authentication tag in.
 * @param [in]      authTagSz  Length of tag to create.
 */
static WARN_UNUSED_RESULT int AesGcmFinal_C(
    Aes* aes, byte* authTag, word32 authTagSz)
{
    /* Calculate authentication tag. */
    GHASH_FINAL(aes, authTag, authTagSz);
    /* XOR in as much of encrypted counter as is required. */
    xorbuf(authTag, AES_INITCTR(aes), authTagSz);
#ifdef OPENSSL_EXTRA
    /* store AAD size for next call */
    aes->gcm.aadLen = aes->aSz;
#endif
    /* Zeroize last block to protect sensitive data. */
    ForceZero(AES_LASTBLOCK(aes), AES_BLOCK_SIZE);

    return 0;
}

#ifdef WOLFSSL_AESNI

#ifdef __cplusplus
    extern "C" {
#endif

/* Assembly code implementations in: aes_gcm_asm.S */
#ifdef HAVE_INTEL_AVX2
extern void AES_GCM_init_avx2(const unsigned char* key, int nr,
    const unsigned char* ivec, unsigned int ibytes, unsigned char* h,
    unsigned char* counter, unsigned char* initCtr);
extern void AES_GCM_aad_update_avx2(const unsigned char* addt,
    unsigned int abytes, unsigned char* tag, unsigned char* h);
extern void AES_GCM_encrypt_block_avx2(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned char* counter);
extern void AES_GCM_ghash_block_avx2(const unsigned char* data,
    unsigned char* tag, unsigned char* h);

extern void AES_GCM_encrypt_update_avx2(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned int nbytes,
    unsigned char* tag, unsigned char* h, unsigned char* counter);
extern void AES_GCM_encrypt_final_avx2(unsigned char* tag,
    unsigned char* authTag, unsigned int tbytes, unsigned int nbytes,
    unsigned int abytes, unsigned char* h, unsigned char* initCtr);
#endif
#ifdef HAVE_INTEL_AVX1
extern void AES_GCM_init_avx1(const unsigned char* key, int nr,
    const unsigned char* ivec, unsigned int ibytes, unsigned char* h,
    unsigned char* counter, unsigned char* initCtr);
extern void AES_GCM_aad_update_avx1(const unsigned char* addt,
    unsigned int abytes, unsigned char* tag, unsigned char* h);
extern void AES_GCM_encrypt_block_avx1(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned char* counter);
extern void AES_GCM_ghash_block_avx1(const unsigned char* data,
    unsigned char* tag, unsigned char* h);

extern void AES_GCM_encrypt_update_avx1(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned int nbytes,
    unsigned char* tag, unsigned char* h, unsigned char* counter);
extern void AES_GCM_encrypt_final_avx1(unsigned char* tag,
    unsigned char* authTag, unsigned int tbytes, unsigned int nbytes,
    unsigned int abytes, unsigned char* h, unsigned char* initCtr);
#endif
extern void AES_GCM_init_aesni(const unsigned char* key, int nr,
    const unsigned char* ivec, unsigned int ibytes, unsigned char* h,
    unsigned char* counter, unsigned char* initCtr);
extern void AES_GCM_aad_update_aesni(const unsigned char* addt,
    unsigned int abytes, unsigned char* tag, unsigned char* h);
extern void AES_GCM_encrypt_block_aesni(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned char* counter);
extern void AES_GCM_ghash_block_aesni(const unsigned char* data,
    unsigned char* tag, unsigned char* h);

extern void AES_GCM_encrypt_update_aesni(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned int nbytes,
    unsigned char* tag, unsigned char* h, unsigned char* counter);
extern void AES_GCM_encrypt_final_aesni(unsigned char* tag,
    unsigned char* authTag, unsigned int tbytes, unsigned int nbytes,
    unsigned int abytes, unsigned char* h, unsigned char* initCtr);

#ifdef __cplusplus
    } /* extern "C" */
#endif

/* Initialize the AES GCM cipher with an IV. AES-NI implementations.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      iv    IV/nonce buffer.
 * @param [in]      ivSz  Length of IV/nonce data.
 */
static WARN_UNUSED_RESULT int AesGcmInit_aesni(
    Aes* aes, const byte* iv, word32 ivSz)
{
    /* Reset state fields. */
    aes->aSz = 0;
    aes->cSz = 0;
    /* Set tag to all zeros as initial value. */
    XMEMSET(AES_TAG(aes), 0, AES_BLOCK_SIZE);
    /* Reset counts of AAD and cipher text. */
    aes->aOver = 0;
    aes->cOver = 0;

#ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_init_avx2((byte*)aes->key, (int)aes->rounds, iv, ivSz,
            aes->gcm.H, AES_COUNTER(aes), AES_INITCTR(aes));
        RESTORE_VECTOR_REGISTERS();
    }
    else
#endif
#ifdef HAVE_INTEL_AVX1
    if (IS_INTEL_AVX1(intel_flags)) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_init_avx1((byte*)aes->key, (int)aes->rounds, iv, ivSz,
            aes->gcm.H, AES_COUNTER(aes), AES_INITCTR(aes));
        RESTORE_VECTOR_REGISTERS();
    }
    else
#endif
    {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_GCM_init_aesni((byte*)aes->key, (int)aes->rounds, iv, ivSz,
            aes->gcm.H, AES_COUNTER(aes), AES_INITCTR(aes));
        RESTORE_VECTOR_REGISTERS();
    }
    return 0;
}

/* Update the AES GCM for encryption with authentication data.
 *
 * Implementation uses AVX2, AVX1 or straight AES-NI optimized assembly code.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      a     Buffer holding authentication data.
 * @param [in]      aSz   Length of authentication data in bytes.
 * @param [in]      endA  Whether no more authentication data is expected.
 */
static WARN_UNUSED_RESULT int AesGcmAadUpdate_aesni(
    Aes* aes, const byte* a, word32 aSz, int endA)
{
    word32 blocks;
    int partial;

    ASSERT_SAVED_VECTOR_REGISTERS();

    if (aSz != 0 && a != NULL) {
        /* Total count of AAD updated. */
        aes->aSz += aSz;
        /* Check if we have unprocessed data. */
        if (aes->aOver > 0) {
            /* Calculate amount we can use - fill up the block. */
            byte sz = AES_BLOCK_SIZE - aes->aOver;
            if (sz > aSz) {
                sz = (byte)aSz;
            }
            /* Copy extra into last GHASH block array and update count. */
            XMEMCPY(AES_LASTGBLOCK(aes) + aes->aOver, a, sz);
            aes->aOver += sz;
            if (aes->aOver == AES_BLOCK_SIZE) {
                /* We have filled up the block and can process. */
            #ifdef HAVE_INTEL_AVX2
                if (IS_INTEL_AVX2(intel_flags)) {
                    AES_GCM_ghash_block_avx2(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                             aes->gcm.H);
                }
                else
            #endif
            #ifdef HAVE_INTEL_AVX1
                if (IS_INTEL_AVX1(intel_flags)) {
                    AES_GCM_ghash_block_avx1(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                             aes->gcm.H);
                }
                else
            #endif
                {
                    AES_GCM_ghash_block_aesni(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                              aes->gcm.H);
                }
                /* Reset count. */
                aes->aOver = 0;
            }
            /* Used up some data. */
            aSz -= sz;
            a += sz;
        }

        /* Calculate number of blocks of AAD and the leftover. */
        blocks = aSz / AES_BLOCK_SIZE;
        partial = aSz % AES_BLOCK_SIZE;
        if (blocks > 0) {
            /* GHASH full blocks now. */
        #ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_AVX2(intel_flags)) {
                AES_GCM_aad_update_avx2(a, blocks * AES_BLOCK_SIZE,
                                        AES_TAG(aes), aes->gcm.H);
            }
            else
        #endif
        #ifdef HAVE_INTEL_AVX1
            if (IS_INTEL_AVX1(intel_flags)) {
                AES_GCM_aad_update_avx1(a, blocks * AES_BLOCK_SIZE,
                                        AES_TAG(aes), aes->gcm.H);
            }
            else
        #endif
            {
                AES_GCM_aad_update_aesni(a, blocks * AES_BLOCK_SIZE,
                                         AES_TAG(aes), aes->gcm.H);
            }
            /* Skip over to end of AAD blocks. */
            a += blocks * AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            /* Cache the partial block. */
            XMEMCPY(AES_LASTGBLOCK(aes), a, (size_t)partial);
            aes->aOver = (byte)partial;
        }
    }
    if (endA && (aes->aOver > 0)) {
        /* No more AAD coming and we have a partial block. */
        /* Fill the rest of the block with zeros. */
        XMEMSET(AES_LASTGBLOCK(aes) + aes->aOver, 0,
                AES_BLOCK_SIZE - aes->aOver);
        /* GHASH last AAD block. */
    #ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_AVX2(intel_flags)) {
            AES_GCM_ghash_block_avx2(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                     aes->gcm.H);
        }
        else
    #endif
    #ifdef HAVE_INTEL_AVX1
        if (IS_INTEL_AVX1(intel_flags)) {
            AES_GCM_ghash_block_avx1(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                     aes->gcm.H);
        }
        else
    #endif
        {
            AES_GCM_ghash_block_aesni(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                      aes->gcm.H);
        }
        /* Clear partial count for next time through. */
        aes->aOver = 0;
    }

    return 0;
}

/* Update the AES GCM for encryption with data and/or authentication data.
 *
 * Implementation uses AVX2, AVX1 or straight AES-NI optimized assembly code.
 *
 * @param [in, out] aes  AES object.
 * @param [out]     c    Buffer to hold cipher text.
 * @param [in]      p    Buffer holding plaintext.
 * @param [in]      cSz  Length of cipher text/plaintext in bytes.
 * @param [in]      a    Buffer holding authentication data.
 * @param [in]      aSz  Length of authentication data in bytes.
 */
static WARN_UNUSED_RESULT int AesGcmEncryptUpdate_aesni(
    Aes* aes, byte* c, const byte* p, word32 cSz, const byte* a, word32 aSz)
{
    word32 blocks;
    int partial;
    int ret;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    /* Hash in A, the Authentication Data */
    ret = AesGcmAadUpdate_aesni(aes, a, aSz, (cSz > 0) && (c != NULL));
    if (ret != 0)
        return ret;

    /* Encrypt plaintext and Hash in C, the Cipher text */
    if (cSz != 0 && c != NULL) {
        /* Update count of cipher text we have hashed. */
        aes->cSz += cSz;
        if (aes->cOver > 0) {
            /* Calculate amount we can use - fill up the block. */
            byte sz = AES_BLOCK_SIZE - aes->cOver;
            if (sz > cSz) {
                sz = (byte)cSz;
            }
            /* Encrypt some of the plaintext. */
            xorbuf(AES_LASTGBLOCK(aes) + aes->cOver, p, sz);
            XMEMCPY(c, AES_LASTGBLOCK(aes) + aes->cOver, sz);
            /* Update count of unused encrypted counter. */
            aes->cOver += sz;
            if (aes->cOver == AES_BLOCK_SIZE) {
                /* We have filled up the block and can process. */
            #ifdef HAVE_INTEL_AVX2
                if (IS_INTEL_AVX2(intel_flags)) {
                    AES_GCM_ghash_block_avx2(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                             aes->gcm.H);
                }
                else
            #endif
            #ifdef HAVE_INTEL_AVX1
                if (IS_INTEL_AVX1(intel_flags)) {
                    AES_GCM_ghash_block_avx1(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                             aes->gcm.H);
                }
                else
            #endif
                {
                    AES_GCM_ghash_block_aesni(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                              aes->gcm.H);
                }
                /* Reset count. */
                aes->cOver = 0;
            }
            /* Used up some data. */
            cSz -= sz;
            p += sz;
            c += sz;
        }

        /* Calculate number of blocks of plaintext and the leftover. */
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        if (blocks > 0) {
            /* Encrypt and GHASH full blocks now. */
        #ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_AVX2(intel_flags)) {
                AES_GCM_encrypt_update_avx2((byte*)aes->key, (int)aes->rounds,
                    c, p, blocks * AES_BLOCK_SIZE, AES_TAG(aes), aes->gcm.H,
                    AES_COUNTER(aes));
            }
            else
        #endif
        #ifdef HAVE_INTEL_AVX1
            if (IS_INTEL_AVX1(intel_flags)) {
                AES_GCM_encrypt_update_avx1((byte*)aes->key, (int)aes->rounds,
                    c, p, blocks * AES_BLOCK_SIZE, AES_TAG(aes), aes->gcm.H,
                    AES_COUNTER(aes));
            }
            else
        #endif
            {
                AES_GCM_encrypt_update_aesni((byte*)aes->key, (int)aes->rounds,
                    c, p, blocks * AES_BLOCK_SIZE, AES_TAG(aes), aes->gcm.H,
                    AES_COUNTER(aes));
            }
            /* Skip over to end of blocks. */
            p += blocks * AES_BLOCK_SIZE;
            c += blocks * AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            /* Encrypt the counter - XOR in zeros as proxy for plaintext. */
            XMEMSET(AES_LASTGBLOCK(aes), 0, AES_BLOCK_SIZE);
        #ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_AVX2(intel_flags)) {
                AES_GCM_encrypt_block_avx2((byte*)aes->key, (int)aes->rounds,
                    AES_LASTGBLOCK(aes), AES_LASTGBLOCK(aes), AES_COUNTER(aes));
            }
            else
        #endif
        #ifdef HAVE_INTEL_AVX1
            if (IS_INTEL_AVX1(intel_flags)) {
                AES_GCM_encrypt_block_avx1((byte*)aes->key, (int)aes->rounds,
                    AES_LASTGBLOCK(aes), AES_LASTGBLOCK(aes), AES_COUNTER(aes));
            }
            else
        #endif
            {
                AES_GCM_encrypt_block_aesni((byte*)aes->key, (int)aes->rounds,
                    AES_LASTGBLOCK(aes), AES_LASTGBLOCK(aes), AES_COUNTER(aes));
            }
            /* XOR the remaining plaintext to calculate cipher text.
             * Keep cipher text for GHASH of last partial block.
             */
            xorbuf(AES_LASTGBLOCK(aes), p, (word32)partial);
            XMEMCPY(c, AES_LASTGBLOCK(aes), (size_t)partial);
            /* Update count of the block used. */
            aes->cOver = (byte)partial;
        }
    }
    RESTORE_VECTOR_REGISTERS();
    return 0;
}

/* Finalize the AES GCM for encryption and calculate the authentication tag.
 *
 * Calls AVX2, AVX1 or straight AES-NI optimized assembly code.
 *
 * @param [in, out] aes        AES object.
 * @param [in]      authTag    Buffer to hold authentication tag.
 * @param [in]      authTagSz  Length of authentication tag in bytes.
 * @return  0 on success.
 */
static WARN_UNUSED_RESULT int AesGcmEncryptFinal_aesni(
    Aes* aes, byte* authTag, word32 authTagSz)
{
    /* AAD block incomplete when > 0 */
    byte over = aes->aOver;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    if (aes->cOver > 0) {
        /* Cipher text block incomplete. */
        over = aes->cOver;
    }
    if (over > 0) {
        /* Fill the rest of the block with zeros. */
        XMEMSET(AES_LASTGBLOCK(aes) + over, 0, AES_BLOCK_SIZE - over);
        /* GHASH last cipher block. */
    #ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_AVX2(intel_flags)) {
            AES_GCM_ghash_block_avx2(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                     aes->gcm.H);
        }
        else
    #endif
    #ifdef HAVE_INTEL_AVX1
        if (IS_INTEL_AVX1(intel_flags)) {
            AES_GCM_ghash_block_avx1(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                     aes->gcm.H);
        }
        else
    #endif
        {
            AES_GCM_ghash_block_aesni(AES_LASTGBLOCK(aes), AES_TAG(aes),
                                      aes->gcm.H);
        }
    }
    /* Calculate the authentication tag. */
#ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        AES_GCM_encrypt_final_avx2(AES_TAG(aes), authTag, authTagSz, aes->cSz,
            aes->aSz, aes->gcm.H, AES_INITCTR(aes));
    }
    else
#endif
#ifdef HAVE_INTEL_AVX1
    if (IS_INTEL_AVX1(intel_flags)) {
        AES_GCM_encrypt_final_avx1(AES_TAG(aes), authTag, authTagSz, aes->cSz,
            aes->aSz, aes->gcm.H, AES_INITCTR(aes));
    }
    else
#endif
    {
        AES_GCM_encrypt_final_aesni(AES_TAG(aes), authTag, authTagSz, aes->cSz,
            aes->aSz, aes->gcm.H, AES_INITCTR(aes));
    }
    RESTORE_VECTOR_REGISTERS();
    return 0;
}

#if defined(HAVE_AES_DECRYPT) || defined(HAVE_AESGCM_DECRYPT)

#ifdef __cplusplus
    extern "C" {
#endif

/* Assembly code implementations in: aes_gcm_asm.S and aes_gcm_x86_asm.S */
#ifdef HAVE_INTEL_AVX2
extern void AES_GCM_decrypt_update_avx2(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned int nbytes,
    unsigned char* tag, unsigned char* h, unsigned char* counter);
extern void AES_GCM_decrypt_final_avx2(unsigned char* tag,
    const unsigned char* authTag, unsigned int tbytes, unsigned int nbytes,
    unsigned int abytes, unsigned char* h, unsigned char* initCtr, int* res);
#endif
#ifdef HAVE_INTEL_AVX1
extern void AES_GCM_decrypt_update_avx1(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned int nbytes,
    unsigned char* tag, unsigned char* h, unsigned char* counter);
extern void AES_GCM_decrypt_final_avx1(unsigned char* tag,
    const unsigned char* authTag, unsigned int tbytes, unsigned int nbytes,
    unsigned int abytes, unsigned char* h, unsigned char* initCtr, int* res);
#endif
extern void AES_GCM_decrypt_update_aesni(const unsigned char* key, int nr,
    unsigned char* out, const unsigned char* in, unsigned int nbytes,
    unsigned char* tag, unsigned char* h, unsigned char* counter);
extern void AES_GCM_decrypt_final_aesni(unsigned char* tag,
    const unsigned char* authTag, unsigned int tbytes, unsigned int nbytes,
    unsigned int abytes, unsigned char* h, unsigned char* initCtr, int* res);

#ifdef __cplusplus
    } /* extern "C" */
#endif

/* Update the AES GCM for decryption with data and/or authentication data.
 *
 * @param [in, out] aes  AES object.
 * @param [out]     p    Buffer to hold plaintext.
 * @param [in]      c    Buffer holding cipher text.
 * @param [in]      cSz  Length of cipher text/plaintext in bytes.
 * @param [in]      a    Buffer holding authentication data.
 * @param [in]      aSz  Length of authentication data in bytes.
 */
static WARN_UNUSED_RESULT int AesGcmDecryptUpdate_aesni(
    Aes* aes, byte* p, const byte* c, word32 cSz, const byte* a, word32 aSz)
{
    word32 blocks;
    int partial;
    int ret;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    /* Hash in A, the Authentication Data */
    ret = AesGcmAadUpdate_aesni(aes, a, aSz, (cSz > 0) && (c != NULL));
    if (ret != 0)
        return ret;

    /* Hash in C, the Cipher text, and decrypt. */
    if (cSz != 0 && p != NULL) {
        /* Update count of cipher text we have hashed. */
        aes->cSz += cSz;
        if (aes->cOver > 0) {
            /* Calculate amount we can use - fill up the block. */
            byte sz = AES_BLOCK_SIZE - aes->cOver;
            if (sz > cSz) {
                sz = (byte)cSz;
            }
            /* Keep a copy of the cipher text for GHASH. */
            XMEMCPY(AES_LASTBLOCK(aes) + aes->cOver, c, sz);
            /* Decrypt some of the cipher text. */
            xorbuf(AES_LASTGBLOCK(aes) + aes->cOver, c, sz);
            XMEMCPY(p, AES_LASTGBLOCK(aes) + aes->cOver, sz);
            /* Update count of unused encrypted counter. */
            aes->cOver += sz;
            if (aes->cOver == AES_BLOCK_SIZE) {
                /* We have filled up the block and can process. */
            #ifdef HAVE_INTEL_AVX2
                if (IS_INTEL_AVX2(intel_flags)) {
                    AES_GCM_ghash_block_avx2(AES_LASTBLOCK(aes), AES_TAG(aes),
                                             aes->gcm.H);
                }
                else
            #endif
            #ifdef HAVE_INTEL_AVX1
                if (IS_INTEL_AVX1(intel_flags)) {
                    AES_GCM_ghash_block_avx1(AES_LASTBLOCK(aes), AES_TAG(aes),
                                             aes->gcm.H);
                }
                else
            #endif
                {
                    AES_GCM_ghash_block_aesni(AES_LASTBLOCK(aes), AES_TAG(aes),
                                              aes->gcm.H);
                }
                /* Reset count. */
                aes->cOver = 0;
            }
            /* Used up some data. */
            cSz -= sz;
            c += sz;
            p += sz;
        }

        /* Calculate number of blocks of plaintext and the leftover. */
        blocks = cSz / AES_BLOCK_SIZE;
        partial = cSz % AES_BLOCK_SIZE;
        if (blocks > 0) {
            /* Decrypt and GHASH full blocks now. */
        #ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_AVX2(intel_flags)) {
                AES_GCM_decrypt_update_avx2((byte*)aes->key, (int)aes->rounds,
                    p, c, blocks * AES_BLOCK_SIZE, AES_TAG(aes), aes->gcm.H,
                    AES_COUNTER(aes));
            }
            else
        #endif
        #ifdef HAVE_INTEL_AVX1
            if (IS_INTEL_AVX1(intel_flags)) {
                AES_GCM_decrypt_update_avx1((byte*)aes->key, (int)aes->rounds,
                    p, c, blocks * AES_BLOCK_SIZE, AES_TAG(aes), aes->gcm.H,
                    AES_COUNTER(aes));
            }
            else
        #endif
            {
                AES_GCM_decrypt_update_aesni((byte*)aes->key, (int)aes->rounds,
                    p, c, blocks * AES_BLOCK_SIZE, AES_TAG(aes), aes->gcm.H,
                    AES_COUNTER(aes));
            }
            /* Skip over to end of blocks. */
            c += blocks * AES_BLOCK_SIZE;
            p += blocks * AES_BLOCK_SIZE;
        }
        if (partial != 0) {
            /* Encrypt the counter - XOR in zeros as proxy for cipher text. */
            XMEMSET(AES_LASTGBLOCK(aes), 0, AES_BLOCK_SIZE);
        #ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_AVX2(intel_flags)) {
                AES_GCM_encrypt_block_avx2((byte*)aes->key, (int)aes->rounds,
                    AES_LASTGBLOCK(aes), AES_LASTGBLOCK(aes), AES_COUNTER(aes));
            }
            else
        #endif
        #ifdef HAVE_INTEL_AVX1
            if (IS_INTEL_AVX1(intel_flags)) {
                AES_GCM_encrypt_block_avx1((byte*)aes->key, (int)aes->rounds,
                    AES_LASTGBLOCK(aes), AES_LASTGBLOCK(aes), AES_COUNTER(aes));
            }
            else
        #endif
            {
                AES_GCM_encrypt_block_aesni((byte*)aes->key, (int)aes->rounds,
                    AES_LASTGBLOCK(aes), AES_LASTGBLOCK(aes), AES_COUNTER(aes));
            }
            /* Keep cipher text for GHASH of last partial block. */
            XMEMCPY(AES_LASTBLOCK(aes), c, (size_t)partial);
            /* XOR the remaining cipher text to calculate plaintext. */
            xorbuf(AES_LASTGBLOCK(aes), c, (word32)partial);
            XMEMCPY(p, AES_LASTGBLOCK(aes), (size_t)partial);
            /* Update count of the block used. */
            aes->cOver = (byte)partial;
        }
    }
    RESTORE_VECTOR_REGISTERS();
    return 0;
}

/* Finalize the AES GCM for decryption and check the authentication tag.
 *
 * Calls AVX2, AVX1 or straight AES-NI optimized assembly code.
 *
 * @param [in, out] aes        AES object.
 * @param [in]      authTag    Buffer holding authentication tag.
 * @param [in]      authTagSz  Length of authentication tag in bytes.
 * @return  0 on success.
 * @return  AES_GCM_AUTH_E when authentication tag doesn't match calculated
 *          value.
 */
static WARN_UNUSED_RESULT int AesGcmDecryptFinal_aesni(
    Aes* aes, const byte* authTag, word32 authTagSz)
{
    int ret = 0;
    int res;
    /* AAD block incomplete when > 0 */
    byte over = aes->aOver;
    byte *lastBlock = AES_LASTGBLOCK(aes);

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    if (aes->cOver > 0) {
        /* Cipher text block incomplete. */
        over = aes->cOver;
        lastBlock = AES_LASTBLOCK(aes);
    }
    if (over > 0) {
        /* Zeroize the unused part of the block. */
        XMEMSET(lastBlock + over, 0, AES_BLOCK_SIZE - over);
        /* Hash the last block of cipher text. */
    #ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_AVX2(intel_flags)) {
            AES_GCM_ghash_block_avx2(lastBlock, AES_TAG(aes), aes->gcm.H);
        }
        else
    #endif
    #ifdef HAVE_INTEL_AVX1
        if (IS_INTEL_AVX1(intel_flags)) {
            AES_GCM_ghash_block_avx1(lastBlock, AES_TAG(aes), aes->gcm.H);
        }
        else
    #endif
        {
            AES_GCM_ghash_block_aesni(lastBlock, AES_TAG(aes), aes->gcm.H);
        }
    }
    /* Calculate and compare the authentication tag. */
#ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        AES_GCM_decrypt_final_avx2(AES_TAG(aes), authTag, authTagSz, aes->cSz,
            aes->aSz, aes->gcm.H, AES_INITCTR(aes), &res);
    }
    else
#endif
#ifdef HAVE_INTEL_AVX1
    if (IS_INTEL_AVX1(intel_flags)) {
        AES_GCM_decrypt_final_avx1(AES_TAG(aes), authTag, authTagSz, aes->cSz,
            aes->aSz, aes->gcm.H, AES_INITCTR(aes), &res);
    }
    else
#endif
    {
        AES_GCM_decrypt_final_aesni(AES_TAG(aes), authTag, authTagSz, aes->cSz,
            aes->aSz, aes->gcm.H, AES_INITCTR(aes), &res);
    }
    RESTORE_VECTOR_REGISTERS();
    /* Return error code when calculated doesn't match input. */
    if (res == 0) {
        ret = AES_GCM_AUTH_E;
    }
    return ret;
}
#endif /* HAVE_AES_DECRYPT || HAVE_AESGCM_DECRYPT */
#endif /* WOLFSSL_AESNI */

/* Initialize an AES GCM cipher for encryption or decryption.
 *
 * Must call wc_AesInit() before calling this function.
 * Call wc_AesGcmSetIV() before calling this function to generate part of IV.
 * Call wc_AesGcmSetExtIV() before calling this function to cache IV.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      key   Buffer holding key.
 * @param [in]      len   Length of key in bytes.
 * @param [in]      iv    Buffer holding IV/nonce.
 * @param [in]      ivSz  Length of IV/nonce in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when aes is NULL, or a length is non-zero but buffer
 *          is NULL, or the IV is NULL and no previous IV has been set.
 * @return  MEMORY_E when dynamic memory allocation fails. (WOLFSSL_SMALL_STACK)
 */
int wc_AesGcmInit(Aes* aes, const byte* key, word32 len, const byte* iv,
    word32 ivSz)
{
    int ret = 0;

    /* Check validity of parameters. */
    if ((aes == NULL) || ((len > 0) && (key == NULL)) ||
            ((ivSz == 0) && (iv != NULL)) ||
            ((ivSz > 0) && (iv == NULL))) {
        ret = BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_AESNI)
    if ((ret == 0) && (aes->streamData == NULL)) {
        /* Allocate buffers for streaming. */
        aes->streamData = (byte*)XMALLOC(5 * AES_BLOCK_SIZE, aes->heap,
                                                              DYNAMIC_TYPE_AES);
        if (aes->streamData == NULL) {
            ret = MEMORY_E;
        }
    }
#endif

    /* Set the key if passed in. */
    if ((ret == 0) && (key != NULL)) {
        ret = wc_AesGcmSetKey(aes, key, len);
    }

    if (ret == 0) {
        /* Set the IV passed in if it is smaller than a block. */
        if ((iv != NULL) && (ivSz <= AES_BLOCK_SIZE)) {
            XMEMMOVE((byte*)aes->reg, iv, ivSz);
            aes->nonceSz = ivSz;
        }
        /* No IV passed in, check for cached IV. */
        if ((iv == NULL) && (aes->nonceSz != 0)) {
            /* Use the cached copy. */
            iv = (byte*)aes->reg;
            ivSz = aes->nonceSz;
        }

        if (iv != NULL) {
            /* Initialize with the IV. */
        #ifdef WOLFSSL_AESNI
            if (haveAESNI
            #ifdef HAVE_INTEL_AVX2
                || IS_INTEL_AVX2(intel_flags)
            #endif
            #ifdef HAVE_INTEL_AVX1
                || IS_INTEL_AVX1(intel_flags)
            #endif
                ) {
                ret = AesGcmInit_aesni(aes, iv, ivSz);
            }
            else
        #endif
            {
                ret = AesGcmInit_C(aes, iv, ivSz);
            }

            aes->nonceSet = 1;
        }
    }

    return ret;
}

/* Initialize an AES GCM cipher for encryption.
 *
 * Must call wc_AesInit() before calling this function.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      key   Buffer holding key.
 * @param [in]      len   Length of key in bytes.
 * @param [in]      iv    Buffer holding IV/nonce.
 * @param [in]      ivSz  Length of IV/nonce in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when aes is NULL, or a length is non-zero but buffer
 *          is NULL, or the IV is NULL and no previous IV has been set.
 */
int wc_AesGcmEncryptInit(Aes* aes, const byte* key, word32 len, const byte* iv,
    word32 ivSz)
{
    return wc_AesGcmInit(aes, key, len, iv, ivSz);
}

/* Initialize an AES GCM cipher for encryption. Get IV.
 *
 * Must call wc_AesGcmSetIV() to generate part of IV before calling this
 * function.
 * Must call wc_AesInit() before calling this function.
 *
 * See wc_AesGcmEncrypt_ex() for non-streaming version of getting IV out.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      key   Buffer holding key.
 * @param [in]      len   Length of key in bytes.
 * @param [in]      iv    Buffer holding IV/nonce.
 * @param [in]      ivSz  Length of IV/nonce in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when aes is NULL, key length is non-zero but key
 *          is NULL, or the IV is NULL or ivOutSz is not the same as cached
 *          nonce size.
 */
int wc_AesGcmEncryptInit_ex(Aes* aes, const byte* key, word32 len, byte* ivOut,
    word32 ivOutSz)
{
    int ret;

    /* Check validity of parameters. */
    if ((aes == NULL) || (ivOut == NULL) || (ivOutSz != aes->nonceSz)) {
        ret = BAD_FUNC_ARG;
    }
    else {
        /* Copy out the IV including generated part for decryption. */
        XMEMCPY(ivOut, aes->reg, ivOutSz);
        /* Initialize AES GCM cipher with key and cached Iv. */
        ret = wc_AesGcmInit(aes, key, len, NULL, 0);
    }

    return ret;
}

/* Update the AES GCM for encryption with data and/or authentication data.
 *
 * All the AAD must be passed to update before the plaintext.
 * Last part of AAD can be passed with first part of plaintext.
 *
 * Must set key and IV before calling this function.
 * Must call wc_AesGcmInit() before calling this function.
 *
 * @param [in, out] aes       AES object.
 * @param [out]     out       Buffer to hold cipher text.
 * @param [in]      in        Buffer holding plaintext.
 * @param [in]      sz        Length of plaintext in bytes.
 * @param [in]      authIn    Buffer holding authentication data.
 * @param [in]      authInSz  Length of authentication data in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when aes is NULL, or a length is non-zero but buffer
 *          is NULL.
 */
int wc_AesGcmEncryptUpdate(Aes* aes, byte* out, const byte* in, word32 sz,
    const byte* authIn, word32 authInSz)
{
    int ret = 0;

    /* Check validity of parameters. */
    if ((aes == NULL) || ((authInSz > 0) && (authIn == NULL)) || ((sz > 0) &&
            ((out == NULL) || (in == NULL)))) {
        ret = BAD_FUNC_ARG;
    }

    /* Check key has been set. */
    if ((ret == 0) && (!aes->gcmKeySet)) {
        ret = MISSING_KEY;
    }
    /* Check IV has been set. */
    if ((ret == 0) && (!aes->nonceSet)) {
        ret = MISSING_IV;
    }

    if ((ret == 0) && aes->ctrSet && (aes->aSz == 0) && (aes->cSz == 0)) {
        aes->invokeCtr[0]++;
        if (aes->invokeCtr[0] == 0) {
            aes->invokeCtr[1]++;
            if (aes->invokeCtr[1] == 0)
                ret = AES_GCM_OVERFLOW_E;
        }
    }

    if (ret == 0) {
        /* Encrypt with AAD and/or plaintext. */
    #if defined(WOLFSSL_AESNI)
        if (haveAESNI
        #ifdef HAVE_INTEL_AVX2
            || IS_INTEL_AVX2(intel_flags)
        #endif
        #ifdef HAVE_INTEL_AVX1
            || IS_INTEL_AVX1(intel_flags)
        #endif
            ) {
            ret = AesGcmEncryptUpdate_aesni(aes, out, in, sz, authIn, authInSz);
        }
        else
    #endif
        {
            /* Encrypt the plaintext. */
            ret = AesGcmCryptUpdate_C(aes, out, in, sz);
            if (ret != 0)
                return ret;
            /* Update the authentication tag with any authentication data and the
             * new cipher text. */
            GHASH_UPDATE(aes, authIn, authInSz, out, sz);
        }
    }

    return ret;
}

/* Finalize the AES GCM for encryption and return the authentication tag.
 *
 * Must set key and IV before calling this function.
 * Must call wc_AesGcmInit() before calling this function.
 *
 * @param [in, out] aes        AES object.
 * @param [out]     authTag    Buffer to hold authentication tag.
 * @param [in]      authTagSz  Length of authentication tag in bytes.
 * @return  0 on success.
 */
int wc_AesGcmEncryptFinal(Aes* aes, byte* authTag, word32 authTagSz)
{
    int ret = 0;

    /* Check validity of parameters. */
    if ((aes == NULL) || (authTag == NULL) || (authTagSz > AES_BLOCK_SIZE) ||
            (authTagSz == 0)) {
        ret = BAD_FUNC_ARG;
    }

    /* Check key has been set. */
    if ((ret == 0) && (!aes->gcmKeySet)) {
        ret = MISSING_KEY;
    }
    /* Check IV has been set. */
    if ((ret == 0) && (!aes->nonceSet)) {
        ret = MISSING_IV;
    }

    if (ret == 0) {
        /* Calculate authentication tag. */
    #ifdef WOLFSSL_AESNI
        if (haveAESNI
        #ifdef HAVE_INTEL_AVX2
            || IS_INTEL_AVX2(intel_flags)
        #endif
        #ifdef HAVE_INTEL_AVX1
            || IS_INTEL_AVX1(intel_flags)
        #endif
            ) {
            ret = AesGcmEncryptFinal_aesni(aes, authTag, authTagSz);
        }
        else
    #endif
        {
            ret = AesGcmFinal_C(aes, authTag, authTagSz);
        }
    }

    if ((ret == 0) && aes->ctrSet) {
        IncCtr((byte*)aes->reg, aes->nonceSz);
    }

    return ret;
}

#if defined(HAVE_AES_DECRYPT) || defined(HAVE_AESGCM_DECRYPT)
/* Initialize an AES GCM cipher for decryption.
 *
 * Must call wc_AesInit() before calling this function.
 *
 * Call wc_AesGcmSetExtIV() before calling this function to use FIPS external IV
 * instead.
 *
 * @param [in, out] aes   AES object.
 * @param [in]      key   Buffer holding key.
 * @param [in]      len   Length of key in bytes.
 * @param [in]      iv    Buffer holding IV/nonce.
 * @param [in]      ivSz  Length of IV/nonce in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when aes is NULL, or a length is non-zero but buffer
 *          is NULL, or the IV is NULL and no previous IV has been set.
 */
int wc_AesGcmDecryptInit(Aes* aes, const byte* key, word32 len, const byte* iv,
    word32 ivSz)
{
    return wc_AesGcmInit(aes, key, len, iv, ivSz);
}

/* Update the AES GCM for decryption with data and/or authentication data.
 *
 * All the AAD must be passed to update before the cipher text.
 * Last part of AAD can be passed with first part of cipher text.
 *
 * Must set key and IV before calling this function.
 * Must call wc_AesGcmInit() before calling this function.
 *
 * @param [in, out] aes       AES object.
 * @param [out]     out       Buffer to hold plaintext.
 * @param [in]      in        Buffer holding cipher text.
 * @param [in]      sz        Length of cipher text in bytes.
 * @param [in]      authIn    Buffer holding authentication data.
 * @param [in]      authInSz  Length of authentication data in bytes.
 * @return  0 on success.
 * @return  BAD_FUNC_ARG when aes is NULL, or a length is non-zero but buffer
 *          is NULL.
 */
int wc_AesGcmDecryptUpdate(Aes* aes, byte* out, const byte* in, word32 sz,
    const byte* authIn, word32 authInSz)
{
    int ret = 0;

    /* Check validity of parameters. */
    if ((aes == NULL) || ((authInSz > 0) && (authIn == NULL)) || ((sz > 0) &&
            ((out == NULL) || (in == NULL)))) {
        ret = BAD_FUNC_ARG;
    }

    /* Check key has been set. */
    if ((ret == 0) && (!aes->gcmKeySet)) {
        ret = MISSING_KEY;
    }
    /* Check IV has been set. */
    if ((ret == 0) && (!aes->nonceSet)) {
        ret = MISSING_IV;
    }

    if (ret == 0) {
        /* Decrypt with AAD and/or cipher text. */
    #if defined(WOLFSSL_AESNI)
        if (haveAESNI
        #ifdef HAVE_INTEL_AVX2
            || IS_INTEL_AVX2(intel_flags)
        #endif
        #ifdef HAVE_INTEL_AVX1
            || IS_INTEL_AVX1(intel_flags)
        #endif
            ) {
            ret = AesGcmDecryptUpdate_aesni(aes, out, in, sz, authIn, authInSz);
        }
        else
    #endif
        {
            /* Update the authentication tag with any authentication data and
             * cipher text. */
            GHASH_UPDATE(aes, authIn, authInSz, in, sz);
            /* Decrypt the cipher text. */
            ret = AesGcmCryptUpdate_C(aes, out, in, sz);
        }
    }

    return ret;
}

/* Finalize the AES GCM for decryption and check the authentication tag.
 *
 * Must set key and IV before calling this function.
 * Must call wc_AesGcmInit() before calling this function.
 *
 * @param [in, out] aes        AES object.
 * @param [in]      authTag    Buffer holding authentication tag.
 * @param [in]      authTagSz  Length of authentication tag in bytes.
 * @return  0 on success.
 */
int wc_AesGcmDecryptFinal(Aes* aes, const byte* authTag, word32 authTagSz)
{
    int ret = 0;

    /* Check validity of parameters. */
    if ((aes == NULL) || (authTag == NULL) || (authTagSz > AES_BLOCK_SIZE) ||
            (authTagSz == 0)) {
        ret = BAD_FUNC_ARG;
    }

    /* Check key has been set. */
    if ((ret == 0) && (!aes->gcmKeySet)) {
        ret = MISSING_KEY;
    }
    /* Check IV has been set. */
    if ((ret == 0) && (!aes->nonceSet)) {
        ret = MISSING_IV;
    }

    if (ret == 0) {
        /* Calculate authentication tag and compare with one passed in.. */
    #ifdef WOLFSSL_AESNI
        if (haveAESNI
        #ifdef HAVE_INTEL_AVX2
            || IS_INTEL_AVX2(intel_flags)
        #endif
        #ifdef HAVE_INTEL_AVX1
            || IS_INTEL_AVX1(intel_flags)
        #endif
            ) {
            ret = AesGcmDecryptFinal_aesni(aes, authTag, authTagSz);
        }
        else
    #endif
        {
            ALIGN32 byte calcTag[AES_BLOCK_SIZE];
            /* Calculate authentication tag. */
            ret = AesGcmFinal_C(aes, calcTag, authTagSz);
            if (ret == 0) {
                /* Check calculated tag matches the one passed in. */
                if (ConstantCompare(authTag, calcTag, (int)authTagSz) != 0) {
                    ret = AES_GCM_AUTH_E;
                }
            }
        }
    }

    /* reset the state */
    if (ret == 0)
        wc_AesFree(aes);

    return ret;
}
#endif /* HAVE_AES_DECRYPT || HAVE_AESGCM_DECRYPT */
#endif /* WOLFSSL_AESGCM_STREAM */
#endif /* WOLFSSL_XILINX_CRYPT */
#endif /* end of block for AESGCM implementation selection */


/* Common to all, abstract functions that build off of lower level AESGCM
 * functions */
#ifndef WC_NO_RNG

static WARN_UNUSED_RESULT WC_INLINE int CheckAesGcmIvSize(int ivSz) {
    return (ivSz == GCM_NONCE_MIN_SZ ||
            ivSz == GCM_NONCE_MID_SZ ||
            ivSz == GCM_NONCE_MAX_SZ);
}


int wc_AesGcmSetExtIV(Aes* aes, const byte* iv, word32 ivSz)
{
    int ret = 0;

    if (aes == NULL || iv == NULL || !CheckAesGcmIvSize((int)ivSz)) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMCPY((byte*)aes->reg, iv, ivSz);

        /* If the IV is 96, allow for a 2^64 invocation counter.
         * For any other size for the nonce, limit the invocation
         * counter to 32-bits. (SP 800-38D 8.3) */
        aes->invokeCtr[0] = 0;
        aes->invokeCtr[1] = (ivSz == GCM_NONCE_MID_SZ) ? 0 : 0xFFFFFFFF;
    #ifdef WOLFSSL_AESGCM_STREAM
        aes->ctrSet = 1;
    #endif
        aes->nonceSz = ivSz;
    }

    return ret;
}


int wc_AesGcmSetIV(Aes* aes, word32 ivSz,
                   const byte* ivFixed, word32 ivFixedSz,
                   WC_RNG* rng)
{
    int ret = 0;

    if (aes == NULL || rng == NULL || !CheckAesGcmIvSize((int)ivSz) ||
        (ivFixed == NULL && ivFixedSz != 0) ||
        (ivFixed != NULL && ivFixedSz != AES_IV_FIXED_SZ)) {

        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        byte* iv = (byte*)aes->reg;

        if (ivFixedSz)
            XMEMCPY(iv, ivFixed, ivFixedSz);

        ret = wc_RNG_GenerateBlock(rng, iv + ivFixedSz, ivSz - ivFixedSz);
    }

    if (ret == 0) {
        /* If the IV is 96, allow for a 2^64 invocation counter.
         * For any other size for the nonce, limit the invocation
         * counter to 32-bits. (SP 800-38D 8.3) */
        aes->invokeCtr[0] = 0;
        aes->invokeCtr[1] = (ivSz == GCM_NONCE_MID_SZ) ? 0 : 0xFFFFFFFF;
    #ifdef WOLFSSL_AESGCM_STREAM
        aes->ctrSet = 1;
    #endif
        aes->nonceSz = ivSz;
    }

    return ret;
}


int wc_AesGcmEncrypt_ex(Aes* aes, byte* out, const byte* in, word32 sz,
                        byte* ivOut, word32 ivOutSz,
                        byte* authTag, word32 authTagSz,
                        const byte* authIn, word32 authInSz)
{
    int ret = 0;

    if (aes == NULL || (sz != 0 && (in == NULL || out == NULL)) ||
        ivOut == NULL || ivOutSz != aes->nonceSz ||
        (authIn == NULL && authInSz != 0)) {

        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        aes->invokeCtr[0]++;
        if (aes->invokeCtr[0] == 0) {
            aes->invokeCtr[1]++;
            if (aes->invokeCtr[1] == 0)
                ret = AES_GCM_OVERFLOW_E;
        }
    }

    if (ret == 0) {
        XMEMCPY(ivOut, aes->reg, ivOutSz);
        ret = wc_AesGcmEncrypt(aes, out, in, sz,
                               (byte*)aes->reg, ivOutSz,
                               authTag, authTagSz,
                               authIn, authInSz);
        if (ret == 0)
            IncCtr((byte*)aes->reg, ivOutSz);
    }

    return ret;
}

int wc_Gmac(const byte* key, word32 keySz, byte* iv, word32 ivSz,
            const byte* authIn, word32 authInSz,
            byte* authTag, word32 authTagSz, WC_RNG* rng)
{
#ifdef WOLFSSL_SMALL_STACK
    Aes *aes = NULL;
#else
    Aes aes[1];
#endif
    int ret;

    if (key == NULL || iv == NULL || (authIn == NULL && authInSz != 0) ||
        authTag == NULL || authTagSz == 0 || rng == NULL) {

        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    if ((aes = (Aes *)XMALLOC(sizeof *aes, NULL,
                              DYNAMIC_TYPE_AES)) == NULL)
        return MEMORY_E;
#endif

    ret = wc_AesInit(aes, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_AesGcmSetKey(aes, key, keySz);
        if (ret == 0)
            ret = wc_AesGcmSetIV(aes, ivSz, NULL, 0, rng);
        if (ret == 0)
            ret = wc_AesGcmEncrypt_ex(aes, NULL, NULL, 0, iv, ivSz,
                                  authTag, authTagSz, authIn, authInSz);
        wc_AesFree(aes);
    }
    ForceZero(aes, sizeof *aes);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(aes, NULL, DYNAMIC_TYPE_AES);
#endif

    return ret;
}

int wc_GmacVerify(const byte* key, word32 keySz,
                  const byte* iv, word32 ivSz,
                  const byte* authIn, word32 authInSz,
                  const byte* authTag, word32 authTagSz)
{
    int ret;
#ifdef HAVE_AES_DECRYPT
#ifdef WOLFSSL_SMALL_STACK
    Aes *aes = NULL;
#else
    Aes aes[1];
#endif

    if (key == NULL || iv == NULL || (authIn == NULL && authInSz != 0) ||
        authTag == NULL || authTagSz == 0 || authTagSz > AES_BLOCK_SIZE) {

        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    if ((aes = (Aes *)XMALLOC(sizeof *aes, NULL,
                              DYNAMIC_TYPE_AES)) == NULL)
        return MEMORY_E;
#endif

    ret = wc_AesInit(aes, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_AesGcmSetKey(aes, key, keySz);
        if (ret == 0)
            ret = wc_AesGcmDecrypt(aes, NULL, NULL, 0, iv, ivSz,
                                  authTag, authTagSz, authIn, authInSz);
        wc_AesFree(aes);
    }
    ForceZero(aes, sizeof *aes);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(aes, NULL, DYNAMIC_TYPE_AES);
#endif
#else
    (void)key;
    (void)keySz;
    (void)iv;
    (void)ivSz;
    (void)authIn;
    (void)authInSz;
    (void)authTag;
    (void)authTagSz;
    ret = NOT_COMPILED_IN;
#endif
    return ret;
}

#endif /* WC_NO_RNG */


WOLFSSL_API int wc_GmacSetKey(Gmac* gmac, const byte* key, word32 len)
{
    if (gmac == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return wc_AesGcmSetKey(&gmac->aes, key, len);
}


WOLFSSL_API int wc_GmacUpdate(Gmac* gmac, const byte* iv, word32 ivSz,
                              const byte* authIn, word32 authInSz,
                              byte* authTag, word32 authTagSz)
{
    if (gmac == NULL) {
        return BAD_FUNC_ARG;
    }

    return wc_AesGcmEncrypt(&gmac->aes, NULL, NULL, 0, iv, ivSz,
                                         authTag, authTagSz, authIn, authInSz);
}

#endif /* HAVE_AESGCM */


#ifdef HAVE_AESCCM

int wc_AesCcmSetKey(Aes* aes, const byte* key, word32 keySz)
{
    if (!((keySz == 16) || (keySz == 24) || (keySz == 32)))
        return BAD_FUNC_ARG;

    return wc_AesSetKey(aes, key, keySz, NULL, AES_ENCRYPTION);
}


/* Checks if the tag size is an accepted value based on RFC 3610 section 2
 * returns 0 if tag size is ok
 */
int wc_AesCcmCheckTagSize(int sz)
{
    /* values here are from RFC 3610 section 2 */
    if (sz != 4 && sz != 6 && sz != 8 && sz != 10 && sz != 12 && sz != 14
            && sz != 16) {
        WOLFSSL_MSG("Bad auth tag size AES-CCM");
        return BAD_FUNC_ARG;
    }
    return 0;
}

#ifdef WOLFSSL_ARMASM
    /* implementation located in wolfcrypt/src/port/arm/armv8-aes.c */

#elif defined(HAVE_COLDFIRE_SEC)
    #error "Coldfire SEC doesn't currently support AES-CCM mode"

#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) && \
        !defined(WOLFSSL_QNX_CAAM)
    /* implemented in wolfcrypt/src/port/caam_aes.c */

#elif defined(WOLFSSL_SILABS_SE_ACCEL)
    /* implemented in wolfcrypt/src/port/silabs/silabs_aes.c */
int wc_AesCcmEncrypt(Aes* aes, byte* out, const byte* in, word32 inSz,
                   const byte* nonce, word32 nonceSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    return wc_AesCcmEncrypt_silabs(
        aes, out, in, inSz,
        nonce, nonceSz,
        authTag, authTagSz,
        authIn, authInSz);
}

#ifdef HAVE_AES_DECRYPT
int  wc_AesCcmDecrypt(Aes* aes, byte* out, const byte* in, word32 inSz,
                   const byte* nonce, word32 nonceSz,
                   const byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    return wc_AesCcmDecrypt_silabs(
        aes, out, in, inSz,
        nonce, nonceSz,
        authTag, authTagSz,
        authIn, authInSz);
}
#endif
#elif defined(FREESCALE_LTC)

/* return 0 on success */
int wc_AesCcmEncrypt(Aes* aes, byte* out, const byte* in, word32 inSz,
                   const byte* nonce, word32 nonceSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    byte *key;
    word32 keySize;
    status_t status;

    /* sanity check on arguments */
    /* note, LTC_AES_EncryptTagCcm() doesn't allow null src or dst
     * ptrs even if inSz is zero (ltc_aes_ccm_check_input_args()), so
     * don't allow it here either.
     */
    if (aes == NULL || out == NULL || in == NULL || nonce == NULL
            || authTag == NULL || nonceSz < 7 || nonceSz > 13) {
        return BAD_FUNC_ARG;
    }

    if (wc_AesCcmCheckTagSize(authTagSz) != 0) {
        return BAD_FUNC_ARG;
    }

    key = (byte*)aes->key;

    status = wc_AesGetKeySize(aes, &keySize);
    if (status != 0) {
        return status;
    }

    status = wolfSSL_CryptHwMutexLock();
    if (status != 0)
        return status;

    status = LTC_AES_EncryptTagCcm(LTC_BASE, in, out, inSz,
        nonce, nonceSz, authIn, authInSz, key, keySize, authTag, authTagSz);
    wolfSSL_CryptHwMutexUnLock();

    return (kStatus_Success == status) ? 0 : BAD_FUNC_ARG;
}

#ifdef HAVE_AES_DECRYPT
int  wc_AesCcmDecrypt(Aes* aes, byte* out, const byte* in, word32 inSz,
                   const byte* nonce, word32 nonceSz,
                   const byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    byte *key;
    word32 keySize;
    status_t status;

    /* sanity check on arguments */
    if (aes == NULL || out == NULL || in == NULL || nonce == NULL
            || authTag == NULL || nonceSz < 7 || nonceSz > 13) {
        return BAD_FUNC_ARG;
    }

    key = (byte*)aes->key;

    status = wc_AesGetKeySize(aes, &keySize);
    if (status != 0) {
        return status;
    }

    status = wolfSSL_CryptHwMutexLock();
    if (status != 0)
        return status;
    status = LTC_AES_DecryptTagCcm(LTC_BASE, in, out, inSz,
        nonce, nonceSz, authIn, authInSz, key, keySize, authTag, authTagSz);
    wolfSSL_CryptHwMutexUnLock();

    if (status != kStatus_Success) {
        XMEMSET(out, 0, inSz);
        return AES_CCM_AUTH_E;
    }
    return 0;
}
#endif /* HAVE_AES_DECRYPT */

#else

/* Software CCM */
static WARN_UNUSED_RESULT int roll_x(
    Aes* aes, const byte* in, word32 inSz, byte* out)
{
    int ret;

    /* process the bulk of the data */
    while (inSz >= AES_BLOCK_SIZE) {
        xorbuf(out, in, AES_BLOCK_SIZE);
        in += AES_BLOCK_SIZE;
        inSz -= AES_BLOCK_SIZE;

        ret = wc_AesEncrypt(aes, out, out);
        if (ret != 0)
            return ret;
    }

    /* process remainder of the data */
    if (inSz > 0) {
        xorbuf(out, in, inSz);
        ret = wc_AesEncrypt(aes, out, out);
        if (ret != 0)
            return ret;
    }

    return 0;
}

static WARN_UNUSED_RESULT int roll_auth(
    Aes* aes, const byte* in, word32 inSz, byte* out)
{
    word32 authLenSz;
    word32 remainder;
    int ret;

    /* encode the length in */
    if (inSz <= 0xFEFF) {
        authLenSz = 2;
        out[0] ^= (byte)(inSz >> 8);
        out[1] ^= (byte)inSz;
    }
    else {
        authLenSz = 6;
        out[0] ^= 0xFF;
        out[1] ^= 0xFE;
        out[2] ^= (byte)(inSz >> 24);
        out[3] ^= (byte)(inSz >> 16);
        out[4] ^= (byte)(inSz >>  8);
        out[5] ^= (byte)inSz;
    }
    /* Note, the protocol handles auth data up to 2^64, but we are
     * using 32-bit sizes right now, so the bigger data isn't handled
     * else {}
     */

    /* start fill out the rest of the first block */
    remainder = AES_BLOCK_SIZE - authLenSz;
    if (inSz >= remainder) {
        /* plenty of bulk data to fill the remainder of this block */
        xorbuf(out + authLenSz, in, remainder);
        inSz -= remainder;
        in += remainder;
    }
    else {
        /* not enough bulk data, copy what is available, and pad zero */
        xorbuf(out + authLenSz, in, inSz);
        inSz = 0;
    }
    ret = wc_AesEncrypt(aes, out, out);

    if ((ret == 0) && (inSz > 0)) {
        ret = roll_x(aes, in, inSz, out);
    }

    return ret;
}


static WC_INLINE void AesCcmCtrInc(byte* B, word32 lenSz)
{
    word32 i;

    for (i = 0; i < lenSz; i++) {
        if (++B[AES_BLOCK_SIZE - 1 - i] != 0) return;
    }
}

#ifdef WOLFSSL_AESNI
static WC_INLINE void AesCcmCtrIncSet4(byte* B, word32 lenSz)
{
    word32 i;

    /* B+1 = B */
    XMEMCPY(B + AES_BLOCK_SIZE * 1, B, AES_BLOCK_SIZE);
    /* B+2,B+3 = B,B+1 */
    XMEMCPY(B + AES_BLOCK_SIZE * 2, B, AES_BLOCK_SIZE * 2);

    for (i = 0; i < lenSz; i++) {
        if (++B[AES_BLOCK_SIZE * 2 - 1 - i] != 0) break;
    }
    B[AES_BLOCK_SIZE * 3 - 1] += 2;
    if (B[AES_BLOCK_SIZE * 3 - 1] < 2) {
        for (i = 1; i < lenSz; i++) {
            if (++B[AES_BLOCK_SIZE * 3 - 1 - i] != 0) break;
        }
    }
    B[AES_BLOCK_SIZE * 4 - 1] += 3;
    if (B[AES_BLOCK_SIZE * 4 - 1] < 3) {
        for (i = 1; i < lenSz; i++) {
            if (++B[AES_BLOCK_SIZE * 4 - 1 - i] != 0) break;
        }
    }
}

static WC_INLINE void AesCcmCtrInc4(byte* B, word32 lenSz)
{
    word32 i;

    B[AES_BLOCK_SIZE - 1] += 4;
    if (B[AES_BLOCK_SIZE - 1] < 4) {
        for (i = 1; i < lenSz; i++) {
            if (++B[AES_BLOCK_SIZE - 1 - i] != 0) break;
        }
    }
}
#endif

/* Software AES - CCM Encrypt */
/* return 0 on success */
int wc_AesCcmEncrypt(Aes* aes, byte* out, const byte* in, word32 inSz,
                   const byte* nonce, word32 nonceSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
#ifndef WOLFSSL_AESNI
    byte A[AES_BLOCK_SIZE];
    byte B[AES_BLOCK_SIZE];
#else
    ALIGN128 byte A[AES_BLOCK_SIZE * 4];
    ALIGN128 byte B[AES_BLOCK_SIZE * 4];
#endif
    byte lenSz;
    word32 i;
    byte mask = 0xFF;
    const word32 wordSz = (word32)sizeof(word32);
    int ret;

    /* sanity check on arguments */
    if (aes == NULL || (inSz != 0 && (in == NULL || out == NULL)) ||
        nonce == NULL || authTag == NULL || nonceSz < 7 || nonceSz > 13 ||
            authTagSz > AES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    /* sanity check on tag size */
    if (wc_AesCcmCheckTagSize((int)authTagSz) != 0) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (aes->devId != INVALID_DEVID)
    #endif
    {
        int crypto_cb_ret =
            wc_CryptoCb_AesCcmEncrypt(aes, out, in, inSz, nonce, nonceSz,
                                      authTag, authTagSz, authIn, authInSz);
        if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
            return crypto_cb_ret;
        /* fall-through when unavailable */
    }
#endif

    XMEMSET(A, 0, sizeof(A));
    XMEMCPY(B+1, nonce, nonceSz);
    lenSz = AES_BLOCK_SIZE - 1 - (byte)nonceSz;
    B[0] = (byte)((authInSz > 0 ? 64 : 0)
                  + (8 * (((byte)authTagSz - 2) / 2))
                  + (lenSz - 1));
    for (i = 0; i < lenSz; i++) {
        if (mask && i >= wordSz)
            mask = 0x00;
        B[AES_BLOCK_SIZE - 1 - i] = (byte)((inSz >> ((8 * i) & mask)) & mask);
    }

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_AesCcmEncrypt B", B, sizeof(B));
#endif

    ret = wc_AesEncrypt(aes, B, A);
    if (ret != 0) {
        ForceZero(B, sizeof(B));
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(B, sizeof(B));
    #endif
        return ret;
    }
#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_AesCcmEncrypt A", A, sizeof(A));
#endif

    if (authInSz > 0) {
        ret = roll_auth(aes, authIn, authInSz, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
    }
    if (inSz > 0) {
        ret = roll_x(aes, in, inSz, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
    }
    XMEMCPY(authTag, A, authTagSz);

    B[0] = lenSz - 1;
    for (i = 0; i < lenSz; i++)
        B[AES_BLOCK_SIZE - 1 - i] = 0;
    ret = wc_AesEncrypt(aes, B, A);
    if (ret != 0) {
        ForceZero(A, sizeof(A));
        ForceZero(B, sizeof(B));
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(A, sizeof(A));
        wc_MemZero_Check(B, sizeof(B));
    #endif
        return ret;
    }
    xorbuf(authTag, A, authTagSz);

    B[15] = 1;
#ifdef WOLFSSL_AESNI
    if (haveAESNI && aes->use_aesni) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        while (inSz >= AES_BLOCK_SIZE * 4) {
            AesCcmCtrIncSet4(B, lenSz);

            AES_ECB_encrypt(B, A, AES_BLOCK_SIZE * 4, (byte*)aes->key,
                            (int)aes->rounds);

            xorbuf(A, in, AES_BLOCK_SIZE * 4);
            XMEMCPY(out, A, AES_BLOCK_SIZE * 4);

            inSz -= AES_BLOCK_SIZE * 4;
            in += AES_BLOCK_SIZE * 4;
            out += AES_BLOCK_SIZE * 4;

            AesCcmCtrInc4(B, lenSz);
        }
        RESTORE_VECTOR_REGISTERS();
    }
#endif
    while (inSz >= AES_BLOCK_SIZE) {
        ret = wc_AesEncrypt(aes, B, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
        xorbuf(A, in, AES_BLOCK_SIZE);
        XMEMCPY(out, A, AES_BLOCK_SIZE);

        AesCcmCtrInc(B, lenSz);
        inSz -= AES_BLOCK_SIZE;
        in += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
    }
    if (inSz > 0) {
        ret = wc_AesEncrypt(aes, B, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
        xorbuf(A, in, inSz);
        XMEMCPY(out, A, inSz);
    }

    ForceZero(A, sizeof(A));
    ForceZero(B, sizeof(B));

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(A, sizeof(A));
    wc_MemZero_Check(B, sizeof(B));
#endif

    return 0;
}

#ifdef HAVE_AES_DECRYPT
/* Software AES - CCM Decrypt */
int  wc_AesCcmDecrypt(Aes* aes, byte* out, const byte* in, word32 inSz,
                   const byte* nonce, word32 nonceSz,
                   const byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
#ifndef WOLFSSL_AESNI
    byte A[AES_BLOCK_SIZE];
    byte B[AES_BLOCK_SIZE];
#else
    ALIGN128 byte B[AES_BLOCK_SIZE * 4];
    ALIGN128 byte A[AES_BLOCK_SIZE * 4];
#endif
    byte* o;
    byte lenSz;
    word32 i, oSz;
    byte mask = 0xFF;
    const word32 wordSz = (word32)sizeof(word32);
    int ret;

    /* sanity check on arguments */
    if (aes == NULL || (inSz != 0 && (in == NULL || out == NULL)) ||
        nonce == NULL || authTag == NULL || nonceSz < 7 || nonceSz > 13 ||
        authTagSz > AES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    /* sanity check on tag size */
    if (wc_AesCcmCheckTagSize((int)authTagSz) != 0) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (aes->devId != INVALID_DEVID)
    #endif
    {
        int crypto_cb_ret =
            wc_CryptoCb_AesCcmDecrypt(aes, out, in, inSz, nonce, nonceSz,
            authTag, authTagSz, authIn, authInSz);
        if (crypto_cb_ret != CRYPTOCB_UNAVAILABLE)
            return crypto_cb_ret;
        /* fall-through when unavailable */
    }
#endif

    o = out;
    oSz = inSz;
    XMEMSET(A, 0, sizeof A);
    XMEMCPY(B+1, nonce, nonceSz);
    lenSz = AES_BLOCK_SIZE - 1 - (byte)nonceSz;

    B[0] = lenSz - 1;
    for (i = 0; i < lenSz; i++)
        B[AES_BLOCK_SIZE - 1 - i] = 0;
    B[15] = 1;

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Add("wc_AesCcmEncrypt A", A, sizeof(A));
    wc_MemZero_Add("wc_AesCcmEncrypt B", B, sizeof(B));
#endif

#ifdef WOLFSSL_AESNI
    if (haveAESNI && aes->use_aesni) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        while (oSz >= AES_BLOCK_SIZE * 4) {
            AesCcmCtrIncSet4(B, lenSz);

            AES_ECB_encrypt(B, A, AES_BLOCK_SIZE * 4, (byte*)aes->key,
                            (int)aes->rounds);

            xorbuf(A, in, AES_BLOCK_SIZE * 4);
            XMEMCPY(o, A, AES_BLOCK_SIZE * 4);

            oSz -= AES_BLOCK_SIZE * 4;
            in += AES_BLOCK_SIZE * 4;
            o += AES_BLOCK_SIZE * 4;

            AesCcmCtrInc4(B, lenSz);
        }
        RESTORE_VECTOR_REGISTERS();
    }
#endif
    while (oSz >= AES_BLOCK_SIZE) {
        ret = wc_AesEncrypt(aes, B, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
        xorbuf(A, in, AES_BLOCK_SIZE);
        XMEMCPY(o, A, AES_BLOCK_SIZE);

        AesCcmCtrInc(B, lenSz);
        oSz -= AES_BLOCK_SIZE;
        in += AES_BLOCK_SIZE;
        o += AES_BLOCK_SIZE;
    }
    if (inSz > 0) {
        ret = wc_AesEncrypt(aes, B, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
        xorbuf(A, in, oSz);
        XMEMCPY(o, A, oSz);
    }

    for (i = 0; i < lenSz; i++)
        B[AES_BLOCK_SIZE - 1 - i] = 0;
    ret = wc_AesEncrypt(aes, B, A);
    if (ret != 0) {
        ForceZero(A, sizeof(A));
        ForceZero(B, sizeof(B));
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(A, sizeof(A));
        wc_MemZero_Check(B, sizeof(B));
    #endif
        return ret;
    }

    o = out;
    oSz = inSz;

    B[0] = (byte)((authInSz > 0 ? 64 : 0)
                  + (8 * (((byte)authTagSz - 2) / 2))
                  + (lenSz - 1));
    for (i = 0; i < lenSz; i++) {
        if (mask && i >= wordSz)
            mask = 0x00;
        B[AES_BLOCK_SIZE - 1 - i] = (byte)((inSz >> ((8 * i) & mask)) & mask);
    }

    ret = wc_AesEncrypt(aes, B, A);
    if (ret != 0) {
        ForceZero(A, sizeof(A));
        ForceZero(B, sizeof(B));
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(A, sizeof(A));
        wc_MemZero_Check(B, sizeof(B));
    #endif
        return ret;
    }

    if (authInSz > 0) {
        ret = roll_auth(aes, authIn, authInSz, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
    }
    if (inSz > 0) {
        ret = roll_x(aes, o, oSz, A);
        if (ret != 0) {
            ForceZero(A, sizeof(A));
            ForceZero(B, sizeof(B));
        #ifdef WOLFSSL_CHECK_MEM_ZERO
            wc_MemZero_Check(A, sizeof(A));
            wc_MemZero_Check(B, sizeof(B));
        #endif
            return ret;
        }
    }

    B[0] = lenSz - 1;
    for (i = 0; i < lenSz; i++)
        B[AES_BLOCK_SIZE - 1 - i] = 0;
    ret = wc_AesEncrypt(aes, B, B);
    if (ret != 0) {
        ForceZero(A, sizeof(A));
        ForceZero(B, sizeof(B));
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        wc_MemZero_Check(A, sizeof(A));
        wc_MemZero_Check(B, sizeof(B));
    #endif
        return ret;
    }
    xorbuf(A, B, authTagSz);

    if (ConstantCompare(A, authTag, (int)authTagSz) != 0) {
        /* If the authTag check fails, don't keep the decrypted data.
         * Unfortunately, you need the decrypted data to calculate the
         * check value. */
        #if defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2) && \
            defined(ACVP_VECTOR_TESTING)
            WOLFSSL_MSG("Preserve output for vector responses");
        #else
            if (inSz > 0)
                XMEMSET(out, 0, inSz);
        #endif
        ret = AES_CCM_AUTH_E;
    }

    ForceZero(A, sizeof(A));
    ForceZero(B, sizeof(B));
    o = NULL;

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(A, sizeof(A));
    wc_MemZero_Check(B, sizeof(B));
#endif

    return ret;
}

#endif /* HAVE_AES_DECRYPT */
#endif /* software CCM */

/* abstract functions that call lower level AESCCM functions */
#ifndef WC_NO_RNG

int wc_AesCcmSetNonce(Aes* aes, const byte* nonce, word32 nonceSz)
{
    int ret = 0;

    if (aes == NULL || nonce == NULL ||
        nonceSz < CCM_NONCE_MIN_SZ || nonceSz > CCM_NONCE_MAX_SZ) {

        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMCPY(aes->reg, nonce, nonceSz);
        aes->nonceSz = nonceSz;

        /* Invocation counter should be 2^61 */
        aes->invokeCtr[0] = 0;
        aes->invokeCtr[1] = 0xE0000000;
    }

    return ret;
}


int wc_AesCcmEncrypt_ex(Aes* aes, byte* out, const byte* in, word32 sz,
                        byte* ivOut, word32 ivOutSz,
                        byte* authTag, word32 authTagSz,
                        const byte* authIn, word32 authInSz)
{
    int ret = 0;

    if (aes == NULL || out == NULL ||
        (in == NULL && sz != 0) ||
        ivOut == NULL ||
        (authIn == NULL && authInSz != 0) ||
        (ivOutSz != aes->nonceSz)) {

        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        aes->invokeCtr[0]++;
        if (aes->invokeCtr[0] == 0) {
            aes->invokeCtr[1]++;
            if (aes->invokeCtr[1] == 0)
                ret = AES_CCM_OVERFLOW_E;
        }
    }

    if (ret == 0) {
        ret = wc_AesCcmEncrypt(aes, out, in, sz,
                               (byte*)aes->reg, aes->nonceSz,
                               authTag, authTagSz,
                               authIn, authInSz);
        if (ret == 0) {
            XMEMCPY(ivOut, aes->reg, aes->nonceSz);
            IncCtr((byte*)aes->reg, aes->nonceSz);
        }
    }

    return ret;
}

#endif /* WC_NO_RNG */

#endif /* HAVE_AESCCM */


/* Initialize Aes for use with async hardware */
int wc_AesInit(Aes* aes, void* heap, int devId)
{
    int ret = 0;

    if (aes == NULL)
        return BAD_FUNC_ARG;

    aes->heap = heap;

#ifdef WOLF_CRYPTO_CB
    aes->devId = devId;
    aes->devCtx = NULL;
#else
    (void)devId;
#endif
#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)
    ret = wolfAsync_DevCtxInit(&aes->asyncDev, WOLFSSL_ASYNC_MARKER_AES,
                                                        aes->heap, devId);
#endif /* WOLFSSL_ASYNC_CRYPT */

#if defined(WOLFSSL_AFALG) || defined(WOLFSSL_AFALG_XILINX_AES)
    aes->alFd = WC_SOCK_NOTSET;
    aes->rdFd = WC_SOCK_NOTSET;
#endif
#ifdef WOLFSSL_KCAPI_AES
    aes->handle = NULL;
    aes->init   = 0;
#endif
#if defined(WOLFSSL_DEVCRYPTO) && \
   (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))
    aes->ctx.cfd = -1;
#endif
#if defined(WOLFSSL_CRYPTOCELL) && defined(WOLFSSL_CRYPTOCELL_AES)
    XMEMSET(&aes->ctx, 0, sizeof(aes->ctx));
#endif
#if defined(WOLFSSL_IMXRT_DCP)
    DCPAesInit(aes);
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    XMEMSET(&aes->maxq_ctx, 0, sizeof(aes->maxq_ctx));
#endif

#ifdef HAVE_AESGCM
#ifdef OPENSSL_EXTRA
    XMEMSET(aes->gcm.aadH, 0, sizeof(aes->gcm.aadH));
    aes->gcm.aadLen = 0;
#endif
#endif

#ifdef WOLFSSL_AESGCM_STREAM
#if defined(WOLFSSL_SMALL_STACK) && !defined(WOLFSSL_AESNI)
    aes->streamData = NULL;
#endif
    aes->keylen = 0;
    aes->nonceSz = 0;
    aes->gcmKeySet = 0;
    aes->nonceSet = 0;
    aes->ctrSet = 0;
#endif

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    ret = wc_psa_aes_init(aes);
#endif

    return ret;
}

#ifdef WOLF_PRIVATE_KEY_ID
int  wc_AesInit_Id(Aes* aes, unsigned char* id, int len, void* heap, int devId)
{
    int ret = 0;

    if (aes == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0 && (len < 0 || len > AES_MAX_ID_LEN))
        ret = BUFFER_E;

    if (ret == 0)
        ret = wc_AesInit(aes, heap, devId);
    if (ret == 0) {
        XMEMCPY(aes->id, id, (size_t)len);
        aes->idLen = len;
        aes->labelLen = 0;
    }

    return ret;
}

int wc_AesInit_Label(Aes* aes, const char* label, void* heap, int devId)
{
    int ret = 0;
    size_t labelLen = 0;

    if (aes == NULL || label == NULL)
        ret = BAD_FUNC_ARG;
    if (ret == 0) {
        labelLen = XSTRLEN(label);
        if (labelLen == 0 || labelLen > AES_MAX_LABEL_LEN)
            ret = BUFFER_E;
    }

    if (ret == 0)
        ret = wc_AesInit(aes, heap, devId);
    if (ret == 0) {
        XMEMCPY(aes->label, label, labelLen);
        aes->labelLen = (int)labelLen;
        aes->idLen = 0;
    }

    return ret;
}
#endif

/* Free Aes from use with async hardware */
void wc_AesFree(Aes* aes)
{
    if (aes == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES)
    wolfAsync_DevCtxFree(&aes->asyncDev, WOLFSSL_ASYNC_MARKER_AES);
#endif /* WOLFSSL_ASYNC_CRYPT */
#if defined(WOLFSSL_AFALG) || defined(WOLFSSL_AFALG_XILINX_AES)
    if (aes->rdFd > 0) { /* negative is error case */
        close(aes->rdFd);
        aes->rdFd = WC_SOCK_NOTSET;
    }
    if (aes->alFd > 0) {
        close(aes->alFd);
        aes->alFd = WC_SOCK_NOTSET;
    }
#endif /* WOLFSSL_AFALG */
#ifdef WOLFSSL_KCAPI_AES
    ForceZero((byte*)aes->devKey, AES_MAX_KEY_SIZE/WOLFSSL_BIT_SIZE);
    if (aes->init == 1) {
        kcapi_cipher_destroy(aes->handle);
    }
    aes->init = 0;
    aes->handle = NULL;
#endif
#if defined(WOLFSSL_DEVCRYPTO) && \
    (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))
    wc_DevCryptoFree(&aes->ctx);
#endif
#if defined(WOLF_CRYPTO_CB) || (defined(WOLFSSL_DEVCRYPTO) && \
    (defined(WOLFSSL_DEVCRYPTO_AES) || defined(WOLFSSL_DEVCRYPTO_CBC))) || \
    (defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_AES))
    ForceZero((byte*)aes->devKey, AES_MAX_KEY_SIZE/WOLFSSL_BIT_SIZE);
#endif
#if defined(WOLFSSL_IMXRT_DCP)
    DCPAesFree(aes);
#endif
#if defined(WOLFSSL_AESGCM_STREAM) && defined(WOLFSSL_SMALL_STACK) && \
    !defined(WOLFSSL_AESNI)
    if (aes->streamData != NULL) {
        XFREE(aes->streamData, aes->heap, DYNAMIC_TYPE_AES);
        aes->streamData = NULL;
    }
#endif

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_CRYPT)
    if (aes->useSWCrypt == 0) {
        se050_aes_free(aes);
    }
#endif

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    wc_psa_aes_free(aes);
#endif

#ifdef WOLFSSL_MAXQ10XX_CRYPTO
    wc_MAXQ10XX_AesFree(aes);
#endif

#ifdef WOLFSSL_CHECK_MEM_ZERO
    wc_MemZero_Check(aes, sizeof(Aes));
#endif
}


int wc_AesGetKeySize(Aes* aes, word32* keySize)
{
    int ret = 0;

    if (aes == NULL || keySize == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_HAVE_PSA) && !defined(WOLFSSL_PSA_NO_AES)
    return wc_psa_aes_get_key_size(aes, keySize);
#endif
#if defined(WOLFSSL_CRYPTOCELL) && defined(WOLFSSL_CRYPTOCELL_AES)
    *keySize = aes->ctx.key.keySize;
    return ret;
#endif
    switch (aes->rounds) {
#ifdef WOLFSSL_AES_128
    case 10:
        *keySize = 16;
        break;
#endif
#ifdef WOLFSSL_AES_192
    case 12:
        *keySize = 24;
        break;
#endif
#ifdef WOLFSSL_AES_256
    case 14:
        *keySize = 32;
        break;
#endif
    default:
        *keySize = 0;
        ret = BAD_FUNC_ARG;
    }

    return ret;
}

#endif /* !WOLFSSL_TI_CRYPT */

#ifdef HAVE_AES_ECB
#if defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_AES) && \
        !defined(WOLFSSL_QNX_CAAM)
    /* implemented in wolfcrypt/src/port/caam/caam_aes.c */

#elif defined(WOLFSSL_AFALG)
    /* implemented in wolfcrypt/src/port/af_alg/afalg_aes.c */

#elif defined(WOLFSSL_DEVCRYPTO_AES)
    /* implemented in wolfcrypt/src/port/devcrypt/devcrypto_aes.c */

#elif defined(WOLFSSL_SCE) && !defined(WOLFSSL_SCE_NO_AES)

/* Software AES - ECB */
int wc_AesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    if ((in == NULL) || (out == NULL) || (aes == NULL))
        return BAD_FUNC_ARG;

    return AES_ECB_encrypt(aes, in, out, sz);
}


int wc_AesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    if ((in == NULL) || (out == NULL) || (aes == NULL))
        return BAD_FUNC_ARG;

    return AES_ECB_decrypt(aes, in, out, sz);
}

#else

/* Software AES - ECB */
static WARN_UNUSED_RESULT int _AesEcbEncrypt(
    Aes* aes, byte* out, const byte* in, word32 sz)
{
    word32 blocks = sz / AES_BLOCK_SIZE;

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (aes->devId != INVALID_DEVID)
    #endif
    {
        int ret = wc_CryptoCb_AesEcbEncrypt(aes, out, in, sz);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif
#ifdef WOLFSSL_IMXRT_DCP
    if (aes->keylen == 16)
        return DCPAesEcbEncrypt(aes, out, in, sz);
#endif
#ifdef WOLFSSL_AESNI
    if (haveAESNI && aes->use_aesni) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_ECB_encrypt(in, out, sz, (byte*)aes->key, (int)aes->rounds);
        RESTORE_VECTOR_REGISTERS();
        blocks = 0;
    }
#endif
    while (blocks > 0) {
      int ret = wc_AesEncryptDirect(aes, out, in);
      if (ret != 0)
        return ret;
      out += AES_BLOCK_SIZE;
      in  += AES_BLOCK_SIZE;
      blocks--;
    }
    return 0;
}

static WARN_UNUSED_RESULT int _AesEcbDecrypt(
    Aes* aes, byte* out, const byte* in, word32 sz)
{
    word32 blocks = sz / AES_BLOCK_SIZE;

#ifdef WOLF_CRYPTO_CB
    #ifndef WOLF_CRYPTO_CB_FIND
    if (aes->devId != INVALID_DEVID)
    #endif
    {
        int ret = wc_CryptoCb_AesEcbDecrypt(aes, out, in, sz);
        if (ret != CRYPTOCB_UNAVAILABLE)
            return ret;
        /* fall-through when unavailable */
    }
#endif
#ifdef WOLFSSL_IMXRT_DCP
    if (aes->keylen == 16)
        return DCPAesEcbDecrypt(aes, out, in, sz);
#endif
#ifdef WOLFSSL_AESNI
    if (haveAESNI && aes->use_aesni) {
        SAVE_VECTOR_REGISTERS(return _svr_ret;);
        AES_ECB_decrypt(in, out, sz, (byte*)aes->key, (int)aes->rounds);
        RESTORE_VECTOR_REGISTERS();
        blocks = 0;
    }
#endif
    while (blocks > 0) {
      int ret = wc_AesDecryptDirect(aes, out, in);
      if (ret != 0)
        return ret;
      out += AES_BLOCK_SIZE;
      in  += AES_BLOCK_SIZE;
      blocks--;
    }
    return 0;
}

int wc_AesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;

    if ((in == NULL) || (out == NULL) || (aes == NULL))
      return BAD_FUNC_ARG;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = _AesEcbEncrypt(aes, out, in, sz);
    RESTORE_VECTOR_REGISTERS();

    return ret;
}

int wc_AesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    int ret;

    if ((in == NULL) || (out == NULL) || (aes == NULL))
      return BAD_FUNC_ARG;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);
    ret = _AesEcbDecrypt(aes, out, in, sz);
    RESTORE_VECTOR_REGISTERS();

    return ret;
}
#endif
#endif /* HAVE_AES_ECB */

#if defined(WOLFSSL_AES_CFB) || defined(WOLFSSL_AES_OFB)
/* Feedback AES mode
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt
 * sz  size of input buffer
 * mode flag to specify AES mode
 *
 * returns 0 on success and negative error values on failure
 */
/* Software AES - CFB Encrypt */
static WARN_UNUSED_RESULT int wc_AesFeedbackEncrypt(
    Aes* aes, byte* out, const byte* in, word32 sz, byte mode)
{
    byte*  tmp = NULL;
    int ret = 0;
    word32 processed;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    /* consume any unused bytes left in aes->tmp */
    processed = min(aes->left, sz);
    xorbufout(out, in, (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left, processed);
#ifdef WOLFSSL_AES_CFB
    if (mode == AES_CFB_MODE) {
        XMEMCPY((byte*)aes->reg + AES_BLOCK_SIZE - aes->left, out, processed);
    }
#endif
    aes->left -= processed;
    out += processed;
    in += processed;
    sz -= processed;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    while (sz >= AES_BLOCK_SIZE) {
        /* Using aes->tmp here for inline case i.e. in=out */
        ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg);
        if (ret != 0)
            break;
    #ifdef WOLFSSL_AES_OFB
        if (mode == AES_OFB_MODE) {
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);
        }
    #endif
        xorbuf((byte*)aes->tmp, in, AES_BLOCK_SIZE);
    #ifdef WOLFSSL_AES_CFB
        if (mode == AES_CFB_MODE) {
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);
        }
    #endif
        XMEMCPY(out, aes->tmp, AES_BLOCK_SIZE);
        out += AES_BLOCK_SIZE;
        in  += AES_BLOCK_SIZE;
        sz  -= AES_BLOCK_SIZE;
        aes->left = 0;
    }

    /* encrypt left over data */
    if ((ret == 0) && sz) {
        ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg);
    }
    if ((ret == 0) && sz) {
        aes->left = AES_BLOCK_SIZE;
        tmp = (byte*)aes->tmp;
    #ifdef WOLFSSL_AES_OFB
        if (mode == AES_OFB_MODE) {
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);
        }
    #endif

        xorbufout(out, in, tmp, sz);
    #ifdef WOLFSSL_AES_CFB
        if (mode == AES_CFB_MODE) {
            XMEMCPY(aes->reg, out, sz);
        }
    #endif
        aes->left -= sz;
    }
    RESTORE_VECTOR_REGISTERS();

    return ret;
}


#ifdef HAVE_AES_DECRYPT
/* CFB 128
 *
 * aes structure holding key to use for decryption
 * out buffer to hold result of decryption (must be at least as large as input
 *     buffer)
 * in  buffer to decrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative error values on failure
 */
/* Software AES - CFB Decrypt */
static WARN_UNUSED_RESULT int wc_AesFeedbackDecrypt(
    Aes* aes, byte* out, const byte* in, word32 sz, byte mode)
{
    int ret = 0;
    word32 processed;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    #ifdef WOLFSSL_AES_CFB
    /* check if more input needs copied over to aes->reg */
    if (aes->left && sz && mode == AES_CFB_MODE) {
        word32 size = min(aes->left, sz);
        XMEMCPY((byte*)aes->reg + AES_BLOCK_SIZE - aes->left, in, size);
    }
    #endif

    /* consume any unused bytes left in aes->tmp */
    processed = min(aes->left, sz);
    xorbufout(out, in, (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left, processed);
    aes->left -= processed;
    out += processed;
    in += processed;
    sz -= processed;

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    while (sz > AES_BLOCK_SIZE) {
        /* Using aes->tmp here for inline case i.e. in=out */
        ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg);
        if (ret != 0)
            break;
    #ifdef WOLFSSL_AES_OFB
        if (mode == AES_OFB_MODE) {
            XMEMCPY((byte*)aes->reg, (byte*)aes->tmp, AES_BLOCK_SIZE);
        }
    #endif
        xorbuf((byte*)aes->tmp, in, AES_BLOCK_SIZE);
    #ifdef WOLFSSL_AES_CFB
        if (mode == AES_CFB_MODE) {
            XMEMCPY(aes->reg, in, AES_BLOCK_SIZE);
        }
    #endif
        XMEMCPY(out, (byte*)aes->tmp, AES_BLOCK_SIZE);
        out += AES_BLOCK_SIZE;
        in  += AES_BLOCK_SIZE;
        sz  -= AES_BLOCK_SIZE;
        aes->left = 0;
    }

    /* decrypt left over data */
    if ((ret == 0) && sz) {
        ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg);
    }
    if ((ret == 0) && sz) {
    #ifdef WOLFSSL_AES_CFB
        if (mode == AES_CFB_MODE) {
            XMEMCPY(aes->reg, in, sz);
        }
    #endif
    #ifdef WOLFSSL_AES_OFB
        if (mode == AES_OFB_MODE) {
            XMEMCPY(aes->reg, aes->tmp, AES_BLOCK_SIZE);
        }
    #endif

        aes->left = AES_BLOCK_SIZE - sz;
        xorbufout(out, in, aes->tmp, sz);
    }
    RESTORE_VECTOR_REGISTERS();

    return ret;
}
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_CFB */

#ifdef WOLFSSL_AES_CFB
/* CFB 128
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative error values on failure
 */
/* Software AES - CFB Encrypt */
int wc_AesCfbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackEncrypt(aes, out, in, sz, AES_CFB_MODE);
}


#ifdef HAVE_AES_DECRYPT
/* CFB 128
 *
 * aes structure holding key to use for decryption
 * out buffer to hold result of decryption (must be at least as large as input
 *     buffer)
 * in  buffer to decrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative error values on failure
 */
/* Software AES - CFB Decrypt */
int wc_AesCfbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackDecrypt(aes, out, in, sz, AES_CFB_MODE);
}
#endif /* HAVE_AES_DECRYPT */


/* shift the whole AES_BLOCK_SIZE array left by 8 or 1 bits */
static void shiftLeftArray(byte* ary, byte shift)
{
    int i;

    if (shift == WOLFSSL_BIT_SIZE) {
        /* shifting over by 8 bits */
        for (i = 0; i < AES_BLOCK_SIZE - 1; i++) {
            ary[i] = ary[i+1];
        }
        ary[i] = 0;
    }
    else {
        /* shifting over by 7 or less bits */
        for (i = 0; i < AES_BLOCK_SIZE - 1; i++) {
            byte carry = ary[i+1] & (0XFF << (WOLFSSL_BIT_SIZE - shift));
            carry >>= (WOLFSSL_BIT_SIZE - shift);
            ary[i] = (byte)((ary[i] << shift) + carry);
        }
        ary[i] = ary[i] << shift;
    }
}


/* returns 0 on success and negative values on failure */
static WARN_UNUSED_RESULT int wc_AesFeedbackCFB8(
    Aes* aes, byte* out, const byte* in, word32 sz, byte dir)
{
    byte *pt;
    int ret = 0;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (sz == 0) {
        return 0;
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    while (sz > 0) {
        ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg);
        if (ret != 0)
            break;
        if (dir == AES_DECRYPTION) {
            pt = (byte*)aes->reg;

            /* LSB + CAT */
            shiftLeftArray(pt, WOLFSSL_BIT_SIZE);
            pt[AES_BLOCK_SIZE - 1] = in[0];
        }

        /* MSB + XOR */
    #ifdef BIG_ENDIAN_ORDER
        ByteReverseWords(aes->tmp, aes->tmp, AES_BLOCK_SIZE);
    #endif
        out[0] = (byte)(aes->tmp[0] ^ in[0]);
        if (dir == AES_ENCRYPTION) {
            pt = (byte*)aes->reg;

            /* LSB + CAT */
            shiftLeftArray(pt, WOLFSSL_BIT_SIZE);
            pt[AES_BLOCK_SIZE - 1] = out[0];
        }

        out += 1;
        in  += 1;
        sz  -= 1;
    }

    RESTORE_VECTOR_REGISTERS();

    return ret;
}


/* returns 0 on success and negative values on failure */
static WARN_UNUSED_RESULT int wc_AesFeedbackCFB1(
    Aes* aes, byte* out, const byte* in, word32 sz, byte dir)
{
    byte tmp;
    byte cur = 0; /* hold current work in order to handle inline in=out */
    byte* pt;
    int bit = 7;
    int ret = 0;

    if (aes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    if (sz == 0) {
        return 0;
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    while (sz > 0) {
        ret = wc_AesEncryptDirect(aes, (byte*)aes->tmp, (byte*)aes->reg);
        if (ret != 0)
            break;
        if (dir == AES_DECRYPTION) {
            pt = (byte*)aes->reg;

            /* LSB + CAT */
            tmp = (0X01 << bit) & in[0];
            tmp = tmp >> bit;
            tmp &= 0x01;
            shiftLeftArray((byte*)aes->reg, 1);
            pt[AES_BLOCK_SIZE - 1] |= tmp;
        }

        /* MSB  + XOR */
        tmp = (0X01 << bit) & in[0];
        pt = (byte*)aes->tmp;
        tmp = (pt[0] >> 7) ^ (tmp >> bit);
        tmp &= 0x01;
        cur |= (tmp << bit);


        if (dir == AES_ENCRYPTION) {
            pt = (byte*)aes->reg;

            /* LSB + CAT */
            shiftLeftArray((byte*)aes->reg, 1);
            pt[AES_BLOCK_SIZE - 1] |= tmp;
        }

        bit--;
        if (bit < 0) {
            out[0] = cur;
            out += 1;
            in  += 1;
            sz  -= 1;
            bit = 7;
            cur = 0;
        }
        else {
            sz -= 1;
        }
    }

    if (ret == 0) {
        if (bit > 0 && bit < 7) {
            out[0] = cur;
        }
    }

    RESTORE_VECTOR_REGISTERS();

    return ret;
}


/* CFB 1
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt (packed to left, i.e. 101 is 0x90)
 * sz  size of input buffer in bits (0x1 would be size of 1 and 0xFF size of 8)
 *
 * returns 0 on success and negative values on failure
 */
int wc_AesCfb1Encrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackCFB1(aes, out, in, sz, AES_ENCRYPTION);
}


/* CFB 8
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative values on failure
 */
int wc_AesCfb8Encrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackCFB8(aes, out, in, sz, AES_ENCRYPTION);
}
#ifdef HAVE_AES_DECRYPT

/* CFB 1
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt
 * sz  size of input buffer in bits (0x1 would be size of 1 and 0xFF size of 8)
 *
 * returns 0 on success and negative values on failure
 */
int wc_AesCfb1Decrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackCFB1(aes, out, in, sz, AES_DECRYPTION);
}


/* CFB 8
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative values on failure
 */
int wc_AesCfb8Decrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackCFB8(aes, out, in, sz, AES_DECRYPTION);
}
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_CFB */

#ifdef WOLFSSL_AES_OFB
/* OFB
 *
 * aes structure holding key to use for encryption
 * out buffer to hold result of encryption (must be at least as large as input
 *     buffer)
 * in  buffer to encrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative error values on failure
 */
/* Software AES - CFB Encrypt */
int wc_AesOfbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackEncrypt(aes, out, in, sz, AES_OFB_MODE);
}


#ifdef HAVE_AES_DECRYPT
/* OFB
 *
 * aes structure holding key to use for decryption
 * out buffer to hold result of decryption (must be at least as large as input
 *     buffer)
 * in  buffer to decrypt
 * sz  size of input buffer
 *
 * returns 0 on success and negative error values on failure
 */
/* Software AES - OFB Decrypt */
int wc_AesOfbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_AesFeedbackDecrypt(aes, out, in, sz, AES_OFB_MODE);
}
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_OFB */


#ifdef HAVE_AES_KEYWRAP

/* Initialize key wrap counter with value */
static WC_INLINE void InitKeyWrapCounter(byte* inOutCtr, word32 value)
{
    word32 i;
    word32 bytes;

    bytes = sizeof(word32);
    for (i = 0; i < sizeof(word32); i++) {
        inOutCtr[i+sizeof(word32)] = (byte)(value >> ((bytes - 1) * 8));
        bytes--;
    }
}

/* Increment key wrap counter */
static WC_INLINE void IncrementKeyWrapCounter(byte* inOutCtr)
{
    int i;

    /* in network byte order so start at end and work back */
    for (i = KEYWRAP_BLOCK_SIZE - 1; i >= 0; i--) {
        if (++inOutCtr[i])  /* we're done unless we overflow */
            return;
    }
}

/* Decrement key wrap counter */
static WC_INLINE void DecrementKeyWrapCounter(byte* inOutCtr)
{
    int i;

    for (i = KEYWRAP_BLOCK_SIZE - 1; i >= 0; i--) {
        if (--inOutCtr[i] != 0xFF)  /* we're done unless we underflow */
            return;
    }
}

int wc_AesKeyWrap_ex(Aes *aes, const byte* in, word32 inSz, byte* out,
        word32 outSz, const byte* iv)
{
    word32 i;
    byte* r;
    int j;
    int ret = 0;

    byte t[KEYWRAP_BLOCK_SIZE];
    byte tmp[AES_BLOCK_SIZE];

    /* n must be at least 2 64-bit blocks, output size is (n + 1) 8 bytes (64-bit) */
    if (aes == NULL || in  == NULL || inSz < 2*KEYWRAP_BLOCK_SIZE ||
        out == NULL || outSz < (inSz + KEYWRAP_BLOCK_SIZE))
        return BAD_FUNC_ARG;

    /* input must be multiple of 64-bits */
    if (inSz % KEYWRAP_BLOCK_SIZE != 0)
        return BAD_FUNC_ARG;

    r = out + 8;
    XMEMCPY(r, in, inSz);
    XMEMSET(t, 0, sizeof(t));

    /* user IV is optional */
    if (iv == NULL) {
        XMEMSET(tmp, 0xA6, KEYWRAP_BLOCK_SIZE);
    } else {
        XMEMCPY(tmp, iv, KEYWRAP_BLOCK_SIZE);
    }

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    for (j = 0; j <= 5; j++) {
        for (i = 1; i <= inSz / KEYWRAP_BLOCK_SIZE; i++) {
            /* load R[i] */
            XMEMCPY(tmp + KEYWRAP_BLOCK_SIZE, r, KEYWRAP_BLOCK_SIZE);

            ret = wc_AesEncryptDirect(aes, tmp, tmp);
            if (ret != 0)
                break;

            /* calculate new A */
            IncrementKeyWrapCounter(t);
            xorbuf(tmp, t, KEYWRAP_BLOCK_SIZE);

            /* save R[i] */
            XMEMCPY(r, tmp + KEYWRAP_BLOCK_SIZE, KEYWRAP_BLOCK_SIZE);
            r += KEYWRAP_BLOCK_SIZE;
        }
        if (ret != 0)
            break;
        r = out + KEYWRAP_BLOCK_SIZE;
    }
    RESTORE_VECTOR_REGISTERS();

    if (ret != 0)
        return ret;

    /* C[0] = A */
    XMEMCPY(out, tmp, KEYWRAP_BLOCK_SIZE);

    return (int)(inSz + KEYWRAP_BLOCK_SIZE);
}

/* perform AES key wrap (RFC3394), return out sz on success, negative on err */
int wc_AesKeyWrap(const byte* key, word32 keySz, const byte* in, word32 inSz,
                  byte* out, word32 outSz, const byte* iv)
{
#ifdef WOLFSSL_SMALL_STACK
    Aes *aes = NULL;
#else
    Aes aes[1];
#endif
    int ret;

    if (key == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    if ((aes = (Aes *)XMALLOC(sizeof *aes, NULL,
                              DYNAMIC_TYPE_AES)) == NULL)
        return MEMORY_E;
#endif

    ret = wc_AesInit(aes, NULL, INVALID_DEVID);
    if (ret != 0)
        goto out;

    ret = wc_AesSetKey(aes, key, keySz, NULL, AES_ENCRYPTION);
    if (ret != 0) {
        wc_AesFree(aes);
        goto out;
    }

    ret = wc_AesKeyWrap_ex(aes, in, inSz, out, outSz, iv);

    wc_AesFree(aes);

  out:
#ifdef WOLFSSL_SMALL_STACK
    if (aes != NULL)
        XFREE(aes, NULL, DYNAMIC_TYPE_AES);
#endif

    return ret;
}

int wc_AesKeyUnWrap_ex(Aes *aes, const byte* in, word32 inSz, byte* out,
        word32 outSz, const byte* iv)
{
    byte* r;
    word32 i, n;
    int j;
    int ret = 0;

    byte t[KEYWRAP_BLOCK_SIZE];
    byte tmp[AES_BLOCK_SIZE];

    const byte* expIv;
    const byte defaultIV[] = {
        0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6
    };

    if (aes == NULL || in == NULL || inSz < 3 * KEYWRAP_BLOCK_SIZE ||
        out == NULL || outSz < (inSz - KEYWRAP_BLOCK_SIZE))
        return BAD_FUNC_ARG;

    /* input must be multiple of 64-bits */
    if (inSz % KEYWRAP_BLOCK_SIZE != 0)
        return BAD_FUNC_ARG;

    /* user IV optional */
    if (iv != NULL)
        expIv = iv;
    else
        expIv = defaultIV;

    /* A = C[0], R[i] = C[i] */
    XMEMCPY(tmp, in, KEYWRAP_BLOCK_SIZE);
    XMEMCPY(out, in + KEYWRAP_BLOCK_SIZE, inSz - KEYWRAP_BLOCK_SIZE);
    XMEMSET(t, 0, sizeof(t));

    SAVE_VECTOR_REGISTERS(return _svr_ret;);

    /* initialize counter to 6n */
    n = (inSz - 1) / KEYWRAP_BLOCK_SIZE;
    InitKeyWrapCounter(t, 6 * n);

    for (j = 5; j >= 0; j--) {
        for (i = n; i >= 1; i--) {

            /* calculate A */
            xorbuf(tmp, t, KEYWRAP_BLOCK_SIZE);
            DecrementKeyWrapCounter(t);

            /* load R[i], starting at end of R */
            r = out + ((i - 1) * KEYWRAP_BLOCK_SIZE);
            XMEMCPY(tmp + KEYWRAP_BLOCK_SIZE, r, KEYWRAP_BLOCK_SIZE);
            ret = wc_AesDecryptDirect(aes, tmp, tmp);
            if (ret != 0)
                break;

            /* save R[i] */
            XMEMCPY(r, tmp + KEYWRAP_BLOCK_SIZE, KEYWRAP_BLOCK_SIZE);
        }
        if (ret != 0)
            break;
    }
    RESTORE_VECTOR_REGISTERS();

    if (ret != 0)
        return ret;

    /* verify IV */
    if (XMEMCMP(tmp, expIv, KEYWRAP_BLOCK_SIZE) != 0)
        return BAD_KEYWRAP_IV_E;

    return (int)(inSz - KEYWRAP_BLOCK_SIZE);
}

int wc_AesKeyUnWrap(const byte* key, word32 keySz, const byte* in, word32 inSz,
                    byte* out, word32 outSz, const byte* iv)
{
#ifdef WOLFSSL_SMALL_STACK
    Aes *aes = NULL;
#else
    Aes aes[1];
#endif
    int ret;

    (void)iv;

    if (key == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    if ((aes = (Aes *)XMALLOC(sizeof *aes, NULL,
                              DYNAMIC_TYPE_AES)) == NULL)
        return MEMORY_E;
#endif


    ret = wc_AesInit(aes, NULL, INVALID_DEVID);
    if (ret != 0)
        goto out;

    ret = wc_AesSetKey(aes, key, keySz, NULL, AES_DECRYPTION);
    if (ret != 0) {
        wc_AesFree(aes);
        goto out;
    }

    ret = wc_AesKeyUnWrap_ex(aes, in, inSz, out, outSz, iv);

    wc_AesFree(aes);

  out:
#ifdef WOLFSSL_SMALL_STACK
    if (aes)
        XFREE(aes, NULL, DYNAMIC_TYPE_AES);
#endif

    return ret;
}

#endif /* HAVE_AES_KEYWRAP */

#ifdef WOLFSSL_AES_XTS

/* Galios Field to use */
#define GF_XTS 0x87

/* This is to help with setting keys to correct encrypt or decrypt type.
 *
 * tweak AES key for tweak in XTS
 * aes   AES key for encrypt/decrypt process
 * key   buffer holding aes key | tweak key
 * len   length of key buffer in bytes. Should be twice that of key size. i.e.
 *       32 for a 16 byte key.
 * dir   direction, either AES_ENCRYPTION or AES_DECRYPTION
 * heap  heap hint to use for memory. Can be NULL
 * devId id to use with async crypto. Can be 0
 *
 * Note: is up to user to call wc_AesFree on tweak and aes key when done.
 *
 * return 0 on success
 */
int wc_AesXtsSetKey(XtsAes* aes, const byte* key, word32 len, int dir,
        void* heap, int devId)
{
    word32 keySz;
    int    ret = 0;

    if (aes == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if ((ret = wc_AesInit(&aes->tweak, heap, devId)) != 0) {
        return ret;
    }
    if ((ret = wc_AesInit(&aes->aes, heap, devId)) != 0) {
        return ret;
    }

    keySz = len/2;
    if (keySz != 16 && keySz != 32) {
        WOLFSSL_MSG("Unsupported key size");
        return WC_KEY_SIZE_E;
    }

    if ((ret = wc_AesSetKey(&aes->aes, key, keySz, NULL, dir)) == 0) {
        ret = wc_AesSetKey(&aes->tweak, key + keySz, keySz, NULL,
                AES_ENCRYPTION);
        if (ret != 0) {
            wc_AesFree(&aes->aes);
        }
    }

    return ret;
}


/* This is used to free up resources used by Aes structs
 *
 * aes AES keys to free
 *
 * return 0 on success
 */
int wc_AesXtsFree(XtsAes* aes)
{
    if (aes != NULL) {
        wc_AesFree(&aes->aes);
        wc_AesFree(&aes->tweak);
    }

    return 0;
}


/* Same process as wc_AesXtsEncrypt but uses a word64 type as the tweak value
 * instead of a byte array. This just converts the word64 to a byte array and
 * calls wc_AesXtsEncrypt.
 *
 * aes    AES keys to use for block encrypt/decrypt
 * out    output buffer to hold cipher text
 * in     input plain text buffer to encrypt
 * sz     size of both out and in buffers
 * sector value to use for tweak
 *
 * returns 0 on success
 */
int wc_AesXtsEncryptSector(XtsAes* aes, byte* out, const byte* in,
        word32 sz, word64 sector)
{
    byte* pt;
    byte  i[AES_BLOCK_SIZE];

    XMEMSET(i, 0, AES_BLOCK_SIZE);
#ifdef BIG_ENDIAN_ORDER
    sector = ByteReverseWord64(sector);
#endif
    pt = (byte*)&sector;
    XMEMCPY(i, pt, sizeof(word64));

    return wc_AesXtsEncrypt(aes, out, in, sz, (const byte*)i, AES_BLOCK_SIZE);
}


/* Same process as wc_AesXtsDecrypt but uses a word64 type as the tweak value
 * instead of a byte array. This just converts the word64 to a byte array.
 *
 * aes    AES keys to use for block encrypt/decrypt
 * out    output buffer to hold plain text
 * in     input cipher text buffer to encrypt
 * sz     size of both out and in buffers
 * sector value to use for tweak
 *
 * returns 0 on success
 */
int wc_AesXtsDecryptSector(XtsAes* aes, byte* out, const byte* in, word32 sz,
        word64 sector)
{
    byte* pt;
    byte  i[AES_BLOCK_SIZE];

    XMEMSET(i, 0, AES_BLOCK_SIZE);
#ifdef BIG_ENDIAN_ORDER
    sector = ByteReverseWord64(sector);
#endif
    pt = (byte*)&sector;
    XMEMCPY(i, pt, sizeof(word64));

    return wc_AesXtsDecrypt(aes, out, in, sz, (const byte*)i, AES_BLOCK_SIZE);
}

#ifdef HAVE_AES_ECB
/* helper function for encrypting / decrypting full buffer at once */
static WARN_UNUSED_RESULT int _AesXtsHelper(
    Aes* aes, byte* out, const byte* in, word32 sz, int dir)
{
    word32 outSz   = sz;
    word32 totalSz = (sz / AES_BLOCK_SIZE) * AES_BLOCK_SIZE; /* total bytes */
    byte*  pt      = out;

    outSz -= AES_BLOCK_SIZE;

    while (outSz > 0) {
        word32 j;
        byte carry = 0;

        /* multiply by shift left and propagate carry */
        for (j = 0; j < AES_BLOCK_SIZE && outSz > 0; j++, outSz--) {
            byte tmpC;

            tmpC   = (pt[j] >> 7) & 0x01;
            pt[j+AES_BLOCK_SIZE] = (byte)((pt[j] << 1) + carry);
            carry  = tmpC;
        }
        if (carry) {
            pt[AES_BLOCK_SIZE] ^= GF_XTS;
        }

        pt += AES_BLOCK_SIZE;
    }

    xorbuf(out, in, totalSz);
    if (dir == AES_ENCRYPTION) {
        return _AesEcbEncrypt(aes, out, out, totalSz);
    }
    else {
        return _AesEcbDecrypt(aes, out, out, totalSz);
    }
}
#endif /* HAVE_AES_ECB */


/* AES with XTS mode. (XTS) XEX encryption with Tweak and cipher text Stealing.
 *
 * xaes  AES keys to use for block encrypt/decrypt
 * out   output buffer to hold cipher text
 * in    input plain text buffer to encrypt
 * sz    size of both out and in buffers
 * i     value to use for tweak
 * iSz   size of i buffer, should always be AES_BLOCK_SIZE but having this input
 *       adds a sanity check on how the user calls the function.
 *
 * returns 0 on success
 */
/* Software AES - XTS Encrypt  */
int wc_AesXtsEncrypt(XtsAes* xaes, byte* out, const byte* in, word32 sz,
        const byte* i, word32 iSz)
{
    int ret = 0;
    word32 blocks = (sz / AES_BLOCK_SIZE);
    Aes *aes, *tweak;

    if (xaes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    aes   = &xaes->aes;
    tweak = &xaes->tweak;

    if (iSz < AES_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }

    if (blocks > 0) {
        byte tmp[AES_BLOCK_SIZE];

        XMEMSET(tmp, 0, AES_BLOCK_SIZE); /* set to 0's in case of improper AES
                                          * key setup passed to encrypt direct*/

        SAVE_VECTOR_REGISTERS(return _svr_ret;);

        ret = wc_AesEncryptDirect(tweak, tmp, i);

        if (ret != 0) {
            RESTORE_VECTOR_REGISTERS();
            return ret;
        }

    #ifdef HAVE_AES_ECB
        /* encrypt all of buffer at once when possible */
        if (in != out) { /* can not handle inline */
            XMEMCPY(out, tmp, AES_BLOCK_SIZE);
            if ((ret = _AesXtsHelper(aes, out, in, sz, AES_ENCRYPTION)) != 0) {
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        }
    #endif

        while (blocks > 0) {
            word32 j;
            byte carry = 0;

    #ifdef HAVE_AES_ECB
            if (in == out)
    #endif
            { /* check for if inline */
                byte buf[AES_BLOCK_SIZE];

                XMEMCPY(buf, in, AES_BLOCK_SIZE);
                xorbuf(buf, tmp, AES_BLOCK_SIZE);
                ret = wc_AesEncryptDirect(aes, out, buf);
                if (ret != 0) {
                    RESTORE_VECTOR_REGISTERS();
                    return ret;
                }
            }
            xorbuf(out, tmp, AES_BLOCK_SIZE);

            /* multiply by shift left and propagate carry */
            for (j = 0; j < AES_BLOCK_SIZE; j++) {
                byte tmpC;

                tmpC   = (tmp[j] >> 7) & 0x01;
                tmp[j] = (byte)((tmp[j] << 1) + carry);
                carry  = tmpC;
            }
            if (carry) {
                tmp[0] ^= GF_XTS;
            }

            in  += AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
            sz  -= AES_BLOCK_SIZE;
            blocks--;
        }

        /* stealing operation of XTS to handle left overs */
        if (sz > 0) {
            byte buf[AES_BLOCK_SIZE];

            XMEMCPY(buf, out - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
            if (sz >= AES_BLOCK_SIZE) { /* extra sanity check before copy */
                RESTORE_VECTOR_REGISTERS();
                return BUFFER_E;
            }
            if (in != out) {
                XMEMCPY(out, buf, sz);
                XMEMCPY(buf, in, sz);
            }
            else {
                byte buf2[AES_BLOCK_SIZE];

                XMEMCPY(buf2, buf, sz);
                XMEMCPY(buf, in, sz);
                XMEMCPY(out, buf2, sz);
            }

            xorbuf(buf, tmp, AES_BLOCK_SIZE);
            ret = wc_AesEncryptDirect(aes, out - AES_BLOCK_SIZE, buf);
            if (ret == 0)
                xorbuf(out - AES_BLOCK_SIZE, tmp, AES_BLOCK_SIZE);
        }
        RESTORE_VECTOR_REGISTERS();
    }
    else {
        WOLFSSL_MSG("Plain text input too small for encryption");
        return BAD_FUNC_ARG;
    }

    return ret;
}


/* Same process as encryption but Aes key is AES_DECRYPTION type.
 *
 * xaes  AES keys to use for block encrypt/decrypt
 * out   output buffer to hold plain text
 * in    input cipher text buffer to decrypt
 * sz    size of both out and in buffers
 * i     value to use for tweak
 * iSz   size of i buffer, should always be AES_BLOCK_SIZE but having this input
 *       adds a sanity check on how the user calls the function.
 *
 * returns 0 on success
 */
/* Software AES - XTS Decrypt */
int wc_AesXtsDecrypt(XtsAes* xaes, byte* out, const byte* in, word32 sz,
        const byte* i, word32 iSz)
{
    int ret = 0;
    word32 blocks = (sz / AES_BLOCK_SIZE);
    Aes *aes, *tweak;

    if (xaes == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

    aes   = &xaes->aes;
    tweak = &xaes->tweak;

    if (iSz < AES_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }

    if (blocks > 0) {
        word32 j;
        byte carry = 0;
        byte tmp[AES_BLOCK_SIZE];
        byte stl = (sz % AES_BLOCK_SIZE);

        XMEMSET(tmp, 0, AES_BLOCK_SIZE); /* set to 0's in case of improper AES
                                          * key setup passed to decrypt direct*/

        SAVE_VECTOR_REGISTERS(return _svr_ret;);

        ret = wc_AesEncryptDirect(tweak, tmp, i);
        if (ret != 0) {
            RESTORE_VECTOR_REGISTERS();
            return ret;
        }

        /* if Stealing then break out of loop one block early to handle special
         * case */
        if (stl > 0) {
            blocks--;
        }

    #ifdef HAVE_AES_ECB
        /* decrypt all of buffer at once when possible */
        if (in != out) { /* can not handle inline */
            XMEMCPY(out, tmp, AES_BLOCK_SIZE);
            if ((ret = _AesXtsHelper(aes, out, in, sz, AES_DECRYPTION)) != 0) {
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
        }
    #endif

        while (blocks > 0) {
    #ifdef HAVE_AES_ECB
            if (in == out)
    #endif
            { /* check for if inline */
                byte buf[AES_BLOCK_SIZE];

                XMEMCPY(buf, in, AES_BLOCK_SIZE);
                xorbuf(buf, tmp, AES_BLOCK_SIZE);
                ret = wc_AesDecryptDirect(aes, out, buf);
                if (ret != 0) {
                    RESTORE_VECTOR_REGISTERS();
                    return ret;
                }
            }
            xorbuf(out, tmp, AES_BLOCK_SIZE);

            /* multiply by shift left and propagate carry */
            for (j = 0; j < AES_BLOCK_SIZE; j++) {
                byte tmpC;

                tmpC   = (tmp[j] >> 7) & 0x01;
                tmp[j] = (byte)((tmp[j] << 1) + carry);
                carry  = tmpC;
            }
            if (carry) {
                tmp[0] ^= GF_XTS;
            }
            carry = 0;

            in  += AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
            sz  -= AES_BLOCK_SIZE;
            blocks--;
        }

        /* stealing operation of XTS to handle left overs */
        if (sz >= AES_BLOCK_SIZE) {
            byte buf[AES_BLOCK_SIZE];
            byte tmp2[AES_BLOCK_SIZE];

            /* multiply by shift left and propagate carry */
            for (j = 0; j < AES_BLOCK_SIZE; j++) {
                byte tmpC;

                tmpC   = (tmp[j] >> 7) & 0x01;
                tmp2[j] = (byte)((tmp[j] << 1) + carry);
                carry  = tmpC;
            }
            if (carry) {
                tmp2[0] ^= GF_XTS;
            }

            XMEMCPY(buf, in, AES_BLOCK_SIZE);
            xorbuf(buf, tmp2, AES_BLOCK_SIZE);
            ret = wc_AesDecryptDirect(aes, out, buf);
            if (ret != 0) {
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
            xorbuf(out, tmp2, AES_BLOCK_SIZE);

            /* tmp2 holds partial | last */
            XMEMCPY(tmp2, out, AES_BLOCK_SIZE);
            in  += AES_BLOCK_SIZE;
            out += AES_BLOCK_SIZE;
            sz  -= AES_BLOCK_SIZE;

            /* Make buffer with end of cipher text | last */
            XMEMCPY(buf, tmp2, AES_BLOCK_SIZE);
            if (sz >= AES_BLOCK_SIZE) { /* extra sanity check before copy */
                RESTORE_VECTOR_REGISTERS();
                return BUFFER_E;
            }
            XMEMCPY(buf, in,   sz);
            XMEMCPY(out, tmp2, sz);

            xorbuf(buf, tmp, AES_BLOCK_SIZE);
            ret = wc_AesDecryptDirect(aes, tmp2, buf);
            if (ret != 0) {
                RESTORE_VECTOR_REGISTERS();
                return ret;
            }
            xorbuf(tmp2, tmp, AES_BLOCK_SIZE);
            XMEMCPY(out - AES_BLOCK_SIZE, tmp2, AES_BLOCK_SIZE);
        }
        RESTORE_VECTOR_REGISTERS();
    }
    else {
        WOLFSSL_MSG("Plain text input too small for encryption");
        return BAD_FUNC_ARG;
    }

    return ret;
}

#endif /* WOLFSSL_AES_XTS */

#ifdef WOLFSSL_AES_SIV

/*
 * See RFC 5297 Section 2.4.
 */
static WARN_UNUSED_RESULT int S2V(
    const byte* key, word32 keySz, const byte* assoc, word32 assocSz,
    const byte* nonce, word32 nonceSz, const byte* data,
    word32 dataSz, byte* out)
{
#ifdef WOLFSSL_SMALL_STACK
    byte* tmp[3] = {NULL, NULL, NULL};
    int i;
    Cmac* cmac;
#else
    byte tmp[3][AES_BLOCK_SIZE];
    Cmac cmac[1];
#endif
    word32 macSz = AES_BLOCK_SIZE;
    int ret = 0;
    word32 zeroBytes;

#ifdef WOLFSSL_SMALL_STACK
    for (i = 0; i < 3; ++i) {
        tmp[i] = (byte*)XMALLOC(AES_BLOCK_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (tmp[i] == NULL) {
            ret = MEMORY_E;
            break;
        }
    }
    if (ret == 0)
#endif
    {
        XMEMSET(tmp[1], 0, AES_BLOCK_SIZE);
        XMEMSET(tmp[2], 0, AES_BLOCK_SIZE);

        ret = wc_AesCmacGenerate(tmp[0], &macSz, tmp[1], AES_BLOCK_SIZE,
                                 key, keySz);
        if (ret == 0) {
            ShiftAndXorRb(tmp[1], tmp[0]);
            ret = wc_AesCmacGenerate(tmp[0], &macSz, assoc, assocSz, key,
                                     keySz);
            if (ret == 0) {
                xorbuf(tmp[1], tmp[0], AES_BLOCK_SIZE);
            }
        }
    }

    if (ret == 0) {
        if (nonceSz > 0) {
            ShiftAndXorRb(tmp[0], tmp[1]);
            ret = wc_AesCmacGenerate(tmp[1], &macSz, nonce, nonceSz, key,
                                     keySz);
            if (ret == 0) {
                xorbuf(tmp[0], tmp[1], AES_BLOCK_SIZE);
            }
        }
        else {
            XMEMCPY(tmp[0], tmp[1], AES_BLOCK_SIZE);
        }
    }

    if (ret == 0) {
        if (dataSz >= AES_BLOCK_SIZE) {

        #ifdef WOLFSSL_SMALL_STACK
            cmac = (Cmac*)XMALLOC(sizeof(Cmac), NULL, DYNAMIC_TYPE_CMAC);
            if (cmac == NULL) {
                ret = MEMORY_E;
            }
            if (ret == 0)
        #endif
            {
            #ifdef WOLFSSL_CHECK_MEM_ZERO
                /* Aes part is checked by wc_AesFree. */
                wc_MemZero_Add("wc_AesCmacGenerate cmac",
                    ((unsigned char *)cmac) + sizeof(Aes),
                    sizeof(Cmac) - sizeof(Aes));
            #endif
                xorbuf(tmp[0], data + (dataSz - AES_BLOCK_SIZE),
                       AES_BLOCK_SIZE);
                ret = wc_InitCmac(cmac, key, keySz, WC_CMAC_AES, NULL);
                if (ret == 0) {
                    ret = wc_CmacUpdate(cmac, data, dataSz - AES_BLOCK_SIZE);
                }
                if (ret == 0) {
                    ret = wc_CmacUpdate(cmac, tmp[0], AES_BLOCK_SIZE);
                }
                if (ret == 0) {
                    ret = wc_CmacFinal(cmac, out, &macSz);
                }
            }
        #ifdef WOLFSSL_SMALL_STACK
            if (cmac != NULL) {
                XFREE(cmac, NULL, DYNAMIC_TYPE_CMAC);
            }
        #elif defined(WOLFSSL_CHECK_MEM_ZERO)
            wc_MemZero_Check(cmac, sizeof(Cmac));
        #endif
        }
        else {
            XMEMCPY(tmp[2], data, dataSz);
            tmp[2][dataSz] |= 0x80;
            zeroBytes = AES_BLOCK_SIZE - (dataSz + 1);
            if (zeroBytes != 0) {
                XMEMSET(tmp[2] + dataSz + 1, 0, zeroBytes);
            }
            ShiftAndXorRb(tmp[1], tmp[0]);
            xorbuf(tmp[1], tmp[2], AES_BLOCK_SIZE);
            ret = wc_AesCmacGenerate(out, &macSz, tmp[1], AES_BLOCK_SIZE, key,
                                     keySz);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    for (i = 0; i < 3; ++i) {
        if (tmp[i] != NULL) {
            XFREE(tmp[i], NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
    }
#endif

    return ret;
}

static WARN_UNUSED_RESULT int AesSivCipher(
    const byte* key, word32 keySz, const byte* assoc,
    word32 assocSz, const byte* nonce, word32 nonceSz,
    const byte* data, word32 dataSz, byte* siv, byte* out,
    int enc)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    Aes* aes = NULL;
#else
    Aes aes[1];
#endif
    byte sivTmp[AES_BLOCK_SIZE];

    if (key == NULL || siv == NULL || out == NULL) {
        WOLFSSL_MSG("Bad parameter");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0 && keySz != 32 && keySz != 48 && keySz != 64) {
        WOLFSSL_MSG("Bad key size. Must be 256, 384, or 512 bits.");
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (enc == 1) {
            ret = S2V(key, keySz / 2, assoc, assocSz, nonce, nonceSz, data,
                      dataSz, sivTmp);
            if (ret != 0) {
                WOLFSSL_MSG("S2V failed.");
            }
            else {
                XMEMCPY(siv, sivTmp, AES_BLOCK_SIZE);
            }
        }
        else {
            XMEMCPY(sivTmp, siv, AES_BLOCK_SIZE);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    if (ret == 0) {
        aes = (Aes*)XMALLOC(sizeof(Aes), NULL, DYNAMIC_TYPE_AES);
        if (aes == NULL) {
            ret = MEMORY_E;
        }
    }
#endif

    if (ret == 0) {
        ret = wc_AesInit(aes, NULL, INVALID_DEVID);
        if (ret != 0) {
            WOLFSSL_MSG("Failed to initialized AES object.");
        }
    }

    if (ret == 0 && dataSz > 0) {
        sivTmp[12] &= 0x7f;
        sivTmp[8] &= 0x7f;
        ret = wc_AesSetKey(aes, key + keySz / 2, keySz / 2, sivTmp,
                           AES_ENCRYPTION);
        if (ret != 0) {
            WOLFSSL_MSG("Failed to set key for AES-CTR.");
        }
        else {
            ret = wc_AesCtrEncrypt(aes, out, data, dataSz);
            if (ret != 0) {
                WOLFSSL_MSG("AES-CTR encryption failed.");
            }
        }
    }

    if (ret == 0 && enc == 0) {
        ret = S2V(key, keySz / 2, assoc, assocSz, nonce, nonceSz, out, dataSz,
                  sivTmp);
        if (ret != 0) {
            WOLFSSL_MSG("S2V failed.");
        }

        if (XMEMCMP(siv, sivTmp, AES_BLOCK_SIZE) != 0) {
            WOLFSSL_MSG("Computed SIV doesn't match received SIV.");
            ret = AES_SIV_AUTH_E;
        }
    }

    wc_AesFree(aes);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(aes, NULL, DYNAMIC_TYPE_AES);
#endif

    return ret;
}

/*
 * See RFC 5297 Section 2.6.
 */
int wc_AesSivEncrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out)
{
    return AesSivCipher(key, keySz, assoc, assocSz, nonce, nonceSz, in, inSz,
                        siv, out, 1);
}

/*
 * See RFC 5297 Section 2.7.
 */
int wc_AesSivDecrypt(const byte* key, word32 keySz, const byte* assoc,
                     word32 assocSz, const byte* nonce, word32 nonceSz,
                     const byte* in, word32 inSz, byte* siv, byte* out)
{
    return AesSivCipher(key, keySz, assoc, assocSz, nonce, nonceSz, in, inSz,
                        siv, out, 0);
}

#endif /* WOLFSSL_AES_SIV */

#endif /* !NO_AES */
