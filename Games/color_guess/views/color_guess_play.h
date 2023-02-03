#pragma once

#include <gui/view.h>

typedef struct ColorGuessPlay ColorGuessPlay;

typedef enum {
    ColorGuessPlayStatusStart,
    ColorGuessPlayStatusIDLE,
} ColorGuessPlayStatus;

//typedef void (*ColorGuessColorSetCallback)(ColorGuessCustomEvent event, void* context);

View* color_guess_play_get_view(ColorGuessPlay* color_guess_static);

ColorGuessPlay* color_guess_play_alloc();

void color_guess_play_free(ColorGuessPlay* color_guess_static);