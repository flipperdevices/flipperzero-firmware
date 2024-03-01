/*!
 *  @file flipper-xremote/views/xremote_custom_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Custom layout UI and view functionality for remote buttons.
 */

#include "xremote_custom_view.h"
#include "../xremote_app.h"

static void xremote_custom_view_draw_vertical(Canvas* canvas, XRemoteViewModel* model) {
    XRemoteAppButtons* buttons = model->context;

    FuriString* text = model->hold && model->ok_pressed ? buttons->custom_ok_hold :
                                                          buttons->custom_ok;
    xremote_canvas_draw_button_wide(
        canvas, model->ok_pressed, 0, 27, furi_string_get_cstr(text), XRemoteIconEnter);

    text = model->hold && model->up_pressed ? buttons->custom_up_hold : buttons->custom_up;
    xremote_canvas_draw_button_wide(
        canvas, model->up_pressed, 0, 45, furi_string_get_cstr(text), XRemoteIconArrowUp);

    text = model->hold && model->down_pressed ? buttons->custom_down_hold : buttons->custom_down;
    xremote_canvas_draw_button_wide(
        canvas, model->down_pressed, 0, 63, furi_string_get_cstr(text), XRemoteIconArrowDown);

    text = model->hold && model->left_pressed ? buttons->custom_left_hold : buttons->custom_left;
    xremote_canvas_draw_button_wide(
        canvas, model->left_pressed, 0, 81, furi_string_get_cstr(text), XRemoteIconArrowLeft);

    text = model->hold && model->right_pressed ? buttons->custom_right_hold :
                                                 buttons->custom_right;
    xremote_canvas_draw_button_wide(
        canvas, model->right_pressed, 0, 99, furi_string_get_cstr(text), XRemoteIconArrowRight);
}

static void xremote_custom_view_draw_horizontal(Canvas* canvas, XRemoteViewModel* model) {
    XRemoteAppButtons* buttons = model->context;

    FuriString* text = model->hold && model->ok_pressed ? buttons->custom_ok_hold :
                                                          buttons->custom_ok;
    xremote_canvas_draw_button_wide(
        canvas, model->ok_pressed, 0, 7, furi_string_get_cstr(text), XRemoteIconEnter);

    text = model->hold && model->up_pressed ? buttons->custom_up_hold : buttons->custom_up;
    xremote_canvas_draw_button_wide(
        canvas, model->up_pressed, 0, 25, furi_string_get_cstr(text), XRemoteIconArrowUp);

    text = model->hold && model->down_pressed ? buttons->custom_down_hold : buttons->custom_down;
    xremote_canvas_draw_button_wide(
        canvas, model->down_pressed, 0, 43, furi_string_get_cstr(text), XRemoteIconArrowDown);

    text = model->hold && model->left_pressed ? buttons->custom_left_hold : buttons->custom_left;
    xremote_canvas_draw_button_wide(
        canvas, model->left_pressed, 64, 20, furi_string_get_cstr(text), XRemoteIconArrowLeft);

    text = model->hold && model->right_pressed ? buttons->custom_right_hold :
                                                 buttons->custom_right;
    xremote_canvas_draw_button_wide(
        canvas, model->right_pressed, 64, 38, furi_string_get_cstr(text), XRemoteIconArrowRight);
}

static void xremote_custom_view_draw_page_name(Canvas* canvas, ViewOrientation orientation) {
    Align align = orientation == ViewOrientationHorizontal ? AlignRight : AlignLeft;
    uint8_t x = orientation == ViewOrientationHorizontal ? 128 : 0;
    uint8_t y = orientation == ViewOrientationHorizontal ? 10 : 12;
    elements_multiline_text_aligned(canvas, x, y, align, AlignTop, "Custom");
}

