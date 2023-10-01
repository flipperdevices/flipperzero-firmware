/*!
 *  @file flipper-xremote/views/xremote_control_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief UI and view functionality for the IR control page.
 */

#include "xremote_control_view.h"

static void xremote_control_view_draw_callback(Canvas* canvas, void* context)
{
    furi_assert(context);
    XRemoteViewModel* model = context;

    xremote_canvas_draw_header(canvas, "Control");
    xremote_canvas_draw_frame(canvas, model->up_pressed, 17, 30, 31, "VOL +");
    xremote_canvas_draw_frame(canvas, model->left_pressed, 4, 50, 23, "< CH");
    xremote_canvas_draw_frame(canvas, model->right_pressed, 37, 50, 23, "CH >");
    xremote_canvas_draw_frame(canvas, model->down_pressed, 17, 70, 31, "VOL -");
    xremote_canvas_draw_button_wide(canvas, model->ok_pressed, 0, 95, "Mute", XRemoteIconEnter);
    xremote_canvas_draw_exit_footer(canvas, "Press to exit");
}

static void xremote_control_view_process(XRemoteView* view, InputEvent* event)
{
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel* model,
        {
            if (event->type == InputTypePress)
            {
                if (event->key == InputKeyOk)
                {
                    model->ok_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_MUTE);
                }
                else if (event->key == InputKeyUp)
                {
                    model->up_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_VOL_UP);
                }
                else if (event->key == InputKeyDown)
                {
                    model->down_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_VOL_DOWN);
                }
                else if (event->key == InputKeyLeft)
                {
                    model->left_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_PREV_CHAN);
                }
                else if (event->key == InputKeyRight)
                {
                    model->right_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_NEXT_CHAN);
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
    return xremote_view_alloc(app_ctx,
        xremote_control_view_input_callback,
        xremote_control_view_draw_callback);
}