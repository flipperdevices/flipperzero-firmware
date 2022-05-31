/*
 *  FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
 *  All rights reserved
 *
 *  SPDX-License-Identifier: GPL-2.0 WITH freertos-exception-2.0
 *
 *  VISIT https://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.
 *
 ***************************************************************************
 *                                                                       *
 *    FreeRTOS provides completely free yet professionally developed,    *
 *    robust, strictly quality controlled, supported, and cross          *
 *    platform software that has become a de facto standard.             *
 *                                                                       *
 *    Help yourself get started quickly and support the FreeRTOS         *
 *    project by purchasing a FreeRTOS tutorial book, reference          *
 *    manual, or both from: https://www.FreeRTOS.org/Documentation       *
 *                                                                       *
 *    Thank you!                                                         *
 *                                                                       *
 ***************************************************************************
 *
 *  This file is part of the FreeRTOS distribution.
 *
 *  FreeRTOS is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by the
 *  Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.
 *
 *  >>!   NOTE: The modification to the GPL is included to allow you to     !<<
 *  >>!   distribute a combined work that includes FreeRTOS without being   !<<
 *  >>!   obliged to provide the source code for proprietary components     !<<
 *  >>!   outside of the FreeRTOS kernel.                                   !<<
 *
 *  FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  Full license text is available from the following
 *  link: https://www.FreeRTOS.org/a00114.html
 *
 *
 ***************************************************************************
 *                                                                       *
 *    Having a problem?  Start by reading the FAQ "My application does   *
 *    not run, what could be wrong?"                                     *
 *                                                                       *
 *    https://www.FreeRTOS.org/FAQHelp.html                              *
 *                                                                       *
 ***************************************************************************
 *
 *  https://www.FreeRTOS.org - Documentation, books, training, latest versions,
 *  license and Real Time Engineers Ltd. contact details.
 *
 *  https://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
 *  including FreeRTOS+Trace - an indispensable productivity tool, a DOS
 *  compatible FAT file system, and our tiny thread aware UDP/IP stack.
 *
 *  https://www.highintegritysystems.com/openrtos/ - Real Time Engineers ltd
 *  license FreeRTOS to High Integrity Systems to sell under the OpenRTOS brand.
 *  Low cost OpenRTOS licenses offer ticketed support, indemnification
 *  and middleware.
 *
 *  https://www.highintegritysystems.com/safertos/ - High Integrity Systems
 *  also provide a safety engineered and independently SIL3 certified version
 *  for use in safety and mission critical applications that require
 *  provable dependability.
 *
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#ifndef __ASSEMBLER__

    #include <stdint.h>

    #include <xtensa/hal.h>
    #include <xtensa/config/core.h>
    #include <xtensa/config/system.h> /* required for XSHAL_CLIB */
    #include <xtensa/xtruntime.h>
    #include "esp_timer.h"            /* required for FreeRTOS run time stats */
    #include "esp_system.h"
    #include "esp_idf_version.h"


    #include <esp_heap_caps.h>
    #include "soc/soc_memory_layout.h"
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0))
    #include "soc/compare_set.h"
#endif /* ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0) */

/*#include "xtensa_context.h" */

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

    #define portCHAR          int8_t
    #define portFLOAT         float
    #define portDOUBLE        double
    #define portLONG          int32_t
    #define portSHORT         int16_t
    #define portSTACK_TYPE    uint8_t
    #define portBASE_TYPE     int

    typedef portSTACK_TYPE           StackType_t;
    typedef portBASE_TYPE            BaseType_t;
    typedef unsigned portBASE_TYPE   UBaseType_t;

    #if ( configUSE_16_BIT_TICKS == 1 )
        typedef uint16_t             TickType_t;
        #define portMAX_DELAY    ( TickType_t ) 0xffff
    #else
        typedef uint32_t             TickType_t;
        #define portMAX_DELAY    ( TickType_t ) 0xffffffffUL
    #endif
/*-----------------------------------------------------------*/

/* portbenchmark */
    #include "portbenchmark.h"

    #include "sdkconfig.h"
    #include "esp_attr.h"

