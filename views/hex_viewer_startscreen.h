#pragma once

#include <gui/view.h>
#include "../helpers/hex_viewer_custom_event.h"

typedef struct BoilerplateStartscreen BoilerplateStartscreen;

typedef void (*BoilerplateStartscreenCallback)(BoilerplateCustomEvent event, void* context);

void hex_viewer_startscreen_set_callback(
    BoilerplateStartscreen* hex_viewer_startscreen,
    BoilerplateStartscreenCallback callback,
    void* context);

View* hex_viewer_startscreen_get_view(BoilerplateStartscreen* hex_viewer_static);

BoilerplateStartscreen* hex_viewer_startscreen_alloc();

void hex_viewer_startscreen_free(BoilerplateStartscreen* hex_viewer_static);