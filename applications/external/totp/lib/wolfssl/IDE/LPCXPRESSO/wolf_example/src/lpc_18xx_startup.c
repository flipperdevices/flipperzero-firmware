/* lpc_18xx_startup.c
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


#include "board.h"
#include <stdint.h>
#include <stdio.h>

/* Top of stack location */
extern void _vStackTop(void);

/* Memory locations */
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

/* Copy memory: src=Source, dst_beg=Destination Begin, dst_end=Destination End */
__attribute__ ((section(".after_vectors")))
void memcpy32(uint32_t* src, uint32_t* dst_beg, uint32_t len)
{
    unsigned int i;
    for (i = 0; i < len; i += sizeof(uint32_t)) {
        *dst_beg++ = *src++;
    }
}

/* Zero address in range */
__attribute__ ((section(".after_vectors")))
void meminit32(uint32_t* start, uint32_t len)
{
    unsigned int i;
    for (i = 0; i < len; i += sizeof(uint32_t)) {
        *start++ = 0;
    }
}

/* Reset Entry Point */
void ResetISR(void)
{
    unsigned int irqPendLoop;
    unsigned int *SectionTableAddr;
    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *RESET_CONTROL = (unsigned int *) 0x40053100;
    volatile unsigned int *NVIC_ICPR = (unsigned int *) 0xE000E280;

    /* Chip cleanup/reset */
    __asm volatile ("cpsid i"); /* Disable interrupts */

    /* Write to LPC_RGU->RESET_CTRL0 */
    *(RESET_CONTROL+0) = 0x10DF0000;
    /* GPIO_RST|AES_RST|ETHERNET_RST|SDIO_RST|DMA_RST|
     * USB1_RST|USB0_RST|LCD_RST */

    /* Write to LPC_RGU->RESET_CTRL1 */
    *(RESET_CONTROL+1) = 0x00DFF7FF;
    /* CAN0_RST|CAN1_RST|I2S_RST|SSP1_RST|SSP0_RST|
     * I2C1_RST|I2C0_RST|UART3_RST|UART1_RST|UART1_RST|UART0_RST|
     * DAC_RST|ADC1_RST|ADC0_RST|QEI_RST|MOTOCONPWM_RST|SCT_RST|
     * RITIMER_RST|TIMER3_RST|TIMER2_RST|TIMER1_RST|TIMER0_RST */

    /* Clear all pending interrupts in the NVIC */
    for (irqPendLoop = 0; irqPendLoop < 8; irqPendLoop++) {
        *(NVIC_ICPR + irqPendLoop) = 0xFFFFFFFF;
    }
    __asm volatile ("cpsie i"); /* Re-enable interrupts */

    /* Init sections */
    SectionTableAddr = &__data_section_table;
    /* Copy the data sections from flash to SRAM */
    while (SectionTableAddr < &__data_section_table_end) {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        memcpy32((uint32_t*)LoadAddr, (uint32_t*)ExeAddr, SectionLen);
    }
    /* Zero fill the bss segment */
    while (SectionTableAddr < &__bss_section_table_end) {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        meminit32((uint32_t*)ExeAddr, SectionLen);
    }

#if defined(__FPU_PRESENT) && __FPU_PRESENT == 1
    fpuInit();
#endif

    /* Board specific SystemInit */
    Board_SystemInit();

    /* Start main */
#if defined (__REDLIB__)
    /* Call the Redlib library, which in turn calls main() */
    extern void __main(void);
    __main() ;
#else
    extern void main(void);
    main();
#endif

    /* Application has ended, so busy wait */
    while(1) {};
}

