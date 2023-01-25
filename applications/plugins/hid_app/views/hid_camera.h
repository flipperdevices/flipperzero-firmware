#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidCamera HidCamera;

HidCamera* hid_camera_alloc(Hid* bt_hid);

void hid_camera_free(HidCamera* hid_camera);

View* hid_camera_get_view(HidCamera* hid_camera);

void hid_camera_set_connected_status(HidCamera* hid_camera, bool connected);
