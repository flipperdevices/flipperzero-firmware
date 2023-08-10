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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "wolfssl/wolfcrypt/settings.h"

#include "rl_net.h"                      /* Network definitions                */
#include <time.h>

#if defined(WOLFSSL_CMSIS_RTOS)
#include "cmsis_os.h"
#elif defined(WOLFSSL_CMSIS_RTOSv2)
#include "cmsis_os2.h"
#endif

/* Dummy definition for test RTC */
#define RTC_YEAR 2019
#if defined(STM32F7xx)
#include "stm32f7xx_hal.h"
#elif defined(STM32F4xx)
#include "stm32f4xx_hal.h"
#elif defined(STM32F2xx)
#include "stm32f2xx_hal.h"
#endif

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//   <h>Server parameter
//   ====================

//   <s.6>Port
//   <i> Default: "11111"
#define SERVER_PORT "11111"
//   </h>

//   <h>Protocol
//   ====================

//   <o>SSL/TLS Version<0=> SSL3 <1=> TLS1.0 <2=> TLS1.1 <3=> TLS1.2 <4=> TLS1.3
#define TLS_VER 3

//   <s.2>Other option
#define OTHER_OPTIONS ""
//   </h>

//   <h>RTC: for validate certificate date
//    <o>Year <1970-2099>
#define RTC_YEAR  2023
#define RTC_MONTH 1
#define RTC_DAY   1
//    </h>

//------------- <<< end of configuration section >>> -----------------------

#warning "write MPU specific Set ups\n"
static void SystemClock_Config (void) {

}

static void MPU_Config (void) {

}

static void CPU_CACHE_Enable (void) {

}

/*-----------------------------------------------------------------------------
 *        Initialize a Flash Memory Card
 *----------------------------------------------------------------------------*/
#if !defined(NO_FILESYSTEM)
#include "rl_fs.h"                      /* FileSystem definitions             */

static void init_filesystem(void)
{
    int32_t retv;

    retv = finit ("M0:");
    if (retv == fsOK) {
        retv = fmount ("M0:");
        if (retv == fsOK) {
            printf ("Drive M0 ready!\n");
        }
        else {
            printf ("Drive M0 mount failed(%d)!\n", retv);
        }
    }
    else {
        printf ("Drive M0 initialization failed!\n");
    }
}
#endif

#if defined(WOLFSSL_CMSIS_RTOS) || defined(WOLFSSL_CMSIS_RTOSv2)

#if defined(WOLFSSL_CMSIS_RTOS)
extern uint32_t os_time;
#endif

uint32_t HAL_GetTick(void)
{
#if defined(WOLFSSL_CMSIS_RTOS)
    return os_time;
#elif defined(WOLFSSL_CMSIS_RTOSv2)
    return osKernelGetTickCount();
#endif
}

double current_time(int reset)
{
#if defined(WOLFSSL_CMSIS_RTOS)
    return (double)os_time / 1000.0;
#elif defined(WOLFSSL_CMSIS_RTOSv2)
    return (double)osKernelGetTickCount() / 1000.0;
#endif
}

#else

#include <stdint.h>
#define DWT                 ((DWT_Type       *)     (0xE0001000UL)     )
typedef struct
{
    uint32_t CTRL;       /*!< Offset: 0x000 (R/W)  Control Register           */
    uint32_t CYCCNT;     /*!< Offset: 0x004 (R/W)  Cycle Count Register       */
} DWT_Type;

extern uint32_t SystemCoreClock;

double current_time(int reset)
{
    if (reset)
        DWT->CYCCNT = 0;
    return ((double)DWT->CYCCNT / SystemCoreClock);
}
#endif

static time_t epochTime;
time_t time(time_t *t)
{
    return epochTime;
}

void setTime(time_t t)
{
    epochTime = t;
}


extern void server_test(void const *arg);

#if defined(WOLFSSL_CMSIS_RTOSv2)
void app_main(void *arg)
#else
void app_main(void const*arg)
#endif
{
    if (netInitialize() == netOK)
        server_test(arg);
    else
        printf("ERROR: netInitialize\n");
}

#if defined(WOLFSSL_CMSIS_RTOS)
osThreadDef(app_main, osPriorityLow, 1, 32 * 1024);
#endif

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
#include <stdio.h>
typedef struct func_args {
    int    argc;
    char** argv;
} func_args;

int myoptind = 0;
char *myoptarg = NULL;

int main(void)
{
    static char *argv[] =
        {   "server",  "-p", SERVER_PORT,
                       "-v",  " ",  OTHER_OPTIONS };
    static   func_args args  =
        {  sizeof(argv)/sizeof(*argv[0]), argv };

    char *verStr[] = { "SSL3", "TLS1.0", "TLS1.1", "TLS1.2", "TLS1.3"};
    #define VERSIZE 2
    char ver[VERSIZE];

    MPU_Config();                             /* Configure the MPU              */
    CPU_CACHE_Enable();                       /* Enable the CPU Cache           */
    HAL_Init();                               /* Initialize the HAL Library     */
    SystemClock_Config();                     /* Configure the System Clock     */

#if !defined(NO_FILESYSTEM)
    init_filesystem ();
#endif

#if defined(WOLFSSL_CMSIS_RTOSv2)
    osKernelInitialize();
#endif

#if defined(DEBUG_WOLFSSL)
    printf("Turning ON Debug message\n");
    wolfSSL_Debugging_ON();
#endif

    snprintf(ver, VERSIZE, "%d", TLS_VER);
    argv[4] = ver;

    printf("SSL/TLS Server\n ");
    printf("    Server Port: %s\n    Version: %s\n", argv[2], verStr[TLS_VER]);
    printf("    Other options: %s\n", OTHER_OPTIONS);
    setTime((RTC_YEAR-1970)*365*24*60*60 +
             RTC_MONTH*30*24*60*60 +
             RTC_DAY*24*60*60);

#if defined(WOLFSSL_CMSIS_RTOS)
    osThreadCreate(osThread(app_main), (void *)&args);
#elif defined(WOLFSSL_CMSIS_RTOSv2)
    osThreadNew(app_main, (void *)&args, NULL);
#endif
    osKernelStart();
}
