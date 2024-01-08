#include "../gravity_app_i.h"
#include <dolphin/dolphin.h>

GravityItem targets[NUM_TARGET_ITEMS] = {
    {"target-ssids",
     {"Add", "Remove", "List"},
     3,
     {"target-ssids add ", "target-ssids remove ", "target-ssids"},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Scan",
     {"Status", "<SSID>", "WiFi", "BT", "BLE", "BT Svcs", "Off"},
     7,
     {"scan", "scan ", "scan wifi", "scan bt", "scan ble", "scan bt services", "scan off"},
     TOGGLE_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"View",
     {"STA", "AP", "BT", "BT Svcs", "BT+AP+STA", "STA+AP"},
     6,
     {"view sta", "view ap", "view bt", "view bt services", "view ap sta bt", "view ap sta"},
     NO_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false},
    {"Select All",
     {"STA", "AP", "BT"},
     3,
     {"select sta all", "select ap all", "select bt all"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Select",
     {"STA", "AP", "BT"},
     3,
     {"select sta ", "select ap ", "select bt "},
     INPUT_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Selected",
     {"AP", "STA", "BT", "AP+STA+BT"},
     4,
     {"selected ap", "selected sta", "selected bt", "selected"},
     NO_ARGS,
     FOCUS_CONSOLE_START,
     NO_TIP,
     false},
    {"Clear",
     {"STA", "STA Sel.", "AP", "AP Sel.", "BT", "BT Sel.", "BT Svcs", "ALL"},
     8,
     {"clear sta",
      "clear sta selected",
      "clear ap",
      "clear ap selected",
      "clear bt",
      "clear bt selected",
      "clear bt services",
      "clear all"},
     NO_ARGS,
     FOCUS_CONSOLE_END,
     NO_TIP,
     false},
    {"Purge", {""}, 1, {""}, NO_ARGS, FOCUS_CONSOLE_END, NO_TIP, true},
};

static void displaySubmenu(GravityApp* app, GravityItem* item) {
    /* As of writing there are no submenus in Targets */
    int newScene = -1;
    if(!strcmp(item->item_string, "Purge")) {
        newScene = GravityScenePurge;
    }
    if(newScene < 0) {
        return;
    }
    scene_manager_next_scene(app->scene_manager, newScene);
}

/* Callback when an option is selected */
static void gravity_scene_targets_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    GravityApp* app = context;
    GravityItem* item = NULL;
    const int selected_option_index = app->selected_menu_options[GRAVITY_MENU_TARGETS][index];
    furi_assert(index < NUM_TARGET_ITEMS);
    app->selected_menu_items[GRAVITY_MENU_TARGETS] = index;

    item = &targets[index];

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
        gravity_uart_tx((uint8_t*)("\n"), 1);

        if(needs_keyboard) {
            view_dispatcher_send_custom_event(app->view_dispatcher, GravityEventStartKeyboard);
        } else {
            view_dispatcher_send_custom_event(app->view_dispatcher, GravityEventStartConsole);
        }
    }
}

/* Callback when a selected option is changed (I Think) */
static void gravity_scene_targets_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    GravityApp* app = variable_item_get_context(item);
    furi_assert(app);

    if(app->selected_menu_items[GRAVITY_MENU_TARGETS] >= NUM_TARGET_ITEMS) {
        app->selected_menu_items[GRAVITY_MENU_TARGETS] = 0;
    }

    const GravityItem* menu_item = &targets[app->selected_menu_items[GRAVITY_MENU_TARGETS]];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_menu_options[GRAVITY_MENU_TARGETS]
                              [app->selected_menu_items[GRAVITY_MENU_TARGETS]] = item_index;
}

/* Callback on entering the scene (initialisation) */
void gravity_scene_targets_on_enter(void* context) {
    GravityApp* app = context;
    VariableItemList* var_item_list = app->targets_menu_list;
    VariableItem* item;

    variable_item_list_set_enter_callback(
        var_item_list, gravity_scene_targets_var_list_enter_callback, app);

    app->currentMenu = GRAVITY_MENU_TARGETS;
    for(int i = 0; i < NUM_TARGET_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            targets[i].item_string,
            targets[i].num_options_menu,
            gravity_scene_targets_var_list_change_callback,
            app);
        variable_item_set_current_value_index(
            item, app->selected_menu_options[GRAVITY_MENU_TARGETS][i]);
        variable_item_set_current_value_text(
            item, targets[i].options_menu[app->selected_menu_options[GRAVITY_MENU_TARGETS][i]]);
    }
    variable_item_list_set_selected_item(
        var_item_list, app->selected_menu_items[GRAVITY_MENU_TARGETS]);

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewTargetsMenu);
}

/* Event handler callback - Handle scene change and tick events */
bool gravity_scene_targets_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    GravityApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        int nextScene = 0;
        if(event.event == GravityEventStartKeyboard) {
            nextScene = Gravity_AppViewTextInput;
        } else if(event.event == GravityEventStartConsole) {
            nextScene = Gravity_AppViewConsoleOutput;
        }
        scene_manager_next_scene(app->scene_manager, nextScene);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_items[GRAVITY_MENU_TARGETS] =
            variable_item_list_get_selected_item_index(app->targets_menu_list);
        consumed = true;
    }
    return consumed;
}

/* Clean up on exit */
void gravity_scene_targets_on_exit(void* context) {
    GravityApp* app = context;
    variable_item_list_reset(app->targets_menu_list);
}
