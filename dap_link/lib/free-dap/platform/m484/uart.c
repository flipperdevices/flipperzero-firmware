// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "M480.h"
#include "hal_config.h"
#include "uart.h"
#include "usb_cdc.h"

#ifdef HAL_CONFIG_ENABLE_VCP

/*- Definitions -------------------------------------------------------------*/
#define UART_BUF_SIZE            256

/*- Types ------------------------------------------------------------------*/
typedef struct
{
  int       wr;
  int       rd;
  uint16_t  data[UART_BUF_SIZE];
} fifo_buffer_t;

/*- Variables --------------------------------------------------------------*/
static volatile fifo_buffer_t uart_rx_fifo;
static volatile fifo_buffer_t uart_tx_fifo;
static volatile bool uart_fifo_overflow = false;

/*- Implementations ---------------------------------------------------------*/

//-----------------------------------------------------------------------------
void uart_init(usb_cdc_line_coding_t *line_coding)
{
  int wls, parity, nsb;

  CLK->UART_CLKSEL_REG = (CLK->UART_CLKSEL_REG & ~UART_CLKSEL_MSK) | (1/*PLL*/ << UART_CLKSEL_POS);
  CLK->APBCLK0 |= UART_APBCLK_EN;

  HAL_GPIO_UART_RX_pullup();
  HAL_GPIO_UART_RX_mfp(UART_RX_MPF);

  HAL_GPIO_UART_TX_pullup();
  HAL_GPIO_UART_TX_mfp(UART_TX_MPF);

  uart_tx_fifo.wr = 0;
  uart_tx_fifo.rd = 0;

  uart_rx_fifo.wr = 0;
  uart_rx_fifo.rd = 0;

  uart_fifo_overflow = false;

  if (USB_CDC_5_DATA_BITS == line_coding->bDataBits)
    wls = 0;
  else if (USB_CDC_6_DATA_BITS == line_coding->bDataBits)
    wls = 1;
  else if (USB_CDC_7_DATA_BITS == line_coding->bDataBits)
    wls = 2;
  else if (USB_CDC_8_DATA_BITS == line_coding->bDataBits)
    wls = 3;
  else
    wls = 3;

  if (USB_CDC_NO_PARITY == line_coding->bParityType)
    parity = 0;
  else if (USB_CDC_ODD_PARITY == line_coding->bParityType)
    parity = UART_LINE_PBE_Msk;
  else if (USB_CDC_EVEN_PARITY == line_coding->bParityType)
    parity = UART_LINE_PBE_Msk | UART_LINE_EPE_Msk;
  else if (USB_CDC_MARK_PARITY == line_coding->bParityType)
    parity = UART_LINE_PBE_Msk | UART_LINE_SPE_Msk;
  else if (USB_CDC_SPACE_PARITY == line_coding->bParityType)
    parity = UART_LINE_PBE_Msk | UART_LINE_SPE_Msk | UART_LINE_EPE_Msk;
  else
    parity = 0;

  if (USB_CDC_1_STOP_BIT == line_coding->bCharFormat)
    nsb = 0;
  else
    nsb = UART_LINE_NSB_Msk;;

  UART_PER->LINE = (wls << UART_LINE_WLS_Pos) | parity | nsb;

  UART_PER->BAUD = UART_BAUD_BAUDM0_Msk | UART_BAUD_BAUDM1_Msk | (UART_CLOCK / line_coding->dwDTERate - 2);

  UART_PER->INTEN = UART_INTEN_RDAIEN_Msk;

  NVIC_EnableIRQ(UART_IRQ_INDEX);
}

//-----------------------------------------------------------------------------
void uart_close(void)
{
  NVIC_DisableIRQ(UART_IRQ_INDEX);

  UART_PER->INTEN = 0;

  HAL_GPIO_UART_RX_mfp(0);
  HAL_GPIO_UART_TX_mfp(0);
}

//-----------------------------------------------------------------------------
bool uart_write_byte(int byte)
{
  int wr = (uart_tx_fifo.wr + 1) % UART_BUF_SIZE;
  bool res = false;

  NVIC_DisableIRQ(UART_IRQ_INDEX);

  if (wr != uart_tx_fifo.rd)
  {
    uart_tx_fifo.data[uart_tx_fifo.wr] = byte;
    uart_tx_fifo.wr = wr;
    res = true;

    UART_PER->INTEN |= UART_INTEN_THREIEN_Msk;
  }

  NVIC_EnableIRQ(UART_IRQ_INDEX);

  return res;
}

//-----------------------------------------------------------------------------
bool uart_read_byte(int *byte)
{
  bool res = false;

  NVIC_DisableIRQ(UART_IRQ_INDEX);

  if (uart_fifo_overflow)
  {
    *byte = (USB_CDC_SERIAL_STATE_OVERRUN << 8);
    uart_fifo_overflow = false;
    res = true;
  }
  else if (uart_rx_fifo.rd != uart_rx_fifo.wr)
  {
    *byte = uart_rx_fifo.data[uart_rx_fifo.rd];
    uart_rx_fifo.rd = (uart_rx_fifo.rd + 1) % UART_BUF_SIZE;
    res = true;
  }

  NVIC_EnableIRQ(UART_IRQ_INDEX);

  return res;
}

//-----------------------------------------------------------------------------
void uart_set_break(bool brk)
{
  if (brk)
    UART_PER->LINE |= UART_LINE_BCB_Msk;
  else
    UART_PER->LINE &= ~UART_LINE_BCB_Msk;
}

//-----------------------------------------------------------------------------
void UART_IRQ_HANDLER(void)
{
  uint32_t flags = UART_PER->INTSTS;

  if (flags & UART_INTSTS_RDAIF_Msk)
  {
    int status = UART_PER->FIFOSTS;
    int byte = UART_PER->DAT & UART_DAT_DAT_Msk;
    int wr = (uart_rx_fifo.wr + 1) % UART_BUF_SIZE;
    int state = 0;

    UART_PER->FIFOSTS = status;

    if (status & UART_FIFOSTS_RXOVIF_Msk)
      state |= USB_CDC_SERIAL_STATE_OVERRUN;

    if (status & UART_FIFOSTS_FEF_Msk)
      state |= USB_CDC_SERIAL_STATE_FRAMING;

    if (status & UART_FIFOSTS_PEF_Msk)
      state |= USB_CDC_SERIAL_STATE_PARITY;

    if (status & UART_FIFOSTS_BIF_Msk)
      state |= USB_CDC_SERIAL_STATE_BREAK;

    byte |= (state << 8);

    if (wr == uart_rx_fifo.rd)
    {
      uart_fifo_overflow = true;
    }
    else
    {
      uart_rx_fifo.data[uart_rx_fifo.wr] = byte;
      uart_rx_fifo.wr = wr;
    }
  }

  if (flags & UART_INTSTS_THREINT_Msk)
  {
    if (uart_tx_fifo.rd == uart_tx_fifo.wr)
    {
      UART_PER->INTEN &= ~UART_INTEN_THREIEN_Msk;
    }
    else
    {
      UART_PER->DAT = uart_tx_fifo.data[uart_tx_fifo.rd];
      uart_tx_fifo.rd = (uart_tx_fifo.rd + 1) % UART_BUF_SIZE;
    }
  }
}

#endif // HAL_CONFIG_ENABLE_VCP

