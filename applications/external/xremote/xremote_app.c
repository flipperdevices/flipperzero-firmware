/*!
 *  @file flipper-xremote/xremote_app.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Shared functionality and data types between the apps.
 */

#include "xremote_app.h"

//////////////////////////////////////////////////////////////////////////////
// XRemote generic functions and definitions
//////////////////////////////////////////////////////////////////////////////

#define XREMOTE_APP_SETTINGS APP_DATA_PATH("xremote.cfg")
#define TAG "XRemoteApp"

#define XREMOTE_ORIENTATION_TEXT_HORIZONTAL "Horizontal"
#define XREMOTE_ORIENTATION_TEXT_VERTICAL "Vertical"
#define XREMOTE_ORIENTATION_INDEX_HORIZONTAL 0
#define XREMOTE_ORIENTATION_INDEX_VERTICAL 1

#define XREMOTE_EXIT_BEHAVIOR_TEXT_PRESS "Press"
#define XREMOTE_EXIT_BEHAVIOR_TEXT_HOLD "Hold"
#define XREMOTE_EXIT_BEHAVIOR_INDEX_PRESS 0
#define XREMOTE_EXIT_BEHAVIOR_INDEX_HOLD 1

const NotificationSequence g_sequence_blink_purple_50 = {
    &message_red_255,
    &message_blue_255,
    &message_delay_50,
    NULL,
};

XRemoteAppExit xremote_app_get_exit_behavior(uint8_t exit_index) {
    return exit_index ? XRemoteAppExitHold : XRemoteAppExitPress;
}

ViewOrientation xremote_app_get_orientation(uint8_t orientation_index) {
    return orientation_index ? ViewOrientationVertical : ViewOrientationHorizontal;
}

const char* xremote_app_get_exit_str(XRemoteAppExit exit_behavior) {
    return exit_behavior == XRemoteAppExitPress ? "Press" : "Hold";
}

const char* xremote_app_get_orientation_str(ViewOrientation view_orientation) {
    return view_orientation == ViewOrientationHorizontal ? "Horizontal" : "Vertical";
}

uint32_t xremote_app_get_orientation_index(ViewOrientation view_orientation) {
    return view_orientation == ViewOrientationHorizontal ? 0 : 1;
}

uint32_t xremote_app_get_exit_index(XRemoteAppExit exit_behavior) {
    return exit_behavior == XRemoteAppExitPress ? 0 : 1;
}

void xremote_app_notification_blink(NotificationApp* notifications) {
    xremote_app_assert_void(notifications);
    notification_message(notifications, &g_sequence_blink_purple_50);
}

//////////////////////////////////////////////////////////////////////////////
// XRemote buttons and custom button pairs
//////////////////////////////////////////////////////////////////////////////

bool xremote_app_extension_load(XRemoteAppButtons* buttons, FuriString* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* tmp = furi_string_alloc();
    bool success = false;

    do {
        /* Open file and read the header */
        if(!flipper_format_buffered_file_open_existing(ff, furi_string_get_cstr(path))) break;

        if(!flipper_format_read_string(ff, "custom_ok", tmp)) break;
        furi_string_set(buttons->custom_ok, tmp);

        if(!flipper_format_read_string(ff, "custom_up", tmp)) break;
        furi_string_set(buttons->custom_up, tmp);

        if(!flipper_format_read_string(ff, "custom_down", tmp)) break;
        furi_string_set(buttons->custom_down, tmp);

        if(!flipper_format_read_string(ff, "custom_left", tmp)) break;
        furi_string_set(buttons->custom_left, tmp);

        if(!flipper_format_read_string(ff, "custom_right", tmp)) break;
        furi_string_set(buttons->custom_right, tmp);

        if(!flipper_format_read_string(ff, "custom_ok_hold", tmp)) break;
        furi_string_set(buttons->custom_ok_hold, tmp);

        if(!flipper_format_read_string(ff, "custom_up_hold", tmp)) break;
        furi_string_set(buttons->custom_up_hold, tmp);

        if(!flipper_format_read_string(ff, "custom_down_hold", tmp)) break;
        furi_string_set(buttons->custom_down_hold, tmp);

        if(!flipper_format_read_string(ff, "custom_left_hold", tmp)) break;
        furi_string_set(buttons->custom_left_hold, tmp);

        if(!flipper_format_read_string(ff, "custom_right_hold", tmp)) break;
        furi_string_set(buttons->custom_right_hold, tmp);

        success = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    flipper_format_free(ff);
    furi_string_free(tmp);

    return success;
}

bool xremote_app_extension_store(XRemoteAppButtons* buttons, FuriString* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_append(ff, furi_string_get_cstr(path))) break;
        if(!flipper_format_write_comment_cstr(ff, "XRemote extension")) break;

        if(!flipper_format_write_string(ff, "custom_ok", buttons->custom_ok)) break;
        if(!flipper_format_write_string(ff, "custom_up", buttons->custom_up)) break;
        if(!flipper_format_write_string(ff, "custom_down", buttons->custom_down)) break;
        if(!flipper_format_write_string(ff, "custom_left", buttons->custom_left)) break;
        if(!flipper_format_write_string(ff, "custom_right", buttons->custom_right)) break;
        if(!flipper_format_write_string(ff, "custom_ok_hold", buttons->custom_ok_hold)) break;
        if(!flipper_format_write_string(ff, "custom_up_hold", buttons->custom_up_hold)) break;
        if(!flipper_format_write_string(ff, "custom_down_hold", buttons->custom_down_hold)) break;
        if(!flipper_format_write_string(ff, "custom_left_hold", buttons->custom_left_hold)) break;
        if(!flipper_format_write_string(ff, "custom_right_hold", buttons->custom_right_hold))
            break;

        success = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    flipper_format_free(ff);

    return success;
}

