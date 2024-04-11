#include "../ac_remote_app_i.h"

typedef enum {
    button_power,
    button_mode,
    button_temp_up,
    button_fan,
    button_temp_down,
    button_vane,
    label_temperature,
} button_id;

const Icon* mode[4][2] = {
    [HvacMitsubishiModeHeat] = {&I_heat_19x20, &I_heat_hover_19x20},
    [HvacMitsubishiModeCold] = {&I_cold_19x20, &I_cold_hover_19x20},
    [HvacMitsubishiModeDry] = {&I_dry_19x20, &I_dry_hover_19x20},
    [HvacMitsubishiModeAuto] = {&I_auto_19x20, &I_auto_hover_19x20}};
const Icon* fan[7][2] = {
    [HvacMitsubishiFanSpeedAuto] = {&I_auto_19x20, &I_auto_hover_19x20},
    [HvacMitsubishiFanSpeed1] = {&I_fan_speed_1_19x20, &I_fan_speed_1_hover_19x20},
    [HvacMitsubishiFanSpeed2] = {&I_fan_speed_2_19x20, &I_fan_speed_2_hover_19x20},
    [HvacMitsubishiFanSpeed3] = {&I_fan_speed_3_19x20, &I_fan_speed_3_hover_19x20},
    [HvacMitsubishiFanSpeed4] = {&I_fan_speed_4_19x20, &I_fan_speed_4_hover_19x20},
    [HvacMitsubishiFanSpeedSilent] = {&I_fan_silent_19x20, &I_fan_silent_hover_19x20}};
const Icon* vane[7][2] = {
    [HvacMitsubishiVaneAuto] = {&I_auto_19x20, &I_auto_hover_19x20},
    [HvacMitsubishiVaneH1] = {&I_vane_h1_19x20, &I_vane_h1_hover_19x20},
    [HvacMitsubishiVaneH2] = {&I_vane_h2_19x20, &I_vane_h2_hover_19x20},
    [HvacMitsubishiVaneH3] = {&I_vane_h3_19x20, &I_vane_h3_hover_19x20},
    [HvacMitsubishiVaneH4] = {&I_vane_h4_19x20, &I_vane_h4_hover_19x20},
    [HvacMitsubishiVaneH5] = {&I_vane_h5_19x20, &I_vane_h5_hover_19x20},
    [HvacMitsubishiVaneAutoMove] = {&I_vane_auto_move_19x20, &I_vane_auto_move_hover_19x20}};

char buffer[4] = {0};

bool ac_remote_load_settings(ACRemoteAppSettings* app_state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);
    FuriString* header = furi_string_alloc();

    uint32_t version = 0;
    bool success = false;
    do {
        if(!flipper_format_buffered_file_open_existing(ff, AC_REMOTE_APP_SETTINGS)) break;
        if(!flipper_format_read_header(ff, header, &version)) break;
        if(!furi_string_equal(header, "AC Remote") || (version != 1)) break;
        if(!flipper_format_read_uint32(ff, "Mode", &app_state->mode, 1)) break;
        if(app_state->mode > HvacMitsubishiModeAuto) break;
        if(!flipper_format_read_uint32(ff, "Temperature", &app_state->temperature, 1)) break;
        if(app_state->temperature > HVAC_MITSUBISHI_TEMPERATURE_MAX) break;
        if(!flipper_format_read_uint32(ff, "Fan", &app_state->fan, 1)) break;
        if(app_state->fan > HvacMitsubishiFanSpeedSilent) break;
        if(!flipper_format_read_uint32(ff, "Vane", &app_state->vane, 1)) break;
        if(app_state->vane > HvacMitsubishiVaneAutoMove) break;
        success = true;
    } while(false);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(header);
    flipper_format_free(ff);
    return success;
}

bool ac_remote_store_settings(ACRemoteAppSettings* app_state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);

    bool success = false;
    do {
        if(!flipper_format_file_open_always(ff, AC_REMOTE_APP_SETTINGS)) break;
        if(!flipper_format_write_header_cstr(ff, "AC Remote", 1)) break;
        if(!flipper_format_write_comment_cstr(ff, "")) break;
        if(!flipper_format_write_uint32(ff, "Mode", &app_state->mode, 1)) break;
        if(!flipper_format_write_uint32(ff, "Temperature", &app_state->temperature, 1)) break;
        if(!flipper_format_write_uint32(ff, "Fan", &app_state->fan, 1)) break;
        if(!flipper_format_write_uint32(ff, "Vane", &app_state->vane, 1)) break;
        success = true;
    } while(false);
    furi_record_close(RECORD_STORAGE);
    flipper_format_free(ff);
    return success;
}

