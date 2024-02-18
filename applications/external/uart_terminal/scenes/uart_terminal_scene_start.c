#include "../uart_terminal_app_i.h"

// Command action type
typedef enum {
    NO_ACTION = 0,
    OPEN_SETUP,
    OPEN_PORT,
    SEND_CMD,
    SEND_AT_CMD,
    SEND_FAST_CMD,
    OPEN_HELP
} ActionType;
// Command availability in different modes
typedef enum { OFF = 0, TEXT_MODE = 1, HEX_MODE = 2, BOTH_MODES = 3 } ModeMask;

#define MAX_OPTIONS (8)

typedef struct {
    const char* item_string;
    const char* options_menu[MAX_OPTIONS];
    int num_options_menu;
    ActionType action;
    ModeMask mode_mask;
} UART_TerminalItem;

static const char at_str[] = "AT";

// NUM_MENU_ITEMS defined in uart_terminal_app_i.h - if you add an entry here, increment it!
static const UART_TerminalItem items[START_MENU_ITEMS] = {
    {"Setup", {""}, 1, OPEN_SETUP, BOTH_MODES},
    {"Open port", {""}, 1, OPEN_PORT, BOTH_MODES},
    {"Send packet", {""}, 1, SEND_CMD, HEX_MODE},
    {"Send command", {""}, 1, SEND_CMD, TEXT_MODE},
    {"Send AT command", {""}, 1, SEND_AT_CMD, TEXT_MODE},
    {"Fast cmd",
     {"help", "uptime", "date", "df -h", "ps", "dmesg", "reboot", "poweroff"},
     8,
     SEND_FAST_CMD,
     TEXT_MODE},
    {"Help", {""}, 1, OPEN_HELP, BOTH_MODES},
};

static uint8_t menu_items_num = 0;
static uint8_t item_indexes[START_MENU_ITEMS] = {0};

static void uart_terminal_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;

    furi_assert(index < menu_items_num);
    uint8_t item_index = item_indexes[index];
    furi_assert(item_index < START_MENU_ITEMS);
    const UART_TerminalItem* item = &items[item_index];

    const int selected_option_index = app->selected_option_index[index];
    furi_assert(selected_option_index < item->num_options_menu);
    app->selected_tx_string = item->options_menu[selected_option_index];
    app->is_command = false;
    app->is_custom_tx_string = false;
    app->selected_menu_index = index;

    switch(item->action) {
    case OPEN_SETUP:
        view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventSetup);
        return;
    case SEND_AT_CMD:
    case SEND_CMD:
    case SEND_FAST_CMD:
        app->is_command = true;

        if(item->action == SEND_AT_CMD) {
            app->selected_tx_string = at_str;
        }

        if(app->hex_mode) {
            view_dispatcher_send_custom_event(
                app->view_dispatcher, UART_TerminalEventStartKeyboardHex);
        } else {
            view_dispatcher_send_custom_event(
                app->view_dispatcher, UART_TerminalEventStartKeyboardText);
        }
        return;
    case OPEN_PORT:
        view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
        return;
    case OPEN_HELP:
        view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartHelp);
        return;
    default:
        return;
    }
}

static void uart_terminal_scene_start_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    uint8_t item_index = item_indexes[app->selected_menu_index];
    const UART_TerminalItem* menu_item = &items[item_index];
    uint8_t option_index = variable_item_get_current_value_index(item);
    furi_assert(option_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[option_index]);
    app->selected_option_index[app->selected_menu_index] = option_index;
}

void uart_terminal_scene_start_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    for(int i = 0; i < START_MENU_ITEMS; ++i) {
        app->selected_option_index[i] = 0;
    }

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_start_var_list_enter_callback, app);

    VariableItem* item;
    menu_items_num = 0;
    for(int i = 0; i < START_MENU_ITEMS; ++i) {
        bool enabled = false;
        if(app->hex_mode && (items[i].mode_mask & HEX_MODE)) {
            enabled = true;
        }
        if(!app->hex_mode && (items[i].mode_mask & TEXT_MODE)) {
            enabled = true;
        }

        if(enabled) {
            item = variable_item_list_add(
                var_item_list,
                items[i].item_string,
                items[i].num_options_menu,
                uart_terminal_scene_start_var_list_change_callback,
                app);
            variable_item_set_current_value_index(item, app->selected_option_index[i]);
            variable_item_set_current_value_text(
                item, items[i].options_menu[app->selected_option_index[i]]);

            item_indexes[menu_items_num] = i;
            menu_items_num++;
        }
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewVarItemList);
}

bool uart_terminal_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventSetup) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalSceneSetup);
        } else if(event.event == UART_TerminalEventStartKeyboardText) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalSceneTextInput);
        } else if(event.event == UART_TerminalEventStartKeyboardHex) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalSceneHexInput);
        } else if(event.event == UART_TerminalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalSceneConsoleOutput);
        } else if(event.event == UART_TerminalEventStartHelp) {
            scene_manager_set_scene_state(
                app->scene_manager, UART_TerminalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, UART_TerminalSceneHelp);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }

    return consumed;
}

void uart_terminal_scene_start_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
