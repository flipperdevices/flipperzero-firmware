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

#include <predef.h>
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <random.h>
#include <init.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/benchmark/benchmark.h>

extern "C" {
void UserMain(void * pd);
}

const char * AppName="benchmark";

typedef struct func_args {
    int argc;
    char** argv;
    int return_code;
} func_args;

void UserMain(void * pd) {
    InitializeStack();
    GetDHCPAddressIfNecessary();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();


    init();
    iprintf("wolfcrypt benchmark Application started\n");
    iprintf("waiting for sufficient entropy before starting...\n\r");
    iprintf("looks like NetBurner is using uart/tcp to seed GetRandomX so ..."
            " input enough uart characters.\n\r");
    {
        BYTE b;
        do {
            b = GetRandomByte();
            iprintf(".");
        } while (!RandomValid());
        iprintf("\n\r");
        (void)b;
    }

    /* run wolfCrypt benchmarks */
    {
        func_args args;
        args.argc = 0;
        args.argv = NULL;

        benchmark_test(&args);
    }
    while (1) {
        OSTimeDly(TICKS_PER_SECOND);
    }
}
