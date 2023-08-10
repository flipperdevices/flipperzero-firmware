/* wolfssl_netos_custom.c
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
#include <stdarg.h>
#include <tx_api.h>
#include <entropy.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/fips_test.h>

int dc_log_printf(char* format, ...);

#if BSP_SIGMA == 0
int dc_log_printf(char* format, ...)
{
    va_list args;

    va_start(args, (format));

    fflush(stdout);
    vprintf(format, args);
    fflush(stdout);

    va_end(args);

    return 0;
}
#endif

unsigned char get_byte_from_pool(void)
{
     unsigned char out;
     float density;

    /* Wait until pool has at least one byte */
    /* TODO: improve this */
     while (ent_get_byte_count() == 0)
          tx_thread_sleep(1);

    /* Stop gathering entropy to avoid race conditions */
     ent_set_status(0);

    /* Pop a single byte from the pool and continue gathering entropy */
     ent_pop(&out, &density);
     ent_set_status(1);

     return out;
}

int my_rng_generate_seed(unsigned char* output, int sz)
{
    word32 i;
    srand(get_byte_from_pool());

    for (i = 0; i < sz; i++) {
        output[i] = (unsigned char) rand();
        srand(get_byte_from_pool());
    }

    return 0;
}

static void appFipsCb(int ok, int err, const char* hash)
{
    dc_log_printf("in appFipsCb Fips callback, ok = %d, err = %d\n", ok, err);
    dc_log_printf("message = %s\n", wc_GetErrorString(err));
    dc_log_printf("hash = %s\n", hash);

    if (err == IN_CORE_FIPS_E) {
        dc_log_printf("In core integrity hash check failure, copy above hash\n");
        dc_log_printf("into verifyCore[] in fips_test.c and rebuild\n");
    }
}

void setAppFipsCb(void)
{
    wolfCrypt_SetCb_fips(appFipsCb);
}
