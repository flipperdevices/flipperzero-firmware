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

/*
 * Implementation of functions defined in portable.h
 */

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "arc/arc_exception.h"
#include "arc/arc_timer.h"
#include "board.h"

#include "arc_freertos_exceptions.h"

volatile unsigned int ulCriticalNesting = 999UL;
volatile unsigned int context_switch_reqflg; /* task context switch request flag in exceptions and interrupts handling */

/**
 * \var exc_nest_count
 * \brief the counter for exc/int processing, =0 no int/exc
 * >1 in int/exc processing
 * @}
 */
uint32_t exc_nest_count;
/* --------------------------------------------------------------------------*/

/**
 * @brief kernel tick interrupt handler of freertos
 */
/* ----------------------------------------------------------------------------*/
static void vKernelTick( void * ptr )
{
    /* clear timer interrupt */
    arc_timer_int_clear( BOARD_OS_TIMER_ID );
    board_timer_update( configTICK_RATE_HZ );

    if( xTaskIncrementTick() )
    {
        portYIELD_FROM_ISR(); /* need to make task switch */
    }
}

/* --------------------------------------------------------------------------*/

/**
 * @brief  setup freertos kernel tick
 */
/* ----------------------------------------------------------------------------*/
static void prvSetupTimerInterrupt( void )
{
    unsigned int cyc = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

    int_disable( BOARD_OS_TIMER_INTNO ); /* disable os timer interrupt */
    arc_timer_stop( BOARD_OS_TIMER_ID );
    arc_timer_start( BOARD_OS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc );

    int_handler_install( BOARD_OS_TIMER_INTNO, ( INT_HANDLER_T ) vKernelTick );
    int_pri_set( BOARD_OS_TIMER_INTNO, INT_PRI_MIN );
    int_enable( BOARD_OS_TIMER_INTNO );
}

/*
 * Setup the stack of a new task so it is ready to be placed under the
 * scheduler control.  The registers have to be placed on the stack in
 * the order that the port expects to find them.
 *
 * For ARC, task context switch is implemented with the help of SWI exception
 * It's not efficient but simple.
 *
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* To ensure asserts in tasks.c don't fail, although in this case the assert
     * is not really required. */
    pxTopOfStack--;

    /* Setup the initial stack of the task.  The stack is set exactly as
     * expected by the portRESTORE_CONTEXT() macro. */

    /* When the task starts is will expect to find the function parameter in
     * R0. */
    *pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */

    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxCode; /* function body */

    /* PC */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) start_r; /* dispatch return address */

    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) portNO_CRITICAL_NESTING;
    return pxTopOfStack;
}

/* --------------------------------------------------------------------------*/

/**
 * @brief  start the freertos scheduler, go to the first task
 *
 * @returns
 */
/* ----------------------------------------------------------------------------*/
BaseType_t xPortStartScheduler( void )
{
    /* Start the timer that generates the tick ISR. */
    prvSetupTimerInterrupt();
    start_dispatch();

    /* Should not get here! */
    return 0;
}

/* --------------------------------------------------------------------------*/

/**
 * @brief
 */
/* ----------------------------------------------------------------------------*/
void vPortEndScheduler( void )
{
}

/* --------------------------------------------------------------------------*/

/**
 * @brief  generate a task switch request in ISR
 */
/* ----------------------------------------------------------------------------*/
void vPortYieldFromIsr( void )
{
    unsigned int status32;

    status32 = cpu_lock_save();
    context_switch_reqflg = true;
    cpu_unlock_restore( status32 );
}

/* --------------------------------------------------------------------------*/

/**
 * @brief
 */
/* ----------------------------------------------------------------------------*/
void vPortYield( void )
{
    unsigned int status32;

    status32 = cpu_lock_save();
    dispatch();
    cpu_unlock_restore( status32 );
}

/* --------------------------------------------------------------------------*/

/**
 * @brief
 */
/* ----------------------------------------------------------------------------*/
void vPortEndTask( void )
{
    #if ( INCLUDE_vTaskDelete == 1 )
        vTaskDelete( NULL ); /* Delete task itself */
    #endif

    while( 1 ) /* Yield to other task */
    {
        vPortYield();
    }
}

