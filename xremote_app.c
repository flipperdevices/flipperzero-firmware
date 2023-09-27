/*!
 *  @file flipper-xremote/xremote_app.c
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Shared functionality and data types between the apps.
 */

#include "xremote_app.h"

XRemoteAppContext* xremote_app_context_alloc(void *arg)
{
    XRemoteAppContext* ctx = malloc(sizeof(XRemoteAppContext));
    ctx->gui = furi_record_open(RECORD_GUI);
    ctx->notifications = furi_record_open(RECORD_NOTIFICATION);
    ctx->view_dispatcher = view_dispatcher_alloc();
    ctx->arg = arg;

    view_dispatcher_enable_queue(ctx->view_dispatcher);
    view_dispatcher_attach_to_gui(ctx->view_dispatcher, ctx->gui, ViewDispatcherTypeFullscreen);
    return ctx;
}

void xremote_app_context_free(XRemoteAppContext* ctx)
{
    xremote_app_assert_void(ctx);
    notification_internal_message(ctx->notifications, &sequence_reset_blue);
    view_dispatcher_free(ctx->view_dispatcher);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(ctx);
}

void xremote_app_view_alloc(XRemoteApp *app, uint32_t view_id, XRemoteViewAllocator allocator)
{
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);

    if (app->view_id == view_id &&
        app->view_ctx != NULL) return;

    xremote_app_view_free(app);
    app->view_id = view_id;

    app->view_ctx = allocator(app->app_ctx->notifications);
    View* app_view = xremote_view_get_view(app->view_ctx);

    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_add_view(view_disp, app->view_id, app_view);
}

void xremote_app_view_free(XRemoteApp* app)
{
    xremote_app_assert_void(app);

    if (app->app_ctx != NULL && app->view_id != XRemoteViewNone)
    {
        ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
        view_dispatcher_remove_view(view_disp, app->view_id);
        app->view_id = XRemoteViewNone;
    }

    if (app->view_ctx != NULL)
    {
        xremote_view_free(app->view_ctx);
        app->view_ctx = NULL;
    }
}

bool xremote_app_has_view(XRemoteApp *app, uint32_t view_id)
{
    xremote_app_assert(app, false);
    return (app->view_id == view_id ||
            app->submenu_id == view_id);
}

void xremote_app_switch_to_view(XRemoteApp *app, uint32_t view_id)
{
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);
    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_switch_to_view(view_disp, view_id);
}

void xremote_app_switch_to_submenu(XRemoteApp *app)
{
    furi_assert(app);
    xremote_app_assert_void(app->app_ctx);
    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_switch_to_view(view_disp, app->submenu_id);
}

void xremote_app_submenu_add(XRemoteApp* app, const char *name, uint32_t index, SubmenuItemCallback callback)
{
    furi_assert(app);
    xremote_app_assert_void(app->submenu);
    submenu_add_item(app->submenu, name, index, callback, app);
}

void xremote_app_submenu_alloc(XRemoteApp* app, uint32_t index, ViewNavigationCallback prev_cb)
{
    furi_assert(app);
    app->submenu = submenu_alloc();
    app->submenu_id = index;

    View* view = submenu_get_view(app->submenu);
    view_set_orientation(view, ViewOrientationVertical);
    view_set_previous_callback(view, prev_cb);

    ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
    view_dispatcher_add_view(view_disp, app->submenu_id, view);
}

void xremote_app_submenu_free(XRemoteApp *app)
{
    xremote_app_assert_void(app);

    /* Remove submenu view from dispatcher */
    if (app->submenu_id != XRemoteViewNone && app->app_ctx != NULL)
    {
        ViewDispatcher* view_disp = app->app_ctx->view_dispatcher;
        view_dispatcher_remove_view(view_disp, app->submenu_id);
        app->submenu_id = XRemoteViewNone;
    }

    /* Free submenu */
    if (app->submenu != NULL)
    {
        submenu_free(app->submenu);
        app->submenu = NULL;
    }
}

void xremote_app_view_set_previous_callback(XRemoteApp* app, ViewNavigationCallback callback)
{
    furi_assert(app);
    xremote_app_assert_void(app->view_ctx);
    View* view = xremote_view_get_view(app->view_ctx);
    view_set_previous_callback(view, callback);
}

void xremote_app_set_view_context(XRemoteApp* app, void *context, XRemoteViewClearCallback on_clear)
{
    furi_assert(app);
    xremote_app_assert_void(app->view_ctx);
    xremote_view_set_context(app->view_ctx, context, on_clear);
}

void xremote_app_set_user_context(XRemoteApp* app, void *context, XRemoteAppClearCallback on_clear)
{
    furi_assert(app);
    app->on_clear = on_clear;
    app->context = context;
}

void xremote_app_user_context_free(XRemoteApp* app)
{
    furi_assert(app);
    xremote_app_assert_void(app->context);
    xremote_app_assert_void(app->on_clear);

    app->on_clear(app->context);
    app->on_clear = NULL;
    app->context = NULL;
}

XRemoteApp* xremote_app_alloc(XRemoteAppContext *ctx)
{
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

void xremote_app_free(XRemoteApp* app)
{
    xremote_app_assert_void(app);
    xremote_app_submenu_free(app);
    xremote_app_view_free(app);

    if (app->on_clear != NULL)
        app->on_clear(app->context);

    free(app);
}
