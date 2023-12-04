#include "uart.h"

#define UART_CH (FuriHalUartIdUSART1)
#define LP_UART_CH (FuriHalUartIdLPUART1)
#define BAUDRATE (115200)

struct Uart {
    void* app;
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

void uart_set_handle_rx_data_cb(
    Uart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

void uart_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    Uart* uart = (Uart*)context;

    if(ev == UartIrqEventRXNE) {
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
    if (uart->handle_rx_data_cb) {
        uart->handle_rx_data_cb((uint8_t *)json_buffer, json_buffer_index, uart->app);
        memset(json_buffer, 0, sizeof(json_buffer));
    }
    
    // Reiniciamos el buffer
    json_buffer_index = 0;
}

static void uart_echo_push_to_list(void* context, uint8_t data) {
    Uart* uart = (Uart*)context;
    if (!json_capture_active) {
        if (data == JSON_PREFIX[json_buffer_index]) {
            json_buffer[json_buffer_index++] = data; // Agregar el carácter al buffer
            if (json_buffer_index == strlen(JSON_PREFIX)) {
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
        if (data == '\n') {
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
        uint32_t events = furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) break;
        if(events & WorkerEvtRxDone) {
            if(uart->channel == UART_CH) {
                size_t length = 0;
                do {
                    uint8_t data[64];
                    length = furi_stream_buffer_receive(uart->rx_stream, data, 64, 0);
                    // FURI_LOG_I("UART", "[in]: %s", (char*)data);
                    if(length > 0) {
                        for(size_t i = 0; i < length; i++) {
                            uart_echo_push_to_list(uart, data[i]);
                        }
                    }
                } while(length > 0);
            } else if(uart->channel == LP_UART_CH) {
                size_t len = furi_stream_buffer_receive(uart->rx_stream, uart->rx_buf, RX_BUF_SIZE, 0);
                if(len > 0) {
                    if(uart->handle_rx_data_cb) uart->handle_rx_data_cb(uart->rx_buf, len, uart->app);
                }
            }
        }
    }
    furi_stream_buffer_free(uart->rx_stream);

    return 0;
}

void uart_tx(uint8_t* data, size_t len) {
    furi_hal_uart_tx(UART_CH, data, len);
}

void lp_uart_tx(uint8_t* data, size_t len) {
    furi_hal_uart_tx(LP_UART_CH, data, len);
}

Uart*
    _uart_init(void* app, FuriHalUartId channel, const char* thread_name) {
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
    if(channel == FuriHalUartIdUSART1) {
        furi_hal_console_disable();
    } else if(channel == FuriHalUartIdLPUART1) {
        furi_hal_uart_init(channel, BAUDRATE);
    }
    furi_hal_uart_set_br(channel, BAUDRATE);
    furi_hal_uart_set_irq_cb(channel, uart_on_irq_cb, uart);

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
