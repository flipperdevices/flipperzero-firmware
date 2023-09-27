/*!
 *  @file flipper-xremote/views/xremote_player_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Playback page view callbacks and infrared functionality.
 */

#include "xremote_player_view.h"

static void xremote_player_view_draw_callback(Canvas* canvas, void* context)
{
    furi_assert(context);
    XRemoteViewModel* model = context;

    xremote_canvas_draw_header(canvas, "Playback");
    xremote_canvas_draw_button(canvas, model->up_pressed, 23, 30, XRemoteIconJumpForward);
    xremote_canvas_draw_button(canvas, model->down_pressed, 23, 72, XRemoteIconJumpBackward);
    xremote_canvas_draw_button(canvas, model->ok_pressed, 23, 51, XRemoteIconPlayPause);
    xremote_canvas_draw_button(canvas, model->left_pressed, 2, 51, XRemoteIconFastBackward);
    xremote_canvas_draw_button(canvas, model->right_pressed, 44, 51, XRemoteIconFastForward);
    xremote_canvas_draw_button(canvas, model->back_pressed, 2, 95, XRemoteIconStop);
    xremote_canvas_draw_exit_footer(canvas, "Hold to exit");
}

static void xremote_player_view_process(XRemoteView* view, InputEvent* event)
{
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel* model,
        {
            if (event->type == InputTypePress)
            {
                if (event->key == InputKeyUp)
                {
                    model->up_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_JUMP_FORWARD);
                }
                else if (event->key == InputKeyDown)
                {
                    model->down_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_JUMP_BACKWARD);
                }
                else if (event->key == InputKeyLeft)
                {
                    model->left_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_FAST_BACKWARD);
                }
                else if (event->key == InputKeyRight)
                {
                    model->right_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_FAST_FORWARD);
                }
                else if (event->key == InputKeyOk)
                {
                    model->ok_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_PAUSE_PLAY);
                }
                else if (event->key == InputKeyBack)
                {
                    model->back_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_STOP);
                }
            }
            else if (event->type == InputTypeRelease)
            {
                if (event->key == InputKeyUp) model->up_pressed = false;
                else if (event->key == InputKeyDown) model->down_pressed = false;
                else if (event->key == InputKeyLeft) model->left_pressed = false;
                else if (event->key == InputKeyRight) model->right_pressed = false;
                else if (event->key == InputKeyOk) model->ok_pressed = false;
                else if (event->key == InputKeyBack) model->back_pressed = false;
            }
        },
        true);
}

static bool xremote_player_view_input_callback(InputEvent* event, void* context)
{
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;

    if (event->type == InputTypeLong &&
        event->key == InputKeyBack) return false;

    xremote_player_view_process(view, event);
    return true;
}

XRemoteView* xremote_player_view_alloc(NotificationApp* notifications)
{
    return xremote_view_alloc(notifications,
        xremote_player_view_input_callback,
        xremote_player_view_draw_callback);
}
