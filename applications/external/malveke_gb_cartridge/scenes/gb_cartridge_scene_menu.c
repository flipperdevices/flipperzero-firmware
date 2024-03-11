#include "../gb_cartridge_app.h"
#include <lib/toolbox/value_index.h>
#include <gui/elements.h>
#include <ctype.h>

enum SubmenuIndex {
    SubmenuIndexScene1,
    SubmenuIndexScene2,
    SubmenuIndexScene3,
    // SubmenuIndexScene4,
    SubmenuIndexScene5,
    SubmenuIndexSettings,
};

const char* const gameboy_rom_option_text[2] = {
    "gb",
    "gbc",
};
const uint32_t gameboy_rom_option_value[2] = {
    GBCartridgeRomOptionGB,
    GBCartridgeRomOptionGBC,
};

void toUpperCase(char* str) {
    while(*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void gb_cartridge_scene_menu_submenu_callback(void* context, uint32_t index) {
    GBCartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}
// static char* rom_option_uppercase(uint32_t index) {
//     const char *gameboy_rom_option = gameboy_rom_option_text[index];
//     char *gameboy_rom_option_uppercase = (char*)malloc(strlen(gameboy_rom_option) + 1); // +1 para el carácter nulo

//     if (gameboy_rom_option_uppercase) {
//         strcpy(gameboy_rom_option_uppercase, gameboy_rom_option);
//         toUpperCase(gameboy_rom_option_uppercase);
//     } else {
//         // Manejo de error en caso de falta de memoria
//         return NULL;
//     }

//     return gameboy_rom_option_uppercase;
// }
// static void gameboy_cartridge_set_rom_option(VariableItem* item) {
//     GBCartridge* app = variable_item_get_context(item);
//     uint8_t index = variable_item_get_current_value_index(item);
//     variable_item_set_current_value_text(item, rom_option_uppercase(index));
//     app->gameboy_rom_option_selected_index = gameboy_rom_option_value[index];
//     app->gameboy_rom_option_selected_text = (char*)gameboy_rom_option_text[index];
// }

void gb_cartridge_scene_menu_on_enter(void* context) {
    GBCartridge* app = context;

    variable_item_list_add(app->submenu, "Cartridge Information", 1, NULL, NULL);
    variable_item_list_add(app->submenu, "Dump ROM Cartridge", 1, NULL, NULL);
    variable_item_list_add(app->submenu, "Dump RAM Cartridge", 1, NULL, NULL);

    //  TODO: Implements Write ROM
    // VariableItem* item = variable_item_list_add(
    //     app->submenu,
    //     "Write ROM",
    //     2,
    //     gameboy_cartridge_set_rom_option,
    //     app);
    // app->gameboy_rom_option_selected_index = value_index_uint32(app->gameboy_rom_option_selected_index, gameboy_rom_option_value, 2);
    // variable_item_set_current_value_index(item, app->gameboy_rom_option_selected_index);
    // variable_item_set_current_value_text(item, rom_option_uppercase(app->gameboy_rom_option_selected_index));

    variable_item_list_add(app->submenu, "Write RAM", 1, NULL, NULL);
    // variable_item_list_add(app->submenu, "Settings", 1, NULL, NULL);

    variable_item_list_set_enter_callback(
        app->submenu, gb_cartridge_scene_menu_submenu_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, GBCartridgeViewIdMenu);
}

bool gb_cartridge_scene_menu_on_event(void* context, SceneManagerEvent event) {
    GBCartridge* app = context;
    UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene1);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene2) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene2);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_2);
            return true;
        } else if(event.event == SubmenuIndexScene3) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene3);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_3);
            // } else if(event.event == SubmenuIndexScene4) {
            //     scene_manager_set_scene_state(
            //         app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene4);
            //     scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_4);
        } else if(event.event == SubmenuIndexScene5) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexScene5);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneScene_5);
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, GBCartridgeSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneSettings);
            return true;
        }
    }
    return false;
}

void gb_cartridge_scene_menu_on_exit(void* context) {
    GBCartridge* app = context;
    // submenu_reset(app->submenu);
    variable_item_list_set_selected_item(app->submenu, 0);
    variable_item_list_reset(app->submenu);
}