void xremote_app_buttons_free(XRemoteAppButtons* buttons) {
    xremote_app_assert_void(buttons);
    infrared_remote_free(buttons->remote);
    furi_string_free(buttons->custom_up);
    furi_string_free(buttons->custom_down);
    furi_string_free(buttons->custom_left);
    furi_string_free(buttons->custom_right);
    furi_string_free(buttons->custom_ok);
    furi_string_free(buttons->custom_up_hold);
    furi_string_free(buttons->custom_down_hold);
    furi_string_free(buttons->custom_left_hold);
    furi_string_free(buttons->custom_right_hold);
    furi_string_free(buttons->custom_ok_hold);
    free(buttons);
}

XRemoteAppButtons* xremote_app_buttons_alloc() {
    XRemoteAppButtons* buttons = malloc(sizeof(XRemoteAppButtons));
    buttons->remote = infrared_remote_alloc();
    buttons->app_ctx = NULL;

    /* Setup default buttons for custom layout */
    buttons->custom_up = furi_string_alloc_set_str(XREMOTE_COMMAND_UP);
    buttons->custom_down = furi_string_alloc_set_str(XREMOTE_COMMAND_DOWN);
    buttons->custom_left = furi_string_alloc_set_str(XREMOTE_COMMAND_LEFT);
    buttons->custom_right = furi_string_alloc_set_str(XREMOTE_COMMAND_RIGHT);
    buttons->custom_ok = furi_string_alloc_set_str(XREMOTE_COMMAND_OK);
    buttons->custom_up_hold = furi_string_alloc_set_str(XREMOTE_COMMAND_INPUT);
    buttons->custom_down_hold = furi_string_alloc_set_str(XREMOTE_COMMAND_SETUP);
    buttons->custom_left_hold = furi_string_alloc_set_str(XREMOTE_COMMAND_MENU);
    buttons->custom_right_hold = furi_string_alloc_set_str(XREMOTE_COMMAND_LIST);
    buttons->custom_ok_hold = furi_string_alloc_set_str(XREMOTE_COMMAND_POWER);

    return buttons;
}

XRemoteAppButtons* xremote_app_buttons_load(XRemoteAppContext* app_ctx) {
    /* Show file selection dialog (returns selected file path with app_ctx->file_path) */
    if(!xremote_app_browser_select_file(app_ctx, XREMOTE_APP_EXTENSION)) return NULL;
    XRemoteAppButtons* buttons = xremote_app_buttons_alloc();
    buttons->app_ctx = app_ctx;

    /* Load buttons from the selected path */
    if(!infrared_remote_load(buttons->remote, app_ctx->file_path)) {
        xremote_app_buttons_free(buttons);
        return NULL;
    }

    /* Load custom buttons from the selected path */
    xremote_app_extension_load(buttons, app_ctx->file_path);
    return buttons;
}

//////////////////////////////////////////////////////////////////////////////
// XRemote application settings
//////////////////////////////////////////////////////////////////////////////

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
        uint32_t value = xremote_app_get_orientation_index(settings->orientation);
        if(!flipper_format_write_uint32(ff, "orientation", &value, 1)) break;

        value = xremote_app_get_exit_index(settings->exit_behavior);
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
        settings->orientation = xremote_app_get_orientation(value);

        if(!flipper_format_read_uint32(ff, "appexit", &value, 1)) break;
        settings->exit_behavior = xremote_app_get_exit_behavior(value);

        if(!flipper_format_read_uint32(ff, "repeat", &value, 1)) break;
        settings->repeat_count = value;

        success = true;
    } while(false);

    furi_record_close(RECORD_STORAGE);
    furi_string_free(header);
    flipper_format_free(ff);

    return success;
}

//////////////////////////////////////////////////////////////////////////////
// XRemote gloal context shared between every child application
//////////////////////////////////////////////////////////////////////////////

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

bool xremote_app_browser_select_file(XRemoteAppContext* app_ctx, const char* extension) {
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, XREMOTE_APP_FOLDER);

    if(app_ctx->file_path == NULL) {
        app_ctx->file_path = furi_string_alloc();
        furi_string_set(app_ctx->file_path, XREMOTE_APP_FOLDER);
    }

    /* Open file browser (view and dialogs are managed by the browser itself) */
    DialogsFileBrowserOptions browser;
    dialog_file_browser_set_basic_options(&browser, extension, &I_IR_Icon_10x10);
    browser.base_path = XREMOTE_APP_FOLDER;
    FuriString* path = app_ctx->file_path;

    /* Show file selection dialog (returns selected file path with file_path) */
    bool status = dialog_file_browser_show(dialogs, path, path, &browser);

    /* Cleanup file loading context */
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    return status;
}

const char* xremote_app_context_get_exit_str(XRemoteAppContext* app_ctx) {
    XRemoteAppExit exit_behavior = app_ctx->app_settings->exit_behavior;
    return exit_behavior == XRemoteAppExitHold ? "Hold to exit" : "Press to exit";
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

//////////////////////////////////////////////////////////////////////////////
// XRemote application factory
//////////////////////////////////////////////////////////////////////////////

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

void xremote_app_view_alloc2(
    XRemoteApp* app,
    uint32_t view_id,
    XRemoteViewAllocator2 allocator,
    void* model_ctx) {
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);

    if(app->view_id == view_id && app->view_ctx != NULL) return;

    xremote_app_view_free(app);
    app->view_id = view_id;

    app->view_ctx = allocator(app->app_ctx, model_ctx);
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
    xremote_app_assert(app, NULL);

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
