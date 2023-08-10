/* wolfssl_sce_unit_test.c
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/rsa.h>
#include <wolfssl/wolfcrypt/port/Renesas/renesas-sce-crypt.h>

#include "FreeRTOS.h"

#ifndef NO_SHA
 int sha_test();
#endif

#ifndef NO_SHA256
 int sha256_test();
#endif

#define STACK_SIZE (1 * 1024)

#define RESULT_STR(ret)  if (ret == 0)\
                          printf(" passed \n");\
                         else \
                          printf(" failed \n");

static xSemaphoreHandle exit_semaph;
static byte exit_loop = 0;
static byte sha256_multTst_rslt1 = 0;
static byte sha256_multTst_rslt2 = 0;
static byte Aes128_Cbc_multTst_rslt = 0;
static byte Aes256_Cbc_multTst_rslt = 0;
static byte Aes128_Gcm_multTst_rslt = 0;
static byte Aes256_Gcm_multTst_rslt = 0;

int sce_crypt_AesCbc_multitest();
int sce_crypt_AesGcm_multitest();
int sce_crypt_Sha_AesCbcGcm_multitest();
int sce_crypt_sha_multitest();
int sce_crypt_test();

extern User_SCEPKCbInfo guser_PKCbInfo;

#if defined(HAVE_AES_CBC)

#if defined(WOLFSSL_AES_128)
    sce_aes_wrapped_key_t g_user_aes128_key_index1;
    sce_aes_wrapped_key_t g_user_aes128_key_index2;
#endif

#if defined(WOLFSSL_AES_256)
    sce_aes_wrapped_key_t g_user_aes256_key_index1;
    sce_aes_wrapped_key_t g_user_aes256_key_index2;
#endif

#endif

#if !defined(NO_RSA)
    sce_rsa2048_wrapped_pair_key_t g_wrapped_pair_key;
#endif
typedef struct tagInfo
{
    sce_aes_wrapped_key_t aes_key;
} Info;

#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_128)

static int sce_aes_cbc_test(int prnt, sce_aes_wrapped_key_t* aes_key)
{

    Aes  aes[1];

    byte cipher[AES_BLOCK_SIZE];
    byte plain[AES_BLOCK_SIZE];
    int  ret = 0;

    WOLFSSL_SMALL_STACK_STATIC const byte msg[] = { 
       /* "Now is the time for all " w/o trailing 0 */
        0x6e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };
    byte key[] = "0123456789abcdef   ";  /* align */
    byte iv[]  = "1234567890abcdef   ";  /* align */

    XMEMSET(cipher, 0, AES_BLOCK_SIZE);
    XMEMSET(plain, 0, AES_BLOCK_SIZE);

    if (prnt) {
        printf(" sce_aes_cbc_test() ");
    }
    
    ret = wc_AesInit(aes, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_AesSetKey(aes, key, AES_BLOCK_SIZE, iv, AES_ENCRYPTION);
        XMEMCPY(&aes->ctx.sce_wrapped_key, aes_key,
                        sizeof(sce_aes_wrapped_key_t));

        aes->ctx.keySize = (word32)aes->keylen;
        if (ret == 0) {
            ret = wc_sce_AesCbcEncrypt(aes, cipher, msg, AES_BLOCK_SIZE);
        }

        wc_AesFree(aes);
    }

    if (ret != 0)
        ret = -1;

#ifdef HAVE_AES_DECRYPT
    ret = wc_AesInit(aes, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_AesSetKey(aes, key, AES_BLOCK_SIZE, iv, AES_DECRYPTION);
        XMEMCPY(&aes->ctx.sce_wrapped_key, aes_key,
                sizeof(sce_aes_wrapped_key_t));
        aes->ctx.keySize = (word32)aes->keylen;
        if (ret == 0)
            ret = wc_sce_AesCbcDecrypt(aes, plain, cipher, AES_BLOCK_SIZE);

        wc_AesFree(aes);
    }
    if (ret != 0)
        ret = -2;
    if (XMEMCMP(plain, msg, AES_BLOCK_SIZE) != 0)
        ret = -3;
#endif /* HAVE_AES_DECRYPT */

    (void)plain;

    if (prnt) {
        RESULT_STR(ret)
    }
    
    return ret;
}

static void tskAes128_Cbc_Test(void *pvParam)
{
    int ret = 0;
    Info *p = (Info*)pvParam;
    
    while (exit_loop == 0) {
        ret = sce_aes_cbc_test(0, &p->aes_key);
        vTaskDelay(10/portTICK_PERIOD_MS);
        if (ret != 0) {
            printf(" result was not good(%d). sce_aes_cbc_test\n", ret);
            Aes128_Cbc_multTst_rslt = 1;
        }
    }

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}

#endif

