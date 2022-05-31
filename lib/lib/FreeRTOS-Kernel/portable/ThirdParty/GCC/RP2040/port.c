/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: MIT AND BSD-3-Clause
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

/*----------------------------------------------------------------------
* Implementation of functions defined in portable.h for the RP2040 port.
*----------------------------------------------------------------------*/

#include "FreeRTOS.h"
#include "task.h"
#include "rp2040_config.h"
#include "hardware/clocks.h"
#include "hardware/exception.h"

/*
 * LIB_PICO_MULTICORE == 1, if we are linked with pico_multicore (note that
 * the non SMP FreeRTOS_Kernel is not linked with pico_multicore itself). We
 * use this flag to determine if we need multi-core functionality.
 */
#if ( LIB_PICO_MULTICORE == 1)
    #include "pico/multicore.h"
#endif /* LIB_PICO_MULTICORE */

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL_REG             ( *( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG             ( *( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG    ( *( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_INT_CTRL_REG                 ( *( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_SHPR3_REG                    ( *( ( volatile uint32_t * ) 0xe000ed20 ) )
#define portNVIC_SYSTICK_CLK_BIT              ( 1UL << 2UL )
#define portNVIC_SYSTICK_INT_BIT              ( 1UL << 1UL )
#define portNVIC_SYSTICK_ENABLE_BIT           ( 1UL << 0UL )
#define portNVIC_SYSTICK_COUNT_FLAG_BIT       ( 1UL << 16UL )
#define portNVIC_PENDSVSET_BIT                ( 1UL << 28UL )
#define portMIN_INTERRUPT_PRIORITY            ( 255UL )
#define portNVIC_PENDSV_PRI                   ( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI                  ( portMIN_INTERRUPT_PRIORITY << 24UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR                      ( 0x01000000 )

/* The systick is a 24-bit counter. */
#define portMAX_24_BIT_NUMBER                 ( 0xffffffUL )

/* A fiddle factor to estimate the number of SysTick counts that would have
 * occurred while the SysTick counter is stopped during tickless idle
 * calculations. */
#ifndef portMISSED_COUNTS_FACTOR
    #define portMISSED_COUNTS_FACTOR    ( 45UL )
#endif

/* Let the user override the pre-loading of the initial LR with the address of
 * prvTaskExitError() in case it messes up unwinding of the stack in the
 * debugger. */
#ifdef configTASK_RETURN_ADDRESS
    #define portTASK_RETURN_ADDRESS    configTASK_RETURN_ADDRESS
#else
    #define portTASK_RETURN_ADDRESS    prvTaskExitError
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void );

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__( ( naked ) );
void xPortSysTickHandler( void );
void vPortSVCHandler( void );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void vPortStartFirstTask( void ) __attribute__( ( naked ) );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting
 * variable. */
static UBaseType_t uxCriticalNesting = {0xaaaaaaaa};

/*-----------------------------------------------------------*/

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
    #include "pico/lock_core.h"
    #include "hardware/irq.h"
    #include "event_groups.h"
    #if configSUPPORT_STATIC_ALLOCATION
        static StaticEventGroup_t xStaticEventGroup;
        #define pEventGroup (&xStaticEventGroup)
    #endif /* configSUPPORT_STATIC_ALLOCATION */
    static EventGroupHandle_t xEventGroup;
    #if ( LIB_PICO_MULTICORE == 1 )
        static EventBits_t uxCrossCoreEventBits;
        static spin_lock_t * pxCrossCoreSpinLock;
    #endif /* LIB_PICO_MULTICORE */

    static spin_lock_t * pxYieldSpinLock;
    static uint32_t ulYieldSpinLockSaveValue;
#endif /* configSUPPORT_PICO_SYNC_INTEROP */

/*
 * The number of SysTick increments that make up one tick period.
 */
#if ( configUSE_TICKLESS_IDLE == 1 )
    static uint32_t ulTimerCountsForOneTick = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if ( configUSE_TICKLESS_IDLE == 1 )
    static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
#if ( configUSE_TICKLESS_IDLE == 1 )
    static uint32_t ulStoppedTimerCompensation = 0;
#endif /* configUSE_TICKLESS_IDLE */

/*-----------------------------------------------------------*/

#if ( LIB_PICO_MULTICORE == 1 )
    #define INVALID_LAUNCH_CORE_NUM 0xffu
    static uint8_t ucLaunchCoreNum = INVALID_LAUNCH_CORE_NUM;
    #define portIS_FREE_RTOS_CORE() ( ucLaunchCoreNum == get_core_num() )
#else
    #define portIS_FREE_RTOS_CORE() pdTRUE
#endif /* LIB_PICO_MULTICORE */

/*
 * See header file for description.
 */
StackType_t * pxPortInitialiseStack( StackType_t * pxTopOfStack,
                                     TaskFunction_t pxCode,
                                     void * pvParameters )
{
    /* Simulate the stack frame as it would be created by a context switch
     * interrupt. */
    pxTopOfStack--;                                          /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
    *pxTopOfStack = portINITIAL_XPSR;                        /* xPSR */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) pxCode;                  /* PC */
    pxTopOfStack--;
    *pxTopOfStack = ( StackType_t ) portTASK_RETURN_ADDRESS; /* LR */
    pxTopOfStack -= 5;                                       /* R12, R3, R2 and R1. */
    *pxTopOfStack = ( StackType_t ) pvParameters;            /* R0 */
    pxTopOfStack -= 8;                                       /* R11..R4. */

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
    /* A function that implements a task must not exit or attempt to return to
     * its caller as there is nothing to return to.  If a task wants to exit it
     * should instead call vTaskDelete( NULL ). */
    panic_unsupported();
}
/*-----------------------------------------------------------*/

void vPortSVCHandler( void )
{
    /* This function is no longer used, but retained for backward
     * compatibility. */
}
/*-----------------------------------------------------------*/

void vPortStartFirstTask( void )
{
    __asm volatile (
        "   .syntax unified             \n"
        "   ldr  r2, =pxCurrentTCB      \n"/* Obtain location of pxCurrentTCB. */
        "   ldr  r3, [r2]               \n"
        "   ldr  r0, [r3]               \n"/* The first item in pxCurrentTCB is the task top of stack. */
        "   adds r0, #32                \n"/* Discard everything up to r0. */
        "   msr  psp, r0                \n"/* This is now the new top of stack to use in the task. */
        "   movs r0, #2                 \n"/* Switch to the psp stack. */
        "   msr  CONTROL, r0            \n"
        "   isb                         \n"
        "   pop  {r0-r5}                \n"/* Pop the registers that are saved automatically. */
        "   mov  lr, r5                 \n"/* lr is now in r5. */
        "   pop  {r3}                   \n"/* Return address is now in r3. */
        "   pop  {r2}                   \n"/* Pop and discard XPSR. */
        "   cpsie i                     \n"/* The first task has its context and interrupts can be enabled. */
        "   bx   r3                     \n"/* Finally, jump to the user defined task code. */
    );
}
/*-----------------------------------------------------------*/

#if ( LIB_PICO_MULTICORE == 1 ) && ( configSUPPORT_PICO_SYNC_INTEROP == 1)
    static void prvFIFOInterruptHandler()
    {
        /* We must remove the contents (which we don't care about)
         * to clear the IRQ */
        multicore_fifo_drain();
        multicore_fifo_clear_irq();
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint32_t ulSave = spin_lock_blocking( pxCrossCoreSpinLock );
        EventBits_t ulBits = uxCrossCoreEventBits;
        uxCrossCoreEventBits &= ~ulBits;
        spin_unlock( pxCrossCoreSpinLock, ulSave );
        xEventGroupSetBitsFromISR( xEventGroup, ulBits, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
#endif

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler( void )
{
    /* Make PendSV, CallSV and SysTick the same priority as the kernel. */
    portNVIC_SHPR3_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SHPR3_REG |= portNVIC_SYSTICK_PRI;

    #if (configUSE_DYNAMIC_EXCEPTION_HANDLERS == 1)
        exception_set_exclusive_handler( PENDSV_EXCEPTION, xPortPendSVHandler );
        exception_set_exclusive_handler( SYSTICK_EXCEPTION, xPortSysTickHandler );
        exception_set_exclusive_handler( SVCALL_EXCEPTION, vPortSVCHandler );
    #endif

    /* Start the timer that generates the tick ISR.  Interrupts are disabled
     * here already. */
    vPortSetupTimerInterrupt();

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    #if (LIB_PICO_MULTICORE == 1)
        ucLaunchCoreNum = get_core_num();
        #if ( configSUPPORT_PICO_SYNC_INTEROP == 1)
            multicore_fifo_clear_irq();
            multicore_fifo_drain();
            uint32_t irq_num = 15 + get_core_num();
            irq_set_priority( irq_num, portMIN_INTERRUPT_PRIORITY );
            irq_set_exclusive_handler( irq_num, prvFIFOInterruptHandler );
            irq_set_enabled( irq_num, 1 );
        #endif
    #endif

    /* Start the first task. */
    vPortStartFirstTask();

    /* Should never get here as the tasks will now be executing!  Call the task
     * exit error function to prevent compiler warnings about a static function
     * not being called in the case that the application writer overrides this
     * functionality by defining configTASK_RETURN_ADDRESS.  Call
     * vTaskSwitchContext() so link time optimisation does not remove the
     * symbol. */
    vTaskSwitchContext();
    prvTaskExitError();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
    /* Not implemented in ports where there is nothing to return to. */
    panic_unsupported();
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
    #if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
        /* We are not in an ISR, and pxYieldSpinLock is always dealt with and
         * cleared interrupts are re-enabled, so should be NULL */
        configASSERT( pxYieldSpinLock == NULL );
    #endif /* configSUPPORT_PICO_SYNC_INTEROP */

    /* Set a PendSV to request a context switch. */
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;

    /* Barriers are normally not required but do ensure the code is completely
     * within the specified behaviour for the architecture. */
    __asm volatile ( "dsb" ::: "memory" );
    __asm volatile ( "isb" );
}

/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
    __asm volatile ( "dsb" ::: "memory" );
    __asm volatile ( "isb" );
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
    configASSERT( uxCriticalNesting );
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}

void vPortEnableInterrupts() {
    #if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
        if( pxYieldSpinLock )
        {
            spin_unlock(pxYieldSpinLock, ulYieldSpinLockSaveValue);
            pxYieldSpinLock = NULL;
        }
    #endif
    __asm volatile ( " cpsie i " ::: "memory" );
}

/*-----------------------------------------------------------*/

uint32_t ulSetInterruptMaskFromISR( void )
{
    __asm volatile (
        " mrs r0, PRIMASK    \n"
        " cpsid i            \n"
        " bx lr                "
        ::: "memory"
        );
}
/*-----------------------------------------------------------*/

void vClearInterruptMaskFromISR( __attribute__( ( unused ) ) uint32_t ulMask )
{
    __asm volatile (
        " msr PRIMASK, r0    \n"
        " bx lr                "
        ::: "memory"
        );
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler( void )
{
    /* This is a naked function. */

    __asm volatile
    (
        "   .syntax unified                     \n"
        "   mrs r0, psp                         \n"
        "                                       \n"
        "   ldr r3, =pxCurrentTCB               \n"/* Get the location of the current TCB. */
        "   ldr r2, [r3]                        \n"
        "                                       \n"
        "   subs r0, r0, #32                    \n"/* Make space for the remaining low registers. */
        "   str r0, [r2]                        \n"/* Save the new top of stack. */
        "   stmia r0!, {r4-r7}                  \n"/* Store the low registers that are not saved automatically. */
        "   mov r4, r8                          \n"/* Store the high registers. */
        "   mov r5, r9                          \n"
        "   mov r6, r10                         \n"
        "   mov r7, r11                         \n"
        "   stmia r0!, {r4-r7}                  \n"
        #if portUSE_DIVIDER_SAVE_RESTORE
            "   movs r2, #0xd                   \n"/* Store the divider state. */
            "   lsls r2, #28                    \n"
            /* We expect that the divider is ready at this point (which is
             * necessary to safely save/restore), because:
             * a) if we have not been interrupted since we entered this method,
             *    then >8 cycles have clearly passed, so the divider is done
             * b) if we were interrupted in the interim, then any "safe" - i.e.
             *    does the right thing in an IRQ - use of the divider should
             *    have waited for any in-process divide to complete, saved and
             *    then fully restored the result, thus the result is ready in
             *    that case too. */
            "   ldr r4, [r2, #0x60]             \n"/* SIO_DIV_UDIVIDEND_OFFSET */
            "   ldr r5, [r2, #0x64]             \n"/* SIO_DIV_UDIVISOR_OFFSET */
            "   ldr r6, [r2, #0x74]             \n"/* SIO_DIV_REMAINDER_OFFSET */
            "   ldr r7, [r2, #0x70]             \n"/* SIO_DIV_QUOTIENT_OFFSET */
            /* We actually save the divider state in the 4 words below
             * our recorded stack pointer, so as not to disrupt the stack
             * frame expected by debuggers - this is addressed by
             * portEXTRA_STACK_SIZE */
            "   subs r0, r0, #48                \n"
            "   stmia r0!, {r4-r7}              \n"
        #endif /* portUSE_DIVIDER_SAVE_RESTORE */
        "   push {r3, r14}                      \n"
        "   cpsid i                             \n"
        "   bl vTaskSwitchContext               \n"
        "   cpsie i                             \n"
        "   pop {r2, r3}                        \n"/* lr goes in r3. r2 now holds tcb pointer. */
        "                                       \n"
        "   ldr r1, [r2]                        \n"
        "   ldr r0, [r1]                        \n"/* The first item in pxCurrentTCB is the task top of stack. */
        "   adds r0, r0, #16                    \n"/* Move to the high registers. */
        "   ldmia r0!, {r4-r7}                  \n"/* Pop the high registers. */
        "   mov r8, r4                          \n"
        "   mov r9, r5                          \n"
        "   mov r10, r6                         \n"
        "   mov r11, r7                         \n"
        "                                       \n"
        "   msr psp, r0                         \n"/* Remember the new top of stack for the task. */
        "                                       \n"
        #if portUSE_DIVIDER_SAVE_RESTORE
        "   movs r2, #0xd                       \n"/* Pop the divider state. */
        "   lsls r2, #28                        \n"
        "   subs r0, r0, #48                    \n"/* Go back for the divider state */
        "   ldmia r0!, {r4-r7}                  \n"/* Pop the divider state. */
        /* Note always restore via SIO_DIV_UDIVI*, because we will overwrite the
         * results stopping the calculation anyway, however the sign of results
         * is adjusted by the h/w at read time based on whether the last started
         * division was signed and the inputs' signs differed */
        "   str r4, [r2, #0x60]                 \n"/* SIO_DIV_UDIVIDEND_OFFSET */
        "   str r5, [r2, #0x64]                 \n"/* SIO_DIV_UDIVISOR_OFFSET */
        "   str r6, [r2, #0x74]                 \n"/* SIO_DIV_REMAINDER_OFFSET */
        "   str r7, [r2, #0x70]                 \n"/* SIO_DIV_QUOTIENT_OFFSET */
        #else
        "   subs r0, r0, #32                    \n"/* Go back for the low registers that are not automatically restored. */
        #endif /* portUSE_DIVIDER_SAVE_RESTORE */
        "   ldmia r0!, {r4-r7}                  \n"/* Pop low registers.  */
        "                                       \n"
        "   bx r3                               \n"
    );
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
    uint32_t ulPreviousMask;

    ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        /* Increment the RTOS tick. */
        if( xTaskIncrementTick() != pdFALSE )
        {
            /* Pend a context switch. */
            portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
__attribute__( ( weak ) ) void vPortSetupTimerInterrupt( void )
{
    /* Calculate the constants required to configure the tick interrupt. */
    #if ( configUSE_TICKLESS_IDLE == 1 )
        {
            ulTimerCountsForOneTick = ( clock_get_hz(clk_sys) / configTICK_RATE_HZ );
            xMaximumPossibleSuppressedTicks = portMAX_24_BIT_NUMBER / ulTimerCountsForOneTick;
            ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR;
        }
    #endif /* configUSE_TICKLESS_IDLE */

    /* Stop and reset the SysTick. */
    portNVIC_SYSTICK_CTRL_REG = 0UL;
    portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

    /* Configure SysTick to interrupt at the requested rate. */
    portNVIC_SYSTICK_LOAD_REG = ( clock_get_hz( clk_sys ) / configTICK_RATE_HZ ) - 1UL;
    portNVIC_SYSTICK_CTRL_REG = portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT;
}
/*-----------------------------------------------------------*/

#if ( configUSE_TICKLESS_IDLE == 1 )

    __attribute__( ( weak ) ) void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
    {
        uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
        TickType_t xModifiableIdleTime;

        /* Make sure the SysTick reload value does not overflow the counter. */
        if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
        {
            xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
        }

        /* Stop the SysTick momentarily.  The time the SysTick is stopped for
         * is accounted for as best it can be, but using the tickless mode will
         * inevitably result in some tiny drift of the time maintained by the
         * kernel with respect to calendar time. */
        portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

        /* Calculate the reload value required to wait xExpectedIdleTime
         * tick periods.  -1 is used because this code will execute part way
         * through one of the tick periods. */
        ulReloadValue = portNVIC_SYSTICK_CURRENT_VALUE_REG + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );

        if( ulReloadValue > ulStoppedTimerCompensation )
        {
            ulReloadValue -= ulStoppedTimerCompensation;
        }

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
         * method as that will mask interrupts that should exit sleep mode. */
        __asm volatile ( "cpsid i" ::: "memory" );
        __asm volatile ( "dsb" );
        __asm volatile ( "isb" );

        /* If a context switch is pending or a task is waiting for the scheduler
         * to be unsuspended then abandon the low power entry. */
        if( eTaskConfirmSleepModeStatus() == eAbortSleep )
        {
            /* Restart from whatever is left in the count register to complete
             * this tick period. */
            portNVIC_SYSTICK_LOAD_REG = portNVIC_SYSTICK_CURRENT_VALUE_REG;

            /* Restart SysTick. */
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

            /* Reset the reload register to the value required for normal tick
             * periods. */
            portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

            /* Re-enable interrupts - see comments above the cpsid instruction()
             * above. */
            __asm volatile ( "cpsie i" ::: "memory" );
        }
        else
        {
            /* Set the new reload value. */
            portNVIC_SYSTICK_LOAD_REG = ulReloadValue;

            /* Clear the SysTick count flag and set the count value back to
             * zero. */
            portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

            /* Restart SysTick. */
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
            xModifiableIdleTime = xExpectedIdleTime;
            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

            if( xModifiableIdleTime > 0 )
            {
                __asm volatile ( "dsb" ::: "memory" );
                __asm volatile ( "wfi" );
                __asm volatile ( "isb" );
            }

            configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

            /* Re-enable interrupts to allow the interrupt that brought the MCU
             * out of sleep mode to execute immediately.  see comments above
             * __disable_interrupt() call above. */
            __asm volatile ( "cpsie i" ::: "memory" );
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            /* Disable interrupts again because the clock is about to be stopped
             * and interrupts that execute while the clock is stopped will increase
             * any slippage between the time maintained by the RTOS and calendar
             * time. */
            __asm volatile ( "cpsid i" ::: "memory" );
            __asm volatile ( "dsb" );
            __asm volatile ( "isb" );

            /* Disable the SysTick clock without reading the
             * portNVIC_SYSTICK_CTRL_REG register to ensure the
             * portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set.  Again,
             * the time the SysTick is stopped for is accounted for as best it can
             * be, but using the tickless mode will inevitably result in some tiny
             * drift of the time maintained by the kernel with respect to calendar
             * time*/
            portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT );

            /* Determine if the SysTick clock has already counted to zero and
             * been set back to the current reload value (the reload back being
             * correct for the entire expected idle time) or if the SysTick is yet
             * to count to zero (in which case an interrupt other than the SysTick
             * must have brought the system out of sleep mode). */
            if( ( portNVIC_SYSTICK_CTRL_REG & portNVIC_SYSTICK_COUNT_FLAG_BIT ) != 0 )
            {
                uint32_t ulCalculatedLoadValue;

                /* The tick interrupt is already pending, and the SysTick count
                 * reloaded with ulReloadValue.  Reset the
                 * portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
                 * period. */
                ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL ) - ( ulReloadValue - portNVIC_SYSTICK_CURRENT_VALUE_REG );

                /* Don't allow a tiny value, or values that have somehow
                 * underflowed because the post sleep hook did something
                 * that took too long. */
                if( ( ulCalculatedLoadValue < ulStoppedTimerCompensation ) || ( ulCalculatedLoadValue > ulTimerCountsForOneTick ) )
                {
                    ulCalculatedLoadValue = ( ulTimerCountsForOneTick - 1UL );
                }

                portNVIC_SYSTICK_LOAD_REG = ulCalculatedLoadValue;

                /* As the pending tick will be processed as soon as this
                 * function exits, the tick value maintained by the tick is stepped
                 * forward by one less than the time spent waiting. */
                ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
            }
            else
            {
                /* Something other than the tick interrupt ended the sleep.
                 * Work out how long the sleep lasted rounded to complete tick
                 * periods (not the ulReload value which accounted for part
                 * ticks). */
                ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - portNVIC_SYSTICK_CURRENT_VALUE_REG;

                /* How many complete tick periods passed while the processor
                 * was waiting? */
                ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick;

                /* The reload value is set to whatever fraction of a single tick
                 * period remains. */
                portNVIC_SYSTICK_LOAD_REG = ( ( ulCompleteTickPeriods + 1UL ) * ulTimerCountsForOneTick ) - ulCompletedSysTickDecrements;
            }

            /* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
             * again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
             * value. */
            portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;
            portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;
            vTaskStepTick( ulCompleteTickPeriods );
            portNVIC_SYSTICK_LOAD_REG = ulTimerCountsForOneTick - 1UL;

            /* Exit with interrupts enabled. */
            __asm volatile ( "cpsie i" ::: "memory" );
        }
    }

#endif /* configUSE_TICKLESS_IDLE */

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 ) || ( configSUPPORT_PICO_TIME_INTEROP == 1 )
    static TickType_t prvGetTicksToWaitBefore( absolute_time_t t )
    {
        int64_t xDelay = absolute_time_diff_us(get_absolute_time(), t);
        const uint32_t ulTickPeriod = 1000000 / configTICK_RATE_HZ;
        xDelay -= ulTickPeriod;
        if( xDelay >= ulTickPeriod )
        {
            return xDelay / ulTickPeriod;
        }
        return 0;
    }
#endif

#if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
    uint32_t ulPortLockGetCurrentOwnerId()
    {
        if( portIS_FREE_RTOS_CORE())
        {
            uint32_t exception = __get_current_exception();
            if( !exception )
            {
                return ( uintptr_t ) xTaskGetCurrentTaskHandle();
            }
            /* Note: since ROM as at 0x00000000, these can't be confused with
             * valid task handles (pointers) in RAM */
            /* We make all exception handler/core combinations distinct owners */
            return get_core_num() + exception * 2;
        }
        /* Note: since ROM as at 0x00000000, this can't be confused with
         * valid task handles (pointers) in RAM */
        return get_core_num();
    }

    static inline EventBits_t prvGetEventGroupBit( spin_lock_t * spinLock )
    {
        uint32_t ulBit;
        #if ( configUSE_16_BIT_TICKS == 1 )
            ulBit = 1u << (spin_lock_get_num(spinLock) & 0x7u);
        #else
            ulBit = 1u << spin_lock_get_num(spinLock);
            /* reduce to range 0-24 */
            ulBit |= ulBit << 8u;
            ulBit >>= 8u;
        #endif /* configUSE_16_BIT_TICKS */
        return ( EventBits_t ) ulBit;
    }

    static inline EventBits_t prvGetAllEventGroupBits()
    {
        #if ( configUSE_16_BIT_TICKS == 1 )
            return (EventBits_t) 0xffu;
        #else
            return ( EventBits_t ) 0xffffffu;
        #endif /* configUSE_16_BIT_TICKS */
    }

    void vPortLockInternalSpinUnlockWithWait( struct lock_core * pxLock, uint32_t ulSave )
    {
        configASSERT( !portCHECK_IF_IN_ISR() );
        // note no need to check LIB_PICO_MULTICORE, as this is always returns true if that is not defined
        if( !portIS_FREE_RTOS_CORE() )
        {
            spin_unlock(pxLock->spin_lock, ulSave );
            __wfe();
        }
        else
        {
            configASSERT( pxYieldSpinLock == NULL );

            // we want to hold the lock until the event bits have been set; since interrupts are currently disabled
            // by the spinlock, we can defer until portENABLE_INTERRUPTS is called which is always called when
            // the scheduler is unlocked during this call
            configASSERT(pxLock->spin_lock);
            pxYieldSpinLock = pxLock->spin_lock;
            ulYieldSpinLockSaveValue = ulSave;
            xEventGroupWaitBits( xEventGroup, prvGetEventGroupBit(pxLock->spin_lock),
                                 pdTRUE, pdFALSE, portMAX_DELAY);
            /* sanity check that interrupts were disabled, then re-enabled during the call, which will have
             * taken care of the yield */
            configASSERT( pxYieldSpinLock == NULL);
        }
    }

    void vPortLockInternalSpinUnlockWithNotify( struct lock_core *pxLock, uint32_t ulSave ) {
        EventBits_t uxBits = prvGetEventGroupBit(pxLock->spin_lock );
        if (portIS_FREE_RTOS_CORE()) {
            #if LIB_PICO_MULTICORE
                /* signal an event in case a regular core is waiting */
                __sev();
            #endif
            spin_unlock(pxLock->spin_lock, ulSave );
            if( !portCHECK_IF_IN_ISR() )
            {
                xEventGroupSetBits( xEventGroup, uxBits );
            }
            else
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xEventGroupSetBitsFromISR( xEventGroup, uxBits, &xHigherPriorityTaskWoken );
                portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
            }
        }
        else
        {
            __sev();
            #if ( LIB_PICO_MULTICORE == 1)
                /* We could sent the bits across the FIFO which would have required us to block here if the FIFO was full,
                 * or we could have just set all bits on the other side, however it seems reasonable instead to take
                 * the hit of another spin lock to protect an accurate bit set. */
                if( pxCrossCoreSpinLock != pxLock->spin_lock )
                {
                    spin_lock_unsafe_blocking(pxCrossCoreSpinLock);
                    uxCrossCoreEventBits |= uxBits;
                    spin_unlock_unsafe(pxCrossCoreSpinLock);
                }
                else
                {
                    uxCrossCoreEventBits |= uxBits;
                }
                /* This causes fifo irq on the other (FreeRTOS) core which will do the set the event bits */
                sio_hw->fifo_wr = 0;
            #endif /* LIB_PICO_MULTICORE */
            spin_unlock(pxLock->spin_lock, ulSave);
        }
    }

    bool xPortLockInternalSpinUnlockWithBestEffortWaitOrTimeout( struct lock_core * pxLock, uint32_t ulSave, absolute_time_t uxUntil )
    {
        configASSERT( !portCHECK_IF_IN_ISR() );
        // note no need to check LIB_PICO_MULTICORE, as this is always returns true if that is not defined
        if( !portIS_FREE_RTOS_CORE() )
        {
            spin_unlock(pxLock->spin_lock, ulSave);
            return best_effort_wfe_or_timeout(uxUntil);
        }
        else
        {
            configASSERT( portIS_FREE_RTOS_CORE() );
            configASSERT( pxYieldSpinLock == NULL );

            TickType_t uxTicksToWait = prvGetTicksToWaitBefore( uxUntil );
            if( uxTicksToWait )
            {
                /* We want to hold the lock until the event bits have been set; since interrupts are currently disabled
                 * by the spinlock, we can defer until portENABLE_INTERRUPTS is called which is always called when
                 * the scheduler is unlocked during this call */
                configASSERT(pxLock->spin_lock);
                pxYieldSpinLock = pxLock->spin_lock;
                ulYieldSpinLockSaveValue = ulSave;
                xEventGroupWaitBits( xEventGroup,
                                     prvGetEventGroupBit(pxLock->spin_lock), pdTRUE,
                                     pdFALSE, uxTicksToWait );
                /* sanity check that interrupts were disabled, then re-enabled during the call, which will have
                 * taken care of the yield */
                configASSERT( pxYieldSpinLock == NULL );
            }
            else
            {
                spin_unlock( pxLock->spin_lock, ulSave );
            }
            if ( time_reached( uxUntil ) )
            {
                return true;
            }
            else
            {
                /* We do not want to hog the core */
                portYIELD();
                /* We aren't sure if we've reached the timeout yet; the caller will check */
                return false;
            }
        }
    }

    #if ( configSUPPORT_PICO_SYNC_INTEROP == 1)
        /* runs before main */
        static void __attribute__((constructor)) prvRuntimeInitializer( void )
        {
            /* This must be done even before the scheduler is started, as the spin lock
             * is used by the overrides of the SDK wait/notify primitives */
            #if ( LIB_PICO_MULTICORE == 1 )
                pxCrossCoreSpinLock = spin_lock_instance( next_striped_spin_lock_num() );
            #endif /* portRUNNING_ON_BOTH_CORES */

            /* The event group is not used prior to scheduler init, but is initialized
             * here to since it logically belongs with the spin lock */
            #if ( configSUPPORT_STATIC_ALLOCATION == 1 )
                xEventGroup = xEventGroupCreateStatic(&xStaticEventGroup);
            #else
                xEventGroup = xEventGroupCreate();
            #endif /* configSUPPORT_STATIC_ALLOCATION */
        }
    #endif
#endif /* configSUPPORT_PICO_SYNC_INTEROP */

#if ( configSUPPORT_PICO_TIME_INTEROP == 1 )
    void xPortSyncInternalYieldUntilBefore( absolute_time_t t )
    {
        TickType_t uxTicksToWait = prvGetTicksToWaitBefore(t);
        if( uxTicksToWait )
        {
            vTaskDelay(uxTicksToWait);
        }
    }
#endif /* configSUPPORT_PICO_TIME_INTEROP */
