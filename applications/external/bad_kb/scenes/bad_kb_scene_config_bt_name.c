#include "../bad_kb_app_i.h"

static void bad_kb_scene_config_bt_name_text_input_callback(void* context) {
    BadKbApp* bad_kb = context;

    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, BadKbAppCustomEventTextInputDone);
}

void bad_kb_scene_config_bt_name_on_enter(void* context) {
    BadKbApp* bad_kb = context;
    TextInput* text_input = bad_kb->text_input;

    strlcpy(
        bad_kb->bt_name_buf,
        bad_kb->set_bt_id ? bad_kb->id_config.ble.name : bad_kb->config.ble.name,
        sizeof(bad_kb->bt_name_buf));
    text_input_set_header_text(text_input, "Set BT device name");

    text_input_set_result_callback(
        text_input,
        bad_kb_scene_config_bt_name_text_input_callback,
        bad_kb,
        bad_kb->bt_name_buf,
        sizeof(bad_kb->bt_name_buf),
        true);

    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewTextInput);
}

bool bad_kb_scene_config_bt_name_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadKbAppCustomEventTextInputDone) {
            // Set user config and remember
            strlcpy(bad_kb->config.ble.name, bad_kb->bt_name_buf, sizeof(bad_kb->config.ble.name));
            // Apply to ID config so its temporarily overridden
            if(bad_kb->set_bt_id) {
                strlcpy(
                    bad_kb->id_config.ble.name,
                    bad_kb->bt_name_buf,
                    sizeof(bad_kb->id_config.ble.name));
            }
            bad_kb_config_refresh(bad_kb);
        }
        scene_manager_previous_scene(bad_kb->scene_manager);
    }
    return consumed;
}

void bad_kb_scene_config_bt_name_on_exit(void* context) {
    BadKbApp* bad_kb = context;
    TextInput* text_input = bad_kb->text_input;

    text_input_reset(text_input);
}
