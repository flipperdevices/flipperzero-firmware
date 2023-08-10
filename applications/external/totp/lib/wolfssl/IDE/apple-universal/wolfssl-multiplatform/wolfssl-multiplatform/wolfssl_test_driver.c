/* wolfssl_test_driver.c
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

#include "wolfssl_test_driver.h"

#ifndef WOLFSSL_USER_SETTINGS
#include <wolfssl/options.h>
#endif
#include <wolfssl/ssl.h>

#include "test.h"
#include "benchmark.h"
#include "simple_client_example.h"

typedef struct test_func_args {
    int argc;
    char** argv;
    int return_code;
} test_func_args;



void wolfssl_test(void)
{
    int ret;
    test_func_args args = {0};
    
#ifdef WC_RNG_SEED_CB
    wc_SetSeed_Cb(wc_GenerateSeed);
#endif
    
    printf("Run wolfCrypt Test:\n");
    ret = wolfcrypt_test(&args);
    printf("\nResult of wolfcrypt_test() = %d\n\n", ret);
    
    printf("Run wolfCrypt Benchmark:\n");
    ret = benchmark_test(&args);
    printf("\nResult of benchmark_test() = %d\n\n", ret);
    
    printf("Run simple client test:\n");
    ret = simple_client_example();
    printf("\nResult of simple_client_test() = %d\n\n", ret);
    
}
