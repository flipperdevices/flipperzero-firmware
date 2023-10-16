#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

/*
    Min Age
    Max RSSI
    Purge by age?
    Purge by RSSI?
    Purge unselected?
    Purge unnamed?
*/

UART_TerminalItem purgeMenu[NUM_PURGE_ITEMS] = {
  {"Purge By Age?", {"On", "Off"}, 2, {"On", "Off"}, TOGGLE_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
  {"Purge Age",
  {"5 sec", "10 sec", "20 sec", "30 sec", "60 sec", "90 sec", "120 sec", "3 min", "5 min", "10 min", "30 min", "1 hour"},
  12,
  {"5", "10", "20", "30", "60", "90", "120", "180", "300", "600", "1800", "3600"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Purge By RSSI?", {"On", "Off"}, 2, {"On", "Off"}, TOGGLE_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
  {"Purge RSSI",
  {"-125", "-120", "-115", "-110", "-105", "-100", "-95", "-90", "-85", "-80", "-75", "-70", "-65", "-60", "-55", "-50", "-45", "-40", "-35", "-30", "-25", "-20", "-15", "-10"},
  24,
  {"-125", "-120", "-115", "-110", "-105", "-100", "-95", "-90", "-85", "-80", "-75", "-70", "-65", "-60", "-55", "-50", "-45", "-40", "-35", "-30", "-25", "-20", "-15", "-10"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Purge Unselecetd?", {"On", "Off"}, 2, {"On", "Off"}, TOGGLE_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
  {"Purge Unnamed?", {"On", "Off"}, 2, {"On", "Off"}, TOGGLE_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
  {"Done",
  {"Save & Run", "Run Only", "Save Only"},
  3,
  {"", "run", "save"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false}
};

#define PURGE_MENU_AGE 0
#define PURGE_MENU_RSSI 1
#define PURGE_MENU_AGE_ON 2
#define PURGE_MENU_RSSI_ON 3
#define PURGE_MENU_UNSELECTED_ON 4
#define PURGE_MENU_UNNAMED_ON 5
#define PURGE_MENU_RUN 6

/* Callback when an option is selected */
static void uart_terminal_scene_purge_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem *item = NULL;
    const int selected_option_index = app->selected_option_index[index];

    /* Ignore enter on the boolean settings */
    if (index < PURGE_MENU_RUN && index > PURGE_MENU_RSSI) {
        return;
    }

    furi_assert(index < NUM_PURGE_ITEMS);
    item = &purgeMenu[index];
    furi_assert(selected_option_index < item->num_options_menu);
    dolphin_deed(DolphinDeedGpioUartBridge);

    /* TODO: Compile command string based on selection */
    app->selected_tx_string = "";
    if (!strcmp(item->actual_commands[selected_option_index], "")) {
        app->selected_tx_string = "mana";
    } else if (!strcmp(item->actual_commands[selected_option_index], "off")) {
        app->selected_tx_string = "mana off";
    } else if (!strcmp(item->actual_commands[selected_option_index], "on")) {
        /* The command is mana [verbose] on auth none loud off */
        /* ==> mana on auth loud\0 + up to 6 spaces */
        int cmdLength = 0;
        UART_TerminalItem *thisItem;
        for (int i = 0; i < PURGE_MENU_RUN; ++i) {
            thisItem = &purgeMenu[i];
            cmdLength += strlen(thisItem->actual_commands[app->selected_option_index[i]]);
        }
        /* Add chars for DEAUTH ON\0 & 3 spaces */
        cmdLength += 21;

        char *mana_command = malloc(sizeof(char) * cmdLength);
        if (mana_command == NULL) {
            /* Panic */
            return;
        }
        // strcpy(mana_command, "MANA ");
        // strcat(mana_command, attacks_mana[MANA_MENU_VERBOSE].actual_commands[app->selected_option_index[MANA_MENU_VERBOSE]]);
        // strcat(mana_command, " ON AUTH ");
        // strcat(mana_command, attacks_mana[MANA_MENU_AUTH].actual_commands[app->selected_option_index[MANA_MENU_VERBOSE]]);
        // strcat(mana_command, " LOUD ");
        // strcat(mana_command, attacks_mana[MANA_MENU_MODE].actual_commands[app->selected_option_index[MANA_MENU_MODE]]);
        app->selected_tx_string = mana_command;
        app->free_command = true;
    }
    app->is_command = true;
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

/* Callback when a selected option is changed (I Think) */
static void uart_terminal_scene_purge_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    if (app->selected_menu_index >= NUM_PURGE_ITEMS) {
        app->selected_menu_index = 0;
    }

    const UART_TerminalItem* menu_item = &purgeMenu[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_purge_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->purge_menu_list;
    VariableItem *item;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_purge_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_PURGE;
    for(int i = 0; i < NUM_PURGE_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            purgeMenu[i].item_string,
            purgeMenu[i].num_options_menu,
            uart_terminal_scene_purge_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, purgeMenu[i].options_menu[app->selected_option_index[i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalScenePurge));

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewPurgeMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_purge_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalScenePurge, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewTextInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalScenePurge, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->purge_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_purge_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->purge_menu_list);
}
