#include "esp_flasher_app_i.h"
#include "esp_flasher_uart.h"

#define UART_CH (FuriHalUartIdUSART1)
#define BAUDRATE (115200)

struct EspFlasherUart {
    EspFlasherApp* app;
    FuriHalUartId channel;
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

void esp_flasher_uart_set_handle_rx_data_cb(
    EspFlasherUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

void esp_flasher_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    EspFlasherUart* uart = (EspFlasherUart*)context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

static int32_t uart_worker(void* context) {
    EspFlasherUart* uart = (void*)context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);
            if(len > 0) {
                if(uart->handle_rx_data_cb) uart->handle_rx_data_cb(uart->rx_buf, len, uart->app);
            }
        }
    }

    furi_stream_buffer_free(uart->rx_stream);

    return 0;
}

void esp_flasher_uart_tx(uint8_t* data, size_t len) {
    furi_hal_uart_tx(UART_CH, data, len);
}

EspFlasherUart*
    esp_flasher_uart_init(EspFlasherApp* app, FuriHalUartId channel, const char* thread_name) {
    EspFlasherUart* uart = malloc(sizeof(EspFlasherUart));

    uart->app = app;
    uart->channel = channel;
    uart->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uart->rx_thread, thread_name);
    furi_thread_set_stack_size(uart->rx_thread, 1024);
    furi_thread_set_context(uart->rx_thread, uart);
    furi_thread_set_callback(uart->rx_thread, uart_worker);
    furi_thread_start(uart->rx_thread);
    if(channel == FuriHalUartIdUSART1) {
        furi_hal_console_disable();
    } else if(channel == FuriHalUartIdLPUART1) {
        furi_hal_uart_init(channel, BAUDRATE);
    }
    furi_hal_uart_set_br(channel, BAUDRATE);
    furi_hal_uart_set_irq_cb(channel, esp_flasher_uart_on_irq_cb, uart);

    return uart;
}

EspFlasherUart* esp_flasher_usart_init(EspFlasherApp* app) {
    return esp_flasher_uart_init(app, UART_CH, "EspFlasherUartRxThread");
}

void esp_flasher_uart_free(EspFlasherUart* uart) {
    furi_assert(uart);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    furi_hal_uart_set_irq_cb(uart->channel, NULL, NULL);
    if(uart->channel == FuriHalUartIdLPUART1) {
        furi_hal_uart_deinit(uart->channel);
    }
    furi_hal_console_enable();

    free(uart);
}
