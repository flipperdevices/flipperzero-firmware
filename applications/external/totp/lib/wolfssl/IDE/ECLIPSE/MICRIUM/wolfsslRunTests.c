/* wolfsslRunTests.c
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

#include  <includes.h> /* master includes from Micrium Freescale Kinetis K70*/

#include  <wolfssl/ssl.h>

/*
* Description :  This function runs wolfssl tests.
* Caller(s)   :  main() in app.c
* Note(s)     :  none.
*/

int  wolfsslRunTests (void)
{
    CLK_ERR        err;
    CLK_TS_SEC     ts_unix_sec;
    CPU_BOOLEAN    valid;
    static int initialized = 0;

    if(!initialized) {
        Clk_Init(&err);

        if (err == CLK_ERR_NONE) {
            APP_TRACE_INFO(("Clock module successfully initialized\n"));
        } else {
            APP_TRACE_INFO(("Clock module initialization failed\n"));
            return -1;
        }

        valid = Clk_GetTS_Unix(&ts_unix_sec);

        if (valid == DEF_OK) {
            APP_TRACE_INFO(("Timestamp Unix = %u\n", ts_unix_sec));
        } else {
            APP_TRACE_INFO(("Get TS Unix error\n"));
          }
    #if defined(CURRENT_UNIX_TS)
        valid = Clk_SetTS_Unix(CURRENT_UNIX_TS);
        if (valid != DEF_OK) {
            APP_TRACE_INFO(("Clk_SetTS_Unix error\n"));
            return -1;
        }
    #endif
     initialized = 1;
    }

    #if defined(WOLFSSL_WOLFCRYPT_TEST)
        wolfcrypt_test(NULL);
    #endif
    #if defined(WOLFSSL_BENCHMARK_TEST)
        benchmark_test(NULL);
    #endif
    #if defined(WOLFSSL_CLIENT_TEST)
        wolfssl_client_test();
    #endif
    #if defined(WOLFSSL_SERVER_TEST)
        wolfssl_server_test();
    #endif

    return 0;
}
