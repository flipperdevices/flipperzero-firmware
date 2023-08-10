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


#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/test/test.h>
#include <wolfcrypt/benchmark/benchmark.h>

/* wolfCrypt_Init/wolfCrypt_Cleanup to turn CryptoCell hardware on/off */
#include <wolfssl/wolfcrypt/wc_port.h>

/* SEGGER_RTT_Init, you can potential replace it with other serial terminal */
#include "SEGGER_RTT.h"

int main(void)
{
    int ret;

    SEGGER_RTT_Init();

    if ((ret = wolfCrypt_Init()) != 0) {
        printf("wolfCrypt_Init failed %d\n", ret);
        return -1;
    }

#ifndef NO_CRYPT_TEST
    printf("\nwolfCrypt Test Started\n");
    wolfcrypt_test(NULL);
    printf("\nwolfCrypt Test Completed\n");
#endif

#ifndef NO_CRYPT_BENCHMARK
    printf("\nBenchmark Test Started\n");
    benchmark_test(NULL);
    printf("\nBenchmark Test Completed\n");
#endif

    if ((ret = wolfCrypt_Cleanup()) != 0) {
        printf("wolfCrypt_Cleanup failed %d\n", ret);
        return -1;
    }
    while(1) {
        __WFI();
    }

    return 0;
}

