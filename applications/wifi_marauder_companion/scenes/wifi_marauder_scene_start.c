#include "../wifi_marauder_app_i.h"

#define NUM_MENU_ITEMS (14)

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
    const char* actual_commands[5];
    bool needs_keyboard;
    bool focus_console_start;
};

const struct WifiMarauderItem items[NUM_MENU_ITEMS] = {
    { "View Log from", {"start", "end"}, 2, {}, NO_ARGS, FOCUS_CONSOLE_START },
    { "Scan", {""}, 1, {"scanap"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "List", {"ap", "ssid"}, 2, {"list -a", "list -s"}, NO_ARGS, FOCUS_CONSOLE_START },
    { "Select", {"ap", "ssid"}, 2, {"select -a", "select -s"}, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "Clear List", {"ap", "ssid"}, 2, {"clearlist -a", "clearlist -s"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "SSID", {"add random", "add name", "remove"}, 3, {"ssid -a -g", "ssid -a -n", "ssid -r"}, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "Attack", {"deauth", "probe", "rickroll"}, 3, {"attack -t deauth", "attack -t probe", "attack -t rickroll"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Beacon Spam", {"list", "random", "ap"}, 3, {"attack -t beacon -l", "attack -t beacon -r", "attack -t beacon -a"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Sniff", {"beacon", "deauth", "esp", "pmkid", "pwn"}, 5, {"sniffbeacon", "sniffdeauth", "sniffesp", "sniffpmkid", "sniffpwn"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Sniff PMKID on channel", {""}, 1, {"sniffpmkid -c"}, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "Channel", {"get", "set"}, 2, {"channel", "channel -s"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Update", {""}, 1, {"update -w"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Reboot", {""}, 1, {"reboot"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Help", {""}, 1, {"help"}, NO_ARGS, FOCUS_CONSOLE_START },
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
