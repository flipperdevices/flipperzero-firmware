#include "../flipbip.h"
#include "../helpers/flipbip_file.h"

#define FLIPBIP_SUBMENU_TEXT "** FlipBIP wallet " FLIPBIP_VERSION " **"

enum SubmenuIndex {
    SubmenuIndexScene1BTC = 10,
    SubmenuIndexScene1ETH,
    SubmenuIndexScene1DOGE,
    SubmenuIndexScene1ZEC,
    SubmenuIndexScene1New,
    SubmenuIndexScene1Import,
    SubmenuIndexSettings,
    SubmenuIndexNOP,
};

void flipbip_scene_menu_submenu_callback(void* context, uint32_t index) {
    FlipBip* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipbip_scene_menu_on_enter(void* context) {
    FlipBip* app = context;

    // FlipBIP header with version
    submenu_add_item(
        app->submenu,
        FLIPBIP_SUBMENU_TEXT,
        SubmenuIndexNOP,
        flipbip_scene_menu_submenu_callback,
        app);

    if(flipbip_has_file(FlipBipFileKey, NULL, false) &&
       flipbip_has_file(FlipBipFileDat, NULL, false)) {
        submenu_add_item(
            app->submenu,
            "View BTC wallet",
            SubmenuIndexScene1BTC,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "View ETH wallet",
            SubmenuIndexScene1ETH,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "View DOGE wallet",
            SubmenuIndexScene1DOGE,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "View ZEC (t-addr) wallet",
            SubmenuIndexScene1ZEC,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "Regenerate wallet",
            SubmenuIndexScene1New,
            flipbip_scene_menu_submenu_callback,
            app);
    } else {
        submenu_add_item(
            app->submenu,
            "Generate new wallet",
            SubmenuIndexScene1New,
            flipbip_scene_menu_submenu_callback,
            app);
    }
    submenu_add_item(
        app->submenu,
        app->mnemonic_menu_text,
        SubmenuIndexScene1Import,
        flipbip_scene_menu_submenu_callback,
        app);

    submenu_add_item(
        app->submenu, "Settings", SubmenuIndexSettings, flipbip_scene_menu_submenu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipBipSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdMenu);
}

bool flipbip_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipBip* app = context;
    //UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1BTC) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinBTC0;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1BTC);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1ETH) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinETH60;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1ETH);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1DOGE) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinDOGE3;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1DOGE);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1ZEC) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinZEC133;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1ZEC);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1New) {
            app->overwrite_saved_seed = 1;
            app->import_from_mnemonic = 0;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1New);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1Import) {
            app->import_from_mnemonic = 1;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1Import);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneSettings);
            return true;
        } else if(event.event == SubmenuIndexNOP) {
            return true;
        }
    }
    return false;
}

void flipbip_scene_menu_on_exit(void* context) {
    FlipBip* app = context;
    submenu_reset(app->submenu);
}