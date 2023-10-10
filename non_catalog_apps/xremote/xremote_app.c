/*!
 *  @file flipper-xremote/xremote_app.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Shared functionality and data types between the apps.
 */

#include "xremote_app.h"

#define XREMOTE_APP_SETTINGS APP_DATA_PATH("xremote.cfg")
#define TAG "XRemoteApp"

const NotificationSequence g_sequence_blink_purple_50 = {
    &message_red_255,
    &message_blue_255,
    &message_delay_50,
    NULL,
};

XRemoteAppSettings* xremote_app_settings_alloc() {
    XRemoteAppSettings* settings = malloc(sizeof(XRemoteAppSettings));
    settings->orientation = ViewOrientationHorizontal;
    settings->exit_behavior = XRemoteAppExitPress;
    settings->repeat_count = 2;
    return settings;
}

void xremote_app_settings_free(XRemoteAppSettings* settings) {
    xremote_app_assert_void(settings);
    free(settings);
}

bool xremote_app_settings_store(XRemoteAppSettings* settings) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);

    FURI_LOG_I(TAG, "store config file: \'%s\'", XREMOTE_APP_SETTINGS);
    bool success = false;

    do {
        /* Write header in config file */
        if(!flipper_format_file_open_always(ff, XREMOTE_APP_SETTINGS)) break;
        if(!flipper_format_write_header_cstr(ff, "XRemote", 1)) break;
        if(!flipper_format_write_comment_cstr(ff, "")) break;

        /* Write actual configuration to the settings file */
        uint32_t value = settings->orientation == ViewOrientationHorizontal ? 0 : 1;
        if(!flipper_format_write_uint32(ff, "orientation", &value, 1)) break;

        value = settings->exit_behavior == XRemoteAppExitPress ? 0 : 1;
        if(!flipper_format_write_uint32(ff, "appexit", &value, 1)) break;

        value = settings->repeat_count;
        if(!flipper_format_write_uint32(ff, "repeat", &value, 1)) break;

        success = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    flipper_format_free(ff);

    return success;
}

bool xremote_app_settings_load(XRemoteAppSettings* settings) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* header = furi_string_alloc();

    FURI_LOG_I(TAG, "load config file: \'%s\'", XREMOTE_APP_SETTINGS);
    uint32_t version = 0;
    uint32_t value = 0;
    bool success = false;

    do {
        /* Open file and read the header */
        if(!flipper_format_buffered_file_open_existing(ff, XREMOTE_APP_SETTINGS)) break;
        if(!flipper_format_read_header(ff, header, &version)) break;
        if(!furi_string_equal(header, "XRemote") || (version != 1)) break;

        /* Parse config data from the buffer */
        if(!flipper_format_read_uint32(ff, "orientation", &value, 1)) break;
        settings->orientation = value == 0 ? ViewOrientationHorizontal : ViewOrientationVertical;
        if(!flipper_format_read_uint32(ff, "appexit", &value, 1)) break;
        settings->exit_behavior = value == 0 ? XRemoteAppExitPress : XRemoteAppExitHold;
        if(!flipper_format_read_uint32(ff, "repeat", &value, 1)) break;
        settings->repeat_count = value;

        success = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    furi_string_free(header);
    flipper_format_free(ff);

    return success;
}

XRemoteAppContext* xremote_app_context_alloc(void* arg) {
    XRemoteAppContext* ctx = malloc(sizeof(XRemoteAppContext));
    ctx->app_argument = arg;
    ctx->file_path = NULL;

    /* Open GUI and norification records */
    ctx->gui = furi_record_open(RECORD_GUI);
    ctx->notifications = furi_record_open(RECORD_NOTIFICATION);

    /* Allocate and load global app settings */
    ctx->app_settings = xremote_app_settings_alloc();
    xremote_app_settings_load(ctx->app_settings);

    /* Allocate and setup view dispatcher */
    ctx->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(ctx->view_dispatcher);
    view_dispatcher_attach_to_gui(ctx->view_dispatcher, ctx->gui, ViewDispatcherTypeFullscreen);

    return ctx;
}

void xremote_app_context_free(XRemoteAppContext* ctx) {
    xremote_app_assert_void(ctx);
    notification_internal_message(ctx->notifications, &sequence_reset_blue);

    xremote_app_settings_free(ctx->app_settings);
    view_dispatcher_free(ctx->view_dispatcher);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    if(ctx->file_path != NULL) {
        furi_string_free(ctx->file_path);
        ctx->file_path = NULL;
    }

    free(ctx);
}

const char* xremote_app_context_get_exit_str(XRemoteAppContext* ctx) {
    XRemoteAppExit exit_behavior = ctx->app_settings->exit_behavior;
    return exit_behavior == XRemoteAppExitHold ? "Hold to exit" : "Press to exit";
}

void xremote_app_notification_blink(NotificationApp* notifications) {
    xremote_app_assert_void(notifications);
    notification_message(notifications, &g_sequence_blink_purple_50);
}

void xremote_app_context_notify_led(XRemoteAppContext* app_ctx) {
    xremote_app_assert_void(app_ctx);
    xremote_app_notification_blink(app_ctx->notifications);
}

