#pragma once

#include <gui/view.h>
#include "lightmeter_icons.h"
// #include "../lightmeterI2C.h"

// I2C BUS
#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 3

typedef struct MainView MainView;

int get_lx();

MainView* main_view_alloc();

void main_view_free(MainView* main_view);

View* main_view_get_view(MainView* main_view);
