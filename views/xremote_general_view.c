/*!
 *  @file flipper-xremote/views/xremote_general_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief General remote buttons section UI and view functionality.
 */

#include "xremote_general_view.h"
#include "../xremote_app.h"

static void xremote_general_view_draw_vertical(Canvas* canvas, XRemoteViewModel* model) {
    xremote_canvas_draw_button_wide(canvas, model->ok_pressed, 0, 27, "Power", XRemoteIconEnter);
    xremote_canvas_draw_button_wide(canvas, model->up_pressed, 0, 45, "Input", XRemoteIconArrowUp);
    xremote_canvas_draw_button_wide(
        canvas, model->down_pressed, 0, 63, "Setup", XRemoteIconArrowDown);
    xremote_canvas_draw_button_wide(
        canvas, model->left_pressed, 0, 81, "Menu", XRemoteIconArrowLeft);
    xremote_canvas_draw_button_wide(
        canvas, model->right_pressed, 0, 99, "List", XRemoteIconArrowRight);
}

static void xremote_general_view_draw_horizontal(Canvas* canvas, XRemoteViewModel* model) {
    xremote_canvas_draw_button_wide(canvas, model->ok_pressed, 0, 7, "Power", XRemoteIconEnter);
    xremote_canvas_draw_button_wide(canvas, model->up_pressed, 0, 25, "Input", XRemoteIconArrowUp);
    xremote_canvas_draw_button_wide(
        canvas, model->down_pressed, 0, 43, "Setup", XRemoteIconArrowDown);
    xremote_canvas_draw_button_wide(
        canvas, model->left_pressed, 64, 21, "Menu", XRemoteIconArrowLeft);
    xremote_canvas_draw_button_wide(
        canvas, model->right_pressed, 64, 39, "List", XRemoteIconArrowRight);
}

static void xremote_general_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppContext* app_ctx = model->context;
    XRemoteViewDrawFunction xremote_general_view_draw_body;

    ViewOrientation orientation = app_ctx->app_settings->orientation;
    xremote_general_view_draw_body = orientation == ViewOrientationVertical ?
                                         xremote_general_view_draw_vertical :
                                         xremote_general_view_draw_horizontal;

    xremote_canvas_draw_header(canvas, orientation, "General");
    xremote_general_view_draw_body(canvas, model);
    xremote_canvas_draw_exit_footer(canvas, orientation, "Press to exit");
}

static void xremote_general_view_process(XRemoteView* view, InputEvent* event) {
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            if(event->type == InputTypePress) {
                model->context = xremote_view_get_app_context(view);
                InfraredRemoteButton* button = NULL;

                if(event->key == InputKeyOk) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_POWER);
                    if(xremote_view_press_button(view, button)) model->ok_pressed = true;
                } else if(event->key == InputKeyUp) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_INPUT);
                    if(xremote_view_press_button(view, button)) model->up_pressed = true;
                } else if(event->key == InputKeyDown) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_SETUP);
                    if(xremote_view_press_button(view, button)) model->down_pressed = true;
                } else if(event->key == InputKeyLeft) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_MENU);
                    if(xremote_view_press_button(view, button)) model->left_pressed = true;
                } else if(event->key == InputKeyRight) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_LIST);
                    if(xremote_view_press_button(view, button)) model->right_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyOk)
                    model->ok_pressed = false;
                else if(event->key == InputKeyUp)
                    model->up_pressed = false;
                else if(event->key == InputKeyDown)
                    model->down_pressed = false;
                else if(event->key == InputKeyLeft)
                    model->left_pressed = false;
                else if(event->key == InputKeyRight)
                    model->right_pressed = false;
            }
        },
        true);
}

static bool xremote_general_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    if(event->key == InputKeyBack) return false;

    xremote_general_view_process(view, event);
    return true;
}

XRemoteView* xremote_general_view_alloc(void* app_ctx) {
    XRemoteView* view = xremote_view_alloc(
        app_ctx, xremote_general_view_input_callback, xremote_general_view_draw_callback);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            model->context = app_ctx;
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