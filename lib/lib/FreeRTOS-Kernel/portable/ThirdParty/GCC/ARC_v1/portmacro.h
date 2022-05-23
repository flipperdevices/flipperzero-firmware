/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2020 Synopsys, Inc. or its affiliates.  All Rights Reserved.
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
    #include "embARC.h"

    #ifdef __cplusplus
        extern "C" {
    #endif

/* record stack high address for stack check */
    #ifndef configRECORD_STACK_HIGH_ADDRESS
        #define configRECORD_STACK_HIGH_ADDRESS    1
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

/* Type definitions. */
    #define portCHAR          char
    #define portFLOAT         float
    #define portDOUBLE        double
    #define portLONG          long
    #define portSHORT         short
    #define portSTACK_TYPE    unsigned int
    #define portBASE_TYPE     portLONG

    #ifndef Asm
        #define Asm           __asm__ volatile
    #endif

/*
 *  normal constants
 */
    #ifndef NULL
        #define NULL    0       /* invalid pointer */
    #endif /* NULL */

    #ifndef true
        #define true    1       /* true */
    #endif /* true */

    #ifndef false
        #define false    0      /* false */
    #endif /* false */

    typedef portSTACK_TYPE     StackType_t;
    typedef long               BaseType_t;
    typedef unsigned long      UBaseType_t;

    #if ( configUSE_16_BIT_TICKS == 1 )
        typedef uint16_t       TickType_t;
        #define portMAX_DELAY          ( TickType_t ) 0xffff
    #else
        typedef unsigned int   TickType_t;
        #define portMAX_DELAY          ( TickType_t ) 0xffffffffUL
    #endif

    #define portNO_CRITICAL_NESTING    ( ( uint32_t ) 0 )
    #define portSTACK_GROWTH           ( -1 )
    #define portTICK_PERIOD_MS         ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
    #define portBYTE_ALIGNMENT         8
    #define portNOP()               Asm( "nop_s" );
    #define IPM_ENABLE_ALL             1

    #define portYIELD_FROM_ISR()    vPortYieldFromIsr()
    #define portYIELD()             vPortYield()

/* Critical section management. */
    #define portDISABLE_INTERRUPTS() \
    {                                \
        arc_lock();                  \
    }                                \

    #define portENABLE_INTERRUPTS() \
    {                               \
        arc_unlock();               \
    }                               \

    extern volatile unsigned int ulCriticalNesting;

    #define portENTER_CRITICAL() \
    {                            \
        portDISABLE_INTERRUPTS() \
        ulCriticalNesting++;     \
    }


    #define portEXIT_CRITICAL()                                \
    {                                                          \
        if( ulCriticalNesting > portNO_CRITICAL_NESTING )      \
        {                                                      \
            ulCriticalNesting--;                               \
            if( ulCriticalNesting == portNO_CRITICAL_NESTING ) \
            {                                                  \
                portENABLE_INTERRUPTS()                        \
            }                                                  \
        }                                                      \
    }


    #define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
    #define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()              do {} while( 0 ) /* we use the timer */
    #define portALT_GET_RUN_TIME_COUNTER_VALUE( dest )            ( dest = xTickCount )

    void vPortYield( void );
    void vPortYieldFromIsr( void );

    #ifdef __cplusplus
}
    #endif

#endif /* PORTMACRO_H */
