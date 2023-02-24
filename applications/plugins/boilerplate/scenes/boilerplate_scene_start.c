#include "../boilerplate.h"

enum SubmenuIndex {
    SubmenuIndexScene1 = 10,
    SubmenuIndexScene2,
    SubmenuIndexSettings,
};

void boilerplate_scene_start_submenu_callback(void* context, uint32_t index) {
    Boilerplate* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void boilerplate_scene_start_on_enter(void* context) {
    Boilerplate* app = context;

    submenu_add_item(
        app->submenu, "Scene 1", SubmenuIndexScene1, boilerplate_scene_start_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Scene 2", SubmenuIndexScene2, boilerplate_scene_start_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "Settings",
        SubmenuIndexSettings,
        boilerplate_scene_start_submenu_callback,
        app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, BoilerplateSceneStart));
    view_dispatcher_switch_to_view(app->view_dispatcher, BoilerplateViewIdMenu);
}

bool boilerplate_scene_start_on_event(void* context, SceneManagerEvent event) {
    Boilerplate* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, BoilerplateSceneStart, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, BoilerplateSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene2) {
            scene_manager_set_scene_state(
                app->scene_manager, BoilerplateSceneStart, SubmenuIndexScene2);
            scene_manager_next_scene(app->scene_manager, BoilerplateSceneScene_2);
            return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, BoilerplateSceneStart, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, BoilerplateSceneSettings);
            return true;
        }
    }
    return false;
}

void boilerplate_scene_start_on_exit(void* context) {
    Boilerplate* app = context;
    submenu_reset(app->submenu);
}