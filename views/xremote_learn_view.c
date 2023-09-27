/*!
 *  @file flipper-xremote/views/xremote_learn_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Learn new remote page view components and functionality.
 */

#include "xremote_learn_view.h"

static void xremote_learn_view_draw_callback(Canvas* canvas, void* context)
{
    (void)context;
    xremote_canvas_draw_header(canvas, "Learn");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 70, "Coming Soon");
    xremote_canvas_draw_exit_footer(canvas, "Press to exit");
}

XRemoteView* xremote_learn_view_alloc(NotificationApp* notifications)
{
    return xremote_view_alloc(notifications,
        NULL, xremote_learn_view_draw_callback);
}