#include "standby.h"
#if __has_include(<assets_icons.h>)
#include <assets_icons.h>
#endif
#include <totp_icons.h>
#include "../../constants.h"

void totp_scene_standby_render(Canvas* const canvas) {
#if __has_include(<assets_icons.h>)
    canvas_draw_icon(canvas, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 44, &I_WarningDolphinFlip_45x42);
#else
    canvas_draw_icon(canvas, SCREEN_WIDTH - 56, SCREEN_HEIGHT - 48, &I_DolphinCommon_56x48);
#endif

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 10, AlignLeft, AlignTop, "CLI command");

    canvas_draw_str_aligned(canvas, 5, 24, AlignLeft, AlignTop, "is running now");
}
