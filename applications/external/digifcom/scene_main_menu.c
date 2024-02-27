/*
List of menu options


Listen -> Reads a code sent by 2 other devices we listen to -> Save UI
Saved -> Code Picker -> Send screen (Either in "Press OK to Send" or "Waiting for Remote") -> Save UI
Add Manually -> Keyboard Entry -> Save UI
USB Serial -> Text Box scene showing serial log
*/

#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_main_menu.h"

/* 
Setup our scene widgets and create callback hooks 
*/
void fcom_menu_callback(void* context, uint32_t index);
void fcom_main_menu_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_main_menu_scene_on_enter");
    App* app = context;
    submenu_reset(app->submenu);
    submenu_add_item(app->submenu, "Listen", MainMenuSelectionListen, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "Saved", MainMenuSelectionSaved, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "Add Manually", MainMenuSelectionAdd, fcom_menu_callback, app);
    submenu_add_item(app->submenu, "USB A-Com", MainMenuSelectionSerial, fcom_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomMainMenuView);
}

/** main menu callback - sends custom events to the scene manager based on the selection */
void fcom_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case MainMenuSelectionListen:
        scene_manager_handle_custom_event(app->scene_manager, MainMenuSceneSelectionEventListen);
        break;
    case MainMenuSelectionSaved:
        scene_manager_handle_custom_event(app->scene_manager, MainMenuSceneSelectionEventSaved);
        break;
    case MainMenuSelectionAdd:
        scene_manager_handle_custom_event(app->scene_manager, MainMenuSceneSelectionEventAdd);
        break;
    case MainMenuSelectionSerial:
        scene_manager_handle_custom_event(app->scene_manager, MainMenuSceneSelectionEventSerial);
        break;
    }
}

/** main menu event handler - switches scene based on the event */
bool fcom_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_main_menu_scene_on_event");
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case MainMenuSceneSelectionEventListen:
            scene_manager_next_scene(app->scene_manager, FcomListenMenuScene);
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
    FURI_LOG_I(TAG, "fcom_main_menu_scene_on_exit");
    App* app = context;
    submenu_reset(app->submenu);
}
