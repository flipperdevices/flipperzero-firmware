// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021, Alex Taradov <alex@taradov.com>. All rights reserved.

//-----------------------------------------------------------------------------
#include <stdint.h>
#include "rp2040.h"

//-----------------------------------------------------------------------------
#define DUMMY __attribute__ ((weak, alias ("irq_handler_dummy")))

//-----------------------------------------------------------------------------
DUMMY void irq_handler_nmi(void);
DUMMY void irq_handler_hard_fault(void);
DUMMY void irq_handler_sv_call(void);
DUMMY void irq_handler_pend_sv(void);
DUMMY void irq_handler_sys_tick(void);

DUMMY void irq_handler_timer_0(void);
DUMMY void irq_handler_timer_1(void);
DUMMY void irq_handler_timer_2(void);
DUMMY void irq_handler_timer_3(void);
DUMMY void irq_handler_pwm(void);
DUMMY void irq_handler_usbctrl(void);
DUMMY void irq_handler_xip(void);
DUMMY void irq_handler_pio0_0(void);
DUMMY void irq_handler_pio0_1(void);
DUMMY void irq_handler_pio1_0(void);
DUMMY void irq_handler_pio1_1(void);
DUMMY void irq_handler_dma_0(void);
DUMMY void irq_handler_dma_1(void);
DUMMY void irq_handler_io_bank0(void);
DUMMY void irq_handler_io_qspi(void);
DUMMY void irq_handler_sio_proc0(void);
DUMMY void irq_handler_sio_proc1(void);
DUMMY void irq_handler_clocks(void);
DUMMY void irq_handler_spi0(void);
DUMMY void irq_handler_spi1(void);
DUMMY void irq_handler_uart0(void);
DUMMY void irq_handler_uart1(void);
DUMMY void irq_handler_adc_fifo(void);
DUMMY void irq_handler_i2c0(void);
DUMMY void irq_handler_i2c1(void);
DUMMY void irq_handler_rtc(void);

extern int main(void);

extern uint32_t _text_start;
extern uint32_t _stack_top;
extern uint32_t _text;
extern uint32_t _etext;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

//-----------------------------------------------------------------------------
// Note: Vector table does not need to be placed in a separate section on this MCU,
//       but we do so anyway to ensure alignment at 128 byte boundary without
//       potentially wasting space with automatic alignment.
__attribute__ ((used, section(".vectors")))
void (* const vectors[])(void) =
{
  0,                             // 0 - Initial Stack Pointer Value (unused)

  // Cortex-M0+ handlers
  0,                             // 1 - Reset (unused)
  irq_handler_nmi,               // 2 - NMI
  irq_handler_hard_fault,        // 3 - Hard Fault
  0,                             // 4 - Reserved
  0,                             // 5 - Reserved
  0,                             // 6 - Reserved
  0,                             // 7 - Reserved
  0,                             // 8 - Reserved
  0,                             // 9 - Reserved
  0,                             // 10 - Reserved
  irq_handler_sv_call,           // 11 - SVCall
  0,                             // 12 - Reserved
  0,                             // 13 - Reserved
  irq_handler_pend_sv,           // 14 - PendSV
  irq_handler_sys_tick,          // 15 - SysTick

  // Peripheral handlers
  irq_handler_timer_0,           // 0 - timer_irq_0
  irq_handler_timer_1,           // 1 - timer_irq_1
  irq_handler_timer_2,           // 2 - timer_irq_2
  irq_handler_timer_3,           // 3 - timer_irq_3
  irq_handler_pwm,               // 4 - pwm_irq_wrap
  irq_handler_usbctrl,           // 5 - usbctrl_irq
  irq_handler_xip,               // 6 - xip_irq
  irq_handler_pio0_0,            // 7 - pio0_irq_0
  irq_handler_pio0_1,            // 8 - pio0_irq_1
  irq_handler_pio1_0,            // 9 - pio1_irq_0
  irq_handler_pio1_1,            // 10 - pio1_irq_1
  irq_handler_dma_0,             // 11 - dma_irq_0
  irq_handler_dma_1,             // 12 - dma_irq_1
  irq_handler_io_bank0,          // 13 - io_irq_bank0
  irq_handler_io_qspi,           // 14 - io_irq_qspi
  irq_handler_sio_proc0,         // 15 - sio_irq_proc0
  irq_handler_sio_proc1,         // 16 - sio_irq_proc1
  irq_handler_clocks,            // 17 - clocks_irq
  irq_handler_spi0,              // 18 - spi0_irq
  irq_handler_spi1,              // 19 - spi1_irq
  irq_handler_uart0,             // 20 - uart0_irq
  irq_handler_uart1,             // 21 - uart1_irq
  irq_handler_adc_fifo,          // 22 - adc_irq_fifo
  irq_handler_i2c0,              // 23 - i2c0_irq
  irq_handler_i2c1,              // 24 - i2c1_irq
  irq_handler_rtc,               // 25 - rtc_irq
};

