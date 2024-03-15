#include "../camera_suite.h"

enum SubmenuIndex {
    /** Camera. */
    SubmenuIndexSceneCamera = 10,
    /** Cam settings menu. */
    SubmenuIndexCamSettings,
    /** App settings menu. */
    SubmenuIndexAppSettings,
    /** Guide/how-to. */
    SubmenuIndexGuide,
};

void camera_suite_scene_menu_submenu_callback(void* context, uint32_t index) {
    CameraSuite* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void camera_suite_scene_menu_on_enter(void* context) {
    CameraSuite* app = context;

    submenu_add_item(
        app->submenu,
        "Stream Camera to Screen",
        SubmenuIndexSceneCamera,
        camera_suite_scene_menu_submenu_callback,
        app);

    submenu_add_item(
        app->submenu,
        "Camera Settings",
        SubmenuIndexCamSettings,
        camera_suite_scene_menu_submenu_callback,
        app);

    submenu_add_item(
        app->submenu,
        "Application Settings",
        SubmenuIndexAppSettings,
        camera_suite_scene_menu_submenu_callback,
        app);

    submenu_add_item(
        app->submenu,
        "Camera Suite Guide",
        SubmenuIndexGuide,
        camera_suite_scene_menu_submenu_callback,
        app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, CameraSuiteSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, CameraSuiteViewIdMenu);
}

bool camera_suite_scene_menu_on_event(void* context, SceneManagerEvent event) {
    CameraSuite* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        // Exit application.
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSceneCamera) {
            scene_manager_set_scene_state(
                app->scene_manager, CameraSuiteSceneMenu, SubmenuIndexSceneCamera);
            scene_manager_next_scene(app->scene_manager, CameraSuiteSceneCamera);
            return true;
        } else if(event.event == SubmenuIndexAppSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, CameraSuiteSceneMenu, SubmenuIndexAppSettings);
            scene_manager_next_scene(app->scene_manager, CameraSuiteSceneAppSettings);
            return true;
        } else if(event.event == SubmenuIndexCamSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, CameraSuiteSceneMenu, SubmenuIndexCamSettings);
            scene_manager_next_scene(app->scene_manager, CameraSuiteSceneCamSettings);
            return true;
        } else if(event.event == SubmenuIndexGuide) {
            scene_manager_set_scene_state(
                app->scene_manager, CameraSuiteSceneMenu, SubmenuIndexGuide);
            scene_manager_next_scene(app->scene_manager, CameraSuiteSceneGuide);
            return true;
        }
    }
    return false;
}

void camera_suite_scene_menu_on_exit(void* context) {
    CameraSuite* app = context;
    submenu_reset(app->submenu);
}