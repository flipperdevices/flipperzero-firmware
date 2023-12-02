#pragma once

#include <gui/view.h>
#include "../helpers/color_guess_custom_event.h"

typedef struct ColorGuessStartscreen ColorGuessStartscreen;

typedef void (*ColorGuessStartscreenCallback)(ColorGuessCustomEvent event, void* context);

void color_guess_startscreen_set_callback(
    ColorGuessStartscreen* color_guess_startscreen,
    ColorGuessStartscreenCallback callback,
    void* context);

View* color_guess_startscreen_get_view(ColorGuessStartscreen* color_guess_static);

ColorGuessStartscreen* color_guess_startscreen_alloc();

void color_guess_startscreen_free(ColorGuessStartscreen* color_guess_static);