bool xremote_app_send_signal(XRemoteAppContext* app_ctx, InfraredSignal* signal) {
    xremote_app_assert(signal, false);
    XRemoteAppSettings* settings = app_ctx->app_settings;
    infrared_signal_transmit_times(signal, settings->repeat_count);
    xremote_app_context_notify_led(app_ctx);
    return true;
}

void xremote_app_view_alloc(XRemoteApp* app, uint32_t view_id, XRemoteViewAllocator allocator) {
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);

    if(app->view_id == view_id && app->view_ctx != NULL) return;

    xremote_app_view_free(app);
    app->view_id = view_id;

    app->view_ctx = allocator(app->app_ctx);
    View* app_view = xremote_view_get_view(app->view_ctx);

    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_add_view(view_disp, app->view_id, app_view);
}

void xremote_app_view_free(XRemoteApp* app) {
    xremote_app_assert_void(app);

    if(app->app_ctx != NULL && app->view_id != XRemoteViewNone) {
        ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
        view_dispatcher_remove_view(view_disp, app->view_id);
        app->view_id = XRemoteViewNone;
    }

    if(app->view_ctx != NULL) {
        xremote_view_free(app->view_ctx);
        app->view_ctx = NULL;
    }
}

bool xremote_app_has_view(XRemoteApp* app, uint32_t view_id) {
    xremote_app_assert(app, false);
    return (app->view_id == view_id || app->submenu_id == view_id);
}

void xremote_app_switch_to_view(XRemoteApp* app, uint32_t view_id) {
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);
    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_switch_to_view(view_disp, view_id);
}

void xremote_app_switch_to_submenu(XRemoteApp* app) {
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);
    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_switch_to_view(view_disp, app->submenu_id);
}

void xremote_app_submenu_add(
    XRemoteApp* app,
    const char* name,
    uint32_t index,
    SubmenuItemCallback callback) {
    furi_assert(app);
    xremote_app_assert_void(app->submenu);
    submenu_add_item(app->submenu, name, index, callback, app);
}

void xremote_app_submenu_alloc(XRemoteApp* app, uint32_t index, ViewNavigationCallback prev_cb) {
    furi_assert(app);
    app->submenu = submenu_alloc();
    app->submenu_id = index;

    XRemoteAppSettings* settings = app->app_ctx->app_settings;
    View* view = submenu_get_view(app->submenu);
    view_set_previous_callback(view, prev_cb);

#if defined(FW_ORIGIN_Unleashed) || defined(FW_ORIGIN_RM)
    submenu_set_orientation(app->submenu, settings->orientation);
#else
    view_set_orientation(view, settings->orientation);
#endif

    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_add_view(view_disp, app->submenu_id, view);
}

void xremote_app_submenu_free(XRemoteApp* app) {
    xremote_app_assert_void(app);

    /* Remove submenu view from dispatcher */
    if(app->submenu_id != XRemoteViewNone && app->app_ctx != NULL) {
        ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
        view_dispatcher_remove_view(view_disp, app->submenu_id);
        app->submenu_id = XRemoteViewNone;
    }

    /* Free submenu */
    if(app->submenu != NULL) {
        submenu_free(app->submenu);
        app->submenu = NULL;
    }
}

void xremote_app_view_set_previous_callback(XRemoteApp* app, ViewNavigationCallback callback) {
    furi_assert(app);
    xremote_app_assert_void(app->view_ctx);
    View* view = xremote_view_get_view(app->view_ctx);
    view_set_previous_callback(view, callback);
}

void xremote_app_set_view_context(XRemoteApp* app, void* context, XRemoteClearCallback on_clear) {
    furi_assert(app);
    xremote_app_assert_void(app->view_ctx);
    xremote_view_set_context(app->view_ctx, context, on_clear);
}

void xremote_app_set_user_context(XRemoteApp* app, void* context, XRemoteClearCallback on_clear) {
    furi_assert(app);
    app->on_clear = on_clear;
    app->context = context;
}

void xremote_app_user_context_free(XRemoteApp* app) {
    furi_assert(app);
    xremote_app_assert_void(app->context);
    xremote_app_assert_void(app->on_clear);

    app->on_clear(app->context);
    app->on_clear = NULL;
    app->context = NULL;
}

XRemoteApp* xremote_app_alloc(XRemoteAppContext* ctx) {
    furi_assert(ctx);
    XRemoteApp* app = malloc(sizeof(XRemoteApp));
    app->submenu_id = XRemoteViewNone;
    app->view_id = XRemoteViewNone;
    app->app_ctx = ctx;
    app->submenu = NULL;
    app->view_ctx = NULL;
    app->on_clear = NULL;
    app->context = NULL;
    return app;
}

void xremote_app_free(XRemoteApp* app) {
    xremote_app_assert_void(app);
    xremote_app_submenu_free(app);
    xremote_app_view_free(app);

    /* Call clear callback if there is an user context attached  */
    if(app->on_clear != NULL) app->on_clear(app->context);

    free(app);
}
