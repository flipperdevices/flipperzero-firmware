#include "../flipbip39.h"

enum SubmenuIndex {
    SubmenuIndexScene1 = 10,
    SubmenuIndexScene2,
    SubmenuIndexSettings,
};

void flipbip39_scene_menu_submenu_callback(void* context, uint32_t index) {
    FlipBip39* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipbip39_scene_menu_on_enter(void* context) {
    FlipBip39* app = context;

    submenu_add_item(app->submenu, "New BIP39 Mnemonic", SubmenuIndexScene1, flipbip39_scene_menu_submenu_callback, app);
    //submenu_add_item(app->submenu, "Scene 2", SubmenuIndexScene2, flipbip39_scene_menu_submenu_callback, app);
    submenu_add_item(app->submenu, "Settings", SubmenuIndexSettings, flipbip39_scene_menu_submenu_callback, app);

    submenu_set_selected_item(app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipBip39SceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBip39ViewIdMenu);
}

bool flipbip39_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipBip39* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBip39SceneMenu, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, FlipBip39SceneScene_1);
            return true;
        } else if (event.event == SubmenuIndexScene2) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBip39SceneMenu, SubmenuIndexScene2);
            scene_manager_next_scene(app->scene_manager, FlipBip39SceneScene_2);
            return true;
        } else if (event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBip39SceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, FlipBip39SceneSettings);
            return true;
        }
    }
    return false;
}

void flipbip39_scene_menu_on_exit(void* context) {
    FlipBip39* app = context;
    submenu_reset(app->submenu);
}