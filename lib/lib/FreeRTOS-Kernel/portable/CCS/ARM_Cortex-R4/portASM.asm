;/*
; * FreeRTOS Kernel <DEVELOPMENT BRANCH>
; * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
; *
; * SPDX-License-Identifier: MIT
; *
; * Permission is hereby granted, free of charge, to any person obtaining a copy of
; * this software and associated documentation files (the "Software"), to deal in
; * the Software without restriction, including without limitation the rights to
; * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
; * the Software, and to permit persons to whom the Software is furnished to do so,
; * subject to the following conditions:
; *
; * The above copyright notice and this permission notice shall be included in all
; * copies or substantial portions of the Software.
; *
; * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
; * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
; * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
; * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
; * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
; *
; * https://www.FreeRTOS.org
; * https://github.com/FreeRTOS
; *
; */

        .text
        .arm
        .ref vTaskSwitchContext
        .ref xTaskIncrementTick
        .ref ulTaskHasFPUContext
		.ref pxCurrentTCB

;/*-----------------------------------------------------------*/
;
; Save Task Context
;
portSAVE_CONTEXT .macro
		DSB

		; Push R0 as we are going to use it
		STMDB	SP!, {R0}

		; Set R0 to point to the task stack pointer.
		STMDB	SP,{SP}^
		SUB	SP, SP, #4
		LDMIA	SP!,{R0}

		; Push the return address onto the stack.
		STMDB	R0!, {LR}

		; Now LR has been saved, it can be used instead of R0.
		MOV	LR, R0

		; Pop R0 so it can be saved onto the task stack.
		LDMIA	SP!, {R0}

		; Push all the system mode registers onto the task stack.
		STMDB	LR,{R0-LR}^
		SUB	LR, LR, #60

		; Push the SPSR onto the task stack.
		MRS	R0, SPSR
		STMDB	LR!, {R0}

    .if (__TI_VFP_SUPPORT__)
		;Determine if the task maintains an FPU context.
		LDR	R0, ulFPUContextConst
		LDR	R0, [R0]

		; Test the flag
		CMP		R0, #0

		; If the task is not using a floating point context then skip the
		; saving of the FPU registers.
		BEQ		$+16
		FSTMDBD	LR!, {D0-D15}
		FMRX    R1,  FPSCR
		STMFD   LR!, {R1}

		; Save the flag
		STMDB	LR!, {R0}
	.endif

		; Store the new top of stack for the task.
		LDR	R0, pxCurrentTCBConst
		LDR	R0, [R0]
		STR	LR, [R0]

        .endm

;/*-----------------------------------------------------------*/
;
; Restore Task Context
;
portRESTORE_CONTEXT .macro
		LDR		R0, pxCurrentTCBConst
		LDR		R0, [R0]
		LDR		LR, [R0]

	.if (__TI_VFP_SUPPORT__)
		; The floating point context flag is the first thing on the stack.
		LDR		R0, ulFPUContextConst
		LDMFD	LR!, {R1}
		STR		R1, [R0]

		; Test the flag
		CMP		R1, #0

		; If the task is not using a floating point context then skip the
		; VFP register loads.
		BEQ		$+16

		; Restore the floating point context.
		LDMFD   LR!, {R0}
		FLDMIAD	LR!, {D0-D15}
		FMXR    FPSCR, R0
	.endif

		; Get the SPSR from the stack.
		LDMFD	LR!, {R0}
		MSR		SPSR_CSXF, R0

		; Restore all system mode registers for the task.
		LDMFD	LR, {R0-R14}^

		; Restore the return address.
		LDR		LR, [LR, #+60]

		; And return - correcting the offset in the LR to obtain the
		; correct address.
		SUBS	PC, LR, #4
        .endm

;/*-----------------------------------------------------------*/
; Start the first task by restoring its context.

        .def vPortStartFirstTask

vPortStartFirstTask:
        portRESTORE_CONTEXT

;/*-----------------------------------------------------------*/
; Yield to another task.

        .def vPortYieldProcessor

vPortYieldProcessor:
		; Within an IRQ ISR the link register has an offset from the true return
		; address.  SWI doesn't do this. Add the offset manually so the ISR
		; return code can be used.
        ADD     LR, LR, #4

        ; First save the context of the current task.
        portSAVE_CONTEXT

        ; Select the next task to execute. */
        BL      vTaskSwitchContext

        ; Restore the context of the task selected to execute.
        portRESTORE_CONTEXT

;/*-----------------------------------------------------------*/
; Yield to another task from within the FreeRTOS API

		.def vPortYeildWithinAPI

vPortYeildWithinAPI:
		; Save the context of the current task.

        portSAVE_CONTEXT
		; Clear SSI flag.
		MOVW    R0, #0xFFF4
		MOVT 	R0, #0xFFFF
		LDR     R0, [R0]

		; Select the next task to execute. */
        BL      vTaskSwitchContext

        ; Restore the context of the task selected to execute.
        portRESTORE_CONTEXT

;/*-----------------------------------------------------------*/
; Preemptive Tick

        .def vPortPreemptiveTick

vPortPreemptiveTick:

		; Save the context of the current task.
        portSAVE_CONTEXT

        ; Clear interrupt flag
        MOVW    R0, #0xFC88
        MOVT    R0, #0xFFFF
        MOV     R1, #1
        STR     R1, [R0]

        ; Increment the tick count, making any adjustments to the blocked lists
        ; that may be necessary.
        BL      xTaskIncrementTick

        ; Select the next task to execute.
        CMP	R0, #0
        BLNE    vTaskSwitchContext

        ; Restore the context of the task selected to execute.
        portRESTORE_CONTEXT

;-------------------------------------------------------------------------------

	.if (__TI_VFP_SUPPORT__)

		.def vPortInitialiseFPSCR

vPortInitialiseFPSCR:

		MOV		R0, #0
		FMXR    FPSCR, R0
		BX		LR

	.endif ;__TI_VFP_SUPPORT__


pxCurrentTCBConst	.word	pxCurrentTCB
ulFPUContextConst 	.word   ulTaskHasFPUContext
;-------------------------------------------------------------------------------

