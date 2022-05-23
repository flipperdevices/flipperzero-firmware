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

    SECTION .text:CODE:NOROOT(2)
    THUMB

/* Including FreeRTOSConfig.h here will cause build errors if the header file
contains code not understood by the assembler - for example the 'extern' keyword.
To avoid errors place any such code inside a #ifdef __ICCARM__/#endif block so
the code is included in C files but excluded by the preprocessor in assembly
files (__ICCARM__ is defined by the IAR C compiler but not by the IAR assembler. */
#include "FreeRTOSConfig.h"

    PUBLIC SecureContext_LoadContextAsm
    PUBLIC SecureContext_SaveContextAsm

#if ( configENABLE_FPU == 1 )
    #error Cortex-M23 does not have a Floating Point Unit (FPU) and therefore configENABLE_FPU must be set to 0.
#endif
/*-----------------------------------------------------------*/

SecureContext_LoadContextAsm:
    /* pxSecureContext value is in r0. */
    mrs r1, ipsr                    /* r1 = IPSR. */
    cbz r1, load_ctx_therad_mode    /* Do nothing if the processor is running in the Thread Mode. */
    ldmia r0!, {r1, r2}             /* r1 = pxSecureContext->pucCurrentStackPointer, r2 = pxSecureContext->pucStackLimit. */

#if ( configENABLE_MPU == 1 )
    ldmia r1!, {r3}                 /* Read CONTROL register value from task's stack. r3 = CONTROL. */
    msr control, r3                 /* CONTROL = r3. */
#endif /* configENABLE_MPU */

    msr psplim, r2                  /* PSPLIM = r2. */
    msr psp, r1                     /* PSP = r1. */

    load_ctx_therad_mode:
        bx lr
/*-----------------------------------------------------------*/

SecureContext_SaveContextAsm:
    /* pxSecureContext value is in r0. */
    mrs r1, ipsr                    /* r1 = IPSR. */
    cbz r1, save_ctx_therad_mode    /* Do nothing if the processor is running in the Thread Mode. */
    mrs r1, psp                     /* r1 = PSP. */

#if ( configENABLE_MPU == 1 )
    mrs r2, control                 /* r2 = CONTROL. */
    subs r1, r1, #4                 /* Make space for the CONTROL value on the stack. */
    str r1, [r0]                    /* Save the top of stack in context. pxSecureContext->pucCurrentStackPointer = r1. */
    stmia r1!, {r2}                 /* Store CONTROL value on the stack. */
#else /* configENABLE_MPU */
    str r1, [r0]                    /* Save the top of stack in context. pxSecureContext->pucCurrentStackPointer = r1. */
#endif /* configENABLE_MPU */

    movs r1, #0                     /* r1 = securecontextNO_STACK. */
    msr psplim, r1                  /* PSPLIM = securecontextNO_STACK. */
    msr psp, r1                     /* PSP = securecontextNO_STACK i.e. No stack for thread mode until next task's context is loaded. */

    save_ctx_therad_mode:
        bx lr
/*-----------------------------------------------------------*/

    END
