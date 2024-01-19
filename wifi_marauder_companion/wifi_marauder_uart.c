#include "wifi_marauder_app_i.h"
#include "wifi_marauder_uart.h"

#include <xtreme/xtreme.h>

#define UART_CH (xtreme_settings.uart_esp_channel)
#define BAUDRATE (115200)

struct WifiMarauderUart {
    WifiMarauderApp* app;
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    FuriStreamBuffer* pcap_stream;
    FuriHalSerialHandle* serial_handle;
    bool pcap;
    uint8_t mark_test_buf[11];
    uint8_t mark_test_idx;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
    void (*handle_rx_pcap_cb)(uint8_t* buf, size_t len, void* context);
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
    WorkerEvtPcapDone = (1 << 2),
} WorkerEvtFlags;

void wifi_marauder_uart_set_handle_rx_data_cb(
    WifiMarauderUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

void wifi_marauder_uart_set_handle_rx_pcap_cb(
    WifiMarauderUart* uart,
    void (*handle_rx_pcap_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_pcap_cb = handle_rx_pcap_cb;
}

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone | WorkerEvtPcapDone)

void wifi_marauder_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    WifiMarauderUart* uart = (WifiMarauderUart*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        const char* mark_begin = "[BUF/BEGIN]";
        const char* mark_close = "[BUF/CLOSE]";
        if(uart->mark_test_idx != 0) {
            // We are trying to match a marker
            if(data == mark_begin[uart->mark_test_idx] ||
               data == mark_close[uart->mark_test_idx]) {
                // Received char matches next char in a marker, append to test buffer
                uart->mark_test_buf[uart->mark_test_idx++] = data;
                if(uart->mark_test_idx == sizeof(uart->mark_test_buf)) {
                    // Test buffer reached max length, parse what marker this is and discard buffer
                    if(!memcmp(
                           uart->mark_test_buf, (void*)mark_begin, sizeof(uart->mark_test_buf))) {
                        uart->pcap = true;
                    } else if(!memcmp(
                                  uart->mark_test_buf,
                                  (void*)mark_close,
                                  sizeof(uart->mark_test_buf))) {
                        uart->pcap = false;
                    }
                    uart->mark_test_idx = 0;
                }
                // Don't pass to stream
                return;
            } else {
                // Received char doesn't match any expected next char, send current test buffer
                if(uart->pcap) {
                    furi_stream_buffer_send(
                        uart->pcap_stream, uart->mark_test_buf, uart->mark_test_idx, 0);
                    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtPcapDone);
                } else {
                    furi_stream_buffer_send(
                        uart->rx_stream, uart->mark_test_buf, uart->mark_test_idx, 0);
                    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
                }
                // Reset test buffer and try parsing this char from scratch
                uart->mark_test_idx = 0;
            }
        }
        // If we reach here the buffer is empty
        if(data == mark_begin[0]) {
            // Received marker start, append to test buffer
            uart->mark_test_buf[uart->mark_test_idx++] = data;
        } else {
            // Not a marker start and we aren't matching a marker, this is just data
            if(uart->pcap) {
                furi_stream_buffer_send(uart->pcap_stream, &data, 1, 0);
                furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtPcapDone);
            } else {
                furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
                furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
            }
        }
    }
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
        if(events & WorkerEvtPcapDone) {
            size_t len =
                furi_stream_buffer_receive(uart->pcap_stream, uart->rx_buf, RX_BUF_SIZE, 0);
            if(len > 0) {
                if(uart->handle_rx_pcap_cb) uart->handle_rx_pcap_cb(uart->rx_buf, len, uart->app);
            }
        }
    }

    furi_stream_buffer_free(uart->rx_stream);
    furi_stream_buffer_free(uart->pcap_stream);

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
    uart->pcap_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uart->rx_thread, thread_name);
    furi_thread_set_stack_size(uart->rx_thread, 1024);
    furi_thread_set_context(uart->rx_thread, uart);
    furi_thread_set_callback(uart->rx_thread, uart_worker);
    furi_thread_start(uart->rx_thread);
    uart->serial_handle = furi_hal_serial_control_acquire(channel);
    furi_check(uart->serial_handle);
    furi_hal_serial_init(uart->serial_handle, BAUDRATE);
    furi_hal_serial_async_rx_start(uart->serial_handle, wifi_marauder_uart_on_irq_cb, uart, false);

    return uart;
}

WifiMarauderUart* wifi_marauder_usart_init(WifiMarauderApp* app) {
    return wifi_marauder_uart_init(app, UART_CH, "WifiMarauderUartRxThread");
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
