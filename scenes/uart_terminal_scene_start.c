#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>


static void uart_terminal_scene_start_var_list_change_callback(VariableItem* item);

// NUM_MENU_ITEMS defined in uart_terminal_app_i.h - if you add an entry here, increment it!
/* CBC: Looking for a way to best use TOGGLE_ARGS, how's this:
        ** If actual_commands[i] ends with space, display a keyboard to fill in the blank ***
*/
UART_TerminalItem packets[NUM_PACKETS_ITEMS] = {
  {"Beacon",
  {"Status", "target-ssids", "APs", "RickRoll", "Random", "Infinite", "Off"},
  7,
  {"beacon", "beacon target-ssids", "beacon aps", "beacon rickroll", "beacon random ", "beacon infinite", "beacon off"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Probe",
  {"Status", "Any", "target-ssids", "APs", "Off"},
  5,
  {"probe", "probe any", "probe target-ssids", "probe aps", "probe off"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Fuzz",
  {"Status", "Off", "Overflow Beacon", "Overflow Request", "Overflow Response", "Malformed Beacon", "Malformed Request", "Malformed Response"},
  8,
  {"fuzz", "fuzz off", "fuzz beacon overflow", "fuzz req overflow", "fuzz resp overflow", "fuzz beacon malformed", "fuzz req malformed", "fuzz resp malformed"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Deauth",
  {"Status", "Set Delay", "Off", "Frame STA", "Device STA", "Spoof STA", "Frame APs", "Device APs", "Spoof APs", "Frame B'Cast", "Device B'Cast", "Spoof B'Cast"},
  12,
  {"deauth", "deauth ", "deauth off", "deauth frame sta", "deauth device sta", "deauth spoof sta", "deauth frame ap", "deauth device ap", "deauth spoof ap", "deauth frame broadcast", "deauth device broadcast", "deauth spoof broadcast"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false}
};
UART_TerminalItem attacks[NUM_ATTACK_ITEMS] = {
  {"Mana",
  {"Status", "On", "Off", "Clear"},
  4,
  {"mana", "mana on", "mana off", "mana clear"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Mana Verbose",
  {"Status", "On", "Off"},
  3,
  {"mana verbose", "mana verbose on", "mana verbose off"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Mana Loud",
  {"Status", "On", "Off"},
  3,
  {"mana loud", "mana loud on", "mana loud off"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"selectedAP DOS",
  {"Status", "On", "Off"},
  3,
  {"ap-dos", "ap-dos on", "ap-dos off"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"AP Clone Attack",
  {"Status", "Off", "OPN", "WEP", "WPA", "OPN+WEP", "OPN+WPA", "WEP+WPA", "OPN+WEP+WPA"},
  9,
  {"ap-clone", "ap-clone off", "ap-clone on open", "ap-clone on wep", "ap-clone on wpa", "ap-clone on open wep", "ap-clone on open wpa", "ap-clone on wep wpa", "ap-clone on open wep wpa"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Homing",
  {"On", "Off"},
  2,
  {"stalk on", "stalk off"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false}
};
UART_TerminalItem settings[NUM_SETTINGS_ITEMS] = {
  {"Hop",
  {"Status", "On", "Off", "Sequential", "Random", "Default", "Set "},
  7,
  {"hop", "hop on", "hop off", "hop sequential", "hop random", "hop default", "hop "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
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
UART_TerminalItem others[NUM_OTHER_ITEMS] = {
  {"Sniff",
  {"Status", "On", "Off"},
  3,
  {"sniff", "sniff on", "sniff off"},
  NO_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Help",
  {"Info <cmd>", "Get Started", "Commands", "About", "Help"},
  5,
  {"info ", "GET_STARTED", "commands", "gravity-version", "help"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false}
};
// const UART_TerminalItem items[NUM_MENU_ITEMS] = {
//     {"Console", {"View", "Clear"}, 2, {"", "cls"}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP},
//     {"Beacon",
//     {"Status", "target-ssids", "APs", "RickRoll", "Random", "Infinite", "Off"},
//     7,
//     {"beacon", "beacon target-ssids", "beacon aps", "beacon rickroll", "beacon random ", "beacon infinite", "beacon off"},
//     TOGGLE_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Probe",
//     {"Status", "Any", "target-ssids", "APs", "Off"},
//     5,
//     {"probe", "probe any", "probe target-ssids", "probe aps", "probe off"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Fuzz",
//     {"Status", "Off", "Overflow Beacon", "Overflow Request", "Overflow Response", "Malformed Beacon", "Malformed Request", "Malformed Response"},
//     8,
//     {"fuzz", "fuzz off", "fuzz beacon overflow", "fuzz req overflow", "fuzz resp overflow", "fuzz beacon malformed", "fuzz req malformed", "fuzz resp malformed"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Sniff",
//     {"Status", "On", "Off"},
//     3,
//     {"sniff", "sniff on", "sniff off"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"target-ssids",
//     {"Add", "Remove", "List"},
//     3,
//     {"target-ssids add ", "target-ssids remove ", "target-ssids"},
//     TOGGLE_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Scan",
//     {"Status", "<SSID>", "WiFi", "BT", "BLE", "BT Svcs", "Off"},
//     7,
//     {"scan", "scan ", "scan wifi", "scan bt", "scan ble", "scan bt services", "scan off"},
//     TOGGLE_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Hop",
//     {"Status", "On", "Off", "Sequential", "Random", "Default", "Set "},
//     7,
//     {"hop", "hop on", "hop off", "hop sequential", "hop random", "hop default", "hop "},
//     TOGGLE_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"View",
//     {"STA", "AP", "BT", "BT SVCS", "BT+AP+STA", "STA+AP"},
//     6,
//     {"view sta", "view ap", "view bt", "view bt services", "view ap sta bt", "view sta ap"},
//     NO_ARGS,
//     FOCUS_CONSOLE_START,
//     NO_TIP},
//     {"Select",
//     {"STA", "AP", "BT"},
//     3,
//     {"select sta ", "select ap ", "select bt "},
//     INPUT_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Selected",
//     {"STA", "AP", "BT", "AP+STA+BT"},
//     4,
//     {"selected sta", "selected ap", "selected bt", "selected"},
//     NO_ARGS,
//     FOCUS_CONSOLE_START,
//     NO_TIP},
//     {"Clear",
//     {"STA", "STA Sel.", "AP", "AP Sel.", "BT", "BT Sel.", "BT Svcs", "ALL"},
//     8,
//     {"clear sta", "clear sta selected", "clear ap", "clear ap selected", "clear bt", "clear bt selected", "clear bt services", "clear all"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Purge",
//     {"AP", "STA", "BT", "BLE"},
//     4,
//     {"purge ap", "purge sta", "purge bt", "purge ble"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Get",
//     {"pkt expiry", "SSID rnd chars", "Attack millis", "SSID min len", "SSID max len", "default SSID count", "Channel", "MAC", "MAC Randomisation", "Purge Strategy", "Purge Min Age", "Purge Max RSSI"},
//     12,
//     {"get expiry", "get scramble_words", "get attack_millis", "get ssid_len_min", "get ssid_len_max", "get default_ssid_count", "get channel", "get mac", "get mac_rand", "get ble_purge_strat", "get ble_purge_min_age", "get ble_purge_max_rssi"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Set",
//     {"pkt expiry", "SSID rnd chars", "Attack millis", "SSID min len", "SSID max len", "default SSID count", "Channel", "MAC", "MAC Randomisation", "Purge Strategy", "Purge Min Age", "Purge Max RSSI"},
//     12,
//     {"set expiry ", "set scramble_words ", "set attack_millis ", "set ssid_len_min ", "set ssid_len_max ", "set default_ssid_count ", "set channel ", "set mac ", "set mac_rand ", "set BLE_PURGE_STRAT ", "set BLE_PURGE_MIN_AGE ", "set BLE_PURGE_MAX_RSSI "},
//     INPUT_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Deauth",
//     {"Status", "Set Delay", "Off", "Frame STA", "Device STA", "Spoof STA", "Frame APs", "Device APs", "Spoof APs", "Frame B'Cast", "Device B'Cast", "Spoof B'Cast"},
//     12,
//     {"deauth", "deauth ", "deauth off", "deauth frame sta", "deauth device sta", "deauth spoof sta", "deauth frame ap", "deauth device ap", "deauth spoof ap", "deauth frame broadcast", "deauth device broadcast", "deauth spoof broadcast"},
//     TOGGLE_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Mana",
//     {"Status", "On", "Off", "Clear"},
//     4,
//     {"mana", "mana on", "mana off", "mana clear"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Mana Verbose",
//     {"Status", "On", "Off"},
//     3,
//     {"mana verbose", "mana verbose on", "mana verbose off"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Mana Loud",
//     {"Status", "On", "Off"},
//     3,
//     {"mana loud", "mana loud on", "mana loud off"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"selectedAP DOS",
//     {"Status", "On", "Off"},
//     3,
//     {"ap-dos", "ap-dos on", "ap-dos off"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"AP Clone Attack",
//     {"Status", "Off", "OPN", "WEP", "WPA", "OPN+WEP", "OPN+WPA", "WEP+WPA", "OPN+WEP+WPA"},
//     9,
//     {"ap-clone", "ap-clone off", "ap-clone on open", "ap-clone on wep", "ap-clone on wpa", "ap-clone on open wep", "ap-clone on open wpa", "ap-clone on wep wpa", "ap-clone on open wep wpa"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Homing",
//     {"On", "Off"},
//     2,
//     {"stalk on", "stalk off"},
//     NO_ARGS,
//     FOCUS_CONSOLE_END,
//     NO_TIP},
//     {"Help",
//     {"Info <cmd>", "Get Started", "Commands", "About", "Help"},
//     5,
//     {"info ", "GET_STARTED", "commands", "gravity-version", "help"},
//     TOGGLE_ARGS,
//     FOCUS_CONSOLE_START,
//     NO_TIP},
// };

static UART_TerminalItem *currentMenu = others;
static uint8_t currentMenuCount = NUM_TARGET_ITEMS;

void getCurrentMenu(UART_TerminalApp *app) {
    switch (app->currentMenu) {
        case GRAVITY_MENU_MAIN:
            //currentMenu = mainmenu;
            currentMenuCount = NUM_MAIN_ITEMS;
            break;
        case GRAVITY_MENU_TARGETS:
            currentMenu = others;
            currentMenuCount = NUM_TARGET_ITEMS;
            break;
        case GRAVITY_MENU_PACKETS:
            currentMenu = packets;
            currentMenuCount = NUM_PACKETS_ITEMS;
            break;
        case GRAVITY_MENU_ATTACKS:
            currentMenu = attacks;
            currentMenuCount = NUM_ATTACK_ITEMS;
            break;
        case GRAVITY_MENU_SETTINGS:
            currentMenu = settings;
            currentMenuCount = NUM_SETTINGS_ITEMS;
            break;
        case GRAVITY_MENU_FUZZ:
            //currentMenu = mainmenu;
            currentMenuCount = NUM_MAIN_ITEMS;
            break;
        case GRAVITY_MENU_DEAUTH:
            //currentMenu = mainmenu;
            currentMenuCount = NUM_MAIN_ITEMS;
            break;
        default:
            //currentMenu = mainmenu;
            currentMenuCount = NUM_MAIN_ITEMS;
            break;
    }
}

static void displayMenu(UART_TerminalApp *app, UART_TerminalItem *selectedMenu) {
    /* Clear current variable_item_list and build a new one using the menu
       referenced by selectedMenu */
    GravityMenu menuEnum = app->currentMenu;

    if (!strcmp(selectedMenu->item_string, "Targets")) {
        menuEnum = GRAVITY_MENU_TARGETS;
    } else if (!strcmp(selectedMenu->item_string, "Packets")) {
        menuEnum = GRAVITY_MENU_PACKETS;
    } else if (!strcmp(selectedMenu->item_string, "Attacks")) {
        menuEnum = GRAVITY_MENU_ATTACKS;
    } else if (!strcmp(selectedMenu->item_string, "Settings")) {
        menuEnum = GRAVITY_MENU_SETTINGS;
    } else if (!strcmp(selectedMenu->item_string, "Others")) {
        menuEnum = GRAVITY_MENU_OTHERS;
    } else if (!strcmp(selectedMenu->item_string, "Main")) {
        menuEnum = GRAVITY_MENU_MAIN;
    } else {
        // TODO: Display error
    }
    /* With the enum getCurrentMenu will do the rest */
    app->currentMenu = menuEnum;
    getCurrentMenu(app);

    /* Clear the current menu */
    VariableItemList* var_item_list = app->targets_menu_list;
    variable_item_list_reset(var_item_list);

    /* Add the new list */
    VariableItem *item = NULL;
    for (uint8_t i = 0; i < currentMenuCount; ++i) {
        item = variable_item_list_add(
            var_item_list,
            currentMenu[i].item_string,
            currentMenu[i].num_options_menu,
            uart_terminal_scene_start_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, currentMenu[i].options_menu[app->selected_option_index[i]]);
    }
    // variable_item_list_set_selected_item(
    //     var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneStart));
}

/* Callback when an option is selected */
static void uart_terminal_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem *item = NULL;
    const int selected_option_index = app->selected_option_index[index];

    getCurrentMenu(app);

    furi_assert(index < currentMenuCount);
    item = &currentMenu[index];

    /* Are we displaying a submenu or executing something? */
    if (item->isSubMenu) {
        displayMenu(app, item);
    } else {
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

        /* GRAVITY: Set app->gravityMode based on first word in command */

        //char *cmd = strsep(&origCmd, " ");
        /* GRAVITY: strsep is disabled by Flipper's SDK. RYO */

        char *cmd = strToken((char *)app->selected_tx_string, ' ', 1);
        if (!strcmp(cmd, "beacon")) {
            app->gravityCommand = GRAVITY_BEACON;
        } else if (!strcmp(cmd, "target-ssids")) {
            app->gravityCommand = GRAVITY_TARGET_SSIDS;
        } else if (!strcmp(cmd, "probe")) {
            app->gravityCommand = GRAVITY_PROBE;
        } else if (!strcmp(cmd, "fuzz")) {
            app->gravityCommand = GRAVITY_FUZZ;
        } else if (!strcmp(cmd, "sniff")) {
            app->gravityCommand = GRAVITY_SNIFF;
        } else if (!strcmp(cmd, "deauth")) {
            app->gravityCommand = GRAVITY_DEAUTH;
        } else if (!strcmp(cmd, "mana")) {
            app->gravityCommand = GRAVITY_MANA;
        } else if (!strcmp(cmd, "stalk")) {
            app->gravityCommand = GRAVITY_STALK;
        } else if (!strcmp(cmd, "ap-dos")) {
            app->gravityCommand = GRAVITY_AP_DOS;
        } else if (!strcmp(cmd, "ap-clone")) {
            app->gravityCommand = GRAVITY_AP_CLONE;
        } else if (!strcmp(cmd, "scan")) {
            app->gravityCommand = GRAVITY_SCAN;
        } else if (!strcmp(cmd, "hop")) {
            app->gravityCommand = GRAVITY_HOP;
        } else if (!strcmp(cmd, "set")) {
            app->gravityCommand = GRAVITY_SET;
        } else if (!strcmp(cmd, "get")) {
            app->gravityCommand = GRAVITY_GET;
        } else if (!strcmp(cmd, "view")) {
            app->gravityCommand = GRAVITY_VIEW;
        } else if (!strcmp(cmd, "select")) {
            app->gravityCommand = GRAVITY_SELECT;
        } else if (!strcmp(cmd, "selected")) {
            app->gravityCommand = GRAVITY_SELECTED;
        } else if (!strcmp(cmd, "clear")) {
            app->gravityCommand = GRAVITY_CLEAR;
        } else if (!strcmp(cmd, "handshake")) {
            app->gravityCommand = GRAVITY_HANDSHAKE;
        } else if (!strcmp(cmd, "commands")) {
            app->gravityCommand = GRAVITY_COMMANDS;
        } else if (!strcmp(cmd, "info")) {
            app->gravityCommand = GRAVITY_INFO;
        } else {
            app->gravityCommand = GRAVITY_NONE;
        }

        free(cmd);

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
static void uart_terminal_scene_start_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);
    getCurrentMenu(app);

    const UART_TerminalItem* menu_item = &currentMenu[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_start_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->targets_menu_list;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_start_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_TARGETS;
    // for(int i = 0; i < NUM_MAIN_ITEMS; ++i) {
    //     item = variable_item_list_add(
    //         var_item_list,
    //         mainmenu[i].item_string,
    //         mainmenu[i].num_options_menu,
    //         uart_terminal_scene_start_var_list_change_callback,
    //         app);
    //     variable_item_set_current_value_index(item, app->selected_option_index[i]);
    //     variable_item_set_current_value_text(
    //         item, mainmenu[i].options_menu[app->selected_option_index[i]]);
    // }
    UART_TerminalItem itemMain = { .item_string = "Targets" };
    displayMenu(app, &itemMain);

    // variable_item_list_set_selected_item(
    //     var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneStart));

    // view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewVarItemList);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;
    GravityMenu newMenu = GRAVITY_MENU_MAIN;
    UART_TerminalItem menuItem = { .item_string = "Main" };

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartKeyboard) {
            // scene_manager_set_scene_state(
                // app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewTextInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            // scene_manager_set_scene_state(
                // app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->targets_menu_list);
        consumed = true;
    } else if (event.type == SceneManagerEventTypeBack) {
        switch (app->currentMenu) {
            case GRAVITY_MENU_MAIN:
                consumed = false;
                break;
            case GRAVITY_MENU_FUZZ:
            case GRAVITY_MENU_DEAUTH:
                /* Parent of fuzz & deauth is Packets */
                newMenu = GRAVITY_MENU_PACKETS;
                menuItem.item_string = "Packets";
                consumed = true;
                break;
            case GRAVITY_MENU_TARGETS:
            case GRAVITY_MENU_PACKETS:
            case GRAVITY_MENU_ATTACKS:
            case GRAVITY_MENU_SETTINGS:
            case GRAVITY_MENU_OTHERS:
                /* Parent of these is all Main */
                /* They're already set as default */
                consumed = true;
                break;
        }
        /* Create a stub UART_TerminalItem* to direct displayMenu */
        app->currentMenu = newMenu;
        displayMenu(app, &menuItem);
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_start_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->targets_menu_list);
}
