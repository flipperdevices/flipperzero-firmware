#include "evil_portal_app_i.h"
#include "evil_portal_uart.h"
#include "helpers/evil_portal_storage.h"

struct Evil_PortalUart {
    Evil_PortalApp* app;
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

void evil_portal_uart_set_handle_rx_data_cb(
    Evil_PortalUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

void evil_portal_uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    Evil_PortalUart* uart = (Evil_PortalUart*)context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

static int32_t uart_worker(void* context) {
    Evil_PortalUart* uart = (void*)context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);
        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            size_t len = furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);

            if(len > 0) {
                if(uart->handle_rx_data_cb) {
                    uart->handle_rx_data_cb(uart->rx_buf, len, uart->app);

                    if(uart->app->has_command_queue) {
                        if(uart->app->command_index < 1) {
                            if(0 == strncmp(
                                        SET_AP_CMD,
                                        uart->app->command_queue[uart->app->command_index],
                                        strlen(SET_AP_CMD))) {
                                FuriString* out_data = furi_string_alloc();

                                furi_string_cat(out_data, "setap=");
                                furi_string_cat(out_data, (char*)uart->app->ap_name);

                                evil_portal_uart_tx(
                                    (uint8_t*)(furi_string_get_cstr(out_data)),
                                    strlen(furi_string_get_cstr(out_data)));
                                evil_portal_uart_tx((uint8_t*)("\n"), 1);

                                uart->app->sent_ap = true;

                                free(out_data);
                                free(uart->app->ap_name);
                            }

                            uart->app->command_index = 0;
                            uart->app->has_command_queue = false;
                            uart->app->command_queue[0] = "";
                        }
                    }

                    if(uart->app->sent_reset == false) {
                        furi_string_cat(uart->app->portal_logs, (char*)uart->rx_buf);
                    }

                    if(furi_string_utf8_length(uart->app->portal_logs) > 4000) {
                        write_logs(uart->app->portal_logs);
                        furi_string_reset(uart->app->portal_logs);
                    }
                } else {
                    uart->rx_buf[len] = '\0';
                    if(uart->app->sent_reset == false) {
                        furi_string_cat(uart->app->portal_logs, (char*)uart->rx_buf);
                    }

                    if(furi_string_utf8_length(uart->app->portal_logs) > 4000) {
                        write_logs(uart->app->portal_logs);
                        furi_string_reset(uart->app->portal_logs);
                    }
                }
            }
        }
    }

    furi_stream_buffer_free(uart->rx_stream);

    return 0;
}

void evil_portal_uart_tx(uint8_t* data, size_t len) {
    furi_hal_uart_tx(UART_CH, data, len);
}

Evil_PortalUart* evil_portal_uart_init(Evil_PortalApp* app) {
    Evil_PortalUart* uart = malloc(sizeof(Evil_PortalUart));
    uart->app = app;
    // Init all rx stream and thread early to avoid crashes
    uart->rx_stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(uart->rx_thread, "Evil_PortalUartRxThread");
    furi_thread_set_stack_size(uart->rx_thread, 1024);
    furi_thread_set_context(uart->rx_thread, uart);
    furi_thread_set_callback(uart->rx_thread, uart_worker);

    furi_thread_start(uart->rx_thread);

    /*furi_hal_console_disable();
  if (app->BAUDRATE == 0) {
    app->BAUDRATE = 115200;
  }
  furi_hal_uart_set_br(UART_CH, app->BAUDRATE);
  furi_hal_uart_set_irq_cb(UART_CH, evil_portal_uart_on_irq_cb, uart);*/

    furi_hal_console_disable();
    furi_hal_uart_set_br(UART_CH, BAUDRATE);
    furi_hal_uart_set_irq_cb(UART_CH, evil_portal_uart_on_irq_cb, uart);

    return uart;
}

void evil_portal_uart_free(Evil_PortalUart* uart) {
    furi_assert(uart);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    furi_hal_uart_set_irq_cb(UART_CH, NULL, NULL);
    furi_hal_console_enable();

    free(uart);
}
