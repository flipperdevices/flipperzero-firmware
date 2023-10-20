/* nrf51.c
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
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_NRF51) || defined(WOLFSSL_NRF5x)

#include "bsp.h"
#include "nrf_delay.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf_drv_rng.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrf_ecb.h"

#ifdef SOFTDEVICE_PRESENT
    #include "softdevice_handler.h"
    #include "nrf_soc.h"
#endif /* SOFTDEVICE_PRESENT */

/* RTC */
#ifndef NO_CRYPT_BENCHMARK
static byte mRtcInitDone = 0;
static int mRtcSec = 0;
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0); /**< Declaring an instance of nrf_drv_rtc for RTC0. */
#endif /* !NO_CRYPT_BENCHMARK */

/* AES */
#if !defined(NO_AES) && defined(WOLFSSL_NRF51_AES) && !defined(SOFTDEVICE_PRESENT)
    static byte mAesInitDone = 0;
#endif

/** @brief Function for getting vector of random numbers.
 *
 * @param[out] p_buff   Pointer to unit8_t buffer for storing the bytes.
 * @param[in]  length   Number of bytes to take from pool and place in p_buff.
 *
 * @retval     0 = Success, else error
 */
int nrf51_random_generate(byte* output, word32 size)
{
    int remaining = size, length, pos = 0;
    uint8_t available;
    uint32_t err_code;

    /* Make sure RNG is running */
    err_code = nrf_drv_rng_init(NULL);
    if (err_code != NRF_SUCCESS && err_code != NRF_ERROR_INVALID_STATE) {
        return -1;
    }

    while (remaining > 0) {
        available = 0;
        nrf_drv_rng_bytes_available(&available); /* is void */
        length = (remaining < available) ? remaining : available;
        if (length > 0) {
            err_code = nrf_drv_rng_rand(&output[pos], length);
            remaining -= length;
            pos += length;
        }
        if (err_code != NRF_SUCCESS) {
            break;
        }
    }

    return (err_code == NRF_SUCCESS) ? 0 : -1;
}

#if !defined(NO_AES) && defined(WOLFSSL_NRF51_AES)

#ifdef SOFTDEVICE_PRESENT
static const byte* nRF51AesKey = NULL;
#endif
int nrf51_aes_set_key(const byte* key)
{
#ifdef SOFTDEVICE_PRESENT
    nRF51AesKey = key;
#else
    if (!mAesInitDone) {
        nrf_ecb_init();
        mAesInitDone = 1;
    }
    nrf_ecb_set_key(key);
#endif
    return 0;
}


int nrf51_aes_encrypt(const byte* in, const byte* key, word32 rounds, byte* out)
{
    int ret;
    uint32_t err_code = 0;
#ifdef SOFTDEVICE_PRESENT
    nrf_ecb_hal_data_t ecb_hal_data;
#endif

    /* Set key */
    ret = nrf51_aes_set_key(key);
    if (ret != 0) {
        return ret;
    }

#ifdef SOFTDEVICE_PRESENT
    /* Define ECB record */
    XMEMCPY(ecb_hal_data.key, nRF51AesKey, SOC_ECB_KEY_LENGTH);
    XMEMCPY(ecb_hal_data.cleartext, in, SOC_ECB_CLEARTEXT_LENGTH);
    XMEMSET(ecb_hal_data.ciphertext, 0, SOC_ECB_CIPHERTEXT_LENGTH);

    /* Perform block encrypt */
    err_code = sd_ecb_block_encrypt(&ecb_hal_data);
    if (err_code != NRF_SUCCESS) {
        return -1;
    }

    /* Grab result */
    XMEMCPY(out, ecb_hal_data.ciphertext, SOC_ECB_CIPHERTEXT_LENGTH);
#else
    err_code = nrf_ecb_crypt(out, in);
    err_code = err_code ? 0 : -1;
#endif

    return err_code;
}

#endif /* !NO_AES && WOLFSSL_NRF51_AES */


#ifndef NO_CRYPT_BENCHMARK
static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
    {
        mRtcSec++;
        nrf_drv_rtc_counter_clear(&rtc);
        nrf_drv_rtc_int_enable(&rtc, RTC_CHANNEL_INT_MASK(0));

#ifdef BSP_LED_0
        nrf_gpio_pin_toggle(BSP_LED_0);
#endif
    }
}

#ifndef RTC0_CONFIG_FREQUENCY
#define RTC0_CONFIG_FREQUENCY 32768
#endif

static void rtc_config(void)
{
    uint32_t err_code;

    /* Start the internal LFCLK XTAL oscillator */
#if defined(NRF52) || defined(NRF52_SERIES)
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
#else
    err_code = nrf_drv_clock_init(NULL);
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request();
#endif

    /* Initialize RTC instance */
    err_code = nrf_drv_rtc_init(&rtc, NULL, rtc_handler);
    APP_ERROR_CHECK(err_code);

    /* Enable tick event */
    nrf_drv_rtc_tick_enable(&rtc, false);

    /* Set compare channel to trigger interrupt after 1 seconds */
    err_code = nrf_drv_rtc_cc_set(&rtc, 0, RTC0_CONFIG_FREQUENCY, true);
    APP_ERROR_CHECK(err_code);

    /* Power on RTC instance */
    nrf_drv_rtc_enable(&rtc);
}

static int rtc_get_ms(void)
{
    /* Prescaler is 12-bit for COUNTER: frequency = (32768/(PRESCALER+1)) */
    int frequency = (32768 / (rtc_prescaler_get(rtc.p_reg) + 1));
    int counter = nrf_drv_rtc_counter_get(&rtc);

    /* Convert with rounding frequency to milliseconds */
    return ((counter * 1000) + (frequency / 2) ) / frequency;
}

double current_time(int reset)
{
    double time;

    if (!mRtcInitDone) {
        rtc_config();
        mRtcInitDone = 1;
    }

    time = mRtcSec;
    time += (double)rtc_get_ms() / 1000;

    return time;
}
#endif /* !NO_CRYPT_BENCHMARK */

#endif /* WOLFSSL_NRF51 || WOLFSSL_NRF5x */
