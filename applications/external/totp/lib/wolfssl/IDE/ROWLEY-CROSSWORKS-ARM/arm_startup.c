/* arm_startup.c
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


#include "hw.h"
#include <stdio.h>

// Memory initialization
extern uint32_t __data_load_start__[];
extern uint32_t __data_start__[];
extern uint32_t __data_end__[];

extern uint32_t __bss_start__[];
extern uint32_t __bss_end__[];

extern uint32_t __fast_load_start__[];
extern uint32_t __fast_start__[];
extern uint32_t __fast_end__[];

extern uint32_t __stack_process_end__[];

extern uint32_t __heap_start__[];
extern uint32_t __heap_end__[];

// Copy memory: src=Source, dst_beg=Destination Begin, dst_end=Destination End
void memcpy32(uint32_t* src, uint32_t* dst_beg, uint32_t* dst_end)
{
    while (dst_beg < dst_end) {
        *dst_beg++ = *src++;
    }
}
// Zero address in range
void meminit32(uint32_t* start, uint32_t* end)
{
    while (start < end) {
        *start++ = 0;
    }
}

// Entry Point
void reset_handler(void)
{
    // Disable Watchdog
    hw_watchdog_disable();

    // Init sections
    memcpy32(__data_load_start__, __data_start__, __data_end__);
    meminit32(__bss_start__, __bss_end__);
    memcpy32(__fast_load_start__, __fast_start__, __fast_end__);

    // Init heap
    __heap_start__[0] = 0;
    __heap_start__[1] = ((uint32_t)__heap_end__ - (uint32_t)__heap_start__);

    // Init hardware
    hw_init();

    // Start main
    extern void main(void);
    main();

    // Application has ended, so busy wait
    while(1);
}

// Vector Exception/Interrupt Handlers
static void Default_Handler(void)
{
    /* If we get here then need to implement real IRQ handler */
    while(1);
}

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
    printf ("R0 = %x\n", stacked_r0);
    printf ("R1 = %x\n", stacked_r1);
    printf ("R2 = %x\n", stacked_r2);
    printf ("R3 = %x\n", stacked_r3);
    printf ("R12 = %x\n", stacked_r12);
    printf ("LR [R14] = %x  subroutine call return address\n", stacked_lr);
    printf ("PC [R15] = %x  program counter\n", stacked_pc);
    printf ("PSR = %x\n", stacked_psr);
    printf ("CFSR = %x\n", _CFSR);
    printf ("HFSR = %x\n", _HFSR);
    printf ("DFSR = %x\n", _DFSR);
    printf ("AFSR = %x\n", _AFSR);
    printf ("MMAR = %x\n", _MMAR);
    printf ("BFAR = %x\n", _BFAR);

    // Break into the debugger
    __asm("BKPT #0\n");
}

__attribute__( ( naked ) )
void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word HardFault_HandlerC          \n"
    );
}

// Vectors
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

