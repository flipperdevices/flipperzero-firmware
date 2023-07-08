#include "../scope_app_i.h"

typedef enum {
    SubmenuIndexScopeRun,
    SubmenuIndexScopeSetup,
    SubmenuIndexScopeAbout,
} SubmenuIndex;

void scope_scene_start_submenu_callback(void* context, uint32_t index) {
    ScopeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void scope_scene_start_on_enter(void* context) {
    UNUSED(context);
    ScopeApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu, "Run", SubmenuIndexScopeRun, scope_scene_start_submenu_callback, app);

    submenu_add_item(
        submenu, "Setup", SubmenuIndexScopeSetup, scope_scene_start_submenu_callback, app);

    submenu_add_item(
        submenu, "About", SubmenuIndexScopeAbout, scope_scene_start_submenu_callback, app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, ScopeSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, ScopeViewSubmenu);
}

bool scope_scene_start_on_event(void* context, SceneManagerEvent event) {
    ScopeApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScopeAbout) {
            scene_manager_next_scene(app->scene_manager, ScopeSceneAbout);
            consumed = true;
        } else if(event.event == SubmenuIndexScopeRun) {
            scene_manager_next_scene(app->scene_manager, ScopeSceneRun);
            consumed = true;
        } else if(event.event == SubmenuIndexScopeSetup) {
            scene_manager_next_scene(app->scene_manager, ScopeSceneSetup);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, ScopeSceneStart, event.event);
    }

    return consumed;
}

void scope_scene_start_on_exit(void* context) {
    ScopeApp* app = context;
    submenu_reset(app->submenu);
}
