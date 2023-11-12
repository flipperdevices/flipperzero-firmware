/* cryptoCell.c
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

/* This source is included in wc_port.c */
/* WOLFSSL_CRYPTOCELL_C is defined by wc_port.c in case compile tries to
    include this .c directly */
#ifdef WOLFSSL_CRYPTOCELL_C

#ifdef WOLFSSL_CRYPTOCELL

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/port/arm/cryptoCell.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

/* Global Variables (extern) */
CRYS_RND_State_t     wc_rndState;
CRYS_RND_WorkBuff_t  wc_rndWorkBuff;
SaSiRndGenerateVectWorkFunc_t wc_rndGenVectFunc = CRYS_RND_GenerateVector;

static word32 cc310_enableCount = 0;

static void cc310_enable(void)
{
    cc310_enableCount++;

    /* Enable the CC310 HW/IQ once*/

    NRF_CRYPTOCELL->ENABLE = 1;
    NVIC_EnableIRQ(CRYPTOCELL_IRQn);
}

static void cc310_disable(void)
{
    cc310_enableCount--;

    /* Disable HW/IRQ if no more users */
    if (cc310_enableCount == 0) {
        NRF_CRYPTOCELL->ENABLE = 0;
        NVIC_DisableIRQ(CRYPTOCELL_IRQn);
    }
}

int cc310_Init(void)
{
    int ret = 0;
    static int initialized = 0;

    if (!initialized) {
        /* Enable the CC310 HW. */
        cc310_enable();

        /*Initialize the CC310 run-time library*/
        ret = SaSi_LibInit();

        if (ret != SA_SILIB_RET_OK) {
            WOLFSSL_MSG("Error SaSi_LibInit");
            return ret;
        }

        /* RNG CryptoCell CC310 */
        ret = CRYS_RndInit(&wc_rndState, &wc_rndWorkBuff);
        if (ret != CRYS_OK) {
            WOLFSSL_MSG("Error CRYS_RndInit");
            return ret;
        }
        initialized = 1;
    }
    return ret;
}

void cc310_Free(void)
{
    CRYSError_t crys_result;

    SaSi_LibFini();

    crys_result = CRYS_RND_UnInstantiation(&wc_rndState);

    if (crys_result != CRYS_OK) {
        WOLFSSL_MSG("Error RYS_RND_UnInstantiation");
    }
    cc310_disable();
}

int cc310_random_generate(byte* output, word32 size)
{
    CRYSError_t crys_result;

    crys_result = CRYS_RND_GenerateVector(&wc_rndState, size, output);

    return (crys_result == CRYS_OK) ? 0 : -1;
}
#ifdef HAVE_ECC
CRYS_ECPKI_DomainID_t cc310_mapCurve(int curve_id)
{
    switch(curve_id)
    {
        case ECC_CURVE_DEF: return CRYS_ECPKI_DomainID_secp256r1; /* default */
        case ECC_SECP160K1: return CRYS_ECPKI_DomainID_secp160k1;
        case ECC_SECP160R1: return CRYS_ECPKI_DomainID_secp160r1;
        case ECC_SECP160R2: return CRYS_ECPKI_DomainID_secp160r2;
        case ECC_SECP192K1: return CRYS_ECPKI_DomainID_secp192k1;
        case ECC_SECP192R1: return CRYS_ECPKI_DomainID_secp192r1;
        case ECC_SECP224K1: return CRYS_ECPKI_DomainID_secp224k1;
        case ECC_SECP224R1: return CRYS_ECPKI_DomainID_secp224r1;
        case ECC_SECP256K1: return CRYS_ECPKI_DomainID_secp256k1;
        case ECC_SECP256R1: return CRYS_ECPKI_DomainID_secp256r1;
        case ECC_SECP384R1: return CRYS_ECPKI_DomainID_secp384r1;
        case ECC_SECP521R1: return CRYS_ECPKI_DomainID_secp521r1;
        default: WOLFSSL_MSG("Curve not identified");
                 return CRYS_ECPKI_DomainID_Builded;
    }
}
#endif /* HAVE_ECC */

