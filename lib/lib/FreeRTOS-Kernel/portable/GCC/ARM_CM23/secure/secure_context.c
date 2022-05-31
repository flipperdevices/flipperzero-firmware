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

/* Secure context includes. */
#include "secure_context.h"

/* Secure heap includes. */
#include "secure_heap.h"

/* Secure port macros. */
#include "secure_port_macros.h"

/**
 * @brief CONTROL value for privileged tasks.
 *
 * Bit[0] - 0 --> Thread mode is privileged.
 * Bit[1] - 1 --> Thread mode uses PSP.
 */
#define securecontextCONTROL_VALUE_PRIVILEGED      0x02

/**
 * @brief CONTROL value for un-privileged tasks.
 *
 * Bit[0] - 1 --> Thread mode is un-privileged.
 * Bit[1] - 1 --> Thread mode uses PSP.
 */
#define securecontextCONTROL_VALUE_UNPRIVILEGED    0x03

/**
 * @brief Size of stack seal values in bytes.
 */
#define securecontextSTACK_SEAL_SIZE               8

/**
 * @brief Stack seal value as recommended by ARM.
 */
#define securecontextSTACK_SEAL_VALUE              0xFEF5EDA5

/**
 * @brief Maximum number of secure contexts.
 */
#ifndef secureconfigMAX_SECURE_CONTEXTS
    #define secureconfigMAX_SECURE_CONTEXTS        8UL
#endif
/*-----------------------------------------------------------*/

/**
 * @brief Pre-allocated array of secure contexts.
 */
SecureContext_t xSecureContexts[ secureconfigMAX_SECURE_CONTEXTS ];
/*-----------------------------------------------------------*/

/**
 * @brief Get a free secure context for a task from the secure context pool (xSecureContexts).
 *
 * This function ensures that only one secure context is allocated for a task.
 *
 * @param[in] pvTaskHandle The task handle for which the secure context is allocated.
 *
 * @return Index of a free secure context in the xSecureContexts array.
 */
static uint32_t ulGetSecureContext( void * pvTaskHandle );

/**
 * @brief Return the secure context to the secure context pool (xSecureContexts).
 *
 * @param[in] ulSecureContextIndex Index of the context in the xSecureContexts array.
 */
static void vReturnSecureContext( uint32_t ulSecureContextIndex );

/* These are implemented in assembly. */
extern void SecureContext_LoadContextAsm( SecureContext_t * pxSecureContext );
extern void SecureContext_SaveContextAsm( SecureContext_t * pxSecureContext );
/*-----------------------------------------------------------*/

static uint32_t ulGetSecureContext( void * pvTaskHandle )
{
    /* Start with invalid index. */
    uint32_t i, ulSecureContextIndex = secureconfigMAX_SECURE_CONTEXTS;

    for( i = 0; i < secureconfigMAX_SECURE_CONTEXTS; i++ )
    {
        if( ( xSecureContexts[ i ].pucCurrentStackPointer == NULL ) &&
            ( xSecureContexts[ i ].pucStackLimit == NULL ) &&
            ( xSecureContexts[ i ].pucStackStart == NULL ) &&
            ( xSecureContexts[ i ].pvTaskHandle == NULL ) &&
            ( ulSecureContextIndex == secureconfigMAX_SECURE_CONTEXTS ) )
        {
            ulSecureContextIndex = i;
        }
        else if( xSecureContexts[ i ].pvTaskHandle == pvTaskHandle )
        {
            /* A task can only have one secure context. Do not allocate a second
             * context for the same task. */
            ulSecureContextIndex = secureconfigMAX_SECURE_CONTEXTS;
            break;
        }
    }

    return ulSecureContextIndex;
}
/*-----------------------------------------------------------*/

