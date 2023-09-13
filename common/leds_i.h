#pragma once

#include <furi_hal.h>
#include <gui/gui.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

#include "leds.h"

#define LED_COUNT 4
struct FlipboardLeds {
    uint32_t color[LED_COUNT];
};

const GpioPin* const pin_ws2812_leds = &gpio_ext_pc3;
const GpioPin* const pin_status_led = &gpio_ext_pa7;
