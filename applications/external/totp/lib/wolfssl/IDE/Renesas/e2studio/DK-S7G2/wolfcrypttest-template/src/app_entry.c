/* app_entry.c
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




#include <app.h>
#include "stdio.h"
extern void initialise_monitor_handles(void);

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/random.h>
#include "wolfcrypt/test/test.h"

typedef struct func_args {
    int argc;
    char** argv;
    int return_code;
} func_args;

void app_entry(void)
{
    func_args args;

    args.argc = 0;
    args.argv = NULL;
    args.return_code = 0;

    initialise_monitor_handles();
    wolfCrypt_Init();

#if 1
    /* sanity check on RNG */
    printf("Doing quick sanity check on RNG\n");
    {
        int i;
        for (i = 0; i < 10; i++) {
            int j, ret;
            WC_RNG rng;
            unsigned char buffer[20] = {0};

            wc_InitRng(&rng);
            ret = wc_RNG_GenerateBlock(&rng, buffer, 20);
            if (ret != 0) {
                printf("Error generating random block\n");
            }
            for (j = 0; j < 20; j++) {
                printf("%02X", buffer[j]);
            }
            printf("\n");
            wc_FreeRng(&rng);
        }
    }
    printf("\n");
#endif

    wolfcrypt_test(&args);
    wolfCrypt_Cleanup();
    printf("done with wolfcrypt test, ret = %d\n", args.return_code);
}
