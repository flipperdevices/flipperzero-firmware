#include "../ac_remote_app_i.h"

extern const Icon I_PwrOff_25x27;
extern const Icon I_PwrOff_hvr_25x27;
extern const Icon I_PwrOn_25x27;
extern const Icon I_PwrOn_hvr_25x27;

extern const Icon I_ModeHeat_25x27;
extern const Icon I_ModeHeat_hvr_25x27;
extern const Icon I_ModeCold_25x27;
extern const Icon I_ModeCold_hvr_25x27;
extern const Icon I_ModeDry_25x27;
extern const Icon I_ModeDry_hvr_25x27;
extern const Icon I_ModeAuto_25x27;
extern const Icon I_ModeAuto_hvr_25x27;

extern const Icon I_Fan1_25x27;
extern const Icon I_Fan1_hvr_25x27;
extern const Icon I_Fan2_25x27;
extern const Icon I_Fan2_hvr_25x27;
extern const Icon I_Fan3_25x27;
extern const Icon I_Fan3_hvr_25x27;
extern const Icon I_Fan4_25x27;
extern const Icon I_Fan4_hvr_25x27;
extern const Icon I_FanAuto_25x27;
extern const Icon I_FanAuto_hvr_25x27;
extern const Icon I_FanSilent_25x27;
extern const Icon I_FanSilent_hvr_25x27;

extern const Icon I_VaneAuto_25x27;
extern const Icon I_VaneAuto_hvr_25x27;
extern const Icon I_VaneH1_25x27;
extern const Icon I_VaneH1_hvr_25x27;
extern const Icon I_VaneH2_25x27;
extern const Icon I_VaneH2_hvr_25x27;
extern const Icon I_VaneH3_25x27;
extern const Icon I_VaneH3_hvr_25x27;
extern const Icon I_VaneH4_25x27;
extern const Icon I_VaneH4_hvr_25x27;
extern const Icon I_VaneH5_25x27;
extern const Icon I_VaneH5_hvr_25x27;
extern const Icon I_VaneAutoMove_25x27;
extern const Icon I_VaneAutoMove_hvr_25x27;

extern const Icon I_TooCool_25x13;
extern const Icon I_TooCool_hvr_25x13;
extern const Icon I_TooWarm_25x13;
extern const Icon I_TooWarm_hvr_25x13;

#define MAX_OPTIONS (16)
typedef struct
{
    const uint8_t values_count;
    const int commands[MAX_OPTIONS];
    const uint16_t matrix_place_x;
    const uint16_t matrix_place_y;
    const uint16_t x;
    const uint16_t y;
    const Icon *icon_name[MAX_OPTIONS];
    const Icon *icon_name_selected[MAX_OPTIONS];
} ACRemoteButtonItem;

#define ID_LABEL_TEMPERATURE 1
#define ID_BUTTON_POWER 0
#define ID_BUTTON_MODE 1
#define ID_BUTTON_TOO_COOL 2
#define ID_BUTTON_FAN 3
#define ID_BUTTON_TOO_WARM 4
#define ID_BUTTON_VANE 5

