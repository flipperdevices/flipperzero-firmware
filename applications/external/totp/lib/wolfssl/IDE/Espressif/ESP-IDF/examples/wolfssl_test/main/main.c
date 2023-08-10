/* main.c
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

/* ESP-IDF */
#include <esp_log.h>
#include "sdkconfig.h"

/* wolfSSL */
#include <wolfssl/wolfcrypt/settings.h>
#include <user_settings.h>
#include <wolfssl/version.h>
#ifndef WOLFSSL_ESPIDF
#warning "problem with wolfSSL user settings. Check components/wolfssl/include"
#endif

#include <wolfcrypt/test/test.h>

/*
** the wolfssl component can be installed in either:
**
**   - the ESP-IDF component directory
**
**       ** OR **
**
**   - the local project component directory
**
** it is not recommended to install in both.
**
*/

/*
** although the wolfcrypt/test includes a default time setting,
** see the enclosed optional time helper for adding NNTP.
** be sure to add "time_helper.c" in main/CMakeLists.txt
*/
#undef WOLFSSL_USE_TIME_HELPER
#if defined(WOLFSSL_USE_TIME_HELPER)
    #include "time_helper.h" */
#endif

/* see wolfssl/wolfcrypt/test/test.h */
extern void wolf_crypt_task();


static const char* const TAG = "wolfssl_test";

#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)

#include "wolfssl/wolfcrypt/port/atmel/atmel.h"

/* when you need to use a custom slot allocation, */
/* enable the definition CUSTOM_SLOT_ALLOCAION.   */
#if defined(CUSTOM_SLOT_ALLOCATION)

static byte mSlotList[ATECC_MAX_SLOT];

/* initialize slot array */
void my_atmel_slotInit()
{
    int i;
    for (i = 0; i < ATECC_MAX_SLOT; i++) {
        mSlotList[i] = ATECC_INVALID_SLOT;
    }
}

/* allocate slot depending on slotType */
int my_atmel_alloc(int slotType)
{
    int i, slot = ATECC_INVALID_SLOT;

    switch (slotType) {
        case ATMEL_SLOT_ENCKEY:
            slot = 4;
            break;
        case ATMEL_SLOT_DEVICE:
            slot = 0;
            break;
        case ATMEL_SLOT_ECDHE:
            slot = 0;
            break;
        case ATMEL_SLOT_ECDHE_ENC:
            slot = 4;
            break;
        case ATMEL_SLOT_ANY:
            for (i = 0; i < ATECC_MAX_SLOT; i++) {
                if (mSlotList[i] == ATECC_INVALID_SLOT) {
                    slot = i;
                    break;
                } /* if */
            } /* for */
    } /* switch */

    return slot;
}

/* free slot array       */
void my_atmel_free(int slotId)
{
    if (slotId >= 0 && slotId < ATECC_MAX_SLOT) {
        mSlotList[slotId] = ATECC_INVALID_SLOT;
    }
}

#endif /* CUSTOM_SLOT_ALLOCATION                                        */
#endif /* WOLFSSL_ESPWROOM32SE && HAVE_PK_CALLBACK && WOLFSSL_ATECC508A */


/* entry point */
void app_main(void)
{
    int rc = 0;
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "---------------------- BEGIN MAIN ----------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");
    ESP_LOGI(TAG, "--------------------------------------------------------");


    ESP_LOGI(TAG, "CONFIG_IDF_TARGET = %s", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_STRING = %s", LIBWOLFSSL_VERSION_STRING);

#if defined(LIBWOLFSSL_VERSION_GIT_HASH)
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_GIT_HASH = %s", LIBWOLFSSL_VERSION_GIT_HASH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_SHORT_HASH )
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_GIT_SHORT_HASH = %s", LIBWOLFSSL_VERSION_GIT_SHORT_HASH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_HASH_DATE)
    ESP_LOGI(TAG, "LIBWOLFSSL_VERSION_GIT_HASH_DATE = %s", LIBWOLFSSL_VERSION_GIT_HASH_DATE);
#endif


    /* some interesting settings are target specific (ESP32, -C3, -S3, etc */
#if defined(CONFIG_IDF_TARGET_ESP32C3)
    /* not available for C3 at this time */
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    ESP_LOGI(TAG, "CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ = %u MHz",
                   CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ
             );
    ESP_LOGI(TAG, "Xthal_have_ccount = %u", Xthal_have_ccount);
#else
    ESP_LOGI(TAG, "CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ = %u MHz",
                   CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
            );
    ESP_LOGI(TAG, "Xthal_have_ccount = %u", Xthal_have_ccount);
#endif

    /* all platforms: stack high water mark check */
    ESP_LOGI(TAG, "Stack HWM: %d\n", uxTaskGetStackHighWaterMark(NULL));

    /* check to see if we are using hardware encryption */
#if defined(NO_ESP32_CRYPT)
    ESP_LOGI(TAG, "NO_ESP32_CRYPT defined! HW acceleration DISABLED.");
#else
    #if defined(CONFIG_IDF_TARGET_ESP32C3)
        #error "ESP32_CRYPT not yet supported on ESP32-C3"
    #elif defined(CONFIG_IDF_TARGET_ESP32S2)
        #error "ESP32_CRYPT not yet supported on ESP32-S2"
    #elif defined(CONFIG_IDF_TARGET_ESP32S3)
        /* #error "ESP32_CRYPT not yet supported on ESP32-S3" */
        ESP_LOGI(TAG, "ESP32_CRYPT is enabled for  ESP32-S3.");
    #else
        ESP_LOGI(TAG, "ESP32_CRYPT is enabled.");
    #endif
#endif



#if defined (WOLFSSL_USE_TIME_HELPER)
    set_time();
#endif

/* when using atecc608a on esp32-wroom-32se */
#if defined(WOLFSSL_ESPWROOM32SE) && defined(HAVE_PK_CALLBACKS) \
                                  && defined(WOLFSSL_ATECC508A)
    #if defined(CUSTOM_SLOT_ALLOCATION)
    my_atmel_slotInit();
    /* to register the callback, it needs to be initialized. */
    if ((wolfCrypt_Init()) != 0) {
        ESP_LOGE(TAG, "wolfCrypt_Init failed");
        return;
    }
    atmel_set_slot_allocator(my_atmel_alloc, my_atmel_free);
    #endif
#endif

#ifdef NO_CRYPT_TEST
    ESP_LOGI(TAG, "NO_CRYPT_TEST defined, skipping wolf_test_task");
#else
    /* Although wolfCrypt_Init() may be explicitly called above,
    ** Note it is still always called in wolf_test_task.
    */
    rc = wolf_test_task();
    /* note wolfCrypt_Cleanup() should always be called when finished.
    ** This is called at the end of wolf_test_task();
    */

    if (rc == 0) {
        ESP_LOGI(TAG, "wolf_test_task complete success result code = %d", rc);
    }
    else {
        ESP_LOGE(TAG, "wolf_test_task FAIL result code = %d", rc);
        /* see wolfssl/wolfcrypt/error-crypt.h */
    }

    /* after the test, we'll just wait */
    while (1) {
        /* nothing */
    }
#endif
}
