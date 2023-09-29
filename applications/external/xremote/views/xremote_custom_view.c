/*!
 *  @file flipper-xremote/views/xremote_custom_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Custom layout UI and view functionality for remote buttons.
 */

#include "xremote_custom_view.h"

static void xremote_custom_view_draw_callback(Canvas* canvas, void* context) {
    (void)context;
    xremote_canvas_draw_header(canvas, "Custom");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 70, "Coming Soon");
    xremote_canvas_draw_exit_footer(canvas, "Press to exit");
}

XRemoteView* xremote_custom_view_alloc(NotificationApp* notifications) {
    return xremote_view_alloc(notifications, NULL, xremote_custom_view_draw_callback);
}