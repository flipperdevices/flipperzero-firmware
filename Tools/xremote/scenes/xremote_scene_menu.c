#include "../xremote.h"

enum SubmenuIndex {
    SubmenuIndexCreate = 10,
    SubmenuIndexLoad,
    SubmenuIndexSettings,
    SubmenuIndexInfoscreen,
};

void xremote_scene_menu_submenu_callback(void* context, uint32_t index) {
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void xremote_scene_menu_on_enter(void* context) {
    XRemote* app = context;

    submenu_add_item(app->submenu, "New Command Chain", SubmenuIndexCreate, xremote_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Run Saved Command", SubmenuIndexLoad, xremote_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Settings", SubmenuIndexSettings, xremote_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "App Info", SubmenuIndexInfoscreen, xremote_scene_menu_submenu_callback, app);

    submenu_set_selected_item(app->submenu, scene_manager_get_scene_state(app->scene_manager, XRemoteSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdMenu);
}

bool xremote_scene_menu_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexCreate) {
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneMenu, SubmenuIndexCreate);
            scene_manager_next_scene(app->scene_manager, XRemoteSceneCreate);
            return true;
        } else if (event.event == SubmenuIndexLoad) {
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneMenu, SubmenuIndexLoad);
            scene_manager_next_scene(app->scene_manager, XRemoteSceneXrList);
            return true;
        } else if (event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, XRemoteSceneSettings);
            return true;
        } else if (event.event == SubmenuIndexInfoscreen) {
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneMenu, SubmenuIndexInfoscreen);
            scene_manager_next_scene(app->scene_manager, XRemoteSceneInfoscreen);
            return true;
        }
    }
    return false;
}

void xremote_scene_menu_on_exit(void* context) {
    XRemote* app = context;
    submenu_reset(app->submenu);
}