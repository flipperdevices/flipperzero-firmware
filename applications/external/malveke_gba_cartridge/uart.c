#include "uart.h"

#define UART_CH (FuriHalSerialIdUsart)
#define LP_UART_CH (FuriHalSerialIdLpuart)
#define BAUDRATE (115200UL)

struct Uart {
    void* app;
    FuriThread* rx_thread;
    FuriHalSerialHandle* serial_handle;
    FuriHalSerialId channel;
    FuriThread* worker_thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
};

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEventFlags;

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

void uart_set_handle_rx_data_cb(
    Uart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

static void wifi_marauder_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    Uart* uart = (Uart*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

static void
    uart_on_irq_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    Uart* uart = (Uart*)context;
    UNUSED(handle);

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}

// Define una constante para el prefijo que estamos buscando
#define JSON_PREFIX "JSON:"

// Variables globales
static char json_buffer[2048]; // Ajusta el tamaño según tus necesidades
static size_t json_buffer_index = 0;
static bool json_capture_active = false;
// static bool json_finded = false;

// Prototipo de la función
// static void process_json_buffer();

static void process_json_buffer(void* context) {
    Uart* uart = (Uart*)context;
    // Agregamos el terminador nulo al final del buffer
    json_buffer[json_buffer_index] = '\0';
    if(uart->handle_rx_data_cb) {
        uart->handle_rx_data_cb((uint8_t*)json_buffer, json_buffer_index, uart->app);
        memset(json_buffer, 0, sizeof(json_buffer));
    }

    // Reiniciamos el buffer
    json_buffer_index = 0;
}

static void uart_echo_push_to_list(void* context, uint8_t data) {
    Uart* uart = (Uart*)context;
    if(!json_capture_active) {
        if(data == JSON_PREFIX[json_buffer_index]) {
            json_buffer[json_buffer_index++] = data; // Agregar el carácter al buffer
            if(json_buffer_index == strlen(JSON_PREFIX)) {
                // Encontramos el prefijo, comenzamos a capturar
                json_buffer_index = 0;
                json_capture_active = true;
            }
        } else {
            // Reiniciamos el índice si no coincide con el prefijo
            json_buffer_index = 0;
        }
    } else {
        // Capturamos caracteres hasta encontrar '\n'
        json_buffer[json_buffer_index++] = data;
        if(data == '\n') {
            // Terminamos de capturar la línea, procesamos el buffer
            json_capture_active = false;
            process_json_buffer(uart);
        }
    }
}
static int32_t uart_worker(void* context) {
    furi_assert(context);
    Uart* uart = (Uart*)context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            if(uart->channel == UART_CH) {
                size_t length = 0;
                do {
                    uint8_t data[64];
                    length = furi_stream_buffer_receive(uart->rx_stream, data, 64, 0);

                    if(length > 0) {
                        for(size_t i = 0; i < length; i++) {
                            uart_echo_push_to_list(uart, data[i]);
                            // FURI_LOG_I("UART", "[in]: %c - %d", (const char)data[i], data[i]);
                        }
                    }
                } while(length > 0);
            } else if(uart->channel == LP_UART_CH) {
                size_t len =
                    furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);
                if(len > 0) {
                    if(uart->handle_rx_data_cb)
                        uart->handle_rx_data_cb(uart->rx_buf, len, uart->app);
                }
            }
        }
    }
    furi_stream_buffer_free(uart->rx_stream);

    return 0;
}
void uart_tx(void* app, uint8_t* data, size_t len) {
    Uart* uart = (Uart*)app;
    furi_hal_serial_tx(uart->serial_handle, data, len);
}

Uart* _uart_init(void* app, FuriHalSerialId channel, const char* thread_name) {
    Uart* uart = (Uart*)malloc(sizeof(Uart));
    uart->app = app;
    uart->channel = channel;
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
        channel == FuriHalSerialIdUsart ? uart_on_irq_cb : wifi_marauder_uart_on_irq_cb,
        uart,
        false);

    return uart;
}

Uart* usart_init(void* app) {
    return _uart_init(app, UART_CH, "UartRxThread");
}

Uart* lp_uart_init(void* app) {
    return _uart_init(app, LP_UART_CH, "LPUartRxThread");
}

void uart_free(Uart* uart) {
    furi_assert(uart);

    furi_hal_serial_async_rx_stop(uart->serial_handle);
    furi_hal_serial_deinit(uart->serial_handle);
    furi_hal_serial_control_release(uart->serial_handle);

    furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtStop);
    furi_thread_join(uart->rx_thread);
    furi_thread_free(uart->rx_thread);

    free(uart);
}
