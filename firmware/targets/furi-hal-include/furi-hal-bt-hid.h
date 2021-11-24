#pragma once

#include <stdint.h>
#include <stdbool.h>

void furi_hal_bt_hid_start();

void furi_hal_bt_hid_stop();

bool furi_hal_bt_hid_is_started();

bool furi_hal_bt_hid_kb_press(uint16_t button);

bool furi_hal_bt_hid_kb_release(uint16_t button);

bool furi_hal_bt_hid_kb_release_all(uint16_t button);
