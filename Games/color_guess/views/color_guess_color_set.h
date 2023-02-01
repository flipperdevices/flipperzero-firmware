#pragma once

#include <gui/view.h>

typedef struct ColorGuessColorSet ColorGuessColorSet;

typedef enum {
    ColorGuessColorSetStatusStart,
    ColorGuessColorSetStatusIDLE,
} ColorGuessColorSetStatus;

//typedef void (*ColorGuessColorSetCallback)(ColorGuessCustomEvent event, void* context);

View* color_guess_color_set_get_view(ColorGuessColorSet* color_guess_static);

ColorGuessColorSet* color_guess_color_set_alloc();

void color_guess_color_set_free(ColorGuessColorSet* color_guess_static);