#ifndef NO_RSA
CRYS_RSA_HASH_OpMode_t cc310_hashModeRSA(enum wc_HashType hash_type, int isHashed)
{
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
            return isHashed? CRYS_RSA_After_MD5_mode : CRYS_RSA_HASH_MD5_mode;
        #endif
        case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
            return isHashed? CRYS_RSA_After_SHA1_mode : CRYS_RSA_HASH_SHA1_mode;
        #endif
        case WC_HASH_TYPE_SHA224:
        #ifdef WOLFSSL_SHA224
            return isHashed? CRYS_RSA_After_SHA224_mode : CRYS_RSA_HASH_SHA224_mode;
        #endif
        case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
            return isHashed? CRYS_RSA_After_SHA256_mode : CRYS_RSA_HASH_SHA256_mode;
        #endif
        case WC_HASH_TYPE_SHA384:
        #ifdef WOLFSSL_SHA384
            return isHashed? CRYS_RSA_After_SHA384_mode : CRYS_RSA_HASH_SHA384_mode;
        #endif
        case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            return isHashed? CRYS_RSA_After_SHA512_mode : CRYS_RSA_HASH_SHA512_mode;
        #endif
        case WC_HASH_TYPE_NONE:
            /* default to SHA256 */
            return isHashed? CRYS_RSA_After_SHA256_mode : CRYS_RSA_HASH_SHA256_mode;
        default:
            return CRYS_RSA_After_HASH_NOT_KNOWN_mode;
    }
}
#endif /* !NO_RSA */

#ifdef HAVE_ECC
CRYS_ECPKI_HASH_OpMode_t cc310_hashModeECC(int hash_size)
{
    CRYS_ECPKI_HASH_OpMode_t hash_mode;
    switch (hash_size)
    {
        case 20:
            hash_mode = CRYS_ECPKI_AFTER_HASH_SHA1_mode;
            break;
        case 28:
            hash_mode = CRYS_ECPKI_AFTER_HASH_SHA224_mode;
            break;
        case 32:
            hash_mode = CRYS_ECPKI_AFTER_HASH_SHA256_mode;
            break;
        case 48:
            hash_mode = CRYS_ECPKI_AFTER_HASH_SHA384_mode;
            break;
        case 64:
            hash_mode = CRYS_ECPKI_AFTER_HASH_SHA512_mode;
            break;
        default:
            hash_mode = CRYS_ECPKI_HASH_OpModeLast;
            break;
    }
    return hash_mode;
}
#endif /* HAVE_ECC */
#endif /* WOLFSSL_CRYPTOCELL*/

#if !defined(NO_CRYPT_BENCHMARK) && defined(WOLFSSL_nRF5x_SDK_15_2)

static int mRtcSec = 0;
static const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(0);

static void rtc_handler(nrfx_rtc_int_type_t int_type)
{
    if (int_type == NRFX_RTC_INT_COMPARE0) {
        mRtcSec++;
        nrfx_rtc_counter_clear(&rtc);
        nrfx_rtc_int_enable(&rtc, RTC_CHANNEL_INT_MASK(0));
#ifdef BSP_LED_1
        nrf_gpio_pin_toggle(BSP_LED_1);
#endif
    }
    else if (int_type == NRF_DRV_RTC_INT_TICK) {
#ifdef BSP_LED_0
        nrf_gpio_pin_toggle(BSP_LED_0);
#endif
    }
}

static void rtc_config(void)
{
    uint32_t err_code;
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;

    /* configure gpio for pin toggling. */
    bsp_board_init(BSP_INIT_LEDS);

    /* start the internal LFCLK XTAL oscillator.*/
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);

    /* Initialize RTC instance */
    err_code = nrfx_rtc_init(&rtc, &config, rtc_handler);
    APP_ERROR_CHECK(err_code);

    /* Enable tick event */
    nrfx_rtc_tick_enable(&rtc, false);

    /* Set compare channel to trigger interrupt after 1 seconds */
    err_code = nrfx_rtc_cc_set(&rtc, 0, RTC_INPUT_FREQ, true);
    APP_ERROR_CHECK(err_code);

    /* Power on RTC instance */
    nrfx_rtc_enable(&rtc);
}

static int rtc_get_ms(void)
{
    /* Prescaler is 12-bit for COUNTER: frequency = (32768/(PRESCALER+1)) */
    int frequency = (RTC_INPUT_FREQ / (rtc_prescaler_get(rtc.p_reg) + 1));
    uint32_t counter = nrfx_rtc_counter_get(&rtc);

    /* Convert with rounding frequency to milliseconds */
    return ((counter * 1000) + (frequency / 2) ) / frequency;
}

double current_time(int reset)
{
    double time;
    static int initialized = 0;

    if (!initialized) {
        rtc_config();
        initialized = 1;
    }
    time = mRtcSec;
    time += (double)rtc_get_ms() / 1000;

    return time;
}

int nrf_random_generate(byte* output, word32 size)
{
    uint32_t err_code;
    static int initialized = 0;

    /* RNG must be initialized once */
    if (!initialized) {
        err_code = nrf_drv_rng_init(NULL);
        if (err_code != NRF_SUCCESS) {
            return -1;
        }
        initialized = 1;
    }
    nrf_drv_rng_block_rand(output, size);
    return 0;
}
#endif /* !NO_CRYPT_BENCHMARK && WOLFSSL_nRF5x_SDK_15_2 */

#endif /* WOLFSSL_CRYPTOCELL_C */
