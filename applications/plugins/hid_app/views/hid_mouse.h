#pragma once

#include <gui/view.h>

typedef struct HidMouse HidMouse;

HidMouse* hid_mouse_alloc();

void hid_mouse_free(HidMouse* hid_mouse);

View* hid_mouse_get_view(HidMouse* hid_mouse);

void hid_mouse_set_connected_status(HidMouse* hid_mouse, bool connected);

void hid_mouse_set_conn_type(HidMouse* hid_mouse, bool is_bluetooth);
