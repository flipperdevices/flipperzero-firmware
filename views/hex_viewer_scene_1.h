#pragma once

#include <gui/view.h>
#include "../helpers/hex_viewer_custom_event.h"

typedef struct BoilerplateScene1 BoilerplateScene1;

typedef void (*BoilerplateScene1Callback)(BoilerplateCustomEvent event, void* context);

void hex_viewer_scene_1_set_callback(
    BoilerplateScene1* hex_viewer_scene_1,
    BoilerplateScene1Callback callback,
    void* context);

View* hex_viewer_scene_1_get_view(BoilerplateScene1* hex_viewer_static);

BoilerplateScene1* hex_viewer_scene_1_alloc();

void hex_viewer_scene_1_free(BoilerplateScene1* hex_viewer_static);