#ifdef WOLFSSL_AES_256
static int sce_aes256_test(int prnt, sce_aes_wrapped_key_t* aes_key)
{
    Aes enc[1];
    byte cipher[AES_BLOCK_SIZE];
    byte plain[AES_BLOCK_SIZE];
    Aes dec[1];
    int  ret = 0;

    /* Test vectors from NIST Special Publication 800-38A, 2001 Edition,
     * Appendix F.2.5  */
    WOLFSSL_SMALL_STACK_STATIC const byte msg[] = {
        0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
        0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a
    };


    WOLFSSL_SMALL_STACK_STATIC byte key[] = {
        0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
        0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
        0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
        0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
    };
    WOLFSSL_SMALL_STACK_STATIC byte iv[]  = {
        0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
        0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
    };

    if (prnt)
        printf(" sce_aes256_test() ");
    
    if (wc_AesInit(enc, NULL, INVALID_DEVID) != 0) {
        ret = -1;
        goto out;
    }

    if (wc_AesInit(dec, NULL, INVALID_DEVID) != 0){
        ret = -2;
        goto out;
    }

    ret = wc_AesSetKey(enc, key, (int) sizeof(key), iv, AES_ENCRYPTION);
    if (ret != 0){
        ret = -3;
        goto out;
    } else {
        XMEMCPY(&enc->ctx.sce_wrapped_key, aes_key,
                                sizeof(sce_aes_wrapped_key_t));
        enc->ctx.keySize = (word32)enc->keylen;
    }

    ret = wc_AesSetKey(dec, key, (int) sizeof(key), iv, AES_DECRYPTION);
    if (ret != 0) {
        ret = -4;
        goto out;
    } else {
        XMEMCPY(&dec->ctx.sce_wrapped_key, aes_key,
                                sizeof(sce_aes_wrapped_key_t));
        dec->ctx.keySize = (word32)dec->keylen;
    }

    XMEMSET(cipher, 0, AES_BLOCK_SIZE);
    ret = wc_sce_AesCbcEncrypt(enc, cipher, msg, (int) sizeof(msg));

    if (ret != 0) {
        ret = -5;
        goto out;
    }

    XMEMSET(plain, 0, AES_BLOCK_SIZE);
    ret = wc_sce_AesCbcDecrypt(dec, plain, cipher, (int) sizeof(cipher));

    if (ret != 0){
        ret = -6;
        goto out;
    }
    if (XMEMCMP(plain, msg, (int) sizeof(plain))) {
        ret = -7;
        goto out;
    }

out:
    wc_AesFree(enc);
    wc_AesFree(dec);

    if (prnt) {
        RESULT_STR(ret)
    }
    
    return ret;
}

static void tskAes256_Cbc_Test(void *pvParam)
{
    int ret = 0;
    Info *p = (Info*)pvParam;
    
    while (exit_loop == 0) {
        ret = sce_aes256_test(0, &p->aes_key);
        vTaskDelay(10/portTICK_PERIOD_MS);
        if (ret != 0) {
            printf(" result was not good(%d). sce_aes256_test\n", ret);
            Aes256_Cbc_multTst_rslt = 1;
        }
    }

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}

#endif /* WOLFSSL_AES_256 */

