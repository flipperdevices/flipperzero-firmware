#pragma once

#include <gui/view.h>
#include "../helpers/flipbip39_custom_event.h"

typedef struct FlipBip39Scene1 FlipBip39Scene1;

typedef void (*FlipBip39Scene1Callback)(FlipBip39CustomEvent event, void* context);

void flipbip39_scene_1_set_callback(
    FlipBip39Scene1* flipbip39_scene_1,
    FlipBip39Scene1Callback callback,
    void* context);

View* flipbip39_scene_1_get_view(FlipBip39Scene1* flipbip39_static);

FlipBip39Scene1* flipbip39_scene_1_alloc();

void flipbip39_scene_1_free(FlipBip39Scene1* flipbip39_static);