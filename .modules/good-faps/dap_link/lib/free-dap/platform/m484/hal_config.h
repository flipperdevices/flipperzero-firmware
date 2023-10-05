// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _HAL_CONFIG_H_
#define _HAL_CONFIG_H_

/*- Includes ----------------------------------------------------------------*/
#include "M480.h"
#include "hal_gpio.h"

/*- Definitions -------------------------------------------------------------*/
//#define HAL_BOARD_GENERIC
#define HAL_BOARD_M484_DAP

#if defined(HAL_BOARD_GENERIC)
  #define HAL_CONFIG_ENABLE_VCP

  HAL_GPIO_PIN(SWCLK_TCK,      B, 0)
  HAL_GPIO_PIN(SWDIO_TMS,      B, 1)
  HAL_GPIO_PIN(TDI,            B, 2)
  HAL_GPIO_PIN(TDO,            B, 3)
  HAL_GPIO_PIN(nRESET,         B, 4)

  HAL_GPIO_PIN(VCP_STATUS,     A, 5);
  HAL_GPIO_PIN(DAP_STATUS,     C, 14);

  HAL_GPIO_PIN(BOOT_ENTER,     A, 7);

  HAL_GPIO_PIN(UART_RX,        A, 0)
  HAL_GPIO_PIN(UART_TX,        A, 1)

  #define UART_PER             UART0
  #define UART_RX_MPF          7
  #define UART_TX_MPF          7
  #define UART_APBCLK_EN       CLK_APBCLK0_UART0CKEN_Msk
  #define UART_CLKSEL_REG      CLKSEL1
  #define UART_CLKSEL_POS      CLK_CLKSEL1_UART0SEL_Pos
  #define UART_CLKSEL_MSK      CLK_CLKSEL1_UART0SEL_Msk
  #define UART_IRQ_INDEX       UART0_IRQn
  #define UART_IRQ_HANDLER     irq_handler_uart0
  #define UART_CLOCK           192000000

#elif defined(HAL_BOARD_M484_DAP)
  HAL_GPIO_PIN(SWCLK_TCK,      B, 2)
  HAL_GPIO_PIN(SWDIO_TMS,      B, 5)
  HAL_GPIO_PIN(TDI,            A, 8)
  HAL_GPIO_PIN(TDO,            A, 11)
  HAL_GPIO_PIN(nRESET,         F, 5)

  HAL_GPIO_PIN(DAP_STATUS,     B, 7);

  HAL_GPIO_PIN(BOOT_ENTER,     A, 6);

#else
  #error No board defined
#endif

#endif // _HAL_CONFIG_H_

