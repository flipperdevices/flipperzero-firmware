#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
typedef enum { NO_ARGS = 0, INPUT_ARGS, TOGGLE_ARGS } InputArgs;

typedef enum { FOCUS_CONSOLE_END = 0, FOCUS_CONSOLE_START, FOCUS_CONSOLE_TOGGLE } FocusConsole;

#define SHOW_STOPSCAN_TIP (true)
#define NO_TIP (false)

#define MAX_OPTIONS (9)
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
    {"Beacon",
    {"Status", "RickRoll", "Random", "Infinite", "target-ssids", "Off"},
    6,
    {"beacon", "beacon rickroll", "beacon random ", "beacon infinite", "beacon user", "beacon off"},
    TOGGLE_ARGS,
    FOCUS_CONSOLE_END,
    NO_TIP},
    {"Probe",
    {"Status", "Any", "target-ssids", "Off"},
    4,
    {"probe", "probe any", "probe ssids", "probe off"},
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
    {"Status", "On", "Off"},
    3,
    {"scan", "scan on", "scan off"},
    NO_ARGS,
    FOCUS_CONSOLE_END,
    NO_TIP},
    {"Hop",
    {"Status", "On", "Off", "Default", "Set "},
    5,
    {"hop", "hop on", "hop off", "hop default", "hop "},
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
    {"Clear",
    {"STA", "AP"},
    2,
    {"clear sta", "clear ap"},
    NO_ARGS,
    FOCUS_CONSOLE_END,
    NO_TIP},
    {"Get",
    {"SSID_LEN_MIN", "SSID_LEN_MAX", "DEFAULT_SSID_COUNT", "Channel", "MAC", "MAC_RAND"},
    6,
    {"get ssid_len_min", "get ssid_len_max", "get default_ssid_count", "get channel", "get mac", "get mac_rand"},
    NO_ARGS,
    FOCUS_CONSOLE_END,
    NO_TIP},
    {"Set",
    {"SSID_LEN_MIN", "SSID_LEN_MAX", "DEFAULT_SSID_COUNT", "Channel", "MAC", "MAC_RAND"},
    6,
    {"set ssid_len_min ", "set ssid_len_max ", "set default_ssid_count ", "set channel ", "set mac ", "set mac_rand "},
    INPUT_ARGS,
    FOCUS_CONSOLE_END,
    NO_TIP},
    {"Deauth",
    {"Status", "Set Delay", "Off", "Frame STA", "Device STA", "Spoof STA", "Frame B'Cast", "Device B'Cast", "Spoof B'Cast"},
    9,
    {"deauth", "deauth ", "deauth off", "deauth frame sta", "deauth device sta", "deauth spoof sta", "deauth frame broadcast", "deauth device broadcast", "deauth spoof broadcast"},
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
    {"Help",
    {"Commands", "Help"},
    2,
    {"commands", "help"},
    NO_ARGS,
    FOCUS_CONSOLE_START,
    NO_TIP},
};

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
    app->is_command = (0 <= index); // was 1 to ignore the first row
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