/* "mux" data structure (spinlock) */
    typedef struct
    {
        /* owner field values:
         * 0                - Uninitialized (invalid)
         * portMUX_FREE_VAL - Mux is free, can be locked by either CPU
         * CORE_ID_REGVAL_PRO / CORE_ID_REGVAL_APP - Mux is locked to the particular core
         *
         * Any value other than portMUX_FREE_VAL, CORE_ID_REGVAL_PRO, CORE_ID_REGVAL_APP indicates corruption
         */
        uint32_t owner;

        /* count field:
         * If mux is unlocked, count should be zero.
         * If mux is locked, count is non-zero & represents the number of recursive locks on the mux.
         */
        uint32_t count;
        #ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
            const char * lastLockedFn;
            int lastLockedLine;
        #endif
    } portMUX_TYPE;

    #define portMUX_FREE_VAL      0xB33FFFFF

/* Special constants for vPortCPUAcquireMutexTimeout() */
    #define portMUX_NO_TIMEOUT    ( -1 ) /* When passed for 'timeout_cycles', spin forever if necessary */
    #define portMUX_TRY_LOCK      0      /* Try to acquire the spinlock a single time only */

/* Keep this in sync with the portMUX_TYPE struct definition please. */
    #ifndef CONFIG_FREERTOS_PORTMUX_DEBUG
        #define portMUX_INITIALIZER_UNLOCKED \
    {                                        \
        .owner = portMUX_FREE_VAL,           \
        .count = 0,                          \
    }
    #else
        #define portMUX_INITIALIZER_UNLOCKED \
    {                                        \
        .owner = portMUX_FREE_VAL,           \
        .count = 0,                          \
        .lastLockedFn = "(never locked)",    \
        .lastLockedLine = -1                 \
    }
    #endif /* ifndef CONFIG_FREERTOS_PORTMUX_DEBUG */


    #define portASSERT_IF_IN_ISR()    vPortAssertIfInISR()
    void vPortAssertIfInISR();

    #define portCRITICAL_NESTING_IN_TCB    1

/*
 * Modifications to portENTER_CRITICAL.
 *
 * For an introduction, see "Critical Sections & Disabling Interrupts" in docs/api-guides/freertos-smp.rst
 *
 * The original portENTER_CRITICAL only disabled the ISRs. This is enough for single-CPU operation: by
 * disabling the interrupts, there is no task switch so no other tasks can meddle in the data, and because
 * interrupts are disabled, ISRs can't corrupt data structures either.
 *
 * For multiprocessing, things get a bit more hairy. First of all, disabling the interrupts doesn't stop
 * the tasks or ISRs on the other processors meddling with our CPU. For tasks, this is solved by adding
 * a spinlock to the portENTER_CRITICAL macro. A task running on the other CPU accessing the same data will
 * spinlock in the portENTER_CRITICAL code until the first CPU is done.
 *
 * For ISRs, we now also need muxes: while portENTER_CRITICAL disabling interrupts will stop ISRs on the same
 * CPU from meddling with the data, it does not stop interrupts on the other cores from interfering with the
 * data. For this, we also use a spinlock in the routines called by the ISR, but these spinlocks
 * do not disable the interrupts (because they already are).
 *
 * This all assumes that interrupts are either entirely disabled or enabled. Interrupt priority levels
 * will break this scheme.
 *
 * Remark: For the ESP32, portENTER_CRITICAL and portENTER_CRITICAL_ISR both alias vTaskEnterCritical, meaning
 * that either function can be called both from ISR as well as task context. This is not standard FreeRTOS
 * behaviour; please keep this in mind if you need any compatibility with other FreeRTOS implementations.
 */
    void vPortCPUInitializeMutex( portMUX_TYPE * mux );
    #ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
        #error CONFIG_FREERTOS_PORTMUX_DEBUG not supported in Amazon FreeRTOS
    #endif

    void vTaskExitCritical();
    void vTaskEnterCritical();
    static inline void vPortConsumeSpinlockArg( int unused,
                                                ... )
    {
    }

