#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

UART_TerminalItem settings[NUM_SETTINGS_ITEMS] = {
  {"Synchronise",
  {"Load From ESP32", "Save To Flipper", "Load From Flipper"},
  3,
  {"sync", "save", "load"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Hop Mode",
  {"Get", "Sequential", "Random"},
  3,
  {"get HOP_MODE", "set HOP_MODE SEQUENTIAL", "set HOP_MODE RANDOM"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Hop",
  {"Status", "On", "Off", "Default", "Set "},
  5,
  {"hop", "hop on", "hop off", "hop default", "hop "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"SSID Len Min",
  {"Get", "Set"},
  2,
  {"get SSID_LEN_MIN", "set SSID_LEN_MIN "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"SSID Len Max",
  {"Get", "Set"},
  2,
  {"get SSID_LEN_MAX", "set SSID_LEN_MAX "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Default SSID Count",
  {"Get", "Set"},
  2,
  {"get DEFAULT_SSID_COUNT", "set DEFAULT_SSID_COUNT "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Channel",
  {"Get", "Ch. 1", "Ch. 2", "Ch. 3", "Ch. 4", "Ch. 5", "Ch. 6", "Ch. 7", "Ch. 8",\
        "Ch. 9", "Ch. 10", "Ch. 11", "Ch. 12", "Ch. 13", "Ch. 14"},
  15,
  {"get CHANNEL", "set CHANNEL 1", "set CHANNEL 2", "set CHANNEL 3", "set CHANNEL 4",\
        "set CHANNEL 5", "set CHANNEL 6", "set CHANNEL 7", "set CHANNEL 8", \
        "set CHANNEL 9", "set CHANNEL 10", "set CHANNEL 11", "set CHANNEL 12", \
        "set CHANNEL 13", "set CHANNEL 14"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"MAC",
  {""},
  1,
  {"mac"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  true},
  {"Attack Millis",
  {"Get", "Set"},
  2,
  {"get ATTACK_MILLIS", "set ATTACK_MILLIS "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Mac Randomisation",
  {"Get", "On", "Off"},
  3,
  {"get MAC_RAND", "set MAC_RAND ON", "set MAC_RAND_OFF"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Packet Expiry",
  {"Get", "Set"},
  2,
  {"get EXPIRY", "set EXPIRY "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Disable SSID Dictionary",
  {"Get", "Random Words", "Random Chars"},
  3,
  {"get SCRAMBLE_WORDS", "set SCRAMBLE_WORDS FALSE", "set SCRAMBLE_WORDS TRUE"},
  NO_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Purge Strategy", {"Set"}, 1, {""}, NO_ARGS, FOCUS_CONSOLE_START, NO_TIP, true},
  {"BLE Purge Strategy",
  {"Get"},
  1,
  {"get BLE_PURGE_STRAT"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"BLE Purge Max RSSI",
  {"Get"},
  1,
  {"get BLE_PURGE_MAX_RSSI"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"BLE Purge Min Age",
  {"Get"},
  1,
  {"get BLE_PURGE_MIN_AGE"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false}
};

static void displaySubmenu(UART_TerminalApp *app, UART_TerminalItem *item) {
    int newScene = -1;
    if (!strcmp(item->item_string, "Purge Strategy")) {
        newScene = UART_TerminalScenePurge;
    } else if (!strcmp(item->item_string, "MAC")) {
        newScene = UART_TerminalSceneSettingsMac;
    }
    if (newScene < 0) {
        return;
    }
    scene_manager_next_scene(app->scene_manager, newScene);
}

/* Callback when an option is selected */
static void uart_terminal_scene_settings_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem *item = NULL;
    const int selected_option_index = app->selected_menu_options[GRAVITY_MENU_SETTINGS][index];
    furi_assert(index < NUM_SETTINGS_ITEMS);
    app->selected_menu_items[GRAVITY_MENU_SETTINGS] = index;

    item = &settings[index];

    /* Are we displaying a submenu or executing something? */
    if (item->isSubMenu) {
        /* Display next scene */
        displaySubmenu(app, item);
    } else if (!strcmp(item->actual_commands[selected_option_index], "sync")) {
        do_sync(app);
    } else if (!strcmp(item->actual_commands[selected_option_index], "save")) {
        save_settings(app);
    } else if (!strcmp(item->actual_commands[selected_option_index], "load")) {
        //load_settings(app);
    } else {
        /* Update data model if necessary */
        switch (app->selected_menu_items[GRAVITY_MENU_SETTINGS]) {
            case SETTINGS_MENU_CHANNEL:
                /* Channel is selected - Update channel */
                app->channel = app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_CHANNEL];
                break;
            case SETTINGS_MENU_MAC_RAND:
                /* Set MAC randomisation true if "On" is selected. If "Off" (or otherwise) set it off */
                app->mac_rand = (app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_MAC_RAND] == OPTIONS_MAC_RAND_ON);
                break;
            case SETTINGS_MENU_HOP_MODE:
                if (app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_HOP_MODE] == OPTIONS_HOP_MODE_RANDOM) {
                    app->hopMode = HOP_MODE_RANDOM;
                } else { /* Default to sequential hopping */
                    app->hopMode = HOP_MODE_SEQUENTIAL;
                }
                break;
            case SETTINGS_MENU_DICT_DISABLE:
                app->dict_disabled = (app->selected_menu_options[GRAVITY_MENU_SETTINGS][SETTINGS_MENU_DICT_DISABLE] == OPTIONS_DICT_CHARS);
                break;
            default:
                break;
        }
        /* Run a command */
        dolphin_deed(DolphinDeedGpioUartBridge);
        furi_assert(selected_option_index < item->num_options_menu);
        app->selected_tx_string = item->actual_commands[selected_option_index];
        /* Don't clear screen if command is an empty string */
        app->is_command = (strlen(app->selected_tx_string) > 0);
        app->is_custom_tx_string = false;
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

    if (app->selected_menu_items[GRAVITY_MENU_SETTINGS] >= NUM_SETTINGS_ITEMS) {
        app->selected_menu_items[GRAVITY_MENU_SETTINGS] = 0;
    }

    const UART_TerminalItem* menu_item = &settings[app->selected_menu_items[GRAVITY_MENU_SETTINGS]];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_menu_options[GRAVITY_MENU_SETTINGS][app->selected_menu_items[GRAVITY_MENU_SETTINGS]] = item_index;
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
        variable_item_set_current_value_index(item, app->selected_menu_options[GRAVITY_MENU_SETTINGS][i]);
        variable_item_set_current_value_text(
            item, settings[i].options_menu[app->selected_menu_options[GRAVITY_MENU_SETTINGS][i]]);
    }
    variable_item_list_set_selected_item(var_item_list, app->selected_menu_items[GRAVITY_MENU_SETTINGS]);

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewSettingsMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        int nextScene = 0;
        if (event.event == UART_TerminalEventStartKeyboard) {
            nextScene = UART_TerminalAppViewTextInput;
        } else if (event.event == UART_TerminalEventStartConsole) {
            nextScene = UART_TerminalAppViewConsoleOutput;
        }
        scene_manager_next_scene(app->scene_manager, nextScene);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_items[GRAVITY_MENU_SETTINGS] = variable_item_list_get_selected_item_index(app->settings_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_settings_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->settings_menu_list);
}