//-----------------------------------------------------------------------------
void irq_handler_dummy(void)
{
  while (1);
}

//-----------------------------------------------------------------------------
__attribute__((naked, used, noreturn, section(".boot.entry"))) void boot_entry(void)
{
  // Note: This code must be position independent, it is linked at 0x10000000, but
  //       loaded at 0x20041f00.
  XIP_SSI->SSIENR = 0;

  XIP_SSI->BAUDR = 2; // Must be even

  // Read unique ID
  XIP_SSI->RX_SAMPLE_DLY = (1 << XIP_SSI_RX_SAMPLE_DLY_RSD_Pos);

  XIP_SSI->CTRLR0 = (XIP_SSI_CTRLR0_SPI_FRF_STD << XIP_SSI_CTRLR0_SPI_FRF_Pos) |
      (XIP_SSI_CTRLR0_TMOD_TX_AND_RX << XIP_SSI_CTRLR0_TMOD_Pos) |
      ((32-1) << XIP_SSI_CTRLR0_DFS_32_Pos);

  XIP_SSI->SSIENR = XIP_SSI_SSIENR_SSI_EN_Msk;

  XIP_SSI->DR0 = 0x4b000000/*RUID*/;
  XIP_SSI->DR0 = 0;
  XIP_SSI->DR0 = 0;
  XIP_SSI->DR0 = 0;
  XIP_SSI->DR0 = 0;
  XIP_SSI->DR0 = 0;

  while (1)
  {
    int sr = XIP_SSI->SR;
    if ((sr & XIP_SSI_SR_TFE_Msk) && (0 == (sr & XIP_SSI_SR_BUSY_Msk)))
      break;
  }

  (void)XIP_SSI->DR0;

  // UID is placed at the same address as this boot sector
  volatile uint32_t *uid = (volatile uint32_t *)0x20041f00;
  uid[0] = XIP_SSI->DR0;
  uid[1] = XIP_SSI->DR0;
  uid[2] = XIP_SSI->DR0;
  uid[3] = XIP_SSI->DR0;
  uid[4] = XIP_SSI->DR0;

  XIP_SSI->SSIENR = 0;

  // Setup for XIP
  XIP_SSI->CTRLR0 = (XIP_SSI_CTRLR0_SPI_FRF_STD << XIP_SSI_CTRLR0_SPI_FRF_Pos) |
      (XIP_SSI_CTRLR0_TMOD_EEPROM_READ << XIP_SSI_CTRLR0_TMOD_Pos) |
      ((32-1) << XIP_SSI_CTRLR0_DFS_32_Pos);

  XIP_SSI->CTRLR1 = (0 << XIP_SSI_CTRLR1_NDF_Pos);

  XIP_SSI->SPI_CTRLR0 = (0x03/*READ_DATA*/ << XIP_SSI_SPI_CTRLR0_XIP_CMD_Pos) |
    ((24 / 4) << XIP_SSI_SPI_CTRLR0_ADDR_L_Pos) |
    (XIP_SSI_SPI_CTRLR0_INST_L_8B << XIP_SSI_SPI_CTRLR0_INST_L_Pos) |
    (XIP_SSI_SPI_CTRLR0_TRANS_TYPE_1C1A << XIP_SSI_SPI_CTRLR0_TRANS_TYPE_Pos);

  XIP_SSI->SSIENR = XIP_SSI_SSIENR_SSI_EN_Msk;

  uint32_t *src = &_text_start;
  uint32_t *dst = &_text;

  while (dst < &_edata)
    *dst++ = *src++;

  dst = &_bss;
  while (dst < &_ebss)
    *dst++ = 0;

  SCB->VTOR = (uint32_t)vectors;

  asm (R"asm(
    msr    msp, %[sp]
    bx     %[reset]
    )asm"
    :: [sp] "r" (&_stack_top), [reset] "r" (main)
  );

  __builtin_unreachable();
}
