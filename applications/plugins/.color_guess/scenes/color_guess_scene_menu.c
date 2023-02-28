#include "../color_guess.h"

enum SubmenuIndex {
    SubmenuIndexPlay = 10,
    SubmenuIndexColorSet,
    SubmenuIndexSettings,
};

void color_guess_scene_menu_submenu_callback(void* context, uint32_t index) {
    ColorGuess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void color_guess_scene_menu_on_enter(void* context) {
    ColorGuess* app = context;

    submenu_add_item(
        app->submenu, "Play Game", SubmenuIndexPlay, color_guess_scene_menu_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "Set Color",
        SubmenuIndexColorSet,
        color_guess_scene_menu_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Settings",
        SubmenuIndexSettings,
        color_guess_scene_menu_submenu_callback,
        app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, ColorGuessSceneMenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, ColorGuessViewIdMenu);
}

bool color_guess_scene_menu_on_event(void* context, SceneManagerEvent event) {
    ColorGuess* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexColorSet) {
            scene_manager_set_scene_state(
                app->scene_manager, ColorGuessSceneMenu, SubmenuIndexColorSet);
            scene_manager_next_scene(app->scene_manager, ColorGuessSceneColorSet);
            return true;
        } else if(event.event == SubmenuIndexPlay) {
            scene_manager_set_scene_state(
                app->scene_manager, ColorGuessSceneMenu, SubmenuIndexPlay);
            scene_manager_next_scene(app->scene_manager, ColorGuessScenePlay);
            return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, ColorGuessSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, ColorGuessSceneSettings);
            return true;
        }
    }
    return false;
}

void color_guess_scene_menu_on_exit(void* context) {
    ColorGuess* app = context;
    submenu_reset(app->submenu);
}