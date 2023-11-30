#pragma once

#include <furi_hal.h>
#include <gui/gui.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

#include "leds.h"
#include "led_driver.h"

// The WS2812b LEDs that are connected to PC3.
#define LED_COUNT 4
struct FlipboardLeds {
    Resources* resources;
    uint32_t color[LED_COUNT];
    LedDriver* led_driver;
};
const GpioPin* const pin_ws2812_leds = &gpio_ext_pc3;

// The status LED that is connected to PA7.
const GpioPin* const pin_status_led = &gpio_ext_pa7;
