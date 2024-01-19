#include "wifi_marauder_app_i.h"
#include "wifi_marauder_uart.h"

#define UART_CH (FuriHalSerialIdUsart)
#define LP_UART_CH (FuriHalSerialIdLpuart)
#define BAUDRATE (230400)

struct WifiMarauderUart {
    WifiMarauderApp* app;
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    FuriHalSerialHandle* serial_handle;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

void wifi_marauder_uart_set_handle_rx_data_cb(
    WifiMarauderUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

void wifi_marauder_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    WifiMarauderUart* uart = (WifiMarauderUart*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

void workaround_on_irq_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    // For some reason furi_hal_serial enforces different callbacks for usart/lpuart
    // The callbacks receive context and handle pointers, user callbacks can handle it
    // API really should not be doing this, if it gets fixed then revert this commit
    wifi_marauder_uart_on_irq_cb(handle, event, context);
}

static int32_t uart_worker(void* context) {
    WifiMarauderUart* uart = (void*)context;

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

void wifi_marauder_uart_tx(WifiMarauderUart* uart, uint8_t* data, size_t len) {
    furi_hal_serial_tx(uart->serial_handle, data, len);
}

WifiMarauderUart* wifi_marauder_uart_init(
    WifiMarauderApp* app,
    FuriHalSerialId channel,
    const char* thread_name) {
    WifiMarauderUart* uart = malloc(sizeof(WifiMarauderUart));

    uart->app = app;
    uart->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uart->rx_thread, thread_name);
    furi_thread_set_stack_size(uart->rx_thread, 1024);
    furi_thread_set_context(uart->rx_thread, uart);
    furi_thread_set_callback(uart->rx_thread, uart_worker);
    furi_thread_start(uart->rx_thread);
    uart->serial_handle = furi_hal_serial_control_acquire(channel);
    if(!uart->serial_handle) {
        furi_delay_ms(5000);
    }
    furi_check(uart->serial_handle);
    furi_hal_serial_init(uart->serial_handle, BAUDRATE);
    furi_hal_serial_async_rx_start(
        uart->serial_handle,
        channel == FuriHalSerialIdUsart ? workaround_on_irq_cb : wifi_marauder_uart_on_irq_cb,
        uart,
        false);

    return uart;
}

WifiMarauderUart* wifi_marauder_usart_init(WifiMarauderApp* app) {
    return wifi_marauder_uart_init(app, UART_CH, "WifiMarauderUartRxThread");
}

WifiMarauderUart* wifi_marauder_lp_uart_init(WifiMarauderApp* app) {
    return wifi_marauder_uart_init(app, LP_UART_CH, "WifiMarauderLPUartRxThread");
}

void wifi_marauder_uart_free(WifiMarauderUart* uart) {
    furi_assert(uart);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    furi_hal_serial_deinit(uart->serial_handle);
    furi_hal_serial_control_release(uart->serial_handle);

    free(uart);
}