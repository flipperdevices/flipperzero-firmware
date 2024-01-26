#pragma once

#include <gui/view.h>
#include "../helpers/flippertag_custom_event.h"

typedef struct FlipperTagScene1 FlipperTagScene1;

typedef void (*FlipperTagScene1Callback)(FlipperTagCustomEvent event, void* context);

void flippertag_scene_1_set_callback(
    FlipperTagScene1* FlipperTag_scene_1,
    FlipperTagScene1Callback callback,
    void* context);

View* flippertag_scene_1_get_view(FlipperTagScene1* FlipperTag_static);

FlipperTagScene1* flippertag_scene_1_alloc();

void flippertag_scene_1_free(FlipperTagScene1* FlipperTag_static);