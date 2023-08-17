#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
typedef enum { NO_ARGS = 0, INPUT_ARGS, TOGGLE_ARGS } InputArgs;

typedef enum { FOCUS_CONSOLE_END = 0, FOCUS_CONSOLE_START, FOCUS_CONSOLE_TOGGLE } FocusConsole;

#define SHOW_STOPSCAN_TIP (true)
#define NO_TIP (false)

#define MAX_OPTIONS (12)
typedef struct {
    const char* item_string;
    const char* options_menu[MAX_OPTIONS];
    int num_options_menu;
    const char* actual_commands[MAX_OPTIONS];
    InputArgs needs_keyboard;
    FocusConsole focus_console;
    bool show_stopscan_tip;
} UART_TerminalItem;

// NUM_MENU_ITEMS defined in uart_terminal_app_i.h - if you add an entry here, increment it!
/* CBC: Looking for a way to best use TOGGLE_ARGS, how's this:
        ** If actual_commands[i] ends with space, display a keyboard to fill in the blank ***
*/
const UART_TerminalItem items[NUM_MENU_ITEMS] = {
    {"Console", {"View", "Clear"}, 2, {"", "cls"}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP},
    {"Bluetooth",
     {"On", "Off"},
     2,
     {"bluetooth on", "bluetooth off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Beacon",
     {"Status", "target-ssids", "APs", "RickRoll", "Random", "Infinite", "Off"},
     7,
     {"beacon",
      "beacon target-ssids",
      "beacon aps",
      "beacon rickroll",
      "beacon random ",
      "beacon infinite",
      "beacon off"},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Probe",
     {"Status", "Any", "target-ssids", "APs", "Off"},
     5,
     {"probe", "probe any", "probe target-ssids", "probe aps", "probe off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Fuzz",
     {"Status",
      "Off",
      "Overflow Beacon",
      "Overflow Request",
      "Overflow Response",
      "Malformed Beacon",
      "Malformed Request",
      "Malformed Response"},
     8,
     {"fuzz",
      "fuzz off",
      "fuzz beacon overflow",
      "fuzz req overflow",
      "fuzz resp overflow",
      "fuzz beacon malformed",
      "fuzz req malformed",
      "fuzz resp malformed"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Sniff",
     {"Status", "On", "Off"},
     3,
     {"sniff", "sniff on", "sniff off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"target-ssids",
     {"Add", "Remove", "List"},
     3,
     {"target-ssids add ", "target-ssids remove ", "target-ssids"},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Scan",
     {"Status", "On", "Off", "<ssid>"},
     4,
     {"scan", "scan on", "scan off", "scan "},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Hop",
     {"Status", "On", "Off", "Sequential", "Random", "Default", "Set "},
     7,
     {"hop", "hop on", "hop off", "hop sequential", "hop random", "hop default", "hop "},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"View",
     {"STA", "AP", "STA+AP"},
     3,
     {"view sta", "view ap", "view sta ap"},
     NO_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP},
    {"Select",
     {"STA", "AP"},
     2,
     {"select sta ", "select ap "},
     INPUT_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Selected",
     {"STA", "AP", "STA+AP"},
     3,
     {"selected sta", "selected ap", "selected"},
     NO_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP},
    {"Clear",
     {"STA", "AP", "ALL"},
     3,
     {"clear sta", "clear ap", "clear all"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Get",
     {"pkt expiry",
      "SSID rnd chars",
      "Attack millis",
      "SSID min len",
      "SSID max len",
      "default SSID count",
      "Channel",
      "MAC",
      "MAC Randomisation"},
     9,
     {"get expiry",
      "get scramble_words",
      "get attack_millis",
      "get ssid_len_min",
      "get ssid_len_max",
      "get default_ssid_count",
      "get channel",
      "get mac",
      "get mac_rand"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Set",
     {"pkt expiry",
      "SSID rnd chars",
      "Attack millis",
      "SSID min len",
      "SSID max len",
      "default SSID count",
      "Channel",
      "MAC",
      "MAC Randomisation"},
     9,
     {"set expiry ",
      "set scramble_words ",
      "set attack_millis ",
      "set ssid_len_min ",
      "set ssid_len_max ",
      "set default_ssid_count ",
      "set channel ",
      "set mac ",
      "set mac_rand "},
     INPUT_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Deauth",
     {"Status",
      "Set Delay",
      "Off",
      "Frame STA",
      "Device STA",
      "Spoof STA",
      "Frame APs",
      "Device APs",
      "Spoof APs",
      "Frame B'Cast",
      "Device B'Cast",
      "Spoof B'Cast"},
     12,
     {"deauth",
      "deauth ",
      "deauth off",
      "deauth frame sta",
      "deauth device sta",
      "deauth spoof sta",
      "deauth frame ap",
      "deauth device ap",
      "deauth spoof ap",
      "deauth frame broadcast",
      "deauth device broadcast",
      "deauth spoof broadcast"},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Mana",
     {"Status", "On", "Off", "Clear"},
     4,
     {"mana", "mana on", "mana off", "mana clear"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Mana Verbose",
     {"Status", "On", "Off"},
     3,
     {"mana verbose", "mana verbose on", "mana verbose off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Mana Loud",
     {"Status", "On", "Off"},
     3,
     {"mana loud", "mana loud on", "mana loud off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"selectedAP DOS",
     {"Status", "On", "Off"},
     3,
     {"ap-dos", "ap-dos on", "ap-dos off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"AP Clone Attack",
     {"Status", "Off", "OPN", "WEP", "WPA", "OPN+WEP", "OPN+WPA", "WEP+WPA", "OPN+WEP+WPA"},
     9,
     {"ap-clone",
      "ap-clone off",
      "ap-clone on open",
      "ap-clone on wep",
      "ap-clone on wpa",
      "ap-clone on open wep",
      "ap-clone on open wpa",
      "ap-clone on wep wpa",
      "ap-clone on open wep wpa"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP},
    {"Homing", {"802.11", "ANY"}, 2, {"stalk on", "stalk on"}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP},
    {"Help",
     {"Info <cmd>", "Get Started", "Commands", "About", "Help"},
     5,
     {"info ", "GET_STARTED", "commands", "gravity-version", "help"},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP},
};

char* strToken(char* cmdLine, char sep, int tokenNum) {
    int i;
    int tokenCount = 0;
    for(i = 0; i < (int)strlen(cmdLine) && tokenCount != tokenNum; ++i) {
        if(cmdLine[i] == sep) {
            ++tokenCount;
        }
    }
    if(cmdLine[i - 1] == sep || cmdLine[i - 1] == '\0') {
        /* Found the end of the token, now find the beginning */
        int j;
        for(j = (i - 2); j > 0 && cmdLine[j] != sep; --j) {
        }
        /* Token runs from index j to (i - 2) */
        char* retVal = malloc(sizeof(char) * (i - j));
        if(retVal == NULL) {
            printf("GRAVITY: Failed to malloc token\n");
            return NULL;
        }
        strncpy(retVal, cmdLine, (i - j - 1));
        retVal[i - j - 1] = '\0';
        return retVal;
    } else {
        /* No token */
        if(tokenNum == 1) {
            return cmdLine;
        } else {
            return NULL;
        }
    }
    return NULL;
}

/* Callback when an option is selected */
static void uart_terminal_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;

    furi_assert(index < NUM_MENU_ITEMS);
    const UART_TerminalItem* item = &items[index];

    dolphin_deed(DolphinDeedGpioUartBridge);

    const int selected_option_index = app->selected_option_index[index];
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
    char* cmd = strToken((char*)app->selected_tx_string, ' ', 1);
    if(!strcmp(cmd, "beacon")) {
        app->gravityCommand = GRAVITY_BEACON;
    } else if(!strcmp(cmd, "target-ssids")) {
        app->gravityCommand = GRAVITY_TARGET_SSIDS;
    } else if(!strcmp(cmd, "probe")) {
        app->gravityCommand = GRAVITY_PROBE;
    } else if(!strcmp(cmd, "fuzz")) {
        app->gravityCommand = GRAVITY_FUZZ;
    } else if(!strcmp(cmd, "sniff")) {
        app->gravityCommand = GRAVITY_SNIFF;
    } else if(!strcmp(cmd, "deauth")) {
        app->gravityCommand = GRAVITY_DEAUTH;
    } else if(!strcmp(cmd, "mana")) {
        app->gravityCommand = GRAVITY_MANA;
    } else if(!strcmp(cmd, "stalk")) {
        app->gravityCommand = GRAVITY_STALK;
    } else if(!strcmp(cmd, "ap-dos")) {
        app->gravityCommand = GRAVITY_AP_DOS;
    } else if(!strcmp(cmd, "ap-clone")) {
        app->gravityCommand = GRAVITY_AP_CLONE;
    } else if(!strcmp(cmd, "scan")) {
        app->gravityCommand = GRAVITY_SCAN;
    } else if(!strcmp(cmd, "hop")) {
        app->gravityCommand = GRAVITY_HOP;
    } else if(!strcmp(cmd, "set")) {
        app->gravityCommand = GRAVITY_SET;
    } else if(!strcmp(cmd, "get")) {
        app->gravityCommand = GRAVITY_GET;
    } else if(!strcmp(cmd, "view")) {
        app->gravityCommand = GRAVITY_VIEW;
    } else if(!strcmp(cmd, "select")) {
        app->gravityCommand = GRAVITY_SELECT;
    } else if(!strcmp(cmd, "selected")) {
        app->gravityCommand = GRAVITY_SELECTED;
    } else if(!strcmp(cmd, "clear")) {
        app->gravityCommand = GRAVITY_CLEAR;
    } else if(!strcmp(cmd, "handshake")) {
        app->gravityCommand = GRAVITY_HANDSHAKE;
    } else if(!strcmp(cmd, "commands")) {
        app->gravityCommand = GRAVITY_COMMANDS;
    } else if(!strcmp(cmd, "info")) {
        app->gravityCommand = GRAVITY_INFO;
    } else {
        app->gravityCommand = GRAVITY_NONE;
    }

    free(cmd);

    /* GRAVITY: For TOGGLE_ARGS display a keyboard if actual_command ends with ' ' */
    int cmdLen = strlen(app->selected_tx_string);
    bool needs_keyboard =
        ((item->needs_keyboard == INPUT_ARGS) ||
         (item->needs_keyboard == TOGGLE_ARGS && (app->selected_tx_string[cmdLen - 1] == ' ')));
    /* Initialise the serial console */
    uart_terminal_uart_tx((uint8_t*)("\n"), 1);

    if(needs_keyboard) {
        view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartKeyboard);
    } else {
        view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
    }
}

/* Callback when a selected option is changed (I Think) */
static void uart_terminal_scene_start_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    const UART_TerminalItem* menu_item = &items[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_start_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_start_var_list_enter_callback, app);

    VariableItem* item;
    for(int i = 0; i < NUM_MENU_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            items[i].item_string,
            items[i].num_options_menu,
            uart_terminal_scene_start_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, items[i].options_menu[app->selected_option_index[i]]);
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewVarItemList);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewTextInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_start_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
