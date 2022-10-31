#pragma once

#include <gui/view.h>
#include "lightmeter_icons.h"


// I2C BUS
#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 3

typedef struct MainView MainView;

typedef void (*LightMeterMainViewButtonCallback)(void* context);

int get_lx();

void lightmeter_main_view_set_left_callback(
    MainView* lightmeter_main_view,
    LightMeterMainViewButtonCallback callback,
    void* context);

MainView* main_view_alloc();

void main_view_free(MainView* main_view);

View* main_view_get_view(MainView* main_view);

void main_view_set_iso(MainView* main_view, int iso);