#if defined(WOLFSSL_AES_256)
static int sce_aesgcm256_test(int prnt, sce_aes_wrapped_key_t* aes256_key)
{
    Aes enc[1];
    Aes dec[1];
    User_SCEPKCbInfo userContext;
    
    
    /*
     * This is Test Case 16 from the document Galois/
     * Counter Mode of Operation (GCM) by McGrew and
     * Viega.
     */
    WOLFSSL_SMALL_STACK_STATIC const byte p[] =
    {
        0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
        0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
        0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
        0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
        0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
        0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
        0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
        0xba, 0x63, 0x7b, 0x39
    };

    WOLFSSL_SMALL_STACK_STATIC const byte a[] =
    {
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xab, 0xad, 0xda, 0xd2
    };

    WOLFSSL_SMALL_STACK_STATIC const byte k1[] =
    {
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08
    };

    WOLFSSL_SMALL_STACK_STATIC const byte iv1[] =
    {
        0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,
        0xde, 0xca, 0xf8, 0x88
    };

    WOLFSSL_SMALL_STACK_STATIC const byte c1[] =
    {
        0x52, 0x2d, 0xc1, 0xf0, 0x99, 0x56, 0x7d, 0x07,
        0xf4, 0x7f, 0x37, 0xa3, 0x2a, 0x84, 0x42, 0x7d,
        0x64, 0x3a, 0x8c, 0xdc, 0xbf, 0xe5, 0xc0, 0xc9,
        0x75, 0x98, 0xa2, 0xbd, 0x25, 0x55, 0xd1, 0xaa,
        0x8c, 0xb0, 0x8e, 0x48, 0x59, 0x0d, 0xbb, 0x3d,
        0xa7, 0xb0, 0x8b, 0x10, 0x56, 0x82, 0x88, 0x38,
        0xc5, 0xf6, 0x1e, 0x63, 0x93, 0xba, 0x7a, 0x0a,
        0xbc, 0xc9, 0xf6, 0x62
    };

    WOLFSSL_SMALL_STACK_STATIC const byte t1[] =
    {
        0x76, 0xfc, 0x6e, 0xce, 0x0f, 0x4e, 0x17, 0x68,
        0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b
    };

    byte resultT[sizeof(t1)];
    byte resultP[sizeof(p) + AES_BLOCK_SIZE];
    byte resultC[sizeof(p) + AES_BLOCK_SIZE];
    int  result = 0;
    int  ret;

    (void) result;

    if (prnt) {
        printf(" sce_aes256_gcm_test() ");
    }
    
    XMEMSET(resultT, 0, sizeof(resultT));
    XMEMSET(resultC, 0, sizeof(resultC));
    XMEMSET(resultP, 0, sizeof(resultP));
    XMEMSET(&userContext, 0, sizeof(User_SCEPKCbInfo));

    if (wc_AesInit(enc, NULL, INVALID_DEVID) != 0) {
        ret = -1;
        goto out;
    }
    if (wc_AesInit(dec, NULL, INVALID_DEVID) != 0) {
        ret = -2;
        goto out;
    }

    result = wc_AesGcmSetKey(enc, k1, sizeof(k1));
    if (result != 0) {
        ret = -3;
        goto out;
    } else {
        userContext.sce_wrapped_key_aes256 = (void*)aes256_key;
        userContext.keyflgs_crypt.bits.aes256_installedkey_set = 1;
        enc->ctx.keySize = (word32)enc->keylen;
    }

    /* AES-GCM encrypt and decrypt both use AES encrypt internally */
    result = wc_sce_AesGcmEncrypt(enc, resultC, p, sizeof(p),
                                (byte*)iv1, sizeof(iv1), 
                                resultT, sizeof(resultT),
                                 a, sizeof(a), &userContext);

    if (result != 0) {
        ret = -4;
        goto out;
    }

    result = wc_AesGcmSetKey(dec, k1, sizeof(k1));
    if (result != 0) {
        ret = -7;
        goto out;
    } else {
        dec->ctx.keySize = (word32)enc->keylen;
    }

    result = wc_sce_AesGcmDecrypt(dec, resultP, resultC, sizeof(c1),
                iv1, sizeof(iv1), resultT, sizeof(resultT),
                 a, sizeof(a), &userContext);
    if (result != 0){
        ret = -8;
        goto out;
    }
    if (XMEMCMP(p, resultP, sizeof(p))) {
        ret = -9;
        goto out;
    }

    XMEMSET(resultT, 0, sizeof(resultT));
    XMEMSET(resultC, 0, sizeof(resultC));
    XMEMSET(resultP, 0, sizeof(resultP));

    wc_AesGcmSetKey(enc, k1, sizeof(k1));
    /* AES-GCM encrypt and decrypt both use AES encrypt internally */
    result = wc_sce_AesGcmEncrypt(enc, resultC, p, sizeof(p),
                                (byte*)iv1, sizeof(iv1),
                                resultT + 1, sizeof(resultT) - 1,
                                a, sizeof(a), &userContext);
    if (result != 0) {
        ret = -10;
        goto out;
    }

    result = wc_sce_AesGcmDecrypt(enc, resultP, resultC, sizeof(p),
              iv1, sizeof(iv1), resultT + 1, sizeof(resultT) - 1, 
              a, sizeof(a), &userContext);

    if (result != 0) {
        ret = -11;
        goto out;
    }
    if (XMEMCMP(p, resultP, sizeof(p))) {
        ret = -12;
        goto out;
    }

    ret = 0;

  out:
    wc_AesFree(enc);
    wc_AesFree(dec);
    
    if (prnt) {
        RESULT_STR(ret)
    }
    
    return ret;
}

