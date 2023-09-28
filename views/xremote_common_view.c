/*!
 *  @file flipper-xremote/views/xremote_common_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Common view and canvas functionality shared between the pages.
 */

#include "xremote_common_view.h"

#include "../infrared/infrared_remote.h"
#include "../xremote_app.h"

struct XRemoteView {
    XRemoteViewClearCallback on_clear;
    NotificationApp* notifications;
    View* view;
    void *context;
};

const NotificationSequence g_sequence_blink_purple_50 = {
    &message_red_255,
    &message_blue_255,
    &message_delay_50,
    NULL,
};

XRemoteView* xremote_view_alloc(NotificationApp* notifications, ViewInputCallback input_cb, ViewDrawCallback draw_cb)
{
    XRemoteView* remote_view = malloc(sizeof(XRemoteView));
    remote_view->view = view_alloc();
    remote_view->notifications = notifications;

    remote_view->context = NULL;
    remote_view->on_clear = NULL;

    view_set_orientation(remote_view->view, ViewOrientationVertical);
    view_allocate_model(remote_view->view, ViewModelTypeLocking, sizeof(XRemoteViewModel));

    view_set_input_callback(remote_view->view, input_cb);
    view_set_draw_callback(remote_view->view, draw_cb);
    view_set_context(remote_view->view, remote_view);

    return remote_view;
}

void xremote_view_clear_context(XRemoteView* rview)
{
    furi_assert(rview);

    if (rview->context != NULL &&
        rview->on_clear != NULL)
    {
        rview->on_clear(rview->context);
        rview->context = NULL;
    }
}

void xremote_view_set_context(XRemoteView* rview, void *context, XRemoteViewClearCallback on_clear)
{
    furi_assert(rview);
    xremote_view_clear_context(rview);
    rview->context = context;
    rview->on_clear = on_clear;
}

void* xremote_view_get_context(XRemoteView* rview)
{
    furi_assert(rview);
    return rview->context;
}

void xremote_view_free(XRemoteView* rview)
{
    furi_assert(rview);
    xremote_view_clear_context(rview);
    view_free(rview->view);
    free(rview);
}

View* xremote_view_get_view(XRemoteView* rview)
{
    furi_assert(rview);
    return rview->view;
}

static InfraredRemoteButton* xremote_view_get_button_by_name(XRemoteView *rview, const char* name)
{
    xremote_app_assert(rview->context, NULL);
    InfraredRemote* remote = (InfraredRemote*)rview->context;
    return infrared_remote_get_button_by_name(remote, name);
}

void xremote_view_send_ir(XRemoteView *rview, const char *name)
{
    InfraredRemoteButton* button = xremote_view_get_button_by_name(rview, name);
    xremote_app_assert_void(button);

    InfraredSignal* signal = infrared_remote_button_get_signal(button);
    xremote_app_assert_void(signal);

    infrared_signal_transmit(signal);
    notification_message(rview->notifications, &g_sequence_blink_purple_50);
}

