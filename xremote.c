/*!
 *  @file flipper-xremote/xremote.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Entrypoint and factory of the XRemote main app.
 */

#include "xremote.h"
#include "xremote_learn.h"
#include "xremote_control.h"
#include "xremote_settings.h"
#include "xremote_analyzer.h"

#include "views/xremote_about_view.h"
#include "views/xremote_learn_view.h"
#include "views/xremote_signal_view.h"

#define TAG "XRemote"

void xremote_get_version(char* version, size_t length) {
    snprintf(
        version,
        length,
        "%d.%d.%d",
        XREMOTE_VERSION_MAJOR,
        XREMOTE_VERSION_MINOR,
        XREMOTE_BUILD_NUMBER);
}

static uint32_t xremote_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewSubmenu;
}

static uint32_t xremote_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void xremote_child_clear_callback(void* context) {
    xremote_app_assert_void(context);
    xremote_app_free((XRemoteApp*)context);
}

static XRemoteApp* xremote_about_alloc(XRemoteAppContext* app_ctx) {
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    xremote_app_view_alloc(app, XRemoteViewAbout, xremote_about_view_alloc);
    xremote_app_view_set_previous_callback(app, xremote_view_exit_callback);
    return app;
}

void xremote_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    XRemoteApp* app = (XRemoteApp*)context;

    /* Cleanup previous app first */
    xremote_app_user_context_free(app);
    XRemoteApp* child = NULL;

    /* Allocate child app and view based on submenu selection */
    if(index == XRemoteViewLearn)
        child = xremote_learn_alloc(app->app_ctx);
    else if(index == XRemoteViewIRSubmenu)
        child = xremote_control_alloc(app->app_ctx);
    else if(index == XRemoteViewAnalyzer)
        child = xremote_analyzer_alloc(app->app_ctx);
    else if(index == XRemoteViewSettings)
        child = xremote_settings_alloc(app->app_ctx);
    else if(index == XRemoteViewAbout)
        child = xremote_about_alloc(app->app_ctx);

    if(child != NULL) {
        /* Switch to the view of newely allocated app */
        xremote_app_set_user_context(app, child, xremote_child_clear_callback);
        xremote_app_switch_to_view(child, index);
    }
}

int32_t xremote_main(void* p) {
    /* Allocate context and main application */
    XRemoteAppContext* context = xremote_app_context_alloc(p);
    XRemoteApp* app = xremote_app_alloc(context);

    /* Allocate and build the menu */
    xremote_app_submenu_alloc(app, XRemoteViewSubmenu, xremote_exit_callback);
    xremote_app_submenu_add(app, "Learn", XRemoteViewLearn, xremote_submenu_callback);
    xremote_app_submenu_add(app, "Saved", XRemoteViewIRSubmenu, xremote_submenu_callback);
    xremote_app_submenu_add(app, "Analyzer", XRemoteViewAnalyzer, xremote_submenu_callback);
    xremote_app_submenu_add(app, "Settings", XRemoteViewSettings, xremote_submenu_callback);
    xremote_app_submenu_add(app, "About", XRemoteViewAbout, xremote_submenu_callback);

    /* Switch to main menu by default and run disparcher*/
    xremote_app_switch_to_view(app, XRemoteViewSubmenu);
    view_dispatcher_run(app->app_ctx->view_dispatcher);

    /* Cleanup and exit */
    xremote_app_free(app);
    xremote_app_context_free(context);
    return 0;
}