static void tskAes256_Gcm_Test(void *pvParam)
{
    int ret = 0;
    Info *p = (Info*)pvParam;
    
    while (exit_loop == 0) {
        ret = sce_aesgcm256_test(0, &p->aes_key);
        vTaskDelay(10/portTICK_PERIOD_MS);
        if (ret != 0) {
            printf(" result was not good(%d). sce_aesgcm256_test\n", ret);
            Aes256_Gcm_multTst_rslt = 1;
        }
    }

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

#if defined(WOLFSSL_AES_128)

static int sce_aesgcm128_test(int prnt, sce_aes_wrapped_key_t* aes128_key)
{
    Aes enc[1];
    Aes dec[1];
    User_SCEPKCbInfo userContext;
    
    /*
     * This is Test Case 16 from the document Galois/
     * Counter Mode of Operation (GCM) by McGrew and
     * Viega.
     */
    WOLFSSL_SMALL_STACK_STATIC const byte p[] =
    {
        0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
        0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
        0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
        0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
        0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
        0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
        0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
        0xba, 0x63, 0x7b, 0x39
    };

    WOLFSSL_SMALL_STACK_STATIC const byte t1[] =
    {
        0x76, 0xfc, 0x6e, 0xce, 0x0f, 0x4e, 0x17, 0x68,
        0xcd, 0xdf, 0x88, 0x53, 0xbb, 0x2d, 0x55, 0x1b
    };

    /* The following is an interesting test case from the example
     * FIPS test vectors for AES-GCM. IVlen = 1 byte */
    WOLFSSL_SMALL_STACK_STATIC const byte p3[] =
    {
        0x57, 0xce, 0x45, 0x1f, 0xa5, 0xe2, 0x35, 0xa5,
        0x8e, 0x1a, 0xa2, 0x3b, 0x77, 0xcb, 0xaf, 0xe2
    };

    WOLFSSL_SMALL_STACK_STATIC const byte k3[] =
    {
        0xbb, 0x01, 0xd7, 0x03, 0x81, 0x1c, 0x10, 0x1a,
        0x35, 0xe0, 0xff, 0xd2, 0x91, 0xba, 0xf2, 0x4b
    };

    WOLFSSL_SMALL_STACK_STATIC const byte iv3[] =
    {
        0xca
    };

    WOLFSSL_SMALL_STACK_STATIC const byte c3[] =
    {
        0x6b, 0x5f, 0xb3, 0x9d, 0xc1, 0xc5, 0x7a, 0x4f,
        0xf3, 0x51, 0x4d, 0xc2, 0xd5, 0xf0, 0xd0, 0x07
    };

    WOLFSSL_SMALL_STACK_STATIC const byte a3[] =
    {
        0x40, 0xfc, 0xdc, 0xd7, 0x4a, 0xd7, 0x8b, 0xf1,
        0x3e, 0x7c, 0x60, 0x55, 0x50, 0x51, 0xdd, 0x54
    };

    WOLFSSL_SMALL_STACK_STATIC const byte t3[] =
    {
        0x06, 0x90, 0xed, 0x01, 0x34, 0xdd, 0xc6, 0x95,
        0x31, 0x2e, 0x2a, 0xf9, 0x57, 0x7a, 0x1e, 0xa6
    };

    byte resultT[sizeof(t1)];
    byte resultP[sizeof(p) + AES_BLOCK_SIZE];
    byte resultC[sizeof(p) + AES_BLOCK_SIZE];
    int  result = 0;
    int ret;

    (void) result;

    if (prnt) {
        printf(" sce_aes128_gcm_test() ");
    }

    XMEMSET(resultT, 0, sizeof(resultT));
    XMEMSET(resultC, 0, sizeof(resultC));
    XMEMSET(resultP, 0, sizeof(resultP));
    XMEMSET(&userContext, 0, sizeof(User_SCEPKCbInfo));

    if (wc_AesInit(enc, NULL, INVALID_DEVID) != 0) {
        ret = -1;
        goto out;
    }

    if (wc_AesInit(dec, NULL, INVALID_DEVID) != 0) {
        ret = -2;
        goto out;
    }

    wc_AesGcmSetKey(enc, k3, sizeof(k3));
    if (result != 0) {
        ret = -3;
        goto out;
    } else {
        userContext.sce_wrapped_key_aes128 = aes128_key;
        userContext.keyflgs_crypt.bits.aes128_installedkey_set = 1;
        enc->ctx.keySize = (word32)enc->keylen;
    }
    /* AES-GCM encrypt and decrypt both use AES encrypt internally */
    result = wc_AesGcmEncrypt(enc, resultC, p3, sizeof(p3), iv3, sizeof(iv3),
                                        resultT, sizeof(t3), a3, sizeof(a3));
    if (result != 0) {
        ret = -4;
        goto out;
    }
    

    result = wc_AesGcmDecrypt(enc, resultP, resultC, sizeof(c3),
                      iv3, sizeof(iv3), resultT, sizeof(t3), a3, sizeof(a3));
    if (result != 0) {
        ret = -5;
        goto out;
    }
    if (XMEMCMP(p3, resultP, sizeof(p3))) {
        ret = -6;
        goto out;
    }

    ret = 0;

  out:
    wc_AesFree(enc);
    wc_AesFree(dec);
    
    if (prnt) {
        RESULT_STR(ret)
    }
    
    return ret;
}

static void tskAes128_Gcm_Test(void *pvParam)
{
    int ret = 0;
    Info *p = (Info*)pvParam;
    
    while (exit_loop == 0) {
        ret = sce_aesgcm128_test(0, &p->aes_key);
        vTaskDelay(10/portTICK_PERIOD_MS);
        if (ret != 0) {
            printf(" result was not good(%d). sce_aesgcm128_test\n", ret);
            Aes128_Gcm_multTst_rslt = 1;
        }
    }

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}

#endif

#if !defined(NO_RSA)

/* testing rsa sign/verify w/ rsa 2048 bit key */
#define TEST_STRING     "Everyone gets Friday off."
#define TEST_STRING2    "Everyone gets Friday ofv."
#define TEST_STRING_SZ   25
#define RSA_TEST_BYTES   256 /* up to 2048-bit key */

static int sce_rsa_test(int prnt, int keySize)
{
    int ret = 0;
    
    RsaKey *key = (RsaKey *)XMALLOC(sizeof *key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    WC_RNG rng;
    const char inStr [] = TEST_STRING;
    const char inStr2[] = TEST_STRING2;
    const word32 inLen = (word32)TEST_STRING_SZ;
    const word32 outSz = RSA_TEST_BYTES;
    byte *in = NULL;
    byte *in2 = NULL;
    byte *out= NULL;
    byte *out2 = NULL;
    
    in = (byte*)XMALLOC(inLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    in2 = (byte*)XMALLOC(inLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    out= (byte*)XMALLOC(outSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    out2 = (byte*)XMALLOC(outSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (key == NULL || in == NULL || out == NULL ||
        in2 == NULL || out2 == NULL) {
        ret = -1;
        goto out;
    }
    
    XMEMSET(&rng, 0, sizeof(rng));
    XMEMSET(key, 0, sizeof *key);
    XMEMCPY(in, inStr, inLen);
    XMEMCPY(in2, inStr2, inLen);
    XMEMSET(out,  0, outSz);
    XMEMSET(out2, 0, outSz);
    
    ret = wc_InitRsaKey_ex(key, NULL, 7890/* fixed devid for TSIP/SCE*/);
    if (ret != 0) {
        goto out;
    }
    
    if ((ret = wc_InitRng(&rng)) != 0)
        goto out;

    if ((ret = wc_RsaSetRNG(key, &rng)) != 0)
        goto out;

    /* make rsa key by SCE */
    if ((ret = wc_MakeRsaKey(key, keySize, 65537, &rng)) != 0) {
        goto out;
    }
    
    ret = wc_RsaPublicEncrypt(in, inLen, out, outSz, key, &rng);
    if (ret < 0) {
        goto out;
    }

    ret = wc_RsaPrivateDecrypt(out, keySize/8, out2, outSz, key);
    if (ret < 0) {
        ret = -1;
        goto out;
    }
    if (XMEMCMP(in, out2, inLen) != 0) {
        ret = -2;
        goto out;
    }

    ret = 0;
out:
    if (key != NULL) {
        wc_FreeRsaKey(key);
        XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (in != NULL) {
        XFREE(in, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (in2 != NULL) {
        XFREE(in2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (out != NULL) {
        XFREE(out, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (out2 != NULL) {
        XFREE(out2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    
    return ret;
}

static int sce_rsa_SignVerify_test(int prnt, int keySize)
{
    int ret = 0;
    
    RsaKey *key = (RsaKey *)XMALLOC(sizeof *key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    WC_RNG rng;
    const char inStr [] = TEST_STRING;
    const char inStr2[] = TEST_STRING2;
    const word32 inLen = (word32)TEST_STRING_SZ;
    const word32 outSz = RSA_TEST_BYTES;

    byte *in = NULL;
    byte *in2 = NULL;
    byte *out= NULL;

    in = (byte*)XMALLOC(inLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    in2 = (byte*)XMALLOC(inLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    out= (byte*)XMALLOC(outSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    
    (void) prnt;

    if (key == NULL || in == NULL || out == NULL) {
        ret = -1;
        goto out;
    }
    
    XMEMSET(&rng, 0, sizeof(rng));
    XMEMSET(key, 0, sizeof *key);
    XMEMCPY(in, inStr, inLen);
    XMEMCPY(in2, inStr2, inLen);

    ret = wc_InitRsaKey_ex(key, NULL, 7890/* fixed devid for TSIP/SCE*/);
    if (ret != 0) {
        goto out;
    }
    
    if ((ret = wc_InitRng(&rng)) != 0)
        goto out;
    
    if ((ret = wc_RsaSetRNG(key, &rng)) != 0)
        goto out;

    /* make rsa key by SCE */
    if ((ret = wc_MakeRsaKey(key, keySize, 65537, &rng)) != 0) {
        goto out;
    }
    
    guser_PKCbInfo.keyflgs_crypt.bits.message_type = 0;
    ret = wc_RsaSSL_Sign(in, inLen, out, outSz, key, &rng);
    if (ret < 0) {
        goto out;
    }

    /* this should fail */
    ret = wc_RsaSSL_Verify(in2, inLen, out, keySize/8, key);
    if (ret != FSP_ERR_CRYPTO_SCE_AUTHENTICATION) {
        ret = -1;
        goto out;
    }
    /* this should succeed */
    ret = wc_RsaSSL_Verify(in, inLen, out, keySize/8, key);
    if (ret < 0) {
        ret = -1;
        goto out;
    }
    ret = 0;
  out:
    if (key != NULL) {
        wc_FreeRsaKey(key);
        XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (in != NULL) {
        XFREE(in, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (in2 != NULL) {
        XFREE(in2, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (out != NULL) {
        XFREE(out, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    
    return ret;
}
#endif

int sce_crypt_test()
{
    int ret = 0;
    fsp_err_t sce_error_code;

    /* Generate AES sce Key */
    sce_error_code = R_SCE_AES128_WrappedKeyGenerate(&g_user_aes128_key_index1);

    if (sce_error_code == FSP_SUCCESS)
        sce_error_code = R_SCE_AES128_WrappedKeyGenerate(
                                                &g_user_aes128_key_index2);

    if (sce_error_code == FSP_SUCCESS)
        sce_error_code = R_SCE_AES256_WrappedKeyGenerate(
                                                &g_user_aes256_key_index1);

    if (sce_error_code == FSP_SUCCESS)
        sce_error_code = R_SCE_AES256_WrappedKeyGenerate(
                                                &g_user_aes256_key_index2);
   
   if (sce_error_code == FSP_SUCCESS) {
       #if defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
        /* set up Crypt Call back */
       
        Clr_CallbackCtx(&guser_PKCbInfo);

        ret = wc_CryptoCb_CryptInitRenesasCmn(NULL, &guser_PKCbInfo);

        if ( ret > 0)
            ret = 0;

        if (ret == 0) {
            printf(" sce_rsa_test(512)(this will be done"
            " by SW because SCE doesn't support 512 bits key size.)");
            ret = sce_rsa_test(1, 512);
            RESULT_STR(ret)
        }
        
        if (ret == 0) {
            printf(" sce_rsa_test(1024)");
            ret = sce_rsa_test(1, 1024);
            RESULT_STR(ret)
        }
        
        if (ret == 0) {
            printf(" sce_rsa_SignVerify_test(512)(this will be done"
            " by SW because SCE doesn't support 512 bits key size.)");
            ret = sce_rsa_SignVerify_test(1, 512);
            RESULT_STR(ret)
        }
        
        if (ret == 0) {
            printf(" sce_rsa_SignVerify_test(1024)");
            ret = sce_rsa_SignVerify_test(1, 1024);
            RESULT_STR(ret)
        }
        
        Clr_CallbackCtx(&guser_PKCbInfo);
        
        if (ret == 0) {
            printf(" sce_rsa_test(2048)");
            ret = sce_rsa_test(1, 2048);
            RESULT_STR(ret)
        }
        
        if (ret == 0 && sce_error_code == FSP_SUCCESS) {
            printf(" sce_rsa_SignVerify_test(2048)");
            ret = sce_rsa_SignVerify_test(1, 2048);
            RESULT_STR(ret)
        }

       #endif /* WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY */

   #ifndef NO_SHA256
        printf(" sha256_test()");
        ret = sha256_test();
        RESULT_STR(ret)
   #endif

        ret = sce_aes_cbc_test(1, &g_user_aes128_key_index1);

        if (ret == 0) {

            ret = sce_aes256_test(1, &g_user_aes256_key_index1);

        }

        if (ret == 0) {

            ret = sce_aesgcm128_test(1, &g_user_aes128_key_index1);

        }

        if (ret == 0) {

            ret = sce_aesgcm256_test(1, &g_user_aes256_key_index1);

        }
    }
    else
        ret = -1;

    return ret;
}

#ifndef NO_SHA256

void tskSha256_Test1(void *pvParam)
{
    int ret = 0;
    (void)pvParam;

    while (exit_loop == 0) {
        ret = sha256_test();
        vTaskDelay(10/portTICK_PERIOD_MS);
        if (ret != 0) {
            printf(" result was not good(%d). sha_test\n", ret);
            sha256_multTst_rslt1 = 1;
        }
    }

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}

static void tskSha256_Test2(void *pvParam)
{
    int ret = 0;
    (void)pvParam;

    while (exit_loop == 0) {
        ret = sha256_test();
        vTaskDelay(10/portTICK_PERIOD_MS);
        if (ret != 0) {
            printf(" result was not good(%d). sha256_test\n", ret);
            sha256_multTst_rslt2 = 1;
        }
    }

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

int sce_crypt_sha256_multitest()
{
    int ret = 0;
    int num = 0;
    int i;
    BaseType_t xRet;
    
#ifndef NO_SHA256
    num+=2;
#endif
    exit_loop = 0;
    sha256_multTst_rslt1 = 0;
    sha256_multTst_rslt2 = 0;

    exit_semaph = xSemaphoreCreateCounting(num, 0);
    xRet = pdPASS;
    
#ifndef NO_SHA256
    xRet = xTaskCreate(tskSha256_Test1, "sha256_test1",
                                        STACK_SIZE, NULL, 2, NULL);
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskSha256_Test2, "sha256_test2",
                                        STACK_SIZE, NULL, 3, NULL);
#endif

    if (xRet == pdPASS) {
        printf(" Waiting for completing tasks ...");
        vTaskDelay(10000/portTICK_PERIOD_MS);
        exit_loop = true;

        for (i = 0; i < num; i++) {
            if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
                printf("exit semaphore not released by test task");
                ret = -1;
                xRet = pdFAIL;
            }
        }
    }
    
    vSemaphoreDelete(exit_semaph);
    
    if ((xRet == pdPASS) &&
       (sha256_multTst_rslt1 == 0 && sha256_multTst_rslt2 == 0))
        ret = 0;
    else
        ret = -1;
    
    RESULT_STR(ret)

    return ret;
}


int sce_crypt_AesCbc_multitest()
{
    int ret = 0;
    int num = 0;
    int i;
    Info info_aes1;
    Info info_aes2;
    Info info_aes256_1;
    Info info_aes256_2;
    BaseType_t xRet;
    
#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_128)
    num+=2;
#endif
#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_256)
    num+=2;
#endif
    exit_loop = 0;
    Aes128_Cbc_multTst_rslt = 0;
    Aes256_Cbc_multTst_rslt = 0;

    exit_semaph = xSemaphoreCreateCounting(num, 0);
    xRet = pdPASS;
    
#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_128)
    XMEMCPY(&info_aes1.aes_key, &g_user_aes128_key_index1,
                                            sizeof(sce_aes_wrapped_key_t));
    xRet = xTaskCreate(tskAes128_Cbc_Test, "aes_cbc_tes1t",
                                    STACK_SIZE, &info_aes1, 3, NULL);
#endif
#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_128)
    XMEMCPY(&info_aes2.aes_key, &g_user_aes128_key_index2,
                                            sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes128_Cbc_Test, "aes_cbc_test2",
                                    STACK_SIZE, &info_aes2, 3, NULL);
#endif

#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_256)
    XMEMCPY(&info_aes256_1.aes_key, &g_user_aes256_key_index1,
                                            sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes256_Cbc_Test, "aes256_cbc_test1",
                                    STACK_SIZE, &info_aes256_1, 3, NULL);
#endif
#if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_256)
    XMEMCPY(&info_aes256_2.aes_key, &g_user_aes256_key_index2,
                                            sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes256_Cbc_Test, "aes256_cbc_test2",
                                    STACK_SIZE, &info_aes256_2, 3, NULL);
#endif
    
    if (xRet == pdPASS) {
    printf(" Waiting for completing tasks ...   ");
        vTaskDelay(10000/portTICK_PERIOD_MS);
        exit_loop = 1;

        for (i = 0; i < num; i++) {
            if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
                printf("exit semaphore not released by test task");
                ret = -1;
                xRet = pdFAIL;
            }
        }
    }
    
    vSemaphoreDelete(exit_semaph);

    if ((xRet == pdPASS) &&
       (Aes128_Cbc_multTst_rslt == 0 && Aes256_Cbc_multTst_rslt == 0))
        ret = 0;
    else
        ret = -1;

    RESULT_STR(ret)

    return ret;
}


int sce_crypt_AesGcm_multitest()
{
    int ret = 0;
    int num = 0;
    int i;
    Info info_aes1;
    Info info_aes2;
    Info info_aes256_1;
    Info info_aes256_2;
    BaseType_t xRet;
    
#if defined(WOLFSSL_AES_128)
    num+=2;
#endif
#if defined(WOLFSSL_AES_256)
    num+=2;
#endif

    exit_loop = 0;
    Aes128_Gcm_multTst_rslt = 0;
    Aes256_Gcm_multTst_rslt = 0;

    exit_semaph = xSemaphoreCreateCounting(num, 0);
    xRet = pdPASS;
    
#if defined(WOLFSSL_AES_128)
    XMEMCPY(&info_aes1.aes_key, &g_user_aes128_key_index1,
                                    sizeof(sce_aes_wrapped_key_t));
    xTaskCreate(tskAes128_Gcm_Test, "aes128_gcm_test1",
                                    STACK_SIZE, &info_aes1, 3, NULL);
#endif

#if defined(WOLFSSL_AES_128)
    XMEMCPY(&info_aes2.aes_key, &g_user_aes128_key_index2,
                                        sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes128_Gcm_Test, "aes128_gcm_test2",
                                        STACK_SIZE, &info_aes2, 3, NULL);
#endif

#if defined(WOLFSSL_AES_256)
    XMEMCPY(&info_aes256_1.aes_key, &g_user_aes256_key_index1,
                                        sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes256_Gcm_Test, "aes256_gcm_test1",
                                    STACK_SIZE, &info_aes256_1, 3, NULL);
#endif

#if defined(WOLFSSL_AES_256)
    XMEMCPY(&info_aes256_2.aes_key, &g_user_aes256_key_index2,
                                        sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes256_Gcm_Test, "aes256_gcm_test2",
                                    STACK_SIZE, &info_aes256_2, 3, NULL);
#endif

    if (xRet == pdPASS) {
        printf(" Waiting for completing tasks ...   ");
        vTaskDelay(10000/portTICK_PERIOD_MS);
        exit_loop = 1;

        for (i = 0; i < num; i++) {
            if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
                printf("exit semaphore not released by test task");
                ret = -1;
                xRet = pdFAIL;
            }
        }
    }
    
    vSemaphoreDelete(exit_semaph);

    if ((xRet == pdPASS) &&
        (Aes128_Gcm_multTst_rslt == 0 && Aes256_Gcm_multTst_rslt == 0))
        ret = 0;
    else
        ret = -1;

    RESULT_STR(ret)

    return ret;
}

int sce_crypt_Sha_AesCbcGcm_multitest()
{
    int ret = 0;
    int num = 0;
    int i;
    Info info_aes128cbc;
    Info info_aes128gcm;
    Info info_aes256cbc;
    Info info_aes256gcm;
    BaseType_t xRet;
    
#ifndef NO_SHA256
    num+=2;
#endif

#if defined(WOLFSSL_AES_128)
    num+=2;
#endif
#if defined(WOLFSSL_AES_256)
    num+=2;
#endif

    exit_loop = 0;
    sha256_multTst_rslt1 = 0;
    sha256_multTst_rslt2 = 0;
    Aes128_Cbc_multTst_rslt = 0;
    Aes256_Cbc_multTst_rslt = 0;
    Aes128_Gcm_multTst_rslt = 0;
    Aes256_Gcm_multTst_rslt = 0;

    exit_semaph = xSemaphoreCreateCounting(num, 0);
    xRet = pdPASS;
    
#ifndef NO_SHA256
    xRet = xTaskCreate(tskSha256_Test1, "sha256_test1", 
                                            STACK_SIZE, NULL, 3, NULL);

    if (xRet == pdPASS)
        xRet = xTaskCreate(tskSha256_Test2, "sha256_test2", 
                                            STACK_SIZE, NULL, 3, NULL);
#endif

#if defined(WOLFSSL_AES_128)
    XMEMCPY(&info_aes128cbc.aes_key, &g_user_aes128_key_index1, 
                                                sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes128_Cbc_Test, "aes128_cbc_test1", 
                                    STACK_SIZE, &info_aes128cbc, 3, NULL);
#endif

#if defined(WOLFSSL_AES_128)
    XMEMCPY(&info_aes128gcm.aes_key, &g_user_aes128_key_index2, 
                                                sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes128_Gcm_Test, "aes128_gcm_test2", 
                                    STACK_SIZE, &info_aes128gcm, 3, NULL);
#endif

#if defined(WOLFSSL_AES_256)
    XMEMCPY(&info_aes256cbc.aes_key, &g_user_aes256_key_index1, 
                                                sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes256_Cbc_Test, "aes256_cbc_test1", 
                                    STACK_SIZE, &info_aes256cbc, 3, NULL);
#endif

#if defined(WOLFSSL_AES_256)
    XMEMCPY(&info_aes256gcm.aes_key, &g_user_aes256_key_index2, 
                                                sizeof(sce_aes_wrapped_key_t));
    if (xRet == pdPASS)
        xRet = xTaskCreate(tskAes256_Gcm_Test, "aes256_gcm_test2", 
                                    STACK_SIZE, &info_aes256gcm, 3, NULL);
#endif

    if (xRet == pdPASS) {
        printf(" Waiting for completing tasks ...   ");
        vTaskDelay(10000/portTICK_PERIOD_MS);
        exit_loop = 1;

        for (i = 0; i < num; i++) {
            if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
                printf("exit semaphore not released by test task");
                ret = -1;
                xRet = pdFAIL;
            }
        }
    }
    
    vSemaphoreDelete(exit_semaph);
    
    if ((xRet == pdPASS) && 
        (Aes128_Gcm_multTst_rslt == 0 && Aes256_Gcm_multTst_rslt == 0) &&
        (sha256_multTst_rslt1 == 0 && sha256_multTst_rslt2 == 0)) {
        
        ret = 0;
    }
    else {
        ret = -1;
    }
    
    RESULT_STR(ret)

    return ret;
}

