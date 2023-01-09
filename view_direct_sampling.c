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
