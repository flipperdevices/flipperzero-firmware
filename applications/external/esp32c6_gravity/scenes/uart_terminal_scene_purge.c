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
    {"Purge By Age?", {"On", "Off"}, 2, {"on", "off"}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
    {"Purge Age",
     {"5 sec",
      "10 sec",
      "20 sec",
      "30 sec",
      "60 sec",
      "90 sec",
      "120 sec",
      "3 min",
      "5 min",
      "10 min",
      "30 min",
      "1 hour"},
     12,
     {"5", "10", "20", "30", "60", "90", "120", "180", "300", "600", "1800", "3600"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Purge By RSSI?", {"On", "Off"}, 2, {"on", "off"}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
    {"Purge RSSI",
     {"-125", "-115", "-105", "-95", "-85", "-75", "-65", "-55", "-45", "-35", "-25", "-15"},
     12,
     {"-125", "-115", "-105", "-95", "-85", "-75", "-65", "-55", "-45", "-35", "-25", "-15"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Purge Unselected?",
     {"On", "Off"},
     2,
     {"on", "off"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Purge Unnamed?", {"On", "Off"}, 2, {"on", "off"}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP, false},
    {"Done",
     {"Save Default", "Purge APs", "Purge STAs", "Purge BT", "Purge BLE"},
     5,
     {"save", "ap", "sta", "bt", "ble"},
     NO_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false}};

enum PurgeMenuItems {
    PURGE_MENU_AGE_ON = 0,
    PURGE_MENU_AGE,
    PURGE_MENU_RSSI_ON,
    PURGE_MENU_RSSI,
    PURGE_MENU_UNSELECTED_ON,
    PURGE_MENU_UNNAMED_ON,
    PURGE_MENU_RUN,
};

VariableItem* purgeMenuItemViews[NUM_PURGE_ITEMS];

int indexOf(char* val, const char** array, int arrayLen) {
    int i;
    for(i = 0; i < arrayLen && strcmp(val, array[i]); ++i) {
    }
    if(i == arrayLen) {
        i = -1;
    }
    return i;
}

static void purgeLoadFromMemory(UART_TerminalApp* app) {
    /* A purge strategy is in memory, use it for initial values */
    // Figure out what index in purgeMenu[PURGE_MENU_AGE].options_menu[]
    //      purgeAge is & set app->selected_option_index[PURGE_MENU_AGE]
    if(app->purgeStrategy == 0) {
        // TODO: Report missing value
        return;
    }
    char str[5];
    itoa(app->purgeAge, str, 10);
    int idx = indexOf(
        str,
        purgeMenu[PURGE_MENU_AGE].actual_commands,
        purgeMenu[PURGE_MENU_AGE].num_options_menu);
    if(idx >= 0) {
        app->selected_option_index[PURGE_MENU_AGE] = idx;
    }
    // Find index of purgeRSSI in purgeMenu[PURGE_MENU_RSSI].options_menu[]
    // app->selected_option_index[PURGE_MENU_RSSI] = that
    itoa(app->purgeRSSI, str, 10);
    idx = indexOf(
        str,
        purgeMenu[PURGE_MENU_RSSI].actual_commands,
        purgeMenu[PURGE_MENU_RSSI].num_options_menu);
    if(idx >= 0) {
        app->selected_option_index[PURGE_MENU_RSSI] = idx;
    }
    /* Now set the boolean values */
    int idxOn = indexOf(
        "on",
        purgeMenu[PURGE_MENU_AGE_ON].actual_commands,
        purgeMenu[PURGE_MENU_AGE_ON].num_options_menu);
    int idxOff = indexOf(
        "off",
        purgeMenu[PURGE_MENU_AGE_ON].actual_commands,
        purgeMenu[PURGE_MENU_AGE_ON].num_options_menu);
    if((app->purgeStrategy & GRAVITY_PURGE_AGE) == GRAVITY_PURGE_AGE) {
        app->selected_option_index[PURGE_MENU_AGE_ON] = idxOn;
    } else {
        app->selected_option_index[PURGE_MENU_AGE_ON] = idxOff;
    }
    if((app->purgeStrategy & GRAVITY_PURGE_RSSI) == GRAVITY_PURGE_RSSI) {
        app->selected_option_index[PURGE_MENU_RSSI_ON] = idxOn;
    } else {
        app->selected_option_index[PURGE_MENU_RSSI_ON] = idxOff;
    }
    if((app->purgeStrategy & GRAVITY_PURGE_UNNAMED) == GRAVITY_PURGE_UNNAMED) {
        app->selected_option_index[PURGE_MENU_UNNAMED_ON] = idxOn;
    } else {
        app->selected_option_index[PURGE_MENU_UNNAMED_ON] = idxOff;
    }
    if((app->purgeStrategy & GRAVITY_PURGE_UNSELECTED) == GRAVITY_PURGE_UNSELECTED) {
        app->selected_option_index[PURGE_MENU_UNSELECTED_ON] = idxOn;
    } else {
        app->selected_option_index[PURGE_MENU_UNSELECTED_ON] = idxOff;
    }
}
/* Callback when an option is selected */
static void uart_terminal_scene_purge_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    UART_TerminalItem* item = NULL;
    int selected_option_index = app->selected_option_index[index];

    furi_assert(index < NUM_PURGE_ITEMS);
    item = &purgeMenu[index];
    furi_assert(selected_option_index < item->num_options_menu);

    /* Cycle through options when enter pressed */
    if(index < PURGE_MENU_RUN) {
        // increment selected_option_index % number of options
        selected_option_index = (selected_option_index + 1) % item->num_options_menu;
        app->selected_option_index[index] = selected_option_index;
        // YAGNI: Null check
        variable_item_set_current_value_index(purgeMenuItemViews[index], selected_option_index);
        variable_item_set_current_value_text(
            purgeMenuItemViews[index], item->options_menu[selected_option_index]);
        return;
    }

    app->free_command = false;

    dolphin_deed(DolphinDeedGpioUartBridge);
    bool bAge = strcmp(
        purgeMenu[PURGE_MENU_AGE_ON].actual_commands[app->selected_option_index[PURGE_MENU_AGE_ON]],
        "off");
    bool bRSSI = strcmp(
        purgeMenu[PURGE_MENU_RSSI_ON]
            .actual_commands[app->selected_option_index[PURGE_MENU_RSSI_ON]],
        "off");
    bool bUnselected = strcmp(
        purgeMenu[PURGE_MENU_UNSELECTED_ON]
            .actual_commands[app->selected_option_index[PURGE_MENU_UNSELECTED_ON]],
        "off");
    bool bUnnamed = strcmp(
        purgeMenu[PURGE_MENU_UNNAMED_ON]
            .actual_commands[app->selected_option_index[PURGE_MENU_UNNAMED_ON]],
        "off");
    const char* strAge =
        purgeMenu[PURGE_MENU_AGE].actual_commands[app->selected_option_index[PURGE_MENU_AGE]];
    const char* strRSSI =
        purgeMenu[PURGE_MENU_RSSI].actual_commands[app->selected_option_index[PURGE_MENU_RSSI]];
    /* At this point we're ready to save or run the configured purge strategy */
    /* Expected command values: save, ap, sta, bt, ble */
    /* Initialise the serial console */
    uart_terminal_uart_tx((uint8_t*)("\n"), 1);
    int strat = 0;
    if(bAge) {
        strat += GRAVITY_PURGE_AGE;
    }
    if(bRSSI) {
        strat += GRAVITY_PURGE_RSSI;
    }
    if(bUnnamed) {
        strat += GRAVITY_PURGE_UNNAMED;
    }
    if(bUnselected) {
        strat += GRAVITY_PURGE_UNSELECTED;
    }
    if(strat == 0) {
        strat = GRAVITY_PURGE_NONE;
    }
    if(!strcmp(item->actual_commands[selected_option_index], "save")) {
        /* Compile selected strategies into their binary values */
        /* Save age and RSSI directly, then launch console to save strategy
           Probably something to refactor into a better UI in the near future */
        //SET BLE_PURGE_MIN_AGE BLE_PURGE_MAX_RSSI 23 + strlen + \n\0
        /* I agonised a bit over whether age and RSSI should be saved all the
           time, or only when selected. These will only be saved where they
           have been selected for use. */
        char saveCmd[29] = "";
        if(bAge) {
            app->purgeAge = atoi(strAge);
            strcpy(saveCmd, "set BLE_PURGE_MIN_AGE ");
            strcat(saveCmd, strAge);
            strcat(saveCmd, "\n");
            uart_terminal_uart_tx((uint8_t*)saveCmd, strlen(saveCmd));
            // YAGNI: Delay
        }
        if(bRSSI) {
            app->purgeRSSI = atoi(strRSSI);
            strcpy(saveCmd, "set BLE_PURGE_MAX_RSSI ");
            strcat(saveCmd, strRSSI);
            strcat(saveCmd, "\n");
            uart_terminal_uart_tx((uint8_t*)saveCmd, strlen(saveCmd));
            // YAGNI: Delay
        }
        char strStrat[3];
        itoa(strat, strStrat, 10);
        strcpy(saveCmd, "set BLE_PURGE_STRAT ");
        strcat(saveCmd, strStrat);
        if(saveCmd[strlen(saveCmd) - 1] == ' ') {
            saveCmd[strlen(saveCmd) - 1] = '\0';
        }
        char* tx_command = malloc(sizeof(char) * (strlen(saveCmd) + 1));
        if(tx_command == NULL) {
            //TODO - Panic
            return;
        }
        memset(tx_command, '\0', strlen(saveCmd) + 1);
        strncpy(tx_command, saveCmd, strlen(saveCmd));
        app->selected_tx_string = tx_command;
        app->free_command = true;
        /* Save defaults in Flipper app */
        app->purgeStrategy = strat;
    } else {
        //purge [ AP | STA | BT | BLE ]+ [ RSSI [ <maxRSSI> ] | AGE [ <minAge> ] | UNNAMED | UNSELECTED | NONE ]+
        // PURGE BLE RSSI -100 AGE 3600 UNNAMED UNSELECTED
        char purgeCmd[49];
        strcpy(purgeCmd, "purge ");
        // ap/sta/bt/ble
        strcat(purgeCmd, item->actual_commands[selected_option_index]);
        strcat(purgeCmd, " ");
        if(bRSSI) {
            strcat(purgeCmd, "RSSI ");
            strcat(purgeCmd, strRSSI);
            strcat(purgeCmd, " ");
        }
        if(bAge) {
            strcat(purgeCmd, "AGE ");
            strcat(purgeCmd, strAge);
            strcat(purgeCmd, " ");
        }
        if(bUnnamed) {
            strcat(purgeCmd, "UNNAMED ");
        }
        if(bUnselected) {
            strcat(purgeCmd, "UNSELECTED ");
        }
        if(!(bAge || bRSSI || bUnselected || bUnnamed)) {
            strcat(purgeCmd, "NONE ");
        }
        /* Remove a trailing space to avoid displaying keyboard */
        if(purgeCmd[strlen(purgeCmd) - 1] == ' ') {
            purgeCmd[strlen(purgeCmd) - 1] = '\0';
        }
        char* tx_command = malloc(sizeof(char) * (strlen(purgeCmd) + 1));
        if(tx_command == NULL) {
            // TODO - Panic
            return;
        }
        memset(tx_command, '\0', strlen(purgeCmd) + 1);
        strncpy(tx_command, purgeCmd, strlen(purgeCmd));
        app->selected_tx_string = tx_command;
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
    bool needs_keyboard =
        ((item->needs_keyboard == INPUT_ARGS) ||
         (item->needs_keyboard == TOGGLE_ARGS && (app->selected_tx_string[cmdLen - 1] == ' ')));

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

    if(app->selected_menu_index >= NUM_PURGE_ITEMS) {
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
    VariableItem* item;

    /* Initialise app->free_command */
    app->free_command = false;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_purge_var_list_enter_callback, app);

    /* Load purge config from Flipper memory if we've been here before */
    if(app->purgeStrategy != 0) {
        purgeLoadFromMemory(app);
    }
    /* Need to create the menu before we can set values for selected_options_index[] */
    app->currentMenu = GRAVITY_MENU_PURGE;
    for(int i = 0; i < NUM_PURGE_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            purgeMenu[i].item_string,
            purgeMenu[i].num_options_menu,
            uart_terminal_scene_purge_var_list_change_callback,
            app);
        purgeMenuItemViews[i] = item;
        /* When transitioning between views app->selected_option_index[i] may
           be referencing a different view's options menu, and may be out of
           bounds of mainmenu[i].options_menu[].
           If that is the case, use 0 instead */
        if(app->selected_option_index[i] >= purgeMenu[i].num_options_menu) {
            app->selected_option_index[i] = 0;
        }
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, purgeMenu[i].options_menu[app->selected_option_index[i]]);
    }
    /* Set selected menu item when returning back to the scene */
    app->selected_menu_index =
        scene_manager_get_scene_state(app->scene_manager, UART_TerminalScenePurge);
    if(app->selected_menu_index >= NUM_PURGE_ITEMS) {
        app->selected_menu_index = 0;
    }
    variable_item_list_set_selected_item(var_item_list, app->selected_menu_index);

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
        app->selected_menu_index =
            variable_item_list_get_selected_item_index(app->purge_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void uart_terminal_scene_purge_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->purge_menu_list);
}
