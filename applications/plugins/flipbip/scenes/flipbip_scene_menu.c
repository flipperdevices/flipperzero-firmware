#include "../flipbip.h"

enum SubmenuIndex {
    SubmenuIndexScene1 = 10,
    // SubmenuIndexScene2,
    SubmenuIndexSettings,
};

void flipbip_scene_menu_submenu_callback(void* context, uint32_t index) {
    FlipBip* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipbip_scene_menu_on_enter(void* context) {
    FlipBip* app = context;

    if(app->bip44_coin == FlipBipCoinBTC0) { // BTC
        submenu_add_item(
            app->submenu,
            "Generate BTC wallet",
            SubmenuIndexScene1,
            flipbip_scene_menu_submenu_callback,
            app);
    }
    if(app->bip44_coin == FlipBipCoinETH60) { // ETH
        submenu_add_item(
            app->submenu,
            "Generate ETH wallet",
            SubmenuIndexScene1,
            flipbip_scene_menu_submenu_callback,
            app);
    }
    //submenu_add_item(app->submenu, "Scene 2", SubmenuIndexScene2, flipbip_scene_menu_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Settings", SubmenuIndexSettings, flipbip_scene_menu_submenu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipBipSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdMenu);
}

bool flipbip_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipBip* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
            // } else if (event.event == SubmenuIndexScene2) {
            //     scene_manager_set_scene_state(
            //         app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene2);
            //     scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_2);
            //     return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneSettings);
            return true;
        }
    }
    return false;
}

void flipbip_scene_menu_on_exit(void* context) {
    FlipBip* app = context;
    submenu_reset(app->submenu);
}