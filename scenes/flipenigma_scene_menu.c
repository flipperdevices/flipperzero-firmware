#include "../flipenigma.h"

enum SubmenuIndex {
    SubmenuIndexTextInput = 10,
    SubmenuIndexSettings,
};

void flipenigma_scene_menu_submenu_callback(void* context, uint32_t index) {
    FlipEnigma* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipenigma_scene_menu_on_enter(void* context) {
    FlipEnigma* app = context;

    submenu_add_item(
        app->submenu,
        "Message Entry",
        SubmenuIndexTextInput,
        flipenigma_scene_menu_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Rotor Settings",
        SubmenuIndexSettings,
        flipenigma_scene_menu_submenu_callback,
        app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipEnigmaSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdMenu);
}

bool flipenigma_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipEnigma* app = context;
    //UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        // exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexTextInput) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipEnigmaSceneMenu, SubmenuIndexTextInput);
            // set input state to "Active" to get correct scene behavior
            app->input_state = FlipEnigmaTextInputActive;
            scene_manager_next_scene(app->scene_manager, FlipEnigmaSceneStartscreen);
            return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipEnigmaSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, FlipEnigmaSceneSettings);
            return true;
        }
    }
    return false;
}

void flipenigma_scene_menu_on_exit(void* context) {
    FlipEnigma* app = context;
    submenu_reset(app->submenu);
}