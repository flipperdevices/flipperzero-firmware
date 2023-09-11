#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

void* flipboard_leds_alloc();
void flipboard_leds_reset(void* leds);
void flipboard_leds_free(void* leds);
void flipboard_leds_set(void* leds, uint32_t led, uint32_t color);
void flipboard_leds_update(void* leds);