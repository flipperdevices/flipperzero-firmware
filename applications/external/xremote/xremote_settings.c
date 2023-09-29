/*!
 *  @file flipper-xremote/xremote_settings.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief XRemote settins functionality and menu.
 */

#include "xremote_settings.h"
#include "views/xremote_settings_view.h"

static uint32_t xremote_settings_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewSubmenu;
}

XRemoteApp* xremote_settings_alloc(XRemoteAppContext* app_ctx) {
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    xremote_app_view_alloc(app, XRemoteViewSettings, xremote_settings_view_alloc);
    xremote_app_view_set_previous_callback(app, xremote_settings_view_exit_callback);
    return app;
}
