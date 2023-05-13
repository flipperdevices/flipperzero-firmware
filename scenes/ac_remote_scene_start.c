#include "../ac_remote_app_i.h"

#define MAX_OPTIONS (16)
typedef struct
{
    const char *item_string;
    const char *options_menu[MAX_OPTIONS];
    int num_options_menu;
    const int actual_commands[MAX_OPTIONS];
} AC_RemoteItem;

// NUM_MENU_ITEMS defined in ac_remote_app_i.h - if you add an entry here, increment it!
const AC_RemoteItem items[NUM_MENU_ITEMS] = {
    {"Power", {"Off", "On"}, 2, {false, true}},
    {"Mode", {"Heat", "Dry", "Cold", "Auto"}, 4, {AC_HOT, AC_COLD, AC_DRY, AC_AUTO}},
    {"Temperature",
     {"16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"},
     16,
     {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}},
    {"Fan",
     {"Speed 1", "Speed 2", "Speed 3", "Speed 4", "Auto", "Silent"},
     6,
     {FAN_SPEED_1, FAN_SPEED_2, FAN_SPEED_3, FAN_SPEED_4, FAN_SPEED_AUTO, FAN_SPEED_SILENT}},
    {"Vanne",
     {"Auto", "H1", "H2", "H3", "H4", "H5", "Auto Move"},
     7,
     {VANNE_AUTO, VANNE_H1, VANNE_H2, VANNE_H3, VANNE_H4, VANNE_H5, VANNE_AUTO_MOVE}},
};

static void ac_remote_scene_start_var_list_enter_callback(void *context, uint32_t index)
{
    furi_assert(context);
    AC_RemoteApp *app = context;

    furi_assert(index < NUM_MENU_ITEMS);
    app->app_state.selected_menu_index = index;

    view_dispatcher_send_custom_event(app->view_dispatcher, AC_RemoteEventSendCommand);
}

static void ac_remote_scene_start_var_list_change_callback(VariableItem *item)
{
    furi_assert(item);

    AC_RemoteApp *app = variable_item_get_context(item);
    furi_assert(app);

    const AC_RemoteItem *menu_item = &items[app->app_state.selected_menu_index];
    uint8_t item_index = variable_item_get_current_value_index(item);
    furi_assert(item_index < menu_item->num_options_menu);
    variable_item_set_current_value_text(item, menu_item->options_menu[item_index]);
    app->app_state.selected_option_index[app->app_state.selected_menu_index] = item_index;
}

void ac_remote_scene_start_on_enter(void *context)
{
    AC_RemoteApp *app = context;
    VariableItemList *var_item_list = app->var_item_list;

    variable_item_list_set_enter_callback(
        var_item_list, ac_remote_scene_start_var_list_enter_callback, app);

    VariableItem *item;
    for (int i = 0; i < NUM_MENU_ITEMS; ++i)
    {
        item = variable_item_list_add(
            var_item_list,
            items[i].item_string,
            items[i].num_options_menu,
            ac_remote_scene_start_var_list_change_callback,
            app);
        variable_item_set_current_value_index(item, app->app_state.selected_option_index[i]);
        variable_item_set_current_value_text(
            item, items[i].options_menu[app->app_state.selected_option_index[i]]);
    }

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, AC_RemoteSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, AC_RemoteAppViewVarItemList);
}

bool ac_remote_scene_start_on_event(void *context, SceneManagerEvent event)
{
    UNUSED(context);
    AC_RemoteApp *app = context;
    bool consumed = false;

    if (event.type == SceneManagerEventTypeCustom)
    {
        if (event.event == AC_RemoteEventSendCommand)
        {
            NotificationApp *notifications = furi_record_open(RECORD_NOTIFICATION);
            notification_message(notifications, &sequence_blink_white_100);

            ACState acstate;

            acstate.ac_power = items[0].actual_commands[app->app_state.selected_option_index[0]];
            acstate.ac_mode = items[1].actual_commands[app->app_state.selected_option_index[1]];
            acstate.ac_temp = items[2].actual_commands[app->app_state.selected_option_index[2]];
            acstate.ac_fanmode = items[3].actual_commands[app->app_state.selected_option_index[3]];
            acstate.ac_vannemode = items[4].actual_commands[app->app_state.selected_option_index[4]];

            send_ac_mitsubishi(&acstate);

            notification_message(notifications, &sequence_blink_stop);
        }
        consumed = true;
    }
    else if (event.type == SceneManagerEventTypeTick)
    {
        app->app_state.selected_menu_index =
            variable_item_list_get_selected_item_index(app->var_item_list);
        consumed = true;
    }

    return consumed;
}

void ac_remote_scene_start_on_exit(void *context)
{
    AC_RemoteApp *app = context;
    variable_item_list_reset(app->var_item_list);
}
