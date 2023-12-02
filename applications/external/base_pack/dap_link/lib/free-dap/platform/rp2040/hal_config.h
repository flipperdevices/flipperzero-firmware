// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _HAL_CONFIG_H_
#define _HAL_CONFIG_H_

/*- Includes ----------------------------------------------------------------*/
#include "hal_gpio.h"

/*- Definitions -------------------------------------------------------------*/
HAL_GPIO_PIN(SWCLK_TCK,      0, 11, sio_11)
HAL_GPIO_PIN(SWDIO_TMS,      0, 12, sio_12)
HAL_GPIO_PIN(TDI,            0, 13, sio_13)
HAL_GPIO_PIN(TDO,            0, 14, sio_14)
HAL_GPIO_PIN(nRESET,         0, 15, sio_15)

HAL_GPIO_PIN(VCP_STATUS,     0, 2, sio_2);
HAL_GPIO_PIN(DAP_STATUS,     0, 25, sio_25);

HAL_GPIO_PIN(UART_TX,        0, 0, uart0_tx)
HAL_GPIO_PIN(UART_RX,        0, 1, uart0_rx)

#define UART_PER             UART0
#define UART_RESET_MASK      RESETS_RESET_uart0_Msk
#define UART_IRQ_INDEX       UART0_IRQ_IRQn
#define UART_IRQ_HANDLER     irq_handler_uart0
#define UART_CLOCK           120000000

#endif // _HAL_CONFIG_H_


