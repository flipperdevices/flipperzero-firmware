#pragma once

#include <gui/view.h>
#include "../helpers/hex_viewer_custom_event.h"

typedef struct HexViewerScene2 HexViewerScene2;

typedef void (*HexViewerScene2Callback)(HexViewerCustomEvent event, void* context);

void hex_viewer_scene_2_set_callback(
    HexViewerScene2* instance,
    HexViewerScene2Callback callback,
    void* context);

HexViewerScene2* hex_viewer_scene_2_alloc();

void hex_viewer_scene_2_free(HexViewerScene2* hex_viewer_static);

View* hex_viewer_scene_2_get_view(HexViewerScene2* boilerpate_static);
