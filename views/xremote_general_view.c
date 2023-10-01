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
    furi_assert(context);
    XRemoteViewModel* model = context;

    xremote_canvas_draw_header(canvas, "General");
    xremote_canvas_draw_button_wide(canvas, model->ok_pressed, 0, 27, "Power", XRemoteIconEnter);
    xremote_canvas_draw_button_wide(canvas, model->up_pressed, 0, 45, "Input", XRemoteIconArrowUp);
    xremote_canvas_draw_button_wide(canvas, model->down_pressed, 0, 63, "Setup", XRemoteIconArrowDown);
    xremote_canvas_draw_button_wide(canvas, model->left_pressed, 0, 81, "Menu", XRemoteIconArrowLeft);
    xremote_canvas_draw_button_wide(canvas, model->right_pressed, 0, 99, "List", XRemoteIconArrowRight);
    xremote_canvas_draw_exit_footer(canvas, "Press to exit");
}

static void xremote_general_view_process(XRemoteView* view, InputEvent* event)
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
                    xremote_view_send_ir(view, XREMOTE_COMMAND_POWER);
                }
                else if (event->key == InputKeyUp)
                {
                    model->up_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_INPUT);
                }
                else if (event->key == InputKeyDown)
                {
                    model->down_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_SETUP);
                }
                else if (event->key == InputKeyLeft)
                {
                    model->left_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_MENU);
                }
                else if (event->key == InputKeyRight)
                {
                    model->right_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_LIST);
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

static bool xremote_general_view_input_callback(InputEvent* event, void* context)
{
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    if (event->key == InputKeyBack) return false;

    xremote_general_view_process(view, event);
    return true;
}

XRemoteView* xremote_general_view_alloc(void* app_ctx)
{
    return xremote_view_alloc(app_ctx,
        xremote_general_view_input_callback,
        xremote_general_view_draw_callback);
}