const ACRemoteButtonItem button_items[NUM_BUTTON_ITEMS] = {
    {2, {AC_ON, AC_OFF}, 0, 0, 3, 22,
        {&I_PwrOn_25x27, &I_PwrOff_25x27}, {&I_PwrOn_hvr_25x27, &I_PwrOff_hvr_25x27}},
    {4, {AC_MODE_HEAT, AC_MODE_COLD, AC_MODE_DRY, AC_MODE_AUTO}, 1, 0, 36, 22,
        {&I_ModeHeat_25x27, &I_ModeCold_25x27, &I_ModeDry_25x27, &I_ModeAuto_25x27},
        {&I_ModeHeat_hvr_25x27, &I_ModeCold_hvr_25x27, &I_ModeDry_hvr_25x27, &I_ModeAuto_hvr_25x27}},
    {1, {AC_TOO_COOL}, 0, 1, 3, 59,
        {&I_TooCool_25x13}, {&I_TooCool_hvr_25x13}},
    {6, {AC_FAN_SPEED_1, AC_FAN_SPEED_2, AC_FAN_SPEED_3, AC_FAN_SPEED_4, AC_FAN_SPEED_AUTO, AC_FAN_SPEED_SILENT}, 1, 1, 36, 59,
        {&I_Fan1_25x27, &I_Fan2_25x27, &I_Fan3_25x27, &I_Fan4_25x27, &I_FanAuto_25x27, &I_FanSilent_25x27},
        {&I_Fan1_hvr_25x27, &I_Fan2_hvr_25x27, &I_Fan3_hvr_25x27, &I_Fan4_hvr_25x27, &I_FanAuto_hvr_25x27, &I_FanSilent_hvr_25x27}},
    {1, {AC_TOO_WARM}, 0, 2, 3, 106,
        {&I_TooWarm_25x13}, {&I_TooWarm_hvr_25x13}},
    {7, {AC_VANE_AUTO, AC_VANE_H1, AC_VANE_H2, AC_VANE_H3, AC_VANE_H4, AC_VANE_H5, AC_VANE_AUTO_MOVE}, 1, 2, 36, 91,
        {&I_VaneAuto_25x27, &I_VaneH1_25x27, &I_VaneH2_25x27, &I_VaneH3_25x27, &I_VaneH4_25x27, &I_VaneH5_25x27, &I_VaneAutoMove_25x27},
        {&I_VaneAuto_hvr_25x27, &I_VaneH1_hvr_25x27, &I_VaneH2_hvr_25x27, &I_VaneH3_hvr_25x27, &I_VaneH4_hvr_25x27, &I_VaneH5_hvr_25x27, &I_VaneAutoMove_hvr_25x27}}
};

