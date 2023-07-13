#pragma once

#include <gui/view.h>
#include "../helpers/flipchess_custom_event.h"

typedef struct FlipChessScene1 FlipChessScene1;

typedef void (*FlipChessScene1Callback)(FlipChessCustomEvent event, void* context);

void flipchess_scene_1_set_callback(
    FlipChessScene1* flipchess_scene_1,
    FlipChessScene1Callback callback,
    void* context);

View* flipchess_scene_1_get_view(FlipChessScene1* flipchess_static);

FlipChessScene1* flipchess_scene_1_alloc();

void flipchess_scene_1_free(FlipChessScene1* flipchess_static);