/*!
 *  @file flipper-xremote/views/xremote_player_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Playback page view callbacks and infrared functionality.
 */

#include "xremote_player_view.h"
#include "../xremote_app.h"

static void xremote_player_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppContext* app_ctx = model->context;

    xremote_canvas_draw_header(canvas, "Playback");
    xremote_canvas_draw_button(canvas, model->up_pressed, 23, 30, XRemoteIconJumpForward);
    xremote_canvas_draw_button(canvas, model->down_pressed, 23, 72, XRemoteIconJumpBackward);
    xremote_canvas_draw_button(canvas, model->ok_pressed, 23, 51, XRemoteIconPlayPause);
    xremote_canvas_draw_button(canvas, model->left_pressed, 2, 51, XRemoteIconFastBackward);
    xremote_canvas_draw_button(canvas, model->right_pressed, 44, 51, XRemoteIconFastForward);

    if(app_ctx->app_settings->exit_behavior == XRemoteAppExitHold) {
        xremote_canvas_draw_button(canvas, model->back_pressed, 2, 95, XRemoteIconStop);
    } else {
        xremote_canvas_draw_button_wide(
            canvas, model->back_pressed, 2, 95, "Hold", XRemoteIconStop);
        xremote_canvas_draw_icon(canvas, 50, 102, XRemoteIconBack);
    }

    xremote_canvas_draw_exit_footer(canvas, xremote_app_context_get_exit_str(app_ctx));
}

static void xremote_player_view_process(XRemoteView* view, InputEvent* event) {
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            model->context = xremote_view_get_app_context(view);

            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_JUMP_FORWARD);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_JUMP_BACKWARD);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_FAST_BACKWARD);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_FAST_FORWARD);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_PLAY_PAUSE);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                    xremote_view_send_ir(view, XREMOTE_COMMAND_STOP);
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp)
                    model->up_pressed = false;
                else if(event->key == InputKeyDown)
                    model->down_pressed = false;
                else if(event->key == InputKeyLeft)
                    model->left_pressed = false;
                else if(event->key == InputKeyRight)
                    model->right_pressed = false;
                else if(event->key == InputKeyOk)
                    model->ok_pressed = false;
                else if(event->key == InputKeyBack)
                    model->back_pressed = false;
            }
        },
        true);
}

static bool xremote_player_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    XRemoteAppContext* app_ctx = xremote_view_get_app_context(view);
    XRemoteAppExit exit = app_ctx->app_settings->exit_behavior;

    if(event->key == InputKeyBack && event->type == InputTypeShort && exit == XRemoteAppExitPress)
        return false;
    else if(event->key == InputKeyBack && event->type == InputTypeLong && exit == XRemoteAppExitHold)
        return false;

    xremote_player_view_process(view, event);
    return true;
}

XRemoteView* xremote_player_view_alloc(void* app_ctx) {
    XRemoteView* view = xremote_view_alloc(
        app_ctx, xremote_player_view_input_callback, xremote_player_view_draw_callback);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            model->context = xremote_view_get_app_context(view);
            model->up_pressed = false;
            model->down_pressed = false;
            model->left_pressed = false;
            model->right_pressed = false;
            model->back_pressed = false;
            model->ok_pressed = false;
        },
        true);

    return view;
}
