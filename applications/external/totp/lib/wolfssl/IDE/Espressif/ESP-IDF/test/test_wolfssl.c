/*
* wolfssl unit tests
*/

#include <stdio.h>
#include <string.h>

#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_timer.h"

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/wolfmath.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
static const char* const TAG = "wolfssl unit test";
static xSemaphoreHandle exit_semaph;
static volatile bool exit_loop=false;
static uint64_t startTime = 0;

#define SHA_STACK_SIZE (20*1024)
#define TIMES_SHA 500
#define TIMES_AES 100

#ifndef NO_SHA
int sha_test();
#endif
#ifndef NO_SHA256
int sha256_test();
#endif
#ifdef WOLFSSL_SHA384
int sha384_test(void);
#endif
#ifdef WOLFSSL_SHA512
int sha512_test(void);
#endif

static uint64_t rsa_elapsedTime = 0;
static void esp32TimerStart()
{
    startTime = esp_timer_get_time();
}

static uint64_t  esp32elapsedTime()
{
    /* return elapsed time since wc_esp32AesTimeStart() is called in us */
    return esp_timer_get_time() - startTime;
}

#ifndef NO_RSA
int  rsa_test(void);
static void tskRsa_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskRsa_Test");

    while(exit_loop==false) {
        if(rsa_elapsedTime == 0) esp32TimerStart();
        ret = rsa_test();
        if(rsa_elapsedTime == 0) rsa_elapsedTime = esp32elapsedTime()/1000;
        if(ret != 0) {
            printf("result was not good(rsa_test)(%d)\n",ret);
            TEST_FAIL_MESSAGE("tskRsa_Test\n");
        }
    }

   ESP_LOGI(TAG, "leave tskRsa_Test");

   xSemaphoreGive(exit_semaph);
   vTaskDelete(NULL);
}
#endif

#ifndef NO_AES
int aes_test(void);
static void tskAes_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskAes_Test");

    while(exit_loop==false) {
        ret = aes_test();
        if(ret != 0) {
            printf("result was not good(aes_test)(%d)\n",ret);
            TEST_FAIL_MESSAGE("tskAes_Test\n");
        }
    }

   ESP_LOGI(TAG, "leave tskAes_Test");

   xSemaphoreGive(exit_semaph);
   vTaskDelete(NULL);
}

int aesgcm_test(void);

static void tskAesGcm_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskAesGcm_Test");

    while(exit_loop==false) {
        ret = aesgcm_test();
        if(ret != 0) {
            printf(" results was not good(%d). aesGcm_test\n",ret);
            TEST_FAIL_MESSAGE("aesGcm_test\n");
        }
    }
    ESP_LOGI(TAG, "leave tskAesGcm_Test");
    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}

#ifdef WOLFSSL_AES_192
int aes192_test(void);
static void tskAes192_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskAes192_Test");

    while(exit_loop==false) {
        ret = aes192_test();
        if(ret != 0) {
            printf(" results was not good(%d). aes192_test\n",ret);
            TEST_FAIL_MESSAGE("aes192_test\n");
        }
    }
    ESP_LOGI(TAG, "leave tskAes192_Test");
    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif
#ifdef WOLFSSL_AES_256
int aes256_test(void);
static void tskAes256_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskAes256_Test");

    while(exit_loop==false) {
        ret = aes256_test();
        if(ret != 0) {
            printf(" results was not good(%d). aes256_test\n", ret);
            TEST_FAIL_MESSAGE("aes256_test\n");
        }
    }
    ESP_LOGI(TAG, "leave tskAes256_Test");
    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

#if (!defined(NO_RSA) || defined(HAVE_ECC)) && !defined(WC_NO_RNG)

