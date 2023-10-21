#include "../gb_cartridge_app.h"

enum SubmenuIndex {
    SubmenuIndexScene1 = 10,
    SubmenuIndexScene2,
    SubmenuIndexScene3,
    SubmenuIndexScene4,
    SubmenuIndexScene5,
    SubmenuIndexSettings,
};

void gb_cartridge_scene_menu_submenu_callback(void* context, uint32_t index) {
    GBCartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void gb_cartridge_scene_menu_on_enter(void* context) {
    GBCartridge* app = context;

    submenu_add_item(app->submenu, "Cartridge Information", SubmenuIndexScene1, gb_cartridge_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Dump ROM Cartridge", SubmenuIndexScene2, gb_cartridge_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Dump RAM Cartridge", SubmenuIndexScene3, gb_cartridge_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Write ROM", SubmenuIndexScene4, gb_cartridge_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Write RAM", SubmenuIndexSettings, gb_cartridge_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Settings", SubmenuIndexSettings, gb_cartridge_scene_menu_submenu_callback, app);

    submenu_set_selected_item(app->submenu, scene_manager_get_scene_state(app->scene_manager, GBCartridgeSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, GBCartridgeViewIdMenu);
}

bool gb_cartridge_scene_menu_on_event(void* context, SceneManagerEvent event) {
    GBCartridge* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_1);
            return true;
        } else if (event.event == SubmenuIndexScene2) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene2);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_2);
            return true;
        } else if (event.event == SubmenuIndexScene3) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene3);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_3);
        } else if (event.event == SubmenuIndexScene4) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene4);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_4);
        } else if (event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneSettings);
            return true;
        }
    }
    return false;
}

void gb_cartridge_scene_menu_on_exit(void* context) {
    GBCartridge* app = context;
    submenu_reset(app->submenu);
}