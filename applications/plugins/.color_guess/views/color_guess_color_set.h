#pragma once

#include <gui/view.h>
#include "../helpers/color_guess_custom_event.h"

typedef struct ColorGuessColorSet ColorGuessColorSet;

typedef void (*ColorGuessColorSetCallback)(ColorGuessCustomEvent event, void* context);

typedef enum {
    ColorGuessColorSetStatusStart,
    ColorGuessColorSetStatusIDLE,
} ColorGuessColorSetStatus;

void color_guess_color_set_set_callback(
    ColorGuessColorSet* instance,
    ColorGuessColorSetCallback callback,
    void* context);

ColorGuessColorSet* color_guess_color_set_alloc();

void color_guess_color_set_free(ColorGuessColorSet* color_guess_static);

View* color_guess_color_set_get_view(ColorGuessColorSet* color_guess_static);
