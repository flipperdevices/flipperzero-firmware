/* esp32_util.c
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
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/version.h>

#if defined(WOLFSSL_ESP32_CRYPT) && \
  (!defined(NO_AES)        || !defined(NO_SHA) || !defined(NO_SHA256) ||\
   defined(WOLFSSL_SHA384) || defined(WOLFSSL_SHA512))

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>


/*
 * initialize our mutex used to lock hardware access
 *
 * returns:
 *   0 upon success,
 *   BAD_MUTEX_E for null mutex
 *   other value from wc_InitMutex()
 *
 */
int esp_CryptHwMutexInit(wolfSSL_Mutex* mutex) {
    if (mutex == NULL) {
        return BAD_MUTEX_E;
    }

    return wc_InitMutex(mutex);
}

/*
 * call the ESP-IDF mutex lock; xSemaphoreTake
 *
 */
int esp_CryptHwMutexLock(wolfSSL_Mutex* mutex, TickType_t xBlockTime) {
    if (mutex == NULL) {
        WOLFSSL_ERROR_MSG("esp_CryptHwMutexLock called with null mutex");
        return BAD_MUTEX_E;
    }

#ifdef SINGLE_THREADED
    return wc_LockMutex(mutex); /* xSemaphoreTake take with portMAX_DELAY */
#else
    return ((xSemaphoreTake( *mutex, xBlockTime ) == pdTRUE) ? 0 : BAD_MUTEX_E);
#endif
}

/*
 * call the ESP-IDF mutex UNlock; xSemaphoreGive
 *
 */
int esp_CryptHwMutexUnLock(wolfSSL_Mutex* mutex) {
    if (mutex == NULL) {
        WOLFSSL_ERROR_MSG("esp_CryptHwMutexLock called with null mutex");
        return BAD_MUTEX_E;
    }

#ifdef SINGLE_THREADED
    return wc_UnLockMutex(mutex);
#else
    xSemaphoreGive(*mutex);
    return 0;
#endif
}

/*
** Version / Platform info.
**
** This could evolve into a wolfSSL-wide feature. For now, here only. See:
** https://github.com/wolfSSL/wolfssl/pull/6149
*/
#if defined(WOLFSSL_ESPIDF)
    #include <esp_log.h>
    #include "sdkconfig.h"
    const char* TAG = "Version Info";
    #define WOLFSSL_VERSION_PRINTF(...) ESP_LOGI(TAG, __VA_ARGS__)
#else
    #include <stdio.h>
    #define WOLFSSL_VERSION_PRINTF(...) { printf(__VA_ARGS__); printf("\n"); }
#endif

/*
*******************************************************************************
** Specific Platforms
*******************************************************************************
*/

/*
** Specific platforms: Espressif
*/
#if defined(WOLFSSL_ESPIDF)
static int ShowExtendedSystemInfo_platform_espressif()
{
#if defined(CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
    WOLFSSL_VERSION_PRINTF("CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ: %u MHz",
                           CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ);
#endif

#if CONFIG_IDF_TARGET_ESP32

    WOLFSSL_VERSION_PRINTF("Xthal_have_ccount: %u",
                           Xthal_have_ccount);

    /* this is the legacy stack size */
#if defined(CONFIG_MAIN_TASK_STACK_SIZE)
    WOLFSSL_VERSION_PRINTF("CONFIG_MAIN_TASK_STACK_SIZE: %d",
                           CONFIG_MAIN_TASK_STACK_SIZE);
#endif

    /* this is the modern stack size */
#if defined(CONFIG_ESP_MAIN_TASK_STACK_SIZE)
    WOLFSSL_VERSION_PRINTF("CONFIG_ESP_MAIN_TASK_STACK_SIZE: %d",
                           CONFIG_ESP_MAIN_TASK_STACK_SIZE);
#endif

#if defined(CONFIG_TIMER_TASK_STACK_SIZE)
    WOLFSSL_VERSION_PRINTF("CONFIG_TIMER_TASK_STACK_SIZE: %d",
                           CONFIG_TIMER_TASK_STACK_SIZE);
#endif

#if defined(CONFIG_TIMER_TASK_STACK_DEPTH)
    WOLFSSL_VERSION_PRINTF("CONFIG_TIMER_TASK_STACK_DEPTH: %d",
                           CONFIG_TIMER_TASK_STACK_DEPTH);
#endif

#if defined(SINGLE_THREADED)
    /* see also HAVE_STACK_SIZE_VERBOSE */
    char thisHWM = 0;
    WOLFSSL_VERSION_PRINTF("Stack HWM: %x", (size_t) &thisHWM);
#else
    WOLFSSL_VERSION_PRINTF("Stack HWM: %d",
                           uxTaskGetStackHighWaterMark(NULL));
#endif

#elif CONFIG_IDF_TARGET_ESP32S2
    WOLFSSL_VERSION_PRINTF("Xthal_have_ccount = %u",
                           Xthal_have_ccount);
#elif CONFIG_IDF_TARGET_ESP32C6
    /* not supported at this time */
#elif CONFIG_IDF_TARGET_ESP32C3
    /* not supported at this time */
#elif CONFIG_IDF_TARGET_ESP32S3
    WOLFSSL_VERSION_PRINTF("Xthal_have_ccount = %u",
                           Xthal_have_ccount);
#elif CONFIG_IDF_TARGET_ESP32H2
    /* not supported at this time */
#elif CONFIG_IDF_TARGET_ESP32C2
    /* not supported at this time */
#else
    /* not supported at this time */
#endif

    /* check to see if we are using hardware encryption */
#if defined(NO_ESP32_CRYPT)
    WOLFSSL_VERSION_PRINTF("NO_ESP32_CRYPT defined! "
                           "HW acceleration DISABLED.");
#else
    /* first show what platform hardware acceleration is enabled
    ** (some new platforms may not be supported yet) */
#if defined(CONFIG_IDF_TARGET_ESP32)
    WOLFSSL_VERSION_PRINTF("ESP32_CRYPT is enabled for ESP32.");
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    WOLFSSL_VERSION_PRINTF("ESP32_CRYPT is enabled for ESP32-S2.");
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    WOLFSSL_VERSION_PRINTF("ESP32_CRYPT is enabled for ESP32-S3.");
#else
#error "ESP32_CRYPT not yet supported on this IDF TARGET"
#endif

    /* Even though enabled, some specifics may be disabled */
#if defined(NO_WOLFSSL_ESP32_CRYPT_HASH)
    WOLFSSL_VERSION_PRINTF("NO_WOLFSSL_ESP32_CRYPT_HASH is defined!"
                           "(disabled HW SHA).");
#endif

#if defined(NO_WOLFSSL_ESP32_CRYPT_AES)
    WOLFSSL_VERSION_PRINTF("NO_WOLFSSL_ESP32_CRYPT_AES is defined!"
                           "(disabled HW AES).");
#endif

#if defined(NO_WOLFSSL_ESP32_CRYPT_RSA_PRI)
    WOLFSSL_VERSION_PRINTF("NO_WOLFSSL_ESP32_CRYPT_RSA_PRI defined!"
                           "(disabled HW RSA)");
#endif
#endif

    return 0;
}
#endif

