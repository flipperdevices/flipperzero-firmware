/*!
 *  @file flipper-xremote/xremote_analyzer.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Infrared remote singnal analyzer and custom view events.
 */

#include "xremote_analyzer.h"
#include "views/xremote_signal_view.h"

struct XRemoteSignalAnalyzer {
    XRemoteSignalReceiver* ir_receiver;
    XRemoteClearCallback on_clear;
    XRemoteAppContext* app_ctx;
    InfraredSignal* ir_signal;
    XRemoteView* signal_view;
    void* context;
    bool pause;
};

InfraredSignal* xremote_signal_analyzer_get_ir_signal(XRemoteSignalAnalyzer* analyzer) {
    xremote_app_assert(analyzer, NULL);
    return analyzer->ir_signal;
}

XRemoteSignalReceiver* xremote_signal_analyzer_get_ir_receiver(XRemoteSignalAnalyzer* analyzer) {
    xremote_app_assert(analyzer, NULL);
    return analyzer->ir_receiver;
}

XRemoteAppContext* xremote_signal_analyzer_get_app_context(XRemoteSignalAnalyzer* analyzer) {
    xremote_app_assert(analyzer, NULL);
    return analyzer->app_ctx;
}

void xremote_signal_analyzer_send_event(XRemoteSignalAnalyzer* analyzer, XRemoteEvent event) {
    xremote_app_assert_void(analyzer);
    ViewDispatcher* view_disp = analyzer->app_ctx->view_dispatcher;
    view_dispatcher_send_custom_event(view_disp, event);
}

static void
    xremote_signal_analyzer_switch_to_view(XRemoteSignalAnalyzer* analyzer, XRemoteViewID view_id) {
    xremote_app_assert_void(analyzer);
    ViewDispatcher* view_disp = analyzer->app_ctx->view_dispatcher;
    view_dispatcher_switch_to_view(view_disp, view_id);
}

static void xremote_signal_analyzer_rx_stop(XRemoteSignalAnalyzer* analyzer) {
    xremote_app_assert_void(analyzer);
    analyzer->pause = true;
    xremote_signal_receiver_stop(analyzer->ir_receiver);
}

static void xremote_signal_analyzer_rx_start(XRemoteSignalAnalyzer* analyzer) {
    xremote_app_assert_void(analyzer);
    analyzer->pause = false;
    xremote_signal_receiver_start(analyzer->ir_receiver);
}

static uint32_t xremote_signal_analyzer_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewAnalyzer;
}

static void xremote_signal_analyzer_signal_callback(void* context, InfraredSignal* signal) {
    XRemoteSignalAnalyzer* analyzer = context;
    xremote_app_assert_void(!analyzer->pause);
    analyzer->pause = true;

    infrared_signal_set_signal(analyzer->ir_signal, signal);
    xremote_signal_analyzer_send_event(analyzer, XRemoteEventSignalReceived);
}

static bool xremote_signal_analyzer_custom_event_callback(void* context, uint32_t event) {
    xremote_app_assert(context, false);
    XRemoteSignalAnalyzer* analyzer = context;

    if(event == XRemoteEventSignalExit) {
        xremote_signal_analyzer_rx_stop(analyzer);
        xremote_signal_analyzer_switch_to_view(analyzer, XRemoteViewSubmenu);
    } else if(event == XRemoteEventSignalReceived) {
        xremote_signal_analyzer_rx_stop(analyzer);
        xremote_signal_analyzer_switch_to_view(analyzer, XRemoteViewSignal);
    } else if(event == XRemoteEventSignalRetry) {
        xremote_signal_analyzer_rx_start(analyzer);
        xremote_signal_analyzer_switch_to_view(analyzer, XRemoteViewAnalyzer);
    } else if(event == XRemoteEventSignalSend) {
        XRemoteAppContext* app_ctx = analyzer->app_ctx;
        xremote_app_send_signal(app_ctx, analyzer->ir_signal);
    }

    return true;
}

static XRemoteSignalAnalyzer* xremote_signal_analyzer_alloc(XRemoteAppContext* app_ctx) {
    XRemoteSignalAnalyzer* analyzer = malloc(sizeof(XRemoteSignalAnalyzer));
    analyzer->ir_signal = infrared_signal_alloc();
    analyzer->app_ctx = app_ctx;
    analyzer->pause = false;

    analyzer->signal_view = xremote_signal_success_view_alloc(app_ctx, analyzer);
    View* view = xremote_view_get_view(analyzer->signal_view);
    view_set_previous_callback(view, xremote_signal_analyzer_view_exit_callback);
    view_dispatcher_add_view(app_ctx->view_dispatcher, XRemoteViewSignal, view);

    view_dispatcher_set_custom_event_callback(
        app_ctx->view_dispatcher, xremote_signal_analyzer_custom_event_callback);
    view_dispatcher_set_event_callback_context(app_ctx->view_dispatcher, analyzer);

    analyzer->ir_receiver = xremote_signal_receiver_alloc(app_ctx);
    xremote_signal_receiver_set_context(analyzer->ir_receiver, analyzer, NULL);
    xremote_signal_receiver_set_rx_callback(
        analyzer->ir_receiver, xremote_signal_analyzer_signal_callback);

    return analyzer;
}

static void xremote_signal_analyzer_free(XRemoteSignalAnalyzer* analyzer) {
    xremote_app_assert_void(analyzer);
    xremote_signal_receiver_stop(analyzer->ir_receiver);

    ViewDispatcher* view_disp = analyzer->app_ctx->view_dispatcher;
    view_dispatcher_set_custom_event_callback(view_disp, NULL);
    view_dispatcher_set_event_callback_context(view_disp, NULL);

    view_dispatcher_remove_view(view_disp, XRemoteViewSignal);
    xremote_view_free(analyzer->signal_view);

    xremote_signal_receiver_free(analyzer->ir_receiver);
    infrared_signal_free(analyzer->ir_signal);
    free(analyzer);
}

static void xremote_signal_analyzer_clear_callback(void* context) {
    XRemoteSignalAnalyzer* analyzer = context;
    xremote_signal_analyzer_free(analyzer);
}

XRemoteApp* xremote_analyzer_alloc(XRemoteAppContext* app_ctx) {
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    app->view_id = XRemoteViewAnalyzer;

    XRemoteSignalAnalyzer* analyzer = xremote_signal_analyzer_alloc(app_ctx);
    app->view_ctx = xremote_signal_view_alloc(app->app_ctx, analyzer);
    View* view = xremote_view_get_view(app->view_ctx);

    ViewDispatcher* view_disp = app_ctx->view_dispatcher;
    view_dispatcher_add_view(view_disp, app->view_id, view);

    xremote_app_view_set_previous_callback(app, xremote_signal_analyzer_view_exit_callback);
    xremote_app_set_view_context(app, analyzer, xremote_signal_analyzer_clear_callback);

    xremote_signal_receiver_start(analyzer->ir_receiver);
    return app;
}