static void vReturnSecureContext( uint32_t ulSecureContextIndex )
{
    xSecureContexts[ ulSecureContextIndex ].pucCurrentStackPointer = NULL;
    xSecureContexts[ ulSecureContextIndex ].pucStackLimit = NULL;
    xSecureContexts[ ulSecureContextIndex ].pucStackStart = NULL;
    xSecureContexts[ ulSecureContextIndex ].pvTaskHandle = NULL;
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_Init( void )
{
    uint32_t ulIPSR, i;
    static uint32_t ulSecureContextsInitialized = 0;

    /* Read the Interrupt Program Status Register (IPSR) value. */
    secureportREAD_IPSR( ulIPSR );

    /* Do nothing if the processor is running in the Thread Mode. IPSR is zero
     * when the processor is running in the Thread Mode. */
    if( ( ulIPSR != 0 ) && ( ulSecureContextsInitialized == 0 ) )
    {
        /* Ensure to initialize secure contexts only once. */
        ulSecureContextsInitialized = 1;

        /* No stack for thread mode until a task's context is loaded. */
        secureportSET_PSPLIM( securecontextNO_STACK );
        secureportSET_PSP( securecontextNO_STACK );

        /* Initialize all secure contexts. */
        for( i = 0; i < secureconfigMAX_SECURE_CONTEXTS; i++ )
        {
            xSecureContexts[ i ].pucCurrentStackPointer = NULL;
            xSecureContexts[ i ].pucStackLimit = NULL;
            xSecureContexts[ i ].pucStackStart = NULL;
            xSecureContexts[ i ].pvTaskHandle = NULL;
        }

        #if ( configENABLE_MPU == 1 )
            {
                /* Configure thread mode to use PSP and to be unprivileged. */
                secureportSET_CONTROL( securecontextCONTROL_VALUE_UNPRIVILEGED );
            }
        #else /* configENABLE_MPU */
            {
                /* Configure thread mode to use PSP and to be privileged. */
                secureportSET_CONTROL( securecontextCONTROL_VALUE_PRIVILEGED );
            }
        #endif /* configENABLE_MPU */
    }
}
/*-----------------------------------------------------------*/

#if ( configENABLE_MPU == 1 )
    secureportNON_SECURE_CALLABLE SecureContextHandle_t SecureContext_AllocateContext( uint32_t ulSecureStackSize,
                                                                                       uint32_t ulIsTaskPrivileged,
                                                                                       void * pvTaskHandle )
#else /* configENABLE_MPU */
    secureportNON_SECURE_CALLABLE SecureContextHandle_t SecureContext_AllocateContext( uint32_t ulSecureStackSize,
                                                                                       void * pvTaskHandle )
#endif /* configENABLE_MPU */
{
    uint8_t * pucStackMemory = NULL;
    uint8_t * pucStackLimit;
    uint32_t ulIPSR, ulSecureContextIndex;
    SecureContextHandle_t xSecureContextHandle = securecontextINVALID_CONTEXT_ID;

    #if ( configENABLE_MPU == 1 )
        uint32_t * pulCurrentStackPointer = NULL;
    #endif /* configENABLE_MPU */

    /* Read the Interrupt Program Status Register (IPSR) and Process Stack Limit
     * Register (PSPLIM) value. */
    secureportREAD_IPSR( ulIPSR );
    secureportREAD_PSPLIM( pucStackLimit );

    /* Do nothing if the processor is running in the Thread Mode. IPSR is zero
     * when the processor is running in the Thread Mode.
     * Also do nothing, if a secure context us already loaded. PSPLIM is set to
     * securecontextNO_STACK when no secure context is loaded. */
    if( ( ulIPSR != 0 ) && ( pucStackLimit == securecontextNO_STACK ) )
    {
        /* Ontain a free secure context. */
        ulSecureContextIndex = ulGetSecureContext( pvTaskHandle );

        /* Were we able to get a free context? */
        if( ulSecureContextIndex < secureconfigMAX_SECURE_CONTEXTS )
        {
            /* Allocate the stack space. */
            pucStackMemory = pvPortMalloc( ulSecureStackSize + securecontextSTACK_SEAL_SIZE );

            if( pucStackMemory != NULL )
            {
                /* Since stack grows down, the starting point will be the last
                 * location. Note that this location is next to the last
                 * allocated byte for stack (excluding the space for seal values)
                 * because the hardware decrements the stack pointer before
                 * writing i.e. if stack pointer is 0x2, a push operation will
                 * decrement the stack pointer to 0x1 and then write at 0x1. */
                xSecureContexts[ ulSecureContextIndex ].pucStackStart = pucStackMemory + ulSecureStackSize;

                /* Seal the created secure process stack. */
                *( uint32_t * )( pucStackMemory + ulSecureStackSize ) = securecontextSTACK_SEAL_VALUE;
                *( uint32_t * )( pucStackMemory + ulSecureStackSize + 4 ) = securecontextSTACK_SEAL_VALUE;

                /* The stack cannot go beyond this location. This value is
                 * programmed in the PSPLIM register on context switch.*/
                xSecureContexts[ ulSecureContextIndex ].pucStackLimit = pucStackMemory;

                xSecureContexts[ ulSecureContextIndex ].pvTaskHandle = pvTaskHandle;

                #if ( configENABLE_MPU == 1 )
                    {
                        /* Store the correct CONTROL value for the task on the stack.
                         * This value is programmed in the CONTROL register on
                         * context switch. */
                        pulCurrentStackPointer = ( uint32_t * ) xSecureContexts[ ulSecureContextIndex ].pucStackStart;
                        pulCurrentStackPointer--;

                        if( ulIsTaskPrivileged )
                        {
                            *( pulCurrentStackPointer ) = securecontextCONTROL_VALUE_PRIVILEGED;
                        }
                        else
                        {
                            *( pulCurrentStackPointer ) = securecontextCONTROL_VALUE_UNPRIVILEGED;
                        }

                        /* Store the current stack pointer. This value is programmed in
                         * the PSP register on context switch. */
                        xSecureContexts[ ulSecureContextIndex ].pucCurrentStackPointer = ( uint8_t * ) pulCurrentStackPointer;
                    }
                #else /* configENABLE_MPU */
                    {
                        /* Current SP is set to the starting of the stack. This
                         * value programmed in the PSP register on context switch. */
                        xSecureContexts[ ulSecureContextIndex ].pucCurrentStackPointer = xSecureContexts[ ulSecureContextIndex ].pucStackStart;
                    }
                #endif /* configENABLE_MPU */

                /* Ensure to never return 0 as a valid context handle. */
                xSecureContextHandle = ulSecureContextIndex + 1UL;
            }
        }
    }

    return xSecureContextHandle;
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_FreeContext( SecureContextHandle_t xSecureContextHandle, void * pvTaskHandle )
{
    uint32_t ulIPSR, ulSecureContextIndex;

    /* Read the Interrupt Program Status Register (IPSR) value. */
    secureportREAD_IPSR( ulIPSR );

    /* Do nothing if the processor is running in the Thread Mode. IPSR is zero
     * when the processor is running in the Thread Mode. */
    if( ulIPSR != 0 )
    {
        /* Only free if a valid context handle is passed. */
        if( ( xSecureContextHandle > 0UL ) && ( xSecureContextHandle <= secureconfigMAX_SECURE_CONTEXTS ) )
        {
            ulSecureContextIndex = xSecureContextHandle - 1UL;

            /* Ensure that the secure context being deleted is associated with
             * the task. */
            if( xSecureContexts[ ulSecureContextIndex ].pvTaskHandle == pvTaskHandle )
            {
                /* Free the stack space. */
                vPortFree( xSecureContexts[ ulSecureContextIndex ].pucStackLimit );

                /* Return the secure context back to the free secure contexts pool. */
                vReturnSecureContext( ulSecureContextIndex );
            }
        }
    }
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_LoadContext( SecureContextHandle_t xSecureContextHandle, void * pvTaskHandle )
{
    uint8_t * pucStackLimit;
    uint32_t ulSecureContextIndex;

    if( ( xSecureContextHandle > 0UL ) && ( xSecureContextHandle <= secureconfigMAX_SECURE_CONTEXTS ) )
    {
        ulSecureContextIndex = xSecureContextHandle - 1UL;

        secureportREAD_PSPLIM( pucStackLimit );

        /* Ensure that no secure context is loaded and the task is loading it's
         * own context. */
        if( ( pucStackLimit == securecontextNO_STACK ) &&
            ( xSecureContexts[ ulSecureContextIndex ].pvTaskHandle == pvTaskHandle ) )
        {
            SecureContext_LoadContextAsm( &( xSecureContexts[ ulSecureContextIndex ] ) );
        }
    }
}
/*-----------------------------------------------------------*/

secureportNON_SECURE_CALLABLE void SecureContext_SaveContext( SecureContextHandle_t xSecureContextHandle, void * pvTaskHandle )
{
    uint8_t * pucStackLimit;
    uint32_t ulSecureContextIndex;

    if( ( xSecureContextHandle > 0UL ) && ( xSecureContextHandle <= secureconfigMAX_SECURE_CONTEXTS ) )
    {
        ulSecureContextIndex = xSecureContextHandle - 1UL;

        secureportREAD_PSPLIM( pucStackLimit );

        /* Ensure that task's context is loaded and the task is saving it's own
         * context. */
        if( ( xSecureContexts[ ulSecureContextIndex ].pucStackLimit == pucStackLimit ) &&
            ( xSecureContexts[ ulSecureContextIndex ].pvTaskHandle == pvTaskHandle ) )
        {
            SecureContext_SaveContextAsm( &( xSecureContexts[ ulSecureContextIndex ] ) );
        }
    }
}
/*-----------------------------------------------------------*/
