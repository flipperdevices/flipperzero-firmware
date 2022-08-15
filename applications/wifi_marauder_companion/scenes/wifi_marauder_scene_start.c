#include "../wifi_marauder_app_i.h"

#define NUM_MENU_ITEMS (15)

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
#define INPUT_ARGS          (true)
#define NO_ARGS             (false)
#define FOCUS_CONSOLE_START (true)
#define FOCUS_CONSOLE_END   (false)
struct WifiMarauderItem {
    const char* item_string;
    const char* options_menu[5];
    int num_options_menu;
    bool needs_keyboard;
    bool focus_console_start;
};

const struct WifiMarauderItem items[NUM_MENU_ITEMS] = {
    { "View Log", {"(start)", "(end)"}, 2, NO_ARGS, FOCUS_CONSOLE_START },
    { "attack -t beacon", {"-l", "-r", "-a"}, 3, NO_ARGS, FOCUS_CONSOLE_END },
    { "attack -t", {"deauth", "probe", "rickroll"}, 3, NO_ARGS, FOCUS_CONSOLE_END },
    { "channel", {}, 0, NO_ARGS, FOCUS_CONSOLE_END },
    { "channel -s", {}, 0, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "clearlist", {"-a", "-s"}, 2, NO_ARGS, FOCUS_CONSOLE_END },
    { "help", {}, 0, NO_ARGS, FOCUS_CONSOLE_START },
    { "list", {"-a", "-s"}, 2, NO_ARGS, FOCUS_CONSOLE_START },
    { "reboot", {}, 0, NO_ARGS, FOCUS_CONSOLE_END },
    { "scanap", {}, 0, NO_ARGS, FOCUS_CONSOLE_END },
    { "select", {"-a", "-s"}, 2, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "sniff", {"beacon", "deauth", "esp", "pmkid", "pwn"}, 5, NO_ARGS, FOCUS_CONSOLE_END },
    { "sniffpmkid -c", {}, 0, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "ssid", {"-a -g", "-a -n", "-r"}, 3, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "update -w", {}, 0, NO_ARGS, FOCUS_CONSOLE_END },
};

typedef struct WifiMarauderItem WifiMarauderItem;

static void wifi_marauder_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    app->selected_tx_string = items[index].item_string;
    app->is_command = (1 <= index);
    app->is_custom_tx_string = false;
    app->selected_menu_index = index;
    app->focus_console_start = items[index].focus_console_start;
    if (items[index].needs_keyboard) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderEventStartKeyboard);
    } else {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderEventStartConsole);
    }
}

static void wifi_marauder_scene_start_var_list_change_callback(VariableItem* item) {
    WifiMarauderItem* menu_item = variable_item_get_context(item);
    furi_assert(menu_item);
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
}

void wifi_marauder_scene_start_on_enter(void* context) {
    WifiMarauderApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, wifi_marauder_scene_start_var_list_enter_callback, app);

    VariableItem* item;
    for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
        item = variable_item_list_add(var_item_list, items[i].item_string, items[i].num_options_menu, wifi_marauder_scene_start_var_list_change_callback, (WifiMarauderItem*)&items[i]);
        if (items[i].num_options_menu) {
            variable_item_set_current_value_index(item, 0);
            variable_item_set_current_value_text(item, items[i].options_menu[0]);
        }
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, WifiMarauderSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewVarItemList);
}

bool wifi_marauder_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    WifiMarauderApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if (event.event == WifiMarauderEventStartKeyboard) {
            scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, WifiMarauderAppViewTextInput);
        } else if (event.event == WifiMarauderEventStartConsole) {
            scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, WifiMarauderAppViewConsoleOutput);
        }
        consumed = true;
    }

    return consumed;
}

void wifi_marauder_scene_start_on_exit(void* context) {
    WifiMarauderApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
