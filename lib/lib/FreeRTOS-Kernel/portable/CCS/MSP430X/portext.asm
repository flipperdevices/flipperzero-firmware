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

; * The definition of the "register test" tasks, as described at the top of
; * main.c

	.include data_model.h

	.global xTaskIncrementTick
	.global vTaskSwitchContext
	.global vPortSetupTimerInterrupt
	.global pxCurrentTCB
	.global usCriticalNesting

	.def vPortPreemptiveTickISR
	.def vPortCooperativeTickISR
	.def vPortYield
	.def xPortStartScheduler

;-----------------------------------------------------------

portSAVE_CONTEXT .macro

	;Save the remaining registers.
	pushm_x	#12, r15
	mov.w	&usCriticalNesting, r14
	push_x r14
	mov_x	&pxCurrentTCB, r12
	mov_x	sp, 0( r12 )
	.endm
;-----------------------------------------------------------

portRESTORE_CONTEXT .macro

	mov_x	&pxCurrentTCB, r12
	mov_x	@r12, sp
	pop_x	r15
	mov.w	r15, &usCriticalNesting
	popm_x	#12, r15
	nop
	pop.w	sr
	nop
	ret_x
	.endm
;-----------------------------------------------------------

;*
;* The RTOS tick ISR.
;*
;* If the cooperative scheduler is in use this simply increments the tick
;* count.
;*
;* If the preemptive scheduler is in use a context switch can also occur.
;*/

	.text
	.align 2

vPortPreemptiveTickISR: .asmfunc

	; The sr is not saved in portSAVE_CONTEXT() because vPortYield() needs
	;to save it manually before it gets modified (interrupts get disabled).
	push.w sr
	portSAVE_CONTEXT

	call_x	#xTaskIncrementTick
	call_x	#vTaskSwitchContext

	portRESTORE_CONTEXT
	.endasmfunc
;-----------------------------------------------------------

	.align 2

vPortCooperativeTickISR: .asmfunc

	; The sr is not saved in portSAVE_CONTEXT() because vPortYield() needs
	;to save it manually before it gets modified (interrupts get disabled).
	push.w sr
	portSAVE_CONTEXT

	call_x	#xTaskIncrementTick

	portRESTORE_CONTEXT

	.endasmfunc
;-----------------------------------------------------------

;
; Manual context switch called by the portYIELD() macro.
;

	.align 2

vPortYield: .asmfunc

	; The sr needs saving before it is modified.
	push.w	sr

	; Now the SR is stacked we can disable interrupts.
	dint
	nop

	; Save the context of the current task.
	portSAVE_CONTEXT

	; Select the next task to run.
	call_x	#vTaskSwitchContext

	; Restore the context of the new task.
	portRESTORE_CONTEXT
	.endasmfunc
;-----------------------------------------------------------


;
; Start off the scheduler by initialising the RTOS tick timer, then restoring
; the context of the first task.
;

	.align 2

xPortStartScheduler: .asmfunc

	; Setup the hardware to generate the tick.  Interrupts are disabled
	; when this function is called.
	call_x	#vPortSetupTimerInterrupt

	; Restore the context of the first task that is going to run.
	portRESTORE_CONTEXT
	.endasmfunc
;-----------------------------------------------------------

	.end

