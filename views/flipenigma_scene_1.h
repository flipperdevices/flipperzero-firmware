#pragma once

#include <gui/view.h>
#include "../helpers/flipenigma_custom_event.h"

typedef struct FlipEnigmaScene1 FlipEnigmaScene1;

typedef void (*FlipEnigmaScene1Callback)(FlipEnigmaCustomEvent event, void* context);

void flipenigma_scene_1_set_callback(
    FlipEnigmaScene1* flipenigma_scene_1,
    FlipEnigmaScene1Callback callback,
    void* context);

View* flipenigma_scene_1_get_view(FlipEnigmaScene1* flipenigma_static);

FlipEnigmaScene1* flipenigma_scene_1_alloc();

void flipenigma_scene_1_free(FlipEnigmaScene1* flipenigma_static);