/*
*******************************************************************************
** All Platforms
*******************************************************************************
*/

/*
** All platforms: git details
*/
static int ShowExtendedSystemInfo_git()
{
#if defined(HAVE_WC_INTROSPECTION) && !defined(ALLOW_BINARY_MISMATCH_INTROSPECTION)
#pragma message("WARNING: both HAVE_VERSION_EXTENDED_INFO and " \
                "HAVE_WC_INTROSPECTION are enabled. Some extended " \
                "information details will not be available.")

    WOLFSSL_VERSION_PRINTF("HAVE_WC_INTROSPECTION enabled. "
                           "Some extended system details not available.");
#else
    /* Display some interesting git values that may change,
    ** but not desired for introspection which requires object code to be
    ** maximally bitwise-invariant.
    */
#if defined(LIBWOLFSSL_VERSION_GIT_ORIGIN)
        /* git config --get remote.origin.url */
    WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_GIT_ORIGIN = %s",
                           LIBWOLFSSL_VERSION_GIT_ORIGIN);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_BRANCH)
    /* git rev-parse --abbrev-ref HEAD */
    WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_GIT_BRANCH = %s",
                           LIBWOLFSSL_VERSION_GIT_BRANCH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_HASH)
    WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_GIT_HASH = %s",
                           LIBWOLFSSL_VERSION_GIT_HASH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_SHORT_HASH )
    WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_GIT_SHORT_HASH = %s",
                           LIBWOLFSSL_VERSION_GIT_SHORT_HASH);
#endif

#if defined(LIBWOLFSSL_VERSION_GIT_HASH_DATE)
    WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_GIT_HASH_DATE = %s",
                           LIBWOLFSSL_VERSION_GIT_HASH_DATE);
#endif

#endif /* else not HAVE_WC_INTROSPECTION */
    return 0;
}

/*
** All platforms: thread details
*/
static int ShowExtendedSystemInfo_thread()
{
    /* all platforms: stack high water mark check */
#if defined(SINGLE_THREADED)
    WOLFSSL_VERSION_PRINTF("SINGLE_THREADED");
#else
    WOLFSSL_VERSION_PRINTF("NOT SINGLE_THREADED");
#endif
    return 0;
}

/*
** All Platforms: platform details
*/
static int ShowExtendedSystemInfo_platform()
{
#if defined(WOLFSSL_ESPIDF)
#if defined(CONFIG_IDF_TARGET)
    WOLFSSL_VERSION_PRINTF("CONFIG_IDF_TARGET = %s",
                           CONFIG_IDF_TARGET);
    ShowExtendedSystemInfo_platform_espressif();
#endif
#endif
    return 0;
}

/*
*******************************************************************************
** The public ShowExtendedSystemInfo()
*******************************************************************************
*/

int ShowExtendedSystemInfo(void)
    {
        WOLFSSL_VERSION_PRINTF("Extended Version and Platform Information.");

#if defined(LIBWOLFSSL_VERSION_STRING)
        WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_STRING = %s",
                               LIBWOLFSSL_VERSION_STRING);
#endif

#if defined(LIBWOLFSSL_VERSION_HEX)
        WOLFSSL_VERSION_PRINTF("LIBWOLFSSL_VERSION_HEX = %x",
                               LIBWOLFSSL_VERSION_HEX);
#endif

#if defined(WOLFSSL_MULTI_INSTALL_WARNING)
        /* CMake may have detected undesired multiple installs, so give warning. */
        WOLFSSL_VERSION_PRINTF("");
        WOLFSSL_VERSION_PRINTF("WARNING: Multiple wolfSSL installs found.");
        WOLFSSL_VERSION_PRINTF("Check ESP-IDF and local project [components] directory.");
        WOLFSSL_VERSION_PRINTF("");
#endif

        ShowExtendedSystemInfo_git(); /* may be limited during active introspection */
        ShowExtendedSystemInfo_platform();
        ShowExtendedSystemInfo_thread();
        return 0;
    }



int esp_ShowExtendedSystemInfo()
{
    return ShowExtendedSystemInfo();
}

#endif