void ac_remote_scene_universal_common_item_callback(void* context, uint32_t index) {
    AC_RemoteApp* ac_remote = context;
    uint32_t event = ac_remote_custom_event_pack(AC_RemoteCustomEventTypeButtonSelected, index);
    view_dispatcher_send_custom_event(ac_remote->view_dispatcher, event);
}

void ac_remote_scene_mitsubishi_on_enter(void* context) {
    AC_RemoteApp* ac_remote = context;
    ACRemotePanel* ac_remote_panel = ac_remote->ac_remote_panel;
    ac_remote->hvac_mitsubishi_data = hvac_mitsubishi_init();

    if(!ac_remote_load_settings(&ac_remote->app_state)) {
        ac_remote->app_state.mode = HvacMitsubishiModeAuto;
        ac_remote->app_state.fan = HvacMitsubishiFanSpeedAuto;
        ac_remote->app_state.vane = HvacMitsubishiVaneAuto;
        ac_remote->app_state.temperature = 23;
    }

    hvac_mitsubishi_set_mode(ac_remote->hvac_mitsubishi_data, ac_remote->app_state.mode);
    hvac_mitsubishi_set_fan_speed(ac_remote->hvac_mitsubishi_data, ac_remote->app_state.fan);
    hvac_mitsubishi_set_vane(ac_remote->hvac_mitsubishi_data, ac_remote->app_state.vane);
    hvac_mitsubishi_set_temperature(
        ac_remote->hvac_mitsubishi_data, ac_remote->app_state.temperature);

    view_stack_add_view(ac_remote->view_stack, ac_remote_panel_get_view(ac_remote_panel));
    ac_remote_panel_reserve(ac_remote_panel, 2, 3);

    ac_remote_panel_add_item(
        ac_remote_panel,
        button_power,
        0,
        0,
        6,
        17,
        &I_off_19x20,
        &I_off_hover_19x20,
        ac_remote_scene_universal_common_item_callback,
        context);
    ac_remote_panel_add_icon(ac_remote_panel, 9, 39, &I_off_text_14x5);
    ac_remote_panel_add_item(
        ac_remote_panel,
        button_mode,
        1,
        0,
        39,
        17,
        mode[ac_remote->app_state.mode][0],
        mode[ac_remote->app_state.mode][1],
        ac_remote_scene_universal_common_item_callback,
        context);
    ac_remote_panel_add_icon(ac_remote_panel, 39, 39, &I_mode_text_20x5);
    ac_remote_panel_add_icon(ac_remote_panel, 0, 63, &I_frame_30x39);
    ac_remote_panel_add_item(
        ac_remote_panel,
        button_temp_up,
        0,
        1,
        3,
        51,
        &I_tempup_24x21,
        &I_tempup_hover_24x21,
        ac_remote_scene_universal_common_item_callback,
        context);
    ac_remote_panel_add_item(
        ac_remote_panel,
        button_temp_down,
        0,
        2,
        3,
        93,
        &I_tempdown_24x21,
        &I_tempdown_hover_24x21,
        ac_remote_scene_universal_common_item_callback,
        context);
    ac_remote_panel_add_item(
        ac_remote_panel,
        button_fan,
        1,
        1,
        39,
        54,
        fan[ac_remote->app_state.fan][0],
        fan[ac_remote->app_state.fan][1],
        ac_remote_scene_universal_common_item_callback,
        context);
    ac_remote_panel_add_icon(ac_remote_panel, 41, 76, &I_fan_text_14x5);
    ac_remote_panel_add_item(
        ac_remote_panel,
        button_vane,
        1,
        2,
        39,
        91,
        vane[ac_remote->app_state.vane][0],
        vane[ac_remote->app_state.vane][1],
        ac_remote_scene_universal_common_item_callback,
        context);
    ac_remote_panel_add_icon(ac_remote_panel, 38, 113, &I_vane_text_20x5);

    ac_remote_panel_add_label(ac_remote_panel, 0, 6, 11, FontPrimary, "AC remote");

    snprintf(buffer, sizeof(buffer), "%ld", ac_remote->app_state.temperature);
    ac_remote_panel_add_label(ac_remote_panel, label_temperature, 4, 86, FontKeyboard, buffer);

    view_set_orientation(view_stack_get_view(ac_remote->view_stack), ViewOrientationVertical);
    view_dispatcher_switch_to_view(ac_remote->view_dispatcher, AC_RemoteAppViewStack);
}

