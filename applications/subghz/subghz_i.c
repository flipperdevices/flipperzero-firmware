#include "subghz_i.h"

#include <math.h>
#include <furi.h>
#include <furi-hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <notification/notification-messages.h>

void subghz_begin(FuriHalSubGhzPreset preset) {
    furi_hal_subghz_reset();
    furi_hal_subghz_idle();
    furi_hal_subghz_load_preset(preset);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
}

void subghz_rx(uint32_t frequency) {
    furi_hal_subghz_idle();
    furi_hal_subghz_set_frequency_and_path(frequency);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_flush_rx();
    furi_hal_subghz_rx();
}

void subghz_tx(uint32_t frequency) {
    furi_hal_subghz_idle();
    furi_hal_subghz_set_frequency_and_path(frequency);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_cc1101_g0, true);
    furi_hal_subghz_tx();
    
}

void subghz_idle(void) {
    furi_hal_subghz_idle();
}

void subghz_end(void) {
    furi_hal_subghz_sleep();
}