/* Vector Exception/Interrupt Handlers */
__attribute__ ((section(".after_vectors")))
static void Default_Handler(void)
{
    /* Loop forever */
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

    /* Configurable Fault Status Register */
    /* Consists of MMSR, BFSR and UFSR */
    _CFSR = (*((volatile uint32_t *)(0xE000ED28)));

    /* Hard Fault Status Register */
    _HFSR = (*((volatile uint32_t *)(0xE000ED2C)));

    /* Debug Fault Status Register */
    _DFSR = (*((volatile uint32_t *)(0xE000ED30)));

    /* Auxiliary Fault Status Register */
    _AFSR = (*((volatile uint32_t *)(0xE000ED3C)));

    /* Read the Fault Address Registers. These may not contain valid values. */
    /* Check BFARVALID/MMARVALID to see if they are valid values */
    /* MemManage Fault Address Register */
    _MMAR = (*((volatile uint32_t *)(0xE000ED34)));
    /* Bus Fault Address Register */
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

    /* Break into the debugger */
    __asm("BKPT #0\n");
}

__attribute__( ( naked, section(".after_vectors") ) )
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

/* Forward declaration of IRQ handlers */
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

void NMI_Handler(void) ALIAS(Default_Handler);
void MemManage_Handler(void) ALIAS(Default_Handler);
void BusFault_Handler(void) ALIAS(Default_Handler);
void UsageFault_Handler(void) ALIAS(Default_Handler);
void SVC_Handler(void) ALIAS(Default_Handler);
void DebugMon_Handler(void) ALIAS(Default_Handler);
void PendSV_Handler(void) ALIAS(Default_Handler);
void SysTick_Handler(void) ALIAS(Default_Handler);

void DAC_IRQHandler(void) ALIAS(Default_Handler);
void DMA_IRQHandler(void) ALIAS(Default_Handler);
void FLASHEEPROM_IRQHandler(void) ALIAS(Default_Handler);
void ETH_IRQHandler(void) ALIAS(Default_Handler);
void SDIO_IRQHandler(void) ALIAS(Default_Handler);
void LCD_IRQHandler(void) ALIAS(Default_Handler);
void USB0_IRQHandler(void) ALIAS(Default_Handler);
void USB1_IRQHandler(void) ALIAS(Default_Handler);
void SCT_IRQHandler(void) ALIAS(Default_Handler);
void RIT_IRQHandler(void) ALIAS(Default_Handler);
void TIMER0_IRQHandler(void) ALIAS(Default_Handler);
void TIMER1_IRQHandler(void) ALIAS(Default_Handler);
void TIMER2_IRQHandler(void) ALIAS(Default_Handler);
void TIMER3_IRQHandler(void) ALIAS(Default_Handler);
void MCPWM_IRQHandler(void) ALIAS(Default_Handler);
void ADC0_IRQHandler(void) ALIAS(Default_Handler);
void I2C0_IRQHandler(void) ALIAS(Default_Handler);
void I2C1_IRQHandler(void) ALIAS(Default_Handler);
void ADC1_IRQHandler(void) ALIAS(Default_Handler);
void SSP0_IRQHandler(void) ALIAS(Default_Handler);
void SSP1_IRQHandler(void) ALIAS(Default_Handler);
void UART0_IRQHandler(void) ALIAS(Default_Handler);
void UART1_IRQHandler(void) ALIAS(Default_Handler);
void UART2_IRQHandler(void) ALIAS(Default_Handler);
void UART3_IRQHandler(void) ALIAS(Default_Handler);
void I2S0_IRQHandler(void) ALIAS(Default_Handler);
void I2S1_IRQHandler(void) ALIAS(Default_Handler);
void SPIFI_IRQHandler(void) ALIAS(Default_Handler);
void SGPIO_IRQHandler(void) ALIAS(Default_Handler);
void GPIO0_IRQHandler(void) ALIAS(Default_Handler);
void GPIO1_IRQHandler(void) ALIAS(Default_Handler);
void GPIO2_IRQHandler(void) ALIAS(Default_Handler);
void GPIO3_IRQHandler(void) ALIAS(Default_Handler);
void GPIO4_IRQHandler(void) ALIAS(Default_Handler);
void GPIO5_IRQHandler(void) ALIAS(Default_Handler);
void GPIO6_IRQHandler(void) ALIAS(Default_Handler);
void GPIO7_IRQHandler(void) ALIAS(Default_Handler);
void GINT0_IRQHandler(void) ALIAS(Default_Handler);
void GINT1_IRQHandler(void) ALIAS(Default_Handler);
void EVRT_IRQHandler(void) ALIAS(Default_Handler);
void CAN1_IRQHandler(void) ALIAS(Default_Handler);
void ATIMER_IRQHandler(void) ALIAS(Default_Handler);
void RTC_IRQHandler(void) ALIAS(Default_Handler);
void WDT_IRQHandler(void) ALIAS(Default_Handler);
void CAN0_IRQHandler(void) ALIAS(Default_Handler);
void QEI_IRQHandler(void) ALIAS(Default_Handler);

