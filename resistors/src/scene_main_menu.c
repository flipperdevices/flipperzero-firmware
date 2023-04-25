#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"

/* main menu scene */

/** main menu callback - sends custom events to the scene manager based on the selection */
void resistors_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case ResistorsMainMenuScene4BarResistor:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuScene4BarSelectionEvent);
        break;
    case ResistorsMainMenuScene5BarResistor:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuScene5BarSelectionEvent);
        break;
    }
}

/** main menu scene - resets the submenu, and gives it content, callbacks and selection enums */
void resistors_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Resistors");
    submenu_add_item(
        app->submenu,
        "4-bar resistor",
        ResistorsMainMenuScene4BarResistor,
        resistors_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "5-bar resistor",
        ResistorsMainMenuScene5BarResistor,
        resistors_menu_callback,
        app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ResistorsSubmenuView);
}

/** main menu event handler - switches scene based on the event */
bool resistors_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case ResistorsMainMenuScene4BarSelectionEvent:
            app->state->resistor_type = Resistor4Band;
            scene_manager_next_scene(app->scene_manager, ResistorsEditScene);
            consumed = true;
            break;
        case ResistorsMainMenuScene5BarSelectionEvent:
            app->state->resistor_type = Resistor5Band;
            scene_manager_next_scene(app->scene_manager, ResistorsEditScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void resistors_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}
