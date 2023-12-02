// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2022, Alex Taradov <alex@taradov.com>. All rights reserved.

#ifndef _UART_H_
#define _UART_H_

/*- Includes ----------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "usb_cdc.h"

/*- Prototypes --------------------------------------------------------------*/
void uart_init(usb_cdc_line_coding_t *line_coding);
void uart_close(void);
bool uart_write_byte(int byte);
bool uart_read_byte(int *byte);
void uart_set_break(bool brk);

#endif // _UART_H_

