#include "../evil_portal_app_i.h"

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
} Evil_PortalItem;

// NUM_MENU_ITEMS defined in evil_portal_app_i.h - if you add an entry here,
// increment it!
const Evil_PortalItem items[NUM_MENU_ITEMS] = {
    // send command
    {"Start portal", {""}, 1, {SET_HTML_CMD}, NO_ARGS, FOCUS_CONSOLE_END, SHOW_STOPSCAN_TIP},

    // stop portal
    {"Stop portal", {""}, 1, {RESET_CMD}, NO_ARGS, FOCUS_CONSOLE_START, SHOW_STOPSCAN_TIP},

    // console
    {"Save logs", {""}, 1, {"savelogs"}, NO_ARGS, FOCUS_CONSOLE_START, SHOW_STOPSCAN_TIP},

    // help
    {"Help", {""}, 1, {"help"}, NO_ARGS, FOCUS_CONSOLE_START, SHOW_STOPSCAN_TIP},
};

static void evil_portal_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    furi_assert(context);
    Evil_PortalApp* app = context;

    furi_assert(index < NUM_MENU_ITEMS);
    const Evil_PortalItem* item = &items[index];

    const int selected_option_index = app->selected_option_index[index];
    furi_assert(selected_option_index < item->num_options_menu);
    app->selected_tx_string = item->actual_commands[selected_option_index];
    app->is_command = true;
    app->is_custom_tx_string = false;
    app->selected_menu_index = index;
    app->focus_console_start = (item->focus_console == FOCUS_CONSOLE_TOGGLE) ?
                                   (selected_option_index == 0) :
                                   item->focus_console;
    app->show_stopscan_tip = item->show_stopscan_tip;

    view_dispatcher_send_custom_event(app->view_dispatcher, Evil_PortalEventStartConsole);
}

static void evil_portal_scene_start_var_list_change_callback(VariableItem* item) {
    furi_assert(item);

    Evil_PortalApp* app = variable_item_get_context(item);
    furi_assert(app);

    const Evil_PortalItem* menu_item = &items[app->selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->selected_option_index[app->selected_menu_index] = item_index;
}

void evil_portal_scene_start_on_enter(void* context) {
    Evil_PortalApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, evil_portal_scene_start_var_list_enter_callback, app);

    VariableItem* item;
    for(int i = 0; i < NUM_MENU_ITEMS; ++i) {
        item = variable_item_list_add(
            var_item_list,
            items[i].item_string,
            items[i].num_options_menu,
            evil_portal_scene_start_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->selected_option_index[i]);
        variable_item_set_current_value_text(
            item, items[i].options_menu[app->selected_option_index[i]]);
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, Evil_PortalSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, Evil_PortalAppViewVarItemList);
}

bool evil_portal_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    Evil_PortalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == Evil_PortalEventStartPortal) {
            scene_manager_set_scene_state(
                app->scene_manager, Evil_PortalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, Evil_PortalAppViewStartPortal);
        } else if(event.event == Evil_PortalEventStartKeyboard) {
            scene_manager_set_scene_state(
                app->scene_manager, Evil_PortalSceneStart, app->selected_menu_index);
        } else if(event.event == Evil_PortalEventStartConsole) {
            scene_manager_set_scene_state(
                app->scene_manager, Evil_PortalSceneStart, app->selected_menu_index);
            scene_manager_next_scene(app->scene_manager, Evil_PortalAppViewConsoleOutput);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        app->selected_menu_index = variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }

    return consumed;
}

void evil_portal_scene_start_on_exit(void* context) {
    Evil_PortalApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
