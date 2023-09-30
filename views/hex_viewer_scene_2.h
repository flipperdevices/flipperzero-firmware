#pragma once

#include <gui/view.h>
#include "../helpers/hex_viewer_custom_event.h"

typedef struct BoilerplateScene2 BoilerplateScene2;

typedef void (*BoilerplateScene2Callback)(BoilerplateCustomEvent event, void* context);

void hex_viewer_scene_2_set_callback(
    BoilerplateScene2* instance,
    BoilerplateScene2Callback callback,
    void * context);

BoilerplateScene2* hex_viewer_scene_2_alloc();

void hex_viewer_scene_2_free(BoilerplateScene2* hex_viewer_static);

View* hex_viewer_scene_2_get_view(BoilerplateScene2* boilerpate_static);
