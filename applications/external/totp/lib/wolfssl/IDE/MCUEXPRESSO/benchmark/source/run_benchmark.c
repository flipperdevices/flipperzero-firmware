/* run_benchmark.c
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
#include "fsl_rtc.h"
#include "fsl_trng.h"
#include "FreeRTOS.h"
#include "task.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT685S_cm33.h"
#include "fsl_debug_console.h"

#include <wolfssl/wolfcrypt/wc_port.h>
#include "benchmark.h"

#define STACK_DEPTH 60000
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

static void doBenchmark(void* params)
{
    int ret;

    /* initialize wolfCrypt and run tests */
    if (wolfCrypt_Init() == 0) {
    	ret = benchmark_test(NULL);
        PRINTF("Return of benchmark_test = %d\r\n", ret);
        wolfCrypt_Cleanup();
    }
    else {
    	PRINTF("Failed to initialize wolfCrypt\r\n");
    }
}

int main(void)
{
    TaskHandle_t b = NULL;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    setup(); /* set the RTC and start the TRNG */

    if (xTaskCreate(doBenchmark, "wolfSSL Benchmark", STACK_DEPTH, NULL,
    		0, &b) != pdPASS) {
    	PRINTF("Error creating benchmark task\r\n");
    }
    vTaskStartScheduler();
    TRNG_Deinit(TRNG0);
    vTaskDelete(b);
    return 0 ;
}
