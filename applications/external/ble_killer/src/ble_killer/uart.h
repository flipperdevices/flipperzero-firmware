#include "furi_hal.h"
#include <gui/modules/text_box.h>


#define RX_BUF_SIZE 1024

#define START_SCAN 0
#define TEST_BTN 1
#define UART_INIT 2

typedef struct UART_TerminalApp {
    uint8_t uart_ch;
    uint8_t baudrate_index;
    FuriStreamBuffer* rx_stream;
    FuriThread* rx_thread;
    uint8_t rx_buf[RX_BUF_SIZE + 1];
    TextBox *text_box;
    FuriString *text_box_store;
    size_t text_box_store_strlen;
    FuriTimer* console_timer;
    FuriTimer* scan_timer;
    int BAUDRATE;
    int uart_is_init;
    FuriHalSerialHandle* serial_handle;
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context);
} uart_app;


void uart_init(uart_app* uapp, int baudrate, FuriHalSerialId ch);
static void scan_console_recv(void *ctx);
void uart_terminal_uart_set_handle_rx_data_cb(uart_app* uart, void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context));
void uart_terminal_console_output_handle_rx_data_cb(uint8_t* buf, size_t len, void* context);
void uart_terminal_uart_free(uart_app* uart);
static void baudRate_change_callback(VariableItem* item);