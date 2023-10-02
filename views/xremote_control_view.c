/*!
 *  @file flipper-xremote/views/xremote_control_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief UI and view functionality for the IR control page.
 */

#include "xremote_control_view.h"
#include "../xremote_app.h"

static void xremote_control_view_draw_vertical(Canvas* canvas, XRemoteViewModel* model)
{
    xremote_canvas_draw_frame(canvas, model->up_pressed, 17, 30, 31, "VOL +");
    xremote_canvas_draw_frame(canvas, model->left_pressed, 4, 50, 23, "< CH");
    xremote_canvas_draw_frame(canvas, model->right_pressed, 37, 50, 23, "CH >");
    xremote_canvas_draw_frame(canvas, model->down_pressed, 17, 70, 31, "VOL -");
    xremote_canvas_draw_button_wide(canvas, model->ok_pressed, 0, 95, "Mute", XRemoteIconEnter);
}

static void xremote_control_view_draw_horizontal(Canvas* canvas, XRemoteViewModel* model)
{
    xremote_canvas_draw_frame(canvas, model->up_pressed, 17, 5, 31, "VOL +");
    xremote_canvas_draw_frame(canvas, model->left_pressed, 4, 25, 23, "< CH");
    xremote_canvas_draw_frame(canvas, model->right_pressed, 37, 25, 23, "CH >");
    xremote_canvas_draw_frame(canvas, model->down_pressed, 17, 45, 31, "VOL -");
    xremote_canvas_draw_button_size(canvas, model->ok_pressed, 70, 30, 44, "Mute", XRemoteIconEnter);
}

static void xremote_control_view_draw_callback(Canvas* canvas, void* context)
{
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppContext *app_ctx = model->context;
    XRemoteViewDrawFunction xremote_control_view_draw_body;

    ViewOrientation orientation = app_ctx->app_settings->orientation;
    xremote_control_view_draw_body = orientation == ViewOrientationVertical ?
        xremote_control_view_draw_vertical : xremote_control_view_draw_horizontal;

    xremote_canvas_draw_header(canvas, orientation, "Control");
    xremote_control_view_draw_body(canvas, model);
    xremote_canvas_draw_exit_footer(canvas, orientation, "Press to exit");
}

static void xremote_control_view_process(XRemoteView* view, InputEvent* event)
{
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel* model,
        {
            model->context = xremote_view_get_app_context(view);
            InfraredRemoteButton* button = NULL;

            if (event->type == InputTypePress)
            {
                if (event->key == InputKeyOk)
                {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_MUTE);
                    if (xremote_view_press_button(view, button)) model->ok_pressed = true;
                }
                else if (event->key == InputKeyUp)
                {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_VOL_UP);
                    if (xremote_view_press_button(view, button)) model->up_pressed = true;
                }
                else if (event->key == InputKeyDown)
                {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_VOL_DOWN);
                    if (xremote_view_press_button(view, button)) model->down_pressed = true;
                }
                else if (event->key == InputKeyLeft)
                {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_PREV_CHAN);
                    if (xremote_view_press_button(view, button)) model->left_pressed = true;
                }
                else if (event->key == InputKeyRight)
                {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_NEXT_CHAN);
                    if (xremote_view_press_button(view, button)) model->right_pressed = true;
                }
            }
            else if (event->type == InputTypeRelease)
            {
                if (event->key == InputKeyOk) model->ok_pressed = false;
                else if (event->key == InputKeyUp) model->up_pressed = false;
                else if (event->key == InputKeyDown) model->down_pressed = false;
                else if (event->key == InputKeyLeft) model->left_pressed = false;
                else if (event->key == InputKeyRight) model->right_pressed = false;
            }
        },
        true);
}

static bool xremote_control_view_input_callback(InputEvent* event, void* context)
{
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    if (event->key == InputKeyBack) return false;

    xremote_control_view_process(view, event);
    return true;
}

XRemoteView* xremote_control_view_alloc(void *app_ctx)
{
    XRemoteView *view = xremote_view_alloc(app_ctx,
        xremote_control_view_input_callback,
        xremote_control_view_draw_callback);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel* model,
        {
            model->context = app_ctx;
            model->up_pressed = false;
            model->down_pressed = false;
            model->left_pressed = false;
            model->right_pressed = false;
            model->back_pressed = false;
            model->ok_pressed = false;
        },
        true
    );

    return view;
}