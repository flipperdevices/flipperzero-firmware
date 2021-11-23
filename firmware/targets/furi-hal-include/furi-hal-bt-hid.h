#pragma once

#include <stdint.h>
#include <stdbool.h>

bool furi_hal_bt_hid_init();

void furi_hal_bt_hid_deinit();

bool furi_hal_bt_hid_kb_press(uint16_t button);

bool furi_hal_bt_hid_kb_release(uint16_t button);

bool furi_hal_bt_hid_kb_release_all(uint16_t button);
