/*!
 *  @file flipper-xremote/views/xremote_signal_view.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Signal analyzer page view components and functionality.
 */

#include "xremote_signal_view.h"
#include "../xremote_analyzer.h"
#include "../xremote_app.h"

static void xremote_signal_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteSignalAnalyzer* analyzer = model->context;
    XRemoteAppContext* app_ctx = xremote_signal_analyzer_get_app_context(analyzer);

    ViewOrientation orientation = app_ctx->app_settings->orientation;
    uint8_t y = orientation == ViewOrientationHorizontal ? 17 : 49;
    const char* text = "Press any\nbutton on\nthe remote.";

    xremote_canvas_draw_header(canvas, orientation, "Analyzer");
    elements_multiline_text_aligned(canvas, 0, y, AlignLeft, AlignTop, text);

    const char* exit_str = xremote_app_context_get_exit_str(app_ctx);
    xremote_canvas_draw_exit_footer(canvas, orientation, exit_str);
}

static void xremote_signal_success_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XRemoteViewModel* model = context;
    XRemoteSignalAnalyzer* analyzer = model->context;

    XRemoteAppContext* app_ctx = xremote_signal_analyzer_get_app_context(analyzer);
    InfraredSignal* ir_signal = xremote_signal_analyzer_get_ir_signal(analyzer);

    xremote_canvas_draw_header(canvas, app_ctx->app_settings->orientation, "IR Signal");
    char signal_info[128];

    if(infrared_signal_is_raw(ir_signal)) {
        InfraredRawSignal* raw = infrared_signal_get_raw_signal(ir_signal);

        snprintf(
            signal_info,
            sizeof(signal_info),
            "Type: RAW\n"
            "T-Size: %u\n"
            "D-Cycle: %.2f\n",
            raw->timings_size,
            (double)raw->duty_cycle);
    } else {
        InfraredMessage* message = infrared_signal_get_message(ir_signal);
        const char* infrared_protocol = infrared_get_protocol_name(message->protocol);

        snprintf(
            signal_info,
            sizeof(signal_info),
            "Proto: %s\n"
            "Addr: 0x%lX\n"
            "Cmd: 0x%lX\n",
            infrared_protocol,
            message->address,
            message->command);
    }

    if(app_ctx->app_settings->orientation == ViewOrientationHorizontal) {
        elements_multiline_text_aligned(canvas, 0, 17, AlignLeft, AlignTop, signal_info);
        xremote_canvas_draw_button_wide(
            canvas, model->ok_pressed, 68, 26, "Send", XRemoteIconEnter);
        xremote_canvas_draw_button_wide(
            canvas, model->back_pressed, 68, 44, "Retry", XRemoteIconBack);
    } else {
        elements_multiline_text_aligned(canvas, 0, 39, AlignLeft, AlignTop, signal_info);
        xremote_canvas_draw_button_wide(
            canvas, model->ok_pressed, 0, 88, "Send", XRemoteIconEnter);
        xremote_canvas_draw_button_wide(
            canvas, model->back_pressed, 0, 106, "Retry", XRemoteIconBack);
    }
}

static void xremote_signal_success_view_process(XRemoteView* view, InputEvent* event) {
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            XRemoteSignalAnalyzer* analyzer = xremote_view_get_context(view);
            model->context = analyzer;

            if(event->type == InputTypePress) {
                if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    xremote_signal_analyzer_send_event(analyzer, XRemoteEventSignalSend);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                    xremote_signal_analyzer_send_event(analyzer, XRemoteEventSignalRetry);
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyOk)
                    model->ok_pressed = false;
                else if(event->key == InputKeyBack)
                    model->back_pressed = false;
            }
        },
        true);
}

static void xremote_signal_view_process(XRemoteView* view, InputEvent* event) {
    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            XRemoteSignalAnalyzer* analyzer = xremote_view_get_context(view);
            XRemoteAppContext* app_ctx = xremote_view_get_app_context(view);

            XRemoteAppExit exit = app_ctx->app_settings->exit_behavior;
            model->context = analyzer;

            if((event->type == InputTypeShort || event->type == InputTypeLong) &&
               event->key == InputKeyBack) {
                if((event->type == InputTypeShort && exit == XRemoteAppExitPress) ||
                   (event->type == InputTypeLong && exit == XRemoteAppExitHold)) {
                    model->back_pressed = true;
                    xremote_signal_analyzer_send_event(analyzer, XRemoteEventSignalExit);
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyOk)
                    model->ok_pressed = false;
                else if(event->key == InputKeyBack)
                    model->back_pressed = false;
                else if(event->key == InputKeyRight)
                    model->right_pressed = false;
            }
        },
        true);
}

static bool xremote_signal_success_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    xremote_signal_success_view_process(view, event);
    return true;
}

static bool xremote_signal_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    XRemoteView* view = (XRemoteView*)context;
    xremote_signal_view_process(view, event);
    return true;
}

XRemoteView* xremote_signal_success_view_alloc(void* app_ctx, void* analyzer) {
    XRemoteView* view = xremote_view_alloc(
        app_ctx,
        xremote_signal_success_view_input_callback,
        xremote_signal_success_view_draw_callback);
    xremote_view_set_context(view, analyzer, NULL);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            model->context = analyzer;
            model->back_pressed = false;
            model->ok_pressed = false;
        },
        true);

    return view;
}

XRemoteView* xremote_signal_view_alloc(void* app_ctx, void* analyzer) {
    XRemoteView* view = xremote_view_alloc(
        app_ctx, xremote_signal_view_input_callback, xremote_signal_view_draw_callback);
    xremote_view_set_context(view, analyzer, NULL);

    with_view_model(
        xremote_view_get_view(view),
        XRemoteViewModel * model,
        {
            model->context = analyzer;
            model->back_pressed = false;
        },
        true);

    return view;
}
