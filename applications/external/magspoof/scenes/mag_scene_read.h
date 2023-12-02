#pragma once

#include <gui/modules/text_box.h>

#define UART_RX_BUF_SIZE (320)
#define UART_TERMINAL_TEXT_BOX_STORE_SIZE (4096)
#define UART_TERMINAL_TEXT_INPUT_STORE_SIZE (512)
#define UART_CH (FuriHalUartIdUSART1)
#define UART_BAUDRATE (9600)

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

typedef enum {
    UARTEventRxData = 100,
} UARTEvents;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)
