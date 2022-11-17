#include "../rpc_debug_app.h"

void rpc_debug_app_scene_test_app_error_on_enter(void* context) {
    RpcDebugApp* app = context;
    UNUSED(app);
}

bool rpc_debug_app_scene_test_app_error_on_event(void* context, SceneManagerEvent event) {
    RpcDebugApp* app = context;
    SceneManager* scene_manager = app->scene_manager;

    UNUSED(scene_manager);
    UNUSED(event);

    bool consumed = false;

    return consumed;
}

void rpc_debug_app_scene_test_app_error_on_exit(void* context) {
    RpcDebugApp* app = context;
    UNUSED(app);
    // submenu_reset(app->submenu);
}