void xremote_canvas_draw_icon(Canvas* canvas, uint8_t x, uint8_t y, XRemoteIcon icon)
{
    if (icon == XRemoteIconEnter)
    {
        canvas_draw_circle(canvas, x - 2, y, 4);
        canvas_draw_disc(canvas, x - 2, y, 2);
    }
    else if (icon == XRemoteIconBack)
    {
        canvas_draw_triangle(canvas, x - 4, y - 2, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x + 1, y - 2, x - 5, y - 2);
        canvas_draw_line(canvas, x + 1, y + 3, x - 3, y + 3);
        canvas_draw_line(canvas, x + 3, y + 1, x + 2, y + 2);
        canvas_draw_line(canvas, x + 3, y, x + 2, y - 1);
    }
    else if (icon == XRemoteIconArrowUp)
    {
        canvas_draw_triangle(canvas, x - 2, y - 2, 5, 3, CanvasDirectionBottomToTop);
        canvas_draw_line(canvas, x - 2, y - 3, x - 2, y + 4);
    }
    else if (icon == XRemoteIconArrowDown)
    {
        canvas_draw_triangle(canvas, x - 2, y + 2, 5, 3, CanvasDirectionTopToBottom);
        canvas_draw_line(canvas, x - 2, y - 4, x - 2, y + 3);
    }
    else if (icon == XRemoteIconArrowLeft)
    {
        canvas_draw_triangle(canvas, x - 4, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x + 2, y, x - 5, y);
    }
    else if (icon == XRemoteIconArrowRight)
    {
        canvas_draw_triangle(canvas, x, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_line(canvas, x - 6, y, x + 1, y);
    }
    else if (icon == XRemoteIconJumpForward)
    {
        canvas_draw_triangle(canvas, x - 2, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_triangle(canvas, x - 5, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_line(canvas, x + 1, y - 2, x + 1, y + 2);
        canvas_draw_line(canvas, x - 4, y, x, y);
    }
    else if (icon == XRemoteIconJumpBackward)
    {
        canvas_draw_triangle(canvas, x - 2, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_triangle(canvas, x + 1, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x - 5, y - 2, x - 5, y + 2);
        canvas_draw_line(canvas, x, y, x - 4, y);
    }
    else if (icon == XRemoteIconFastForward)
    {
        canvas_draw_triangle(canvas, x - 1, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_triangle(canvas, x - 4, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_line(canvas, x - 3, y, x, y);
    }
    else if (icon == XRemoteIconFastBackward)
    {
        canvas_draw_triangle(canvas, x - 3, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_triangle(canvas, x, y, 5, 3, CanvasDirectionRightToLeft);
        canvas_draw_line(canvas, x - 1, y, x - 4, y);
    }
    else if (icon == XRemoteIconPlayPause)
    {
        canvas_draw_triangle(canvas, x - 5, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_dot(canvas, x - 4, y);
        canvas_draw_line(canvas, x - 1, y - 2, x - 1, y + 2);
        canvas_draw_line(canvas, x + 1, y - 2, x + 1, y + 2);
    }
    else if (icon == XRemoteIconPlay)
    {
        canvas_draw_triangle(canvas, x - 3, y, 5, 3, CanvasDirectionLeftToRight);
        canvas_draw_dot(canvas, x - 2, y);
    }
    else if (icon == XRemoteIconPause)
    {
        canvas_draw_line(canvas, x - 3, y - 2, x - 3, y + 2);
        canvas_draw_line(canvas, x - 1, y - 2, x - 1, y + 2);
    }
    else if (icon == XRemoteIconStop)
    {
        canvas_draw_box(canvas, x - 4, y - 2, 5, 5);
    }
    else if (icon == XRemoteIconOk)
    {
        canvas_draw_str(canvas, x - 7, y + 4, "OK");
    }
}

void xremote_canvas_draw_header(Canvas* canvas, const char* section)
{
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 0, 0, AlignLeft, AlignTop, "XRemote");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 20, section);
}

void xremote_canvas_draw_exit_footer(Canvas* canvas, char *text)
{
    canvas_set_font(canvas, FontSecondary);
    xremote_canvas_draw_icon(canvas, 6, 124, XRemoteIconBack);
    canvas_draw_str(canvas, 12, 128, text);
}

void xremote_canvas_draw_button(Canvas* canvas, bool pressed, uint8_t x, uint8_t y, XRemoteIcon icon)
{
    canvas_draw_icon(canvas, x, y, &I_Button_18x18);

    if (pressed)
    {
        elements_slightly_rounded_box(canvas, x + 3, y + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }

    xremote_canvas_draw_icon(canvas, x + 11, y + 8, icon);
    canvas_set_color(canvas, ColorBlack);
}

void xremote_canvas_draw_button_wide(Canvas* canvas, bool pressed, uint8_t x, uint8_t y, char* text, XRemoteIcon icon)
{
    (void)icon;
    elements_slightly_rounded_frame(canvas, x + 4, y, 56, 15);

    if (pressed)
    {
        elements_slightly_rounded_box(canvas, x + 6, y + 2, 52, 11);
        canvas_set_color(canvas, ColorWhite);
    }

    xremote_canvas_draw_icon(canvas, x + 16, y + 7, icon);
    elements_multiline_text_aligned(canvas, x + 26, y + 10, AlignLeft, AlignBottom, text);
    canvas_set_color(canvas, ColorBlack);
}
