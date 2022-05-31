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

/**
 * \file
 * \ingroup OS_FREERTOS
 * \brief  freertos support for arc processor
 *	like task dispatcher, interrupt handler
 */
/** @cond OS_FREERTOS_ASM_ARC_SUPPORT */

/*
 * core-dependent part in assemble language (for arc)
 */
#define __ASSEMBLY__
#include "arc/arc.h"
#include "arc/arc_asm_common.h"

/*
 *  task dispatcher
 *
 */
	.text
	.align 4
	.global dispatch
dispatch:
/*
 *  the pre-conditions of this routine are task context, CPU is
 *  locked, dispatch is enabled.
 */
	SAVE_NONSCRATCH_REGS		/* save callee save registers */
	mov	r1, dispatch_r
	PUSH	r1			/* save return address */
	ld	r0, [pxCurrentTCB]
	bl	dispatcher

/* return routine when task dispatch happened in task context */
dispatch_r:
	RESTORE_NONSCRATCH_REGS		/* recover registers */
	j	[blink]

/*
 *  start dispatch
 */
	.global start_dispatch
	.align 4
start_dispatch:
/*
 *  this routine is called in the non-task context during the startup of the kernel
 *  , and all the interrupts are locked.
 *
 *  when the dispatcher is called, the cpu is locked, no nest exception (CPU exception/interrupt).
 *  In target_initialize, all interrupt priority mask should be cleared, cpu should be
 *  locked, the interrupts outside the kernel such as fiq can be
 *  enabled.
 */
	clri
	mov	r0, 0
	st	r0, [exc_nest_count]
	b	dispatcher_0
/*
 *  dispatcher
 */
dispatcher:
	ld	r1, [ulCriticalNesting]
	PUSH	r1			/* save critical nesting */
	st	sp, [r0]		/* save stack pointer of current task, r0->pxCurrentTCB */
	jl	vTaskSwitchContext	/* change the value of pxCurrentTCB */
/*
 *  before dispatcher is called, task context | cpu locked | dispatch enabled
 *  should be satisfied. In this routine, the processor will jump
 *  into the entry of next to run task
 *
 *  i.e. kernel mode, IRQ disabled, dispatch enabled
 */
dispatcher_0:
	ld	r1, [pxCurrentTCB]
	ld	sp, [r1]	/* recover task stack */
#if ARC_FEATURE_STACK_CHECK
	lr r0, [AUX_STATUS32]
	bclr r0, r0, AUX_STATUS_BIT_SC
	flag r0
	jl	vPortSetStackCheck
	lr r0, [AUX_STATUS32]
	bset r0, r0, AUX_STATUS_BIT_SC
	flag r0
#endif
	POP	r0		/* get critical nesting */
	st	r0, [ulCriticalNesting]
	POP	r0		/* get return address  */
	j	[r0]

/*
 *  task startup routine
 *
 */
	.text
	.global start_r
	.align 4
start_r:
	seti	/* unlock cpu */
	mov	blink, vPortEndTask	/* set return address */
	POP	r1			/* get task function body */
	POP	r0			/* get task parameters */
	j	[r1]

/****** exceptions and interrupts handing ******/
/****** entry for exception handling ******/
	.global exc_entry_cpu
	.align 4
exc_entry_cpu:

	EXCEPTION_PROLOGUE


	mov	blink,	sp
	mov	r3, sp		/* as exception handler's para(p_excinfo) */

	ld	r1, [exc_nest_count]
	add	r1, r1, 1
	st	r1, [exc_nest_count]
	brne	r1, 0, exc_handler_1
/* change to exception stack if interrupt happened in task context */
	mov	sp, _e_stack
exc_handler_1:
	PUSH	blink

/* find the exception cause */
#if ARC_FEATURE_CORE_700
	lr	r0, [AUX_ECR]
	lsr	r0, r0, 16
	bmsk	r0, r0, 7
#endif
	mov	r1, exc_int_handler_table
	ld.as	r2, [r1, r0]

	mov	r0, r3
	jl	[r2]		/* !!!!jump to exception handler where interrupts are not allowed! */

/* interrupts are not allowed */
ret_exc:
	POP	sp
	mov	r1, exc_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	st 	r0, [r1]
	brne	r0, 0, ret_exc_1  /* nested exception case */
	lr	r1, [AUX_IRQ_LV12]
	brne	r1, 0, ret_exc_1  /* nested or pending interrupt case */

	ld	r0, [context_switch_reqflg]
	brne	r0, 0, ret_exc_2
