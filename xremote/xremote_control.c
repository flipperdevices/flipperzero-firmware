/*!
 *  @file flipper-xremote/xremote_control.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Remote controller application menu and view factory.
 */

#include "xremote_control.h"
#include "xremote_edit.h"
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

static uint32_t xremote_control_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewIRSubmenu;
}

static void xremote_buttons_clear_callback(void* context) {
    xremote_app_assert_void(context);
    xremote_app_buttons_free((XRemoteAppButtons*)context);
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
    else if(index == XRemoteViewIRPlayback)
        xremote_app_view_alloc(app, index, xremote_player_view_alloc);
    else if(index == XRemoteViewIRCustomPage)
        xremote_app_view_alloc2(app, index, xremote_custom_view_alloc, app->context);
    else if(index == XRemoteViewIRCustomEditPage)
        xremote_edit_view_alloc(app, index, app->context);

    if(app->view_ctx != NULL) {
        if(index != XRemoteViewIRCustomEditPage) {
            xremote_app_view_set_previous_callback(app, xremote_control_view_exit_callback);
            xremote_app_set_view_context(app, app->context, NULL);
        }

        xremote_app_switch_to_view(app, index);
    }
}

XRemoteApp* xremote_control_alloc(XRemoteAppContext* app_ctx) {
    /* Open file browser and load buttons from selected file */
    XRemoteAppButtons* buttons = xremote_app_buttons_load(app_ctx);
    xremote_app_assert(buttons, NULL);

    /* Allocate remote controller app with submenu */
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    xremote_app_set_user_context(app, buttons, xremote_buttons_clear_callback);
    xremote_app_submenu_alloc(app, XRemoteViewIRSubmenu, xremote_control_submenu_exit_callback);

    xremote_app_submenu_add(
        app, "General", XRemoteViewIRGeneral, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Control", XRemoteViewIRControl, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Navigation", XRemoteViewIRNavigation, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Playback", XRemoteViewIRPlayback, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Custom", XRemoteViewIRCustomPage, xremote_control_submenu_callback);
    xremote_app_submenu_add(
        app, "Edit", XRemoteViewIRCustomEditPage, xremote_control_submenu_callback);

    return app;
}
