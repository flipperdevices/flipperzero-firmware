/*!
 *  @file flipper-xremote/views/xremote_general_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief General remote buttons section UI and view functionality.
 */

#include "xremote_general_view.h"

static void xremote_general_view_draw_callback(Canvas* canvas, void* context)
{
    (void)context;
    xremote_canvas_draw_header(canvas, "General");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 70, "Coming Soon");
    xremote_canvas_draw_exit_footer(canvas, "Press to exit");
}

XRemoteView* xremote_general_view_alloc(NotificationApp* notifications)
{
    return xremote_view_alloc(notifications,
        NULL, xremote_general_view_draw_callback);
}