int mp_performance_check(int mul, int mulmod, int exptmod)
{
    int ret = 0;
    int i,j,k;
    word32 Xbits;
    word32 Ybits;
    word32 Pbits;
    byte strX[256];
    byte strY[256];
    byte strP[256];
    uint64_t elapsedTime1;
    uint64_t elapsedTime2;
    uint64_t elapsedTime3;
    RNG rng;
    mp_int z1, z2, z3, x, p, y;

    /* sanity check */
    if(!(mul||mulmod||exptmod)) return ret;

    XMEMSET(strX, 0, sizeof(strX));
    XMEMSET(strY, 0, sizeof(strY));
    XMEMSET(strP, 0, sizeof(strP));

    wc_InitRng(&rng);

    ret = wc_RNG_GenerateBlock(&rng, strX, sizeof(strX));
    if (ret != 0) {
        return -1; //generating block failed!
    }
    ret = wc_RNG_GenerateBlock(&rng, strY, sizeof(strY));
    if (ret != 0) {
        return -1; //generating block failed!
    }
    ret = wc_RNG_GenerateBlock(&rng, strP, sizeof(strP));
    if (ret != 0) {
        return -1; //generating block failed!
    }

    if((strP[255]&1) == 0) {
        strP[255] |= 1;
    }

    for(i = 0; i < 256; i+=16) {
        for(j = 0; j < 256; j+=16) {
            for(k = 0; k < 256; k+=16) {
            /* set init mp_int */
            if((ret = mp_init_multi(&z1, &x, &y, &p, &z2, NULL)) != MP_OKAY) {
                TEST_FAIL_MESSAGE("mp_init failed.\n");
            }

            /* set big-numbers into mp_inits */
            if(ret == 0 && mp_read_unsigned_bin(&x, (const byte*)&strX[i+1], i+1) != MP_OKAY) {
                ESP_LOGE(TAG,"failed to set strX into mp_init");
                return -1;
            }
            if(ret == 0 && mp_read_unsigned_bin(&y, (const byte*)&strY[j+1], j+1) != MP_OKAY) {
                ESP_LOGE(TAG,"failed to set strY into mp_init");
                return -1;
            }
            if(ret == 0 && mp_read_unsigned_bin(&p, (const byte*)&strP[k+1], k+1) != MP_OKAY) {
                ESP_LOGE(TAG,"failed to set strY into mp_init");
                return -1;
            }
                Xbits = mp_count_bits(&x);
                Ybits = mp_count_bits(&y);
                Pbits = mp_count_bits(&p);

                if(mul) {
                    /* Z = X * Y */
                    esp32TimerStart();
                    if(ret == 0 && (ret = mp_mul(&x, &y, &z1)) != MP_OKAY) {
                        printf("(%d,%d) Xbits = %d, Ybits = %d Pbits = %d ret = %d\n",
                                                    i, j, Xbits, Ybits, Pbits, ret);
                        TEST_FAIL_MESSAGE("mp_mul() failed");
                    }
                    elapsedTime1 = esp32elapsedTime();
                }
                if(mulmod) {
                    /* Z = X^Y mod M */
                    esp32TimerStart();
                    if(ret == 0 && (ret = mp_mulmod(&x, &y, &p, &z2)) != MP_OKAY) {
                        printf("(%d,%d) Xbits = %d, Ybits = %d Pbits = %d ret = %d\n",
                                                    i , j, Xbits, Ybits, Pbits, ret);
                        printf("X = %s Y = %s P = %s\n", mp_isodd(&x)==MP_YES?"ODD":"EVEN",
                                                        mp_isodd(&y)==MP_YES?"ODD":"EVEN",
                                                        mp_isodd(&p)==MP_YES?"ODD":"EVEN");
                        TEST_FAIL_MESSAGE("mp_mulmod() failed");
                    }
                    elapsedTime2 = esp32elapsedTime();
                }
                if(exptmod) {
                    /* Z = X^Y mod M */
                    esp32TimerStart();
                    if(ret == 0 && (ret = mp_exptmod(&x, &y, &p, &z3)) != MP_OKAY) {
                        printf("(%d,%d) Xbits = %d, Ybits = %d Pbits = %d ret = %d\n",
                                                    i , j, Xbits, Ybits, Pbits, ret);
                        printf("X = %s Y = %s P = %s\n", mp_isodd(&x)==MP_YES?"ODD":"EVEN",
                                                            mp_isodd(&y)==MP_YES?"ODD":"EVEN",
                                                            mp_isodd(&p)==MP_YES?"ODD":"EVEN");
                        TEST_FAIL_MESSAGE("mp_exptmod() failed");
                    }
                    elapsedTime3 = esp32elapsedTime()/1000;
                }

                printf("(%d,%d) Xbits = %d, Ybits = %d Pbits = %d",
                                                        i , j, Xbits, Ybits, Pbits);
                if(mul) {
                    printf(" mul = %llu (us)", (unsigned long long)elapsedTime1);
                }
                if(mulmod) {
                    printf(" mulmod = %llu (us)\n", (unsigned long long)elapsedTime2);
                }
                if(exptmod) {
                    printf(" exptmod = %llu (ms)\n", (unsigned long long)elapsedTime3);
                }
            }
        }
    }

    mp_clear(&z1);
    mp_clear(&z2);
    mp_clear(&z3);
    mp_clear(&x);
    mp_clear(&p);
    mp_clear(&y);

    wc_FreeRng(&rng);

    return ret;
}

