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
     {"Get",
      "Ch. 1",
      "Ch. 2",
      "Ch. 3",
      "Ch. 4",
      "Ch. 5",
      "Ch. 6",
      "Ch. 7",
      "Ch. 8",
      "Ch. 9",
      "Ch. 10",
      "Ch. 11",
      "Ch. 12",
      "Ch. 13",
      "Ch. 14"},
     15,
     {"get CHANNEL",
      "set CHANNEL 1",
      "set CHANNEL 2",
      "set CHANNEL 3",
      "set CHANNEL 4",
      "set CHANNEL 5",
      "set CHANNEL 6",
      "set CHANNEL 7",
      "set CHANNEL 8",
      "set CHANNEL 9",
      "set CHANNEL 10",
      "set CHANNEL 11",
      "set CHANNEL 12",
      "set CHANNEL 13",
      "set CHANNEL 14"},
     NO_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false},
    {"MAC", {"Get", "Set"}, 2, {"get MAC", "set"}, TOGGLE_ARGS, FOCUS_CONSOLE_START, NO_TIP, false},
    {"Attack Pkts",
     {"Get", "Set"},
     2,
     {"get ATTACK_PKTS", "set ATTACK_PKTS "},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false},
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
    {"Hop Mode",
     {"Get", "Sequential", "Random"},
     3,
     {"get HOP_MODE", "set HOP_MODE SEQUENTIAL", "set HOP_MODE RANDOM"},
     NO_ARGS,
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
     {"Get", "Set"},
     2,
     {"get BLE_PURGE_STRAT", "set BLE_PURGE_STRAT "},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false},
    {"BLE Purge Max RSSI",
     {"Get", "Set"},
     2,
     {"get BLE_PURGE_MAX_RSSI", "set BLE_PURGE_MAX_RSSI "},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false},
    {"BLE Purge Min Age",
     {"Get", "Set"},
     2,
     {"get BLE_PURGE_MIN_AGE", "set BLE_PURGE_MIN_AGE "},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false}};

static void displaySubmenu(UART_TerminalApp* app, UART_TerminalItem* item) {
    int newScene = -1;
    if(!strcmp(item->item_string, "Get")) {
        // Get Settings menu
        //newScene = UART_TerminalSceneSettingsGet;
    } else if(!strcmp(item->item_string, "Set")) {
        //newScene = UART_TerminalSceneSettingsSet;
    } else if(!strcmp(item->item_string, "Purge Strategy")) {
        newScene = UART_TerminalScenePurge;
    } else if(!strcmp(item->item_string, "MAC")) {
        newScene = UART_TerminalSceneSettingsMac;
    }
    if(newScene < 0) {
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
    UART_TerminalItem* item = NULL;
    const int selected_option_index = app->selected_option_index[index];

    furi_assert(index < NUM_SETTINGS_ITEMS);
    item = &settings[index];

    /* Are we displaying a submenu or executing something? */
    /* The MAC menu item only uses a submenu for setting */
    if(item->isSubMenu || (!strcmp(item->item_string, "MAC") &&
                           !strcmp(item->actual_commands[selected_option_index], "set"))) {
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
        bool needs_keyboard =
            ((item->needs_keyboard == INPUT_ARGS) ||
             (item->needs_keyboard == TOGGLE_ARGS &&
              (app->selected_tx_string[cmdLen - 1] == ' ')));
        /* Initialise the serial console */
        uart_terminal_uart_tx((uint8_t*)("\n"), 1);

        if(needs_keyboard) {
            view_dispatcher_send_custom_event(
                app->view_dispatcher, UART_TerminalEventStartKeyboard);
        } else {
            view_dispatcher_send_custom_event(
                app->view_dispatcher, UART_TerminalEventStartConsole);
        }
    }
}

/* Callback when a selected option is changed (I Think) */
static void uart_terminal_scene_settings_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    if(app->selected_menu_index >= NUM_SETTINGS_ITEMS) {
        app->selected_menu_index = 0;
    }

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
    VariableItem* item;

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
        /* When transitioning between views app->selected_option_index[i] may
           be referencing a different view's options menu, and may be out of
           bounds of mainmenu[i].options_menu[].
           If that is the case, use 0 instead */
        if(app->selected_option_index[i] >= settings[i].num_options_menu) {
            app->selected_option_index[i] = 0;
        }
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, settings[i].options_menu[app->selected_option_index[i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneSettings));

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
        app->selected_menu_index =
            variable_item_list_get_selected_item_index(app->settings_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_settings_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->settings_menu_list);
}