/* Vectors */
extern void (* const g_pfnVectors[])(void);
__attribute__ ((used,section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    // Core Level - CM3
    &_vStackTop,                // The initial stack pointer
    ResetISR,                   // The reset handler
    NMI_Handler,                // The NMI handler
    HardFault_Handler,          // The hard fault handler
    MemManage_Handler,          // The MPU fault handler
    BusFault_Handler,           // The bus fault handler
    UsageFault_Handler,         // The usage fault handler
    0,                          // Reserved
    0,                          // Reserved
    0,                          // Reserved
    0,                          // Reserved
    SVC_Handler,                // SVCall handler
    DebugMon_Handler,           // Debug monitor handler
    0,                          // Reserved
    PendSV_Handler,             // The PendSV handler
    SysTick_Handler,            // The SysTick handler

    // Chip Level - LPC18
    DAC_IRQHandler,             // 16
    0,                          // 17
    DMA_IRQHandler,             // 18
    0,                          // 19
    FLASHEEPROM_IRQHandler,     // 20
    ETH_IRQHandler,             // 21
    SDIO_IRQHandler,            // 22
    LCD_IRQHandler,             // 23
    USB0_IRQHandler,            // 24
    USB1_IRQHandler,            // 25
    SCT_IRQHandler,             // 26
    RIT_IRQHandler,             // 27
    TIMER0_IRQHandler,          // 28
    TIMER1_IRQHandler,          // 29
    TIMER2_IRQHandler,          // 30
    TIMER3_IRQHandler,          // 31
    MCPWM_IRQHandler,           // 32
    ADC0_IRQHandler,            // 33
    I2C0_IRQHandler,            // 34
    I2C1_IRQHandler,            // 35
    0,                          // 36
    ADC1_IRQHandler,            // 37
    SSP0_IRQHandler,            // 38
    SSP1_IRQHandler,            // 39
    UART0_IRQHandler,           // 40
    UART1_IRQHandler,           // 41
    UART2_IRQHandler,           // 42
    UART3_IRQHandler,           // 43
    I2S0_IRQHandler,            // 44
    I2S1_IRQHandler,            // 45
    SPIFI_IRQHandler,           // 46
    SGPIO_IRQHandler,           // 47
    GPIO0_IRQHandler,           // 48
    GPIO1_IRQHandler,           // 49
    GPIO2_IRQHandler,           // 50
    GPIO3_IRQHandler,           // 51
    GPIO4_IRQHandler,           // 52
    GPIO5_IRQHandler,           // 53
    GPIO6_IRQHandler,           // 54
    GPIO7_IRQHandler,           // 55
    GINT0_IRQHandler,           // 56
    GINT1_IRQHandler,           // 57
    EVRT_IRQHandler,            // 58
    CAN1_IRQHandler,            // 59
    0,                          // 60
    0,                          // 61
    ATIMER_IRQHandler,          // 62
    RTC_IRQHandler,             // 63
    0,                          // 64
    WDT_IRQHandler,             // 65
    0,                          // 66
    CAN0_IRQHandler,            // 67
    QEI_IRQHandler,             // 68
};