/** @brief Acquire a portmux spinlock with a timeout
 *
 * @param mux Pointer to portmux to acquire.
 * @param timeout_cycles Timeout to spin, in CPU cycles. Pass portMUX_NO_TIMEOUT to wait forever,
 * portMUX_TRY_LOCK to try a single time to acquire the lock.
 *
 * @return true if mutex is successfully acquired, false on timeout.
 */
    bool vPortCPUAcquireMutexTimeout( portMUX_TYPE * mux,
                                      int timeout_cycles );
    void vPortCPUReleaseMutex( portMUX_TYPE * mux );

    #define portENTER_CRITICAL( ... )        do { vTaskEnterCritical(); vPortConsumeSpinlockArg( 0, ## __VA_ARGS__ ); } while( 0 )
    #define portEXIT_CRITICAL( ... )         do { vTaskExitCritical(); vPortConsumeSpinlockArg( 0, ## __VA_ARGS__ ); } while( 0 )


    #define portENTER_CRITICAL_ISR( mux )    vPortCPUAcquireMutexTimeout( mux, portMUX_NO_TIMEOUT )
    #define portEXIT_CRITICAL_ISR( mux )     vPortCPUReleaseMutex( mux )

    #define portENTER_CRITICAL_SAFE( mux ) \
    do {                                   \
        if( xPortInIsrContext() ) {        \
            portENTER_CRITICAL_ISR( mux ); \
        }                                  \
        else {                             \
            portENTER_CRITICAL( mux );     \
        }                                  \
    } while( 0 )

    #define portEXIT_CRITICAL_SAFE( mux ) \
    do {                                  \
        if( xPortInIsrContext() ) {       \
            portEXIT_CRITICAL_ISR( mux ); \
        }                                 \
        else {                            \
            portEXIT_CRITICAL( mux );     \
        }                                 \
    } while( 0 )


/* Critical section management. NW-TODO: replace XTOS_SET_INTLEVEL with more efficient version, if any? */
/* These cannot be nested. They should be used with a lot of care and cannot be called from interrupt level. */
/* */
/* Only applies to one CPU. See notes above & below for reasons not to use these. */
    #define portDISABLE_INTERRUPTS()    do { XTOS_SET_INTLEVEL( XCHAL_EXCM_LEVEL ); portbenchmarkINTERRUPT_DISABLE(); } while( 0 )
    #define portENABLE_INTERRUPTS()     do { portbenchmarkINTERRUPT_RESTORE( 0 ); XTOS_SET_INTLEVEL( 0 ); } while( 0 )

/* Cleaner solution allows nested interrupts disabling and restoring via local registers or stack. */
/* They can be called from interrupts too. */
/* WARNING: Only applies to current CPU. See notes above. */
    static inline unsigned portENTER_CRITICAL_NESTED()
    {
        unsigned state = XTOS_SET_INTLEVEL( XCHAL_EXCM_LEVEL );

        portbenchmarkINTERRUPT_DISABLE();
        return state;
    }
    #define portEXIT_CRITICAL_NESTED( state )             do { portbenchmarkINTERRUPT_RESTORE( state ); XTOS_RESTORE_JUST_INTLEVEL( state ); } while( 0 )

/* These FreeRTOS versions are similar to the nested versions above */
    #define portSET_INTERRUPT_MASK_FROM_ISR()             portENTER_CRITICAL_NESTED()
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( state )    portEXIT_CRITICAL_NESTED( state )

/*Because the ROM routines don't necessarily handle a stack in external RAM correctly, we force */
/*the stack memory to always be internal. */
    #define portTcbMemoryCaps (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)
    #define portStackMemoryCaps (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)

    #define pvPortMallocTcbMem(size) heap_caps_malloc(size, portTcbMemoryCaps)
    #define pvPortMallocStackMem(size)  heap_caps_malloc(size, portStackMemoryCaps)

/*xTaskCreateStatic uses these functions to check incoming memory. */
    #define portVALID_TCB_MEM( ptr )          ( esp_ptr_internal( ptr ) && esp_ptr_byte_accessible( ptr ) )
    #ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
        #define portVALID_STACK_MEM( ptr )    esp_ptr_byte_accessible( ptr )
    #else
        #define portVALID_STACK_MEM( ptr )    ( esp_ptr_internal( ptr ) && esp_ptr_byte_accessible( ptr ) )
    #endif