bool ac_remote_scene_mitsubishi_on_event(void* context, SceneManagerEvent event) {
    AC_RemoteApp* ac_remote = context;
    SceneManager* scene_manager = ac_remote->scene_manager;
    ACRemotePanel* ac_remote_panel = ac_remote->ac_remote_panel;
    UNUSED(scene_manager);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        uint16_t event_type;
        int16_t event_value;
        ac_remote_custom_event_unpack(event.event, &event_type, &event_value);
        if(event_type == AC_RemoteCustomEventTypeSendCommand) {
            NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);
            notification_message(notifications, &sequence_blink_white_100);
            hvac_mitsubishi_send(ac_remote->hvac_mitsubishi_data);
            notification_message(notifications, &sequence_blink_stop);
        } else if(event_type == AC_RemoteCustomEventTypeButtonSelected) {
            ac_remote->app_state.power = HvacMitsubishiPowerOn;
            switch(event_value) {
            case button_power:
                ac_remote->app_state.power = HvacMitsubishiPowerOff;
                break;
            case button_mode:
                ac_remote->app_state.mode++;
                if(ac_remote->app_state.mode > HvacMitsubishiModeAuto) {
                    ac_remote->app_state.mode = HvacMitsubishiModeHeat;
                }
                hvac_mitsubishi_set_mode(
                    ac_remote->hvac_mitsubishi_data, ac_remote->app_state.mode);
                ac_remote_panel_item_set_icons(
                    ac_remote_panel,
                    button_mode,
                    mode[ac_remote->app_state.mode][0],
                    mode[ac_remote->app_state.mode][1]);
                break;
            case button_fan:
                ac_remote->app_state.fan++;
                if(ac_remote->app_state.fan > HvacMitsubishiFanSpeedSilent) {
                    ac_remote->app_state.fan = HvacMitsubishiFanSpeedAuto;
                }
                hvac_mitsubishi_set_fan_speed(
                    ac_remote->hvac_mitsubishi_data, ac_remote->app_state.fan);
                ac_remote_panel_item_set_icons(
                    ac_remote_panel,
                    button_fan,
                    fan[ac_remote->app_state.fan][0],
                    fan[ac_remote->app_state.fan][1]);
                break;
            case button_vane:
                ac_remote->app_state.vane++;
                if(ac_remote->app_state.vane > HvacMitsubishiVaneAutoMove) {
                    ac_remote->app_state.vane = HvacMitsubishiVaneAuto;
                }
                hvac_mitsubishi_set_vane(
                    ac_remote->hvac_mitsubishi_data, ac_remote->app_state.vane);
                ac_remote_panel_item_set_icons(
                    ac_remote_panel,
                    button_vane,
                    vane[ac_remote->app_state.vane][0],
                    vane[ac_remote->app_state.vane][1]);
                break;
            case button_temp_up:
                if(ac_remote->app_state.temperature < 31) {
                    ac_remote->app_state.temperature++;
                    hvac_mitsubishi_set_temperature(
                        ac_remote->hvac_mitsubishi_data, ac_remote->app_state.temperature);
                    snprintf(buffer, sizeof(buffer), "%ld", ac_remote->app_state.temperature);
                    ac_remote_panel_label_set_string(ac_remote_panel, label_temperature, buffer);
                }
                break;
            case button_temp_down:
                if(ac_remote->app_state.temperature > 16) {
                    ac_remote->app_state.temperature--;
                    hvac_mitsubishi_set_temperature(
                        ac_remote->hvac_mitsubishi_data, ac_remote->app_state.temperature);
                    snprintf(buffer, sizeof(buffer), "%ld", ac_remote->app_state.temperature);
                    ac_remote_panel_label_set_string(ac_remote_panel, label_temperature, buffer);
                }
                break;
            default:
                break;
            }
            hvac_mitsubishi_power(ac_remote->hvac_mitsubishi_data, ac_remote->app_state.power);
            uint32_t event = ac_remote_custom_event_pack(AC_RemoteCustomEventTypeSendCommand, 0);
            view_dispatcher_send_custom_event(ac_remote->view_dispatcher, event);
        }
        consumed = true;
    }
    return consumed;
}

void ac_remote_scene_mitsubishi_on_exit(void* context) {
    AC_RemoteApp* ac_remote = context;
    ACRemotePanel* ac_remote_panel = ac_remote->ac_remote_panel;
    ac_remote_store_settings(&ac_remote->app_state);
    hvac_mitsubishi_deinit(ac_remote->hvac_mitsubishi_data);
    view_stack_remove_view(ac_remote->view_stack, ac_remote_panel_get_view(ac_remote_panel));
    ac_remote_panel_reset(ac_remote_panel);
}
