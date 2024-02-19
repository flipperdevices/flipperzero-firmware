#pragma once

#include <gui/view.h>
#include "../imu_mouse.h"

typedef void (*AirMouseViewExit)(void* context);

typedef struct AirMouseView AirMouseView;

AirMouseView* air_mouse_view_alloc(AirMouseViewExit exit_callback, void* context);

void air_mouse_view_free(AirMouseView* air_mouse);

View* air_mouse_view_get_view(AirMouseView* air_mouse);

void air_mouse_view_set_hid_api(
    AirMouseView* air_mouse,
    const ImuHidApi* hid,
    void* hid_inst,
    bool is_ble_interface);

void air_mouse_view_set_device(AirMouseView* air_mouse, void* imu_device);

void air_mouse_view_set_connected_status(AirMouseView* air_mouse, bool connected);
