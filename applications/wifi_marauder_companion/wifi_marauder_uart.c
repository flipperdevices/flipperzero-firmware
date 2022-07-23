#include "wifi_marauder_uart.h"

const FuriHalUartId UART_CH = FuriHalUartIdUSART1;
const int BAUDRATE = 115200;

void wifi_marauder_uart_tx(uint8_t *data, size_t len) {
    furi_hal_uart_tx(UART_CH, data, len);
}

void wifi_marauder_uart_init() {
    furi_hal_uart_init(UART_CH, BAUDRATE);
}
