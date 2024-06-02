#pragma once

#include <gui/view.h>
#include "../helpers/flipenigma_custom_event.h"

typedef struct FlipEnigmaStartscreen FlipEnigmaStartscreen;

typedef void (*FlipEnigmaStartscreenCallback)(FlipEnigmaCustomEvent event, void* context);

void flipenigma_startscreen_set_callback(
    FlipEnigmaStartscreen* flipenigma_startscreen,
    FlipEnigmaStartscreenCallback callback,
    void* context);

View* flipenigma_startscreen_get_view(FlipEnigmaStartscreen* flipenigma_static);

FlipEnigmaStartscreen* flipenigma_startscreen_alloc();

void flipenigma_startscreen_free(FlipEnigmaStartscreen* flipenigma_static);