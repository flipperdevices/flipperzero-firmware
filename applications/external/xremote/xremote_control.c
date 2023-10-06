/*!
 *  @file flipper-xremote/xremote_control.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Remote controller application menu and view factory.
 */

#include "xremote_control.h"
#include "infrared/infrared_remote.h"

#include "views/xremote_general_view.h"
#include "views/xremote_control_view.h"
#include "views/xremote_navigation_view.h"
#include "views/xremote_player_view.h"
#include "views/xremote_custom_view.h"

static uint32_t xremote_control_submenu_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewSubmenu;
}

static uint32_t xremote_navigation_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewIRSubmenu;
}

static void xremote_ir_clear_callback(void* context) {
    xremote_app_assert_void(context);
    infrared_remote_free((InfraredRemote*)context);
}

static void xremote_control_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    XRemoteApp* app = context;

    /* Allocate new view based on selection */
    if(index == XRemoteViewIRGeneral)
        xremote_app_view_alloc(app, index, xremote_general_view_alloc);
    else if(index == XRemoteViewIRControl)
        xremote_app_view_alloc(app, index, xremote_control_view_alloc);
    else if(index == XRemoteViewIRNavigation)
        xremote_app_view_alloc(app, index, xremote_navigation_view_alloc);
    else if(index == XRemoteViewIRPlayer)
        xremote_app_view_alloc(app, index, xremote_player_view_alloc);
    else if(index == XRemoteViewIRCustom)
        xremote_app_view_alloc(app, index, xremote_custom_view_alloc);

    if(app->view_ctx != NULL) {
        xremote_app_view_set_previous_callback(app, xremote_navigation_view_exit_callback);
        xremote_app_set_view_context(app, app->context, NULL);
        xremote_app_switch_to_view(app, index);
    }
}

static InfraredRemote* xremote_load_ir_buttons(XRemoteAppContext* app_ctx) {
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, XREMOTE_APP_FOLDER);

    /* Open file browser (view and dialogs are managed by the browser itself) */
    DialogsFileBrowserOptions browser;
    dialog_file_browser_set_basic_options(&browser, XREMOTE_APP_EXTENSION, &I_IR_Icon_10x10);
    browser.base_path = XREMOTE_APP_FOLDER;

    if(app_ctx->file_path == NULL) {
        app_ctx->file_path = furi_string_alloc();
        furi_string_set(app_ctx->file_path, XREMOTE_APP_FOLDER);
    }

    /* Show file selection dialog (returns selected file path with variable file_path) */
    if(!dialog_file_browser_show(dialogs, app_ctx->file_path, app_ctx->file_path, &browser)) {
        furi_record_close(RECORD_STORAGE);
        furi_record_close(RECORD_DIALOGS);
        return NULL;
    }

    /* Load buttons from the selected path */
    InfraredRemote* remote = infrared_remote_alloc();
    bool success = infrared_remote_load(remote, app_ctx->file_path);

    /* Cleanup file loading context */
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    if(!success) {
        infrared_remote_free(remote);
        return NULL;
    }

    return remote;
}

XRemoteApp* xremote_control_alloc(XRemoteAppContext* app_ctx) {
    /* Open file browser and load buttons from selected file */
    InfraredRemote* remote = xremote_load_ir_buttons(app_ctx);
    xremote_app_assert(remote, NULL);

    /* Allocate remote controller app with submenu */
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    xremote_app_submenu_alloc(app, XRemoteViewIRSubmenu, xremote_control_submenu_exit_callback);
    xremote_app_submenu_add(
        app, "General", XRemoteViewIRGeneral, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Control", XRemoteViewIRControl, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Navigation", XRemoteViewIRNavigation, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Playback", XRemoteViewIRPlayer, xremote_control_submenu_callback);
    xremote_app_submenu_add(app, "Custom", XRemoteViewIRCustom, xremote_control_submenu_callback);
    xremote_app_set_user_context(app, remote, xremote_ir_clear_callback);

    return app;
}
