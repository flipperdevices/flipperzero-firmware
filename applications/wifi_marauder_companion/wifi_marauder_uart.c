#include "wifi_marauder_uart.h"

#include <stream_buffer.h>

#define UART_CH (FuriHalUartIdUSART1)
#define BAUDRATE (115200)
#define RX_BUF_SIZE (64)

struct WifiMarauderUart {
    StreamBufferHandle_t rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE];
};

void wifi_marauder_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    WifiMarauderUart* uart = (WifiMarauderUart*)context;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if(ev == UartIrqEventRXNE) {
        xStreamBufferSendFromISR(uart->rx_stream, &data, 1, &xHigherPriorityTaskWoken);
        //furi_thread_flags_set(furi_thread_get_id(usb_uart->thread), WorkerEvtRxDone);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// TODO: need rx worker thread to receive into rx_stream / rx_buf
// add a "Console" menu item to print output?

void wifi_marauder_uart_tx(uint8_t *data, size_t len) {
    furi_hal_uart_tx(UART_CH, data, len);
}

WifiMarauderUart* wifi_marauder_uart_init() {
    furi_hal_console_disable();
    furi_hal_uart_set_br(UART_CH, BAUDRATE);

    WifiMarauderUart *uart = malloc(sizeof(WifiMarauderUart));
    uart->rx_stream = xStreamBufferCreate(RX_BUF_SIZE, 1);

    return uart;
}

void wifi_marauder_uart_free(WifiMarauderUart* uart) {
    furi_assert(uart);
    vStreamBufferDelete(uart->rx_stream);
    free(uart);
}