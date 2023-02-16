#pragma once

#include <gui/view.h>
#include "../helpers/color_guess_custom_event.h"

extern const Icon* digits[17];

#define TIMER_FORMAT "%02ld:%02ld" //"%.2d:%.2d"
#define TIMER_LENGHT 12
#define CLOSENESS_LENGTH 7
#define CLOSENESS_FORMAT "%d%%"
#define ARR_SIZE(arr) (sizeof((arr)) / sizeof((arr[0])))

typedef struct ColorGuessPlay ColorGuessPlay;

typedef void (*ColorGuessPlayCallback)(ColorGuessCustomEvent event, void* context);

typedef enum {
    ColorGuessPlayStatusStart,
    ColorGuessPlayStatusIDLE,
} ColorGuessPlayStatus;

void color_guess_play_set_callback(
    ColorGuessPlay* color_guess_play,
    ColorGuessPlayCallback callback,
    void* context);

View* color_guess_play_get_view(ColorGuessPlay* color_guess_static);

ColorGuessPlay* color_guess_play_alloc();

void color_guess_play_free(ColorGuessPlay* color_guess_static);