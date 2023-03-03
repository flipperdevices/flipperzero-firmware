#pragma once

#include <gui/view.h>
#include "../helpers/flipbip_custom_event.h"

typedef struct FlipBipStartscreen FlipBipStartscreen;

typedef void (*FlipBipStartscreenCallback)(FlipBipCustomEvent event, void* context);

void flipbip_startscreen_set_callback(
    FlipBipStartscreen* flipbip_startscreen,
    FlipBipStartscreenCallback callback,
    void* context);

View* flipbip_startscreen_get_view(FlipBipStartscreen* flipbip_static);

FlipBipStartscreen* flipbip_startscreen_alloc();

void flipbip_startscreen_free(FlipBipStartscreen* flipbip_static);