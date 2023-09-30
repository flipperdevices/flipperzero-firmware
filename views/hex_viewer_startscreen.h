#pragma once

#include <gui/view.h>
#include "../helpers/hex_viewer_custom_event.h"

typedef struct HexViewerStartscreen HexViewerStartscreen;

typedef void (*HexViewerStartscreenCallback)(HexViewerCustomEvent event, void* context);

void hex_viewer_startscreen_set_callback(
    HexViewerStartscreen* hex_viewer_startscreen,
    HexViewerStartscreenCallback callback,
    void* context);

View* hex_viewer_startscreen_get_view(HexViewerStartscreen* hex_viewer_static);

HexViewerStartscreen* hex_viewer_startscreen_alloc();

void hex_viewer_startscreen_free(HexViewerStartscreen* hex_viewer_static);