/* Z = X * Y */
int mp_unitest_mul(const char* strZ, const char* strX, const char* strY, int verbose)
{
    int ret = 0;
    char* buf = NULL;
    char* bufZ = NULL;
    int radixX_size;
    int radixZ_size;
    int radixY_size;

    mp_int z, x, y;

     /* set init mp_int */
    if((ret = mp_init_multi(&z, &x, &y, NULL, NULL, NULL)) != MP_OKAY) {
        TEST_FAIL_MESSAGE("mp_init failed.\n");
    }
    /* set big-numbers into mp_inits */
    if(ret == 0 && mp_read_radix(&x, strX, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strX into mp_init");
        return -1;
    }
    if(ret == 0 && mp_read_radix(&y, strY, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strY into mp_init");
        return -1;
    }
    /* Z = X * Y */
    if(ret == 0 && (ret = mp_mul(&x, &y, &z)) != MP_OKAY) {
        TEST_FAIL_MESSAGE("mp_mul() failed\n");
    }

    mp_radix_size(&z, 16, &radixZ_size);
    bufZ = (char*)XMALLOC(radixZ_size, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if(bufZ != NULL) {
        mp_toradix(&z, bufZ, 16);
        bufZ[radixZ_size-1] ='\0';
    }

    if(verbose) {
        mp_dump("x_value", &x, 1);
        mp_dump("y_value", &y, 1);
        mp_dump("z_value", &z, 1);

        mp_radix_size(&x, 16, &radixX_size);
        mp_radix_size(&y, 16, &radixY_size);
        radixX_size = max(radixX_size, radixY_size);

        buf = (char*)XMALLOC(radixX_size, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if(buf != NULL) {
            mp_toradix(&x, buf, 16);
            buf[radixX_size-1] ='\0';
            printf("X : %s ", buf);

            mp_toradix(&y, buf, 16);
            buf[radixY_size-1] ='\0';
            printf("Y : %s ", buf);
        }
        if(bufZ != NULL) {
            printf("Z : %s \n", bufZ);
        }
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    TEST_ASSERT_EQUAL_STRING_MESSAGE(strZ, bufZ,
                                   "mp_mulmod returns an unexpected result.");
    XFREE(bufZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return 0;

}

int mp_unitest_mulmod(const char* strZ, const char* strX, const char* strY,
               const char* strM, int verbose)
{
    int ret = 0;
    char* buf;
    char* bufZ;
    int radixX_size;
    int radixZ_size;
    int radixY_size;
    int radixM_size;

    mp_int z, x, y, m;

     /* set init mp_int */
    if((ret = mp_init_multi(&z, &x, &y, &m, NULL, NULL)) != MP_OKAY) {
        TEST_FAIL_MESSAGE("mp_init failed.\n");
    }
    /* set big-numbers into mp_inits */
    if(ret == 0 && mp_read_radix(&x, strX, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strX into mp_init");
        return -1;
    }
    if(ret == 0 && mp_read_radix(&y, strY, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strY into mp_init");
        return -1;
    }
    if(ret == 0 && mp_read_radix(&m, strM, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strM into mp_init");
        return -1;
    }
    /* Z = X^Y mod M */
    if(ret == 0 && (ret = mp_mulmod(&x, &y, &m, &z)) != MP_OKAY) {
        TEST_FAIL_MESSAGE("mp_mulmod() failed\n");
    }

    mp_radix_size(&z, 16, &radixZ_size);
    bufZ = (char*)XMALLOC(radixZ_size, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if(bufZ != NULL) {
        mp_toradix(&z, bufZ, 16);
        bufZ[radixZ_size-1] ='\0';
    }

    if(verbose) {
        mp_dump("x_value", &x, 1);
        mp_dump("y_value", &y, 1);
        mp_dump("m_value", &m, 1);
        mp_dump("z_value", &z, 1);

        mp_radix_size(&x, 16, &radixX_size);
        mp_radix_size(&y, 16, &radixY_size);
        mp_radix_size(&m, 16, &radixM_size);
        radixX_size = max(radixX_size, max(radixY_size, radixM_size));

        buf = (char*)XMALLOC(radixX_size, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if(buf != NULL) {
            mp_toradix(&x, buf, 16);
            buf[radixX_size-1] ='\0';
            printf("X : %s ", buf);

            mp_toradix(&y, buf, 16);
            buf[radixY_size-1] ='\0';
            printf("Y : %s ", buf);

            mp_toradix(&m, buf, 16);
            buf[radixM_size-1] ='\0';
            printf("M : %s ", buf);
        }
        if(bufZ != NULL) {
            printf("Z : %s \n", bufZ);
        }
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    TEST_ASSERT_EQUAL_STRING_MESSAGE(strZ, bufZ,
                                   "mp_mulmod returns an unexpected result.");
    XFREE(bufZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return 0;

}

int mp_unitest_exptmod(const char* strZ, const char* strX, const char* strY,
               const char* strM, int verbose)
{
    int ret = 0;
    char* buf = NULL;
    char* bufZ = NULL;
    int radixX_size;
    int radixZ_size;
    int radixY_size;
    int radixM_size;

    mp_int z, x, y, m;

     /* set init mp_int */
    if((ret = mp_init_multi(&z, &x, &y, &m, NULL, NULL)) != MP_OKAY) {
        TEST_FAIL_MESSAGE("mp_init failed.\n");
    }
    /* set big-numbers into mp_inits */
    if(ret == 0 && mp_read_radix(&x, strX, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strX into mp_init");
        return -1;
    }
    if(ret == 0 && mp_read_radix(&y, strY, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strY into mp_init");
        return -1;
    }
    if(ret == 0 && mp_read_radix(&m, strM, 16) != MP_OKAY) {
        ESP_LOGE(TAG,"failed to set strM into mp_init");
        return -1;
    }
    /* Z = X^Y mod M */
    if(ret == 0 && (ret = mp_exptmod(&x, &y, &m, &z)) != MP_OKAY) {
        TEST_FAIL_MESSAGE("mp_exptmod() failed\n");
    }

    mp_radix_size(&z, 16, &radixZ_size);
    bufZ = (char*)XMALLOC(radixZ_size, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if(bufZ != NULL) {
        mp_toradix(&z, bufZ, 16);
        bufZ[radixZ_size-1] ='\0';
    }

    if(verbose) {
        mp_dump("x_value", &x, 1);
        mp_dump("y_value", &y, 1);
        mp_dump("m_value", &m, 1);
        mp_dump("z_value", &z, 1);

        mp_radix_size(&x, 16, &radixX_size);
        mp_radix_size(&y, 16, &radixY_size);
        mp_radix_size(&m, 16, &radixM_size);
        radixX_size = max(radixX_size, max(radixY_size, radixM_size));

        buf = (char*)XMALLOC(radixX_size, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if(buf != NULL) {
            mp_toradix(&x, buf, 16);
            buf[radixX_size-1] ='\0';
            printf("X : %s ", buf);

            mp_toradix(&y, buf, 16);
            buf[radixY_size-1] ='\0';
            printf("Y : %s ", buf);

            mp_toradix(&m, buf, 16);
            buf[radixM_size-1] ='\0';
            printf("M : %s ", buf);
        }
        if(bufZ != NULL) {
            printf("Z : %s \n", bufZ);
        }
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    TEST_ASSERT_EQUAL_STRING_MESSAGE(strZ, bufZ,
                                   "mp_exptmod returns an unexpected result.");
    XFREE(bufZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return 0;

}

TEST_CASE("wolfssl mp exptmod test"   , "[wolfssl]")
{
    ESP_LOGI(TAG, "mp test");
    int verbose = 1;

    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("02", "5", "1", "3", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("01", "-5", "1", "3", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("CE331E6D30A77A57", "1234", "A",
                           "FFFFFFFFFFFFFFFF", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("01000000", "1000", "2", "FFFFFFF", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("01000000", "2", "128",
                            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("014B5A90", "1234", "2", "FFFFFFF", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("01234321", "1111", "2", "FFFFFFFF", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("02", "5", "1", "3", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("22", "55", "1", "33", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("0222", "555", "1", "333", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("2222", "5555", "1", "3333", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("11", "5555", "1", "33", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("55", "1111", "1", "77", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_exptmod("88", "1111", "2", "BB", verbose));
}

TEST_CASE("wolfssl mp mulmod test"   , "[wolfssl]")
{
    ESP_LOGI(TAG, "mp test");
    int verbose = 1;
    /*                                      Z    X    Y    M */
    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod("02", "5", "1", "3", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod("01", "-5", "1", "3", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod("02", "-64", "A", "3", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod("74C3AC", "123456", "55555", "AAAAA1", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod("73A068", "123456", "55555", "AAAAA3", verbose));

    mp_unitest_mulmod(
    "010C530243ADE5EA7C557E9A2FF5B4573195665A89CB921F573267B15CD2BCB6467E925235AA752CC2D08B07D31497B497744CA3685A46E76247439826628589DD814AC9EEE9EF8B4B44BEE2DB6065BE3C51B788E4ECFF39FB28C3D8EBE10FC9989D97CDC6624E32EBD222E222A2E93085FC2D05E4EB73375F7FC7B11E9B3024",
    "A4F780E83C3FAC34878787D4876BA7582E48C7637A26C6E720974FC7416150A3865D44F6D08E3DA38EB4296928C564D9A0008D8A0D63E0B8EF54D14D54FBEAB540E43D2ED6BE54806D9150C1944437CC3D8B2486A1FB932A6691B529E0E2A46524CB0825BA4F4E1B9C24554DB1913169E5373173A3B7CBBF77C3403C8C7AE86A",
    "6520379E44C1A2C359342010E1038F8C3644D9A47A9346A80C92B48A6986872D74C3BDDB49B2D93C554B588D4A4448614FADBC066CC10F3EB20A2422EA857B7DD0BF60C9CB7D733B12761BD785BCD122D97ECA0A8F1D0F705BC094B66EE5C96712AE3B14B5AA6AD9E50C6A3020BA01DA4FB94E3934527ADCDB3DE51C368B37C2",
    "BE7070B80418E528FE66D89088E0F1B7C3D0D23EE64B9474B0FFB0F763A5AB7EAFB62BB738161A50BFF1CA873AD5B0DAF8437A15B97EEA2A80D251B035AF07F3F25D243A4B8756481B3C249ADA7080BD3C8B034A0C8371DEE30370A2B760091B5EC73DA06460E3A9068DD3FF42BB0A94272D57420DB02DE0BA182560921192F3",
    verbose);

    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod(
    "76DBEC4349F316F82C27F59CD87378C83234E28C6E99990A5F9CDCF5A627CD1B2F405FA31AA0EAF7CB433B01D3592E5EF11134E4F8C8126D4DF33F4022A9A2A5B2FA1E6D25B66C628A9BE974136C823CE564E39D1ED099B5B401B8805DE19EFA776C6263265D6E9928714EBD74FFFFD89179BD594DCDC74F5687E311C26B613E539311DC310D36D04D831704524820DE3FC70BC2E43B929F9B4FF40DF4367B241095577C80B6BEB7F2BDE84282EEC9F9FE2A289E4115DB788803392C96A8A86A2FCABCBC6545BABB80F65CE2821C07DA899CEA8D172681F398341FC449D21630DDD0F100E28D351CFF2F2C4E2341B5851632FC6597E00149E98752A5895CDC41",
    "1678D8E98C4A0BA91E0F8318A15A453A562B36A4EC315B445617CF93CD5287B0284FEE83EF0A5DBEFCEC1801E402D860C050CE24F74D715B57A9E0BE2D8F91FF61F7124983270F06A2E6A00E33A32E26A1A358B1B178C969D37BCB544ED8B3A0AF3BFEA5AB970AE7613ACF0B5C7B5B4AB578C96911752528DA7B3B40189E75C67EC3F6CEA16D5B53C5742FF3C425EB7C3F20D5198CC5B8CCAD0A2D59AD58DD88FB203396341456CF9415CEB3BFB1FB7B2CFE9599E755AB8FD55C5A8B9AD044E6E2DE44FC996297C3C900F76D118572EDF860F507B3C1AC3590B9B47AD85F9D930CEDE1DA1F0AE0DCF69D6DE44304FE22704C51262B9230D00A988438555F7EAF",
    "3D76E6173983A5E1F98C04CDD512274E7259E7C26355A5BA7F067B343699E01D6FDE6A27E3D73B1BBFE2F32FC4B412F8D16F1F538D09227BEECDC9BFF1D05DCF0D28112209D3077C2F9E88BBE62AA7E61948172142B793E6AA65D11AFFF182F0D758609B113C45EF43934233767F0C2337853750892E4E45413C87D02D051DC4BAD0782724672E9C8AC888A2EFDDC5C3AC24B0BD846F473070C815A784B4E6C1E16F2F4428379E80AC9226CDE94C5141B316EA3573E64487EB460E80E888FF270A9AB06CD17447FE082C3F7B2809EAA20227E9AE1924978793947E67CB664D938E061F1CE4B1F36795ED7166022D8B35F3C671ECB77EA7B0AA481E1E0D575887",
    "E98A5D15A4D434B959A2DAAF74C8C9032638FA48FC4D306EEA7689CE4FF687DE323A466E38125837220D80AC2DAF2F123E627360666890B26F4717042BCAB726B710C213F97A620A933290420D162EFAD729D79F54E4FC6574F8F6436B4E9E347FCB6B1C1ADE8281BF085D3FC0B6B1A8A59C8170A74E3287151C780EF018FEEB4B372BE9E1F7FA51C658B9D80603EDC00318558B98FEB1F6D03DFA63C03819C700EF4D9960B4BACEE3CED96B2D7694FFFB77184AFE65F00A915C3B229485D02018592EA53303AC1B5F78321125EE7F9621A9D676978D667EB291D0362EA31DBFF185EDC03E60B85A9FAB80E0EA5D5F7556C74D518ED41F34A636F1301F51992F",
    verbose));

    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod(
    "DBEE64C8091CF78EC926AF5D51E06FD92FE0007DA3AF33317D9E131F3CDE711AFB30F8F61E6ACF99EAD21822E4DBC2B31B910D7E357230F255DA6C90EEA36A1E926A7F44CF34FBE544C1DC30B8DF4D5735495FEB1D0E10827222B6AF069BCD3F66120C4A19D8D533E4222FE65FCB323CF993A8DB07B896F0D10AAF94A996EB9B",
    "1572A17625B1609DB02793AEC41541DB2C9B0301EEF6805804945BA77656B207BF46F90762DEBB3E0057A43A95D09C6C852267E602FD3851D925323F875CB1E743818CBE17814D2DB4921BAA7DA2F8E5EFBD6373FF0E2020206BC89870F0B734AD3E331EB010CC713F09683165EC5BF551B3B70A497178EB926C0726EDDB2E37",
    "773FB6148DC513087EC9C4EAD4BA0DA49EB36EDE1A7AF88988EF363C11BC83E8306C817C47F34DCAEA560162552E4B89A9BD6F01F67402AAE38466069534A1E2CA65FEA32D4397956C6FD5B438F6F99530FAF89C252BB61451CC2EB35BD6DC1AEC2D095B3F3AD0B84E271FDC2AEEACA9595D076311830BD47480B67D6245BF56",
    "FF3011C23C6BB4D69E6BC193D148CE802DBEAFF7BAB2D7C3C4536E1502AA61B9EA059B79670BCED9FB988C1D6BF45AA7A05E5418E931447CC752D86DA03ED6142D7B159D1E398796DDA833552A8E32C0C4E5B8CBCD328DAD7BE5C67E4D6FF3A4C5A640BE903A336A24B2808112ACE37B2663CF88B9FF74233752F0C4275D451F",
    verbose));

    TEST_ASSERT_EQUAL(0, mp_unitest_mulmod(
        "7C65",
        "10000000000",
        "555555",
        "12345", verbose));

}

TEST_CASE("wolfssl mp mul test"   , "[wolfssl]")
{
    ESP_LOGI(TAG, "mp test");
    int verbose = 1;

    TEST_ASSERT_EQUAL(0, mp_unitest_mul("0A", "5", "2", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mul("-0A", "-5", "2", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mul("0A", "-5", "-2", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mul("06260060", "1234", "5678", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mul("038E83", "123", "321", verbose));
    TEST_ASSERT_EQUAL(0, mp_unitest_mul("75CD7FCBBC", "123456", "6789A", verbose));

    TEST_ASSERT_EQUAL(0, mp_unitest_mul(
    "19C26D7DC91D2123426ECCDB5A0290A9F7D9D01456D266BA549E859373D9D4FE0B4EDAE8763FA94458E4BED29CB7DD6CEF5C52136D2F310D2EABA373C750760EEE3FAE6EA618C60E9A4A0F2C3F3AC87612A0F032866F1CA4DA95CCCC47D2C51F",
    "77BDC7B4C5616B142B7762251075BA95FB475D570179E730D418A9143D18905F",
    "371280CB239B46A1BB0C036755ADF3C4474E1696FB2E70AA200225322C50270D44AE8490F24AA22D0FD9AE89EEF9449285FCED5FBC8F424A4A5E82DFEB3B0341",
    verbose));

    TEST_ASSERT_EQUAL(0, mp_unitest_mul(
    "033676FE7B625BF0759F7E8932B6B50D5F45E16E1C670AD20F1CDA5DFFA433685937CA8422A9CB916CC8",
    "165196BA298CD54975DC483C4D21A51EA0A146783CFB41522E76E50C",
    "24D9D5CA7D9CCC06F5E70F1963E6",
    verbose));

    TEST_ASSERT_EQUAL(0, mp_unitest_mul(
        "65D67946D0D9AB634DD18E5308A1F963C3AAE65F949A81C16C91B5B9DACE18A170ED73E3FC1F01EC8FF7D78EDD29B71FA00D9F5D8FF9116AA93E6F8F09DB306A8C1EEA42DCE9C8C9345B4B84E58CEED1F24A93B14FF47E81A766DF842A25BB941F3F07310C749B47E27C85D544FFE9021AB31AD3484AF40F738A04E9BCC0EFB6B36E56ABE76D982AD7B4D2527526F29B9CFD2A0F6570532C286EA7937354F1A6903480B4D8554E7ED707A08E07CAD5EB713752125163626CF52178574011072D2E536D126C2FF096505515FCA91B5BCEC2D765FC0A1AE90CC8F3C2E18529CF6647E4F732B1D16685189FE24148C752B3AF23F1D8481507CFB89DC1DC70E87B06748D4F1E32B489E1288FC5B02466C8395A59E92862F83F985D021B9C4C42B768BFD3FC81C189F7B88755EB879CF781EFC97C41D0ABB428F7351FA9D62A12A316FE2D2C24D8ECF87132D8476A1B0611793E9EFEE2ADDE1AB7116F69D7744150C770555C54B6BE88ECDD5CF33DF321147061FCB42BEF3D69663695FFD74D42493A",
    "13DADD9D122C5264497863C0BC2A893CD6C951E94198ABC0DAFF499BE89C6A425548B15F582E714D866563E4ACC81197D5B95BF87BD960D3647E51764F31977BFE2D0A7A7727C27BF7B3134148F126B5F1F0F00730C172C76D768874006C6C86E82F8698621773ED0B91CCA997FBC712D788D384E7831F51A6DD4FB4819D26026",
    "5210C04ED4FDF80D630BA81DCA6BA05495156D7DDBE1DEE0E057825F7FADACE70EBE4D5F82E51E465514DDC86E5B46DD780C182865A6C9E29F7456E3090229247BFC18094E31B15B1BE3A962601202D5C65979FF96CA64FB93F29110A76500DEB1003B6CEA2BA79CBAD007FA35610C43DE59FD0A22E07CA0A9943F6B101B4C9663AF0CADD26014586933AC536A917F4A62A31BD5EE8BFD452F63E4ED4D656DDC22D2413B44D02029887F7DC31200FD793C83F1927F306D00535EB1E0E8BEDA6486A41C509E8DB4369BC4D0C36767AA0127EC3E8C6EF4B5C07F2AD9FB51910B771F112A10F21128E26428B74F69586C486B04EEB64E84A36D4BD03C315B90CE8F",
    verbose));
}

TEST_CASE("wolfssl mp mul performance test"   , "[wolfssl]")
{
    int mul = 0, mulmod = 0, exptmod = 0;

    TEST_ASSERT_EQUAL(0, mp_performance_check(mul, mulmod, exptmod));
}
#endif/* (!NO_RSA || HAVE_ECC) && !WC_NO_RNG */

TEST_CASE("wolfssl aes test"  , "[wolfssl]")
{
    ESP_LOGI(TAG, "aes test");
    TEST_ASSERT_EQUAL(0, aes_test());
#ifdef WOLFSSL_AES_192
    ESP_LOGI(TAG, "aes_192 test");
    TEST_ASSERT_EQUAL(0, aes192_test());
#endif
#ifdef WOLFSSL_AES_256
    ESP_LOGI(TAG, "aes_256 test");
    TEST_ASSERT_EQUAL(0, aes256_test());
#endif
    ESP_LOGI(TAG, "aes-gcm test");
    TEST_ASSERT_EQUAL(0, aesgcm_test());
}

#endif

TEST_CASE("wolfssl sha crypt-test", "[wolfssl]")
{
#ifndef NO_SHA
    ESP_LOGI(TAG, "sha_test()");
    TEST_ASSERT_EQUAL(0, sha_test());
#endif
#ifndef NO_SHA256
    ESP_LOGI(TAG, "sha256_test()");
    TEST_ASSERT_EQUAL(0, sha256_test());
#endif
#ifdef WOLFSSL_SHA384
    ESP_LOGI(TAG, "sha384_test()");
    TEST_ASSERT_EQUAL(0, sha384_test());
#endif
#ifdef WOLFSSL_SHA512
    ESP_LOGI(TAG, "sha512_test()");
    TEST_ASSERT_EQUAL(0, sha512_test());
#endif
}


#ifndef NO_SHA
static void tskSha_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskSha_Test");

    while(exit_loop==false) {
        ret = sha_test();
        if(ret != 0) {
            printf(" results was not good(%d). sha_test\n", ret);
            TEST_FAIL_MESSAGE("tskSha_Test\n");
        }
    }

    ESP_LOGI(TAG, "leave tskSha_Test");

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

#ifndef NO_SHA256
static void tskSha256_Test(void *pvParam)
{
    int ret;

    ESP_LOGI(TAG, "enter tskSha256_Test");

    while(exit_loop==false) {
        ret = sha256_test();
        if(ret != 0) {
            printf("results was not good(%d). sha256_test\n", ret);
            TEST_FAIL_MESSAGE("sha256_test() failed");
        }
    }

    ESP_LOGI(TAG, "leave tskSha256_Test");

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

#ifdef WOLFSSL_SHA384
static void tskSha384_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskSha384_Test");

    while(exit_loop==false) {
        ret = sha384_test();
        if(ret != 0) {
            printf("results was not good(%d). sha384_test\n", ret);
            TEST_FAIL_MESSAGE("sha384_test() failed\n");
        }
    }

    ESP_LOGI(TAG, "leave tskSha384_Test");

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

#ifdef WOLFSSL_SHA512
static void tskSha512_Test(void *pvParam)
{
    int ret = 0;

    ESP_LOGI(TAG, "enter tskSha512_Test");

    while(exit_loop==false) {
        ret = sha512_test();
        if(ret != 0) {
            printf(" results was not good(%d). sha512_test\n", ret);
            TEST_FAIL_MESSAGE("tskSha512_Test() failed\n");
        }
    }

    ESP_LOGI(TAG, "leave tskSha512_test()");

    xSemaphoreGive(exit_semaph);
    vTaskDelete(NULL);
}
#endif

TEST_CASE("wolfssl sha multi-thread test ", "[wolfssl]")
{
    int num = 0;
    int i;
#ifndef NO_SHA
    num++;
#endif
#ifndef NO_SHA256
    num++;
#endif
#ifdef WOLFSSL_SHA384
    num++;
#endif
#ifdef WOLFSSL_SHA512
    num++;
#endif

    exit_loop = false;

    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_SHA
    xTaskCreate(tskSha_Test, "sha_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_SHA256
    xTaskCreate(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_SHA384
    xTaskCreate(tskSha384_Test, "sha384_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_SHA512
    xTaskCreate(tskSha512_Test, "sha512_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000/portTICK_PERIOD_MS);
    exit_loop = true;

    for(i = 0;i < num;i++) {
         if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
             TEST_FAIL_MESSAGE("exit semaphore not released by test task");
        }
    }
    vSemaphoreDelete(exit_semaph);

}

TEST_CASE("wolfssl aes multi-thread test ", "[wolfssl]")
{
    int num = 0;
    int i;

#ifndef NO_AES
    num++;
    num++;
#ifdef WOLFSSL_AES_192
    num++;
#endif
#ifdef WOLFSSL_AES_256
    num++;
#endif
#endif

    exit_loop = false;
    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    xTaskCreate(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_AES_192
    xTaskCreate(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_AES_256
    xTaskCreate(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif

    ESP_LOGI(TAG, "Waiting for 10s ...");
    vTaskDelay(10000/portTICK_PERIOD_MS);
    exit_loop = true;

    for(i = 0;i < num;i++){
        if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
            TEST_FAIL_MESSAGE("exit semaphore not released by test task");
        }
    }

    vSemaphoreDelete(exit_semaph);

}

TEST_CASE("wolfssl aes sha sha256 multi-thread test ", "[wolfssl]")
{
    int num = 0;
    int i;

#ifndef NO_AES
    num++;
    num++;
#ifdef WOLFSSL_AES_192
    num++;
#endif
#ifdef WOLFSSL_AES_256
    num++;
#endif
#endif
#ifndef NO_SHA
    num++;
#endif
#ifndef NO_SHA256
    num++;
#endif

    exit_loop = false;

#ifndef CONFIG_FREERTOS_UNICORE
    num *= 2;
    printf("num=%d\n", num);

    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    if(xTaskCreatePinnedToCore(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -1 \n");
    if(xTaskCreatePinnedToCore(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -2 \n");
    if(xTaskCreatePinnedToCore(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -3 \n");
    if(xTaskCreatePinnedToCore(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -4 \n");
#endif
#ifdef WOLFSSL_AES_192
    if(xTaskCreatePinnedToCore(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -5 \n");
    if(xTaskCreatePinnedToCore(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -6 \n");
#endif
#ifdef WOLFSSL_AES_256
    if(xTaskCreatePinnedToCore(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -7 \n");
    if(xTaskCreatePinnedToCore(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -8 \n");
#endif
#ifndef NO_SHA
    if(xTaskCreatePinnedToCore(tskSha_Test, "Sha_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -9 \n");
    if(xTaskCreatePinnedToCore(tskSha_Test, "Sha_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -10 \n");
#endif
#ifndef NO_SHA256
    if(xTaskCreatePinnedToCore(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -11 \n");
    if(xTaskCreatePinnedToCore(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -12 \n");
#endif

#else

    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    xTaskCreate(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_AES_192
    xTaskCreate(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_AES_256
    xTaskCreate(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_SHA
    xTaskCreate(tskSha_Test, "Sha_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_SHA256
    xTaskCreate(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif

#endif /* CONFIG_FREERTOS_UNICORE */

    ESP_LOGI(TAG, "Waiting for 15s ...");
    vTaskDelay(15000/portTICK_PERIOD_MS);
    exit_loop = true;

    for(i = 0;i < num;i++){
       if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
           TEST_FAIL_MESSAGE("exit semaphore not released by test task");
       }
    }
    vSemaphoreDelete(exit_semaph);
}

TEST_CASE("wolfssl aes sha384 sha512 multi-thread test ", "[wolfssl]")
{
    int num = 0;
    int i;

#ifndef NO_AES
    num++;
    num++;
#ifdef WOLFSSL_AES_192
    num++;
#endif
#ifdef WOLFSSL_AES_256
    num++;
#endif
#endif
#ifdef WOLFSSL_SHA384
    num++;
#endif
#ifdef WOLFSSL_SHA512
    num++;
#endif


    exit_loop = false;

#ifndef CONFIG_FREERTOS_UNICORE
    num *= 2;
    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    if(xTaskCreatePinnedToCore(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -1 \n");
    if(xTaskCreatePinnedToCore(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -2 \n");
    if(xTaskCreatePinnedToCore(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -3 \n");
    if(xTaskCreatePinnedToCore(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -4 \n");
#endif
#ifdef WOLFSSL_AES_192
    if(xTaskCreatePinnedToCore(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -5 \n");
    if(xTaskCreatePinnedToCore(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -6 \n");
#endif
#ifdef WOLFSSL_AES_256
    if(xTaskCreatePinnedToCore(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -7 \n");
    if(xTaskCreatePinnedToCore(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -8 \n");
#endif
#ifdef WOLFSSL_SHA384
    if(xTaskCreatePinnedToCore(tskSha384_Test, "sha384_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -13 \n");
    if(xTaskCreatePinnedToCore(tskSha384_Test, "sha384_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -14 \n");
#endif
#ifdef WOLFSSL_SHA512
    printf("start sha512\n");
    if(xTaskCreatePinnedToCore(tskSha512_Test, "Sha512_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -15 \n");
    if(xTaskCreatePinnedToCore(tskSha512_Test, "Sha512_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -16 \n");

#endif

#else

    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    xTaskCreate(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_AES_192
    xTaskCreate(tskAes192_Test, "Aes192_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifdef WOLFSSL_AES_256
    xTaskCreate(tskAes256_Test, "Aes256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_SHA
    xTaskCreate(tskSha_Test, "Sha_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_SHA256
    xTaskCreate(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif

#endif /* CONFIG_FREERTOS_UNICORE */

    ESP_LOGI(TAG, "Waiting for 15s ...");
    vTaskDelay(15000/portTICK_PERIOD_MS);
    exit_loop = true;


    for(i = 0;i < num;i++){
       if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
           TEST_FAIL_MESSAGE("exit semaphore not released by test task");
       }
    }
    vSemaphoreDelete(exit_semaph);
}

TEST_CASE("wolfssl aes sha256 rsa multi-thread test ", "[wolfssl]")
{
    int num = 0;
    int i;

#ifndef NO_AES
    num++;
    num++;
#endif
#ifndef NO_SHA256
    num++;
#endif
#ifndef NO_RSA
    num++;
#endif

    exit_loop = false;

#ifndef CONFIG_FREERTOS_UNICORE
    num *= 2;
    printf("num=%d\n", num);

    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    if(xTaskCreatePinnedToCore(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -1 \n");
    if(xTaskCreatePinnedToCore(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -2 \n");
    if(xTaskCreatePinnedToCore(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -3 \n");
    if(xTaskCreatePinnedToCore(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -4 \n");
#endif
#ifndef NO_SHA256
    if(xTaskCreatePinnedToCore(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -5 \n");
    if(xTaskCreatePinnedToCore(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -6 \n");
#endif
#ifndef NO_RSA
    if(xTaskCreatePinnedToCore(tskRsa_Test, "rsa_test", SHA_STACK_SIZE, NULL, 3, NULL, 0)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -7 \n");
    if(xTaskCreatePinnedToCore(tskRsa_Test, "rsa_test", SHA_STACK_SIZE, NULL, 3, NULL, 1)!=pdPASS)
        ESP_LOGE(TAG, "failed to create task -8 \n");
#endif
#else

    exit_semaph = xSemaphoreCreateCounting(num, 0);

#ifndef NO_AES
    xTaskCreate(tskAes_Test, "Aes_test", SHA_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(tskAesGcm_Test, "AesGcm_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_SHA256
    xTaskCreate(tskSha256_Test, "sha256_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#ifndef NO_RSA
    xTaskCreate(tskRsa_Test, "rsa_test", SHA_STACK_SIZE, NULL, 3, NULL);
#endif
#endif /* CONFIG_FREERTOS_UNICORE */

    ESP_LOGI(TAG, "Waiting for 15s ...");
    vTaskDelay(15000/portTICK_PERIOD_MS);
    exit_loop = true;

    /* wait until rsa test finishes */
    while(rsa_elapsedTime==0){ vTaskDelay(1); }

    ESP_LOGI(TAG, "Waiting another %llu s. rsa test would take more time to finish.",
               (rsa_elapsedTime+3000)/portTICK_PERIOD_MS);
    vTaskDelay((rsa_elapsedTime+3000)/portTICK_PERIOD_MS);

    for(i = 0;i < num;i++){
       if(!xSemaphoreTake(exit_semaph, 2000/portTICK_PERIOD_MS)) {
           TEST_FAIL_MESSAGE("exit semaphore not released by test task");
       }
    }
    vSemaphoreDelete(exit_semaph);
}
