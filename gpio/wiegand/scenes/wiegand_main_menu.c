#include "../wiegand.h"

/*
Triggers a custom event that is handled in the main menu on_scene handler.
@param context Pointer to the application context.
@param index Index of the selected menu item to map to custom event.
*/
void wiegand_menu_callback(void* context, uint32_t index) {
    App* app = context;
    WiegandMainMenuEvent event = WiegandMainMenuUnknownEvent;
    switch(index) {
    case WiegandMainMenuInstructions:
        event = WiegandMainMenuInstructionsEvent;
        break;
    case WiegandMainMenuRead:
        event = WiegandMainMenuReadEvent;
        break;
    case WiegandMainMenuLoad:
        event = WiegandMainMenuLoadEvent;
        break;
    }

    if(event != WiegandMainMenuUnknownEvent) {
        scene_manager_handle_custom_event(app->scene_manager, event);
    }
}

/*
Displays the main menu.
@param context Pointer to the application context. 
*/
void wiegand_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Wiegand");
    submenu_add_item(
        app->submenu, "Instructions", WiegandMainMenuInstructions, wiegand_menu_callback, app);
    submenu_add_item(app->submenu, "Read", WiegandMainMenuRead, wiegand_menu_callback, app);
    submenu_add_item(app->submenu, "Load", WiegandMainMenuLoad, wiegand_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandSubmenuView);
}

bool wiegand_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case WiegandMainMenuInstructionsEvent:
            scene_manager_next_scene(app->scene_manager, WiegandInstructionsScene);
            consumed = true;
            break;
        case WiegandMainMenuReadEvent:
            scene_manager_next_scene(app->scene_manager, WiegandReadScene);
            consumed = true;
            break;
        case WiegandMainMenuLoadEvent:
            scene_manager_next_scene(app->scene_manager, WiegandLoadScene);
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
    }
    return consumed;
}
