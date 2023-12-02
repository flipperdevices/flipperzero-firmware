#pragma once

#include <stdlib.h> // malloc
#include <stdint.h> // uint32_t
#include <stdarg.h> // __VA_ARGS__
#include <string.h>
#include <stdio.h>

#include <furi.h>
#include <furi_hal.h>

#define SEADER_UART_RX_BUF_SIZE (128)

typedef struct {
    uint8_t uart_ch;
    uint8_t flow_pins;
    uint8_t baudrate_mode;
    uint32_t baudrate;
} SeaderUartConfig;

typedef struct {
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint8_t protocol;
} SeaderUartState;

struct SeaderUartBridge {
    SeaderUartConfig cfg;
    SeaderUartConfig cfg_new;

    FuriThread* thread;
    FuriThread* tx_thread;

    FuriStreamBuffer* rx_stream;

    FuriSemaphore* tx_sem;

    SeaderUartState st;

    uint8_t rx_buf[SEADER_UART_RX_BUF_SIZE];
    uint8_t tx_buf[SEADER_UART_RX_BUF_SIZE];
    size_t tx_len;
};

typedef struct SeaderUartBridge SeaderUartBridge;
