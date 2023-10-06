/*!
 *  @file flipper-xremote/xremote_learn.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Functionality to read and store input from new remote.
 */

#include "xremote_learn.h"
#include "views/xremote_learn_view.h"

static uint32_t xremote_learn_view_exit_callback(void* context) {
    UNUSED(context);
    return XRemoteViewSubmenu;
}

XRemoteApp* xremote_learn_alloc(XRemoteAppContext* app_ctx) {
    XRemoteApp* app = xremote_app_alloc(app_ctx);
    xremote_app_view_alloc(app, XRemoteViewLearn, xremote_learn_view_alloc);
    xremote_app_view_set_previous_callback(app, xremote_learn_view_exit_callback);
    return app;
}
