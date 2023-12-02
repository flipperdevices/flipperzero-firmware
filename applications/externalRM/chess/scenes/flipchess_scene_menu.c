#include "../flipchess.h"

enum SubmenuIndex {
    SubmenuIndexScene1New = 10,
    SubmenuIndexScene1Resume,
    SubmenuIndexScene1Import,
    SubmenuIndexSettings,
};

void flipchess_scene_menu_submenu_callback(void* context, uint32_t index) {
    FlipChess* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipchess_scene_menu_on_enter(void* context) {
    FlipChess* app = context;

    submenu_add_item(
        app->submenu,
        "New Game",
        SubmenuIndexScene1New,
        flipchess_scene_menu_submenu_callback,
        app);

    if(app->import_game == 1) {
        submenu_add_item(
            app->submenu,
            "Resume Game",
            SubmenuIndexScene1Resume,
            flipchess_scene_menu_submenu_callback,
            app);
    }

    // submenu_add_item(
    //     app->submenu,
    //     "Import Game",
    //     SubmenuIndexScene1Import,
    //     flipchess_scene_menu_submenu_callback,
    //     app);

    submenu_add_item(
        app->submenu, "Settings", SubmenuIndexSettings, flipchess_scene_menu_submenu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipChessSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdMenu);
}

bool flipchess_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipChess* app = context;
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
                app->scene_manager, FlipChessSceneMenu, SubmenuIndexScene1New);
            scene_manager_next_scene(app->scene_manager, FlipChessSceneScene_1);
            return true;
        }
        if(event.event == SubmenuIndexScene1Resume) {
            app->import_game = 1;
            scene_manager_set_scene_state(
                app->scene_manager, FlipChessSceneMenu, SubmenuIndexScene1Resume);
            scene_manager_next_scene(app->scene_manager, FlipChessSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1Import) {
            app->import_game = 1;
            app->input_state = FlipChessTextInputGame;
            text_input_set_header_text(app->text_input, "Enter board FEN");
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipChessViewIdTextInput);
            return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipChessSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, FlipChessSceneSettings);
            return true;
        }
    }
    return false;
}

void flipchess_scene_menu_on_exit(void* context) {
    FlipChess* app = context;
    submenu_reset(app->submenu);
}