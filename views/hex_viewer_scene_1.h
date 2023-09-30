#pragma once

#include <gui/view.h>
#include "../helpers/hex_viewer_custom_event.h"

typedef struct HexViewerScene1 HexViewerScene1;

typedef void (*HexViewerScene1Callback)(HexViewerCustomEvent event, void* context);

void hex_viewer_scene_1_set_callback(
    HexViewerScene1* hex_viewer_scene_1,
    HexViewerScene1Callback callback,
    void* context);

View* hex_viewer_scene_1_get_view(HexViewerScene1* hex_viewer_static);

HexViewerScene1* hex_viewer_scene_1_alloc();

void hex_viewer_scene_1_free(HexViewerScene1* hex_viewer_static);