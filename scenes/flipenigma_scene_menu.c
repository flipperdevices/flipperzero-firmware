#include "../flipenigma.h"

enum SubmenuIndex {
    SubmenuIndexScene1New = 10,
    SubmenuIndexScene1Resume,
    SubmenuIndexScene1Import,
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
        "New Game",
        SubmenuIndexScene1New,
        flipenigma_scene_menu_submenu_callback,
        app);

    if(app->import_game == 1) {
        submenu_add_item(
            app->submenu,
            "Resume Game",
            SubmenuIndexScene1Resume,
            flipenigma_scene_menu_submenu_callback,
            app);
    }

    // submenu_add_item(
    //     app->submenu,
    //     "Import Game",
    //     SubmenuIndexScene1Import,
    //     flipenigma_scene_menu_submenu_callback,
    //     app);

    submenu_add_item(
        app->submenu, "Settings", SubmenuIndexSettings, flipenigma_scene_menu_submenu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipEnigmaSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdMenu);
}

bool flipenigma_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipEnigma* app = context;
    //UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1New) {
            app->import_game = 0;
            scene_manager_set_scene_state(
                app->scene_manager, FlipEnigmaSceneMenu, SubmenuIndexScene1New);
            scene_manager_next_scene(app->scene_manager, FlipEnigmaSceneScene_1);
            return true;
        }
        if(event.event == SubmenuIndexScene1Resume) {
            app->import_game = 1;
            scene_manager_set_scene_state(
                app->scene_manager, FlipEnigmaSceneMenu, SubmenuIndexScene1Resume);
            scene_manager_next_scene(app->scene_manager, FlipEnigmaSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1Import) {
            app->import_game = 1;
            app->input_state = FlipEnigmaTextInputGame;
            text_input_set_header_text(app->text_input, "Enter board FEN");
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdTextInput);
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