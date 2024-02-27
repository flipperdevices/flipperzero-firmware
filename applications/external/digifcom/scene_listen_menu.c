/*
A menu to select which listen mode the fcom should enter

-2 prong / vpet
-3 prong / penx
-xros mini
-color
*/

#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_listen_menu.h"

void fcom_listen_menu_callback(void* context, uint32_t index);
void fcom_listen_menu_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_listen_menu_scene_on_enter");
    App* app = context;
    submenu_reset(app->submenu);
    submenu_add_item(
        app->submenu, "V-Pet / 2Prong", ListenMenuSelection2Prong, fcom_listen_menu_callback, app);
    submenu_add_item(
        app->submenu, "PenX / 3Prong", ListenMenuSelection3Prong, fcom_listen_menu_callback, app);
    submenu_add_item(
        app->submenu, "Xros Mini", ListenMenuSelectionXrosMini, fcom_listen_menu_callback, app);
    submenu_add_item(
        app->submenu, "Color", ListenMenuSelectionColor, fcom_listen_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomMainMenuView);
}

/** main menu callback - sends custom events to the scene manager based on the selection */
void fcom_listen_menu_callback(void* context, uint32_t index) {
    FURI_LOG_I(TAG, "fcom_listen_menu_callback");
    App* app = context;
    switch(index) {
    case ListenMenuSelection2Prong:
        scene_manager_handle_custom_event(app->scene_manager, ListenMenuSceneSelectionEvent2Prong);
        break;
    case ListenMenuSelection3Prong:
        scene_manager_handle_custom_event(app->scene_manager, ListenMenuSceneSelectionEvent3Prong);
        break;
    case ListenMenuSelectionXrosMini:
        scene_manager_handle_custom_event(
            app->scene_manager, ListenMenuSceneSelectionEventXrosMini);
        break;
    case ListenMenuSelectionColor:
        scene_manager_handle_custom_event(app->scene_manager, ListenMenuSceneSelectionEventColor);
        break;
    }
}

/** main menu event handler - switches scene based on the event */
bool fcom_listen_menu_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_listen_menu_scene_on_event");
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case ListenMenuSceneSelectionEvent2Prong:
            strncpy(app->state->current_code, "V0\n", MAX_DIGIROM_LEN);
            furi_string_reset(app->state->r_code);
            furi_string_reset(app->state->s_code);
            scene_manager_next_scene(app->scene_manager, FcomReadCodeScene);
            consumed = true;
            break;
        case ListenMenuSceneSelectionEvent3Prong:
            strncpy(app->state->current_code, "X0\n", MAX_DIGIROM_LEN);
            furi_string_reset(app->state->r_code);
            furi_string_reset(app->state->s_code);
            scene_manager_next_scene(app->scene_manager, FcomReadCodeScene);
            consumed = true;
            break;
        case ListenMenuSceneSelectionEventXrosMini:
            strncpy(app->state->current_code, "Y0\n", MAX_DIGIROM_LEN);
            furi_string_reset(app->state->r_code);
            furi_string_reset(app->state->s_code);
            scene_manager_next_scene(app->scene_manager, FcomReadCodeScene);
            consumed = true;
            break;
        case ListenMenuSceneSelectionEventColor:
            strncpy(app->state->current_code, "C0\n", MAX_DIGIROM_LEN);
            furi_string_reset(app->state->r_code);
            furi_string_reset(app->state->s_code);
            scene_manager_next_scene(app->scene_manager, FcomReadCodeScene);
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

void fcom_listen_menu_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_listen_menu_scene_on_exit");
    App* app = context;
    submenu_reset(app->submenu);
}
