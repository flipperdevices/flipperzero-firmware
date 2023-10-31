#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

// Auth: None, WEP, WPA
// Mana Mode: Normal, Loud, <todo>
// verbose
// Run: Status, Start, Stop
// Clear Mana Cache

UART_TerminalItem attacks_mana[NUM_ATTACK_MANA_ITEMS] = {
  {"Authentication",
  {"None", "WEP", "WPA"},
  3,
  {"auth none ", "auth wep ", "auth wpa "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Mana Mode",
  {"Normal", "Loud"},
  2,
  {"", "LOUD "},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Verbose",
  {"On", "Off"},
  2,
  {"VERBOSE ", ""},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Run",
  {"Status", "Start", "Stop"},
  3,
  {"", "on", "off"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_START,
  NO_TIP,
  false},
  {"Clear Mana Cache", {""}, 1, {"clear"}, TOGGLE_ARGS, FOCUS_CONSOLE_START, NO_TIP, false}
};

enum ManaMenuItems {
    MANA_MENU_AUTH = 0,
    MANA_MENU_MODE,
    MANA_MENU_VERBOSE,
    MANA_MENU_RUN,
    MANA_MENU_CLEAR,
};

VariableItem *manaMenuItemViews[NUM_ATTACK_MANA_ITEMS];

/* Callback when an option is selected */
static void uart_terminal_scene_attacks_mana_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem *item = NULL;
    int selected_option_index = app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][index];

    /* Cycle through options when enter pressed */
    if (index < MANA_MENU_RUN) {
        // increment selected_option_index % number of options
        selected_option_index = (selected_option_index + 1) % item->num_options_menu;
        app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][index] = selected_option_index;
        // YAGNI: Null check
        variable_item_set_current_value_index(manaMenuItemViews[index], selected_option_index);
        variable_item_set_current_value_text(manaMenuItemViews[index], item->options_menu[selected_option_index]);
        return;
    }

    furi_assert(index < NUM_ATTACK_MANA_ITEMS);
    item = &attacks_mana[index];
    furi_assert(selected_option_index < item->num_options_menu);
    dolphin_deed(DolphinDeedGpioUartBridge);

    /* Are we getting status, starting or stopping? */
    app->selected_tx_string = "";
    if (!strcmp(item->actual_commands[selected_option_index], "")) {
        app->selected_tx_string = "mana";
    } else if (!strcmp(item->actual_commands[selected_option_index], "off")) {
        app->selected_tx_string = "mana off";
    } else if (!strcmp(item->actual_commands[selected_option_index], "clear")) {
        app->selected_tx_string = "mana clear";
    } else if (!strcmp(item->actual_commands[selected_option_index], "on")) {
        /* The command is mana [verbose] [loud] [auth [none | wep | wpa]] [clear] [on | off] */
        /* ==> mana clear verbose loud auth none off\0 ==  38, or strlen + "mana" + 5 spaces + \0 */
        int cmdLength = 0;
        UART_TerminalItem *thisItem;
        for (int i = 0; i <= MANA_MENU_RUN; ++i) {
            thisItem = &attacks_mana[i];
            cmdLength += strlen(thisItem->actual_commands[app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][i]]);
        }
        /* Add chars for MANA \0 */
        cmdLength += 6;

        char *mana_command = malloc(sizeof(char) * cmdLength);
        if (mana_command == NULL) {
            /* Panic */
            return;
        }
        memset(mana_command, '\0', cmdLength);
        strcpy(mana_command, "mana ");
        strcat(mana_command, attacks_mana[MANA_MENU_VERBOSE].actual_commands[app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][MANA_MENU_VERBOSE]]);
        strcat(mana_command, attacks_mana[MANA_MENU_MODE].actual_commands[app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][MANA_MENU_MODE]]);
        strcat(mana_command, attacks_mana[MANA_MENU_AUTH].actual_commands[app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][MANA_MENU_AUTH]]);
        strcat(mana_command, attacks_mana[MANA_MENU_RUN].actual_commands[app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][MANA_MENU_RUN]]);
        app->selected_tx_string = mana_command;
        app->free_command = true;
    }
    app->is_command = true;
    app->is_custom_tx_string = false;
    app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA] = index;
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
static void uart_terminal_scene_attacks_mana_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    if (app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA] >= NUM_ATTACK_MANA_ITEMS) {
        app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA] = 0;
    }

    const UART_TerminalItem* menu_item = &attacks_mana[app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA]];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA]] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_attacks_mana_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->attacks_mana_menu_list;
    VariableItem *item;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_attacks_mana_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_ATTACKS_MANA;
    for(int i = 0; i < NUM_ATTACK_MANA_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            attacks_mana[i].item_string,
            attacks_mana[i].num_options_menu,
            uart_terminal_scene_attacks_mana_var_list_change_callback,
            app);
        manaMenuItemViews[i] = item;
        variable_item_set_current_value_index(item, app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][i]);
        variable_item_set_current_value_text(
            item, attacks_mana[i].options_menu[app->selected_menu_options[GRAVITY_MENU_ATTACKS_MANA][i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneAttacksMana));

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewAttacksManaMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_attacks_mana_on_event(void* context, SceneManagerEvent event) {
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
        scene_manager_set_scene_state(app->scene_manager, UART_TerminalSceneAttacksMana, app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA]);
        scene_manager_next_scene(app->scene_manager, nextScene);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA] = variable_item_list_get_selected_item_index(app->attacks_mana_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_attacks_mana_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->attacks_mana_menu_list);
    scene_manager_set_scene_state(app->scene_manager, UART_TerminalSceneAttacksMana, app->selected_menu_items[GRAVITY_MENU_ATTACKS_MANA]);
}