/*
 * Wrapper for the Xtensa compare-and-set instruction. This subroutine will atomically compare
 * *addr to 'compare'. If *addr == compare, *addr is set to *set. *set is updated with the previous
 * value of *addr (either 'compare' or some other value.)
 *
 * Warning: From the ISA docs: in some (unspecified) cases, the s32c1i instruction may return the
 * *bitwise inverse* of the old mem if the mem wasn't written. This doesn't seem to happen on the
 * ESP32 (portMUX assertions would fail).
 */
        static inline void uxPortCompareSet( volatile uint32_t * addr,
                                             uint32_t compare,
                                             uint32_t * set )
        {
            #if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0))
            __asm__ __volatile__ (
                "WSR       %2,SCOMPARE1 \n"
                "S32C1I     %0, %1, 0   \n"
                : "=r" ( *set )
                : "r" ( addr ), "r" ( compare ), "0" ( *set )
                );
            #else
            #if ( XCHAL_HAVE_S32C1I > 0 )
                __asm__ __volatile__ (
                    "WSR 	    %2,SCOMPARE1 \n"
                    "S32C1I     %0, %1, 0	 \n"
                    : "=r" ( *set )
                    : "r" ( addr ), "r" ( compare ), "0" ( *set )
                    );
            #else
                /* No S32C1I, so do this by disabling and re-enabling interrupts (slower) */
                uint32_t intlevel, old_value;
                __asm__ __volatile__ ( "rsil %0, " XTSTR( XCHAL_EXCM_LEVEL ) "\n"
                                       : "=r" ( intlevel ) );

                old_value = *addr;

                if( old_value == compare )
                {
                    *addr = *set;
                }

                __asm__ __volatile__ ( "memw \n"
                                       "wsr %0, ps\n"
                                       : : "r" ( intlevel ) );

                *set = old_value;
            #endif /* if ( XCHAL_HAVE_S32C1I > 0 ) */
            #endif /* #if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)) */
        }

        #if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0))
        void uxPortCompareSetExtram( volatile uint32_t * addr,
                                     uint32_t compare,
                                     uint32_t * set );
        #else
        static inline void uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
        {
        #if defined(CONFIG_ESP32_SPIRAM_SUPPORT)
            compare_and_set_extram(addr, compare, set);
        #endif
        }
        #endif

/*-----------------------------------------------------------*/

/* Architecture specifics. */
    #define portSTACK_GROWTH      ( -1 )
    #define portTICK_PERIOD_MS    ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
    #define portBYTE_ALIGNMENT    4
    #define portNOP()    XT_NOP()
/*-----------------------------------------------------------*/

/* Fine resolution time */
    #define portGET_RUN_TIME_COUNTER_VALUE()    xthal_get_ccount()
/*ccount or esp_timer are initialized elsewhere */
    #define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

    #ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
/* Coarse resolution time (us) */
        #define portALT_GET_RUN_TIME_COUNTER_VALUE( x )    x = ( uint32_t ) esp_timer_get_time()
    #endif



/* Kernel utilities. */
    void vPortYield( void );
    void _frxt_setup_switch( void );
    #define portYIELD()             vPortYield()
    #define portYIELD_FROM_ISR()    { traceISR_EXIT_TO_SCHEDULER(); _frxt_setup_switch(); }

    static inline uint32_t xPortGetCoreID();

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
    #define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
    #define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

/* When coprocessors are defined, we to maintain a pointer to coprocessors area. */
/* We currently use a hack: redefine field xMPU_SETTINGS in TCB block as a structure that can hold: */
/* MPU wrappers, coprocessor area pointer, trace code structure, and more if needed. */
/* The field is normally used for memory protection. FreeRTOS should create another general purpose field. */
    typedef struct
    {
        #if XCHAL_CP_NUM > 0
            volatile StackType_t * coproc_area; /* Pointer to coprocessor save area; MUST BE FIRST */
        #endif

        #if portUSING_MPU_WRAPPERS
            /* Define here mpu_settings, which is port dependent */
            int mpu_setting; /* Just a dummy example here; MPU not ported to Xtensa yet */
        #endif

        #if configUSE_TRACE_FACILITY_2
            struct
            {
                /* Cf. porttraceStamp() */
                int taskstamp;      /* Stamp from inside task to see where we are */
                int taskstampcount; /* A counter usually incremented when we restart the task's loop */
            } porttrace;
        #endif
    } xMPU_SETTINGS;