bool ac_remote_load_settings(AppState *app_state)
{
    Storage *storage = furi_record_open(RECORD_STORAGE);

    storage_common_stat(storage, SAVING_DIRECTORY, NULL);
    File *file = storage_file_alloc(storage);
    uint16_t bytes_readed = 0;
    if (storage_file_open(file, SAVING_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING))
    {
        bytes_readed = storage_file_read(file, app_state, sizeof(AppState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    return bytes_readed == sizeof(AppState);
}

void ac_remote_save_settings(const AppState *app_state)
{
    Storage *storage = furi_record_open(RECORD_STORAGE);

    if (storage_common_stat(storage, SAVING_DIRECTORY, NULL) == FSE_NOT_EXIST)
    {
        if (!storage_simply_mkdir(storage, SAVING_DIRECTORY))
        {
            return;
        }
    }

    File *file = storage_file_alloc(storage);
    if (storage_file_open(file, SAVING_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS))
    {
        storage_file_write(file, app_state, sizeof(AppState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

void ac_remote_scene_start_item_callback(VariableButtonItem *item)
{
    furi_assert(item);

    AC_RemoteApp *app = ac_remote_panel_item_get_context(item);
    furi_assert(app);
    view_stack_add_view(app->view_stack, ac_remote_panel_get_view(app->ac_remote_panel));
    ACRemotePanel *ac_remote_panel = app->ac_remote_panel;

    uint16_t id = ac_remote_panel_item_get_index(item);
    uint8_t currient_value_index = ac_remote_panel_item_get_current_value_index(item);
    ac_remote_panel_item_set_icon_name(item, button_items[id].icon_name[currient_value_index], button_items[id].icon_name_selected[currient_value_index]);

    switch (id)
    {
    case ID_BUTTON_TOO_COOL:
        if (app->app_state.ac_state.ac_temp < 31)
        {
            app->app_state.ac_state.ac_temp++;
            char buffer[12] = {0};
            snprintf(buffer, sizeof(buffer), "%d", app->app_state.ac_state.ac_temp);
            ac_remote_panel_label_set_string(ac_remote_panel, ID_LABEL_TEMPERATURE, buffer);
        }
        break;
    case ID_BUTTON_TOO_WARM:
        if (app->app_state.ac_state.ac_temp > 16)
        {
            app->app_state.ac_state.ac_temp--;
            char buffer[12] = {0};
            snprintf(buffer, sizeof(buffer), "%d", app->app_state.ac_state.ac_temp);
            ac_remote_panel_label_set_string(ac_remote_panel, ID_LABEL_TEMPERATURE, buffer);
        }
        break;
    case ID_BUTTON_POWER:
        app->app_state.ac_state.ac_power = button_items[id].commands[currient_value_index];
        break;
    case ID_BUTTON_MODE:
        app->app_state.ac_state.ac_mode = button_items[id].commands[currient_value_index];
        break;
    case ID_BUTTON_FAN:
        app->app_state.ac_state.ac_fanmode = button_items[id].commands[currient_value_index];
        break;
    case ID_BUTTON_VANE:
        app->app_state.ac_state.ac_vanemode = button_items[id].commands[currient_value_index];
        break;
    default:
        break;
    }
    app->app_state.currient_value_index[id] = currient_value_index;

    view_dispatcher_send_custom_event(app->view_dispatcher, AC_RemoteEventTypeSendCommand);
}

void ac_remote_scene_start_on_enter(void *context)
{
    AC_RemoteApp *app = context;

    ac_remote_load_settings(&app->app_state);
    if (app->app_state.magic != MAGIC)
    {
        app->app_state.magic = MAGIC;
        for (int i = 0; i < NUM_BUTTON_ITEMS; i++)
        {
            app->app_state.currient_value_index[i] = 0;
        }
        app->app_state.ac_state.ac_power = button_items[ID_BUTTON_POWER].commands[0];
        app->app_state.ac_state.ac_mode = button_items[ID_BUTTON_MODE].commands[0];
        app->app_state.ac_state.ac_fanmode = button_items[ID_BUTTON_FAN].commands[0];
        app->app_state.ac_state.ac_vanemode = button_items[ID_BUTTON_VANE].commands[0];
        app->app_state.ac_state.ac_temp = AC_MIN_TEMPERATURE;
    }

    view_stack_add_view(app->view_stack, ac_remote_panel_get_view(app->ac_remote_panel));

    ACRemotePanel *ac_remote_panel = app->ac_remote_panel;

    ac_remote_panel_reserve(ac_remote_panel, 2, 3);

    for (int i = 0; i < NUM_BUTTON_ITEMS; ++i)
    {
        int current_value_index = app->app_state.currient_value_index[i];
        ac_remote_panel_add_item(
            ac_remote_panel,
            i,
            current_value_index,
            button_items[i].values_count,
            button_items[i].matrix_place_x,
            button_items[i].matrix_place_y,
            button_items[i].x,
            button_items[i].y,
            button_items[i].icon_name[current_value_index],
            button_items[i].icon_name_selected[current_value_index],
            ac_remote_scene_start_item_callback,
            context);
    }

    ac_remote_panel_add_label(ac_remote_panel, 0, 6, 10, FontPrimary, "AC remote");

    char buffer[12] = {0};
    snprintf(buffer, sizeof(buffer), "%d", app->app_state.ac_state.ac_temp);
    ac_remote_panel_add_label(ac_remote_panel, ID_LABEL_TEMPERATURE, 5, 96, FontBigNumbers, buffer);

    view_set_orientation(view_stack_get_view(app->view_stack), ViewOrientationVertical);
    view_dispatcher_switch_to_view(app->view_dispatcher, AC_RemoteAppViewStack);
}

bool ac_remote_scene_start_on_event(void *context, SceneManagerEvent event)
{
    UNUSED(context);
    UNUSED(event);
    AC_RemoteApp* app = context;
    bool consumed = false;
    if (event.type == SceneManagerEventTypeCustom)
    {
        if (event.event == AC_RemoteEventTypeSendCommand)
        {
            NotificationApp *notifications = furi_record_open(RECORD_NOTIFICATION);
            notification_message(notifications, &sequence_blink_white_100);

            send_ac_mitsubishi(&app->app_state.ac_state);

            notification_message(notifications, &sequence_blink_stop);
        }
        consumed = true;
    }
    return consumed;
}

void ac_remote_scene_start_on_exit(void *context)
{
    AC_RemoteApp *app = context;

    ac_remote_save_settings(&app->app_state);

    ACRemotePanel *ac_remote_panel = app->ac_remote_panel;
    view_stack_remove_view(app->view_stack, ac_remote_panel_get_view(ac_remote_panel));
    ac_remote_panel_reset(ac_remote_panel);
}
