/* wolf_main.c
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


/* wolfSSL includes */
#ifndef WOLFSSL_USER_SETTINGS
    #include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/random.h> /* for CUSTOM_RAND_TYPE */
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Infineon Includes */
#include "Ifx_Types.h"
#include "IfxStm.h"

extern int send_UART(const char* str);
static void my_logging_cb(const int logLevel, const char *const logMessage)
{
    send_UART(logMessage);
    send_UART("\r\n");
    (void)logLevel; /* not used */
}

/* TIME CODE */
/* Optionally you can define NO_ASN_TIME to disable all cert time checks */
static int hw_get_time_sec(void)
{
    /* get time in seconds */
    return IfxStm_get(&MODULE_STM0) / IfxStm_getFrequency(&MODULE_STM0);
}

/* This is used by wolfCrypt asn.c for cert time checking */
unsigned long my_time(unsigned long* timer)
{
    (void)timer;
    return hw_get_time_sec();
}

#ifndef WOLFCRYPT_ONLY
/* This is used by TLS only */
unsigned int LowResTimer(void)
{
    return hw_get_time_sec();
}
#endif

#ifndef NO_CRYPT_BENCHMARK
/* This is used by wolfCrypt benchmark tool only */
double current_time(int reset)
{
    double timeNow;
    uint64_t timeMs, ticks = IfxStm_get(&MODULE_STM0);
    (void)reset;
    timeMs = ticks / (IfxStm_getFrequency(&MODULE_STM0) / 1000);
    timeNow = (timeMs / 1000); // sec
    timeNow += (double)(timeMs % 1000) / 1000; // ms
    return timeNow;
}
#endif

/* RNG CODE */
/* TODO: Implement real RNG */
static unsigned int gCounter;
unsigned int hw_rand(void)
{
    //#warning Must implement your own random source

    return ++gCounter;
}

unsigned int my_rng_seed_gen(void)
{
    return hw_rand();
}

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

void run_wolf_tests(void)
{
    func_args args;

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif
    wolfSSL_SetLoggingCb(my_logging_cb);

    /* initialize wolfSSL */
#ifdef WOLFCRYPT_ONLY
    wolfCrypt_Init();
#else
    wolfSSL_Init();
#endif

    memset(&args, 0, sizeof(args));
    args.return_code = NOT_COMPILED_IN; /* default */

    printf("Running wolfCrypt Tests...\n");
#ifndef NO_CRYPT_TEST
    args.return_code = 0;
    wolfcrypt_test(&args);
    printf("Crypt Test: Return code %d\n", args.return_code);
#else
    args.return_code = NOT_COMPILED_IN;
#endif

    printf("Running wolfCrypt Benchmarks...\n");
#ifndef NO_CRYPT_BENCHMARK
    args.return_code = 0;
    benchmark_test(&args);
#else
    args.return_code = NOT_COMPILED_IN;
#endif
    printf("Benchmark Test: Return code %d\n", args.return_code);

#ifdef WOLFCRYPT_ONLY
    wolfCrypt_Cleanup();
#else
    wolfSSL_Cleanup();
#endif
}
