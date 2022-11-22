#pragma once

#include <gui/view.h>

typedef struct HidKeyboard HidKeyboard;

HidKeyboard* hid_keyboard_alloc();

void hid_keyboard_free(HidKeyboard* hid_keyboard);

View* hid_keyboard_get_view(HidKeyboard* hid_keyboard);

void hid_keyboard_set_connected_status(HidKeyboard* hid_keyboard, bool connected);

void hid_keyboard_set_conn_type(HidKeyboard* hid_keyboard, bool is_bluetooth);