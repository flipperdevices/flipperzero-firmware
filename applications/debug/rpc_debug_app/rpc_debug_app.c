#include "rpc_debug_app.h"

static bool rpc_debug_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    RpcDebugApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool rpc_debug_app_back_event_callback(void* context) {
    furi_assert(context);
    RpcDebugApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void rpc_debug_app_tick_event_callback(void* context) {
    furi_assert(context);
    RpcDebugApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

static RpcDebugApp* rpc_debug_app_alloc() {
    RpcDebugApp* app = malloc(sizeof(RpcDebugApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->scene_manager = scene_manager_alloc(&rpc_debug_app_scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, rpc_debug_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, rpc_debug_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, rpc_debug_app_tick_event_callback, 100);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(app->view_dispatcher, RpcDebugAppViewSubmenu, submenu_get_view(app->submenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, RpcDebugAppViewSubmenu);
    return app;
}

static void rpc_debug_app_free(RpcDebugApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, RpcDebugAppViewSubmenu);

    free(app->submenu);
    free(app->scene_manager);
    free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    free(app);
}

int32_t rpc_debug_app(void* args) {
    UNUSED(args);

    RpcDebugApp* app = rpc_debug_app_alloc();

    scene_manager_next_scene(app->scene_manager, RpcDebugAppSceneStart);
    view_dispatcher_run(app->view_dispatcher);

    rpc_debug_app_free(app);
    return 0;
}