#ifdef CPU_MK82FN256VLL15
    // Add specific driver interrupt handlers below
    Default_Handler,    /* DMA0_DMA16_IRQn              = 0,                /**< DMA channel 0,16 transfer complete */
    Default_Handler,    /* DMA1_DMA17_IRQn              = 1,                /**< DMA channel 1,17 transfer complete */
    Default_Handler,    /* DMA2_DMA18_IRQn              = 2,                /**< DMA channel 2,18 transfer complete */
    Default_Handler,    /* DMA3_DMA19_IRQn              = 3,                /**< DMA channel 3,19 transfer complete */
    Default_Handler,    /* DMA4_DMA20_IRQn              = 4,                /**< DMA channel 4,20 transfer complete */
    Default_Handler,    /* DMA5_DMA21_IRQn              = 5,                /**< DMA channel 5,21 transfer complete */
    Default_Handler,    /* DMA6_DMA22_IRQn              = 6,                /**< DMA channel 6,22 transfer complete */
    Default_Handler,    /* DMA7_DMA23_IRQn              = 7,                /**< DMA channel 7,23 transfer complete */
    Default_Handler,    /* DMA8_DMA24_IRQn              = 8,                /**< DMA channel 8,24 transfer complete */
    Default_Handler,    /* DMA9_DMA25_IRQn              = 9,                /**< DMA channel 9,25 transfer complete */
    Default_Handler,    /* DMA10_DMA26_IRQn             = 10,               /**< DMA channel 10,26 transfer complete */
    Default_Handler,    /* DMA11_DMA27_IRQn             = 11,               /**< DMA channel 11,27 transfer complete */
    Default_Handler,    /* DMA12_DMA28_IRQn             = 12,               /**< DMA channel 12,28 transfer complete */
    Default_Handler,    /* DMA13_DMA29_IRQn             = 13,               /**< DMA channel 13,29 transfer complete */
    Default_Handler,    /* DMA14_DMA30_IRQn             = 14,               /**< DMA channel 14,30 transfer complete */
    Default_Handler,    /* DMA15_DMA31_IRQn             = 15,               /**< DMA channel 15,31 transfer complete */
    Default_Handler,    /* DMA_Error_IRQn               = 16,               /**< DMA channel 0 - 31 error */
    Default_Handler,    /* MCM_IRQn                     = 17,               /**< MCM normal interrupt */
    Default_Handler,    /* FTFA_IRQn                    = 18,               /**< FTFA command complete */
    Default_Handler,    /* Read_Collision_IRQn          = 19,               /**< FTFA read collision */
    Default_Handler,    /* LVD_LVW_IRQn                 = 20,               /**< PMC controller low-voltage detect, low-voltage warning */
    Default_Handler,    /* LLWU_IRQn                    = 21,               /**< Low leakage wakeup unit */
    Default_Handler,    /* WDOG_EWM_IRQn                = 22,               /**< Single interrupt vector for  WDOG and EWM */
    Default_Handler,    /* TRNG0_IRQn                   = 23,               /**< True randon number generator */
    Default_Handler,    /* I2C0_IRQn                    = 24,               /**< Inter-integrated circuit 0 */
    Default_Handler,    /* I2C1_IRQn                    = 25,               /**< Inter-integrated circuit 1 */
    Default_Handler,    /* SPI0_IRQn                    = 26,               /**< Serial peripheral Interface 0 */
    Default_Handler,    /* SPI1_IRQn                    = 27,               /**< Serial peripheral Interface 1 */
    Default_Handler,    /* I2S0_Tx_IRQn                 = 28,               /**< Integrated interchip sound 0 transmit interrupt */
    Default_Handler,    /* I2S0_Rx_IRQn                 = 29,               /**< Integrated interchip sound 0 receive interrupt */
    Default_Handler,    /* LPUART0_IRQn                 = 30,               /**< LPUART0 receive/transmit/error interrupt */
    Default_Handler,    /* LPUART1_IRQn                 = 31,               /**< LPUART1 receive/transmit/error interrupt */
    Default_Handler,    /* LPUART2_IRQn                 = 32,               /**< LPUART2 receive/transmit/error interrupt */
    Default_Handler,    /* LPUART3_IRQn                 = 33,               /**< LPUART3 receive/transmit/error interrupt */
    Default_Handler,    /* LPUART4_IRQn                 = 34,               /**< LPUART4 receive/transmit/error interrupt */
    Default_Handler,    /* Reserved51_IRQn              = 35,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved52_IRQn              = 36,               /**< Reserved interrupt */
    Default_Handler,    /* EMVSIM0_IRQn                 = 37,               /**< EMVSIM0 common interrupt */
    Default_Handler,    /* EMVSIM1_IRQn                 = 38,               /**< EMVSIM1 common interrupt */
    Default_Handler,    /* ADC0_IRQn                    = 39,               /**< Analog-to-digital converter 0 */
    Default_Handler,    /* CMP0_IRQn                    = 40,               /**< Comparator 0 */
    Default_Handler,    /* CMP1_IRQn                    = 41,               /**< Comparator 1 */
    Default_Handler,    /* FTM0_IRQn                    = 42,               /**< FlexTimer module 0 fault, overflow and channels interrupt */
    Default_Handler,    /* FTM1_IRQn                    = 43,               /**< FlexTimer module 1 fault, overflow and channels interrupt */
    Default_Handler,    /* FTM2_IRQn                    = 44,               /**< FlexTimer module 2 fault, overflow and channels interrupt */
    Default_Handler,    /* CMT_IRQn                     = 45,               /**< Carrier modulator transmitter */
    Default_Handler,    /* RTC_IRQn                     = 46,               /**< Real time clock */
    Default_Handler,    /* RTC_Seconds_IRQn             = 47,               /**< Real time clock seconds */
    Default_Handler,    /* PIT0CH0_IRQn                 = 48,               /**< Periodic interrupt timer 0 channel 0 */
    Default_Handler,    /* PIT0CH1_IRQn                 = 49,               /**< Periodic interrupt timer 0 channel 1 */
    Default_Handler,    /* PIT0CH2_IRQn                 = 50,               /**< Periodic interrupt timer 0 channel 2 */
    Default_Handler,    /* PIT0CH3_IRQn                 = 51,               /**< Periodic interrupt timer 0 channel 3 */
    Default_Handler,    /* PDB0_IRQn                    = 52,               /**< Programmable delay block */
    Default_Handler,    /* USB0_IRQn                    = 53,               /**< USB OTG interrupt */
    Default_Handler,    /* USBDCD_IRQn                  = 54,               /**< USB charger detect */
    Default_Handler,    /* Reserved71_IRQn              = 55,               /**< Reserved interrupt */
    Default_Handler,    /* DAC0_IRQn                    = 56,               /**< Digital-to-analog converter 0 */
    Default_Handler,    /* MCG_IRQn                     = 57,               /**< Multipurpose clock generator */
    Default_Handler,    /* LPTMR0_LPTMR1_IRQn           = 58,               /**< Single interrupt vector for  Low Power Timer 0 and 1 */
    Default_Handler,    /* PORTA_IRQn                   = 59,               /**< Port A pin detect interrupt */
    Default_Handler,    /* PORTB_IRQn                   = 60,               /**< Port B pin detect interrupt */
    Default_Handler,    /* PORTC_IRQn                   = 61,               /**< Port C pin detect interrupt */
    Default_Handler,    /* PORTD_IRQn                   = 62,               /**< Port D pin detect interrupt */
    Default_Handler,    /* PORTE_IRQn                   = 63,               /**< Port E pin detect interrupt */
    Default_Handler,    /* SWI_IRQn                     = 64,               /**< Software interrupt */
    Default_Handler,    /* SPI2_IRQn                    = 65,               /**< Serial peripheral Interface 2 */
    Default_Handler,    /* Reserved82_IRQn              = 66,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved83_IRQn              = 67,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved84_IRQn              = 68,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved85_IRQn              = 69,               /**< Reserved interrupt */
    Default_Handler,    /* FLEXIO0_IRQn                 = 70,               /**< FLEXIO0 */
    Default_Handler,    /* FTM3_IRQn                    = 71,               /**< FlexTimer module 3 fault, overflow and channels interrupt */
    Default_Handler,    /* Reserved88_IRQn              = 72,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved89_IRQn              = 73,               /**< Reserved interrupt */
    Default_Handler,    /* I2C2_IRQn                    = 74,               /**< Inter-integrated circuit 2 */
    Default_Handler,    /* Reserved91_IRQn              = 75,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved92_IRQn              = 76,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved93_IRQn              = 77,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved94_IRQn              = 78,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved95_IRQn              = 79,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved96_IRQn              = 80,               /**< Reserved interrupt */
    Default_Handler,    /* SDHC_IRQn                    = 81,               /**< Secured digital host controller */
    Default_Handler,    /* Reserved98_IRQn              = 82,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved99_IRQn              = 83,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved100_IRQn             = 84,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved101_IRQn             = 85,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved102_IRQn             = 86,               /**< Reserved interrupt */
    Default_Handler,    /* TSI0_IRQn                    = 87,               /**< Touch Sensing Input */
    Default_Handler,    /* TPM1_IRQn                    = 88,               /**< TPM1 single interrupt vector for all sources */
    Default_Handler,    /* TPM2_IRQn                    = 89,               /**< TPM2 single interrupt vector for all sources */
    Default_Handler,    /* Reserved106_IRQn             = 90,               /**< Reserved interrupt */
    Default_Handler,    /* I2C3_IRQn                    = 91,               /**< Inter-integrated circuit 3 */
    Default_Handler,    /* Reserved108_IRQn             = 92,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved109_IRQn             = 93,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved110_IRQn             = 94,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved111_IRQn             = 95,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved112_IRQn             = 96,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved113_IRQn             = 97,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved114_IRQn             = 98,               /**< Reserved interrupt */
    Default_Handler,    /* Reserved115_IRQn             = 99,               /**< Reserved interrupt */
    Default_Handler,    /* QuadSPI0_IRQn                = 100,              /**< qspi */
    Default_Handler,    /* Reserved117_IRQn             = 101,              /**< Reserved interrupt */
    Default_Handler,    /* Reserved118_IRQn             = 102,              /**< Reserved interrupt */
    Default_Handler,    /* Reserved119_IRQn             = 103,              /**< Reserved interrupt */
    Default_Handler,    /* LTC0_IRQn                    = 104,              /**< LP Trusted Cryptography */
    Default_Handler,    /* Reserved121_IRQn             = 105,              /**< Reserved interrupt */
    Default_Handler,    /* Reserved122_IRQn             = 106               /**< Reserved interrupt */
#endif /* CPU_MK82FN256VLL15 */
};