#if ARC_FEATURE_STACK_CHECK

/*
 * !!! Note !!!
 * This a trick!!!
 * It's a copy from task.c. We need to konw the definition of TCB for the purpose of hardware
 * stack check. Pls don't forget to update it when FreeRTOS is updated.
 */
    typedef struct tskTaskControlBlock       /* The old naming convention is used to prevent breaking kernel aware debuggers. */
    {
        volatile StackType_t * pxTopOfStack; /*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

        #if ( portUSING_MPU_WRAPPERS == 1 )
            xMPU_SETTINGS xMPUSettings;     /*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
        #endif

        ListItem_t xStateListItem;                  /*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
        ListItem_t xEventListItem;                  /*< Used to reference a task from an event list. */
        UBaseType_t uxPriority;                     /*< The priority of the task.  0 is the lowest priority. */
        StackType_t * pxStack;                      /*< Points to the start of the stack. */
        char pcTaskName[ configMAX_TASK_NAME_LEN ]; /*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

        #if ( ( portSTACK_GROWTH > 0 ) || ( configRECORD_STACK_HIGH_ADDRESS == 1 ) )
            StackType_t * pxEndOfStack;     /*< Points to the highest valid address for the stack. */
        #endif

        #if ( portCRITICAL_NESTING_IN_TCB == 1 )
            UBaseType_t uxCriticalNesting;  /*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
        #endif

        #if ( configUSE_TRACE_FACILITY == 1 )
            UBaseType_t uxTCBNumber;        /*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
            UBaseType_t uxTaskNumber;       /*< Stores a number specifically for use by third party trace code. */
        #endif

        #if ( configUSE_MUTEXES == 1 )
            UBaseType_t uxBasePriority;     /*< The priority last assigned to the task - used by the priority inheritance mechanism. */
            UBaseType_t uxMutexesHeld;
        #endif

        #if ( configUSE_APPLICATION_TASK_TAG == 1 )
            TaskHookFunction_t pxTaskTag;
        #endif

        #if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )
            void * pvThreadLocalStoragePointers[ configNUM_THREAD_LOCAL_STORAGE_POINTERS ];
        #endif

        #if ( configGENERATE_RUN_TIME_STATS == 1 )
            uint32_t ulRunTimeCounter;      /*< Stores the amount of time the task has spent in the Running state. */
        #endif

        #if ( configUSE_NEWLIB_REENTRANT == 1 )

            /* Allocate a Newlib reent structure that is specific to this task.
             * Note Newlib support has been included by popular demand, but is not
             * used by the FreeRTOS maintainers themselves.  FreeRTOS is not
             * responsible for resulting newlib operation.  User must be familiar with
             * newlib and must provide system-wide implementations of the necessary
             * stubs. Be warned that (at the time of writing) the current newlib design
             * implements a system-wide malloc() that must be provided with locks. */
            struct  _reent xNewLib_reent;
        #endif

        #if ( configUSE_TASK_NOTIFICATIONS == 1 )
            volatile uint32_t ulNotifiedValue;
            volatile uint8_t ucNotifyState;
        #endif

        /* See the comments above the definition of
         * tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */
        #if ( tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0 ) /*lint !e731 !e9029 Macro has been consolidated for readability reasons. */
            uint8_t ucStaticallyAllocated;                     /*< Set to pdTRUE if the task is a statically allocated to ensure no attempt is made to free the memory. */
        #endif

        #if ( INCLUDE_xTaskAbortDelay == 1 )
            uint8_t ucDelayAborted;
        #endif

        #if ( configUSE_POSIX_ERRNO == 1 )
            int iTaskErrno;
        #endif
    } tskTCB;


    void vPortSetStackCheck( TaskHandle_t old,
                             TaskHandle_t new )
    {
        if( new != NULL )
        {
            arc_aux_write( AUX_USTACK_BASE, ( uint32_t ) ( new->pxEndOfStack ) );
            arc_aux_write( AUX_USTACK_TOP, ( uint32_t ) ( new->pxStack ) );
        }
    }
#endif /* if ARC_FEATURE_STACK_CHECK */