/* Main hack to use MPU_wrappers even when no MPU is defined (warning: mpu_setting should not be accessed; otherwise move this above xMPU_SETTINGS) */
    #if ( XCHAL_CP_NUM > 0 || configUSE_TRACE_FACILITY_2 ) && !portUSING_MPU_WRAPPERS /* If MPU wrappers not used, we still need to allocate coproc area */
        #undef portUSING_MPU_WRAPPERS
        #define portUSING_MPU_WRAPPERS    1                                           /* Enable it to allocate coproc area */
        #define MPU_WRAPPERS_H                                                        /* Override mpu_wrapper.h to disable unwanted code */
        #define PRIVILEGED_FUNCTION
        #define PRIVILEGED_DATA
    #endif

    void vApplicationSleep( TickType_t xExpectedIdleTime );
    void vPortSetStackWatchpoint( void* pxStackStart );

    #define portSUPPRESS_TICKS_AND_SLEEP( idleTime )    vApplicationSleep( idleTime )

    /*-----------------------------------------------------------*/

    #if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0))
    /* Architecture specific optimisations. */

    #if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

    /* Check the configuration. */
    #if( configMAX_PRIORITIES > 32 )
        #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 different priorities as tasks that share a priority will time slice.
    #endif

    /* Store/clear the ready priorities in a bit map. */
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

    /*-----------------------------------------------------------*/

    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - __builtin_clz( ( uxReadyPriorities ) ) )

    #endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

    #endif /* ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0) */

    /*-----------------------------------------------------------*/


    void _xt_coproc_release( volatile void * coproc_sa_base );


/*
 * Map to the memory management routines required for the port.
 *
 * Note that libc standard malloc/free are also available for
 * non-FreeRTOS-specific code, and behave the same as
 * pvPortMalloc()/vPortFree().
 */
    #define pvPortMalloc                       heap_caps_malloc_default
    #define vPortFree                          heap_caps_free
    #define xPortGetFreeHeapSize               esp_get_free_heap_size
    #define xPortGetMinimumEverFreeHeapSize    esp_get_minimum_free_heap_size

#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0))
/*
 * Send an interrupt to another core in order to make the task running
 * on it yield for a higher-priority task.
 */

        void vPortYieldOtherCore( BaseType_t coreid ) PRIVILEGED_FUNCTION;

#endif /* ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0) */

/*
 * Callback to set a watchpoint on the end of the stack. Called every context switch to change the stack
 * watchpoint around.
 */
    void vPortSetStackWatchpoint( void * pxStackStart );

/*
 * Returns true if the current core is in ISR context; low prio ISR, med prio ISR or timer tick ISR. High prio ISRs
 * aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 */
    BaseType_t xPortInIsrContext();


/*
 * This function will be called in High prio ISRs. Returns true if the current core was in ISR context
 * before calling into high prio ISR context.
 */
    BaseType_t xPortInterruptedFromISRContext();

/*
 * The structures and methods of manipulating the MPU are contained within the
 * port layer.
 *
 * Fills the xMPUSettings structure with the memory region information
 * contained in xRegions.
 */
    #if ( portUSING_MPU_WRAPPERS == 1 )
        struct xMEMORY_REGION;
        void vPortStoreTaskMPUSettings( xMPU_SETTINGS * xMPUSettings,
                                        const struct xMEMORY_REGION * const xRegions,
                                        StackType_t * pxBottomOfStack,
                                        uint32_t usStackDepth ) PRIVILEGED_FUNCTION;
        void vPortReleaseTaskMPUSettings( xMPU_SETTINGS * xMPUSettings );
    #endif

/* Multi-core: get current core ID */
    static inline uint32_t IRAM_ATTR xPortGetCoreID()
    {
        int id;

        asm (
            "rsr.prid %0\n"
            " extui %0,%0,13,1"
            : "=r" ( id ) );
        return id;
    }

/* Get tick rate per second */
    uint32_t xPortGetTickRateHz( void );

/* porttrace */
    #if configUSE_TRACE_FACILITY_2
        #include "porttrace.h"
    #endif

/* configASSERT_2 if requested */
    #if configASSERT_2
        #include <stdio.h>
        void exit( int );
        #define configASSERT( x )    if( !( x ) ) { porttracePrint( -1 ); printf( "\nAssertion failed in %s:%d\n", __FILE__, __LINE__ ); exit( -1 ); }
    #endif

/* Barriers */
    #define portMEMORY_BARRIER()    __asm volatile ( "" ::: "memory" )


#endif // __ASSEMBLER__

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* PORTMACRO_H */
