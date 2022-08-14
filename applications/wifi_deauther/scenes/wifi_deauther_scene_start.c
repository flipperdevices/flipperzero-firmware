#include "../wifi_deauther_app_i.h"

#define NUM_MENU_ITEMS (24)

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
#define INPUT_ARGS (true)
#define NO_ARGS (false)
#define FOCUS_CONSOLE_START (true)
#define FOCUS_CONSOLE_END (false)
struct WifideautherItem {
    const char* item_string;
    bool needs_keyboard;
    bool focus_console_start;
};

const struct WifideautherItem MenuItems[NUM_MENU_ITEMS] = {
    {"View Log (start)", NO_ARGS, FOCUS_CONSOLE_START},
    {"View Log (end)", NO_ARGS, FOCUS_CONSOLE_END},
    {"help", NO_ARGS, FOCUS_CONSOLE_END},
    {"stop all", NO_ARGS, FOCUS_CONSOLE_END},
    {"scan aps", NO_ARGS, FOCUS_CONSOLE_END},
    {"scan stations", NO_ARGS, FOCUS_CONSOLE_END},
    {"select aps", INPUT_ARGS, FOCUS_CONSOLE_END},
    {"select stations", INPUT_ARGS, FOCUS_CONSOLE_END},
    {"select all", NO_ARGS, FOCUS_CONSOLE_END},
    {"show selected", NO_ARGS, FOCUS_CONSOLE_END},
    {"show all", NO_ARGS, FOCUS_CONSOLE_END},
    {"deselect all", NO_ARGS, FOCUS_CONSOLE_END},
    {"attack deauth", NO_ARGS, FOCUS_CONSOLE_END},
    {"attack beacon", NO_ARGS, FOCUS_CONSOLE_END},
    {"attack deauthall", NO_ARGS, FOCUS_CONSOLE_END},
    {"attack probe", NO_ARGS, FOCUS_CONSOLE_END},
    {"sysinfo", NO_ARGS, FOCUS_CONSOLE_END},
    {"chicken", NO_ARGS, FOCUS_CONSOLE_END},
    {"get settings", NO_ARGS, FOCUS_CONSOLE_END},
    {"set webinterface false", INPUT_ARGS, FOCUS_CONSOLE_END},
    {"set ssid: pwned", INPUT_ARGS, FOCUS_CONSOLE_END},
    {"set password: deauther", INPUT_ARGS, FOCUS_CONSOLE_END},
    {"reboot", NO_ARGS, FOCUS_CONSOLE_END},
    {"save settings", NO_ARGS, FOCUS_CONSOLE_END},
};

static void wifi_deauther_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    WifideautherApp* app = context;
    app->selected_tx_string = MenuItems[index].item_string;
    app->is_command = (2 <= index);
    app->is_custom_tx_string = false;
    app->selected_menu_index = index;
    app->focus_console_start = MenuItems[index].focus_console_start;
    if(MenuItems[index].needs_keyboard) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifideautherEventStartKeyboard);
    } else {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifideautherEventStartConsole);
    }
}

void wifi_deauther_scene_start_on_enter(void* context) {
    WifideautherApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, wifi_deauther_scene_start_var_list_enter_callback, app);

    // TODO: organize menu
    for(int i = 0; i < NUM_MENU_ITEMS; ++i) {
        variable_item_list_add(var_item_list, MenuItems[i].item_string, 0, NULL, NULL);
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, WifideautherSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, WifideautherAppViewVarItemList);
}

bool wifi_deauther_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    WifideautherApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == WifideautherEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, WifideautherSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, WifideautherAppViewTextInput);
        } else if(event.event == WifideautherEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, WifideautherSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, WifideautherAppViewConsoleOutput);
        }
        consumed = true;
    }

    return consumed;
}

void wifi_deauther_scene_start_on_exit(void* context) {
    WifideautherApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
