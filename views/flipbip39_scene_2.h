#pragma once

#include <gui/view.h>
#include "../helpers/flipbip39_custom_event.h"

typedef struct FlipBip39Scene2 FlipBip39Scene2;

typedef void (*FlipBip39Scene2Callback)(FlipBip39CustomEvent event, void* context);

void flipbip39_scene_2_set_callback(
    FlipBip39Scene2* instance,
    FlipBip39Scene2Callback callback,
    void * context);

FlipBip39Scene2* flipbip39_scene_2_alloc();

void flipbip39_scene_2_free(FlipBip39Scene2* flipbip39_static);

View* flipbip39_scene_2_get_view(FlipBip39Scene2* boilerpate_static);
