#include "../wifi_marauder_app_i.h"

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
typedef enum {
    NO_ARGS = 0,
    INPUT_ARGS,
    TOGGLE_ARGS
} InputArgs;

typedef enum {
    FOCUS_CONSOLE_END = 0,
    FOCUS_CONSOLE_START,
    FOCUS_CONSOLE_TOGGLE
} FocusConsole;

typedef struct {
    const char* item_string;
    const char* options_menu[5];
    int num_options_menu;
    const char* actual_commands[5];
    InputArgs needs_keyboard;
    FocusConsole focus_console;
} WifiMarauderItem;

const WifiMarauderItem items[NUM_MENU_ITEMS] = {
    { "View Log from", {"start", "end"}, 2, {}, NO_ARGS, FOCUS_CONSOLE_TOGGLE },
    { "Scan", {""}, 1, {"scanap"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "List", {"ap", "ssid"}, 2, {"list -a", "list -s"}, NO_ARGS, FOCUS_CONSOLE_START },
    { "Select", {"ap", "ssid"}, 2, {"select -a", "select -s"}, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "Clear List", {"ap", "ssid"}, 2, {"clearlist -a", "clearlist -s"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "SSID", {"add random", "add name", "remove"}, 3, {"ssid -a -g", "ssid -a -n", "ssid -r"}, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "Attack", {"deauth", "probe", "rickroll"}, 3, {"attack -t deauth", "attack -t probe", "attack -t rickroll"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Beacon Spam", {"ap", "ssid", "random"}, 3, {"attack -t beacon -a", "attack -t beacon -l", "attack -t beacon -r"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Sniff", {"beacon", "deauth", "esp", "pmkid", "pwn"}, 5, {"sniffbeacon", "sniffdeauth", "sniffesp", "sniffpmkid", "sniffpwn"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Sniff PMKID on channel", {""}, 1, {"sniffpmkid -c"}, INPUT_ARGS, FOCUS_CONSOLE_END },
    { "Channel", {"get", "set"}, 2, {"channel", "channel -s"}, TOGGLE_ARGS, FOCUS_CONSOLE_END },
    { "Update", {""}, 1, {"update -w"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Reboot", {""}, 1, {"reboot"}, NO_ARGS, FOCUS_CONSOLE_END },
    { "Help", {""}, 1, {"help"}, NO_ARGS, FOCUS_CONSOLE_START },
};


static void wifi_marauder_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    if (app->selected_option_index[index] < items[index].num_options_menu) {
        app->selected_tx_string = items[index].actual_commands[app->selected_option_index[index]];
    }
    app->is_command = (1 <= index);
    app->is_custom_tx_string = false;
    app->selected_menu_index = index;
    app->focus_console_start = (items[index].focus_console == FOCUS_CONSOLE_TOGGLE) ? (app->selected_option_index[index] == 0) : items[index].focus_console;

    bool needs_keyboard = (items[index].needs_keyboard == TOGGLE_ARGS) ? (app->selected_option_index[index] == 1) : items[index].needs_keyboard;
    if (needs_keyboard) {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderEventStartKeyboard);
    } else {
        view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderEventStartConsole);
    }
}

static void wifi_marauder_scene_start_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    WifiMarauderApp* app = variable_item_get_context(item);
    furi_assert(app);

    const WifiMarauderItem* menu_item = &items[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

void wifi_marauder_scene_start_on_enter(void* context) {
    WifiMarauderApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, wifi_marauder_scene_start_var_list_enter_callback, app);

    VariableItem* item;
    for (int i = 0; i < NUM_MENU_ITEMS; ++i) {
        item = variable_item_list_add(var_item_list, items[i].item_string, items[i].num_options_menu, wifi_marauder_scene_start_var_list_change_callback, app);
        if (items[i].num_options_menu) {
            variable_item_set_current_value_index(item, app->selected_option_index[i]);
            variable_item_set_current_value_text(item, items[i].options_menu[app->selected_option_index[i]]);
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
    else
    {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }

    return consumed;
}

void wifi_marauder_scene_start_on_exit(void* context) {
    WifiMarauderApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
