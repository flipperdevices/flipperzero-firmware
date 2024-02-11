#pragma once
#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include "uhf_buffer.h"

#define UHF_UART_RX_BUFFER_SIZE 2048
#define UHF_UART_WORKER_STACK_SIZE 1024
#define UHF_UART_DEFAULT_BAUDRATE 115200
#define UHF_UART_FRAME_START 0xBB
#define UHF_UART_FRAME_END 0x7E
#define UHF_UART_WAIT_TICK 1000

typedef void (*CallbackFunction)(uint8_t *data, void *ctx);

typedef enum{
    UHFUartWorkerWaitingFlag,
    UHFUartWorkerExitingFlag
}UHFUartWorkerEventFlag;

static void uhf_uart_received_byte_callback(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void *ctx);

typedef struct{
    FuriHalBus bus;
    FuriHalSerialHandle *handle;
    FuriThread *thread;
    CallbackFunction callback;
    Buffer *buffer;
    uint16_t baudrate;
    bool init_by_app;
    void *ctx;
    volatile int tick;
} UHFUart;

UHFUart* uhf_uart_alloc();
void uhf_uart_free(UHFUart* uart);
void uhf_uart_send(UHFUart* uart, uint8_t* data, size_t size);
void uhf_uart_send_wait(UHFUart* uart, uint8_t* data, size_t size);
void uhf_uart_set_receive_byte_callback(UHFUart* uart, FuriHalSerialAsyncRxCallback callback, bool report_errors, void *ctx);
void uhf_uart_set_baudrate(UHFUart* uart, uint32_t baudrate);

