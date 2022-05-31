/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

#include <avr/wdt.h>

/* Type definitions. */
#define portCHAR                    char
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    long
#define portSHORT                   int

typedef uint8_t                     StackType_t;
typedef int8_t                      BaseType_t;
typedef uint8_t                     UBaseType_t;

#if configUSE_16_BIT_TICKS == 1
    typedef uint16_t                TickType_t;
    #define portMAX_DELAY           ( TickType_t ) 0xffff
#else
    typedef uint32_t                TickType_t;
    #define portMAX_DELAY           ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

/* Critical section management. */

#define portENTER_CRITICAL()        __asm__ __volatile__ (                          \
                                        "in __tmp_reg__, __SREG__"        "\n\t"    \
                                        "cli"                             "\n\t"    \
                                        "push __tmp_reg__"                "\n\t"    \
                                        ::: "memory"                                \
                                        )


#define portEXIT_CRITICAL()         __asm__ __volatile__ (                          \
                                        "pop __tmp_reg__"                 "\n\t"    \
                                        "out __SREG__, __tmp_reg__"       "\n\t"    \
                                        ::: "memory"                                \
                                        )


#define portDISABLE_INTERRUPTS()    __asm__ __volatile__ ( "cli" ::: "memory")
#define portENABLE_INTERRUPTS()     __asm__ __volatile__ ( "sei" ::: "memory")
/*-----------------------------------------------------------*/

/* Architecture specifics. */

/* System Tick  - Scheduler timer
 * Prefer to use the enhanced Watchdog Timer, but also Timer0 is ok.
 */

#if defined(WDIE) && defined(WDIF)              /* If Enhanced WDT with interrupt capability is available */

#define portUSE_WDTO                WDTO_15MS   /* use the Watchdog Timer for xTaskIncrementTick */

/* Watchdog period options:         WDTO_15MS
                                    WDTO_30MS
                                    WDTO_60MS
                                    WDTO_120MS
                                    WDTO_250MS
                                    WDTO_500MS
                                    WDTO_1S
                                    WDTO_2S
*/

#else

#define portUSE_TIMER0                          /* use the 8-bit Timer0 for xTaskIncrementTick */

#endif

#define portSTACK_GROWTH            ( -1 )

/* Timing for the scheduler.
 * Watchdog Timer is 128kHz nominal,
 * but 120 kHz at 5V DC and 25 degrees is actually more accurate,
 * from data sheet.
 */
#if defined( portUSE_WDTO )
#define portTICK_PERIOD_MS          ( (TickType_t) _BV( portUSE_WDTO + 4 ) )
#else
#define portTICK_PERIOD_MS          ( (TickType_t) 1000 / configTICK_RATE_HZ )
#endif

#define portBYTE_ALIGNMENT          1
#define portNOP()                   __asm__ __volatile__ ( "nop" );
/*-----------------------------------------------------------*/

/* Kernel utilities. */
extern void vPortYield( void )      __attribute__ ( ( naked ) );
#define portYIELD()                 vPortYield()

extern void vPortYieldFromISR( void )   __attribute__ ( ( naked ) );
#define portYIELD_FROM_ISR()            vPortYieldFromISR()
/*-----------------------------------------------------------*/

#if defined(__AVR_3_BYTE_PC__)
/* Task function macros as described on the FreeRTOS.org WEB site. */

/* Add .lowtext tag from the avr linker script avr6.x for ATmega2560 and ATmega2561
 * to make sure functions are loaded in low memory.
 */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters ) __attribute__ ((section (".lowtext")))
#else
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#endif

#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
