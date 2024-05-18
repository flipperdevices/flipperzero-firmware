#include "../gba_cartridge_app.h"
#include <lib/toolbox/value_index.h>
#include <gui/elements.h>
#include <ctype.h>

enum SubmenuIndex {
    SubmenuIndexScene1,
};

void gba_cartridge_scene_menu_submenu_callback(void* context, uint32_t index) {
    GBACartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}
void gba_cartridge_scene_menu_on_enter(void* context) {
    GBACartridge* app = context;

    variable_item_list_add(app->submenu, "Cartridge Information", 1, NULL, NULL);
    // variable_item_list_add(app->submenu, "Dump ROM Cartridge", 1, NULL, NULL);
    // variable_item_list_add(app->submenu, "Dump RAM Cartridge", 1, NULL, NULL);
    // variable_item_list_add(app->submenu, "Write ROM", 1, NULL, NULL);
    // variable_item_list_add(app->submenu, "Write RAM", 1, NULL, NULL);

    variable_item_list_set_enter_callback(
        app->submenu, gba_cartridge_scene_menu_submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, GBACartridgeViewIdMenu);
}

bool gba_cartridge_scene_menu_on_event(void* context, SceneManagerEvent event) {
    GBACartridge* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, GBACartridgeSceneMenu, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, GBACartridgeSceneScene_1);
            return true;
        }
    }
    return false;
}

void gba_cartridge_scene_menu_on_exit(void* context) {
    GBACartridge* app = context;
    variable_item_list_set_selected_item(app->submenu, 0);
    variable_item_list_reset(app->submenu);
}