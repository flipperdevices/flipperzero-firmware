#include "transmit_view.h"
#include "transceive_decorate.h"
#include "../si4713app.h"

#include <furi.h>
#include <si4713tuner_icons.h>

// 115 dBuV is the upper limit of the board
#define SI4713_TXPOWER_LIMIT 115
#define TXPOWER_HOLD_INCREMENT 5

void render_transmit_view(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    SI4713AppState* state = ctx;

    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, 1);

    // draw borders, icons, and current frequency
    transceive_decorate(canvas, ctx);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 31, 38, "Transmitting");

    // print current tx_power
    snprintf(state->string_buffer, 9, "%d", state->tx_power);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 45, 57, state->string_buffer);
    canvas_draw_str(canvas, 65, 57, "dBuV");

    // draw tx_power arrow
    canvas_draw_icon(canvas, 4 + state->tx_power, 42, &I_Pin_pointer_5x3);

    // draw tx_power line
    canvas_draw_line(canvas, 6, 41, 121, 41);
}

void transmit_view_handle_input(InputEvent* event, void* app) {
    furi_assert(app);

    SI4713App* ctx = app;

    switch (event->key){
    case InputKeyUp:
        if (ctx->state.tx_power >= SI4713_TXPOWER_LIMIT) break;

        // i figured that if the repeat is going to be
        // executed a lot of times then i should have it
        // be evalauted first (i love else ifs)
        if (event->type == InputTypeRepeat)
            ctx->state.tx_power = MIN(SI4713_TXPOWER_LIMIT, ctx->state.tx_power + TXPOWER_HOLD_INCREMENT);
        else if (event->type == InputTypeShort)
            ctx->state.tx_power++;
        else break;

        if (ctx->state.working == DeviceOn)
            si4713_set_tx_power(ctx->device,ctx->state.tx_power,0);

        break;
    case InputKeyDown:
        if (ctx->state.tx_power <= 0) break;

        if (event->type == InputTypeRepeat)
            ctx->state.tx_power = MAX(0, ctx->state.tx_power - TXPOWER_HOLD_INCREMENT);
        else if (event->type == InputTypeShort)
            ctx->state.tx_power--;
        else break;

        if (ctx->state.working == DeviceOn)
            si4713_set_tx_power(ctx->device, ctx->state.tx_power, 0);

        break;
    default:
        transceive_decorate_handle_input(event, ctx);
    }
}