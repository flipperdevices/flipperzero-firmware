#include "../uart_terminal_app_i.h"

#define MAX_OPTIONS 25

typedef struct {
    const char* item_string;
    int num_options_menu;
    const char* options_menu[MAX_OPTIONS];
} UART_Terminal_Setup_Item;

// SETUP_MENU_ITEMS defined in uart_terminal_app_i.h - if you add an entry here, increment it!
static const UART_Terminal_Setup_Item items[SETUP_MENU_ITEMS] = {
    {"UART Pins", 2, {"13,14", "15,16"}},
    {"Baudrate", 25, {"75",     "110",    "150",    "300",   "600",    "1200",   "1800",
                      "2400",   "4800",   "7200",   "9600",  "14400",  "19200",  "31250",
                      "38400",  "56000",  "57600",  "76800", "115200", "128000", "230400",
                      "250000", "256000", "460800", "921600"}},
    {"HEX mode", 2, {"OFF", "ON"}},
};

static void uart_terminal_scene_setup_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    UART_TerminalApp* app = context;

    furi_assert(index < SETUP_MENU_ITEMS);
    const UART_Terminal_Setup_Item* item = &items[index];

    const int selected_option_index = app->setup_selected_option_index[index];
    furi_assert(selected_option_index < item->num_options_menu);
    app->setup_selected_menu_index = index;
}

static void uart_terminal_scene_setup_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    UART_TerminalApp* app = variable_item_get_context(item);
    furi_assert(app);

    const UART_Terminal_Setup_Item* menu_item = &items[app->setup_selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->setup_selected_option_index[app->setup_selected_menu_index] = item_index;

    // UART Pins
    if(app->setup_selected_menu_index == UART_PINS_ITEM_IDX) {
        app->new_uart_ch = item_index > 0;
    }

    // BaudRate
    if(app->setup_selected_menu_index == BAUDRATE_ITEM_IDX) {
        app->NEW_BAUDRATE = atoi(menu_item->options_menu[item_index]);
    }

    // HEX mode
    if(app->setup_selected_menu_index == HEX_MODE_ITEM_IDX) {
        bool new_mode = item_index > 0;
        if(app->hex_mode != new_mode) {
            app->hex_mode = new_mode;
            app->text_input_store[0] = '\0';
            app->selected_menu_index = 0;
            for(int i = 0; i < START_MENU_ITEMS; ++i) {
                app->selected_option_index[i] = 0;
            }
        }
    }
}

void uart_terminal_scene_setup_on_enter(void* context) {
    UART_TerminalApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, uart_terminal_scene_setup_var_list_enter_callback, app);

    VariableItem* item;
    for(int i = 0; i < SETUP_MENU_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            items[i].item_string,
            items[i].num_options_menu,
            uart_terminal_scene_setup_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->setup_selected_option_index[i]);
        variable_item_set_current_value_text(
            item, items[i].options_menu[app->setup_selected_option_index[i]]);
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, UART_TerminalSceneSetup));

    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewVarItemList);
}

bool uart_terminal_scene_setup_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->setup_selected_menu_index =
            variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }

    return consumed;
}

void uart_terminal_scene_setup_on_exit(void* context) {
    UART_TerminalApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
