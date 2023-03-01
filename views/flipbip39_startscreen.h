#pragma once

#include <gui/view.h>
#include "../helpers/flipbip39_custom_event.h"

typedef struct FlipBip39Startscreen FlipBip39Startscreen;

typedef void (*FlipBip39StartscreenCallback)(FlipBip39CustomEvent event, void* context);

void flipbip39_startscreen_set_callback(
    FlipBip39Startscreen* flipbip39_startscreen,
    FlipBip39StartscreenCallback callback,
    void* context);

View* flipbip39_startscreen_get_view(FlipBip39Startscreen* flipbip39_static);

FlipBip39Startscreen* flipbip39_startscreen_alloc();

void flipbip39_startscreen_free(FlipBip39Startscreen* flipbip39_static);