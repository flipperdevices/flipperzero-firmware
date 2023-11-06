#include "../hex_viewer.h"

enum SubmenuIndex {
    SubmenuIndexScene1 = 10,
    SubmenuIndexScene2,
    // SubmenuIndexScene3,
    SubmenuIndexScene4,
    // SubmenuIndexScene5,
    // SubmenuIndexSettings,
};

void hex_viewer_scene_menu_submenu_callback(void* context, uint32_t index) {
    HexViewer* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void hex_viewer_scene_menu_on_enter(void* context) {
    HexViewer* app = context;

    submenu_set_header(app->submenu, "Select action");
    submenu_add_item(
        app->submenu,
        "Open file ...",
        SubmenuIndexScene4,
        hex_viewer_scene_menu_submenu_callback,
        app);
    // submenu_add_item(app->submenu, "Scene 2 (Inputs/Effects)", SubmenuIndexScene2, hex_viewer_scene_menu_submenu_callback, app);
    // submenu_add_item(app->submenu, "Scene 3 (Buttonmenu)", SubmenuIndexScene3, hex_viewer_scene_menu_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "Scroll to ...",
        SubmenuIndexScene1,
        hex_viewer_scene_menu_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Show info ...",
        SubmenuIndexScene2,
        hex_viewer_scene_menu_submenu_callback,
        app);
    // submenu_add_item(app->submenu, "Settings", SubmenuIndexSettings, hex_viewer_scene_menu_submenu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, HexViewerSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, HexViewerViewIdMenu);
}

bool hex_viewer_scene_menu_on_event(void* context, SceneManagerEvent event) {
    HexViewer* app = context;

    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        // scene_manager_stop(app->scene_manager);
        // view_dispatcher_stop(app->view_dispatcher);
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, HexViewerSceneMenu, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, HexViewerSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene2) {
            scene_manager_set_scene_state(
                app->scene_manager, HexViewerSceneMenu, SubmenuIndexScene2);
            scene_manager_next_scene(app->scene_manager, HexViewerSceneScene_2);
            return true;
        } else if(event.event == SubmenuIndexScene4) {
            scene_manager_set_scene_state(
                app->scene_manager, HexViewerSceneMenu, SubmenuIndexScene4);
            scene_manager_next_scene(app->scene_manager, HexViewerSceneScene_4);
            // } else if (event.event == SubmenuIndexSettings) {
            //     scene_manager_set_scene_state(
            //         app->scene_manager, HexViewerSceneMenu, SubmenuIndexSettings);
            //     scene_manager_next_scene(app->scene_manager, HexViewerSceneSettings);
            //     return true;
        }
    }

    return false;
}

void hex_viewer_scene_menu_on_exit(void* context) {
    HexViewer* app = context;
    submenu_reset(app->submenu);
}