static void xremote_custom_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteAppButtons* buttons = model->context;
    XRemoteAppContext* app_ctx = buttons->app_ctx;

    const char* exit_str = xremote_app_context_get_exit_str(app_ctx);
    ViewOrientation orientation = app_ctx->app_settings->orientation;

    XRemoteViewDrawFunction xremote_custom_view_draw_body;
    xremote_custom_view_draw_body = orientation == ViewOrientationVertical ?
                                        xremote_custom_view_draw_vertical :
                                        xremote_custom_view_draw_horizontal;

    xremote_canvas_draw_header(canvas, orientation, NULL);
    xremote_custom_view_draw_page_name(canvas, orientation);
    xremote_custom_view_draw_body(canvas, model);
    xremote_canvas_draw_exit_footer(canvas, orientation, exit_str);
}

static void xremote_custom_view_process(XRemoteView* view, InputEvent* event) {
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            XRemoteAppButtons* buttons = xremote_view_get_context(view);
            XRemoteAppContext* app_ctx = buttons->app_ctx;
            XRemoteAppExit exit_behavior = app_ctx->app_settings->exit_behavior;
            model->context = buttons;

            if(event->type == InputTypeShort) {
                InfraredRemoteButton* button = NULL;

                if(event->key == InputKeyOk) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_ok);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->ok_pressed = true;
                } else if(event->key == InputKeyUp) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_up);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->up_pressed = true;
                } else if(event->key == InputKeyDown) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_down);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->down_pressed = true;
                } else if(event->key == InputKeyLeft) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_left);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->left_pressed = true;
                } else if(event->key == InputKeyRight) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_right);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->right_pressed = true;
                } else if(event->key == InputKeyBack && exit_behavior == XRemoteAppExitHold) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_BACK);
                    if(xremote_view_press_button(view, button)) model->back_pressed = true;
                }
            } else if(event->type == InputTypeLong) {
                InfraredRemoteButton* button = NULL;
                model->hold = true;

                if(event->key == InputKeyOk) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_ok_hold);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->ok_pressed = true;
                } else if(event->key == InputKeyUp) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_up_hold);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->up_pressed = true;
                } else if(event->key == InputKeyDown) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_down_hold);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->down_pressed = true;
                } else if(event->key == InputKeyLeft) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_left_hold);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->left_pressed = true;
                } else if(event->key == InputKeyRight) {
                    const char* button_name = furi_string_get_cstr(buttons->custom_right_hold);
                    button = xremote_view_get_button_by_name(view, button_name);
                    if(xremote_view_press_button(view, button)) model->right_pressed = true;
                } else if(event->key == InputKeyBack && exit_behavior == XRemoteAppExitPress) {
                    button = xremote_view_get_button_by_name(view, XREMOTE_COMMAND_BACK);
                    if(xremote_view_press_button(view, button)) model->back_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                model->hold = false;

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
                else if(event->key == InputKeyBack)
                    model->back_pressed = false;
            }
        },
        true);
}

static bool xremote_custom_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    XRemoteAppContext* app_ctx = xremote_view_get_app_context(view);
    XRemoteAppExit exit_behavior = app_ctx->app_settings->exit_behavior;

    if(event->key == InputKeyBack) {
        if(event->type == InputTypeShort && exit_behavior == XRemoteAppExitPress)
            return false;
        else if(event->type == InputTypeLong && exit_behavior == XRemoteAppExitHold)
            return false;
    }

    xremote_custom_view_process(view, event);
    return true;
}

XRemoteView* xremote_custom_view_alloc(void* app_ctx, void* model_ctx) {
    XRemoteView* view = xremote_view_alloc(
        app_ctx, xremote_custom_view_input_callback, xremote_custom_view_draw_callback);
    xremote_view_set_context(view, model_ctx, NULL);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            model->context = model_ctx;
            model->up_pressed = false;
            model->down_pressed = false;
            model->left_pressed = false;
            model->right_pressed = false;
            model->back_pressed = false;
            model->ok_pressed = false;
            model->hold = false;
        },
        true);

    return view;
}
