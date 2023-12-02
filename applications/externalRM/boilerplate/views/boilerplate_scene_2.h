#pragma once

#include <gui/view.h>
#include "../helpers/boilerplate_custom_event.h"

typedef struct BoilerplateScene2 BoilerplateScene2;

typedef void (*BoilerplateScene2Callback)(BoilerplateCustomEvent event, void* context);

void boilerplate_scene_2_set_callback(
    BoilerplateScene2* instance,
    BoilerplateScene2Callback callback,
    void* context);

BoilerplateScene2* boilerplate_scene_2_alloc();

void boilerplate_scene_2_free(BoilerplateScene2* boilerplate_static);

View* boilerplate_scene_2_get_view(BoilerplateScene2* boilerpate_static);
