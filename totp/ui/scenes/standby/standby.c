#include "standby.h"
#include <totp_icons.h>
#include "../../constants.h"

#include <assets_icons.h>

void totp_scene_standby_render(Canvas* const canvas) {
    canvas_draw_icon(canvas, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 44, &I_WarningDolphinFlip_45x42);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 10, AlignLeft, AlignTop, "CLI command");

    canvas_draw_str_aligned(canvas, 5, 24, AlignLeft, AlignTop, "is running now");
}