/*!
 *  @file flipper-xremote/views/xremote_about_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief View functionality for version and additional information.
 */

#include "xremote_about_view.h"
#include "../xremote.h"

static void xremote_about_view_draw_callback(Canvas* canvas, void* context) {
    (void)context;
    char version[32];

    xremote_canvas_draw_header(canvas, "About");
    canvas_set_font(canvas, FontSecondary);

    xremote_get_version(version, sizeof(version));
    canvas_draw_str_aligned(canvas, 0, 30, AlignLeft, AlignTop, "Version:");
    canvas_draw_str_aligned(canvas, 35, 30, AlignLeft, AlignTop, version);

    canvas_draw_str_aligned(canvas, 0, 40, AlignLeft, AlignTop, "License: GPLv3");
    canvas_draw_str_aligned(canvas, 0, 50, AlignLeft, AlignTop, "Author: kala13x");

    canvas_draw_str_aligned(canvas, 0, 69, AlignLeft, AlignTop, "Contact:");
    canvas_draw_str_aligned(canvas, 13, 80, AlignLeft, AlignTop, "s.kalatoz");
    canvas_draw_str_aligned(canvas, 29, 90, AlignLeft, AlignTop, "@");
    canvas_draw_str_aligned(canvas, 11, 100, AlignLeft, AlignTop, "gmail.com");

    elements_slightly_rounded_frame(canvas, 9, 78, 45, 33);
    xremote_canvas_draw_exit_footer(canvas, "Press to exit");
}

XRemoteView* xremote_about_view_alloc(NotificationApp* notifications) {
    return xremote_view_alloc(notifications, NULL, xremote_about_view_draw_callback);
}