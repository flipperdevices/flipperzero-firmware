#pragma once

#include <gui/view.h>
#include "../helpers/flipbip_custom_event.h"

typedef struct FlipBipScene1 FlipBipScene1;

typedef void (*FlipBipScene1Callback)(FlipBipCustomEvent event, void* context);

void flipbip_scene_1_set_callback(
    FlipBipScene1* flipbip_scene_1,
    FlipBipScene1Callback callback,
    void* context);

View* flipbip_scene_1_get_view(FlipBipScene1* flipbip_static);

FlipBipScene1* flipbip_scene_1_alloc();

void flipbip_scene_1_free(FlipBipScene1* flipbip_static);