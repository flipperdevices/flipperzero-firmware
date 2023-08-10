// TODO: rename this scene to _save_name

#include "../ublox_i.h"

#define TAG "ublox_scene_enter_file_name"

void ublox_text_input_callback(void* context) {
    Ublox* ublox = context;

    view_dispatcher_send_custom_event(ublox->view_dispatcher, UbloxCustomEventTextInputDone);
}

FuriString* ublox_scene_enter_file_name_get_timename() {
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    FuriString* s = furi_string_alloc();

    // YMD sorts better
    furi_string_printf(
        s,
        "gps-%.4d%.2d%.2d-%.2d%.2d%.2d.kml",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);
    return s;
}

void ublox_scene_enter_file_name_on_enter(void* context) {
    Ublox* ublox = context;
    TextInput* text_input = ublox->text_input;

    text_input_set_header_text(text_input, "Enter KML log file name");
    text_input_set_result_callback(
        text_input, ublox_text_input_callback, context, ublox->text_store, 100, false);

    FuriString* fname = ublox_scene_enter_file_name_get_timename();
    strcpy(ublox->text_store, furi_string_get_cstr(fname));

    //FuriString* full_fname = furi_string_alloc_set(folder_path);

    ValidatorIsFile* validator_is_file =
        // app path folder, app extension, current file name
        validator_is_file_alloc_init(
            furi_string_get_cstr(ublox->logfile_folder), UBLOX_KML_EXTENSION, "");

    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    furi_string_free(fname);
    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewTextInput);
}

bool ublox_scene_enter_file_name_on_event(void* context, SceneManagerEvent event) {
    Ublox* ublox = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UbloxCustomEventTextInputDone) {
            //FuriString* fullname;
            FURI_LOG_I(TAG, "text: %s", ublox->text_store);
            ublox->log_state = UbloxLogStateStartLogging;
            //scene_manager_next_scene(ublox->scene_manager, UbloxSceneDataDisplay);
            // don't add data_display as the next scene, instead go back to the last scene
            scene_manager_previous_scene(ublox->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void ublox_scene_enter_file_name_on_exit(void* context) {
    UNUSED(context);
}
