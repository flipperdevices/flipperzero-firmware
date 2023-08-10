/* armtarget.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/random.h> /* for CUSTOM_RAND_TYPE */

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#ifdef USE_WOLF_ARM_STARTUP

/* Memory initialization */
extern uint32_t __data_load_start__[];
extern uint32_t __data_start__[];
extern uint32_t __data_end__[];

extern uint32_t __bss_start__[];
extern uint32_t __bss_end__[];

extern uint32_t __stack_process_end__[];

extern uint32_t __heap_start__[];
extern uint32_t __heap_end__[];

/* Copy memory: src=Source, dst_beg=Destination Begin, dst_end=Destination End */
void memcpy32(uint32_t* src, uint32_t* dst_beg, uint32_t* dst_end)
{
    while (dst_beg < dst_end) {
        *dst_beg++ = *src++;
    }
}
/* Zero address in range */
void meminit32(uint32_t* start, uint32_t* end)
{
    while (start < end) {
        *start++ = 0;
    }
}
#endif /* USE_WOLF_ARM_STARTUP */

/* Entry Point */
void reset_handler(void)
{
#ifdef USE_WOLF_ARM_STARTUP
    /* Init sections */
    memcpy32(__data_load_start__, __data_start__, __data_end__);
    meminit32(__bss_start__, __bss_end__);

    /* Init heap */
    __heap_start__[0] = 0;
    __heap_start__[1] = ((uintptr_t)__heap_end__ - (uintptr_t)__heap_start__);
#endif /* USE_WOLF_ARM_STARTUP */

    /* Start main */
    extern int main(void);
    main();

    /* Application has ended, so busy wait */
    while(1);
}

#ifdef USE_WOLF_ARM_STARTUP
// Vector Exception/Interrupt Handlers
static void Default_Handler(void)
{
    /* If we get here then need to implement real IRQ handler */
    while(1);
}

__attribute__((section(".sys"))) __attribute__ ((used))
void HardFault_HandlerC( uint32_t *hardfault_args )
{
    /* These are volatile to try and prevent the compiler/linker optimizing them
    away as the variables never actually get used.  If the debugger won't show the
    values of the variables, make them global my moving their declaration outside
    of this function. */
    volatile uint32_t stacked_r0;
    volatile uint32_t stacked_r1;
    volatile uint32_t stacked_r2;
    volatile uint32_t stacked_r3;
    volatile uint32_t stacked_r12;
    volatile uint32_t stacked_lr;
    volatile uint32_t stacked_pc;
    volatile uint32_t stacked_psr;
    volatile uint32_t _CFSR;
    volatile uint32_t _HFSR;
    volatile uint32_t _DFSR;
    volatile uint32_t _AFSR;
    volatile uint32_t _BFAR;
    volatile uint32_t _MMAR;

    stacked_r0 = ((uint32_t)hardfault_args[0]);
    stacked_r1 = ((uint32_t)hardfault_args[1]);
    stacked_r2 = ((uint32_t)hardfault_args[2]);
    stacked_r3 = ((uint32_t)hardfault_args[3]);
    stacked_r12 = ((uint32_t)hardfault_args[4]);
    stacked_lr = ((uint32_t)hardfault_args[5]);
    stacked_pc = ((uint32_t)hardfault_args[6]);
    stacked_psr = ((uint32_t)hardfault_args[7]);

    // Configurable Fault Status Register
    // Consists of MMSR, BFSR and UFSR
    _CFSR = (*((volatile uint32_t *)(0xE000ED28)));

    // Hard Fault Status Register
    _HFSR = (*((volatile uint32_t *)(0xE000ED2C)));

    // Debug Fault Status Register
    _DFSR = (*((volatile uint32_t *)(0xE000ED30)));

    // Auxiliary Fault Status Register
    _AFSR = (*((volatile uint32_t *)(0xE000ED3C)));

    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values
    // MemManage Fault Address Register
    _MMAR = (*((volatile uint32_t *)(0xE000ED34)));
    // Bus Fault Address Register
    _BFAR = (*((volatile uint32_t *)(0xE000ED38)));

    printf ("\n\nHard fault handler (all numbers in hex):\n");
    printf ("R0 = %lx\n", stacked_r0);
    printf ("R1 = %lx\n", stacked_r1);
    printf ("R2 = %lx\n", stacked_r2);
    printf ("R3 = %lx\n", stacked_r3);
    printf ("R12 = %lx\n", stacked_r12);
    printf ("LR [R14] = %lx  subroutine call return address\n", stacked_lr);
    printf ("PC [R15] = %lx  program counter\n", stacked_pc);
    printf ("PSR = %lx\n", stacked_psr);
    printf ("CFSR = %lx\n", _CFSR);
    printf ("HFSR = %lx\n", _HFSR);
    printf ("DFSR = %lx\n", _DFSR);
    printf ("AFSR = %lx\n", _AFSR);
    printf ("MMAR = %lx\n", _MMAR);
    printf ("BFAR = %lx\n", _BFAR);

    // Break into the debugger
    __asm("BKPT #0\n");
}

