#include "furi.h"
#include "main.h"

extern SerialDevice DEBUG_UART;

void handle_uart_write(const void* data, size_t size, void* ctx) {
	app_serial_write(&DEBUG_UART, (uint8_t*)data, (uint16_t)size, HAL_MAX_DELAY);
}

bool register_tty_uart() {
	if(!furi_create("tty", NULL, 0)) {
		return false;
	}
	
	if(furi_open("tty", false, false, handle_uart_write, NULL, NULL) == NULL) {
		return false;
	}

	return true;
}
