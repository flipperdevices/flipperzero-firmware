#include "../uart_terminal_app_i.h"
#include <dolphin/dolphin.h>

UART_TerminalItem packets_fuzz[NUM_PACKETS_FUZZ_ITEMS] = {
  {"Packet Type",
  {"Beacon", "Probe Request", "Probe Response"},
  3,
  {"beacon", "req", "resp"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Fuzz Type",
  {"SSID Overflow", "SSID Malformed"},
  2,
  {"overflow", "malformed"},
  TOGGLE_ARGS,
  FOCUS_CONSOLE_END,
  NO_TIP,
  false},
  {"Target",
  {"Broadcast", "Target-SSIDs", "Selected STA", "Selected AP", "Random"},
  5,
  {"broadcast", "target-ssids", "selectedSTA", "selectedAP", "random"},
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
};

#define FUZZ_MENU_PACKET_TYPE 0
#define FUZZ_MENU_FUZZ_TYPE 1
#define FUZZ_MENU_TARGET 2
#define FUZZ_MENU_RUN 3

/* Callback when an option is selected */
static void uart_terminal_scene_packets_fuzz_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem *item = NULL;
    const int selected_option_index = app->selected_option_index[index];

    /* Don't do anything unless it's the Run menu item */
    if (index != FUZZ_MENU_RUN) {
        return;
    }

    furi_assert(index < NUM_PACKETS_FUZZ_ITEMS);
    item = &packets_fuzz[index];
    furi_assert(selected_option_index < item->num_options_menu);
    dolphin_deed(DolphinDeedGpioUartBridge);

    /* Are we getting status, starting or stopping? */
    app->selected_tx_string = "";
    if (!strcmp(item->actual_commands[selected_option_index], "")) {
        app->selected_tx_string = "fuzz";
    } else if (!strcmp(item->actual_commands[selected_option_index], "off")) {
        app->selected_tx_string = "fuzz off";
    } else if (!strcmp(item->actual_commands[selected_option_index], "on")) {
        /* The command is FUZZ packetType fuzzType target on */
        int cmdLength = 0;
        UART_TerminalItem *thisItem;
        for (int i = 0; i < FUZZ_MENU_RUN; ++i) {
            thisItem = &packets_fuzz[i];
            cmdLength += strlen(thisItem->actual_commands[app->selected_option_index[i]]);
        }
        /* Add chars for FUZZ ON\0 & 4 spaces */
        cmdLength += 11;

        char *fuzz_command = malloc(sizeof(char) * cmdLength);
        if (fuzz_command == NULL) {
            /* Panic */
            return;
        }
        /* sprintf doesn't work. Instead a loop of strcat's
        sprintf(fuzz_command, "FUZZ %s %s %s ON",
                packets_fuzz[0].actual_commands[app->selected_option_index[0]],
                packets_fuzz[1].actual_commands[app->selected_option_index[1]],
                packets_fuzz[2].actual_commands[app->selected_option_index[2]]);
        */
        strcpy(fuzz_command, "FUZZ ");
        for (int i = 0; i < FUZZ_MENU_RUN; ++i) {
            strcat(fuzz_command, packets_fuzz[i].actual_commands[app->selected_option_index[i]]);
            strcat(fuzz_command, " ");
        }
        strcat(fuzz_command, "on");
        app->selected_tx_string = fuzz_command;
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
static void uart_terminal_scene_packets_fuzz_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    const UART_TerminalItem* menu_item = &packets_fuzz[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

/* Callback on entering the scene (initialisation) */
void uart_terminal_scene_packets_fuzz_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->packets_fuzz_menu_list;
    VariableItem *item;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_packets_fuzz_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_PACKETS_FUZZ;
    for(int i = 0; i < NUM_PACKETS_FUZZ_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            packets_fuzz[i].item_string,
            packets_fuzz[i].num_options_menu,
            uart_terminal_scene_packets_fuzz_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, packets_fuzz[i].options_menu[app->selected_option_index[i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalScenePacketsFuzz));

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewPacketsFuzzMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool uart_terminal_scene_packets_fuzz_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalScenePacketsFuzz, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewTextInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalScenePacketsFuzz, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->packets_fuzz_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_packets_fuzz_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->packets_fuzz_menu_list);
}
