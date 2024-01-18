#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"

/* main menu scene */

/*
Read -> reads a code sent by a digimon -> save UI
Read After Send -> Select a code -> Sends the code and then reads the response -> save UI
Send -> load UI -> send UI (either send after OK if first, or wait for code and then send if second)
USB Serial -> Activate serial mode
*/

/* 
Setup our scene widgets and create callback hooks 
*/
void fcom_menu_callback(void* context, uint32_t index);
void fcom_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "F-Com");
    submenu_add_item(app->submenu, "Read", MainMenuSelectionHCSR04, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "Saved", MainMenuSelectionHCSR04, fcom_menu_callback, app);
    /*
    submenu_add_item(
        app->submenu, "4-bar resistor", ResistorsMainMenuSelectionR4, resistors_menu_callback, app);
    submenu_add_item(
        app->submenu, "5-bar resistor", ResistorsMainMenuSelectionR5, resistors_menu_callback, app);
    submenu_add_item(
        app->submenu, "6-bar resistor", ResistorsMainMenuSelectionR6, resistors_menu_callback, app);
    */
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomSubmenuView);
}


/** main menu callback - sends custom events to the scene manager based on the selection */
void fcom_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case MainMenuSelectionHCSR04:
        scene_manager_handle_custom_event(
            app->scene_manager, MainMenuSceneSelectionEventHCSR04);
        break;
    /*
    case ResistorsMainMenuSelectionR4:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuSceneSelectionEventR4);
        break;
    case ResistorsMainMenuSelectionR5:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuSceneSelectionEventR5);
        break;
    case ResistorsMainMenuSelectionR6:
        scene_manager_handle_custom_event(
            app->scene_manager, ResistorsMainMenuSceneSelectionEventR6);
        break;
    */
    }
}

/** main menu event handler - switches scene based on the event */
bool fcom_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case MainMenuSceneSelectionEventHCSR04:
            // Reset state: app_init_resistor(app, R3);
            scene_manager_next_scene(app->scene_manager, FcomHCSR04Scene);
            consumed = true;
            break;
        /*
        case ResistorsMainMenuSceneSelectionEventR4:
            app_init_resistor(app, R4);
            scene_manager_next_scene(app->scene_manager, ResistorsEditScene);
            consumed = true;
            break;
        case ResistorsMainMenuSceneSelectionEventR5:
            app_init_resistor(app, R5);
            scene_manager_next_scene(app->scene_manager, ResistorsEditScene);
            consumed = true;
            break;
        case ResistorsMainMenuSceneSelectionEventR6:
            app_init_resistor(app, R6);
            scene_manager_next_scene(app->scene_manager, ResistorsEditScene);
            consumed = true;
            break;
        */
        }
        break;
    default: // eg. SceneManagerEventTypeBack, SceneManagerEventTypeTick
        consumed = false;
        break;
    }
    return consumed;
}

void fcom_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}
