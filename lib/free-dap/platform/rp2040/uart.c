// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

/*- Includes ----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rp2040.h"
#include "hal_config.h"
#include "uart.h"
#include "usb_cdc.h"

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
  int wlen, parity, stop, baud;

  RESETS_SET->RESET = UART_RESET_MASK;
  RESETS_CLR->RESET = UART_RESET_MASK;
  while (0 == (RESETS->RESET_DONE & UART_RESET_MASK));

  uart_tx_fifo.wr = 0;
  uart_tx_fifo.rd = 0;

  uart_rx_fifo.wr = 0;
  uart_rx_fifo.rd = 0;

  uart_fifo_overflow = false;

  if (USB_CDC_5_DATA_BITS == line_coding->bDataBits)
    wlen = 0;
  else if (USB_CDC_6_DATA_BITS == line_coding->bDataBits)
    wlen = 1;
  else if (USB_CDC_7_DATA_BITS == line_coding->bDataBits)
    wlen = 2;
  else if (USB_CDC_8_DATA_BITS == line_coding->bDataBits)
    wlen = 3;
  else
    wlen = 3;

  if (USB_CDC_NO_PARITY == line_coding->bParityType)
    parity = 0;
  else if (USB_CDC_ODD_PARITY == line_coding->bParityType)
    parity = UART0_UARTLCR_H_PEN_Msk;
  else if (USB_CDC_EVEN_PARITY == line_coding->bParityType)
    parity = UART0_UARTLCR_H_PEN_Msk | UART0_UARTLCR_H_EPS_Msk;
  else if (USB_CDC_MARK_PARITY == line_coding->bParityType)
    parity = UART0_UARTLCR_H_PEN_Msk | UART0_UARTLCR_H_SPS_Msk;
  else if (USB_CDC_SPACE_PARITY == line_coding->bParityType)
    parity = UART0_UARTLCR_H_PEN_Msk | UART0_UARTLCR_H_SPS_Msk | UART0_UARTLCR_H_EPS_Msk;
  else
    parity = 0;

  if (USB_CDC_1_STOP_BIT == line_coding->bCharFormat)
    stop = 0;
  else
    stop = UART0_UARTLCR_H_STP2_Msk;

  baud = (UART_CLOCK * 4) / line_coding->dwDTERate;

  UART_PER->UARTIFLS = (3 << UART0_UARTIFLS_RXIFLSEL_Pos) | (0 << UART0_UARTIFLS_TXIFLSEL_Pos);

  UART_PER->UARTIBRD = baud / 64;
  UART_PER->UARTFBRD = baud % 64;

  UART_PER->UARTLCR_H = (wlen << UART0_UARTLCR_H_WLEN_Pos) | parity | stop | UART0_UARTLCR_H_FEN_Msk;

  UART_PER->UARTIMSC = UART0_UARTIMSC_RXIM_Msk | UART0_UARTIMSC_RTIM_Msk | UART0_UARTIMSC_TXIM_Msk;

  HAL_GPIO_UART_TX_init();
  HAL_GPIO_UART_TX_pullup();

  HAL_GPIO_UART_RX_init();
  HAL_GPIO_UART_RX_pullup();

  UART_PER->UARTCR = UART0_UARTCR_UARTEN_Msk | UART0_UARTCR_RXE_Msk | UART0_UARTCR_TXE_Msk;

  NVIC_EnableIRQ(UART_IRQ_INDEX);
}

//-----------------------------------------------------------------------------
void uart_close(void)
{
  RESETS_SET->RESET = UART_RESET_MASK;
}

//-----------------------------------------------------------------------------
bool uart_write_byte(int byte)
{
  int wr = (uart_tx_fifo.wr + 1) % UART_BUF_SIZE;
  bool res = false;

  NVIC_DisableIRQ(UART_IRQ_INDEX);

  if (uart_tx_fifo.rd == uart_tx_fifo.wr && !UART_PER->UARTFR_b.TXFF)
  {
    UART_PER->UARTDR = byte;
    res = true;
  }
  else if (wr != uart_tx_fifo.rd)
  {
    uart_tx_fifo.data[uart_tx_fifo.wr] = byte;
    uart_tx_fifo.wr = wr;
    res = true;
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
    UART_PER->UARTLCR_H |= UART0_UARTLCR_H_BRK_Msk;
  else
    UART_PER->UARTLCR_H &= ~UART0_UARTLCR_H_BRK_Msk;
}

//-----------------------------------------------------------------------------
void UART_IRQ_HANDLER(void)
{
  int flags = UART_PER->UARTMIS;

  if (flags & (UART0_UARTIMSC_RXIM_Msk | UART0_UARTIMSC_RTIM_Msk))
  {
    while (!UART_PER->UARTFR_b.RXFE)
    {
      int byte = UART_PER->UARTDR;
      int wr = (uart_rx_fifo.wr + 1) % UART_BUF_SIZE;
      int state = 0;

      if (byte & UART0_UARTDR_BE_Msk)
      {
        state |= USB_CDC_SERIAL_STATE_BREAK;
        byte &= ~(UART0_UARTDR_PE_Msk | UART0_UARTDR_FE_Msk);
      }

      if (byte & UART0_UARTDR_PE_Msk)
        state |= USB_CDC_SERIAL_STATE_PARITY;

      if (byte & UART0_UARTDR_FE_Msk)
        state |= USB_CDC_SERIAL_STATE_FRAMING;

      if (byte & UART0_UARTDR_OE_Msk)
        state |= USB_CDC_SERIAL_STATE_OVERRUN;

      byte = (state << 8) | (byte & UART0_UARTDR_DATA_Msk);

      if (wr == uart_rx_fifo.rd)
      {
        uart_fifo_overflow = true;
        break;
      }
      else
      {
        uart_rx_fifo.data[uart_rx_fifo.wr] = byte;
        uart_rx_fifo.wr = wr;
      }
    }
  }

  if (flags & UART0_UARTIMSC_TXIM_Msk)
  {
    UART_PER->UARTICR = UART0_UARTICR_TXIC_Msk;

    while (!UART_PER->UARTFR_b.TXFF)
    {
      if (uart_tx_fifo.rd == uart_tx_fifo.wr)
        break;

      UART_PER->UARTDR = uart_tx_fifo.data[uart_tx_fifo.rd];
      uart_tx_fifo.rd = (uart_tx_fifo.rd + 1) % UART_BUF_SIZE;
    }
  }
}


