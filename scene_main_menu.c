#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"

/* main menu scene */

/*
Read -> Reads a code sent by a digimon (always waits) -> Save UI
Saved -> Code Picker -> Send screen (Either in "Press OK to Send" or "Waiting for Remote") -> Save UI
Add Manually -> Keyboard Entry -> Save UI
USB Serial -> Text Box scene showing serial log
*/

/* 
Setup our scene widgets and create callback hooks 
*/
void fcom_menu_callback(void* context, uint32_t index);
void fcom_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    //submenu_set_header(app->submenu, "F-Com");
    submenu_add_item(app->submenu, "Read", MainMenuSelectionRead, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "Saved", MainMenuSelectionSaved, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "Add Manually", MainMenuSelectionAdd, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "USB A-Com", MainMenuSelectionSerial, fcom_menu_callback, app);
    //"5V on GPIO" to toggle on/off, which just sets up the bridge
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomMainMenuView);
}

/** main menu callback - sends custom events to the scene manager based on the selection */
void fcom_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case MainMenuSelectionRead:
        scene_manager_handle_custom_event(
            app->scene_manager, MainMenuSceneSelectionEventRead);
        break;
    case MainMenuSelectionSaved:
        scene_manager_handle_custom_event(
            app->scene_manager, MainMenuSceneSelectionEventSaved);
        break;
    case MainMenuSelectionAdd:
        scene_manager_handle_custom_event(
            app->scene_manager, MainMenuSceneSelectionEventAdd);
        break;
    case MainMenuSelectionSerial:
        scene_manager_handle_custom_event(
            app->scene_manager, MainMenuSceneSelectionEventSerial);
        break;
    }
}

/** main menu event handler - switches scene based on the event */
bool fcom_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case MainMenuSceneSelectionEventRead:
            scene_manager_next_scene(app->scene_manager, FcomReadCodeScene);
            consumed = true;
            break;
        case MainMenuSceneSelectionEventSaved:
            scene_manager_next_scene(app->scene_manager, FcomCodeSelectScene);
            consumed = true;
            break;
        case MainMenuSceneSelectionEventAdd:
            scene_manager_next_scene(app->scene_manager, FcomAddCodeScene);
            consumed = true;
            break;
        case MainMenuSceneSelectionEventSerial:
            scene_manager_next_scene(app->scene_manager, FcomSerialScene);
            consumed = true;
            break;
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
