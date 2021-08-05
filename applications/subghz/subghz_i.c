#include "subghz_i.h"

#include <math.h>
#include <furi.h>
#include <api-hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <notification/notification-messages.h>

void subghz_begin(ApiHalSubGhzPreset preset) {
    api_hal_subghz_reset();
    api_hal_subghz_idle();
    api_hal_subghz_load_preset(preset);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
}

void subghz_rx(uint32_t frequency) {
    api_hal_subghz_idle();
    api_hal_subghz_set_frequency_and_path(frequency);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
    api_hal_subghz_flush_rx();
    api_hal_subghz_rx();
}

void subghz_tx(uint32_t frequency) {
    api_hal_subghz_idle();
    api_hal_subghz_set_frequency_and_path(frequency);
    hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_cc1101_g0, true);
    api_hal_subghz_tx();
}

void subghz_idle(void) {
    api_hal_subghz_idle();
}

void subghz_end(void) {
    api_hal_subghz_sleep();
}

void subghz_text_callback(string_t text, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;

    // with_view_model(
    //     subghz_receiver->view, (SubghzReceiverModel * model) {
    //         model->counter++;
    //         string_set(model->text, text);
    //         model->scene = 0;
    //         return true;
    //     });
}