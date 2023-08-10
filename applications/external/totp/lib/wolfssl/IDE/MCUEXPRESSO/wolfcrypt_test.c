/* wolfcrypt_test.c
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
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

#ifdef CPU_MIMXRT1176DVMAA_cm7
    #include "MIMXRT1176_cm7.h"
#else
    #include "MIMXRT685S_cm33.h"
#endif


#include "fsl_debug_console.h"

#include <wolfssl/wolfcrypt/wc_port.h>
#include <wolfssl/wolfcrypt/logging.h>
#include "wolfcrypt/test/test.h"


#if defined(FREESCALE_KSDK_2_0_TRNG) && defined(FREESCALE_RTC)
#include "fsl_rtc.h"
#include "fsl_trng.h"
/* start the RTC and TRNG */
static void setup()
{
    rtc_datetime_t date;
    trng_config_t trngConfig;
    status_t status;

    RTC_Init(RTC);

    /* setup a default start date */
    date.year   = 2022U;
    date.month  = 8U;
    date.day    = 17U;
    date.hour   = 15U;
    date.minute = 10;
    date.second = 0;

    RTC_EnableTimer(RTC, false);
    RTC_SetDatetime(RTC, &date);
    RTC_EnableTimer(RTC, true);

    TRNG_GetDefaultConfig(&trngConfig);

    /* Commented in example NXP TRNG as an optional, better random mode */
    trngConfig.sampleMode = kTRNG_SampleModeVonNeumann;

    /* Initialize TRNG */
    status = TRNG_Init(TRNG0, &trngConfig);
    if (status != kStatus_Success) {
        PRINTF("Issues starting TRNG\n");
    }
}
#elif defined(FREESCALE_SNVS_RTC)
#include "fsl_snvs_hp.h"
static void setup()
{
    snvs_hp_rtc_datetime_t rtcDate;
    snvs_hp_rtc_config_t snvsRtcConfig;

/* Init SNVS */
/*
 * snvsConfig->rtccalenable = false;
 * snvsConfig->rtccalvalue = 0U;
 * snvsConfig->srtccalenable = false;
 * snvsConfig->srtccalvalue = 0U;
 * snvsConfig->PIFreq = 0U;
 */
SNVS_HP_RTC_GetDefaultConfig(&snvsRtcConfig);
SNVS_HP_RTC_Init(SNVS, &snvsRtcConfig);

PRINTF("SNVS HP example:\r\n");

/* Set a start date time and start RT */
rtcDate.year   = 2014U;
rtcDate.month  = 12U;
rtcDate.day    = 25U;
rtcDate.hour   = 19U;
rtcDate.minute = 0;
rtcDate.second = 0;

/* Set RTC time to default time and date and start the RTC */
SNVS_HP_RTC_SetDatetime(SNVS, &rtcDate);
}
#else
static void setup()
{
	/* no clock or trng to setup */
}
#endif

int main(void)
{
    volatile int i = 0;
	int ret;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    setup(); /* set the RTC and start the TRNG */

    /* initialize wolfCrypt and run tests */
    if (wolfCrypt_Init() == 0) {
        PRINTF("Running wolfcrypt tests....\r\n");
        wolfSSL_Debugging_ON();
    	ret = wolfcrypt_test(NULL);
        PRINTF("Return of wolfcrypt_test = %d\r\n", ret);
        wolfCrypt_Cleanup();
    }
    else {
    	PRINTF("Failed to initialize wolfCrypt\r\n");
    }

#if defined(FREESCALE_KSDK_2_0_TRNG) && defined(FREESCALE_RTC)
    TRNG_Deinit(TRNG0);
#endif

    while(1) {
        i++;
        __asm volatile ("nop");
    }
    return 0 ;
}
