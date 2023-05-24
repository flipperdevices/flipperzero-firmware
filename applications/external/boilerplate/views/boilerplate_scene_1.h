#pragma once

#include <gui/view.h>
#include "../helpers/boilerplate_custom_event.h"

typedef struct BoilerplateScene1 BoilerplateScene1;

typedef void (*BoilerplateScene1Callback)(BoilerplateCustomEvent event, void* context);

void boilerplate_scene_1_set_callback(
    BoilerplateScene1* boilerplate_scene_1,
    BoilerplateScene1Callback callback,
    void* context);

View* boilerplate_scene_1_get_view(BoilerplateScene1* boilerplate_static);

BoilerplateScene1* boilerplate_scene_1_alloc();

void boilerplate_scene_1_free(BoilerplateScene1* boilerplate_static);