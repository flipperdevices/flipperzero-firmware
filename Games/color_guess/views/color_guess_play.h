#pragma once

#include <gui/view.h>

extern const Icon* digits[17];

#define TIMER_FORMAT "%02ld:%02ld" //"%.2d:%.2d"
#define TIMER_LENGHT 12
#define CLOSENESS_LENGTH 7
#define CLOSENESS_FORMAT "%d%%"

typedef struct ColorGuessPlay ColorGuessPlay;

typedef enum {
    ColorGuessPlayStatusStart,
    ColorGuessPlayStatusIDLE,
} ColorGuessPlayStatus;

//typedef void (*ColorGuessColorSetCallback)(ColorGuessCustomEvent event, void* context);

View* color_guess_play_get_view(ColorGuessPlay* color_guess_static);

ColorGuessPlay* color_guess_play_alloc();

void color_guess_play_free(ColorGuessPlay* color_guess_static);