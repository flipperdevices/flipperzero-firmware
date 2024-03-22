#include "esp_flasher_app_i.h"
#include "esp_flasher_uart.h"

struct EspFlasherUart {
    EspFlasherApp* app;
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
    FuriHalSerialHandle* serial_handle;
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

void esp_flasher_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    EspFlasherUart* uart = (EspFlasherUart*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
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

void esp_flasher_uart_tx(EspFlasherUart* uart, uint8_t* data, size_t len) {
    furi_hal_serial_tx(uart->serial_handle, data, len);
}

void esp_flasher_uart_set_br(EspFlasherUart* uart, uint32_t baud) {
    furi_hal_serial_set_br(uart->serial_handle, baud);
}

EspFlasherUart*
    esp_flasher_uart_init(EspFlasherApp* app, FuriHalSerialId channel, const char* thread_name) {
    EspFlasherUart* uart = malloc(sizeof(EspFlasherUart));

    uart->app = app;
    uart->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uart->rx_thread, thread_name);
    furi_thread_set_stack_size(uart->rx_thread, 1024);
    furi_thread_set_context(uart->rx_thread, uart);
    furi_thread_set_callback(uart->rx_thread, uart_worker);
    furi_thread_start(uart->rx_thread);
    uart->serial_handle = furi_hal_serial_control_acquire(channel);
    furi_check(uart->serial_handle);
    furi_hal_serial_init(uart->serial_handle, BAUDRATE);
    furi_hal_serial_async_rx_start(uart->serial_handle, esp_flasher_uart_on_irq_cb, uart, false);

    return uart;
}

EspFlasherUart* esp_flasher_usart_init(EspFlasherApp* app) {
    return esp_flasher_uart_init(app, UART_CH, "EspFlasherUartRxThread");
}

void esp_flasher_uart_free(EspFlasherUart* uart) {
    furi_assert(uart);

    furi_hal_serial_async_rx_stop(uart->serial_handle);
    furi_hal_serial_deinit(uart->serial_handle);
    furi_hal_serial_control_release(uart->serial_handle);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    free(uart);
}
