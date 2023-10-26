#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

UART_TerminalItem attacks[NUM_ATTACK_ITEMS] = {
    {"Mana", {""}, 1, {""}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP, true},
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
     NO_TIP,
     false},
    {"Homing",
     {"On", "Off"},
     2,
     {"stalk on", "stalk off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false}};

static void displaySubmenu(UART_TerminalApp* app, UART_TerminalItem* item) {
    int newScene = -1;
    if(!strcmp(item->item_string, "Mana")) {
        // Mana menu
        newScene = UART_TerminalSceneAttacksMana;
    }
    if(newScene < 0) {
        return;
    }
    scene_manager_set_scene_state(
        app->scene_manager, UART_TerminalScenePackets, app->selected_menu_index);
    scene_manager_next_scene(app->scene_manager, newScene);
}

/* Callback when an option is selected */
static void uart_terminal_scene_attacks_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem* item = NULL;
    const int selected_option_index = app->selected_option_index[index];

    furi_assert(index < NUM_ATTACK_ITEMS);
    item = &attacks[index];

    /* Are we displaying a submenu or executing something? */
    if(item->isSubMenu) {
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
static void uart_terminal_scene_attacks_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    if(app->selected_menu_index >= NUM_ATTACK_ITEMS) {
        app->selected_menu_index = 0;
    }

    const UART_TerminalItem* menu_item = &attacks[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_attacks_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->attacks_menu_list;
    VariableItem* item;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_attacks_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_ATTACKS;
    for(int i = 0; i < NUM_ATTACK_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            attacks[i].item_string,
            attacks[i].num_options_menu,
            uart_terminal_scene_attacks_var_list_change_callback,
            app);
        /* When transitioning between views app->selected_option_index[i] may
           be referencing a different view's options menu, and may be out of
           bounds of mainmenu[i].options_menu[].
           If that is the case, use 0 instead */
        if(app->selected_option_index[i] >= attacks[i].num_options_menu) {
            app->selected_option_index[i] = 0;
        }
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, attacks[i].options_menu[app->selected_option_index[i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneAttacks));

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewAttacksMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_attacks_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneAttacks, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewTextInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneAttacks, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index =
            variable_item_list_get_selected_item_index(app->attacks_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_attacks_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->attacks_menu_list);
}
