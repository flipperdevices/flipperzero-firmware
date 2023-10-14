#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

UART_TerminalItem settings[NUM_SETTINGS_ITEMS] = {
  {"Hop",
  {"Status", "On", "Off", "Sequential", "Random", "Default", "Set "},
  7,
  {"hop", "hop on", "hop off", "hop sequential", "hop random", "hop default", "hop "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"SSID Len Min",
  {"Get", "Set"},
  2,
  {"GET SSID_LEN_MIN", "SET SSID_LEN_MIN "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"SSID Len Max",
  {"Get", "Set"},
  2,
  {"GET SSID_LEN_MAX", "SET SSID_LEN_MAX "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Default SSID Count",
  {"Get", "Set"},
  2,
  {"GET DEFAULT_SSID_COUNT", "SET DEFAULT_SSID_COUNT "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Channel",
  {"Get", "Ch. 1", "Ch. 2", "Ch. 3", "Ch. 4", "Ch. 5", "Ch. 6", "Ch. 7", "Ch. 8",\
        "Ch. 9", "Ch. 10", "Ch. 11", "Ch. 12", "Ch. 13", "Ch. 14"},
  15,
  {"GET CHANNEL", "SET CHANNEL 1", "SET CHANNEL 2", "SET CHANNEL 3", "SET CHANNEL 4",\
        "SET CHANNEL 5", "SET CHANNEL 6", "SET CHANNEL 7", "SET CHANNEL 8", \
        "SET CHANNEL 9", "SET CHANNEL 10", "SET CHANNEL 11", "SET CHANNEL 12", \
        "SET CHANNEL 13", "SET CHANNEL 14"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"MAC",
  {"Get", "Set"},
  2,
  {"GET MAC", "SET MAC "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Attack Pkts",
  {"Get", "Set"},
  2,
  {"GET ATTACK_PKTS", "SET ATTACK_PKTS "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Attack Millis",
  {"Get", "Set"},
  2,
  {"GET ATTACK_MILLIS", "SET ATTACK_MILLIS "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Mac Randomisation",
  {"Get", "On", "Off"},
  3,
  {"GET MAC_RAND", "SET MAC_RAND ON", "SET MAC_RAND_OFF"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Packet Expiry",
  {"Get", "Set"},
  2,
  {"GET EXPIRY", "SET EXPIRY "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Hop Mode",
  {"Get", "Sequential", "Random"},
  3,
  {"GET HOP_MODE", "SET HOP_MODE SEQUENTIAL", "SET HOP_MODE RANDOM"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Disable SSID Dictionary",
  {"Get", "Random Words", "Random Chars"},
  3,
  {"GET SCRAMBLE_WORDS", "SET SCRAMBLE_WORDS FALSE", "SET SCRAMBLE_WORDS TRUE"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"BLE Purge Strategy",
  {"Get", "Set"},
  2,
  {"GET BLE_PURGE_STRAT", "SET BLE_PURGE_STRAT "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"BLE Purge Max RSSI",
  {"Get", "Set"},
  2,
  {"GET BLE_PURGE_MAX_RSSI", "SET BLE_PURGE_MAX_RSSI "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"BLE Purge Min Age",
  {"Get", "Set"},
  2,
  {"GET BLE_PURGE_MIN_AGE", "SET BLE_PURGE_MIN_AGE "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false}
};
/*
  {"Get",
  {"pkt expiry", "SSID rnd chars", "Attack millis", "SSID min len", "SSID max len", "default SSID count", "Channel", "MAC", "MAC Randomisation", "Purge Strategy", "Purge Min Age", "Purge Max RSSI"},
  12,
  {"get expiry", "get scramble_words", "get attack_millis", "get ssid_len_min", "get ssid_len_max", "get default_ssid_count", "get channel", "get mac", "get mac_rand", "get ble_purge_strat", "get ble_purge_min_age", "get ble_purge_max_rssi"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Set",
  {"pkt expiry", "SSID rnd chars", "Attack millis", "SSID min len", "SSID max len", "default SSID count", "Channel", "MAC", "MAC Randomisation", "Purge Strategy", "Purge Min Age", "Purge Max RSSI"},
  12,
  {"set expiry ", "set scramble_words ", "set attack_millis ", "set ssid_len_min ", "set ssid_len_max ", "set default_ssid_count ", "set channel ", "set mac ", "set mac_rand ", "set BLE_PURGE_STRAT ", "set BLE_PURGE_MIN_AGE ", "set BLE_PURGE_MAX_RSSI "},
  INPUT_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false}
};
*/
static void displaySubmenu(UART_TerminalApp *app, UART_TerminalItem *item) {
    int newScene = -1;
    if (!strcmp(item->item_string, "Get")) {
        // Get Settings menu
        //newScene = UART_TerminalSceneSettingsGet;
    } else if (!strcmp(item->item_string, "Set")) {
        //newScene = UART_TerminalSceneSettingsSet;
    }
    if (newScene < 0) {
        return;
    }
    scene_manager_set_scene_state(
        app->scene_manager, UART_TerminalSceneSettings, app->selected_menu_index);
    scene_manager_next_scene(app->scene_manager, newScene);
}

/* Callback when an option is selected */
static void uart_terminal_scene_settings_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem *item = NULL;
    const int selected_option_index = app->selected_option_index[index];

    furi_assert(index < NUM_SETTINGS_ITEMS);
    item = &settings[index];

    /* Are we displaying a submenu or executing something? */
    if (item->isSubMenu) {
        /* Display next scene */
        displaySubmenu(app, item);
    } else {
        /* Run a command */
        dolphin_deed(DolphinDeedGpioUartBridge);
        furi_assert(selected_option_index < item->num_options_menu);
        app->selected_tx_string = item->actual_commands[selected_option_index];
        /* Don't clear screen if command is an empty string */
        app->is_command = (strlen(app->selected_tx_string) > 0);
        app->is_custom_tx_string = false;
        app->selected_menu_index = index;
        app->focus_console_start = (item->focus_console == FOCUS_CONSOLE_TOGGLE) ?
                                   (selected_option_index == 0) :
                                   item->focus_console;
        app->show_stopscan_tip = item->show_stopscan_tip;

        /* GRAVITY: For TOGGLE_ARGS display a keyboard if actual_command ends with ' ' */
        int cmdLen = strlen(app->selected_tx_string);
        bool needs_keyboard = ((item->needs_keyboard == INPUT_ARGS) ||
                                (item->needs_keyboard == TOGGLE_ARGS &&
                                (app->selected_tx_string[cmdLen-1] == ' ')));
        /* Initialise the serial console */
        uart_terminal_uart_tx((uint8_t*)("\n"), 1);

        if(needs_keyboard) {
            view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartKeyboard);
        } else {
            view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
        }
    }
}

/* Callback when a selected option is changed (I Think) */
static void uart_terminal_scene_settings_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    const UART_TerminalItem* menu_item = &settings[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_settings_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->settings_menu_list;
    VariableItem *item;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_settings_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_SETTINGS;
    for(int i = 0; i < NUM_SETTINGS_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            settings[i].item_string,
            settings[i].num_options_menu,
            uart_terminal_scene_settings_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, settings[i].options_menu[app->selected_option_index[i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneSettings));

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewSettingsMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneSettings, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewTextInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneSettings, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->settings_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_settings_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->settings_menu_list);
}
