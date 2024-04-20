#include "transceive_decorate.h"
#include "../si4713app.h"
#include <si4713tuner_icons.h>

#define FREQ_HOLD_INCREMENT 50

void transceive_decorate(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    SI4713AppState* state = ctx;

    // print frequency in mhz
    // if device is disconnect (or not detected),
    // print out ---.-- instead
    if(state->working == DeviceDisconnected)
        // this is totally safe guys don't worry
        strcpy(state->string_buffer, "---.--");
    else
        snprintf(
            state->string_buffer,
            8,
            state->freq < 10000 ? "0%.2f" : "%.2f",
            (double)state->freq / 100);

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str(canvas, 14, 24, state->string_buffer);

    // draw borders and icons
    canvas_draw_frame(canvas, 9, 5, 110, 24);
    canvas_draw_icon(canvas, 89, 13, &I_MHz_25x11);
    canvas_draw_icon(canvas, 122, 14, &I_ButtonRight_4x7);
    canvas_draw_icon(canvas, 2, 14, &I_ButtonLeft_4x7);
    canvas_draw_frame(canvas, 3, 28, 122, 33);
}

void transceive_decorate_handle_input(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    SI4713App* app = ctx;

    switch(event->key) {
    case InputKeyLeft:
        if(app->state.freq <= 8750) break;

        if(event->type == InputTypeRepeat)
            // i love premature optimization!!!!!
            app->state.freq = MAX(8750, app->state.freq - FREQ_HOLD_INCREMENT);
        else if(event->type == InputTypeShort)
            app->state.freq -= 5;
        else
            break;

        if(app->state.working != DeviceOff) si4713_tune_fm(app->device, app->state.freq);

        break;
    case InputKeyRight:
        if(app->state.freq >= 10800) break;

        if(event->type == InputTypeRepeat)
            app->state.freq = MIN(10800, app->state.freq + FREQ_HOLD_INCREMENT);
        else if(event->type == InputTypeShort)
            app->state.freq += 5;
        else
            break;

        if(app->state.working != DeviceOff) si4713_tune_fm(app->device, app->state.freq);

        break;
    default:
        break;
    }
}