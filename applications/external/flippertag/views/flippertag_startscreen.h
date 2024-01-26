#pragma once

#include <gui/view.h>
#include "../helpers/flippertag_custom_event.h"

typedef struct FlipperTagStartscreen FlipperTagStartscreen;

typedef void (*FlipperTagStartscreenCallback)(FlipperTagCustomEvent event, void* context);

void flippertag_startscreen_set_callback(
    FlipperTagStartscreen* FlipperTag_startscreen,
    FlipperTagStartscreenCallback callback,
    void* context);

View* flippertag_startscreen_get_view(FlipperTagStartscreen* FlipperTag_static);

FlipperTagStartscreen* flippertag_startscreen_alloc();

void flippertag_startscreen_free(FlipperTagStartscreen* FlipperTag_static);