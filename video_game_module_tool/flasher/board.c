#include "board.h"

#include <furi.h>
#include <furi_hal_resources.h>

#define BOARD_RESET_PIN (gpio_ext_pc1)

void board_init(void) {
    furi_hal_gpio_write(&BOARD_RESET_PIN, false);
    furi_hal_gpio_init(&BOARD_RESET_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
}

void board_deinit(void) {
    furi_hal_gpio_write(&BOARD_RESET_PIN, false);
    furi_hal_gpio_init_simple(&BOARD_RESET_PIN, GpioModeAnalog);
}

void board_reset(void) {
    furi_hal_gpio_write(&BOARD_RESET_PIN, true);
    furi_delay_ms(5);
    furi_hal_gpio_write(&BOARD_RESET_PIN, false);
    furi_delay_ms(5);
}
