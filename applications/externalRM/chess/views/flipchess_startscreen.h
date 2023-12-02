#pragma once

#include <gui/view.h>
#include "../helpers/flipchess_custom_event.h"

typedef struct FlipChessStartscreen FlipChessStartscreen;

typedef void (*FlipChessStartscreenCallback)(FlipChessCustomEvent event, void* context);

void flipchess_startscreen_set_callback(
    FlipChessStartscreen* flipchess_startscreen,
    FlipChessStartscreenCallback callback,
    void* context);

View* flipchess_startscreen_get_view(FlipChessStartscreen* flipchess_static);

FlipChessStartscreen* flipchess_startscreen_alloc();

void flipchess_startscreen_free(FlipChessStartscreen* flipchess_static);