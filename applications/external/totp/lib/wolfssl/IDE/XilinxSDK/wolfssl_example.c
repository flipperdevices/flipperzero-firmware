/* wolfssl_example.c
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

#include "xil_printf.h"
#include "xrtcpsu.h"

#ifdef FREERTOS
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif

#include "wolfssl/wolfcrypt/settings.h"
#include "wolfssl/wolfcrypt/wc_port.h"
#include "wolfssl/wolfcrypt/error-crypt.h"
#include "wolfssl/wolfcrypt/logging.h"
#include "wolfcrypt/test/test.h"
#include "wolfcrypt/benchmark/benchmark.h"

/*****************************************************************************
 * Configuration
 ****************************************************************************/

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
typedef struct func_args
{
    int argc;
    char **argv;
    int return_code;
} func_args;

const char menu1[] = "\n"
        "\tb. WolfCrypt Benchmark\n"
        "\tt. WolfCrypt Test\n"
        "\n"
        "\tc. Command mode\n";

/*****************************************************************************
 * Private functions
 ****************************************************************************/
#if !defined(WOLFSSL_XILINX_CRYPT_VERSAL)
/* Test RNG Seed Function */
/* TODO: Must provide real seed to RNG */
unsigned char my_rng_seed_gen(void)
{
    static unsigned int kTestSeed = 1;
    return kTestSeed++;
}
#endif

/*****************************************************************************
 * Public functions
 ****************************************************************************/
#ifdef FREERTOS

static void wolfssl_task( void *pvParameters );
static TaskHandle_t xWolfsslTask;

int main( void )
{
    xil_printf("\nStarting up FreeRTOS\n");

    xTaskCreate(wolfssl_task, /* The function that implements the task. */
                (const char*) "Tx", /* Text name for the task, provided to assist debugging only. */
                configMINIMAL_STACK_SIZE, /* The stack allocated to the task. */
                NULL, /* The task parameter is not used, so set to NULL. */
                tskIDLE_PRIORITY, /* The task runs at the idle priority. */
                &xWolfsslTask);

    /* Start the task. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
     will never be reached.  If the following line does execute, then there was
     insufficient FreeRTOS heap memory available for the idle and/or timer tasks
     to be created.  See the memory management section on the FreeRTOS web site
     for more details. */
    for (;;)
        ;
}

static void wolfssl_task( void *pvParameters )
#else
int main()
#endif
{
    uint8_t cmd[128], c;
    char *arg[sizeof(cmd)/2 + 1];
    size_t cmdlen, argnum, n;
    func_args args;
    int (*func)(int argc, char** argv);
    int command_mode = 0;

#ifdef DEBUG_WOLFSSL
    wolfSSL_Debugging_ON();
#endif

    /* initialize wolfSSL */
    wolfCrypt_Init();

	while (1) {
        memset(&args, 0, sizeof(args));
        args.return_code = NOT_COMPILED_IN; /* default */

        if (!command_mode) {
            xil_printf("\n\t\t\t\tMENU\n");
            xil_printf(menu1);
            xil_printf("Please select one of the above options:\n");
            xil_printf("Both 'b' and 't' allow arguments as if you'd call the regular shell version.\n");
        }


        cmdlen = 0;
        do {
            c = inbyte();
            cmd[cmdlen] = c;
            if (!command_mode) outbyte(c);
            cmdlen++;
            cmdlen %= sizeof(cmd);
        } while (c != '\n' && c != '\r');

        if (cmdlen > 2) {
            outbyte('\n');
            /* Poor man's argv parser */
            XMEMSET(arg, 0, sizeof(arg));
            if (cmd[1] == ' ') {
                if (cmd[0] == 'b') {
                    arg[0] = "wolfcrypt_benchmark";
                    func = wolfcrypt_benchmark_main;
                } else if (cmd[0] == 't') {
                    arg[0] = "wolfcrypt_test";
                    func = wolfcrypt_test_main;
                }
                if (arg[0] != NULL) {
                    argnum = 1;
                    for (n = 2; n < cmdlen; ++n) {
                        switch (cmd[n]) {
                            case ' ':
                            case '\t':
                            case '\r':
                            case '\n':
                                cmd[n] = '\0';
                                if (arg[argnum] != NULL)
                                    argnum++;
                                break;
                            default:
                                if (arg[argnum] == NULL)
                                    arg[argnum] = (char*)&cmd[n];
                                break;
                        }
                    }
                    func(argnum, arg);
                }
            }
        } else {
            switch (cmd[0]) {
            case 't':
                xil_printf("Running wolfCrypt Tests...\n");
            #ifndef NO_CRYPT_TEST
                args.return_code = 0;
                wolfcrypt_test(&args);
            #endif
                xil_printf("Crypt Test: Return code %d\n", args.return_code);
                break;

            case 'b':
                xil_printf("Running wolfCrypt Benchmarks...\n");
            #ifndef NO_CRYPT_BENCHMARK
                args.return_code = 0;
                benchmark_test(&args);
            #endif
                xil_printf("Benchmark Test: Return code %d\n", args.return_code);
                break;

            case 'c':
                if (!command_mode) xil_printf("Entering command mode, enter 'c<enter>' to return to manual mode\n");
                command_mode ^= 1;
                break;

            default:
                if (!command_mode) xil_printf("\nSelection out of range\n");
                break;
            }
        }
    }

    wolfCrypt_Cleanup();
#ifndef FREERTOS
    return 0;
#endif
}