__attribute__((section(".sys")))  __attribute__( ( naked ) )
void HardFault_Handler(void)
{
    __asm volatile
    (
        " movs r0,#4      \n"  /* load bit mask into R0 */
        " mov  r1, lr     \n"  /* load link register into R1 */
        " tst r0, r1      \n"  /* compare with bitmask */
        " beq _MSP        \n"  /* if bitmask is set: stack pointer is in PSP. Otherwise in MSP */
        " mrs r0, psp     \n"  /* otherwise: stack pointer is in PSP */
        " b _GetPC        \n"  /* go to part which loads the PC */
        "_MSP:            \n"  /* stack pointer is in MSP register */
        " mrs r0, msp     \n"  /* load stack pointer into R0 */
        "_GetPC:          \n"  /* find out where the hard fault happened */
        " ldr r1,[r0,#20] \n"  /* load program counter into R1. R1 contains address of the next instruction where the hard fault happened */
        " ldr r2, =HardFault_HandlerC \n"
        " bx r2           \n"
        " bx lr           \n"  /* decode more information. R0 contains pointer to stack frame */
    );
}

/* Vectors Table */
typedef void (*vector_entry)(void);
const vector_entry vectors[] __attribute__ ((section(".vectors"),used)) =
{
    /* Interrupt Vector Table Function Pointers */
                        // Address     Vector IRQ   Source module    Source description
    (vector_entry)__stack_process_end__, //         ARM core         Initial Supervisor SP
    reset_handler,      // 0x0000_0004 1 -          ARM core         Initial Program Counter
    Default_Handler,    // 0x0000_0008 2 -          ARM core         Non-maskable Interrupt (NMI)
    HardFault_Handler,  // 0x0000_000C 3 -          ARM core         Hard Fault
    Default_Handler,    // 0x0000_0010 4 -
    HardFault_Handler,  // 0x0000_0014 5 -          ARM core         Bus Fault
    HardFault_Handler,  // 0x0000_0018 6 -          ARM core         Usage Fault
    Default_Handler,    // 0x0000_001C 7 -
    Default_Handler,    // 0x0000_0020 8 -
    Default_Handler,    // 0x0000_0024 9 -
    Default_Handler,    // 0x0000_0028 10 -
    Default_Handler,    // 0x0000_002C 11 -         ARM core         Supervisor call (SVCall)
    Default_Handler,    // 0x0000_0030 12 -         ARM core         Debug Monitor
    Default_Handler,    // 0x0000_0034 13 -
    Default_Handler,    // 0x0000_0038 14 -         ARM core         Pendable request for system service (PendableSrvReq)
    Default_Handler,    // 0x0000_003C 15 -         ARM core         System tick timer (SysTick)

    /* remainder go below */
};
#endif /* USE_WOLF_ARM_STARTUP */
