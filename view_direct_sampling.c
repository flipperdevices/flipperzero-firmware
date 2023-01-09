/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"

#include <cc1101.h>

/* Read directly from the G0 CC1101 pin, and draw a black or white
 * dot depending on the level. */
void render_view_direct_sampling(Canvas *const canvas, ProtoViewApp *app) {
    UNUSED(app);
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 128; x++) {
            bool level = furi_hal_gpio_read(&gpio_cc1101_g0);
            if (level) canvas_draw_dot(canvas,x,y);
            /* Busy loop: this is a terrible approach as it blocks
             * everything else, but for now it's the best we can do
             * to obtain direct data with some spacing. */
            uint32_t x = 500; while(x--);
        }
    }
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_with_border(canvas,40,60,"Direct sampling",
        ColorWhite,ColorBlack);
}

/* Handle input */
void process_input_direct_sampling(ProtoViewApp *app, InputEvent input) {
    UNUSED(app);
    UNUSED(input);
}

/* Enter view. Stop the subghz thread to prevent access as we read
 * the CC1101 data directly. */
void view_enter_direct_sampling(ProtoViewApp *app) {
    if (app->txrx->txrx_state == TxRxStateRx) {
        subghz_worker_stop(app->txrx->worker);
    }
}

/* Exit view. Restore the subghz thread. */
void view_exit_direct_sampling(ProtoViewApp *app) {
    if (app->txrx->txrx_state == TxRxStateRx) {
        subghz_worker_start(app->txrx->worker);
    }
}
