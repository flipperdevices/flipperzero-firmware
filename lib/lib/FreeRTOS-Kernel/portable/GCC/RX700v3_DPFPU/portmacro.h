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

/* When the FIT configurator or the Smart Configurator is used, platform.h has to be 
 * used. */
    #ifndef configINCLUDE_PLATFORM_H_INSTEAD_OF_IODEFINE_H
        #define configINCLUDE_PLATFORM_H_INSTEAD_OF_IODEFINE_H 0
    #endif

/* If configUSE_TASK_DPFPU_SUPPORT is set to 1 (or undefined) then each task will
 * be created without a DPFPU context, and a task must call vTaskUsesDPFPU() before
 * making use of any DPFPU registers.  If configUSE_TASK_DPFPU_SUPPORT is set to 2 then
 * tasks are created with a DPFPU context by default, and calling vTaskUsesDPFPU() has
 * no effect.  If configUSE_TASK_DPFPU_SUPPORT is set to 0 then tasks never take care
 * of any DPFPU context (even if DPFPU registers are used). */
    #ifndef configUSE_TASK_DPFPU_SUPPORT
        #define configUSE_TASK_DPFPU_SUPPORT 1
    #endif

/*-----------------------------------------------------------*/

/* Type definitions - these are a bit legacy and not really used now, other than
 * portSTACK_TYPE and portBASE_TYPE. */
    #define portCHAR          char
    #define portFLOAT         float
    #define portDOUBLE        double
    #define portLONG          long
    #define portSHORT         short
    #define portSTACK_TYPE    uint32_t
    #define portBASE_TYPE     long

    typedef portSTACK_TYPE   StackType_t;
    typedef long             BaseType_t;
    typedef unsigned long    UBaseType_t;

    #if ( configUSE_16_BIT_TICKS == 1 )
        typedef uint16_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffff
    #else
        typedef uint32_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffffffffUL

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
 * not need to be guarded with a critical section. */
        #define portTICK_TYPE_IS_ATOMIC    1
    #endif

/*-----------------------------------------------------------*/

/* Hardware specifics. */
    #define portBYTE_ALIGNMENT    8     /* Could make four, according to manual. */
    #define portSTACK_GROWTH      -1
    #define portTICK_PERIOD_MS    ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
    #define portNOP()    __asm volatile ( "NOP" )

/* Yield equivalent to "*portITU_SWINTR = 0x01; ( void ) *portITU_SWINTR;"
 * where portITU_SWINTR is the location of the software interrupt register
 * (0x000872E0).  Don't rely on the assembler to select a register, so instead
 * save and restore clobbered registers manually. */
    #define portYIELD()           \
    __asm volatile                \
    (                             \
        "PUSH.L	R10					\n"\
        "MOV.L	#0x872E0, R10		\n"\
        "MOV.B	#0x1, [R10]			\n"\
        "CMP	[R10].UB, R10		\n"\
        "POP    R10					\n"\
        :::"cc"						\
    )

    #define portYIELD_FROM_ISR( x )                           do { if( ( x ) != pdFALSE ) portYIELD(); } while( 0 )

/* Workaround to reduce errors/warnings caused by e2 studio CDT's INDEXER and CODAN. */
    #ifdef __CDT_PARSER__
    #ifndef __asm
    #define __asm asm
    #endif
    #ifndef __attribute__
    #define __attribute__( ... )
    #endif
    #endif

/* These macros should not be called directly, but through the
 * taskENTER_CRITICAL() and taskEXIT_CRITICAL() macros.  An extra check is
 * performed if configASSERT() is defined to ensure an assertion handler does not
 * inadvertently attempt to lower the IPL when the call to assert was triggered
 * because the IPL value was found to be above	configMAX_SYSCALL_INTERRUPT_PRIORITY
 * when an ISR safe FreeRTOS API function was executed.  ISR safe FreeRTOS API
 * functions are those that end in FromISR.  FreeRTOS maintains a separate
 * interrupt API to ensure API function and interrupt entry is as fast and as
 * simple as possible. */
    #define portENABLE_INTERRUPTS()                           __asm volatile ( "MVTIPL	#0")
    #ifdef configASSERT
        #define portASSERT_IF_INTERRUPT_PRIORITY_INVALID()    configASSERT( ( ulPortGetIPL() <= configMAX_SYSCALL_INTERRUPT_PRIORITY ) )
        #define portDISABLE_INTERRUPTS()                      if( ulPortGetIPL() < configMAX_SYSCALL_INTERRUPT_PRIORITY ) __asm volatile ( "MVTIPL	%0"::"i" ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) )
    #else
        #define portDISABLE_INTERRUPTS()                      __asm volatile ( "MVTIPL	%0"::"i" ( configMAX_SYSCALL_INTERRUPT_PRIORITY ) )
    #endif

/* Critical nesting counts are stored in the TCB. */
    #define portCRITICAL_NESTING_IN_TCB    ( 1 )

/* The critical nesting functions defined within tasks.c. */
    extern void vTaskEnterCritical( void );
    extern void vTaskExitCritical( void );
    #define portENTER_CRITICAL()    vTaskEnterCritical()
    #define portEXIT_CRITICAL()     vTaskExitCritical()

/* As this port allows interrupt nesting... */
    uint32_t ulPortGetIPL( void ) __attribute__( ( naked ) );
    void vPortSetIPL( uint32_t ulNewIPL ) __attribute__( ( naked ) );
    #define portSET_INTERRUPT_MASK_FROM_ISR()                              ulPortGetIPL(); portDISABLE_INTERRUPTS()
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus )    vPortSetIPL( uxSavedInterruptStatus )

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
    #define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
    #define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

/*-----------------------------------------------------------*/

/* If configUSE_TASK_DPFPU_SUPPORT is set to 1 (or left undefined) then tasks are
 * created without a DPFPU context and must call vPortTaskUsesDPFPU() to give
 * themselves a DPFPU context before using any DPFPU instructions.  If
 * configUSE_TASK_DPFPU_SUPPORT is set to 2 then all tasks will have a DPFPU context
 * by default. */
    #if( configUSE_TASK_DPFPU_SUPPORT == 1 )
        void vPortTaskUsesDPFPU( void );
    #else
/* Each task has a DPFPU context already, so define this function away to
 * nothing to prevent it being called accidentally. */
        #define vPortTaskUsesDPFPU()
    #endif
    #define portTASK_USES_DPFPU() vPortTaskUsesDPFPU()

/* Definition to allow compatibility with existing FreeRTOS Demo using flop.c. */
    #define portTASK_USES_FLOATING_POINT() vPortTaskUsesDPFPU()

    #ifdef __cplusplus
        }
    #endif

#endif /* PORTMACRO_H */
