#pragma once

#include <gui/view.h>
#include "lightmeter_icons.h"
#include "../../lightmeter_config.h"

typedef struct MainView MainView;

typedef void (*LightMeterMainViewButtonCallback)(void* context);

void lightmeter_main_view_set_left_callback(
    MainView* lightmeter_main_view,
    LightMeterMainViewButtonCallback callback,
    void* context);

MainView* main_view_alloc();

void main_view_free(MainView* main_view);

View* main_view_get_view(MainView* main_view);

void main_view_set_iso(MainView* main_view, int iso);

void main_view_set_nd(MainView* main_view, int nd);

void main_view_set_aperture(MainView* main_view, int aperture);

void main_view_set_speed(MainView* main_view, int speed);
