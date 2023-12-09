#pragma once

#include <furi_hal.h>

#include "leds.h"
#include "led_driver.h"

#include "../app_config.h"

// The number of WS2812b LEDs connected to the FlipBoard.
#define LED_COUNT 4

// The pin that is connected to the WS2812 LEDs.
const GpioPin* const pin_ws2812_leds = &gpio_ext_pc3;

// The status LED that is connected to PA7.
const GpioPin* const pin_status_led = &gpio_ext_pa7;

struct FlipboardLeds {
    Resources* resources;
    uint32_t color[LED_COUNT];
    LedDriver* led_driver;
};

// Bit-banging the WS2812b LEDs isn't great. If we could get interrupted signal will be wrong (wrong LED colors).
typedef struct {
    volatile uint32_t COUNT;
} DWT_Internal;

// Cycle Count Register is at DWT Base Address (E0001000) + 0x004 (R/W)
#define DWT_CYCCNT (0xE0001004UL)
#define DWT_ACCESS ((DWT_Internal*)DWT_CYCCNT)