ret_exc_1:	/* return from non-task context, interrupts or exceptions are nested */

	EXCEPTION_EPILOGUE
#if ARC_FEATURE_CORE_600
	rtie ilink2
#else
	rtie
#endif

/* there is a dispatch request */
ret_exc_2:
	/* clear dispatch request */
	mov	r0, 0
	st	r0, [context_switch_reqflg]

	ld	r0, [pxCurrentTCB]
	breq	r0, 0, ret_exc_1

	SAVE_CALLEE_REGS	/* save callee save registers */

	lr	r0, [AUX_STATUS32]
	bclr	r0, r0, AUX_STATUS_BIT_AE	/* clear exception bit */
	flag	r0

	mov	r1, ret_exc_r	/* save return address */
	PUSH	r1

	bl	dispatcher	/* r0->pxCurrentTCB */

ret_exc_r:
	/* recover exception status */
	lr	r0, [AUX_STATUS32]
	bset	r0, r0, AUX_STATUS_BIT_AE
	flag	r0

	RESTORE_CALLEE_REGS	/* recover registers */
	EXCEPTION_EPILOGUE
#if ARC_FEATURE_CORE_600
	rtie ilink2
#else
	rtie
#endif

/****** entry for normal interrupt exception handling ******/
	.global exc_entry_int	/* entry for interrupt handling */
	.align 4
exc_entry_int:

	INTERRUPT_PROLOGUE

	mov	blink, sp

	/* disable interrupt */
	push r0
	lr  r0, [AUX_STATUS32]
	push r0
    bclr r0, r0, AUX_STATUS_BIT_E1
    bclr r0, r0, AUX_STATUS_BIT_E2
    flag r0
	ld	r3, [exc_nest_count]
	add	r2, r3, 1
	st	r2, [exc_nest_count]
	/* enable interrupt */
	pop r0
	flag r0
	pop r0

	brne	r3, 0, irq_handler_1
/* change to exception stack if interrupt happened in task context */
	mov	sp, _e_stack
#if ARC_FEATURE_STACK_CHECK
	lr r0, [AUX_STATUS32]
	bclr r0, r0, AUX_STATUS_BIT_SC
	flag r0
#endif
irq_handler_1:
	PUSH	blink

/* critical area */
#if ARC_FEATURE_CORE_700
	lr	r0, [AUX_IRQ_CAUSE1]
#endif
	mov	r1, exc_int_handler_table
	ld.as	r2, [r1, r0]	/* r2 = exc_int_handler_table + irqno *4 */
/* handle software triggered interrupt */
	lr	r3, [AUX_IRQ_HINT]
	cmp	r3, r0
	bne.d irq_hint_handled
	xor	r3, r3, r3
	sr	r3, [AUX_IRQ_HINT]
irq_hint_handled:

	jl	[r2]		/* jump to interrupt handler */
/* no interrupts are allowed from here */
ret_int:
	clri	/* disable interrupt */

	POP	sp
	mov	r1, exc_nest_count
	ld	r0, [r1]
	sub	r0, r0, 1
	st	r0, [r1]
/* if there are multi-bits set in IRQ_LV12, it's still in nest interrupt */
	lr	r1, [AUX_IRQ_LV12]

	ld	r0, [context_switch_reqflg]
	brne	r0, 0, ret_int_2
ret_int_1:	/* return from non-task context */
	INTERRUPT_EPILOGUE
#if ARC_FEATURE_CORE_600
/* TODO: series 600 IRQ6 and IRQ7 uses ilink2 */
	rtie ilink1
#else
	rtie
#endif
/* there is a dispatch request */
ret_int_2:
	/* clear dispatch request */
	mov	r0, 0
	st	r0, [context_switch_reqflg]

	ld	r0, [pxCurrentTCB]
	breq 	r0, 0, ret_int_1

/* r1 has old AUX_IRQ_LV12 */
	PUSH	r1
/* clear related bits in IRQ_ACT manually to simulate a irq return  */

	SAVE_CALLEE_REGS	/* save callee save registers */
	mov	r1, ret_int_r	/* save return address */
	PUSH	r1

	bl	dispatcher	/* r0->pxCurrentTCB */

ret_int_r:
	RESTORE_CALLEE_REGS	/* recover registers */
	POPAX	AUX_IRQ_LV12
	INTERRUPT_EPILOGUE
#if ARC_FEATURE_CORE_600
	rtie ilink1
#else
	rtie
#endif

/** @endcond */
